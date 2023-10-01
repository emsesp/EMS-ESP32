/*
 * EMS-ESP - https://github.com/emsesp/EMS-ESP
 * Copyright 2020-2023  Paul Derbyshire
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

#include "emsesp.h"

using namespace std::placeholders; // for `_1` etc

namespace emsesp {

WebStatusService::WebStatusService(AsyncWebServer * server, SecurityManager * securityManager) {
    // rest endpoint for web page
    server->on(EMSESP_STATUS_SERVICE_PATH,
               HTTP_GET,
               securityManager->wrapRequest(std::bind(&WebStatusService::webStatusService, this, _1), AuthenticationPredicates::IS_AUTHENTICATED));
    WiFi.onEvent(std::bind(&WebStatusService::WiFiEvent, this, _1, _2));
}

// handles both WiFI and Ethernet
void WebStatusService::WiFiEvent(WiFiEvent_t event, WiFiEventInfo_t info) {
#ifndef EMSESP_STANDALONE

    switch (event) {
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
        EMSESP::logger().warning("WiFi disconnected. Reason code=%s", disconnectReason(info.wifi_sta_disconnected.reason)); // IDF 4.0
        WiFi.disconnect(true);
        break;

    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
        EMSESP::logger().info("WiFi connected with IP=%s, hostname=%s", WiFi.localIP().toString().c_str(), WiFi.getHostname());
        // EMSESP::system_.syslog_init();
        mDNS_start();
        EMSESP::system_.send_info_mqtt("connected");
        break;

    case ARDUINO_EVENT_ETH_START:
        // EMSESP::logger().info("Ethernet initialized");
        ETH.setHostname(EMSESP::system_.hostname().c_str());

        // configure for static IP
        EMSESP::esp8266React.getNetworkSettingsService()->read([&](NetworkSettings & networkSettings) {
            if (networkSettings.staticIPConfig) {
                ETH.config(networkSettings.localIP, networkSettings.gatewayIP, networkSettings.subnetMask, networkSettings.dnsIP1, networkSettings.dnsIP2);
            }
        });

        break;

    case ARDUINO_EVENT_ETH_GOT_IP:
        // prevent double calls
        if (!EMSESP::system_.ethernet_connected()) {
            EMSESP::logger().info("Ethernet connected with IP=%s, speed %d Mbps", ETH.localIP().toString().c_str(), ETH.linkSpeed());
            // EMSESP::system_.send_heartbeat();
            // EMSESP::system_.syslog_init();
            EMSESP::system_.ethernet_connected(true);
            mDNS_start();
            EMSESP::system_.send_info_mqtt("connected");
        }
        break;

    case ARDUINO_EVENT_ETH_DISCONNECTED:
        EMSESP::logger().warning("Ethernet disconnected");
        EMSESP::system_.ethernet_connected(false);
        break;

    case ARDUINO_EVENT_ETH_STOP:
        EMSESP::logger().info("Ethernet stopped");
        EMSESP::system_.ethernet_connected(false);
        break;

    case ARDUINO_EVENT_WIFI_STA_CONNECTED:
        EMSESP::esp8266React.getNetworkSettingsService()->read([&](NetworkSettings & networkSettings) {
            if (networkSettings.enableIPv6) {
                WiFi.enableIpV6();
            }
        });
        break;

    case ARDUINO_EVENT_ETH_CONNECTED:
        EMSESP::esp8266React.getNetworkSettingsService()->read([&](NetworkSettings & networkSettings) {
            if (networkSettings.enableIPv6) {
                ETH.enableIpV6();
            }
        });
        break;

    case ARDUINO_EVENT_WIFI_STA_GOT_IP6:
    case ARDUINO_EVENT_ETH_GOT_IP6:
        if (EMSESP::system_.ethernet_connected()) {
            EMSESP::logger().info("Ethernet connected with IPv6=%s, speed %d Mbps", ETH.localIPv6().toString().c_str(), ETH.linkSpeed());
        } else {
            EMSESP::logger().info("WiFi connected with IPv6=%s, hostname=%s", WiFi.localIPv6().toString().c_str(), WiFi.getHostname());
        }
        // EMSESP::system_.syslog_init();
        mDNS_start();
        EMSESP::system_.send_info_mqtt("connected");
        break;

    default:
        break;
    }
#endif
}

void WebStatusService::webStatusService(AsyncWebServerRequest * request) {
    auto *     response = new AsyncJsonResponse(false, EMSESP_JSON_SIZE_LARGE);
    JsonObject root     = response->getRoot();

    root["status"]      = EMSESP::bus_status(); // 0, 1 or 2
    root["tx_mode"]     = EMSESP::txservice_.tx_mode();
    root["uptime"]      = EMSbus::bus_uptime();
    root["num_devices"] = EMSESP::count_devices(); // excluding Controller
    root["num_sensors"] = EMSESP::temperaturesensor_.no_sensors();
    root["num_analogs"] = EMSESP::analogsensor_.no_sensors();

    JsonArray  statsJson = root.createNestedArray("stats");
    JsonObject statJson;

    statJson       = statsJson.createNestedObject();
    statJson["id"] = 0;
    statJson["s"]  = EMSESP::rxservice_.telegram_count();
    statJson["f"]  = EMSESP::rxservice_.telegram_error_count();
    statJson["q"]  = EMSESP::rxservice_.quality();

    statJson       = statsJson.createNestedObject();
    statJson["id"] = 1;
    statJson["s"]  = EMSESP::txservice_.telegram_read_count();
    statJson["f"]  = EMSESP::txservice_.telegram_read_fail_count();
    statJson["q"]  = EMSESP::txservice_.read_quality();

    statJson       = statsJson.createNestedObject();
    statJson["id"] = 2;
    statJson["s"]  = EMSESP::txservice_.telegram_write_count();
    statJson["f"]  = EMSESP::txservice_.telegram_write_fail_count();
    statJson["q"]  = EMSESP::txservice_.write_quality();

    if (EMSESP::sensor_enabled()) {
        statJson       = statsJson.createNestedObject();
        statJson["id"] = 3;
        statJson["s"]  = EMSESP::temperaturesensor_.reads();
        statJson["f"]  = EMSESP::temperaturesensor_.fails();
        statJson["q"] =
            EMSESP::temperaturesensor_.reads() == 0 ? 100 : 100 - (uint8_t)((100 * EMSESP::temperaturesensor_.fails()) / EMSESP::temperaturesensor_.reads());
    }
    if (EMSESP::analog_enabled()) {
        statJson       = statsJson.createNestedObject();
        statJson["id"] = 4;
        statJson["s"]  = EMSESP::analogsensor_.reads();
        statJson["f"]  = EMSESP::analogsensor_.fails();
        statJson["q"]  = EMSESP::analogsensor_.reads() == 0 ? 100 : 100 - (uint8_t)((100 * EMSESP::analogsensor_.fails()) / EMSESP::analogsensor_.reads());
    }
    if (Mqtt::enabled()) {
        statJson       = statsJson.createNestedObject();
        statJson["id"] = 5;
        statJson["s"]  = Mqtt::publish_count() - Mqtt::publish_fails();
        statJson["f"]  = Mqtt::publish_fails();
        statJson["q"]  = Mqtt::publish_count() == 0 ? 100 : 100 - (uint8_t)((100 * Mqtt::publish_fails()) / Mqtt::publish_count());
    }

    statJson       = statsJson.createNestedObject();
    statJson["id"] = 6;
    statJson["s"]  = WebAPIService::api_count(); // + WebAPIService::api_fails();
    statJson["f"]  = WebAPIService::api_fails();
    statJson["q"]  = (WebAPIService::api_count() + WebAPIService::api_fails()) == 0
                         ? 100
                         : 100 - (uint8_t)((100 * WebAPIService::api_fails()) / (WebAPIService::api_count() + WebAPIService::api_fails()));

#ifndef EMSESP_STANDALONE
    if (EMSESP::system_.syslog_enabled()) {
        statJson       = statsJson.createNestedObject();
        statJson["id"] = 7;
        statJson["s"]  = EMSESP::system_.syslog_count();
        statJson["f"]  = EMSESP::system_.syslog_fails();
        statJson["q"]  = (EMSESP::system_.syslog_count() + EMSESP::system_.syslog_fails()) == 0
                             ? 100
                             : 100 - (uint8_t)((100 * EMSESP::system_.syslog_fails()) / (EMSESP::system_.syslog_count() + EMSESP::system_.syslog_fails()));
    }
#endif

    response->setLength();
    request->send(response);
}

// start the multicast UDP service so EMS-ESP is discoverable via .local
void WebStatusService::mDNS_start() const {
#ifndef EMSESP_STANDALONE
    MDNS.end();
    EMSESP::esp8266React.getNetworkSettingsService()->read([&](NetworkSettings & networkSettings) {
        if (networkSettings.enableMDNS) {
            if (!MDNS.begin(EMSESP::system_.hostname().c_str())) {
                EMSESP::logger().warning("Failed to start mDNS responder service");
                return;
            }

            std::string address_s = EMSESP::system_.hostname() + ".local";

            MDNS.addService("http", "tcp", 80);   // add our web server and rest API
            MDNS.addService("telnet", "tcp", 23); // add our telnet console

            MDNS.addServiceTxt("http", "tcp", "version", EMSESP_APP_VERSION);
            MDNS.addServiceTxt("http", "tcp", "address", address_s.c_str());

            EMSESP::logger().info("mDNS responder service started");
        }
    });
#else
    EMSESP::esp8266React.getNetworkSettingsService()->read([&](NetworkSettings & networkSettings) {
        if (networkSettings.enableMDNS) {
            EMSESP::logger().info("mDNS responder service started");
        }
    });
#endif
}

const char * WebStatusService::disconnectReason(uint8_t code) {
#ifndef EMSESP_STANDALONE
    switch (code) {
    case WIFI_REASON_UNSPECIFIED: // = 1,
        return "unspecified";
    case WIFI_REASON_AUTH_EXPIRE: // = 2,
        return "auth expire";
    case WIFI_REASON_AUTH_LEAVE: // = 3,
        return "auth leave";
    case WIFI_REASON_ASSOC_EXPIRE: // = 4,
        return "assoc expired";
    case WIFI_REASON_ASSOC_TOOMANY: // = 5,
        return "assoc too many";
    case WIFI_REASON_NOT_AUTHED: // = 6,
        return "not authenticated";
    case WIFI_REASON_NOT_ASSOCED: // = 7,
        return "not assoc";
    case WIFI_REASON_ASSOC_LEAVE: // = 8,
        return "assoc leave";
    case WIFI_REASON_ASSOC_NOT_AUTHED: // = 9,
        return "assoc not authed";
    case WIFI_REASON_DISASSOC_PWRCAP_BAD: // = 10,
        return "disassoc powerCAP bad";
    case WIFI_REASON_DISASSOC_SUPCHAN_BAD: // = 11,
        return "disassoc supchan bad";
    case WIFI_REASON_IE_INVALID: // = 13,
        return "IE invalid";
    case WIFI_REASON_MIC_FAILURE: // = 14,
        return "MIC failure";
    case WIFI_REASON_4WAY_HANDSHAKE_TIMEOUT: // = 15,
        return "4way handshake timeout";
    case WIFI_REASON_GROUP_KEY_UPDATE_TIMEOUT: // = 16,
        return "group key-update timeout";
    case WIFI_REASON_IE_IN_4WAY_DIFFERS: // = 17,
        return "IE in 4way differs";
    case WIFI_REASON_GROUP_CIPHER_INVALID: // = 18,
        return "group cipher invalid";
    case WIFI_REASON_PAIRWISE_CIPHER_INVALID: // = 19,
        return "pairwise cipher invalid";
    case WIFI_REASON_AKMP_INVALID: // = 20,
        return "AKMP invalid";
    case WIFI_REASON_UNSUPP_RSN_IE_VERSION: // = 21,
        return "unsupported RSN_IE version";
    case WIFI_REASON_INVALID_RSN_IE_CAP: // = 22,
        return "invalid RSN_IE_CAP";
    case WIFI_REASON_802_1X_AUTH_FAILED: // = 23,
        return "802 X1 auth failed";
    case WIFI_REASON_CIPHER_SUITE_REJECTED: // = 24,
        return "cipher suite rejected";
    case WIFI_REASON_BEACON_TIMEOUT: // = 200,
        return "beacon timeout";
    case WIFI_REASON_NO_AP_FOUND: // = 201,
        return "no AP found";
    case WIFI_REASON_AUTH_FAIL: // = 202,
        return "auth fail";
    case WIFI_REASON_ASSOC_FAIL: // = 203,
        return "assoc fail";
    case WIFI_REASON_HANDSHAKE_TIMEOUT: // = 204,
        return "handshake timeout";
    default:
        return "unknown";
    }
#endif

    return "";
}

} // namespace emsesp