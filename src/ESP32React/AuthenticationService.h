#ifndef AuthenticationService_H_
#define AuthenticationService_H_

#include "SecurityManager.h"

#include <ESPAsyncWebServer.h>

#define VERIFY_AUTHORIZATION_PATH "/rest/verifyAuthorization"
#define SIGN_IN_PATH "/rest/signIn"

class AuthenticationService {
  public:
    AuthenticationService(AsyncWebServer * server, SecurityManager * securityManager);

  private:
    SecurityManager * _securityManager;

    void signIn(AsyncWebServerRequest * request, JsonVariant json);
    void verifyAuthorization(AsyncWebServerRequest * request);
};

#endif
