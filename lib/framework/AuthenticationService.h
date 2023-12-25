#ifndef AuthenticationService_H_
#define AuthenticationService_H_

#include <Features.h>
#include <PsychicHttp.h>
#include <SecurityManager.h>

#define VERIFY_AUTHORIZATION_PATH "/rest/verifyAuthorization"
#define SIGN_IN_PATH "/rest/signIn"

class AuthenticationService {
  public:
    AuthenticationService(PsychicHttpServer * server, SecurityManager * securityManager);
    void registerURI();

  private:
    SecurityManager *   _securityManager;
    PsychicHttpServer * _server;
};

#endif
