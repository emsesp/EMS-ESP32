// =================================================================================================
// eModbus: Copyright 2020 by Michael Harwerth, Bert Melis and the contributors to eModbus
//               MIT license - see license.md for details
// =================================================================================================
#ifndef _MODBUS_CLIENT_TCP_H
#define _MODBUS_CLIENT_TCP_H

#include "options.h"

#if HAS_FREERTOS || IS_LINUX
#if HAS_FREERTOS
#include <Arduino.h>
#endif

#include "ModbusClient.h"
#include "Client.h"
#include <queue>
#include <vector>
using std::queue;

#define TARGETHOSTINTERVAL 10
#define DEFAULTTIMEOUT 2000

class ModbusClientTCP : public ModbusClient {
public:
  // Constructor takes reference to Client (EthernetClient or WiFiClient)
  explicit ModbusClientTCP(Client& client, uint16_t queueLimit = 100);

  // Alternative Constructor takes reference to Client (EthernetClient or WiFiClient) plus initial target host
  ModbusClientTCP(Client& client, IPAddress host, uint16_t port, uint16_t queueLimit = 100);

  // Destructor: clean up queue, task etc.
  ~ModbusClientTCP();

  // begin: start worker task
  void begin(int coreID = -1);

  // end: stop worker task
  void end();

  // Set default timeout value (and interval)
  void setTimeout(uint32_t timeout = DEFAULTTIMEOUT, uint32_t interval = TARGETHOSTINTERVAL);

  // Switch target host (if necessary)
  bool setTarget(IPAddress host, uint16_t port, uint32_t timeout = 0, uint32_t interval = 0);

  // Return number of unprocessed requests in queue
  uint32_t pendingRequests();

  // Remove all pending request from queue
  void clearQueue();

  // Set number of timeouts to tolerate before a connection is forcibly closed.
  // 0: never, 1..255: desired number
  // Returns previous value.
  uint8_t closeConnectionOnTimeouts(uint8_t n=3);

protected:
  // class describing a target server
  struct TargetHost {
    IPAddress     host;         // IP address
    uint16_t      port;         // Port number
    uint32_t      timeout;      // Time in ms waiting for a response
    uint32_t      interval;     // Time in ms to wait between requests
    
    inline TargetHost& operator=(const TargetHost& t) {
      host = t.host;
      port = t.port;
      timeout = t.timeout;
      interval = t.interval;
      return *this;
    }
    
    inline TargetHost(const TargetHost& t) :
      host(t.host),
      port(t.port),
      timeout(t.timeout),
      interval(t.interval) {}
    
    inline TargetHost() :
      host(IPAddress(0, 0, 0, 0)),
      port(0),
      timeout(0),
      interval(0)
    { }

    inline TargetHost(IPAddress host, uint16_t port, uint32_t timeout, uint32_t interval) :
      host(host),
      port(port),
      timeout(timeout),
      interval(interval)
    { }

    inline bool operator==(const TargetHost& t) {
      if (host != t.host) return false;
      if (port != t.port) return false;
      return true;
    }

    inline bool operator!=(const TargetHost& t) {
      if (host != t.host) return true;
      if (port != t.port) return true;
      return false;
    }
  };

  // class describing the TCP header of Modbus packets
  class ModbusTCPhead {
  public:
    ModbusTCPhead() :
    transactionID(0),
    protocolID(0),
    len(0) {}

    ModbusTCPhead(uint16_t tid, uint16_t pid, uint16_t _len) :
    transactionID(tid),
    protocolID(pid),
    len(_len) {}

    uint16_t transactionID;     // Caller-defined identification
    uint16_t protocolID;        // const 0x0000
    uint16_t len;               // Length of remainder of TCP packet

    inline explicit operator const uint8_t *() {
      uint8_t *cp = headRoom;
      *cp++ = (transactionID >> 8) & 0xFF;
      *cp++ = transactionID  & 0xFF;
      *cp++ = (protocolID >> 8) & 0xFF;
      *cp++ = protocolID  & 0xFF;
      *cp++ = (len >> 8) & 0xFF;
      *cp++ = len  & 0xFF;
      return headRoom;
    }

    inline ModbusTCPhead& operator= (ModbusTCPhead& t) {
      transactionID = t.transactionID;
      protocolID    = t.protocolID;
      len           = t.len;
      return *this;
    }

  protected:
    uint8_t headRoom[6] = {0,0,0,0,0,0};        // Buffer to hold MSB-first TCP header
  };

  struct RequestEntry {
    uint32_t token;
    ModbusMessage msg;
    TargetHost target;
    ModbusTCPhead head;
    bool isSyncRequest;
    RequestEntry(uint32_t t, const ModbusMessage& m, TargetHost tg, bool syncReq = false) :
      token(t),
      msg(m),
      target(tg),
      head(ModbusTCPhead()),
      isSyncRequest(syncReq) {}
  };

  // Base addRequest and syncRequest must be present
  Error addRequestM(ModbusMessage msg, uint32_t token) override;
  ModbusMessage syncRequestM(ModbusMessage msg, uint32_t token) override;
  // TCP-specific addition "...MT()" including adhoc target - used by bridge 
  Error addRequestMT(ModbusMessage msg, uint32_t token, IPAddress targetHost, uint16_t targetPort);
  ModbusMessage syncRequestMT(ModbusMessage msg, uint32_t token, IPAddress targetHost, uint16_t targetPort);

  // addToQueue: send freshly created request to queue
  bool addToQueue(uint32_t token, ModbusMessage request, TargetHost target, bool syncReq = false);

  // handleConnection: worker task method
  static void handleConnection(ModbusClientTCP *instance);
#if IS_LINUX
  static void *pHandle(void *p);
#endif

  // send: send request via Client connection
  void send(RequestEntry *request);

  // receive: get response via Client connection
  ModbusMessage receive(RequestEntry *request);

  queue<RequestEntry *> requests;   // Queue to hold requests to be processed
  #if USE_MUTEX
  mutex qLock;                    // Mutex to protect queue
  #endif
  Client& MT_client;              // Client reference for Internet connections (EthernetClient or WifiClient)
  TargetHost MT_lastTarget;       // last used server
  TargetHost MT_target;           // Description of target server
  uint32_t MT_defaultTimeout;     // Standard timeout value taken if no dedicated was set
  uint32_t MT_defaultInterval;    // Standard interval value taken if no dedicated was set
  uint16_t MT_qLimit;             // Maximum number of requests to accept in queue
  uint8_t MT_timeoutsToClose;     // 0: disregard, 1-255: number of timeouts to tolerate before
                                  //    forcibly closing a connection.

  // Let any ModbusBridge class use protected members
  template<typename SERVERCLASS> friend class ModbusBridge;
};

#endif  // HAS_FREERTOS

#endif  // INCLUDE GUARD
