#ifndef HttpEndpoint_h
#define HttpEndpoint_h

#include <functional>
#include <ESPAsyncWebServer.h>

#include "SecurityManager.h"
#include "StatefulService.h"

#define HTTP_ENDPOINT_ORIGIN_ID "http"

template <class T>
class HttpEndpoint {
  protected:
    JsonStateReader<T>   _stateReader;
    JsonStateUpdater<T>  _stateUpdater;
    StatefulService<T> * _statefulService;

  public:
    HttpEndpoint(JsonStateReader<T>      stateReader,
                 JsonStateUpdater<T>     stateUpdater,
                 StatefulService<T> *    statefulService,
                 AsyncWebServer *        server,
                 const String &          servicePath,
                 SecurityManager *       securityManager,
                 AuthenticationPredicate authenticationPredicate = AuthenticationPredicates::IS_ADMIN)
        : _stateReader(stateReader)
        , _stateUpdater(stateUpdater)
        , _statefulService(statefulService) {
        // Create handler for both GET and POST endpoints
        securityManager->addEndpoint(
            server, servicePath, authenticationPredicate, [this](AsyncWebServerRequest * request, JsonVariant json) { handleRequest(request, json); }, HTTP_ANY); // ALL methods
    }

  protected:
    // for POST
    void handleRequest(AsyncWebServerRequest * request, JsonVariant json) {
        if (request->method() == HTTP_POST) {
            // Handle POST
            if (!json.is<JsonObject>()) {
                request->send(400); // error, bad request
                return;
            }

            StateUpdateResult outcome = _statefulService->updateWithoutPropagation(json.as<JsonObject>(), _stateUpdater);

            if (outcome == StateUpdateResult::ERROR) {
                request->send(400); // error, bad request
                return;
            } else if (outcome == StateUpdateResult::CHANGED || outcome == StateUpdateResult::CHANGED_RESTART) {
                // persist changes
                request->onDisconnect([this] { _statefulService->callUpdateHandlers(); });
                if (outcome == StateUpdateResult::CHANGED_RESTART) {
                    request->send(205); // reboot required
                    return;
                }
            }
        }

        auto *     response   = new AsyncJsonResponse(false);
        JsonObject jsonObject = response->getRoot().to<JsonObject>();
        _statefulService->read(jsonObject, _stateReader);
        response->setLength();
        request->send(response);
    }
};

#endif
