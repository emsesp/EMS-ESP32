// PsychicJson.h
/*
  Async Response to use with ArduinoJson and AsyncWebServer
  Written by Andrew Melvin (SticilFace) with help from me-no-dev and BBlanchon.
  Ported to PsychicHttp by Zach Hoeken

*/
#ifndef PSYCHIC_JSON_H_
#define PSYCHIC_JSON_H_

#include <ArduinoJson.h>
#include "PsychicCore.h"
#include "PsychicResponse.h"
#include "ChunkPrinter.h"

#if ARDUINOJSON_VERSION_MAJOR == 5
#define ARDUINOJSON_5_COMPATIBILITY
#else
#ifndef DYNAMIC_JSON_DOCUMENT_SIZE
#define DYNAMIC_JSON_DOCUMENT_SIZE 4096
#endif
#endif

#ifndef JSON_BUFFER_SIZE
//#define JSON_BUFFER_SIZE 256
#define JSON_BUFFER_SIZE 4 * 1024
#endif

constexpr const char * JSON_MIMETYPE = "application/json";

/*
 * Json Response
 * */

class PsychicJsonResponse : public PsychicResponse {
  protected:
#ifdef ARDUINOJSON_5_COMPATIBILITY
    DynamicJsonBuffer _jsonBuffer;
#else
    DynamicJsonDocument _jsonBuffer;
#endif

    JsonVariant _root;
    size_t      _contentLength;
    bool        _msgPack; // added by proddy

  public:
#ifdef ARDUINOJSON_5_COMPATIBILITY
    PsychicJsonResponse(PsychicRequest * request, bool isArray = false)
        : PsychicResponse(request) {
        setContentType(JSON_MIMETYPE);
        if (isArray)
            _root = _jsonBuffer.createArray();
        else
            _root = _jsonBuffer.createObject();
    }
#else
    PsychicJsonResponse(PsychicRequest * request, bool isArray = false, size_t maxJsonBufferSize = DYNAMIC_JSON_DOCUMENT_SIZE, bool msgPack = false) // added by proddy
        : PsychicResponse(request)
        , _jsonBuffer(maxJsonBufferSize)
        , _msgPack(msgPack) // added by proddy
    {
        setContentType(JSON_MIMETYPE);
        if (isArray)
            _root = _jsonBuffer.createNestedArray();
        else
            _root = _jsonBuffer.createNestedObject();
    }
#endif

    ~PsychicJsonResponse() {
    }

    JsonVariant & getRoot() {
        return _root;
    }

    size_t getLength() {
#ifdef ARDUINOJSON_5_COMPATIBILITY
        return _root.measureLength();
#else
        // return measureJson(_root);
        return (_msgPack) ? measureMsgPack(_root) : measureJson(_root); // added by proddy
#endif
    }

    // size_t _fillBuffer(uint8_t *data, size_t len)
    // {
    //   //TODO: fix
    //   //ChunkPrint dest(data, _sentLength, len);

    //   #ifdef ARDUINOJSON_5_COMPATIBILITY
    //     _root.printTo(dest);
    //   #else
    //     //serializeJson(_root, dest);
    //   #endif
    //   return len;
    // }

    virtual esp_err_t send() override {
        esp_err_t err = ESP_OK;
        // size_t    length = getLength();
        size_t length = JSON_BUFFER_SIZE; // TODO force chunking

        size_t buffer_size;
        char * buffer;

        DUMP(length);

        //how big of a buffer do we want?
        if (length < JSON_BUFFER_SIZE)
            buffer_size = length + 1;
        else
            buffer_size = JSON_BUFFER_SIZE;

        DUMP(buffer_size);

        buffer = (char *)malloc(buffer_size);
        if (buffer == NULL) {
            httpd_resp_send_err(this->_request->request(), HTTPD_500_INTERNAL_SERVER_ERROR, "Unable to allocate memory.");
            return ESP_FAIL;
        }

        //send it in one shot or no?
        if (length < JSON_BUFFER_SIZE) {
            TRACE();

#ifdef ARDUINOJSON_5_COMPATIBILITY
            _root.printTo(buffer, buffer_size);
#else
            // serializeJson(_root, buffer, buffer_size)
            (_msgPack) ? serializeMsgPack(_root, buffer, buffer_size) : serializeJson(_root, buffer, buffer_size); // added by proddy
#endif

            this->setContent((uint8_t *)buffer, length);
            this->setContentType(JSON_MIMETYPE);

            err = PsychicResponse::send();
        } else {
            //helper class that acts as a stream to print chunked responses
            ChunkPrinter dest(this, (uint8_t *)buffer, buffer_size);

            //keep our headers
            this->sendHeaders();

//these commands write to the ChunkPrinter which does the sending
#ifdef ARDUINOJSON_5_COMPATIBILITY
            _root.printTo(dest);
#else
            // serializeJson(_root, dest);                                                                    // added by proddy
            (_msgPack) ? serializeMsgPack(_root, dest) : serializeJson(_root, dest); // added by proddy

#endif

            //send the last bits
            dest.flush();

            //done with our chunked response too
            err = this->finishChunking();
        }

        //let the buffer go
        free(buffer);

        return err;
    }
};

// class PrettyPsychicJsonResponse : public PsychicJsonResponse
// {
//   public:
//     #ifdef ARDUINOJSON_5_COMPATIBILITY
//       PrettyPsychicJsonResponse(PsychicRequest *request, bool isArray = false) : PsychicJsonResponse(request, isArray) {}
//     #else
//       PrettyPsychicJsonResponse(
//         PsychicRequest *request,
//         bool isArray = false,
//         size_t maxJsonBufferSize = DYNAMIC_JSON_DOCUMENT_SIZE)
//         : PsychicJsonResponse{request, isArray, maxJsonBufferSize} {}
//     #endif

//     size_t setLength()
//     {
//       #ifdef ARDUINOJSON_5_COMPATIBILITY
//         _contentLength = _root.measurePrettyLength();
//       #else
//         _contentLength = measureJsonPretty(_root);
//       #endif
//       return _contentLength;
//     }
// };

typedef std::function<esp_err_t(PsychicRequest * request, JsonVariant & json)> PsychicJsonRequestCallback;

class PsychicJsonHandler : public PsychicWebHandler {
  protected:
    PsychicJsonRequestCallback _onRequest;
#ifndef ARDUINOJSON_5_COMPATIBILITY
    const size_t _maxJsonBufferSize = DYNAMIC_JSON_DOCUMENT_SIZE;
#endif

  public:
#ifdef ARDUINOJSON_5_COMPATIBILITY
    PsychicJsonHandler()
        : _onRequest(NULL){};

    PsychicJsonHandler(PsychicJsonRequestCallback onRequest)
        : _onRequest(onRequest) {
    }
#else
    PsychicJsonHandler(size_t maxJsonBufferSize = DYNAMIC_JSON_DOCUMENT_SIZE)
        : _onRequest(NULL)
        , _maxJsonBufferSize(maxJsonBufferSize){};

    PsychicJsonHandler(PsychicJsonRequestCallback onRequest, size_t maxJsonBufferSize = DYNAMIC_JSON_DOCUMENT_SIZE)
        : _onRequest(onRequest)
        , _maxJsonBufferSize(maxJsonBufferSize) {
    }
#endif

    void onRequest(PsychicJsonRequestCallback fn) {
        _onRequest = fn;
    }

    virtual esp_err_t handleRequest(PsychicRequest * request) override {
        //process basic stuff
        PsychicWebHandler::handleRequest(request);

        if (_onRequest) {
#ifdef ARDUINOJSON_5_COMPATIBILITY
            DynamicJsonBuffer jsonBuffer;
            JsonVariant       json = jsonBuffer.parse();
            if (!json.success())
                return request->reply(400);
#else
            DynamicJsonDocument  jsonBuffer(this->_maxJsonBufferSize);
            DeserializationError error = deserializeJson(jsonBuffer, request->body());
            if (error)
                return request->reply(400);

            JsonVariant json = jsonBuffer.as<JsonVariant>();
#endif

            return _onRequest(request, json);
        } else
            return request->reply(500);
    }
};
#endif