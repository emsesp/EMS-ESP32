#ifndef PsychicResponse_h
#define PsychicResponse_h

#include "PsychicCore.h"
#include "time.h"

class PsychicRequest;

class PsychicResponse
{
  protected:
    PsychicRequest *_request;

    int _code;
    char _status[60];
    std::list<HTTPHeader> _headers;
    int64_t _contentLength;
    const char * _body;

  public:
    PsychicResponse(PsychicRequest *request);
    virtual ~PsychicResponse();

    void setCode(int code);

    void setContentType(const char *contentType);
    void setContentLength(int64_t contentLength) { _contentLength = contentLength; }
    int64_t getContentLength(int64_t contentLength) { return _contentLength; }

    void addHeader(const char *field, const char *value);

    void setCookie(const char *key, const char *value, unsigned long max_age = 60*60*24*30, const char *extras = "");

    void setContent(const char *content);
    void setContent(const uint8_t *content, size_t len);

    const char * getContent();
    size_t getContentLength();

    virtual esp_err_t send();
    void sendHeaders();
    esp_err_t sendChunk(uint8_t *chunk, size_t chunksize);
    esp_err_t finishChunking();
};

#endif // PsychicResponse_h