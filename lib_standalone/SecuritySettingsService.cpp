#ifdef EMSESP_STANDALONE

#include <SecuritySettingsService.h>

#if FT_ENABLED(FT_SECURITY)

#include "../../src/emsesp_stub.h" // proddy added

SecuritySettingsService::SecuritySettingsService(AsyncWebServer * server, FS * fs)
    : _httpEndpoint(SecuritySettings::read, SecuritySettings::update, this, server, SECURITY_SETTINGS_PATH, this)
    , _fsPersistence(SecuritySettings::read, SecuritySettings::update, this, fs, SECURITY_SETTINGS_FILE)
// , _jwtHandler(FACTORY_JWT_SECRET) {
{
    addUpdateHandler([&](const String & originId) { configureJWTHandler(); }, false);
}

void SecuritySettingsService::begin() {
    _fsPersistence.readFromFS();
    configureJWTHandler();
}

void SecuritySettingsService::registerURI() {
    _httpEndpoint.registerURI();
    _server->on(GENERATE_TOKEN_PATH, HTTP_GET, wrapRequest(std::bind(&SecuritySettingsService::generateToken, this, _1), AuthenticationPredicates::IS_ADMIN));
}

Authentication SecuritySettingsService::authenticateRequest(PsychicRequest * request) {
    if (request->hasHeader(AUTHORIZATION_HEADER)) {
        auto value = request->header(AUTHORIZATION_HEADER);
        if (value.startsWith(AUTHORIZATION_HEADER_PREFIX)) {
            value = value.substring(AUTHORIZATION_HEADER_PREFIX_LEN);
            return authenticateJWT(value);
        }
    } else if (request->hasParam(ACCESS_TOKEN_PARAMATER)) {
        String value = request->getParam(ACCESS_TOKEN_PARAMATER)->value();
        return authenticateJWT(value);
    }
    return Authentication();
}

void SecuritySettingsService::configureJWTHandler() {
    // _jwtHandler.setSecret(_state.jwtSecret);
}

Authentication SecuritySettingsService::authenticateJWT(String & jwt) {
    DynamicJsonDocument payloadDocument(MAX_JWT_SIZE);
    // _jwtHandler.parseJWT(jwt, payloadDocument);
    if (payloadDocument.is<JsonObject>()) {
        JsonObject parsedPayload = payloadDocument.as<JsonObject>();
        String     username      = parsedPayload["username"];
        for (User _user : _state.users) {
            if (_user.username == username && validatePayload(parsedPayload, &_user)) {
                return Authentication(_user);
            }
        }
    }
    return Authentication();
}

Authentication SecuritySettingsService::authenticate(const String & username, const String & password) {
    for (User _user : _state.users) {
        if (_user.username == username && _user.password == password) {
            return Authentication(_user);
        }
    }
    return Authentication();
}

inline void populateJWTPayload(JsonObject & payload, User * user) {
    payload["username"] = user->username;
    payload["admin"]    = user->admin;
}

boolean SecuritySettingsService::validatePayload(JsonObject & parsedPayload, User * user) {
    DynamicJsonDocument jsonDocument(MAX_JWT_SIZE);
    JsonObject          payload = jsonDocument.to<JsonObject>();
    populateJWTPayload(payload, user);
    return payload == parsedPayload;
}

String SecuritySettingsService::generateJWT(User * user) {
    DynamicJsonDocument jsonDocument(MAX_JWT_SIZE);
    JsonObject          payload = jsonDocument.to<JsonObject>();
    populateJWTPayload(payload, user);
    return "";
}

ArRequestFilterFunction SecuritySettingsService::filterRequest(AuthenticationPredicate predicate) {
    return [this, predicate](AsyncWebServerRequest * request) {
        Authentication authentication = authenticateRequest(request);
        return predicate(authentication);
    };
}

PsychicHttpRequestCallback SecuritySettingsService::wrapRequest(PsychicHttpRequestCallback onRequest, AuthenticationPredicate predicate) {
    return [this, onRequest, predicate](PsychicRequest * request) {
        Authentication authentication = authenticateRequest(request);
        if (!predicate(authentication)) {
            return request->reply(401);
        }
        return onRequest(request);
    };
}

PsychicJsonRequestCallback SecuritySettingsService::wrapCallback(PsychicJsonRequestCallback onRequest, AuthenticationPredicate predicate) {
    return [this, onRequest, predicate](PsychicRequest * request, JsonVariant & json) {
        Authentication authentication = authenticateRequest(request);
        if (!predicate(authentication)) {
            return request->reply(401);
        }
        return onRequest(request, json);
    };
}

#endif

#endif