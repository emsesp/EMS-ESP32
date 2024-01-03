/*
  Asynchronous WebServer library for Espressif MCUs

  Copyright (c) 2016 Hristo Gochkov. All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/
#ifndef PsychicEventSource_H_
#define PsychicEventSource_H_

#include "PsychicCore.h"
#include "PsychicHandler.h"
#include "PsychicClient.h"
#include "PsychicResponse.h"

class PsychicEventSource;
class PsychicEventSourceResponse;
class PsychicEventSourceClient;
class PsychicResponse;

typedef std::function<void(PsychicEventSourceClient *client)> PsychicEventSourceClientCallback;

class PsychicEventSourceClient : public PsychicClient {
  friend PsychicEventSource;

  protected:
    uint32_t _lastId;

  public:
    PsychicEventSourceClient(PsychicClient *client);
    ~PsychicEventSourceClient();

    uint32_t lastId() const { return _lastId; }
    void send(const char *message, const char *event=NULL, uint32_t id=0, uint32_t reconnect=0);
    void sendEvent(const char *event);
};

class PsychicEventSource : public PsychicHandler {
  private:
    PsychicEventSourceClientCallback _onOpen;
    PsychicEventSourceClientCallback _onClose;

  public:
    PsychicEventSource();
    ~PsychicEventSource();

    PsychicEventSourceClient * getClient(int socket) override;
    PsychicEventSourceClient * getClient(PsychicClient *client) override;
    void addClient(PsychicClient *client) override;
    void removeClient(PsychicClient *client) override;
    void openCallback(PsychicClient *client) override;
    void closeCallback(PsychicClient *client) override;

    PsychicEventSource *onOpen(PsychicEventSourceClientCallback fn);
    PsychicEventSource *onClose(PsychicEventSourceClientCallback fn);

    esp_err_t handleRequest(PsychicRequest *request) override final;

    void send(const char *message, const char *event=NULL, uint32_t id=0, uint32_t reconnect=0);
};

class PsychicEventSourceResponse: public PsychicResponse {
  public:
    PsychicEventSourceResponse(PsychicRequest *request);
    virtual esp_err_t send() override;
};

String generateEventMessage(const char *message, const char *event, uint32_t id, uint32_t reconnect);

#endif /* PsychicEventSource_H_ */