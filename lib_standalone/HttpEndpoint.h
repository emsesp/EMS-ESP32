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
                    AuthenticationPredicate authenticationPredicate = AuthenticationPredicates::IS_ADMIN,
                    size_t                  bufferSize              = DEFAULT_BUFFER_SIZE)
        : _stateReader(stateReader)
        , _statefulService(statefulService)
        , _bufferSize(bufferSize) {
    }

    HttpGetEndpoint(JsonStateReader<T>   stateReader,
                    StatefulService<T> * statefulService,
                    AsyncWebServer *     server,
                    const String &       servicePath,
                    size_t               bufferSize = DEFAULT_BUFFER_SIZE)
        : _stateReader(stateReader)
        , _statefulService(statefulService)
        , _bufferSize(bufferSize) {
    }

  protected:
    JsonStateReader<T>   _stateReader;
    StatefulService<T> * _statefulService;
    size_t               _bufferSize;

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
                     AuthenticationPredicate authenticationPredicate = AuthenticationPredicates::IS_ADMIN,
                     size_t                  bufferSize              = DEFAULT_BUFFER_SIZE)
        : _stateReader(stateReader)
        , _stateUpdater(stateUpdater)
        , _statefulService(statefulService)
        , _bufferSize(bufferSize) {
    }

    HttpPostEndpoint(JsonStateReader<T>   stateReader,
                     JsonStateUpdater<T>  stateUpdater,
                     StatefulService<T> * statefulService,
                     AsyncWebServer *     server,
                     const String &       servicePath,
                     size_t               bufferSize = DEFAULT_BUFFER_SIZE)
        : _stateReader(stateReader)
        , _stateUpdater(stateUpdater)
        , _statefulService(statefulService)
        , _bufferSize(bufferSize) {
    }

  protected:
    JsonStateReader<T>   _stateReader;
    JsonStateUpdater<T>  _stateUpdater;
    StatefulService<T> * _statefulService;
    size_t               _bufferSize;

    void updateSettings(AsyncWebServerRequest * request, JsonVariant & json) {
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
                 AuthenticationPredicate authenticationPredicate = AuthenticationPredicates::IS_ADMIN,
                 size_t                  bufferSize              = DEFAULT_BUFFER_SIZE)
        : HttpGetEndpoint<T>(stateReader, statefulService, server, servicePath, securityManager, authenticationPredicate, bufferSize)
        , HttpPostEndpoint<T>(stateReader, stateUpdater, statefulService, server, servicePath, securityManager, authenticationPredicate, bufferSize) {
    }

    HttpEndpoint(JsonStateReader<T>   stateReader,
                 JsonStateUpdater<T>  stateUpdater,
                 StatefulService<T> * statefulService,
                 AsyncWebServer *     server,
                 const String &       servicePath,
                 size_t               bufferSize = DEFAULT_BUFFER_SIZE)
        : HttpGetEndpoint<T>(stateReader, statefulService, server, servicePath, bufferSize)
        , HttpPostEndpoint<T>(stateReader, stateUpdater, statefulService, server, servicePath, bufferSize) {
    }
};

#endif
