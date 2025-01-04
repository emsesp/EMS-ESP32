#ifndef _ESPAsyncWebServer_H_
#define _ESPAsyncWebServer_H_

#include "Arduino.h"
#include "AsyncTCP.h"

#include <functional>
#include <ArduinoJson.h>

class AsyncWebServer;
class AsyncWebServerRequest;
class AsyncWebServerResponse;
class AsyncJsonResponse;
class AsyncEventSource;

class AsyncWebParameter {
  private:
    String _name;
    String _value;
    size_t _size;
    bool   _isForm;
    bool   _isFile;

  public:
    AsyncWebParameter(const String & name, const String & value, bool form = false, bool file = false, size_t size = 0)
        : _name(name)
        , _value(value)
        , _size(size)
        , _isForm(form)
        , _isFile(file) {
    }
    const String & name() const {
        return _name;
    }
    const String & value() const {
        return _value;
    }
    size_t size() const {
        return _size;
    }
    bool isPost() const {
        return _isForm;
    }
    bool isFile() const {
        return _isFile;
    }
};

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

    String _url;

  public:
    void * _tempObject;

    AsyncWebServerRequest(AsyncWebServer *, AsyncClient *) {};
    AsyncWebServerRequest() {};
    ~AsyncWebServerRequest() {};

    AsyncClient * client() {
        return _client;
    }

    WebRequestMethodComposite method() const {
        return _method;
    }

    void method(WebRequestMethodComposite method_s) {
        _method = method_s;
    }

    void addInterestingHeader(const String & name) {};

    size_t args() const {
        return 0;
    }

    void send(AsyncWebServerResponse * response) {};

    void send(AsyncJsonResponse * response) {};

    void send(int code, const String & contentType = String(), const String & content = String()) {};
    void send(int code, const String & contentType, const __FlashStringHelper *) {};

    const String & url() const {
        return _url;
    }

    void url(const String & url_s) {
        _url = url_s;
    }

    bool hasParam(const String & name, bool post, bool file) const {
        return false;
    }

    bool hasParam(const char * name, bool post, bool file) const {
        return false;
    }

    bool hasParam(const char * name) const {
        return false;
    }

    bool hasParam(const __FlashStringHelper * data) const {
        return false;
    }

    bool hasParam(const __FlashStringHelper * data, bool post, bool file) const {
        return false;
    }

    AsyncWebParameter * getParam(const String & name, bool post, bool file) const {
        return nullptr;
    }

    AsyncWebParameter * getParam(const __FlashStringHelper * data, bool post, bool file) const {
        return nullptr;
    }

    AsyncWebParameter * getParam(const __FlashStringHelper * data) const {
        return nullptr;
    }

    AsyncWebParameter * getParam(const char * name) const {
        return nullptr;
    }

    AsyncWebParameter * getParam(size_t num) const {
        return nullptr;
    }

    AsyncWebServerResponse * beginResponse(int code, const String & contentType = String(), const String & content = String()) {
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

    AsyncWebHandler & setFilter(ArRequestFilterFunction fn) {
        return *this;
    }
};

class AsyncWebServerResponse {
  protected:
    int    _code;
    String _contentType;
    size_t _contentLength;
    bool   _sendContentLength;
    bool   _chunked;
    size_t _headLength;
    size_t _sentLength;
    size_t _ackedLength;
    size_t _writtenLength;

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
        : _server(port) {};

    ~AsyncWebServer() {};

    void begin() {};
    void end();

    AsyncWebHandler & addHandler(AsyncWebHandler * handler) {
        return *handler;
    }

    void on(const char * uri, WebRequestMethodComposite method, ArRequestHandlerFunction onRequest) {};
};


class AsyncEventSource : public AsyncWebHandler {
  public:
    AsyncEventSource(const String & url) {};
    ~AsyncEventSource() {};

    size_t count() const {
        return 1;
    }

    void send(const char * message, const char * event = NULL, uint32_t id = 0, uint32_t reconnect = 0) {};
};


#endif
