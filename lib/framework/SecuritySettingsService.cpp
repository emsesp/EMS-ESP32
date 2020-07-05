#include <SecuritySettingsService.h>

#if FT_ENABLED(FT_SECURITY)

SecuritySettingsService::SecuritySettingsService(AsyncWebServer* server, FS* fs) :
    _httpEndpoint(SecuritySettings::read, SecuritySettings::update, this, server, SECURITY_SETTINGS_PATH, this),
    _fsPersistence(SecuritySettings::read, SecuritySettings::update, this, fs, SECURITY_SETTINGS_FILE),
    _jwtHandler(FACTORY_JWT_SECRET) {
  addUpdateHandler([&](const String& originId) { configureJWTHandler(); }, false);
}

void SecuritySettingsService::begin() {
  _fsPersistence.readFromFS();
  configureJWTHandler();
}

Authentication SecuritySettingsService::authenticateRequest(AsyncWebServerRequest* request) {
  AsyncWebHeader* authorizationHeader = request->getHeader(AUTHORIZATION_HEADER);
  if (authorizationHeader) {
    String value = authorizationHeader->value();
    if (value.startsWith(AUTHORIZATION_HEADER_PREFIX)) {
      value = value.substring(AUTHORIZATION_HEADER_PREFIX_LEN);
      return authenticateJWT(value);
    }
  } else if (request->hasParam(ACCESS_TOKEN_PARAMATER)) {
    AsyncWebParameter* tokenParamater = request->getParam(ACCESS_TOKEN_PARAMATER);
    String value = tokenParamater->value();
    return authenticateJWT(value);
  }
  return Authentication();
}

void SecuritySettingsService::configureJWTHandler() {
  _jwtHandler.setSecret(_state.jwtSecret);
}

Authentication SecuritySettingsService::authenticateJWT(String& jwt) {
  DynamicJsonDocument payloadDocument(MAX_JWT_SIZE);
  _jwtHandler.parseJWT(jwt, payloadDocument);
  if (payloadDocument.is<JsonObject>()) {
    JsonObject parsedPayload = payloadDocument.as<JsonObject>();
    String username = parsedPayload["username"];
    for (User _user : _state.users) {
      if (_user.username == username && validatePayload(parsedPayload, &_user)) {
        return Authentication(_user);
      }
    }
  }
  return Authentication();
}

Authentication SecuritySettingsService::authenticate(const String& username, const String& password) {
  for (User _user : _state.users) {
    if (_user.username == username && _user.password == password) {
      return Authentication(_user);
    }
  }
  return Authentication();
}

inline void populateJWTPayload(JsonObject& payload, User* user) {
  payload["username"] = user->username;
  payload["admin"] = user->admin;
}

boolean SecuritySettingsService::validatePayload(JsonObject& parsedPayload, User* user) {
  DynamicJsonDocument jsonDocument(MAX_JWT_SIZE);
  JsonObject payload = jsonDocument.to<JsonObject>();
  populateJWTPayload(payload, user);
  return payload == parsedPayload;
}

String SecuritySettingsService::generateJWT(User* user) {
  DynamicJsonDocument jsonDocument(MAX_JWT_SIZE);
  JsonObject payload = jsonDocument.to<JsonObject>();
  populateJWTPayload(payload, user);
  return _jwtHandler.buildJWT(payload);
}

ArRequestFilterFunction SecuritySettingsService::filterRequest(AuthenticationPredicate predicate) {
  return [this, predicate](AsyncWebServerRequest* request) {
    Authentication authentication = authenticateRequest(request);
    return predicate(authentication);
  };
}

ArRequestHandlerFunction SecuritySettingsService::wrapRequest(ArRequestHandlerFunction onRequest,
                                                              AuthenticationPredicate predicate) {
  return [this, onRequest, predicate](AsyncWebServerRequest* request) {
    Authentication authentication = authenticateRequest(request);
    if (!predicate(authentication)) {
      request->send(401);
      return;
    }
    onRequest(request);
  };
}

ArJsonRequestHandlerFunction SecuritySettingsService::wrapCallback(ArJsonRequestHandlerFunction onRequest,
                                                                   AuthenticationPredicate predicate) {
  return [this, onRequest, predicate](AsyncWebServerRequest* request, JsonVariant& json) {
    Authentication authentication = authenticateRequest(request);
    if (!predicate(authentication)) {
      request->send(401);
      return;
    }
    onRequest(request, json);
  };
}

#else

User ADMIN_USER = User(FACTORY_ADMIN_USERNAME, FACTORY_ADMIN_PASSWORD, true);

SecuritySettingsService::SecuritySettingsService(AsyncWebServer* server, FS* fs) : SecurityManager() {
}
SecuritySettingsService::~SecuritySettingsService() {
}

ArRequestFilterFunction SecuritySettingsService::filterRequest(AuthenticationPredicate predicate) {
  return [this, predicate](AsyncWebServerRequest* request) { return true; };
}

// Return the admin user on all request - disabling security features
Authentication SecuritySettingsService::authenticateRequest(AsyncWebServerRequest* request) {
  return Authentication(ADMIN_USER);
}

// Return the function unwrapped
ArRequestHandlerFunction SecuritySettingsService::wrapRequest(ArRequestHandlerFunction onRequest,
                                                              AuthenticationPredicate predicate) {
  return onRequest;
}

ArJsonRequestHandlerFunction SecuritySettingsService::wrapCallback(ArJsonRequestHandlerFunction onRequest,
                                                                   AuthenticationPredicate predicate) {
  return onRequest;
}

#endif
