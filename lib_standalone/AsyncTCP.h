#ifndef ASYNCTCP_H_
#define ASYNCTCP_H_

#include "Arduino.h"

#include <functional>

class AsyncClient;

struct tcp_pcb;
struct ip_addr;

class AsyncClient {
  public:
    AsyncClient(tcp_pcb * pcb = 0);
    ~AsyncClient();
};

class AsyncServer {
  public:
    AsyncServer(uint16_t port)
        : _port(port){};
    ~AsyncServer(){};

  protected:
    uint16_t _port;
};


#endif
