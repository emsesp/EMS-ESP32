#include "SecuritySettingsService.h"

#include "../core/psram_async_json_response.h"

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

    if (_state.hashed_on_load) {
        _state.hashed_on_load = false;
        _fsPersistence.writeToFS();
        if (had_file) {
            emsesp::EMSESP::logger().notice("Stored passwords are now hashed");
        }
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
    String credentials = adminCredentials();
    if (credentials != _adminCredentials) {
        _adminCredentials = credentials;
        rotateSigningKey();
    }
}

String SecuritySettingsService::adminCredentials() {
    String credentials;
    for (const User & user : _state.users) {
        if (user.admin) {
            credentials += user.username + '\n' + user.password + '\n';
        }
    }
    return credentials;
}

Authentication SecuritySettingsService::authenticateRequest(AsyncWebServerRequest * request) {
    auto authorizationHeader = request->getHeader(AUTHORIZATION_HEADER);
    if (authorizationHeader) {
        String value = authorizationHeader->value();
        if (value.startsWith(AUTHORIZATION_HEADER_PREFIX)) {
            value = value.substring(AUTHORIZATION_HEADER_PREFIX_LEN);
            return authenticateJWT(value);
        }
    }

    return {};
}

// API keys are only accepted here, so they can't be used for the Web UI
Authentication SecuritySettingsService::authenticateApiRequest(AsyncWebServerRequest * request) {
    auto authorizationHeader = request->getHeader(AUTHORIZATION_HEADER);
    if (authorizationHeader) {
        String value = authorizationHeader->value();
        if (value.startsWith(AUTHORIZATION_HEADER_PREFIX)) {
            value = value.substring(AUTHORIZATION_HEADER_PREFIX_LEN);
            if (value.startsWith(ApiKey::PREFIX)) {
                const String id = ApiKey::id(value.c_str());
                for (const ApiKeyEntry & key : _state.api_keys) {
                    if (!id.isEmpty() && key.id == id && ApiKey::matches(value.c_str(), key.hash)) {
                        return Authentication(User(key.label, "", key.write));
                    }
                }
                return {};
            }
        }
    }

    return authenticateRequest(request);
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
                settings.api_keys.push_back({ApiKey::id(key.c_str()), label, write, created, ApiKey::hash(key.c_str())});
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
        if (_user.username == username && PasswordHash::verify(password, _user.password)) {
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
