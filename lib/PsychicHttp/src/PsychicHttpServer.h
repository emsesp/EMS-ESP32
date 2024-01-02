#ifndef PsychicHttpServer_h
#define PsychicHttpServer_h

#include "PsychicCore.h"
#include "PsychicClient.h"
#include "PsychicHandler.h"

class PsychicEndpoint;
class PsychicHandler;
class PsychicStaticFileHandler;

class PsychicHttpServer
{
  protected:
    bool _use_ssl = false;
    std::list<PsychicEndpoint*> _endpoints;
    std::list<PsychicHandler*> _handlers;
    std::list<PsychicClient*> _clients;

    PsychicClientCallback _onOpen;
    PsychicClientCallback _onClose;

    esp_err_t _start();
    virtual esp_err_t _startServer();

  public:
    PsychicHttpServer();
    ~PsychicHttpServer();

    //esp-idf specific stuff
    httpd_handle_t server;
    httpd_config_t config;

    //some limits on what we will accept
    unsigned long maxUploadSize;
    unsigned long maxRequestBodySize;

    PsychicEndpoint *defaultEndpoint;

    static void destroy(void *ctx);

    esp_err_t listen(uint16_t port);

    virtual void stop();

    PsychicHandler& addHandler(PsychicHandler* handler);
    void removeHandler(PsychicHandler* handler);

    void addClient(PsychicClient *client);
    void removeClient(PsychicClient *client);
    PsychicClient* getClient(int socket);
    PsychicClient* getClient(httpd_req_t *req);
    bool hasClient(int socket);
    int count() { return _clients.size(); };
    const std::list<PsychicClient*>& getClientList();

    PsychicEndpoint* on(const char* uri);
    PsychicEndpoint* on(const char* uri, http_method method);
    PsychicEndpoint* on(const char* uri, PsychicHandler *handler);
    PsychicEndpoint* on(const char* uri, http_method method, PsychicHandler *handler);
    PsychicEndpoint* on(const char* uri, PsychicHttpRequestCallback onRequest);
    PsychicEndpoint* on(const char* uri, http_method method, PsychicHttpRequestCallback onRequest);
    PsychicEndpoint* on(const char* uri, PsychicJsonRequestCallback onRequest);
    PsychicEndpoint* on(const char* uri, http_method method, PsychicJsonRequestCallback onRequest);

    static esp_err_t notFoundHandler(httpd_req_t *req, httpd_err_code_t err);
    static esp_err_t defaultNotFoundHandler(PsychicRequest *request);
    void onNotFound(PsychicHttpRequestCallback fn);

    void onOpen(PsychicClientCallback handler);
    void onClose(PsychicClientCallback handler);
    static esp_err_t openCallback(httpd_handle_t hd, int sockfd);
    static void closeCallback(httpd_handle_t hd, int sockfd);

    PsychicStaticFileHandler* serveStatic(const char* uri, fs::FS& fs, const char* path, const char* cache_control = NULL);
};

bool ON_STA_FILTER(PsychicRequest *request);
bool ON_AP_FILTER(PsychicRequest *request);

#endif // PsychicHttpServer_h