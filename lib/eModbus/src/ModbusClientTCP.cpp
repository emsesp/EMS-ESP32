// =================================================================================================
// eModbus: Copyright 2020 by Michael Harwerth, Bert Melis and the contributors to eModbus
//               MIT license - see license.md for details
// =================================================================================================
#include "ModbusClientTCP.h"

#if HAS_FREERTOS || IS_LINUX

#undef LOCAL_LOG_LEVEL
// #define LOCAL_LOG_LEVEL LOG_LEVEL_VERBOSE
#include "Logging.h"

// Constructor takes reference to Client (EthernetClient or WiFiClient)
ModbusClientTCP::ModbusClientTCP(Client& client, uint16_t queueLimit) :
  ModbusClient(),
  MT_client(client),
  MT_lastTarget(IPAddress(0, 0, 0, 0), 0, DEFAULTTIMEOUT, TARGETHOSTINTERVAL),
  MT_target(IPAddress(0, 0, 0, 0), 0, DEFAULTTIMEOUT, TARGETHOSTINTERVAL),
  MT_defaultTimeout(DEFAULTTIMEOUT),
  MT_defaultInterval(TARGETHOSTINTERVAL),
  MT_qLimit(queueLimit),
  MT_timeoutsToClose(0)
  { }

// Alternative Constructor takes reference to Client (EthernetClient or WiFiClient) plus initial target host
ModbusClientTCP::ModbusClientTCP(Client& client, IPAddress host, uint16_t port, uint16_t queueLimit) :
  ModbusClient(),
  MT_client(client),
  MT_lastTarget(IPAddress(0, 0, 0, 0), 0, DEFAULTTIMEOUT, TARGETHOSTINTERVAL),
  MT_target(host, port, DEFAULTTIMEOUT, TARGETHOSTINTERVAL),
  MT_defaultTimeout(DEFAULTTIMEOUT),
  MT_defaultInterval(TARGETHOSTINTERVAL),
  MT_qLimit(queueLimit),
  MT_timeoutsToClose(0)
  { }

// Destructor: clean up queue, task etc.
ModbusClientTCP::~ModbusClientTCP() {
  end();
}

// end: stop worker task
void ModbusClientTCP::end() {
  // Clean up queue
  {
    // Safely lock access
    LOCK_GUARD(lockGuard, qLock);
    // Get all queue entries one by one
    while (!requests.empty()) {
      requests.pop();
    }
  }
  LOG_D("TCP client worker killed.\n");
  // Kill task
  if (worker) {
#if IS_LINUX
    pthread_cancel(worker);
    worker = NULL;
#else
    vTaskDelete(worker);
    worker = nullptr;
#endif
  }
}

// begin: start worker task
#if IS_LINUX
void *ModbusClientTCP::pHandle(void *p) {
  handleConnection(static_cast<ModbusClientTCP *>(p));
  return nullptr;
}
#endif

void ModbusClientTCP::begin(int coreID) {
  if (!worker) {
#if IS_LINUX
    int rc = pthread_create(&worker, NULL, &pHandle, this);
    if (rc) {
      LOG_E("Error creating TCP client thread: %d\n", rc);
    } else {
      LOG_D("TCP client worker started.\n");
    }

#else
    // Create unique task name
    char taskName[18];
    snprintf(taskName, 18, "Modbus%02XTCP", instanceCounter);
    // Start task to handle the queue
    xTaskCreatePinnedToCore((TaskFunction_t)&handleConnection, taskName, CLIENT_TASK_STACK, this, 5, &worker, coreID >= 0 ? coreID : tskNO_AFFINITY);
    LOG_D("TCP client worker %s started\n", taskName);
#endif
  } else {
    LOG_E("Worker thread has been already started!");
  }
}

// Set default timeout value (and interval)
void ModbusClientTCP::setTimeout(uint32_t timeout, uint32_t interval) {
  MT_defaultTimeout = timeout;
  MT_defaultInterval = interval;
}

// Switch target host (if necessary)
// Return true, if host/port is different from last host/port used
bool ModbusClientTCP::setTarget(IPAddress host, uint16_t port, uint32_t timeout, uint32_t interval) {
  MT_target.host = host;
  MT_target.port = port;
  MT_target.timeout = timeout ? timeout : MT_defaultTimeout;
  MT_target.interval = interval ? interval : MT_defaultInterval;
  LOG_D("Target set: %d.%d.%d.%d:%d\n", host[0], host[1], host[2], host[3], port);
  if (MT_target.host == MT_lastTarget.host && MT_target.port == MT_lastTarget.port) return false;
  return true;
}

// Return number of unprocessed requests in queue
uint32_t ModbusClientTCP::pendingRequests() {
  return requests.size();
}

// Remove all pending request from queue
void ModbusClientTCP::clearQueue() {
  std::queue<RequestEntry *> empty;
  LOCK_GUARD(lockGuard, qLock);
  // Delete queue entries if still on the queue
  while (!requests.empty()) {
    RequestEntry *re = requests.front();
    delete re;
    requests.pop();
  }
  // Now flush the queue
  std::swap(requests, empty);
}

// Set number of timeouts to tolerate before a connection is forcibly closed.
// 0: never, 1..255: desired number
// Returns previous value.
uint8_t ModbusClientTCP::closeConnectionOnTimeouts(uint8_t n) {
  uint8_t oldValue = MT_timeoutsToClose;
  MT_timeoutsToClose = n;
  return oldValue;
}

// Base addRequest for preformatted ModbusMessage and last set target
Error ModbusClientTCP::addRequestM(ModbusMessage msg, uint32_t token) {
  Error rc = SUCCESS;        // Return value

  // Add it to the queue, if valid
  if (msg) {
    // Queue add successful?
    if (!addToQueue(token, msg, MT_target)) {
      // No. Return error after deleting the allocated request.
      rc = REQUEST_QUEUE_FULL;
    }
  }

  LOG_D("Add TCP request result: %02X\n", rc);
  return rc;
}

// TCP addRequest for preformatted ModbusMessage and adhoc target
Error ModbusClientTCP::addRequestMT(ModbusMessage msg, uint32_t token, IPAddress targetHost, uint16_t targetPort) {
  Error rc = SUCCESS;        // Return value

  // Add it to the queue, if valid
  if (msg) {
    // Set up adhoc target 
    TargetHost adhocTarget(targetHost, targetPort, MT_defaultTimeout, MT_defaultInterval);
    // Queue add successful?
    if (!addToQueue(token, msg, adhocTarget, true)) {
      // No. Return error after deleting the allocated request.
      rc = REQUEST_QUEUE_FULL;
    }
  }

  LOG_D("Add TCP request result: %02X\n", rc);
  return rc;
}

// Base syncRequest follows the same pattern
ModbusMessage ModbusClientTCP::syncRequestM(ModbusMessage msg, uint32_t token) {
  ModbusMessage response;

  if (msg) {
    // Queue add successful?
    if (!addToQueue(token, msg, MT_target, true)) {
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

// TCP syncRequest with adhoc target parameters
ModbusMessage ModbusClientTCP::syncRequestMT(ModbusMessage msg, uint32_t token, IPAddress targetHost, uint16_t targetPort) {
  ModbusMessage response;

  if (msg) {
    // Set up adhoc target 
    TargetHost adhocTarget(targetHost, targetPort, MT_defaultTimeout, MT_defaultInterval);
    // Queue add successful?
    if (!addToQueue(token, msg, adhocTarget, true)) {
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
bool ModbusClientTCP::addToQueue(uint32_t token, ModbusMessage request, TargetHost target, bool syncReq) {
  bool rc = false;
  // Did we get one?
  LOG_D("Queue size: %d\n", (uint32_t)requests.size());
  HEXDUMP_D("Enqueue", request.data(), request.size());
  if (request) {
    if (requests.size()<MT_qLimit) {
      RequestEntry *re = new RequestEntry(token, request, target, syncReq);
      // inject proper transactionID
      re->head.transactionID = messageCount++;
      re->head.len = request.size();
      // Safely lock queue and push request to queue
      rc = true;
      LOCK_GUARD(lockGuard, qLock);
      requests.push(re);
    }
  }

  return rc;
}

// handleConnection: worker task
// This was created in begin() to handle the queue entries
void ModbusClientTCP::handleConnection(ModbusClientTCP *instance) {
  bool doNotPop;
  unsigned long lastRequest = millis();
  uint16_t timeoutCount = 0;       // Run time counter of consecutive timeouts.

  // Loop forever - or until task is killed
  while (1) {
    // Do we have a request in queue?
    if (!instance->requests.empty()) {
      // Yes. pull it.
      RequestEntry *request = instance->requests.front();
      doNotPop = false;
      LOG_D("Got request from queue\n");

      // Do we have a connection open?
      if (instance->MT_client.connected()) {
        // Empty the RX buffer in case there is a stray response left
        while (instance->MT_client.read() != -1) {}
        // check if lastHost/lastPort!=host/port off the queued request
        if (instance->MT_lastTarget != request->target) {
          // It is different. Disconnect it.
          instance->MT_client.stop();
          LOG_D("Target different, disconnect\n");
          delay(1);  // Give scheduler room to breathe
        } else {
          // it is the same host/port.
          // Give it some slack to get ready again
          while (millis() - lastRequest < request->target.interval) { delay(1); }
        }
      }
      // if client is disconnected (we will have to switch hosts)
      if (!instance->MT_client.connected()) {
        // Serial.println("Client reconnecting");
        // It is disconnected. connect to host/port from queue
        instance->MT_client.connect(request->target.host, request->target.port);
        LOG_D("Target connect (%d.%d.%d.%d:%d).\n", request->target.host[0], request->target.host[1], request->target.host[2], request->target.host[3], request->target.port);

        delay(1);  // Give scheduler room to breathe
      }
      ModbusMessage response;
      // Are we connected (again)?
      if (instance->MT_client.connected()) {
        LOG_D("Is connected. Send request.\n");
        // Yes. Send the request via IP
        instance->send(request);

        // Get the response - if any
        response = instance->receive(request);

        // Did we get a normal response?
        if (response.getError()==SUCCESS) {
          LOG_D("Data response.\n");
          // Reset timeout counter 
          timeoutCount = 0;
          // Yes. Is it a synchronous request?
          if (request->isSyncRequest) {
            // Yes. Put the response into the response map
            {
              LOCK_GUARD(sL, instance->syncRespM);
              instance->syncResponse[request->token] = response;
            }
          // No, async request. Do we have an onResponse handler?
          } else if (instance->onResponse) {
            // Yes. Call it.
            instance->onResponse(response, request->token);
          // No, but do we have an onData handler registered?
          } else if (instance->onData) {
            // Yes. call it
            instance->onData(response, request->token);
          } else {
            LOG_D("No handler for response!\n");
          }
        } else {
          // No, something went wrong. All we have is an error
          LOG_D("Error response.\n");
          // Count it
          {
            LOCK_GUARD(responseCnt, instance->countAccessM);
            instance->errorCount++;
          }
          // Is it a TIMEOUT and do we need to track it?
          if (response.getError()==TIMEOUT && instance->MT_timeoutsToClose) {
            LOG_D("Checking timeout sequence\n");
            // Yes. First count timeout conter up
            timeoutCount++;
            // Is the count above the limit?
            if (timeoutCount > instance->MT_timeoutsToClose) {
              LOG_D("Timeouts: %d exceeding limit (%d), closing connection\n", 
                timeoutCount, instance->MT_timeoutsToClose);
              // Yes. We need to cut the connection
              instance->MT_client.stop();
              delay(1);
              // reset timeout count
              timeoutCount = 0;
            }
          } else {
            // No TIMEOUT or no limit: reset timeout count
            timeoutCount = 0;
          }
          // Is it a synchronous request?
          if (request->isSyncRequest) {
            // Yes. Put the response into the response map
            {
              LOCK_GUARD(sL, instance->syncRespM);
              instance->syncResponse[request->token] = response;
            }
          // No, but do we have an onResponse handler?
          } else if (instance->onResponse) {
            // Yes, call it.
            instance->onResponse(response, request->token);
          // No, but do we have an onError handler?
          } else if (instance->onError) {
            // Yes. Forward the error code to it
            instance->onError(response.getError(), request->token);
          } else {
            LOG_D("No onError handler\n");
          }
        }
        //   set lastHost/lastPort tp host/port
        instance->MT_lastTarget = request->target;
      } else {
        // Oops. Connection failed
        response.setError(request->msg.getServerID(), request->msg.getFunctionCode(), IP_CONNECTION_FAILED);
        // Is it a synchronous request?
        if (request->isSyncRequest) {
          // Yes. Put the response into the response map
          {
            LOCK_GUARD(sL, instance->syncRespM);
            instance->syncResponse[request->token] = response;
          }
        // No, but do we have an onResponse handler?
        } else if (instance->onResponse) {
          // Yes, call it.
          instance->onResponse(response, request->token);
        // Finally, do we have an onError handler?
        } else if (instance->onError) {
          // Yes. Forward the error code to it
          instance->onError(IP_CONNECTION_FAILED, request->token);
        }
      }
      // Clean-up time. 
      if (!doNotPop)
      {
        // Safely lock the queue
        LOCK_GUARD(lockGuard, instance->qLock);

        // Remove the front queue entry if the queue is not empty
        if (!instance->requests.empty()) {
          instance->requests.pop();
        }
        // Delete request
        delete request;
        LOG_D("Request popped from queue.\n");
      }
      lastRequest = millis();
    } else {
      delay(1);  // Give scheduler room to breathe
    }
  }
}

// send: send request via Client connection
void ModbusClientTCP::send(RequestEntry *request) {
  // We have a established connection here, so we can write right away.
  // Move tcpHead and request into one continuous buffer, since the very first request tends to 
  // take too long to be sent to be recognized.
  ModbusMessage m;
  m.add((const uint8_t *)request->head, 6);
  m.append(request->msg);

  MT_client.write(m.data(), m.size());
  // Done. Are we?
  MT_client.flush();
  HEXDUMP_V("Request packet", m.data(), m.size());
}

// receive: get response via Client connection
ModbusMessage ModbusClientTCP::receive(RequestEntry *request) {
  unsigned long lastMillis = millis();     // Timer to check for timeout
  bool hadData = false;               // flag data received
  const uint16_t dataLen(300);        // Modbus Packet supposedly will fit (260<300)
  uint8_t data[dataLen];              // Local buffer to collect received data
  uint16_t dataPtr = 0;               // Pointer into data
  ModbusMessage response;             // Response structure to be returned

  // wait for packet data, overflow or timeout
  while (millis() - lastMillis < request->target.timeout && dataPtr < dataLen && !hadData) {
    // Is there data waiting?
    if (MT_client.available()) {
      // Yes. catch as much as is there and fits into buffer
      while (MT_client.available() && dataPtr < dataLen) {
        data[dataPtr++] = MT_client.read();
      }
      // Register data received
      hadData = true;
      // Rewind EOT and timeout timers
      lastMillis = millis();
    }
    delay(1); // Give scheduler room to breathe
  }
  // Did we get some data?
  if (hadData) {
    LOG_D("Received response.\n");
    HEXDUMP_V("Response packet", data, dataPtr);
    // Yes. check it for validity
    // First transactionID and protocolID shall be identical, length has to match the remainder.
    ModbusTCPhead head(request->head.transactionID, request->head.protocolID, dataPtr - 6);
    // Matching head?
    if (memcmp((const uint8_t *)head, data, 6)) {
      // No. return Error response
      response.setError(request->msg.getServerID(), request->msg.getFunctionCode(), TCP_HEAD_MISMATCH);
      // If the server id does not match that of the request, report error
    } else if (data[6] != request->msg.getServerID()) {
      response.setError(request->msg.getServerID(), request->msg.getFunctionCode(), SERVER_ID_MISMATCH);
      // If the function code does not match that of the request, report error
    } else if ((data[7] & 0x7F) != request->msg.getFunctionCode()) {
      response.setError(request->msg.getServerID(), request->msg.getFunctionCode(), FC_MISMATCH);
    } else {
      // Looks good.
      response.add(data + 6, dataPtr - 6);
    }
  } else {
    // No, timeout must have struck
    response.setError(request->msg.getServerID(), request->msg.getFunctionCode(), TIMEOUT);
  }
  return response;
}

#endif
