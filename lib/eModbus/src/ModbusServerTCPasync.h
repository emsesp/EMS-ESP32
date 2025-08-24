// =================================================================================================
// eModbus: Copyright 2020 by Michael Harwerth, Bert Melis and the contributors to eModbus
//               MIT license - see license.md for details
// =================================================================================================
#ifndef _MODBUS_SERVER_TCP_ASYNC_H
#define _MODBUS_SERVER_TCP_ASYNC_H

#include "options.h"

#include <list>
#include <queue>
#if USE_MUTEX
#include <mutex> // NOLINT
#endif
#include <vector>

#include <Arduino.h>  // for millis()

#if defined(ESP32)
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESPAsyncTCP.h>
#endif

#include "ModbusServer.h"

#if USE_MUTEX
using std::lock_guard;
#endif

class ModbusServerTCPasync : public ModbusServer {

 private:
  class mb_client {
   friend class ModbusServerTCPasync;
   
   public:
    mb_client(ModbusServerTCPasync* s, AsyncClient* c);
    ~mb_client();

   private:
    void onData(uint8_t* data, size_t len);
    void onPoll();
    void onDisconnect();
    void addResponseToOutbox(ModbusMessage* response);
    void handleOutbox();
    ModbusServerTCPasync* server;
    AsyncClient* client;
    uint32_t lastActiveTime;
    ModbusMessage* message;
    Modbus::Error error;
    std::queue<ModbusMessage*> outbox;
    #if USE_MUTEX
    std::mutex obLock;  // outbox protection
    #endif
  };


 public:
  // Constructor
  ModbusServerTCPasync();

  // Destructor: closes the connections
  ~ModbusServerTCPasync();

  // activeClients: return number of clients currently employed
  uint16_t activeClients();

  // start: create task with TCP server to accept requests
  bool start(uint16_t port, uint8_t maxClients, uint32_t timeout, int coreID = -1);

  // stop: drop all connections and kill server task
  bool stop();
 
  // isRunning: return true is server is running
  bool isRunning();

 protected:
  void onClientConnect(AsyncClient* client);
  void onClientDisconnect(mb_client* client);

  AsyncServer* server;
  std::list<mb_client*> clients;
  uint8_t maxNoClients;
  uint32_t idle_timeout;
  #if USE_MUTEX
  std::mutex cListLock;  // client list protection
  #endif
};

#endif
