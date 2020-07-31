#ifndef _ESPAsyncWebServer_H_
#define _ESPAsyncWebServer_H_

#include "Arduino.h"

#include <functional>
#include <AsyncTCP.h>
#include <ArduinoJson.h>

class AsyncWebServer;
class AsyncWebServerRequest;
class AsyncWebServerResponse;
class AsyncJsonResponse;

typedef enum {
    HTTP_GET     = 0b00000001,
    HTTP_POST    = 0b00000010,
    HTTP_DELETE  = 0b00000100,
    HTTP_PUT     = 0b00001000,
    HTTP_PATCH   = 0b00010000,
    HTTP_HEAD    = 0b00100000,
    HTTP_OPTIONS = 0b01000000,
    HTTP_ANY     = 0b01111111,
} WebRequestMethod;

typedef uint8_t                   WebRequestMethodComposite;
typedef std::function<void(void)> ArDisconnectHandler;

class AsyncWebServerRequest {
    friend class AsyncWebServer;
    friend class AsyncCallbackWebHandler;

  private:
    AsyncClient *             _client;
    AsyncWebServer *          _server;
    WebRequestMethodComposite _method;

  public:
    void * _tempObject;

    AsyncWebServerRequest(AsyncWebServer *, AsyncClient *);
    ~AsyncWebServerRequest();

    AsyncClient * client() {
        return _client;
    }

    WebRequestMethodComposite method() const {
        return _method;
    }

    void addInterestingHeader(const String & name){};

    void send(AsyncWebServerResponse * response){};
    void send(AsyncJsonResponse * response){};
    void send(int code, const String & contentType = String(), const String & content = String()){};

    AsyncWebServerResponse * beginResponse(int code, const String & contentType = String(), const String & content = String()) {
        // AsyncWebServerResponse *a = new AsyncWebServerResponse()
        return nullptr;
    }

    size_t headers() const; // get header count
    size_t params() const;  // get arguments count
};

typedef std::function<bool(AsyncWebServerRequest * request)> ArRequestFilterFunction;

class AsyncWebHandler {
  protected:
    String _username;
    String _password;

  public:
    AsyncWebHandler()
        : _username("")
        , _password("") {
    }

    virtual ~AsyncWebHandler() {
    }
    virtual bool canHandle(AsyncWebServerRequest * request __attribute__((unused))) {
        return false;
    }
    virtual void handleRequest(AsyncWebServerRequest * request __attribute__((unused))) {
    }
    virtual void handleUpload(AsyncWebServerRequest * request __attribute__((unused)),
                              const String &          filename __attribute__((unused)),
                              size_t                  index __attribute__((unused)),
                              uint8_t *               data __attribute__((unused)),
                              size_t                  len __attribute__((unused)),
                              bool                    final __attribute__((unused))) {
    }
    virtual void handleBody(AsyncWebServerRequest * request __attribute__((unused)),
                            uint8_t *               data __attribute__((unused)),
                            size_t                  len __attribute__((unused)),
                            size_t                  index __attribute__((unused)),
                            size_t                  total __attribute__((unused))) {
    }

    virtual bool isRequestHandlerTrivial() {
        return true;
    }
};

class AsyncWebServerResponse {
  public:
    AsyncWebServerResponse();
    virtual ~AsyncWebServerResponse();
};

typedef std::function<void(AsyncWebServerRequest * request)> ArRequestHandlerFunction;
typedef std::function<void(AsyncWebServerRequest * request, const String & filename, size_t index, uint8_t * data, size_t len, bool final)> ArUploadHandlerFunction;
typedef std::function<void(AsyncWebServerRequest * request, uint8_t * data, size_t len, size_t index, size_t total)> ArBodyHandlerFunction;

class AsyncWebServer {
  protected:
    AsyncServer _server;

  public:
    AsyncWebServer(uint16_t port)
        : _server(port){};

    ~AsyncWebServer(){};

    void begin(){};
    void end();

    AsyncWebHandler & addHandler(AsyncWebHandler * handler) {
        return *handler;
    }

    void on(const char * uri, WebRequestMethodComposite method, ArRequestHandlerFunction onRequest){};
};


#endif
