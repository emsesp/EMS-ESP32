#ifndef HttpEndpoint_h
#define HttpEndpoint_h

#include <functional>

#include <PsychicHttp.h>

#include <SecurityManager.h>
#include <StatefulService.h>

#define HTTP_ENDPOINT_ORIGIN_ID "http"
#define HTTPS_ENDPOINT_ORIGIN_ID "https"

using namespace std::placeholders; // for `_1` etc

template <class T>
class HttpEndpoint {
  protected:
    JsonStateReader<T>      _stateReader;
    JsonStateUpdater<T>     _stateUpdater;
    StatefulService<T> *    _statefulService;
    size_t                  _bufferSize;
    SecurityManager *       _securityManager;
    AuthenticationPredicate _authenticationPredicate;
    PsychicHttpServer *     _server;
    const char *            _servicePath;

  public:
    HttpEndpoint(JsonStateReader<T>      stateReader,
                 JsonStateUpdater<T>     stateUpdater,
                 StatefulService<T> *    statefulService,
                 PsychicHttpServer *     server,
                 const char *            servicePath,
                 SecurityManager *       securityManager,
                 AuthenticationPredicate authenticationPredicate = AuthenticationPredicates::IS_ADMIN,
                 size_t                  bufferSize              = DEFAULT_BUFFER_SIZE)
        : _stateReader(stateReader)
        , _stateUpdater(stateUpdater)
        , _statefulService(statefulService)
        , _server(server)
        , _servicePath(servicePath)
        , _securityManager(securityManager)
        , _authenticationPredicate(authenticationPredicate)
        , _bufferSize(bufferSize) {
    }

    // register the web server on() endpoints
    void registerURI() {
        _server->on(_servicePath,
                    HTTP_GET,
                    _securityManager->wrapRequest(
                        [this](PsychicRequest * request) {
                            PsychicJsonResponse response   = PsychicJsonResponse(request, false, _bufferSize);
                            JsonObject          jsonObject = response.getRoot();
                            _statefulService->read(jsonObject, _stateReader);
                            return response.send();
                        },
                        _authenticationPredicate));

        _server->on(_servicePath,
                    HTTP_POST,
                    _securityManager->wrapCallback(
                        [this](PsychicRequest * request, JsonVariant & json) {
                            if (!json.is<JsonObject>()) {
                                return request->reply(400);
                            }

                            JsonObject        jsonObject = json.as<JsonObject>();
                            StateUpdateResult outcome    = _statefulService->updateWithoutPropagation(jsonObject, _stateUpdater);

                            if (outcome == StateUpdateResult::ERROR) {
                                return request->reply(400);
                            } else if ((outcome == StateUpdateResult::CHANGED) || (outcome == StateUpdateResult::CHANGED_RESTART)) {
                                // persist the changes to the FS
                                _statefulService->callUpdateHandlers(HTTP_ENDPOINT_ORIGIN_ID);
                            }

                            PsychicJsonResponse response = PsychicJsonResponse(request, false, _bufferSize);
                            jsonObject                   = response.getRoot();

                            _statefulService->read(jsonObject, _stateReader);

                            if (outcome == StateUpdateResult::CHANGED_RESTART) {
                                response.setCode(205); // reboot required
                            }

                            return response.send();
                        },
                        _authenticationPredicate));
    }
};

#endif
