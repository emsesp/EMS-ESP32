#include "AuthenticationService.h"

AuthenticationService::AuthenticationService(AsyncWebServer * server, SecurityManager * securityManager)
    : _securityManager(securityManager) {
    server->on(VERIFY_AUTHORIZATION_PATH, HTTP_GET, [this](AsyncWebServerRequest * request) { verifyAuthorization(request); });
    server->on(SIGN_IN_PATH, [this](AsyncWebServerRequest * request, JsonVariant json) { signIn(request, json); });
}

// Verifies that the request supplied a valid JWT.
void AuthenticationService::verifyAuthorization(AsyncWebServerRequest * request) {
    Authentication authentication = _securityManager->authenticateRequest(request);
    request->send(authentication.authenticated ? 200 : 401);
}

// Signs in a user if the username and password match. Provides a JWT to be used in the Authorization header in subsequent requests.
void AuthenticationService::signIn(AsyncWebServerRequest * request, JsonVariant json) {
    if (json.is<JsonObject>()) {
        String         username       = json["username"];
        String         password       = json["password"];
        Authentication authentication = _securityManager->authenticate(username, password);
        if (authentication.authenticated) {
            User *     user            = authentication.user;
            auto *     response        = new AsyncJsonResponse(false);
            JsonObject jsonObject      = response->getRoot();
            jsonObject["access_token"] = _securityManager->generateJWT(user);
            response->setLength();
            request->send(response);
            return;
        }
    }

    AsyncWebServerResponse * response = request->beginResponse(401);
    request->send(response);
}
