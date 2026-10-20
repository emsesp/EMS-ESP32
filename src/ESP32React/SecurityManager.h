#ifndef SecurityManager_h
#define SecurityManager_h

#include "ArduinoJsonJWT.h"

#include <ESPAsyncWebServer.h>
#include <AsyncJson.h>

#include <memory>
#include <vector>

#define ACCESS_TOKEN_PARAMETER "access_token"
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
    std::unique_ptr<User> user;
    boolean               authenticated = false;

  public:
    explicit Authentication(const User & u)
        : user(std::make_unique<User>(u))
        , authenticated(true) {
    }

    Authentication() = default;
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
    virtual ~SecurityManager() = default;

    // Authenticate, returning the user if found
    virtual Authentication authenticate(const String & username, const String & password) = 0;

    // Generate a JWT for the user provided
    virtual String generateJWT(const User * user) = 0;

    // Check the request header for the Authorization token
    virtual Authentication authenticateRequest(AsyncWebServerRequest * request) = 0;

    // Json endpoints - default POST. Registered with the shared dispatcher.
    void addEndpoint(AsyncWebServer *             server,
                     const String &               path,
                     AuthenticationPredicate      predicate,
                     ArJsonRequestHandlerFunction function,
                     WebRequestMethodComposite    method = HTTP_POST);

    // non-Json endpoints - default GET. Registered with the shared dispatcher.
    void addEndpoint(AsyncWebServer *          server,
                     const String &            path,
                     AuthenticationPredicate   predicate,
                     ArRequestHandlerFunction  function,
                     WebRequestMethodComposite method = HTTP_GET);

  private:
    // Single internal route record. Each route holds either a plain or JSON handler.
    // The URI matcher uses backward-compatible mode by default (constructed from a
    // plain String), which preserves the original library handler's matching semantics
    // (e.g. /api also matches /api/boiler/heating).
    struct RestRoute {
        AsyncURIMatcher              uri;
        WebRequestMethodComposite    method;
        AuthenticationPredicate      predicate;
        ArRequestHandlerFunction     plainHandler;
        ArJsonRequestHandlerFunction jsonHandler;
        bool                         isJson;
        bool                         requiresAuth; // false when predicate is NONE_REQUIRED, lets dispatchRest skip the JWT parse
    };

    // Single catch-all handler. canHandle() claims a request only if some registered
    // route matches its URI + method, so non-matching URLs (static files, websockets,
    // etc.) still fall through to other handlers. handleBody buffers JSON bodies, then
    // handleRequest hands off to SecurityManager::dispatchRest for routing + auth.
    //
    // We can't reuse AsyncCallbackJsonWebHandler here because its canHandle() rejects
    // POST/PUT/PATCH without an application/json content-type (so a bodyless POST like
    // /rest/resetCustomizations would fall through to a 404), and both canHandle and
    // handleRequest are marked final on that class.
    class RestCatchAllHandler : public AsyncWebHandler {
      public:
        explicit RestCatchAllHandler(SecurityManager * owner)
            : _owner(owner) {
        }

        bool canHandle(AsyncWebServerRequest * request) const override;
        bool isRequestHandlerTrivial() const override;
        void handleBody(AsyncWebServerRequest * request, uint8_t * data, size_t len, size_t index, size_t total) override;
        void handleRequest(AsyncWebServerRequest * request) override;

      private:
        static constexpr size_t kMaxBodySize = 16384;

        static bool isJsonContent(AsyncWebServerRequest * request);

        SecurityManager * _owner;
    };

    static bool isPublicPredicate(const AuthenticationPredicate & predicate);

    void ensureRestDispatcher(AsyncWebServer * server);
    void dispatchRest(AsyncWebServerRequest * request, JsonVariant json);

    std::vector<RestRoute> _restRoutes;
    bool                   _restDispatcherInstalled = false;
};

#endif
