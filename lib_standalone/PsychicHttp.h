/*
 * PsychicHttp.h
 *   Standalone hacky version
 */

#ifndef PsychicHttp_h
#define PsychicHttp_h

#include "Arduino.h"
#include <ArduinoJson.h>
#include <functional>
#include <map>
#include <list>

#define esp_err_t uint8_t

class PsychicHttpServer;
class PsychicHttpsServer;
class PsychicEndpoint;
class PsychicHandler;
class PsychicStaticFileHandler;
class PsychicClient;
class PsychicRequest;
class PsychicResponse;
class PsychicEventSource;
class PsychicEventSourceResponse;
class PsychicEventSourceClient;
class PsychicResponse;

typedef std::function<void(PsychicEventSourceClient * client)>                 PsychicEventSourceClientCallback;
typedef std::function<esp_err_t(PsychicRequest * request, JsonVariant & json)> PsychicJsonRequestCallback;
typedef std::function<bool(PsychicRequest * request)>                          PsychicRequestFilterFunction;
typedef std::function<void(PsychicClient * client)>                            PsychicClientCallback;
typedef std::function<esp_err_t(PsychicRequest * request)>                     PsychicHttpRequestCallback;
typedef std::function<esp_err_t(PsychicRequest * request, JsonVariant & json)> PsychicJsonRequestCallback; // added by proddy

/* Request Methods */
#define HTTP_METHOD_MAP(XX)                                                                                                                                    \
    XX(1, GET, GET)                                                                                                                                            \
    XX(3, POST, POST)

enum http_method {
#define XX(num, name, string) HTTP_##name = num,
    HTTP_METHOD_MAP(XX)
#undef XX
};

constexpr const char * JSON_MIMETYPE = "application/json";

enum HTTPAuthMethod { BASIC_AUTH, DIGEST_AUTH };

typedef void * httpd_handle_t;

typedef struct httpd_req {
    httpd_handle_t handle;      /*!< Handle to server instance */
    int            method;      /*!< The type of HTTP request, -1 if unsupported method */
    const char     uri[513];    /*!< The URI of this request (1 byte extra for null termination) */
    size_t         content_len; /*!< Length of the request body */
    void *         aux;         /*!< Internally used members */

} httpd_req_t;

typedef std::map<String, String> SessionData;
enum Disposition { NONE, INLINE, ATTACHMENT, FORM_DATA };

struct ContentDisposition {
    Disposition disposition;
    String      filename;
    String      name;
};

class PsychicWebParameter {
  private:
    String _name;
    String _value;
    size_t _size;
    bool   _isForm;
    bool   _isFile;

  public:
    PsychicWebParameter(const String & name, const String & value, bool form = false, bool file = false, size_t size = 0)
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

class PsychicRequest {
  protected:
    PsychicHttpServer * _server;
    httpd_req_t *       _req;
    SessionData *       _session;
    PsychicClient *     _client;

    http_method _method;
    String      _uri;
    String      _query;
    String      _body;

    std::list<PsychicWebParameter *> _params;

    void _addParams(const String & params);
    void _parseGETParams();
    void _parsePOSTParams();

    const String _extractParam(const String & authReq, const String & param, const char delimit);
    const String _getRandomHexString();

  public:
    PsychicRequest(PsychicHttpServer * server, httpd_req_t * req);
    virtual ~PsychicRequest();

    void * _tempObject;

    PsychicHttpServer *     server();
    httpd_req_t *           request();
    virtual PsychicClient * client();

    bool      isMultipart();
    esp_err_t loadBody();

    const String header(const char * name);
    bool         hasHeader(const char * name);

    static void  freeSession(void * ctx);
    bool         hasSessionKey(const String & key);
    const String getSessionKey(const String & key);
    void         setSessionKey(const String & key, const String & value);

    bool         hasCookie(const char * key);
    const String getCookie(const char * key);

    http_method method() {
        return HTTP_GET;
    }                                         // returns the HTTP method used as enum value (eg. HTTP_GET)
    const String             methodStr();     // returns the HTTP method used as a string (eg. "GET")
    const String             path();          // returns the request path (eg /page?foo=bar returns "/page")
    const String &           uri();           // returns the full request uri (eg /page?foo=bar)
    const String &           query();         // returns the request query data (eg /page?foo=bar returns "foo=bar")
    const String             host();          // returns the requested host (request to http://psychic.local/foo will return "psychic.local")
    const String             contentType();   // returns the Content-Type header value
    size_t                   contentLength(); // returns the Content-Length header value
    const String &           body();          // returns the body of the request
    const ContentDisposition getContentDisposition();

    const String & queryString() {
        return query();
    } //compatability function.  same as query()
    const String & url() {
        return uri();
    } //compatability function.  same as uri()

    void                  loadParams();
    PsychicWebParameter * addParam(PsychicWebParameter * param);
    PsychicWebParameter * addParam(const String & name, const String & value, bool decode = true);
    bool                  hasParam(const char * key);
    PsychicWebParameter * getParam(const char * name);

    const String getFilename();

    bool      authenticate(const char * username, const char * password);
    esp_err_t requestAuthentication(HTTPAuthMethod mode, const char * realm, const char * authFailMsg);

    esp_err_t redirect(const char * url);
    esp_err_t reply(int code);
    esp_err_t reply(const char * content);
    esp_err_t reply(int code, const char * contentType, const char * content);
};

class PsychicHandler {
  protected:
    PsychicRequestFilterFunction _filter;
    PsychicHttpServer *          _server;

    String         _username;
    String         _password;
    HTTPAuthMethod _method;
    String         _realm;
    String         _authFailMsg;

    std::list<PsychicClient *> _clients;

  public:
    PsychicHandler();
    ~PsychicHandler();

    PsychicHandler * setFilter(PsychicRequestFilterFunction fn);
    bool             filter(PsychicRequest * request);

    PsychicHandler *
    setAuthentication(const char * username, const char * password, HTTPAuthMethod method = BASIC_AUTH, const char * realm = "", const char * authFailMsg = "");
    bool      needsAuthentication(PsychicRequest * request);
    esp_err_t authenticate(PsychicRequest * request);

    virtual bool isWebSocket() {
        return false;
    };

    PsychicClient * checkForNewClient(PsychicClient * client);
    void            checkForClosedClient(PsychicClient * client);

    virtual void            addClient(PsychicClient * client);
    virtual void            removeClient(PsychicClient * client);
    virtual PsychicClient * getClient(int socket);
    virtual PsychicClient * getClient(PsychicClient * client);
    virtual void            openCallback(PsychicClient * client){};
    virtual void            closeCallback(PsychicClient * client){};

    bool hasClient(PsychicClient * client);
    int  count() {
        return _clients.size();
    };
    const std::list<PsychicClient *> & getClientList();

    virtual bool canHandle(PsychicRequest * request) {
        return true;
    };
    virtual esp_err_t handleRequest(PsychicRequest * request) = 0;
};

class PsychicEndpoint {
    friend PsychicHttpServer;

  private:
    PsychicHttpServer * _server;
    String              _uri;
    http_method         _method;
    PsychicHandler *    _handler;

  public:
    PsychicEndpoint();
    PsychicEndpoint(PsychicHttpServer * server, http_method method, const char * uri);

    PsychicEndpoint * setHandler(PsychicHandler * handler);
    PsychicHandler *  handler();

    PsychicEndpoint * setFilter(PsychicRequestFilterFunction fn);
    PsychicEndpoint *
    setAuthentication(const char * username, const char * password, HTTPAuthMethod method = BASIC_AUTH, const char * realm = "", const char * authFailMsg = "");

    String uri();

    static esp_err_t requestCallback(httpd_req_t * req);
};

typedef struct httpd_config {
    uint16_t max_uri_handlers; /*!< Maximum allowed uri handlers */
} httpd_config_t;

httpd_config_t config;

class PsychicHttpServer {
  public:
    PsychicHttpServer();
    ~PsychicHttpServer();

    httpd_handle_t server;
    httpd_config_t config;

    unsigned long maxUploadSize;
    unsigned long maxRequestBodySize;

    PsychicEndpoint * defaultEndpoint;
    PsychicHandler &  addHandler(PsychicHandler * handler);

    esp_err_t listen(uint16_t port);

    PsychicEndpoint * on(const char * uri);
    PsychicEndpoint * on(const char * uri, http_method method);
    PsychicEndpoint * on(const char * uri, PsychicHttpRequestCallback onRequest);
    PsychicEndpoint * on(const char * uri, http_method method, PsychicHttpRequestCallback onRequest);
    PsychicEndpoint * on(const char * uri, PsychicHandler * handler);
    PsychicEndpoint * on(const char * uri, http_method method, PsychicHandler * handler);
    PsychicEndpoint * on(const char * uri, http_method method, PsychicJsonRequestCallback onRequest); // added proddy

    void onNotFound(PsychicHttpRequestCallback fn);

    void             onOpen(PsychicClientCallback handler);
    void             onClose(PsychicClientCallback handler);
    static esp_err_t openCallback(httpd_handle_t hd, int sockfd);
    static void      closeCallback(httpd_handle_t hd, int sockfd);
};

class PsychicHttpsServer : public PsychicHttpServer {
  public:
    PsychicHttpsServer();
    ~PsychicHttpsServer();

    uint8_t listen(uint16_t port, const char * cert, const char * private_key);
};

struct HTTPHeader {
    char * field;
    char * value;
};

class DefaultHeaders {
    std::list<HTTPHeader> _headers;

  public:
    DefaultHeaders() {
    }

    void addHeader(const String & field, const String & value) {
        addHeader(field.c_str(), value.c_str());
    }

    void addHeader(const char * field, const char * value) {
        HTTPHeader header;
        _headers.push_back(header);
    }

    const std::list<HTTPHeader> & getHeaders() {
        return _headers;
    }

    DefaultHeaders(DefaultHeaders const &)             = delete;
    DefaultHeaders & operator=(DefaultHeaders const &) = delete;

    static DefaultHeaders & Instance() {
        static DefaultHeaders instance;
        return instance;
    }
};

class PsychicWebHandler : public PsychicHandler {
  protected:
    PsychicHttpRequestCallback _requestCallback;

  public:
    PsychicWebHandler();
    ~PsychicWebHandler();

    virtual bool        canHandle(PsychicRequest * request) override;
    virtual esp_err_t   handleRequest(PsychicRequest * request) override;
    PsychicWebHandler * onRequest(PsychicHttpRequestCallback fn);
};

class PsychicResponse {
  protected:
    PsychicRequest * _request;

    int                   _code;
    char                  _status[60];
    std::list<HTTPHeader> _headers;
    int64_t               _contentLength;
    const char *          _body;

  public:
    PsychicResponse(PsychicRequest * request);
    virtual ~PsychicResponse();

    void setCode(int code);

    void setContentType(const char * contentType);
    void setContentLength(int64_t contentLength) {
        _contentLength = contentLength;
    }
    int64_t getContentLength(int64_t contentLength) {
        return _contentLength;
    }

    void         addHeader(const char * field, const char * value);
    void         setCookie(const char * key, const char * value, unsigned long max_age = 60 * 60 * 24 * 30, const char * extras = "");
    void         setContent(const char * content);
    void         setContent(const uint8_t * content, size_t len);
    const char * getContent();
    size_t       getContentLength();
    esp_err_t    send();
    void         sendHeaders();
    esp_err_t    sendChunk(uint8_t * chunk, size_t chunksize);
    esp_err_t    finishChunking();
};

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
    PsychicJsonResponse(PsychicRequest * request, bool isArray = false, size_t maxJsonBufferSize = 4096, bool msgPack = false) // added by proddy
        : PsychicResponse(request)
        , _jsonBuffer(maxJsonBufferSize)
        , _msgPack(msgPack) // added by proddy
    {
    }

    ~PsychicJsonResponse() {
    }

    JsonVariant & getRoot() {
        return _root;
    }

    size_t getLength() {
        // return measureJson(_root);
        return (_msgPack) ? measureMsgPack(_root) : measureJson(_root); // added by proddy
    }

    esp_err_t send() {
        return 0;
    }
};

class PsychicClient {
  protected:
    httpd_handle_t _server;
    int            _socket;

  public:
    PsychicClient(httpd_handle_t server, int socket);
    ~PsychicClient();

    //no idea if this is the right way to do it or not, but lets see.
    //pointer to our derived class (eg. PsychicWebSocketConnection)
    void * _friend;

    bool isNew = false;

    bool operator==(PsychicClient & rhs) const {
        return _socket == rhs.socket();
    }

    httpd_handle_t server();
    int            socket();
    esp_err_t      close();

    IPAddress localIP();
    IPAddress remoteIP();
};

class PsychicEventSourceClient : public PsychicClient {
  protected:
    uint32_t _lastId;

  public:
    PsychicEventSourceClient(PsychicClient * client);
    ~PsychicEventSourceClient();

    uint32_t lastId() const {
        return _lastId;
    }
    void send(const char * message, const char * event = NULL, uint32_t id = 0, uint32_t reconnect = 0);
    void sendEvent(const char * event);
};

class PsychicEventSource : public PsychicHandler {
  private:
    PsychicEventSourceClientCallback _onOpen;
    PsychicEventSourceClientCallback _onClose;

  public:
    PsychicEventSource();
    ~PsychicEventSource();

    PsychicEventSourceClient * getClient(int socket) override;
    PsychicEventSourceClient * getClient(PsychicClient * client) override;
    void                       addClient(PsychicClient * client) override;
    void                       removeClient(PsychicClient * client) override;
    void                       openCallback(PsychicClient * client) override;
    void                       closeCallback(PsychicClient * client) override;

    PsychicEventSource * onOpen(PsychicEventSourceClientCallback fn);
    PsychicEventSource * onClose(PsychicEventSourceClientCallback fn);

    esp_err_t handleRequest(PsychicRequest * request) override final;

    void send(const char * message, const char * event = NULL, uint32_t id = 0, uint32_t reconnect = 0);
};

class PsychicEventSourceResponse : public PsychicResponse {
  public:
    PsychicEventSourceResponse(PsychicRequest * request);
    esp_err_t send();
};

String generateEventMessage(const char * message, const char * event, uint32_t id, uint32_t reconnect);



#endif