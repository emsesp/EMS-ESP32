#ifndef HttpEndpoint_h
#define HttpEndpoint_h

#include <functional>

#include <ESPAsyncWebServer.h>

#include <SecurityManager.h>
#include <StatefulService.h>

#define HTTP_ENDPOINT_ORIGIN_ID "http"

using namespace std::placeholders; // for `_1` etc

template <class T>
class HttpGetEndpoint {
  public:
    HttpGetEndpoint(JsonStateReader<T>      stateReader,
                    StatefulService<T> *    statefulService,
                    AsyncWebServer *        server,
                    const String &          servicePath,
                    SecurityManager *       securityManager,
                    AuthenticationPredicate authenticationPredicate = AuthenticationPredicates::IS_ADMIN)
        : _stateReader(stateReader)
        , _statefulService(statefulService) {
        server->on(servicePath.c_str(), HTTP_GET, securityManager->wrapRequest(std::bind(&HttpGetEndpoint::fetchSettings, this, _1), authenticationPredicate));
    }

    HttpGetEndpoint(JsonStateReader<T> stateReader, StatefulService<T> * statefulService, AsyncWebServer * server, const String & servicePath)
        : _stateReader(stateReader)
        , _statefulService(statefulService) {
        server->on(servicePath.c_str(), HTTP_GET, std::bind(&HttpGetEndpoint::fetchSettings, this, _1));
    }

  protected:
    JsonStateReader<T>   _stateReader;
    StatefulService<T> * _statefulService;

    void fetchSettings(AsyncWebServerRequest * request) {
        AsyncJsonResponse * response   = new AsyncJsonResponse(false);
        JsonObject          jsonObject = response->getRoot().to<JsonObject>();
        _statefulService->read(jsonObject, _stateReader);

        response->setLength();
        request->send(response);
    }
};

template <class T>
class HttpPostEndpoint {
  public:
    HttpPostEndpoint(JsonStateReader<T>      stateReader,
                     JsonStateUpdater<T>     stateUpdater,
                     StatefulService<T> *    statefulService,
                     AsyncWebServer *        server,
                     const String &          servicePath,
                     SecurityManager *       securityManager,
                     AuthenticationPredicate authenticationPredicate = AuthenticationPredicates::IS_ADMIN)
        : _stateReader(stateReader)
        , _stateUpdater(stateUpdater)
        , _statefulService(statefulService)
        , _updateHandler(servicePath, securityManager->wrapCallback(std::bind(&HttpPostEndpoint::updateSettings, this, _1, _2), authenticationPredicate)) {
        _updateHandler.setMethod(HTTP_POST);
        server->addHandler(&_updateHandler);
    }

    HttpPostEndpoint(JsonStateReader<T>   stateReader,
                     JsonStateUpdater<T>  stateUpdater,
                     StatefulService<T> * statefulService,
                     AsyncWebServer *     server,
                     const String &       servicePath)
        : _stateReader(stateReader)
        , _stateUpdater(stateUpdater)
        , _statefulService(statefulService)
        , _updateHandler(servicePath, std::bind(&HttpPostEndpoint::updateSettings, this, _1, _2)) {
        _updateHandler.setMethod(HTTP_POST);
        server->addHandler(&_updateHandler);
    }

  protected:
    JsonStateReader<T>          _stateReader;
    JsonStateUpdater<T>         _stateUpdater;
    StatefulService<T> *        _statefulService;
    AsyncCallbackJsonWebHandler _updateHandler;

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
};

template <class T>
class HttpEndpoint : public HttpGetEndpoint<T>, public HttpPostEndpoint<T> {
  public:
    HttpEndpoint(JsonStateReader<T>      stateReader,
                 JsonStateUpdater<T>     stateUpdater,
                 StatefulService<T> *    statefulService,
                 AsyncWebServer *        server,
                 const String &          servicePath,
                 SecurityManager *       securityManager,
                 AuthenticationPredicate authenticationPredicate = AuthenticationPredicates::IS_ADMIN)
        : HttpGetEndpoint<T>(stateReader, statefulService, server, servicePath, securityManager, authenticationPredicate)
        , HttpPostEndpoint<T>(stateReader, stateUpdater, statefulService, server, servicePath, securityManager, authenticationPredicate) {
    }

    HttpEndpoint(JsonStateReader<T>   stateReader,
                 JsonStateUpdater<T>  stateUpdater,
                 StatefulService<T> * statefulService,
                 AsyncWebServer *     server,
                 const String &       servicePath)
        : HttpGetEndpoint<T>(stateReader, statefulService, server, servicePath)
        , HttpPostEndpoint<T>(stateReader, stateUpdater, statefulService, server, servicePath) {
    }
};

#endif
