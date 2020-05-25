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

#include "network.h"

MAKE_PSTR(logger_name, "network")

namespace emsesp {

uuid::log::Logger Network::logger_{F_(logger_name), uuid::log::Facility::KERN};

void Network::start() {
#ifndef EMSESP_STANDALONE
    // We want the device to come up in opmode=0 (WIFI_OFF), when erasing the flash this is not the default.
    // If needed, we save opmode=0 before disabling persistence so the device boots with WiFi disabled in the future.
    if (WiFi.getMode() != WIFI_OFF) {
        WiFi.mode(WIFI_OFF);
    }
    WiFi.persistent(false);
    WiFi.disconnect(true);
    WiFi.setAutoReconnect(false);
    WiFi.mode(WIFI_STA);
#endif

#if defined(ESP8266)
    sta_mode_connected_    = WiFi.onStationModeConnected(std::bind(&Network::sta_mode_connected, this, _1));
    sta_mode_disconnected_ = WiFi.onStationModeDisconnected(std::bind(&Network::sta_mode_disconnected, this, _1));
    sta_mode_got_ip_       = WiFi.onStationModeGotIP(std::bind(&Network::sta_mode_got_ip, this, _1));

    connect();   // connect to WiFi
    ota_setup(); // initialize OTA

#elif defined(ESP32)
    WiFi.mode(WIFI_MODE_MAX);
    WiFi.mode(WIFI_MODE_NULL);

    WiFi.onEvent(std::bind(&Network::sta_mode_connected, this, _1, _2), WiFiEvent_t::SYSTEM_EVENT_STA_CONNECTED);
    WiFi.onEvent(std::bind(&Network::sta_mode_disconnected, this, _1, _2), WiFiEvent_t::SYSTEM_EVENT_STA_DISCONNECTED);
    WiFi.onEvent(std::bind(&Network::sta_mode_got_ip, this, _1, _2), WiFiEvent_t::SYSTEM_EVENT_STA_GOT_IP);
    WiFi.onEvent(std::bind(&Network::sta_mode_start, this, _1, _2), WiFiEvent_t::SYSTEM_EVENT_STA_START);

    connect();   // connect to WiFi
    ota_setup(); // initialize OTA

#endif
}

#if defined(ESP32)
// set the ESP32 Wifi hostname. Didn't work during setup and I didn't want to use a delay().
// see https://github.com/espressif/arduino-esp32/blob/master/libraries/WiFi/examples/WiFiIPv6/WiFiIPv6.ino#L79
void Network::sta_mode_start(WiFiEvent_t event, WiFiEventInfo_t info) {
    WiFi.setHostname(Settings().hostname().c_str());
}
#endif

#if defined(ESP8266)
void Network::sta_mode_connected(const WiFiEventStationModeConnected & event) {
    LOG_INFO(F("Connected to %s (%02X:%02X:%02X:%02X:%02X:%02X) on channel %u"),
                 event.ssid.c_str(),
                 event.bssid[0],
                 event.bssid[1],
                 event.bssid[2],
                 event.bssid[3],
                 event.bssid[4],
                 event.bssid[5],
                 event.channel);

    // turn off safe mode
    System::save_safe_mode(false);
}
#elif defined(ESP32)
void Network::sta_mode_connected(WiFiEvent_t event, WiFiEventInfo_t info) {
    LOG_INFO(F("Connected to %s (%02X:%02X:%02X:%02X:%02X:%02X) on channel %u"),
                 info.connected.ssid,
                 info.sta_connected.mac[0],
                 info.sta_connected.mac[1],
                 info.sta_connected.mac[2],
                 info.sta_connected.mac[3],
                 info.sta_connected.mac[4],
                 info.sta_connected.mac[5],
                 info.connected.channel);

    // turn off safe mode
    System::save_safe_mode(false);
}
#endif

#if defined(ESP8266)
void Network::sta_mode_disconnected(const WiFiEventStationModeDisconnected & event) {
    // disconnect if failed to connect a few times, unless already in safe mode!
    if (event.reason == 201) {
        if (++disconnect_count_ == 3) {
            if (System::safe_mode()) {
                LOG_ERROR(F("Failed to connect to WiFi %s after %d attempts"), event.ssid.c_str(), disconnect_count_ - 1);
                disconnect_count_ = 0;
            } else {
                LOG_ERROR(F("Failed to connect to WiFi. Rebooting into Safe mode"));
                System::restart(true); // set safe mode and restart
            }
        }
    }
}
#elif defined(ESP32)
void Network::sta_mode_disconnected(WiFiEvent_t event, WiFiEventInfo_t info) {
    // LOG_ERROR(F("Failed to connect to WiFi %s, reason code %d"), info.disconnected.ssid, info.disconnected.reason);
}
#endif

#if defined(ESP8266)
void Network::sta_mode_got_ip(const WiFiEventStationModeGotIP & event) {
    LOG_INFO(F("Obtained IPv4 address %s/%s and gateway %s"),
                 uuid::printable_to_string(event.ip).c_str(),
                 uuid::printable_to_string(event.mask).c_str(),
                 uuid::printable_to_string(event.gw).c_str());
}
#elif defined(ESP32)
void Network::sta_mode_got_ip(WiFiEvent_t event, WiFiEventInfo_t info) {
    LOG_INFO(F("Obtained IPv4 address %s/%s and gateway %s"),
                 uuid::printable_to_string(IPAddress(info.got_ip.ip_info.ip.addr)).c_str(),
                 uuid::printable_to_string(IPAddress(info.got_ip.ip_info.netmask.addr)).c_str(),
                 uuid::printable_to_string(IPAddress(info.got_ip.ip_info.gw.addr)).c_str());
}
#endif

void Network::connect() {
    Settings settings;

    if (settings.wifi_ssid().empty()) {
        return;
    }

#ifndef EMSESP_STANDALONE

    // WiFi.mode(WIFI_STA);

    if (!settings.hostname().empty()) {
#if defined(ESP8266)
        WiFi.config(INADDR_ANY, INADDR_ANY, INADDR_ANY);
        WiFi.hostname(settings.hostname().c_str());
#elif defined(ESP32)
        WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE);
#endif
    }

    if (!settings.wifi_ssid().empty()) {
        WiFi.begin(settings.wifi_ssid().c_str(), settings.wifi_password().c_str());
    }

#endif
}

void Network::reconnect() {
    disconnect();
    connect();
}

void Network::disconnect() {
#if defined(ESP8266)
    WiFi.disconnect();
#elif defined(ESP32)
    WiFi.disconnect(true);
#endif
}

// OTA Setup
void Network::ota_setup() {
#ifndef EMSESP_STANDALONE

    if (ota_) {
        delete ota_;
        ota_ = nullptr;
    }

    ota_ = new ArduinoOTAClass;

    Settings settings;

    ota_->setPort(OTA_PORT);

    if (settings.hostname().empty()) {
        ota_->setHostname(EMSESP_DEFAULT_HOSTNAME);
    } else {
        ota_->setHostname(settings.hostname().c_str());
    }

    ota_->setPassword(settings.admin_password().c_str());

    ota_->onStart([this]() {
        LOG_DEBUG(F("OTA starting (send type %d)..."), ota_->getCommand());

        // turn off stuff to stop interference
        EMSuart::stop(); // UART stop
        in_ota_ = true;  // set flag so all other services stop
    });

    ota_->onEnd([this]() { LOG_DEBUG(F("OTA done, automatically restarting")); });

    ota_->onProgress([this](unsigned int progress, unsigned int total) {
        /*
        static unsigned int _progOld;
        unsigned int        _prog = (progress / (total / 100));
        if (_prog != _progOld) {
            LOG_DEBUG(F("[OTA] Progress: %u%%"), _prog);
            _progOld = _prog;
        }
        */
    });

    ota_->onError([this](ota_error_t error) {
        if (error == OTA_AUTH_ERROR) {
            LOG_ERROR(F("[OTA] Auth Failed"));
        } else if (error == OTA_BEGIN_ERROR) {
            LOG_ERROR(F("[OTA] Begin Failed"));
        } else if (error == OTA_CONNECT_ERROR) {
            LOG_ERROR(F("[OTA] Connect Failed"));
        } else if (error == OTA_RECEIVE_ERROR) {
            LOG_ERROR(F("[OTA] Receive Failed"));
        } else if (error == OTA_END_ERROR) {
            LOG_ERROR(F("[OTA] End Failed"));
        } else {
            LOG_ERROR(F("[OTA] Error %d"), error);
        };
    });

    // start ota service
    ota_->begin();
    LOG_INFO(F("Listening to firmware updates on %s.local:%u"), ota_->getHostname().c_str(), OTA_PORT);

#endif
}

// loop - mainly calls OTA
bool Network::loop() {
#ifndef EMSESP_STANDALONE
    if (ota_) {
        ota_->handle();
    }
#endif

    return !in_ota_;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

void Network::scan(uuid::console::Shell & shell) {
#ifndef EMSESP_STANDALONE
    int8_t ret = WiFi.scanNetworks(true);
    if (ret == WIFI_SCAN_RUNNING) {
        shell.println(F("Scanning for WiFi networks..."));

        shell.block_with([](uuid::console::Shell & shell, bool stop) -> bool {
            int8_t ret = WiFi.scanComplete();

            if (ret == WIFI_SCAN_RUNNING) {
                return stop;
            } else if (ret == WIFI_SCAN_FAILED || ret < 0) {
                shell.println(F("WiFi scan failed"));
                return true;
            } else {
                shell.printfln(F("Found %u networks:"), ret);
                shell.println();

                for (uint8_t i = 0; i < (uint8_t)ret; i++) {
                    shell.printfln(F(" %s (channel %u at %d dBm) %s"), WiFi.SSID(i).c_str(), WiFi.channel(i), WiFi.RSSI(i), WiFi.BSSIDstr(i).c_str());
                }
                shell.println();

                WiFi.scanDelete();
                return true;
            }
        });
    } else {
        shell.println(F("WiFi scan failed"));
    }
#endif
}

void Network::show_network(uuid::console::Shell & shell) {
#ifndef EMSESP_STANDALONE
    switch (WiFi.status()) {
    case WL_IDLE_STATUS:
        shell.printfln(F("WiFi: idle"));
        break;

    case WL_NO_SSID_AVAIL:
        shell.printfln(F("WiFi: network not found"));
        break;

    case WL_SCAN_COMPLETED:
        shell.printfln(F("WiFi: network scan complete"));
        break;

    case WL_CONNECTED: {
        shell.printfln(F("WiFi: connected"));
        shell.println();

        shell.printfln(F("SSID: %s"), WiFi.SSID().c_str());
        shell.printfln(F("BSSID: %s"), WiFi.BSSIDstr().c_str());
        shell.printfln(F("RSSI: %d dBm (%d %%)"), WiFi.RSSI(), wifi_quality());
        shell.println();

        shell.printfln(F("MAC address: %s"), WiFi.macAddress().c_str());
#if defined(ESP8266)
        shell.printfln(F("Hostname: %s"), WiFi.hostname().c_str());
#elif defined(ESP32)
        shell.printfln(F("Hostname: %s"), WiFi.getHostname());
#endif

        shell.println();
        shell.printfln(F("IPv4 address: %s/%s"), uuid::printable_to_string(WiFi.localIP()).c_str(), uuid::printable_to_string(WiFi.subnetMask()).c_str());
        shell.printfln(F("IPv4 gateway: %s"), uuid::printable_to_string(WiFi.gatewayIP()).c_str());
        shell.printfln(F("IPv4 nameserver: %s"), uuid::printable_to_string(WiFi.dnsIP()).c_str());
    } break;

    case WL_CONNECT_FAILED:
        shell.printfln(F("WiFi: connection failed"));
        break;

    case WL_CONNECTION_LOST:
        shell.printfln(F("WiFi: connection lost"));
        break;

    case WL_DISCONNECTED:
        shell.printfln(F("WiFi: disconnected"));
        break;

    case WL_NO_SHIELD:
    default:
        shell.printfln(F("WiFi: unknown"));
        break;
    }

    shell.println();
#endif
}

#pragma GCC diagnostic pop

// Return the quality (Received Signal Strength Indicator) of the WiFi network as a %. Or -1 if disconnected.
//  High quality: 90% ~= -55dBm
//  Medium quality: 50% ~= -75dBm
//  Low quality: 30% ~= -85dBm
//  Unusable quality: 8% ~= -96dBm
int8_t Network::wifi_quality() {
#ifndef EMSESP_STANDALONE
    if (WiFi.status() != WL_CONNECTED) {
        return -1;
    }
    int dBm = WiFi.RSSI();
#else
    int8_t dBm = -70;
#endif
    if (dBm <= -100) {
        return 0;
    }
    if (dBm >= -50) {
        return 100;
    }
    return 2 * (dBm + 100);
}

} // namespace emsesp
