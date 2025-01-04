#ifndef SecurityManager_h
#define SecurityManager_h

#include "ArduinoJsonJWT.h"

#include <ESPAsyncWebServer.h>
#include <AsyncJson.h>

#include <list>

#define ACCESS_TOKEN_PARAMATER "access_token"

#define AUTHORIZATION_HEADER "Authorization"
#define AUTHORIZATION_HEADER_PREFIX "Bearer "
#define AUTHORIZATION_HEADER_PREFIX_LEN 7

class User {
  public:
    String username;
    String password;
    bool   admin;

  public:
    User(String username, String password, bool admin)
        : username(std::move(username))
        , password(std::move(password))
        , admin(admin) {
    }
};

class Authentication {
  public:
    User *  user          = nullptr;
    boolean authenticated = false;

  public:
    explicit Authentication(const User & user)
        : user(new User(user))
        , authenticated(true) {
    }

    Authentication() = default;

    ~Authentication() {
        delete user;
    }
};

typedef std::function<boolean(Authentication & authentication)> AuthenticationPredicate;

class AuthenticationPredicates {
  public:
    static bool NONE_REQUIRED(const Authentication & authentication) {
        (void)authentication;
        return true;
    };
    static bool IS_AUTHENTICATED(const Authentication & authentication) {
        return authentication.authenticated;
    };
    static bool IS_ADMIN(const Authentication & authentication) {
        return authentication.authenticated && authentication.user->admin;
    };
};

class SecurityManager {
  public:
    // Authenticate, returning the user if found
    virtual Authentication authenticate(const String & username, const String & password) = 0;

    // Generate a JWT for the user provided
    virtual String generateJWT(const User * user) = 0;

    // Check the request header for the Authorization token
    virtual Authentication authenticateRequest(AsyncWebServerRequest * request) = 0;

    // Filter a request with the provided predicate, only returning true if the predicate matches.
    virtual ArRequestFilterFunction filterRequest(AuthenticationPredicate predicate) = 0;

    // Wrap the provided request to provide validation against an AuthenticationPredicate.
    virtual ArRequestHandlerFunction wrapRequest(ArRequestHandlerFunction onRequest, AuthenticationPredicate predicate) = 0;

    // Wrap the provided json request callback to provide validation against an AuthenticationPredicate.
    virtual ArJsonRequestHandlerFunction wrapCallback(ArJsonRequestHandlerFunction onRequest, AuthenticationPredicate predicate) = 0;

    // Json endpoints - default POST
    void addEndpoint(AsyncWebServer *             server,
                     const String &               path,
                     AuthenticationPredicate      predicate,
                     ArJsonRequestHandlerFunction function,
                     WebRequestMethodComposite    method = HTTP_POST) {
        auto handler = new AsyncCallbackJsonWebHandler(path);
        handler->onRequest(
            wrapCallback([this, function](AsyncWebServerRequest * request, JsonVariant json) { function(request, json); }, AuthenticationPredicates::IS_ADMIN));
        handler->setMethod(method);
        server->addHandler(handler);
    }

    // non-Json endpoints - default GET
    void addEndpoint(AsyncWebServer *          server,
                     const String &            path,
                     AuthenticationPredicate   predicate,
                     ArRequestHandlerFunction  function,
                     WebRequestMethodComposite method = HTTP_GET) {
        auto * handler = new AsyncCallbackWebHandler();
        handler->onRequest(wrapRequest([this, function](AsyncWebServerRequest * request) { function(request); }, predicate));
        handler->setUri(path);
        handler->setMethod(method);
        server->addHandler(handler);
    }
};

#endif
