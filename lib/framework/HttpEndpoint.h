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
        // We can't use HTTP_ANY and process one a single endpoint due to the way the ESPAsyncWebServer library works
        // Could also use server->on() but this is more efficient

        // create the GET
        GEThandler = new AsyncCallbackWebHandler();
        GEThandler->setUri(servicePath);
        GEThandler->setMethod(HTTP_GET);
        GEThandler->onRequest(securityManager->wrapRequest(std::bind(&HttpEndpoint::fetchSettings, this, _1), authenticationPredicate));
        server->addHandler(GEThandler);

        // create the POST
        POSThandler =
            new AsyncCallbackJsonWebHandler(servicePath,
                                            securityManager->wrapCallback(std::bind(&HttpEndpoint::updateSettings, this, _1, _2), authenticationPredicate));
        POSThandler->setMethod(HTTP_POST);
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
