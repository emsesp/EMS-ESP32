#include "SecuritySettingsService.h"

#include "../core/psram_async_json_response.h"
#include "../core/login_limiter.h"

#include <emsesp.h>

#include <array>

#include <bootloader_random.h>
#include <esp_random.h>
#include <mbedtls/md.h>

static constexpr size_t JWT_KEY_BYTES = 32;

// before NTP has set the clock, tokens get no expiry and only their version is checked
static bool clock_set(time_t now) {
    return now > 1700000000; // November 2023
}

static String to_hex(const uint8_t * data, size_t len) {
    String hex;
    hex.reserve(len * 2);
    for (size_t i = 0; i < len; i++) {
        char byte[3];
        snprintf(byte, sizeof(byte), "%02x", data[i]);
        hex += byte;
    }
    return hex;
}

SecuritySettingsService::SecuritySettingsService(AsyncWebServer * server, FS * fs)
    : _httpEndpoint(SecuritySettings::readForWeb, SecuritySettings::updateFromWeb, this, server, SECURITY_SETTINGS_PATH, this)
    , _fsPersistence(SecuritySettings::read, SecuritySettings::update, this, fs, SECURITY_SETTINGS_FILE)
    , _jwtHandler("")
    , _fs(fs) {
    addUpdateHandler([this] { onSettingsUpdated(); }, false);

    // adding, changing or removing users and passwords
    _httpEndpoint.requirePassword([](JsonObject, SecuritySettings &) { return true; });

    addEndpoint(server, API_KEYS_PATH, AuthenticationPredicates::IS_ADMIN, [this](AsyncWebServerRequest * request) { listApiKeys(request); });
    addEndpoint(server, API_KEYS_PATH, AuthenticationPredicates::IS_ADMIN, [this](AsyncWebServerRequest * request, JsonVariant json) {
        apiKeyAction(request, json);
    });
}

void SecuritySettingsService::begin() {
    auto &     nvs      = emsesp::EMSESP::nvs_;
    const bool had_file = _fs->exists(SECURITY_SETTINGS_FILE);
    if (!had_file) {
        // fresh install or factory reset, tokens from before must not survive it
        nvs.remove(emsesp::EMSESP_NVS_JWT_KEY);
        nvs.remove(emsesp::EMSESP_NVS_TOKENS_RESET);
    } else if (!nvs.isKey(emsesp::EMSESP_NVS_JWT_KEY)) {
        // upgrade from a firmware that signed tokens with the Super User password
        nvs.putBool(emsesp::EMSESP_NVS_TOKENS_RESET, true);
    }

    _fsPersistence.readFromFS();
    _adminCredentials = adminCredentials(); // after hashing, so hashing alone doesn't invalidate the tokens
    loadSigningKey();

    if (_state.hashed_on_load || _state.dropped_on_load) {
        if (had_file && _state.hashed_on_load) {
            emsesp::EMSESP::logger().notice("Stored passwords are now hashed");
        }
        _state.hashed_on_load  = false;
        _state.dropped_on_load = false;
        _fsPersistence.writeToFS();
    }
}

// the signing key is random per device, so tokens can't be forged from the public factory settings
// runs before WiFi starts, which bootloader_random needs to produce true random numbers
void SecuritySettingsService::loadSigningKey() {
    auto & nvs = emsesp::EMSESP::nvs_;
    String key = nvs.isKey(emsesp::EMSESP_NVS_JWT_KEY) ? nvs.getString(emsesp::EMSESP_NVS_JWT_KEY) : "";
    if (key.length() == JWT_KEY_BYTES * 2) {
        _jwtHandler.setSecret(key);
        return;
    }

    std::array<uint8_t, JWT_KEY_BYTES> random{};
    bootloader_random_enable();
    esp_fill_random(random.data(), random.size());
    bootloader_random_disable();

    saveSigningKey(to_hex(random.data(), random.size()));
}

// invalidates all tokens. The new key is derived from the old one, so it keeps the entropy collected at boot
void SecuritySettingsService::rotateSigningKey() {
    String old_key = _jwtHandler.getSecret();

    std::array<uint8_t, JWT_KEY_BYTES> random{};
    esp_fill_random(random.data(), random.size());

    std::array<uint8_t, 32> digest{};
    mbedtls_md_hmac(mbedtls_md_info_from_type(MBEDTLS_MD_SHA256),
                    reinterpret_cast<const unsigned char *>(old_key.c_str()),
                    old_key.length(),
                    random.data(),
                    random.size(),
                    digest.data());

    saveSigningKey(to_hex(digest.data(), digest.size()));
    emsesp::EMSESP::logger().notice("Admin password changed, all access tokens are invalid");
}

void SecuritySettingsService::saveSigningKey(const String & key) {
    if (!emsesp::EMSESP::nvs_.putString(emsesp::EMSESP_NVS_JWT_KEY, key)) {
        emsesp::EMSESP::logger().err("Failed to store the access token key");
    }
    _jwtHandler.setSecret(key);
}

void SecuritySettingsService::onSettingsUpdated() {
    const PasswordHash::Digest credentials = adminCredentials();
    if (credentials != _adminCredentials) {
        _adminCredentials = credentials;
        rotateSigningKey();
    }
}

PasswordHash::Digest SecuritySettingsService::adminCredentials() {
    stringPSRAM credentials;
    for (const User & user : _state.users) {
        if (user.admin) {
            credentials += user.username.c_str();
            credentials += '\n';
            credentials += user.password;
            credentials += '\n';
        }
    }
    return PasswordHash::digest(credentials.c_str());
}

String SecuritySettingsService::bearerToken(AsyncWebServerRequest * request) {
    auto authorizationHeader = request->getHeader(AUTHORIZATION_HEADER);
    if (authorizationHeader) {
        const String & value = authorizationHeader->value();
        if (value.startsWith(AUTHORIZATION_HEADER_PREFIX)) {
            return value.substring(AUTHORIZATION_HEADER_PREFIX_LEN);
        }
    }
    return "";
}

Authentication SecuritySettingsService::authenticateRequest(AsyncWebServerRequest * request) {
    String token = bearerToken(request);
    if (token.isEmpty()) {
        return {};
    }

    Authentication authentication = authenticateJWT(token);
    if (authentication.authenticated) {
        authentication.session            = true;
        authentication.password_confirmed = _passwordConfirm.confirmed(token.c_str(), millis());
    }
    return authentication;
}

bool SecuritySettingsService::confirmPassword(AsyncWebServerRequest * request, const String & password) {
    String         token          = bearerToken(request);
    Authentication authentication = authenticateJWT(token);
    if (!authentication.authenticated || !PasswordHash::verify(password, authentication.user->password.c_str())) {
        return false;
    }

    _passwordConfirm.confirm(token.c_str(), millis());
    return true;
}

// API keys and Basic logins are only accepted here, so they can't be used for the Web UI
Authentication SecuritySettingsService::authenticateApiRequest(AsyncWebServerRequest * request) {
    auto authorizationHeader = request->getHeader(AUTHORIZATION_HEADER);
    if (authorizationHeader) {
        String value = authorizationHeader->value();
        if (value.startsWith("Basic ")) {
            return authenticateBasic(request, value);
        }
        if (value.startsWith(AUTHORIZATION_HEADER_PREFIX)) {
            value = value.substring(AUTHORIZATION_HEADER_PREFIX_LEN);
            if (value.startsWith(ApiKey::PREFIX)) {
                return authenticateApiKey(value.c_str());
            }
        }
    }

    return authenticateRequest(request);
}

Authentication SecuritySettingsService::authenticateApiKey(const char * key) {
    const String id = ApiKey::id(key);
    for (const ApiKeyEntry & entry : _state.api_keys) {
        if (!id.isEmpty() && entry.id == id && ApiKey::matches(key, entry.hash.c_str())) {
            return Authentication(User(entry.label, "", entry.write));
        }
    }
    return {};
}

// a browser's login prompt: a user's password, or an API key as the password with any username.
// Wrong passwords count towards the same sign-in limits as the Web UI
Authentication SecuritySettingsService::authenticateBasic(AsyncWebServerRequest * request, const String & header) {
    std::string username;
    std::string password;
    if (!BasicAuth::parse(header.c_str(), username, password)) {
        return {};
    }

    Authentication authentication;
    if (ApiKey::is_key(password.c_str())) {
        // tools that only support Basic get the key's rights. A browser keeps the login and sends it with
        // requests that other sites trigger, so there it only reads. Browsers send Sec-Fetch-Site on every request
        authentication       = authenticateApiKey(password.c_str());
        authentication.basic = authentication.authenticated && request->hasHeader("Sec-Fetch-Site");
        return authentication;
    } else {
        const User * user = nullptr;
        for (const User & u : _state.users) {
            if (u.username == username.c_str()) {
                user = &u;
                break;
            }
        }

        const uint32_t now = millis();
        if (user && _basicAuthCache.find(username, password, user->password.c_str(), now)) {
            authentication = Authentication(*user);
        } else {
            auto &       limiter = LoginLimiter::instance();
            const String source  = request->client()->remoteIP().toString();
            if (limiter.retry_after(source.c_str(), now)) {
                return {};
            }
            if (user && PasswordHash::verify(String(password.c_str()), user->password.c_str())) {
                limiter.succeeded(source.c_str());
                _basicAuthCache.add(username, password, user->password.c_str(), now);
                authentication = Authentication(*user);
            } else {
                limiter.failed(source.c_str(), now);
                emsesp::EMSESP::logger().notice("Failed API sign-in for user '%s' from %s", username.c_str(), source.c_str());
                return {};
            }
        }
    }

    authentication.basic = authentication.authenticated;
    return authentication;
}

void SecuritySettingsService::listApiKeys(AsyncWebServerRequest * request) {
    auto *     response = new emsesp::PsramAsyncJsonResponse(false);
    JsonObject root     = response->getRoot();
    JsonArray  keys     = root["keys"].to<JsonArray>();
    for (const ApiKeyEntry & key : _state.api_keys) {
        JsonObject k = keys.add<JsonObject>();
        k["id"]      = key.id;
        k["label"]   = key.label;
        k["write"]   = key.write;
        k["created"] = key.created;
    }
    response->setLength();
    request->send(response);
}

// create returns the key, the only time it's shown
void SecuritySettingsService::apiKeyAction(AsyncWebServerRequest * request, JsonVariant json) {
    if (!authenticateRequest(request).password_confirmed) {
        request->send(PASSWORD_REQUIRED);
        return;
    }

    const String action = json["action"] | "";

    if (action == "create") {
        const String label = json["label"] | "";
        if (label.isEmpty() || label.length() > ApiKey::MAX_LABEL || _state.api_keys.size() >= ApiKey::MAX_KEYS) {
            request->send(400);
            return;
        }

        const String   key     = ApiKey::generate();
        const time_t   now     = time(nullptr);
        const uint32_t created = clock_set(now) ? static_cast<uint32_t>(now) : 0;
        const bool     write   = json["write"] | false;
        update(
            [&](SecuritySettings & settings) {
                settings.api_keys.push_back({ApiKey::id(key.c_str()), label, write, created, ApiKey::hash(key.c_str()).c_str()});
                return StateUpdateResult::CHANGED;
            });
        emsesp::EMSESP::logger().notice("API key '%s' created", label.c_str());

        auto *     response = new emsesp::PsramAsyncJsonResponse(false);
        JsonObject root     = response->getRoot();
        root["key"]         = key;
        response->setLength();
        request->send(response);
        return;
    }

    if (action == "revoke") {
        const String id      = json["id"] | "";
        bool         removed = false;
        update(
            [&](SecuritySettings & settings) {
                for (auto it = settings.api_keys.begin(); it != settings.api_keys.end(); ++it) {
                    if (it->id == id) {
                        emsesp::EMSESP::logger().notice("API key '%s' revoked", it->label.c_str());
                        settings.api_keys.erase(it);
                        removed = true;
                        return StateUpdateResult::CHANGED;
                    }
                }
                return StateUpdateResult::UNCHANGED;
            });
        request->send(removed ? 200 : 404);
        return;
    }

    request->send(400);
}

Authentication SecuritySettingsService::authenticateJWT(String & jwt) {
    JsonDocument payloadDocument;
    _jwtHandler.parseJWT(jwt, payloadDocument);
    if (payloadDocument.is<JsonObject>()) {
        JsonObject parsedPayload = payloadDocument.as<JsonObject>();
        String     username      = parsedPayload["username"];
        for (const User & _user : _state.users) {
            if (_user.username == username && validatePayload(parsedPayload, &_user)) {
                return Authentication(_user);
            }
        }
    }

    return {};
}

Authentication SecuritySettingsService::authenticate(const String & username, const String & password) {
    for (const User & _user : _state.users) {
        if (_user.username == username && PasswordHash::verify(password, _user.password.c_str())) {
            return Authentication(_user);
        }
    }

    return {};
}

// the user must still exist with the same rights and token version, and the token must not have expired
boolean SecuritySettingsService::validatePayload(JsonObject parsedPayload, const User * user) {
    if ((parsedPayload["admin"] | false) != user->admin || (parsedPayload["ver"] | 0u) != user->ver) {
        return false;
    }

    if (parsedPayload["exp"].is<uint32_t>()) {
        const time_t now = time(nullptr);
        if (clock_set(now) && static_cast<uint32_t>(now) >= parsedPayload["exp"].as<uint32_t>()) {
            return false;
        }
    }

    return true;
}

String SecuritySettingsService::generateJWT(const User * user) {
    JsonDocument jsonDocument;
    JsonObject   payload = jsonDocument.to<JsonObject>();
    payload["username"]  = user->username;
    payload["admin"]     = user->admin;
    payload["ver"]       = user->ver;

    const time_t now = time(nullptr);
    if (clock_set(now)) {
        payload["iat"] = static_cast<uint32_t>(now);
        payload["exp"] = static_cast<uint32_t>(now) + SESSION_TOKEN_LIFETIME;
    }
    return _jwtHandler.buildJWT(payload);
}
