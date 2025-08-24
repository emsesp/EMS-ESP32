// =================================================================================================
// eModbus: Copyright 2020 by Michael Harwerth, Bert Melis and the contributors to eModbus
//               MIT license - see license.md for details
// =================================================================================================
#ifndef _MODBUS_CLIENT_TCP_ASYNC_H
#define _MODBUS_CLIENT_TCP_ASYNC_H
#include <Arduino.h>
#if defined ESP32
#include <AsyncTCP.h>
#elif defined ESP8266
#include <ESPAsyncTCP.h>
#endif
#include "options.h"
#include "ModbusMessage.h"
#include "ModbusClient.h"
#include <list>
#include <map>
#include <vector>
#if USE_MUTEX
#include <mutex>      // NOLINT
#endif

using std::vector;

#define DEFAULTTIMEOUT 10000
#define DEFAULTIDLETIME 60000

class ModbusClientTCPasync : public ModbusClient {
public:
  // Constructor takes address and port
  explicit ModbusClientTCPasync(IPAddress address, uint16_t port = 502, uint16_t queueLimit = 100);

  // Destructor: clean up queue, task etc.
  ~ModbusClientTCPasync();

  // optionally manually connect to modbus server. Otherwise connection will be made upon first request
  void connect();
  // Connect to another Modbus server
  void connect(IPAddress host, uint16_t port = 502);

  // manually disconnect from modbus server. Connection will also auto close after idle time
  void disconnect(bool force = false);

  // Set timeout value
  void setTimeout(uint32_t timeout);

  // Set idle timeout value (time before connection auto closes after being idle)
  void setIdleTimeout(uint32_t timeout);

  // Set maximum amount of messages awaiting a response. Subsequent messages will be queued.
  void setMaxInflightRequests(uint32_t maxInflightRequests);

// Remove all pending request from queue
  void clearQueue();

protected:

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

    inline ModbusTCPhead& operator= (const ModbusTCPhead& t) {
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
    ModbusTCPhead head;
    uint32_t sentTime;
    bool isSyncRequest;
    RequestEntry(uint32_t t, const ModbusMessage& m, bool syncReq = false) :
      token(t),
      msg(m),
      head(ModbusTCPhead()),
      sentTime(0),
      isSyncRequest(syncReq) {}
  };

  // Base addRequest and syncRequest both must be present
  Error addRequestM(ModbusMessage msg, uint32_t token) override;
  ModbusMessage syncRequestM(ModbusMessage msg, uint32_t token) override;

  // addToQueue: send freshly created request to queue
  bool addToQueue(int32_t token, ModbusMessage request, bool syncReq = false);

  // send: send request via Client connection
  bool send(RequestEntry *request);

  // receive: get response via Client connection
  // TCPResponse* receive(uint8_t* data, size_t length);

  // TCP handling code, all static taking a class instancs as param
  void onConnected();
  void onDisconnected();
  void onACError(AsyncClient* c, int8_t error);
  // void onTimeout(uint32_t time);
  // void onAck(size_t len, uint32_t time);
  void onPacket(uint8_t* data, size_t length);
  void onPoll();
  void handleSendingQueue();

  std::list<RequestEntry*> txQueue;           // Queue to hold requests to be sent
  std::map<uint16_t, RequestEntry*> rxQueue;  // Queue to hold requests to be processed
  #if USE_MUTEX
  std::mutex sLock;                         // Mutex to protect state
  std::mutex qLock;                         // Mutex to protect queues
  #endif

  AsyncClient MTA_client;           // Async TCP client
  uint32_t MTA_timeout;             // Standard timeout value taken
  uint32_t MTA_idleTimeout;         // Standard timeout value taken
  uint16_t MTA_qLimit;              // Maximum number of requests to accept in queue
  uint32_t MTA_maxInflightRequests; // Maximum number of inflight requests
  uint32_t MTA_lastActivity;        // Last time there was activity (disabled when queues are not empty)
  enum {
    DISCONNECTED,
    CONNECTING,
    CONNECTED
  } MTA_state;                      // TCP connection state
  IPAddress MTA_host;
  uint16_t MTA_port;
};

#endif
