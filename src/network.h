/*
 * EMS-ESP - https://github.com/proddy/EMS-ESP
 * Copyright 2019  Paul Derbyshire
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

// code by nomis - https://github.com/nomis


#ifndef EMSESP_NETWORK_H_
#define EMSESP_NETWORK_H_

#ifndef EMSESP_STANDALONE
#if defined(ESP8266)
#include <ESP8266WiFi.h>
#elif defined(ESP32)
#include <WiFi.h>
#endif
#include <ArduinoOTA.h>
#endif

#include <string>
#include <vector>
#include <functional>

#include "settings.h"
#include "system.h"
#include "helpers.h"

namespace emsesp {

using namespace std::placeholders; // for `_1`

class Network {
  public:
    static void connect();
    static void reconnect();
    static void disconnect();
    static void scan(uuid::console::Shell & shell);
    static void show_network(uuid::console::Shell & shell);

    static int8_t wifi_quality();

    void start();
    bool loop();

    bool connected() {
#ifndef EMSESP_STANDALONE
        return (WiFi.status() == WL_CONNECTED);
#else
        return true;
#endif
    }

  private:
    static constexpr uint16_t OTA_PORT = 8266;

#if defined(ESP8266)
    void sta_mode_connected(const WiFiEventStationModeConnected & event);
    void sta_mode_disconnected(const WiFiEventStationModeDisconnected & event);
    void sta_mode_got_ip(const WiFiEventStationModeGotIP & event);

    WiFiEventHandler sta_mode_connected_;
    WiFiEventHandler sta_mode_disconnected_;
    WiFiEventHandler sta_mode_got_ip_;

    ArduinoOTAClass * ota_;

#elif defined(ESP32)

    void sta_mode_connected(WiFiEvent_t event, WiFiEventInfo_t info);
    void sta_mode_disconnected(WiFiEvent_t event, WiFiEventInfo_t info);
    void sta_mode_got_ip(WiFiEvent_t event, WiFiEventInfo_t info);
    void sta_mode_start(WiFiEvent_t event, WiFiEventInfo_t info);

    ArduinoOTAClass * ota_;

#endif

    static uuid::log::Logger logger_;

    void ota_setup();

    uint8_t disconnect_count_ = 0;
    bool    in_ota_           = false;
};

} // namespace emsesp

#endif
