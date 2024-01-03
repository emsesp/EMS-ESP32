#ifndef PsychicRequest_h
#define PsychicRequest_h

#include "PsychicCore.h"
#include "PsychicHttpServer.h"
#include "PsychicClient.h"
#include "PsychicWebParameter.h"
#include "PsychicResponse.h"

typedef std::map<String, String> SessionData;

enum Disposition { NONE, INLINE, ATTACHMENT, FORM_DATA};

struct ContentDisposition {
  Disposition disposition;
  String filename;
  String name;
};

class PsychicRequest {
  friend PsychicHttpServer;

  protected:
    PsychicHttpServer *_server;
    httpd_req_t *_req;
    SessionData *_session;
    PsychicClient *_client;

    http_method _method;
    String _uri;
    String _query;
    String _body;

    std::list<PsychicWebParameter*> _params;

    void _addParams(const String& params);
    void _parseGETParams();
    void _parsePOSTParams();

    const String _extractParam(const String& authReq, const String& param, const char delimit);
    const String _getRandomHexString();

  public:
    PsychicRequest(PsychicHttpServer *server, httpd_req_t *req);
    virtual ~PsychicRequest();

    void *_tempObject;

    PsychicHttpServer * server();
    httpd_req_t * request();
    virtual PsychicClient * client();

    bool isMultipart();
    esp_err_t loadBody();

    const String header(const char *name);
    bool hasHeader(const char *name);

    static void freeSession(void *ctx);
    bool hasSessionKey(const String& key);
    const String getSessionKey(const String& key);
    void setSessionKey(const String& key, const String& value);

    bool hasCookie(const char * key);
    const String getCookie(const char * key);

    http_method method();       // returns the HTTP method used as enum value (eg. HTTP_GET)
    const String methodStr();   // returns the HTTP method used as a string (eg. "GET")
    const String path();        // returns the request path (eg /page?foo=bar returns "/page")
    const String& uri();        // returns the full request uri (eg /page?foo=bar)
    const String& query();      // returns the request query data (eg /page?foo=bar returns "foo=bar")
    const String host();        // returns the requested host (request to http://psychic.local/foo will return "psychic.local")
    const String contentType(); // returns the Content-Type header value
    size_t contentLength();     // returns the Content-Length header value
    const String& body();       // returns the body of the request
    const ContentDisposition getContentDisposition();

    const String& queryString() { return query(); }  //compatability function.  same as query()
    const String& url() { return uri(); }            //compatability function.  same as uri()

    void loadParams();
    PsychicWebParameter * addParam(PsychicWebParameter *param);
    PsychicWebParameter * addParam(const String &name, const String &value, bool decode = true);
    bool hasParam(const char *key);
    PsychicWebParameter * getParam(const char *name);

    const String getFilename();

    bool authenticate(const char * username, const char * password);
    esp_err_t requestAuthentication(HTTPAuthMethod mode, const char* realm, const char* authFailMsg);

    esp_err_t redirect(const char *url);
    esp_err_t reply(int code);
    esp_err_t reply(const char *content);
    esp_err_t reply(int code, const char *contentType, const char *content);
};

#endif // PsychicRequest_h