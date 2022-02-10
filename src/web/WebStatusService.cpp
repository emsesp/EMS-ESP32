/*
 * EMS-ESP - https://github.com/emsesp/EMS-ESP
 * Copyright 2020  Paul Derbyshire
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
    switch (event) {
    case SYSTEM_EVENT_STA_DISCONNECTED:
        EMSESP::logger().info(F("WiFi disconnected. Reason code=%d"), info.disconnected.reason);
        WiFi.disconnect(true);
        break;

    case SYSTEM_EVENT_STA_GOT_IP:
#ifndef EMSESP_STANDALONE
        EMSESP::logger().info(F("WiFi connected with IP=%s, hostname=%s"), WiFi.localIP().toString().c_str(), WiFi.getHostname());
#endif
        EMSESP::esp8266React.getNetworkSettingsService()->read([&](NetworkSettings & networkSettings) {
            if (!networkSettings.enableIPv6) {
                EMSESP::system_.send_heartbeat();
                EMSESP::system_.syslog_init();
            }
        });
        mDNS_start();
        break;

    case SYSTEM_EVENT_ETH_START:
        // EMSESP::logger().info(F("Ethernet initialized"));
        ETH.setHostname(EMSESP::system_.hostname().c_str());

        // configure for static IP
        EMSESP::esp8266React.getNetworkSettingsService()->read([&](NetworkSettings & networkSettings) {
            if (networkSettings.staticIPConfig) {
                ETH.config(networkSettings.localIP, networkSettings.gatewayIP, networkSettings.subnetMask, networkSettings.dnsIP1, networkSettings.dnsIP2);
            }
        });

        break;

    case SYSTEM_EVENT_ETH_GOT_IP:
        // prevent double calls
        if (!EMSESP::system_.ethernet_connected()) {
#ifndef EMSESP_STANDALONE
            EMSESP::logger().info(F("Ethernet connected with IP=%s, speed %d Mbps"), ETH.localIP().toString().c_str(), ETH.linkSpeed());
#endif
            EMSESP::esp8266React.getNetworkSettingsService()->read([&](NetworkSettings & networkSettings) {
                if (!networkSettings.enableIPv6) {
                    EMSESP::system_.send_heartbeat();
                    EMSESP::system_.syslog_init();
                }
            });
            EMSESP::system_.ethernet_connected(true);
            mDNS_start();
        }
        break;

    case SYSTEM_EVENT_ETH_DISCONNECTED:
        EMSESP::logger().info(F("Ethernet disconnected"));
        EMSESP::system_.ethernet_connected(false);
        break;

    case SYSTEM_EVENT_ETH_STOP:
        EMSESP::logger().info(F("Ethernet stopped"));
        EMSESP::system_.ethernet_connected(false);
        break;

#ifndef EMSESP_STANDALONE
    case SYSTEM_EVENT_STA_CONNECTED:
        EMSESP::esp8266React.getNetworkSettingsService()->read([&](NetworkSettings & networkSettings) {
            if (networkSettings.enableIPv6) {
                WiFi.enableIpV6();
            }
        });
        break;

    case SYSTEM_EVENT_ETH_CONNECTED:
        EMSESP::esp8266React.getNetworkSettingsService()->read([&](NetworkSettings & networkSettings) {
            if (networkSettings.enableIPv6) {
                ETH.enableIpV6();
            }
        });
        break;

    case SYSTEM_EVENT_GOT_IP6:
        if (EMSESP::system_.ethernet_connected()) {
            EMSESP::logger().info(F("Ethernet connected with IP=%s, speed %d Mbps"), ETH.localIPv6().toString().c_str(), ETH.linkSpeed());
        } else {
            EMSESP::logger().info(F("WiFi connected with IP=%s, hostname=%s"), WiFi.localIPv6().toString().c_str(), WiFi.getHostname());
        }
        EMSESP::system_.send_heartbeat();
        EMSESP::system_.syslog_init();
        mDNS_start();
        break;
#endif

    default:
        break;
    }
}

void WebStatusService::webStatusService(AsyncWebServerRequest * request) {
    AsyncJsonResponse * response = new AsyncJsonResponse(false, EMSESP_JSON_SIZE_MEDIUM_DYN);
    JsonObject          root     = response->getRoot();

    root["status"]           = EMSESP::bus_status();    // 0, 1 or 2
    root["num_devices"]      = EMSESP::count_devices(); // excluding Controller
    root["num_sensors"]      = EMSESP::dallassensor_.no_sensors();
    root["num_analogs"]      = EMSESP::analogsensor_.no_sensors();
    root["tx_mode"]          = EMSESP::txservice_.tx_mode();
    root["rx_received"]      = EMSESP::rxservice_.telegram_count();
    root["tx_reads"]         = EMSESP::txservice_.telegram_read_count();
    root["tx_writes"]        = EMSESP::txservice_.telegram_write_count();
    root["rx_quality"]       = EMSESP::rxservice_.quality();
    root["tx_read_quality"]  = EMSESP::txservice_.read_quality();
    root["tx_write_quality"] = EMSESP::txservice_.write_quality();
    root["rx_fails"]         = EMSESP::rxservice_.telegram_error_count();
    root["tx_read_fails"]    = EMSESP::txservice_.telegram_read_fail_count();
    root["tx_write_fails"]   = EMSESP::txservice_.telegram_write_fail_count();
    root["sensor_fails"]     = EMSESP::dallassensor_.fails();
    root["sensor_reads"]     = EMSESP::dallassensor_.reads();
    root["sensor_quality"] = EMSESP::dallassensor_.reads() == 0 ? 100 : 100 - (uint8_t)((100 * EMSESP::dallassensor_.fails()) / EMSESP::dallassensor_.reads());
    root["analog_fails"]   = EMSESP::analogsensor_.fails();
    root["analog_reads"]   = EMSESP::analogsensor_.reads();
    root["analog_quality"] = EMSESP::analogsensor_.reads() == 0 ? 100 : 100 - (uint8_t)((100 * EMSESP::analogsensor_.fails()) / EMSESP::analogsensor_.reads());
    root["mqtt_fails"]     = Mqtt::publish_fails();
    root["mqtt_count"]     = Mqtt::publish_count();
    root["mqtt_quality"]   = Mqtt::publish_count() == 0 ? 100 : 100 - (Mqtt::publish_fails() * 100) / (Mqtt::publish_count() + Mqtt::publish_fails());
    root["api_calls"]      = WebAPIService::api_count(); // + WebAPIService::api_fails();
    root["api_fails"]      = WebAPIService::api_fails();
    root["api_quality"] =
        WebAPIService::api_count() == 0 ? 100 : 100 - (WebAPIService::api_fails() * 100) / (WebAPIService::api_count() + WebAPIService::api_fails());
    root["uptime"] = EMSbus::bus_uptime();

    response->setLength();
    request->send(response);
}

// start the multicast UDP service so EMS-ESP is discoverable via .local
void WebStatusService::mDNS_start() {
#ifndef EMSESP_STANDALONE
    if (!MDNS.begin(EMSESP::system_.hostname().c_str())) {
        EMSESP::logger().warning(F("Failed to start mDNS responder service"));
        return;
    }

    std::string address_s = EMSESP::system_.hostname() + ".local";

    MDNS.addService("http", "tcp", 80);   // add our web server and rest API
    MDNS.addService("telnet", "tcp", 23); // add our telnet console

    MDNS.addServiceTxt("http", "tcp", "version", EMSESP_APP_VERSION);
    MDNS.addServiceTxt("http", "tcp", "address", address_s.c_str());
#endif

    EMSESP::logger().info(F("mDNS responder service started"));
}

} // namespace emsesp