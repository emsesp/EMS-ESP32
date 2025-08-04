// =================================================================================================
// eModbus: Copyright 2020 by Michael Harwerth, Bert Melis and the contributors to eModbus
//               MIT license - see license.md for details
// =================================================================================================

#include "ModbusServerTCPasync.h"
#define LOCAL_LOG_LEVEL LOG_LEVEL_VERBOSE
// #undef LOCAL_LOG_LEVEL
#include "Logging.h"

ModbusServerTCPasync::mb_client::mb_client(ModbusServerTCPasync* s, AsyncClient* c) :
  server(s),
  client(c),
  lastActiveTime(millis()),
  message(nullptr),
  error(SUCCESS),
  outbox() {
    client->onData([](void* i, AsyncClient* c, void* data, size_t len) { (static_cast<mb_client*>(i))->onData(static_cast<uint8_t*>(data), len); }, this);
    client->onPoll([](void* i, AsyncClient* c) { (static_cast<mb_client*>(i))->onPoll(); }, this);
    client->onDisconnect([](void* i, AsyncClient* c) { (static_cast<mb_client*>(i))->onDisconnect(); }, this);
    client->setNoDelay(true);
}

ModbusServerTCPasync::mb_client::~mb_client() {
  // clear outbox, if data is left
  while (!outbox.empty()) {
    outbox.pop();
  }

  delete client;  // will also close connection, if any
}

void ModbusServerTCPasync::mb_client::onData(uint8_t* data, size_t len) {
  lastActiveTime = millis();
  LOG_D("data len %u\n", len);

  Error error = SUCCESS;
  size_t i = 0;
  while (i < len) {
    // 0. start
    if (!message) {
      message = new ModbusMessage(8);
      error = SUCCESS;
    }

    //  1. get minimal 8 bytes to move on
    while (message->size() < 8 && i < len) {
      message->push_back(data[i++]);
    }
    
    // 2. preliminary validation: protocol bytes and message length
    if ((*message)[2] != 0 || (*message)[3] != 0) {
      error = TCP_HEAD_MISMATCH;
      LOG_D("invalid protocol\n");
    }
    size_t messageLength = (((*message)[4] << 8) | (*message)[5]) + 6;
    if (messageLength > 262) {  // 256 + MBAP(6) = 262
      error = PACKET_LENGTH_ERROR;
      LOG_D("max length error\n");
    }
    if (error != SUCCESS) {
      ModbusMessage response;
      response.setError(message->getServerID(), message->getFunctionCode(), error);
      message->resize(4);
      message->add(static_cast<uint16_t>(3));
      message->append(response);
      addResponseToOutbox(message);  // outbox has pointer ownership now
      // reset to starting values and process remaining data
      message = nullptr;
      return;  // protocol validation, abort further parsing
    }

    // 3. receive until request is complete
    while (message->size() < messageLength && i < len) {
      message->push_back(data[i++]);
    }
    if (message->size() == messageLength) {
      LOG_D("request complete (len:%d)\n", message->size());
    } else {
      LOG_D("request incomplete (len:%d), waiting for next TCP packet\n", message->size());
      continue;
    }

    // 4. request complete, process
    ModbusMessage request(messageLength - 6);  // create request without MBAP, with server ID
    request.add(message->data() + 6, message->size() - 6);
    ModbusMessage userData;
    if (server->isServerFor(request.getServerID())) {
      MBSworker callback = server->getWorker(request.getServerID(), request.getFunctionCode());
      if (callback) {
        // request is well formed and is being served by user API
        userData = callback(request);
        // Process Response
        // One of the predefined types?
        if (userData[0] == 0xFF && (userData[1] == 0xF0 || userData[1] == 0xF1)) {
          // Yes. Check it
          switch (userData[1]) {
          case 0xF0: // NIL
            userData.clear();
            LOG_D("NIL response\n");
            break;
          case 0xF1: // ECHO
            userData = request;
            if (request.getFunctionCode() == WRITE_MULT_REGISTERS ||
                request.getFunctionCode() == WRITE_MULT_COILS) {
              userData.resize(6);
            }
            LOG_D("ECHO response\n");
            break;
          default:   // Will not get here!
            break;
          }
        } else {
          // No. User provided data response
          LOG_D("Data response\n");
        }
        error = SUCCESS;
      } else {  // no worker found
        error = ILLEGAL_FUNCTION;
      }
    } else {  // mismatch server ID
      error = INVALID_SERVER;
    }
    if (error != SUCCESS) {
      userData.setError(request.getServerID(), request.getFunctionCode(), error);
    }
    // Keep transaction id and protocol id
    message->resize(4);
    // Add new payload length
    message->add(static_cast<uint16_t>(userData.size()));
    // Append payload
    message->append(userData);
    // Transfer message data to outbox
    addResponseToOutbox(message);
    message = nullptr;
  }  // end while loop iterating incoming data
}

void ModbusServerTCPasync::mb_client::onPoll() {
  LOCK_GUARD(lock1, obLock);
  handleOutbox();
  if (server->idle_timeout > 0 && 
      millis() - lastActiveTime > server->idle_timeout) {
    LOG_D("client idle, closing\n");
    client->close();
  }
}

void ModbusServerTCPasync::mb_client::onDisconnect() {
  LOG_D("client disconnected\n");
  server->onClientDisconnect(this);
}

void ModbusServerTCPasync::mb_client::addResponseToOutbox(ModbusMessage* response) {
  if (response->size() > 0) {
    LOCK_GUARD(lock1, obLock);
    outbox.push(response);
    handleOutbox();
  }
}

void ModbusServerTCPasync::mb_client::handleOutbox() {
  while (!outbox.empty()) {
    ModbusMessage* m = outbox.front();
    if (m->size() <= client->space()) {
      LOG_D("sending (%d)\n", m->size());
      client->add(reinterpret_cast<const char*>(m->data()), m->size(), ASYNC_WRITE_FLAG_COPY);
      client->send();
      delete m;
      outbox.pop();
    } else {
      return;
    }
  }
}

ModbusServerTCPasync::ModbusServerTCPasync() :
  server(nullptr),
  clients(),
  maxNoClients(5),
  idle_timeout(60000) {
    // setup will be done in 'start'
}


ModbusServerTCPasync::~ModbusServerTCPasync() {
  stop();
  delete server;
}


uint16_t ModbusServerTCPasync::activeClients() {
  LOCK_GUARD(lock1, cListLock);
  return clients.size();
}


bool ModbusServerTCPasync::start(uint16_t port, uint8_t maxClients, uint32_t timeout, int coreID) {
  // don't restart if already running
  if (server) {
    LOG_W("Server already running.\n");
    return false;
  }
  
  maxNoClients = maxClients;
  idle_timeout = timeout;
  server = new AsyncServer(port);
  if (server) {
    server->setNoDelay(true);
    server->onClient([](void* i, AsyncClient* c) { (static_cast<ModbusServerTCPasync*>(i))->onClientConnect(c); }, this);
    server->begin();
    LOG_D("Modbus server started\n");
    return true;
  }
  LOG_E("Could not start server\n");
  return false;
}

bool ModbusServerTCPasync::stop() {
  
  if (!server) {
    LOG_W("Server not running.\n");
    return false;
  }
  
  // stop server to prevent new clients connecting
  server->end();

  // now close existing clients
  LOCK_GUARD(lock1, cListLock);
  while (!clients.empty()) {
    // prevent onDisconnect handler to be called, resulting in deadlock
    clients.front()->client->onDisconnect(nullptr, nullptr);
    delete clients.front();
    clients.pop_front();
  }
  delete server;
  server = nullptr;
  LOG_D("Modbus server stopped\n");
  return true;
}

bool ModbusServerTCPasync::isRunning() {
  if (server) return true;
  else return false;
}

void ModbusServerTCPasync::onClientConnect(AsyncClient* client) {
  LOG_D("new client\n");
  LOCK_GUARD(lock1, cListLock);
  if (clients.size() < maxNoClients) {
    clients.emplace_back(new mb_client(this, client));
    LOG_D("nr clients: %u\n", clients.size());
  } else {
    LOG_D("max number of clients reached, closing new\n");
    client->close(true);
    delete client;
  }
}

void ModbusServerTCPasync::onClientDisconnect(mb_client* client) {
  LOCK_GUARD(lock1, cListLock);
  // delete mb_client from list
  clients.remove_if([client](mb_client* i) { return i->client == client->client; });
  // delete client itself
  delete client;
  LOG_D("nr clients: %u\n", clients.size());
}
