/*
Copyright (c) 2022 Bert Melis. All rights reserved.

This work is licensed under the terms of the MIT license.  
For a copy, see <https://opensource.org/licenses/MIT> or
the LICENSE file.
*/

#pragma once


// #include "esp_tls.h"
#include <WiFiClient.h>
#include <ESP_SSLClient.h>
#include "Transport.h"

namespace espMqttClientInternals {

class ClientSecureSync : public Transport {
  public:
    ClientSecureSync();
    ~ClientSecureSync();
    bool   connect(IPAddress ip, uint16_t port) override;
    bool   connect(const char * host, uint16_t port) override;
    size_t write(const uint8_t * buf, size_t size) override;
    int    read(uint8_t * buf, size_t size) override;
    void   stop() override;
    bool   connected() override;
    bool   disconnected() override;

    WiFiClient    basic_client;
    ESP_SSLClient client;
};

} // namespace espMqttClientInternals
