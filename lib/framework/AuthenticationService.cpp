#include <AuthenticationService.h>

using namespace std::placeholders; // for `_1` etc

#if FT_ENABLED(FT_SECURITY)

AuthenticationService::AuthenticationService(AsyncWebServer * server, SecurityManager * securityManager)
    : _securityManager(securityManager)
    , _signInHandler(SIGN_IN_PATH, std::bind(&AuthenticationService::signIn, this, _1, _2)) {
    server->on(VERIFY_AUTHORIZATION_PATH, HTTP_GET, std::bind(&AuthenticationService::verifyAuthorization, this, _1));
    _signInHandler.setMethod(HTTP_POST);
    _signInHandler.setMaxContentLength(MAX_AUTHENTICATION_SIZE);
    server->addHandler(&_signInHandler);
}

/**
 * Verifies that the request supplied a valid JWT.
 */
void AuthenticationService::verifyAuthorization(AsyncWebServerRequest * request) {
    Authentication authentication = _securityManager->authenticateRequest(request);
    request->send(authentication.authenticated ? 200 : 401);
}

/**
 * Signs in a user if the username and password match. Provides a JWT to be used in the Authorization header in
 * subsequent requests.
 */
void AuthenticationService::signIn(AsyncWebServerRequest * request, JsonVariant json) {
    if (json.is<JsonObject>()) {
        String         username       = json["username"];
        String         password       = json["password"];
        Authentication authentication = _securityManager->authenticate(username, password);
        if (authentication.authenticated) {
            User *              user       = authentication.user;
            AsyncJsonResponse * response   = new AsyncJsonResponse(false);
            JsonObject          jsonObject = response->getRoot();
            jsonObject["access_token"]     = _securityManager->generateJWT(user);
            response->setLength();
            request->send(response);
            return;
        }
    }
    AsyncWebServerResponse * response = request->beginResponse(401);
    request->send(response);
}

#endif
