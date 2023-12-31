#include <AuthenticationService.h>

using namespace std::placeholders; // for `_1` etc

AuthenticationService::AuthenticationService(PsychicHttpServer * server, SecurityManager * securityManager)
    : _server(server)
    , _securityManager(securityManager) {
}

void AuthenticationService::registerURI() {
    // Signs in a user if the username and password match. Provides a JWT to be used in the Authorization header in subsequent requests
    _server->on(SIGN_IN_PATH, HTTP_POST, [this](PsychicRequest * request, JsonVariant & json) {
        if (json.is<JsonObject>()) {
            String         username       = json["username"];
            String         password       = json["password"];
            Authentication authentication = _securityManager->authenticate(username, password);
            if (authentication.authenticated) {
                PsychicJsonResponse response = PsychicJsonResponse(request, false, 256);
                JsonObject          root     = response.getRoot();
                root["access_token"]         = _securityManager->generateJWT(authentication.user);
                return response.send();
            }
        }
        return request->reply(401);
    });

    // Verifies that the request supplied a valid JWT
    _server->on(VERIFY_AUTHORIZATION_PATH, HTTP_GET, [this](PsychicRequest * request) {
        Authentication authentication = _securityManager->authenticateRequest(request);
        return request->reply(authentication.authenticated ? 200 : 401);
    });
}
