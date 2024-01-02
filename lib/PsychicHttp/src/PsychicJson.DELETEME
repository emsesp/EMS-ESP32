#include "PsychicJson.h"

#ifdef ARDUINOJSON_5_COMPATIBILITY
  PsychicJsonResponse::PsychicJsonResponse(PsychicRequest *request, bool isArray) : PsychicResponse(request)
  {
    setContentType(JSON_MIMETYPE);
    if (isArray)
      _root = _jsonBuffer.createArray();
    else
      _root = _jsonBuffer.createObject();
  }
#else
  PsychicJsonResponse::PsychicJsonResponse(PsychicRequest *request, bool isArray, size_t maxJsonBufferSize) :
    PsychicResponse(request),
    _jsonBuffer(maxJsonBufferSize)
  {
    setContentType(JSON_MIMETYPE);
    if (isArray)
      _root = _jsonBuffer.createNestedArray();
    else
      _root = _jsonBuffer.createNestedObject();
  }
#endif

JsonVariant &PsychicJsonResponse::getRoot() { return _root; }

size_t PsychicJsonResponse::getLength()
{
  #ifdef ARDUINOJSON_5_COMPATIBILITY
    return _root.measureLength();
  #else
    return measureJson(_root);
  #endif
}

esp_err_t PsychicJsonResponse::send()
{
  esp_err_t err = ESP_OK;
  size_t length = getLength();
  size_t buffer_size;
  char *buffer;

  DUMP(length);

  //how big of a buffer do we want?
  if (length < JSON_BUFFER_SIZE)
    buffer_size = length+1;
  else
    buffer_size = JSON_BUFFER_SIZE;

  DUMP(buffer_size);

  buffer = (char *)malloc(buffer_size);
  if (buffer == NULL) {
    httpd_resp_send_err(this->_request->request(), HTTPD_500_INTERNAL_SERVER_ERROR, "Unable to allocate memory.");
    return ESP_FAIL;
  }

  //send it in one shot or no?
  if (length < JSON_BUFFER_SIZE)
  {
    TRACE();

    #ifdef ARDUINOJSON_5_COMPATIBILITY
      _root.printTo(buffer, buffer_size);
    #else
      serializeJson(_root, buffer, buffer_size);
    #endif

    this->setContent((uint8_t *)buffer, length);
    this->setContentType(JSON_MIMETYPE);

    err = PsychicResponse::send();
  }
  else
  {
    //helper class that acts as a stream to print chunked responses
    ChunkPrinter dest(this, (uint8_t *)buffer, buffer_size);

    //keep our headers
    this->sendHeaders();

    //these commands write to the ChunkPrinter which does the sending
    #ifdef ARDUINOJSON_5_COMPATIBILITY
      _root.printTo(dest);
    #else
      serializeJson(_root, dest);
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

#ifdef ARDUINOJSON_5_COMPATIBILITY
  PsychicJsonHandler::PsychicJsonHandler() :
    _onRequest(NULL)
  {};

  PsychicJsonHandler::PsychicJsonHandler(PsychicJsonRequestCallback onRequest) :
    _onRequest(onRequest)
  {}
#else
  PsychicJsonHandler::PsychicJsonHandler(size_t maxJsonBufferSize) :
    _onRequest(NULL),
    _maxJsonBufferSize(maxJsonBufferSize)
  {};

  PsychicJsonHandler::PsychicJsonHandler(PsychicJsonRequestCallback onRequest, size_t maxJsonBufferSize) :
    _onRequest(onRequest),
    _maxJsonBufferSize(maxJsonBufferSize)
  {}
#endif

void PsychicJsonHandler::onRequest(PsychicJsonRequestCallback fn) { _onRequest = fn; }

esp_err_t PsychicJsonHandler::handleRequest(PsychicRequest *request)
{
  //process basic stuff
  PsychicWebHandler::handleRequest(request);

  if (_onRequest)
  {
    #ifdef ARDUINOJSON_5_COMPATIBILITY
      DynamicJsonBuffer jsonBuffer;
      JsonVariant json = jsonBuffer.parse();
      if (!json.success())
        return request->reply(400);
    #else
      DynamicJsonDocument jsonBuffer(this->_maxJsonBufferSize);
      DeserializationError error = deserializeJson(jsonBuffer, request->body());
      if (error)
        return request->reply(400);

      JsonVariant json = jsonBuffer.as<JsonVariant>();
    #endif

    return _onRequest(request, json);
  }
  else
    return request->reply(500);
}