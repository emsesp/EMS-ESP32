#include "SecurityManager.h"

void SecurityManager::addEndpoint(AsyncWebServer *             server,
                                  const String &               path,
                                  AuthenticationPredicate      predicate,
                                  ArJsonRequestHandlerFunction function,
                                  WebRequestMethodComposite    method) {
    ensureRestDispatcher(server);
    const bool requiresAuth = !isPublicPredicate(predicate);
    _restRoutes.push_back({AsyncURIMatcher(path), method, std::move(predicate), {}, std::move(function), true, requiresAuth});
}

void SecurityManager::addEndpoint(AsyncWebServer *          server,
                                  const String &            path,
                                  AuthenticationPredicate   predicate,
                                  ArRequestHandlerFunction  function,
                                  WebRequestMethodComposite method) {
    ensureRestDispatcher(server);
    const bool requiresAuth = !isPublicPredicate(predicate);
    _restRoutes.push_back({AsyncURIMatcher(path), method, std::move(predicate), std::move(function), {}, false, requiresAuth});
}

// Detects routes registered with AuthenticationPredicates::NONE_REQUIRED so we can
// skip the JWT parse in dispatchRest. Relies on std::function::target returning the
// raw function pointer when the predicate was assigned directly from the static fn;
// if a caller wraps NONE_REQUIRED in a lambda this falls back to "requires auth"
// which is correctness-preserving (just no optimization).
bool SecurityManager::isPublicPredicate(const AuthenticationPredicate & predicate) {
    using Fn  = bool (*)(const Authentication &);
    auto * fn = predicate.target<Fn>();
    return fn != nullptr && *fn == &AuthenticationPredicates::NONE_REQUIRED;
}

// Lazily attach the single catch-all handler. Idempotent.
void SecurityManager::ensureRestDispatcher(AsyncWebServer * server) {
    if (_restDispatcherInstalled || server == nullptr) {
        return;
    }

    _restDispatcherInstalled = true;
    server->addHandler(new RestCatchAllHandler(this));
}

void SecurityManager::dispatchRest(AsyncWebServerRequest * request, JsonVariant json) {
    WebRequestMethod method = request->method();

    for (auto & route : _restRoutes) {
        if (!route.method.matches(method)) {
            continue;
        }

        if (!route.uri.matches(request)) {
            continue;
        }

        if (route.requiresAuth) {
            Authentication authentication = authenticateRequest(request);
            if (!route.predicate(authentication)) {
                request->send(401);
                return;
            }
        }

        if (route.isJson) {
            route.jsonHandler(request, json);
        } else {
            route.plainHandler(request);
        }

        return;
    }

    // canHandle returned true so some route matched the URI+method;
    // a mismatch here means the request slipped between the two checks.
    request->send(404);
}

// ---- RestCatchAllHandler ----

bool SecurityManager::RestCatchAllHandler::canHandle(AsyncWebServerRequest * request) const {
    if (!request->isHTTP()) {
        return false;
    }

    for (const auto & route : _owner->_restRoutes) {
        if (route.method.matches(request->method()) && route.uri.matches(request)) {
            return true;
        }
    }

    return false;
}

// Returning false ensures the server invokes handleBody() so we can buffer JSON bodies.
bool SecurityManager::RestCatchAllHandler::isRequestHandlerTrivial() const {
    return false;
}

void SecurityManager::RestCatchAllHandler::handleBody(AsyncWebServerRequest * request, uint8_t * data, size_t len, size_t index, size_t total) {
    // Only buffer JSON bodies; everything else is routed with an empty JsonVariant.
    if (total == 0 || total > kMaxBodySize || !isJsonContent(request)) {
        return;
    }

    if (index == 0 && request->_tempObject == nullptr) {
        request->_tempObject = calloc(total + 1, sizeof(uint8_t)); // freed by request destructor
        if (request->_tempObject == nullptr) {
            request->abort();
            return;
        }
    }

    if (request->_tempObject != nullptr) {
        memcpy(static_cast<uint8_t *>(request->_tempObject) + index, data, len);
    }
}

void SecurityManager::RestCatchAllHandler::handleRequest(AsyncWebServerRequest * request) {
    JsonDocument doc;
    JsonVariant  json;
    if (request->_tempObject != nullptr) {
        if (deserializeJson(doc, static_cast<const char *>(request->_tempObject))) {
            request->send(400);
            return;
        }

        json = doc.as<JsonVariant>();
    }

    _owner->dispatchRest(request, json);
}

bool SecurityManager::RestCatchAllHandler::isJsonContent(AsyncWebServerRequest * request) {
    return request->contentType().equalsIgnoreCase("application/json");
}
