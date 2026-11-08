#include "AuthenticationService.h"

#include "../core/psram_async_json_response.h"

AuthenticationService::AuthenticationService(AsyncWebServer * server, SecurityManager * securityManager)
    : _securityManager(securityManager) {
    // None of these need authentication: verifyAuthorization checks the JWT itself, and signIn IS the authentication flow.
    securityManager->addEndpoint(server, VERIFY_AUTHORIZATION_PATH, AuthenticationPredicates::NONE_REQUIRED, [this](AsyncWebServerRequest * request) {
        verifyAuthorization(request);
    });
    securityManager->addEndpoint(server, SIGN_IN_PATH, AuthenticationPredicates::NONE_REQUIRED, [this](AsyncWebServerRequest * request, JsonVariant json) {
        signIn(request, json);
    });
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
            auto *     response        = new emsesp::PsramAsyncJsonResponse(false);
            JsonObject jsonObject      = response->getRoot();
            jsonObject["access_token"] = _securityManager->generateJWT(authentication.user.get());
            response->setLength();
            request->send(response);
            return;
        }
    }

    AsyncWebServerResponse * response = request->beginResponse(401);
    request->send(response);
}
