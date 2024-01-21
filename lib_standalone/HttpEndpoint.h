#ifndef HttpEndpoint_h
#define HttpEndpoint_h

#include <functional>

#include <AsyncJson.h>
#include <ESPAsyncWebServer.h>

#include <SecurityManager.h>
#include <StatefulService.h>

#define HTTP_ENDPOINT_ORIGIN_ID "http"

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
    }

    HttpGetEndpoint(JsonStateReader<T> stateReader, StatefulService<T> * statefulService, AsyncWebServer * server, const String & servicePath)
        : _stateReader(stateReader)
        , _statefulService(statefulService) {
    }

  protected:
    JsonStateReader<T>   _stateReader;
    StatefulService<T> * _statefulService;

    void fetchSettings(AsyncWebServerRequest * request) {
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
        , _statefulService(statefulService) {
    }

    HttpPostEndpoint(JsonStateReader<T>   stateReader,
                     JsonStateUpdater<T>  stateUpdater,
                     StatefulService<T> * statefulService,
                     AsyncWebServer *     server,
                     const String &       servicePath)
        : _stateReader(stateReader)
        , _stateUpdater(stateUpdater)
        , _statefulService(statefulService) {
    }

  protected:
    JsonStateReader<T>   _stateReader;
    JsonStateUpdater<T>  _stateUpdater;
    StatefulService<T> * _statefulService;

    void updateSettings(AsyncWebServerRequest * request, JsonVariant json) {
        if (!json.is<JsonObject>()) {
            return;
        }
        JsonObject        jsonObject = json.as<JsonObject>();
        StateUpdateResult outcome    = _statefulService->updateWithoutPropagation(jsonObject, _stateUpdater);
        if (outcome == StateUpdateResult::ERROR) {
            return;
        }
        if (outcome == StateUpdateResult::CHANGED) {
        }
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
