#include "PsychicWebHandler.h"

PsychicWebHandler::PsychicWebHandler() : 
  PsychicHandler(),
  _requestCallback(NULL),
  _onOpen(NULL),
  _onClose(NULL)
  {}
PsychicWebHandler::~PsychicWebHandler() {}

bool PsychicWebHandler::canHandle(PsychicRequest *request) {
  return true;
}

esp_err_t PsychicWebHandler::handleRequest(PsychicRequest *request)
{
  //lookup our client
  PsychicClient *client = checkForNewClient(request->client());
  if (client->isNew)
    openCallback(client);

  /* Request body cannot be larger than a limit */
  if (request->contentLength() > request->server()->maxRequestBodySize)
  {
    ESP_LOGE(PH_TAG, "Request body too large : %d bytes", request->contentLength());

    /* Respond with 400 Bad Request */
    char error[60];
    sprintf(error, "Request body must be less than %u bytes!", request->server()->maxRequestBodySize);
    httpd_resp_send_err(request->request(), HTTPD_400_BAD_REQUEST, error);

    /* Return failure to close underlying connection else the incoming file content will keep the socket busy */
    return ESP_FAIL;
  }

  //get our body loaded up.
  esp_err_t err = request->loadBody();
  if (err != ESP_OK)
    return err;

  //load our params in.
  request->loadParams();

  //okay, pass on to our callback.
  if (this->_requestCallback != NULL)
    err = this->_requestCallback(request);

  return err;
}

PsychicWebHandler * PsychicWebHandler::onRequest(PsychicHttpRequestCallback fn) {
  _requestCallback = fn;
  return this;
}

void PsychicWebHandler::openCallback(PsychicClient *client) {
  if (_onOpen != NULL)
    _onOpen(client);
}

void PsychicWebHandler::closeCallback(PsychicClient *client) {
  if (_onClose != NULL)
    _onClose(getClient(client));
}

PsychicWebHandler * PsychicWebHandler::onOpen(PsychicClientCallback fn) {
  _onOpen = fn;
  return this;
}

PsychicWebHandler * PsychicWebHandler::onClose(PsychicClientCallback fn) {
  _onClose = fn;
  return this;
}