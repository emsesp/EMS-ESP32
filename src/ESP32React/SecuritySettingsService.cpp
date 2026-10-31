#include "SecuritySettingsService.h"

#include "../core/psram_async_json_response.h"

#include <emsesp.h>

#include <array>

#include <bootloader_random.h>
#include <esp_random.h>
#include <mbedtls/md.h>

static constexpr size_t JWT_KEY_BYTES = 32;

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

    addEndpoint(server, GENERATE_TOKEN_PATH, AuthenticationPredicates::IS_ADMIN, [this](AsyncWebServerRequest * request) { generateToken(request); });
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
    } else if (request->hasParam(ACCESS_TOKEN_PARAMETER)) {
        auto   tokenParamater = request->getParam(ACCESS_TOKEN_PARAMETER);
        String value          = tokenParamater->value();
        return authenticateJWT(value);
    }

    return {};
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

inline void populateJWTPayload(JsonObject payload, const User * user) {
    payload["username"] = user->username;
    payload["admin"]    = user->admin;
}

boolean SecuritySettingsService::validatePayload(JsonObject parsedPayload, const User * user) {
    JsonDocument jsonDocument;
    JsonObject   payload = jsonDocument.to<JsonObject>();
    populateJWTPayload(payload, user);
    return payload == parsedPayload;
}

String SecuritySettingsService::generateJWT(const User * user) {
    JsonDocument jsonDocument;
    JsonObject   payload = jsonDocument.to<JsonObject>();
    populateJWTPayload(payload, user);
    return _jwtHandler.buildJWT(payload);
}

void SecuritySettingsService::generateToken(AsyncWebServerRequest * request) {
    auto usernameParam = request->getParam("username");
    for (const User & _user : _state.users) {
        if (_user.username == usernameParam->value()) {
            auto *     response = new emsesp::PsramAsyncJsonResponse(false);
            JsonObject root     = response->getRoot();
            root["token"]       = generateJWT(&_user);
            response->setLength();
            request->send(response);
            return;
        }
    }

    request->send(401);
}
