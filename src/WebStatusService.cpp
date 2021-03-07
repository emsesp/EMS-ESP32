/*
 * EMS-ESP - https://github.com/proddy/EMS-ESP
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

namespace emsesp {

WebStatusService::WebStatusService(AsyncWebServer * server, SecurityManager * securityManager) {
    // rest endpoint for web page
    server->on(EMSESP_STATUS_SERVICE_PATH, HTTP_GET, securityManager->wrapRequest(std::bind(&WebStatusService::webStatusService, this, std::placeholders::_1), AuthenticationPredicates::IS_AUTHENTICATED));
    WiFi.onEvent(std::bind(&WebStatusService::WiFiEvent, this, std::placeholders::_1, std::placeholders::_2));
}

// handles both WiFI and Ethernet
void WebStatusService::WiFiEvent(WiFiEvent_t event, WiFiEventInfo_t info) {
    switch (event) {
    case SYSTEM_EVENT_STA_DISCONNECTED:
        EMSESP::logger().info(F("WiFi Disconnected. Reason code=%d"), info.disconnected.reason);
        break;

    case SYSTEM_EVENT_STA_GOT_IP:
#ifndef EMSESP_STANDALONE
        EMSESP::logger().info(F("WiFi Connected with IP=%s, hostname=%s"), WiFi.localIP().toString().c_str(), WiFi.getHostname());
#endif
        EMSESP::system_.wifi_tweak();
        EMSESP::system_.send_heartbeat();
        break;

    case SYSTEM_EVENT_ETH_START:
        EMSESP::logger().info(F("Ethernet Started"));
        ETH.setHostname(EMSESP::system_.hostname().c_str());
        break;

    case SYSTEM_EVENT_ETH_GOT_IP:
        // prevent double calls
        if (!EMSESP::system_.ethernet_connected()) {
#ifndef EMSESP_STANDALONE
            EMSESP::logger().info(F("Ethernet Connected with IP=%s, speed %d Mbps"), ETH.localIP().toString().c_str(), ETH.linkSpeed());
#endif
            EMSESP::system_.send_heartbeat();
            EMSESP::system_.ethernet_connected(true);
        }
        break;

    case SYSTEM_EVENT_ETH_DISCONNECTED:
        EMSESP::logger().info(F("Ethernet Disconnected"));
        EMSESP::system_.ethernet_connected(false);
        break;

    case SYSTEM_EVENT_ETH_STOP:
        EMSESP::logger().info(F("Ethernet Stopped"));
        EMSESP::system_.ethernet_connected(false);
        break;

    default:
        break;
    }
}

void WebStatusService::webStatusService(AsyncWebServerRequest * request) {
    AsyncJsonResponse * response = new AsyncJsonResponse(false, EMSESP_JSON_SIZE_MEDIUM_DYN);
    JsonObject          root     = response->getRoot();

    root["status"]      = EMSESP::bus_status(); // 0, 1 or 2
    root["rx_received"] = EMSESP::rxservice_.telegram_count();
    root["tx_sent"]     = EMSESP::txservice_.telegram_read_count() + EMSESP::txservice_.telegram_write_count();
    root["rx_quality"]  = EMSESP::rxservice_.quality();
    root["tx_quality"]  = EMSESP::txservice_.quality();

    response->setLength();
    request->send(response);
}

} // namespace emsesp