#ifndef SecurityManager_h
#define SecurityManager_h

#include "Arduino.h"
#include "ESPAsyncWebServer.h"
#include "AsyncJson.h"

#include <list>

#define FACTORY_JWT_SECRET "ems-esp"
#define ACCESS_TOKEN_PARAMATER "access_token"
#define AUTHORIZATION_HEADER "Authorization"
#define AUTHORIZATION_HEADER_PREFIX "Bearer "
#define AUTHORIZATION_HEADER_PREFIX_LEN 7

#define MAX_JWT_SIZE 128

class User {
  public:
    String username;
    String password;
    bool   admin;

  public:
    User(String username, String password, bool admin)
        : username(username)
        , password(password)
        , admin(admin) {
    }
};

class Authentication {
  public:
    User *  user;
    boolean authenticated;

  public:
    Authentication(User & user)
        : user(new User(user))
        , authenticated(true) {
    }
    Authentication()
        : user(nullptr)
        , authenticated(false) {
    }
    ~Authentication() {
        delete (user);
    }
};

typedef std::function<boolean(Authentication & authentication)> AuthenticationPredicate;

class AuthenticationPredicates {
  public:
    static bool NONE_REQUIRED(Authentication & authentication) {
        return true;
    };
    static bool IS_AUTHENTICATED(Authentication & authentication) {
        return authentication.authenticated;
    };
    static bool IS_ADMIN(Authentication & authentication) {
        return authentication.authenticated && authentication.user->admin;
    };
};

class SecurityManager {
  public:
    virtual Authentication               authenticateRequest(AsyncWebServerRequest * request)                                    = 0;
    virtual ArRequestFilterFunction      filterRequest(AuthenticationPredicate predicate)                                        = 0;
    virtual ArRequestHandlerFunction     wrapRequest(ArRequestHandlerFunction onRequest, AuthenticationPredicate predicate)      = 0;
    virtual ArJsonRequestHandlerFunction wrapCallback(ArJsonRequestHandlerFunction onRequest, AuthenticationPredicate predicate) = 0;

    void addEndpoint(AsyncWebServer *             server,
                     const String &               path,
                     AuthenticationPredicate      predicate,
                     ArJsonRequestHandlerFunction function,
                     WebRequestMethodComposite    method = HTTP_POST) {
    }

    // non-Json endpoints - default GET
    void addEndpoint(AsyncWebServer *          server,
                     const String &            path,
                     AuthenticationPredicate   predicate,
                     ArRequestHandlerFunction  function,
                     WebRequestMethodComposite method = HTTP_GET) {
    }
};

#endif
