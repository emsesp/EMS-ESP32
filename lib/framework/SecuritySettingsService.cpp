#include <SecuritySettingsService.h>

#include "../../src/emsesp_stub.hpp"

SecuritySettingsService::SecuritySettingsService(AsyncWebServer * server, FS * fs)
    : _httpEndpoint(SecuritySettings::read, SecuritySettings::update, this, server, SECURITY_SETTINGS_PATH, this)
    , _fsPersistence(SecuritySettings::read, SecuritySettings::update, this, fs, SECURITY_SETTINGS_FILE)
    , _jwtHandler(FACTORY_JWT_SECRET) {
    addUpdateHandler([&] { configureJWTHandler(); }, false);
    server->on(GENERATE_TOKEN_PATH,
               HTTP_GET,
               wrapRequest(std::bind(&SecuritySettingsService::generateToken, this, std::placeholders::_1), AuthenticationPredicates::IS_ADMIN));
}

void SecuritySettingsService::begin() {
    _fsPersistence.readFromFS();
    configureJWTHandler();
}

Authentication SecuritySettingsService::authenticateRequest(AsyncWebServerRequest * request) {
    AsyncWebHeader * authorizationHeader = request->getHeader(AUTHORIZATION_HEADER);
    if (authorizationHeader) {
        String value = authorizationHeader->value();
        if (value.startsWith(AUTHORIZATION_HEADER_PREFIX)) {
            value = value.substring(AUTHORIZATION_HEADER_PREFIX_LEN);
            return authenticateJWT(value);
        }
    } else if (request->hasParam(ACCESS_TOKEN_PARAMATER)) {
        AsyncWebParameter * tokenParamater = request->getParam(ACCESS_TOKEN_PARAMATER);
        String              value          = tokenParamater->value();
        return authenticateJWT(value);
    }
    return Authentication();
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

inline void populateJWTPayload(JsonObject payload, User * user) {
    payload["username"] = user->username;
    payload["admin"]    = user->admin;
}

boolean SecuritySettingsService::validatePayload(JsonObject parsedPayload, User * user) {
    JsonDocument jsonDocument;
    JsonObject   payload = jsonDocument.to<JsonObject>();
    populateJWTPayload(payload, user);
    return payload == parsedPayload;
}

String SecuritySettingsService::generateJWT(User * user) {
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
    AsyncWebParameter * usernameParam = request->getParam("username");
    for (User _user : _state.users) {
        if (_user.username == usernameParam->value()) {
            AsyncJsonResponse * response = new AsyncJsonResponse(false);
            JsonObject          root     = response->getRoot();
            root["token"]                = generateJWT(&_user);
            response->setLength();
            request->send(response);
            return;
        }
    }
    request->send(401);
}

void SecuritySettings::read(SecuritySettings & settings, JsonObject root) {
    // secret
    root["jwt_secret"] = settings.jwtSecret;

    // users
    JsonArray users = root["users"].to<JsonArray>();
    for (User user : settings.users) {
        JsonObject userRoot  = users.add<JsonObject>();
        userRoot["username"] = user.username;
        userRoot["password"] = user.password;
        userRoot["admin"]    = user.admin;
    }
}

StateUpdateResult SecuritySettings::update(JsonObject root, SecuritySettings & settings) {
    // secret
    settings.jwtSecret = root["jwt_secret"] | FACTORY_JWT_SECRET;

    // users
    settings.users.clear();
    if (root["users"].is<JsonArray>()) {
        for (JsonVariant user : root["users"].as<JsonArray>()) {
            settings.users.push_back(User(user["username"], user["password"], user["admin"]));
        }
    } else {
        settings.users.push_back(User(FACTORY_ADMIN_USERNAME, FACTORY_ADMIN_PASSWORD, true));
        settings.users.push_back(User(FACTORY_GUEST_USERNAME, FACTORY_GUEST_PASSWORD, false));
    }
    return StateUpdateResult::CHANGED;
}
