// =================================================================================================
// eModbus: Copyright 2020 by Michael Harwerth, Bert Melis and the contributors to eModbus
//               MIT license - see license.md for details
// =================================================================================================
#include "ModbusClientTCPasync.h"
#define LOCAL_LOG_LEVEL LOG_LEVEL_VERBOSE
// #undef LOCAL_LOG_LEVEL
#include "Logging.h"

ModbusClientTCPasync::ModbusClientTCPasync(IPAddress address, uint16_t port, uint16_t queueLimit) :
  ModbusClient(),
  txQueue(),
  rxQueue(),
  MTA_client(),
  MTA_timeout(DEFAULTTIMEOUT),
  MTA_idleTimeout(DEFAULTIDLETIME),
  MTA_qLimit(queueLimit),
  MTA_maxInflightRequests(queueLimit),
  MTA_lastActivity(0),
  MTA_state(DISCONNECTED),
  MTA_host(address),
  MTA_port(port)
    {
      // attach all handlers on async tcp events
      MTA_client.onConnect([](void* i, AsyncClient* c) { (static_cast<ModbusClientTCPasync*>(i))->onConnected(); }, this);
      MTA_client.onDisconnect([](void* i, AsyncClient* c) { (static_cast<ModbusClientTCPasync*>(i))->onDisconnected(); }, this);
      MTA_client.onError([](void* i, AsyncClient* c, int8_t error) { (static_cast<ModbusClientTCPasync*>(i))->onACError(c, error); }, this);
      // MTA_client.onTimeout([](void* i, AsyncClient* c, uint32_t time) { (static_cast<ModbusClientTCPasync*>(i))->onTimeout(time); }, this);
      // MTA_client.onAck([](void* i, AsyncClient* c, size_t len, uint32_t time) { (static_cast<ModbusClientTCPasync*>(i))->onAck(len, time); }, this);
      MTA_client.onData([](void* i, AsyncClient* c, void* data, size_t len) { (static_cast<ModbusClientTCPasync*>(i))->onPacket(static_cast<uint8_t*>(data), len); }, this);
      MTA_client.onPoll([](void* i, AsyncClient* c) { (static_cast<ModbusClientTCPasync*>(i))->onPoll(); }, this);

      // disable nagle algorithm ref Modbus spec
      MTA_client.setNoDelay(true);
    }

// Destructor: clean up queue, task etc.
ModbusClientTCPasync::~ModbusClientTCPasync() {
  // Clean up queue
  {
    // Safely lock access
    LOCK_GUARD(lock1, qLock);
    LOCK_GUARD(lock2, sLock);
    // Delete all elements from queues
    while (!txQueue.empty()) {
      delete txQueue.front();
      txQueue.pop_front();
    }
    for (auto it = rxQueue.cbegin(); it != rxQueue.cend();/* no increment */) {
      delete it->second;
      it = rxQueue.erase(it);
    }
  }
  // force close client
  MTA_client.close(true);
}

// optionally manually connect to modbus server. Otherwise connection will be made upon first request
void ModbusClientTCPasync::connect() {
  LOG_D("connecting\n");
  LOCK_GUARD(lock1, sLock);
  // only connect if disconnected
  if (MTA_state == DISCONNECTED) {
    MTA_state = CONNECTING;
    MTA_client.connect(MTA_host, MTA_port);
  }
}

// connect to another modbus server.
void ModbusClientTCPasync::connect(IPAddress host, uint16_t port) {
  // First disconnect, if connected
  disconnect(true);
  // Set new host and port
  MTA_host = host;
  MTA_port = port;
  connect();
}

// manually disconnect from modbus server. Connection will also auto close after idle time
void ModbusClientTCPasync::disconnect(bool force) {
  LOG_D("disconnecting\n");
  MTA_client.close(force);
}

// Set timeout value
void ModbusClientTCPasync::setTimeout(uint32_t timeout) {
  MTA_timeout = timeout;
}

// Set idle timeout value (time before connection auto closes after being idle)
void ModbusClientTCPasync::setIdleTimeout(uint32_t timeout) {
  MTA_idleTimeout = timeout;
}

void ModbusClientTCPasync::setMaxInflightRequests(uint32_t maxInflightRequests) {
  MTA_maxInflightRequests = maxInflightRequests;
}

// Remove all pending request from queue
void ModbusClientTCPasync::clearQueue()
{
  LOCK_GUARD(lock1, qLock);
  LOCK_GUARD(lock2, sLock);
  // Delete all elements from queues
  while (!txQueue.empty()) {
    delete txQueue.front();
    txQueue.pop_front();
  }
}

// Base addRequest for preformatted ModbusMessage and last set target
Error ModbusClientTCPasync::addRequestM(ModbusMessage msg, uint32_t token) {
  Error rc = SUCCESS;        // Return value

  // Add it to the queue, if valid
  if (msg) {
    // Queue add successful?
    if (!addToQueue(token, msg)) {
      // No. Return error after deleting the allocated request.
      rc = REQUEST_QUEUE_FULL;
    }
  }

  LOG_D("Add TCP request result: %02X\n", rc);
  return rc;
}

// Base syncRequest follows the same pattern
ModbusMessage ModbusClientTCPasync::syncRequestM(ModbusMessage msg, uint32_t token) {
  ModbusMessage response;

  if (msg) {
    // Queue add successful?
    if (!addToQueue(token, msg, true)) {
      // No. Return error after deleting the allocated request.
      response.setError(msg.getServerID(), msg.getFunctionCode(), REQUEST_QUEUE_FULL);
    } else {
      // Request is queued - wait for the result.
      response = waitSync(msg.getServerID(), msg.getFunctionCode(), token);
    }
  } else {
    response.setError(msg.getServerID(), msg.getFunctionCode(), EMPTY_MESSAGE);
  }
  return response;
}

// addToQueue: send freshly created request to queue
bool ModbusClientTCPasync::addToQueue(int32_t token, ModbusMessage request, bool syncReq) {
  // Did we get one?
  if (request) {
    LOCK_GUARD(lock1, qLock);
    if (txQueue.size() + rxQueue.size() < MTA_qLimit) {
      HEXDUMP_V("Enqueue", request.data(), request.size());
      RequestEntry *re = new RequestEntry(token, request, syncReq);
      if (!re) return false;  //TODO: proper error returning in case allocation fails
      // inject proper transactionID
      re->head.transactionID = messageCount++;
      re->head.len = request.size();
      // if we're already connected, try to send and push to rxQueue
      // or else push to txQueue and (re)connect
      if (MTA_state == CONNECTED && send(re)) {
        re->sentTime = millis();
        rxQueue[re->head.transactionID] = re;
      } else {
        txQueue.push_back(re);
        if (MTA_state == DISCONNECTED) {
          connect();
        }
      }
      return true;
    }
    LOG_E("queue is full\n");
  }
  return false;
}

void ModbusClientTCPasync::onConnected() {
  LOG_D("connected\n");
  LOCK_GUARD(lock1, sLock);
  MTA_state = CONNECTED;
  MTA_lastActivity = millis();
  // from now on onPoll will be called every 500 msec
}

void ModbusClientTCPasync::onDisconnected() {
  LOG_D("disconnected\n");
  LOCK_GUARD(lock1, sLock);
  MTA_state = DISCONNECTED;

  // empty queue on disconnect, calling errorcode on every waiting request
  LOCK_GUARD(lock2, qLock);
  while (!txQueue.empty()) {
    RequestEntry* r = txQueue.front();
    if (onError) {
      onError(IP_CONNECTION_FAILED, r->token);
    }
    delete r;
    txQueue.pop_front();
  }
  while (!rxQueue.empty()) {
    RequestEntry *r = rxQueue.begin()->second;
    if (onError) {
      onError(IP_CONNECTION_FAILED, r->token);
    }
    delete r;
    rxQueue.erase(rxQueue.begin());
  }
}


void ModbusClientTCPasync::onACError(AsyncClient* c, int8_t error) {
  // onDisconnect will alse be called, so nothing to do here
  LOG_W("TCP error: %s\n", c->errorToString(error));
}

/*
void onTimeout(uint32_t time) {
  // timeOut is handled by onPoll or onDisconnect
}

void onAck(size_t len, uint32_t time) {
  // assuming we don't need this
}
*/
void ModbusClientTCPasync::onPacket(uint8_t* data, size_t length) {
  LOG_D("packet received (len:%u)\n", length);
  // reset idle timeout
  MTA_lastActivity = millis();

  if (length) {
    LOG_D("parsing (len:%u)\n", length + 1);
  }
  while (length > 0) {
    RequestEntry* request = nullptr;
    ModbusMessage* response = nullptr;
    uint16_t transactionID = 0;
    bool isOkay = false;

    // 1. Check for valid modbus message

    // MBAP header is 6 bytes, we can't do anything with less
    // total message should fit MBAP plus remaining bytes (in data[4], data[5])
    if (length > 6) {
      transactionID = (data[0] << 8) | data[1];
      uint16_t protocolID = (data[2] << 8) | data[3];
      uint16_t messageLength = (data[4] << 8) | data[5];
      if (protocolID == 0 &&
        length >= (uint32_t)messageLength + 6 &&
        messageLength < 256) {
        response = new ModbusMessage(messageLength);
        response->add(&data[6], messageLength);
        LOG_D("packet validated (len:%d)\n", messageLength);

        // on next iteration: adjust remaining length and pointer to data
        length -= 6 + messageLength;
        data += 6 + messageLength;
        isOkay = true;
      }
    }

    if (!isOkay) {
      // invalid packet, abort function
      LOG_W("packet invalid\n");
      return;
    } else {
      // 2. we got a valid response, match with a request
      LOCK_GUARD(lock1, qLock);
      auto i = rxQueue.find(transactionID);
      if (i != rxQueue.end()) {
        // found it, handle it and stop iterating
        request = i->second;
        i = rxQueue.erase(i);
        LOG_D("matched request\n");
      } else {
        // TCP packet did not yield valid modbus response, abort function
        LOG_W("no matching request found\n");
        return;
      }
    }

    // 3. we have a valid request and a valid response, call appropriate callback
    if (request) {
      // compare request with response
      Error error = SUCCESS;
      if (request->msg.getFunctionCode() != (response->getFunctionCode() & 0x7F)) {
        error = FC_MISMATCH;
      } else if (request->msg.getServerID() != response->getServerID()) {
        error = SERVER_ID_MISMATCH;
      } else {
        error = response->getError();
      }

      if (error != SUCCESS) {
        LOCK_GUARD(errorCntLock, countAccessM);
        errorCount++;
      }

      if (request->isSyncRequest) {
        {
          LOCK_GUARD(sL ,syncRespM);
          syncResponse[request->token] = *response;
        }
      } else if (onResponse) {
        onResponse(*response, request->token);
      } else {
        if (error == SUCCESS) {
          if (onData) {
            onData(*response, request->token);
          }
        } else {
          if (onError) {
            onError(response->getError(), request->token);
          }
        }
      }
      delete request;
    }
    delete response;

  }  // end processing of incoming data

  // check if we have to send the next request
  LOCK_GUARD(lock1, qLock);
  handleSendingQueue();
}

void ModbusClientTCPasync::onPoll() {
  {
  LOCK_GUARD(lock1, qLock);

  // try to send whatever is waiting
  handleSendingQueue();

  // next check if timeout has struck for oldest request
  if (!rxQueue.empty()) {
    RequestEntry* request = rxQueue.begin()->second;
    if (millis() - request->sentTime > MTA_timeout) {
      LOG_D("request timeouts (now:%lu-sent:%u)\n", millis(), request->sentTime);
      // oldest element timeouts, call onError and clean up
      if (onError) {
        // Handle timeout error
        onError(TIMEOUT, request->token);
      }
      delete request;
      rxQueue.erase(rxQueue.begin());
    }
  }
    
  }  // end lockguard scope

  // if nothing happened during idle timeout, gracefully close connection
  if (millis() - MTA_lastActivity > MTA_idleTimeout) {
    disconnect();
  }
}

void ModbusClientTCPasync::handleSendingQueue() {
  // ATTENTION: This method does not have a lock guard.
  // Calling sites must assure shared resources are protected
  // by mutex.

  // try to send everything we have waiting
  std::list<RequestEntry*>::iterator it = txQueue.begin();
  while (it != txQueue.end()) {
    // get the actual element
    if (send(*it)) {
      // after sending, update timeout value, add to other queue and remove from this queue
      (*it)->sentTime = millis();
      rxQueue[(*it)->head.transactionID] = (*it);      // push request to other queue
      it = txQueue.erase(it);  // remove from toSend queue and point i to next request
    } else {
      // sending didn't succeed, try next request
      ++it;
    }
  }
}

bool ModbusClientTCPasync::send(RequestEntry* re) {
  // ATTENTION: This method does not have a lock guard.
  // Calling sites must assure shared resources are protected
  // by mutex.

  if (rxQueue.size() >= MTA_maxInflightRequests) {
    return false;
  }

  // check if TCP client is able to send
  if (MTA_client.space() > ((uint32_t)re->msg.size() + 6)) {
    // Write TCP header first
    MTA_client.add(reinterpret_cast<const char *>((const uint8_t *)(re->head)), 6, ASYNC_WRITE_FLAG_COPY);
    // Request comes next
    MTA_client.add(reinterpret_cast<const char*>(re->msg.data()), re->msg.size(), ASYNC_WRITE_FLAG_COPY);
    // done
    MTA_client.send();
    LOG_D("request sent (msgid:%d)\n", re->head.transactionID);
    return true;
  }
  return false;
}
