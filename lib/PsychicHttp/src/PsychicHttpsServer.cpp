#include "PsychicHttpsServer.h"

PsychicHttpsServer::PsychicHttpsServer() : PsychicHttpServer()
{
  //for a SSL server
  ssl_config = HTTPD_SSL_CONFIG_DEFAULT();
  ssl_config.httpd.open_fn = PsychicHttpServer::openCallback;
  ssl_config.httpd.close_fn = PsychicHttpServer::closeCallback;
  ssl_config.httpd.uri_match_fn = httpd_uri_match_wildcard;
  ssl_config.httpd.global_user_ctx = this;
  ssl_config.httpd.global_user_ctx_free_fn = destroy;
  ssl_config.httpd.max_uri_handlers = 20;

  // each SSL connection takes about 45kb of heap
  // a barebones sketch with PsychicHttp has ~150kb of heap available
  // if we set it higher than 2 and use all the connections, we get lots of memory errors.
  // not to mention there is no heap left over for the program itself.
  ssl_config.httpd.max_open_sockets = 2;
}

PsychicHttpsServer::~PsychicHttpsServer() {}

esp_err_t PsychicHttpsServer::listen(uint16_t port, const char *cert, const char *private_key)
{
  this->_use_ssl = true;

  this->ssl_config.port_secure = port;
  this->ssl_config.cacert_pem = (uint8_t *)cert;
  this->ssl_config.cacert_len = strlen(cert)+1;
  this->ssl_config.prvtkey_pem = (uint8_t *)private_key;
  this->ssl_config.prvtkey_len = strlen(private_key)+1;

  return this->_start();
}

esp_err_t PsychicHttpsServer::_startServer()
{
  if (this->_use_ssl)
    return httpd_ssl_start(&this->server, &this->ssl_config);
  else
    return httpd_start(&this->server, &this->config);
}

void PsychicHttpsServer::stop()
{
  if (this->_use_ssl)
    httpd_ssl_stop(this->server);
  else
    httpd_stop(this->server);
}