/*
Copyright (c) 2022 Bert Melis. All rights reserved.

This work is licensed under the terms of the MIT license.  
For a copy, see <https://opensource.org/licenses/MIT> or
the LICENSE file.
*/

#pragma once

#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)

// Added for EMS-ESP
#include "../Config.h"
#if defined(EMC_CLIENT_SECURE)
#include <WiFiClientSecure.h> // includes IPAddress
#else
#include <WiFiClient.h>
#endif
#include "Transport.h"

namespace espMqttClientInternals {

class ClientSecureSync : public Transport {
  public:
    ClientSecureSync();
    bool   connect(IPAddress ip, uint16_t port) override;
    bool   connect(const char * host, uint16_t port) override;
    size_t write(const uint8_t * buf, size_t size) override;
    int    read(uint8_t * buf, size_t size) override;
    void   stop() override;
    bool   connected() override;
    bool   disconnected() override;
    // added for EMS-ESP
#if defined(EMC_CLIENT_SECURE)
    WiFiClientSecure client;
#else
    WiFiClient client;
#endif
};

} // namespace espMqttClientInternals

#endif
