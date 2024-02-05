#ifndef HttpEndpoint_h
#define HttpEndpoint_h

#include <functional>

#include <ESPAsyncWebServer.h>

#include <SecurityManager.h>
#include <StatefulService.h>

#define HTTP_ENDPOINT_ORIGIN_ID "http"

using namespace std::placeholders; // for `_1` etc

template <class T>
class HttpEndpoint {
  protected:
    JsonStateReader<T>   _stateReader;
    JsonStateUpdater<T>  _stateUpdater;
    StatefulService<T> * _statefulService;

    AsyncCallbackWebHandler *     GEThandler;
    AsyncCallbackJsonWebHandler * POSThandler;

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
        // Create the GET and POST endpoints
        POSThandler = new AsyncCallbackJsonWebHandler(servicePath,
                                                      securityManager->wrapCallback(
                                                          [this](AsyncWebServerRequest * request, JsonVariant json) {
                                                              //
                                                              if (request->method() == HTTP_GET) {
                                                                  fetchSettings(request);
                                                              } else if (request->method() == HTTP_POST) {
                                                                  updateSettings(request, json);
                                                              } else {
                                                                  request->send(405, "application/json", "{\"message\":\"Method Not Allowed\"}");
                                                              }
                                                          },
                                                          authenticationPredicate));
        server->addHandler(POSThandler);
    }

  protected:
    // for POST
    void updateSettings(AsyncWebServerRequest * request, JsonVariant json) {
        if (!json.is<JsonObject>()) {
            request->send(400);
            return;
        }

        JsonObject        jsonObject = json.as<JsonObject>();
        StateUpdateResult outcome    = _statefulService->updateWithoutPropagation(jsonObject, _stateUpdater);

        if (outcome == StateUpdateResult::ERROR) {
            request->send(400);
            return;
        } else if ((outcome == StateUpdateResult::CHANGED) || (outcome == StateUpdateResult::CHANGED_RESTART)) {
            request->onDisconnect([this]() { _statefulService->callUpdateHandlers(HTTP_ENDPOINT_ORIGIN_ID); });
        }

        AsyncJsonResponse * response = new AsyncJsonResponse(false);
        jsonObject                   = response->getRoot().to<JsonObject>();
        _statefulService->read(jsonObject, _stateReader);

        if (outcome == StateUpdateResult::CHANGED_RESTART) {
            response->setCode(205); // reboot required
        }

        response->setLength();
        request->send(response);
    }

    // for GET
    void fetchSettings(AsyncWebServerRequest * request) {
        AsyncJsonResponse * response   = new AsyncJsonResponse(false);
        JsonObject          jsonObject = response->getRoot().to<JsonObject>();
        _statefulService->read(jsonObject, _stateReader);

        response->setLength();
        request->send(response);
    }
};

#endif
