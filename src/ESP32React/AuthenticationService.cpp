#include "AuthenticationService.h"

#include "../core/psram_async_json_response.h"
#include "../core/login_limiter.h"

#include <emsesp.h>

AuthenticationService::AuthenticationService(AsyncWebServer * server, SecurityManager * securityManager)
    : _securityManager(securityManager) {
    // None of these need authentication: verifyAuthorization checks the JWT itself, and signIn IS the authentication flow.
    securityManager->addEndpoint(server, VERIFY_AUTHORIZATION_PATH, AuthenticationPredicates::NONE_REQUIRED, [this](AsyncWebServerRequest * request) {
        verifyAuthorization(request);
    });
    securityManager->addEndpoint(server, SIGN_IN_PATH, AuthenticationPredicates::NONE_REQUIRED, [this](AsyncWebServerRequest * request, JsonVariant json) {
        signIn(request, json);
    });
    securityManager->addEndpoint(server, CONFIRM_PASSWORD_PATH, AuthenticationPredicates::IS_AUTHENTICATED, [this](AsyncWebServerRequest * request) {
        passwordConfirmed(request);
    });
    securityManager->addEndpoint(server,
                                 CONFIRM_PASSWORD_PATH,
                                 AuthenticationPredicates::IS_AUTHENTICATED,
                                 [this](AsyncWebServerRequest * request, JsonVariant json) { confirmPassword(request, json); });
}

// Verifies that the request supplied a valid JWT.
void AuthenticationService::verifyAuthorization(AsyncWebServerRequest * request) {
    Authentication authentication = _securityManager->authenticateRequest(request);
    request->send(authentication.authenticated ? 200 : 401);
}

// lets the Web UI ask for the password before a large upload, instead of after it
void AuthenticationService::passwordConfirmed(AsyncWebServerRequest * request) {
    Authentication authentication = _securityManager->authenticateRequest(request);
    auto *         response       = new emsesp::PsramAsyncJsonResponse(false);
    JsonObject     root           = response->getRoot();
    root["confirmed"]             = authentication.password_confirmed;
    response->setLength();
    request->send(response);
}

// the password of the signed-in user, entered again before changes to the firmware, the users or the security settings
void AuthenticationService::confirmPassword(AsyncWebServerRequest * request, JsonVariant json) {
    auto &         limiter = LoginLimiter::instance();
    const String   source  = request->client()->remoteIP().toString();
    const uint32_t wait    = limiter.retry_after(source.c_str(), millis());
    if (wait) {
        AsyncWebServerResponse * response = request->beginResponse(429);
        response->addHeader("Retry-After", String((wait + 999) / 1000));
        request->send(response);
        return;
    }

    if (_securityManager->confirmPassword(request, json["password"] | "")) {
        limiter.succeeded(source.c_str());
        request->send(200);
        return;
    }

    limiter.failed(source.c_str(), millis());
    emsesp::EMSESP::logger().notice("Wrong password entered again from %s", source.c_str());
    request->send(403); // not 401, the session itself is still valid
}

// Signs in a user if the username and password match. Provides a JWT to be used in the Authorization header in subsequent requests.
void AuthenticationService::signIn(AsyncWebServerRequest * request, JsonVariant json) {
    auto &         limiter = LoginLimiter::instance();
    const String   source  = request->client()->remoteIP().toString();
    const uint32_t wait    = limiter.retry_after(source.c_str(), millis());
    if (wait) {
        AsyncWebServerResponse * response = request->beginResponse(429);
        response->addHeader("Retry-After", String((wait + 999) / 1000));
        request->send(response);
        return;
    }

    if (json.is<JsonObject>()) {
        String         username       = json["username"];
        String         password       = json["password"];
        Authentication authentication = _securityManager->authenticate(username, password);
        if (authentication.authenticated) {
            limiter.succeeded(source.c_str());
            auto *     response        = new emsesp::PsramAsyncJsonResponse(false);
            JsonObject jsonObject      = response->getRoot();
            jsonObject["access_token"] = _securityManager->generateJWT(authentication.user.get());
            response->setLength();
            request->send(response);
            return;
        }
    }

    limiter.failed(source.c_str(), millis());
    emsesp::EMSESP::logger().notice("Failed sign-in for user '%s' from %s", json["username"] | "", source.c_str());

    AsyncWebServerResponse * response = request->beginResponse(401);
    request->send(response);
}
