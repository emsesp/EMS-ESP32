#ifndef AuthenticationService_H_
#define AuthenticationService_H_

#include <Features.h>
#include <AsyncJson.h>
#include <ESPAsyncWebServer.h>
#include <SecurityManager.h>

#define VERIFY_AUTHORIZATION_PATH "/rest/verifyAuthorization"
#define SIGN_IN_PATH "/rest/signIn"

#define MAX_AUTHENTICATION_SIZE 256

#if FT_ENABLED(FT_SECURITY)

class AuthenticationService {
 public:
  AuthenticationService(AsyncWebServer* server, SecurityManager* securityManager);

 private:
  SecurityManager* _securityManager;
  AsyncCallbackJsonWebHandler _signInHandler;

  // endpoint functions
  void signIn(AsyncWebServerRequest* request, JsonVariant& json);
  void verifyAuthorization(AsyncWebServerRequest* request);
};

#endif  // end FT_ENABLED(FT_SECURITY)
#endif  // end SecurityManager_h
