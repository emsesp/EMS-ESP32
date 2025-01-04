#include "SecuritySettingsService.h"

SecuritySettingsService::SecuritySettingsService(AsyncWebServer * server, FS * fs)
    : _httpEndpoint(SecuritySettings::read, SecuritySettings::update, this, server, SECURITY_SETTINGS_PATH, this)
    , _fsPersistence(SecuritySettings::read, SecuritySettings::update, this, fs, SECURITY_SETTINGS_FILE)
    , _jwtHandler(FACTORY_JWT_SECRET) {
    addUpdateHandler([this] { configureJWTHandler(); }, false);

    server->on(GENERATE_TOKEN_PATH,
               HTTP_GET,
               SecuritySettingsService::wrapRequest([this](AsyncWebServerRequest * request) { generateToken(request); }, AuthenticationPredicates::IS_ADMIN));
}

void SecuritySettingsService::begin() {
    _fsPersistence.readFromFS();
    configureJWTHandler();
}

Authentication SecuritySettingsService::authenticateRequest(AsyncWebServerRequest * request) {
    auto authorizationHeader = request->getHeader(AUTHORIZATION_HEADER);
    if (authorizationHeader) {
        String value = authorizationHeader->value();
        if (value.startsWith(AUTHORIZATION_HEADER_PREFIX)) {
            value = value.substring(AUTHORIZATION_HEADER_PREFIX_LEN);
            return authenticateJWT(value);
        }
    } else if (request->hasParam(ACCESS_TOKEN_PARAMATER)) {
        auto   tokenParamater = request->getParam(ACCESS_TOKEN_PARAMATER);
        String value          = tokenParamater->value();
        return authenticateJWT(value);
    }
    return {};
}

void SecuritySettingsService::configureJWTHandler() {
    _jwtHandler.setSecret(_state.jwtSecret);
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
        if (_user.username == username && _user.password == password) {
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

ArRequestFilterFunction SecuritySettingsService::filterRequest(AuthenticationPredicate predicate) {
    return [this, predicate](AsyncWebServerRequest * request) {
        Authentication authentication = authenticateRequest(request);
        return predicate(authentication);
    };
}

ArRequestHandlerFunction SecuritySettingsService::wrapRequest(ArRequestHandlerFunction onRequest, AuthenticationPredicate predicate) {
    return [this, onRequest, predicate](AsyncWebServerRequest * request) {
        Authentication authentication = authenticateRequest(request);
        if (!predicate(authentication)) {
            request->send(401);
            return;
        }
        onRequest(request);
    };
}

ArJsonRequestHandlerFunction SecuritySettingsService::wrapCallback(ArJsonRequestHandlerFunction onRequest, AuthenticationPredicate predicate) {
    return [this, onRequest, predicate](AsyncWebServerRequest * request, JsonVariant json) {
        Authentication authentication = authenticateRequest(request);
        if (!predicate(authentication)) {
            request->send(401);
            return;
        }
        onRequest(request, json);
    };
}

void SecuritySettingsService::generateToken(AsyncWebServerRequest * request) {
    auto usernameParam = request->getParam("username");
    for (const User & _user : _state.users) {
        if (_user.username == usernameParam->value()) {
            auto *     response = new AsyncJsonResponse(false);
            JsonObject root     = response->getRoot();
            root["token"]       = generateJWT(&_user);
            response->setLength();
            request->send(response);
            return;
        }
    }
    request->send(401);
}
