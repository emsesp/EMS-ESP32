#include "PsychicResponse.h"
#include "PsychicRequest.h"
#include <http_status.h>

PsychicResponse::PsychicResponse(PsychicRequest *request) :
  _request(request),
  _code(200),
  _status(""),
  _contentLength(0),
  _body("")
{
}

PsychicResponse::~PsychicResponse()
{
  //clean up our header variables.  we have to do this since httpd_resp_send doesn't store copies
  for (HTTPHeader header : _headers)
  {
    free(header.field);
    free(header.value);
  }
  _headers.clear();
}

void PsychicResponse::addHeader(const char *field, const char *value)
{
  //these get freed during send()
  HTTPHeader header;
  header.field =(char *)malloc(strlen(field)+1);
  header.value = (char *)malloc(strlen(value)+1);

  strlcpy(header.field, field, strlen(field)+1);
  strlcpy(header.value, value, strlen(value)+1);

  _headers.push_back(header);
}

void PsychicResponse::setCookie(const char *name, const char *value, unsigned long secondsFromNow, const char *extras)
{
  time_t now = time(nullptr);

  String output;
  output = urlEncode(name) + "=" + urlEncode(value);

  //if current time isn't modern, default to using max age
  if (now < 1700000000)
    output += "; Max-Age=" + String(secondsFromNow);    
  //otherwise, set an expiration date
  else
  {
    time_t expirationTimestamp = now + secondsFromNow;

    // Convert the expiration timestamp to a formatted string for the "expires" attribute
    struct tm* tmInfo = gmtime(&expirationTimestamp);
    char expires[30];
    strftime(expires, sizeof(expires), "%a, %d %b %Y %H:%M:%S GMT", tmInfo);
    output += "; Expires=" + String(expires);
  }

  //did we get any extras?
  if (strlen(extras))
    output += "; " + String(extras);

  //okay, add it in.
  addHeader("Set-Cookie", output.c_str());
}

  // time_t now = time(nullptr);
  // // Set the cookie with the "expires" attribute

void PsychicResponse::setCode(int code)
{
  _code = code;
}

void PsychicResponse::setContentType(const char *contentType)
{
  httpd_resp_set_type(_request->request(), contentType);
}

void PsychicResponse::setContent(const char *content)
{
  _body = content;
  setContentLength(strlen(content));
}

void PsychicResponse::setContent(const uint8_t *content, size_t len)
{
  _body = (char *)content;
  setContentLength(len);
}

const char * PsychicResponse::getContent()
{
  return _body;
}

size_t PsychicResponse::getContentLength()
{
  return _contentLength;
}

esp_err_t PsychicResponse::send()
{
  //esp-idf makes you set the whole status.
  sprintf(_status, "%u %s", _code, http_status_reason(_code));
  httpd_resp_set_status(_request->request(), _status);

  //our headers too
  this->sendHeaders();

  //now send it off
  esp_err_t err = httpd_resp_send(_request->request(), getContent(), getContentLength());

  //did something happen?
  if (err != ESP_OK)
    ESP_LOGE(PH_TAG, "Send response failed (%s)", esp_err_to_name(err));

  return err;
}

void PsychicResponse::sendHeaders()
{
  //get our global headers out of the way first
  for (HTTPHeader header : DefaultHeaders::Instance().getHeaders())
    httpd_resp_set_hdr(_request->request(), header.field, header.value);

  //now do our individual headers
  for (HTTPHeader header : _headers)
    httpd_resp_set_hdr(this->_request->request(), header.field, header.value);
}

esp_err_t PsychicResponse::sendChunk(uint8_t *chunk, size_t chunksize)
{
  /* Send the buffer contents as HTTP response chunk */
  esp_err_t err = httpd_resp_send_chunk(this->_request->request(), (char *)chunk, chunksize);
  if (err != ESP_OK)
  {
    ESP_LOGE(PH_TAG, "File sending failed (%s)", esp_err_to_name(err));

    /* Abort sending file */
    httpd_resp_sendstr_chunk(this->_request->request(), NULL);

    /* Respond with 500 Internal Server Error */
    httpd_resp_send_err(this->_request->request(), HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to send file");
  }

  return err;
}

esp_err_t PsychicResponse::finishChunking()
{
  /* Respond with an empty chunk to signal HTTP response completion */
  return httpd_resp_send_chunk(this->_request->request(), NULL, 0);
}