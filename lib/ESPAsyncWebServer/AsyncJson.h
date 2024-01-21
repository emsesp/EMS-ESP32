// AsyncJson.h
/*
  Async Response to use with ArduinoJson and AsyncWebServer
  Written by Andrew Melvin (SticilFace) with help from me-no-dev and BBlanchon.
*/

#ifndef ASYNC_JSON_H_
#define ASYNC_JSON_H_
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <Print.h>

#define DYNAMIC_JSON_DOCUMENT_SIZE 1024

constexpr const char * JSON_MIMETYPE = "application/json";

/*
 * Json Response
 */

class ChunkPrint : public Print {
  private:
    uint8_t * _destination;
    size_t    _to_skip;
    size_t    _to_write;
    size_t    _pos;

  public:
    ChunkPrint(uint8_t * destination, size_t from, size_t len)
        : _destination(destination)
        , _to_skip(from)
        , _to_write(len)
        , _pos{0} {
    }
    virtual ~ChunkPrint() {
    }
    size_t write(uint8_t c) {
        if (_to_skip > 0) {
            _to_skip--;
            return 1;
        } else if (_to_write > 0) {
            _to_write--;
            _destination[_pos++] = c;
            return 1;
        }
        return 0;
    }
    size_t write(const uint8_t * buffer, size_t size) {
        return this->Print::write(buffer, size);
    }
};

// added by Proddy
class MsgpackAsyncJsonResponse : public AsyncAbstractResponse {
  protected:
    JsonDocument _jsonBuffer;
    JsonVariant  _root;
    bool         _isValid;

  public:
    MsgpackAsyncJsonResponse(bool isArray = false)
        : _isValid{false} {
        _code        = 200;
        _contentType = JSON_MIMETYPE;
        if (isArray)
            _root = _jsonBuffer.to<JsonArray>();
        else
            _root = _jsonBuffer.add<JsonObject>();
    }

    ~MsgpackAsyncJsonResponse() {
    }
    JsonVariant getRoot() {
        return _root;
    }
    bool _sourceValid() const {
        return _isValid;
    }
    size_t setLength() {
        _contentLength = measureMsgPack(_root);
        if (_contentLength) {
            _isValid = true;
        }
        return _contentLength;
    }

    size_t getSize() {
        return _jsonBuffer.size();
    }

    size_t _fillBuffer(uint8_t * data, size_t len) {
        ChunkPrint dest(data, _sentLength, len);
        serializeMsgPack(_root, dest);
        return len;
    }
};

class AsyncJsonResponse : public AsyncAbstractResponse {
  protected:
    JsonDocument _jsonBuffer;
    JsonVariant  _root;
    bool         _isValid;

  public:
    AsyncJsonResponse(bool isArray = false)
        : _isValid{false} {
        _code        = 200;
        _contentType = JSON_MIMETYPE;
        if (isArray)
            _root = _jsonBuffer.to<JsonArray>();
        else
            _root = _jsonBuffer.add<JsonObject>();
    }

    ~AsyncJsonResponse() {
    }
    JsonVariant getRoot() {
        return _root;
    }
    bool _sourceValid() const {
        return _isValid;
    }
    size_t setLength() {
        _contentLength = measureJson(_root);
        if (_contentLength) {
            _isValid = true;
        }
        return _contentLength;
    }

    size_t getSize() {
        return _jsonBuffer.size();
    }

    size_t _fillBuffer(uint8_t * data, size_t len) {
        ChunkPrint dest(data, _sentLength, len);
        serializeJson(_root, dest);
        return len;
    }
};

class PrettyAsyncJsonResponse : public AsyncJsonResponse {
  public:
    PrettyAsyncJsonResponse(bool isArray = false)
        : AsyncJsonResponse{isArray} {
    }
    size_t setLength() {
        _contentLength = measureJsonPretty(_root);
        if (_contentLength) {
            _isValid = true;
        }
        return _contentLength;
    }
    size_t _fillBuffer(uint8_t * data, size_t len) {
        ChunkPrint dest(data, _sentLength, len);
        serializeJsonPretty(_root, dest);
        return len;
    }
};

typedef std::function<void(AsyncWebServerRequest * request, JsonVariant json)> ArJsonRequestHandlerFunction;

class AsyncCallbackJsonWebHandler : public AsyncWebHandler {
  private:
  protected:
    const String                 _uri;
    WebRequestMethodComposite    _method;
    ArJsonRequestHandlerFunction _onRequest;
    size_t                       _contentLength;
#ifndef ARDUINOJSON_5_COMPATIBILITY
    size_t _maxJsonBufferSize;
#endif
    size_t _maxContentLength;

  public:
    AsyncCallbackJsonWebHandler(const String & uri, ArJsonRequestHandlerFunction onRequest)
        : _uri(uri)
        , _method(HTTP_POST | HTTP_PUT | HTTP_PATCH)
        , _onRequest(onRequest)
        , _maxContentLength(16384) {
    }

    void setMethod(WebRequestMethodComposite method) {
        _method = method;
    }
    void setMaxContentLength(int maxContentLength) {
        _maxContentLength = maxContentLength;
    }
    // void setMaxJsonBufferSize(size_t maxJsonBufferSize) {
    //     _maxJsonBufferSize = maxJsonBufferSize;
    // }
    void onRequest(ArJsonRequestHandlerFunction fn) {
        _onRequest = fn;
    }

    virtual bool canHandle(AsyncWebServerRequest * request) override final {
        if (!_onRequest)
            return false;

        if (!(_method & request->method()))
            return false;

        if (_uri.length() && (_uri != request->url() && !request->url().startsWith(_uri + "/")))
            return false;

        if (!request->contentType().equalsIgnoreCase(JSON_MIMETYPE))
            return false;

        request->addInterestingHeader("ANY");
        return true;
    }

    virtual void handleRequest(AsyncWebServerRequest * request) override final {
        if (_onRequest) {
            if (request->_tempObject != NULL) {
                JsonDocument         jsonBuffer;
                DeserializationError error = deserializeJson(jsonBuffer, (uint8_t *)(request->_tempObject));
                if (!error) {
                    JsonVariant json = jsonBuffer.as<JsonVariant>();
                    _onRequest(request, json);
                    return;
                }
            }
            request->send(_contentLength > _maxContentLength ? 413 : 400);
        } else {
            request->send(500);
        }
    }
    virtual void handleUpload(AsyncWebServerRequest * request, const String & filename, size_t index, uint8_t * data, size_t len, bool final) override final {
    }
    virtual void handleBody(AsyncWebServerRequest * request, uint8_t * data, size_t len, size_t index, size_t total) override final {
        if (_onRequest) {
            _contentLength = total;
            if (total > 0 && request->_tempObject == NULL && total < _maxContentLength) {
                request->_tempObject = malloc(total);
            }
            if (request->_tempObject != NULL) {
                memcpy((uint8_t *)(request->_tempObject) + index, data, len);
            }
        }
    }
    virtual bool isRequestHandlerTrivial() override final {
        return _onRequest ? false : true;
    }
};

#endif
