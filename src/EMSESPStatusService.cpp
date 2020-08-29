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

#include "EMSESPStatusService.h"
#include "emsesp.h"
#include "mqtt.h"
#include "version.h"

namespace emsesp {

EMSESPStatusService::EMSESPStatusService(AsyncWebServer * server, SecurityManager * securityManager) {
    // rest endpoint for web page
    server->on(EMSESP_STATUS_SERVICE_PATH,
               HTTP_GET,
               securityManager->wrapRequest(std::bind(&EMSESPStatusService::emsespStatusService, this, std::placeholders::_1),
                                            AuthenticationPredicates::IS_AUTHENTICATED));

// trigger on wifi connects/disconnects
#ifdef ESP32
    WiFi.onEvent(onStationModeDisconnected, WiFiEvent_t::SYSTEM_EVENT_STA_DISCONNECTED);
    WiFi.onEvent(onStationModeGotIP, WiFiEvent_t::SYSTEM_EVENT_STA_GOT_IP);
#elif defined(ESP8266)
    _onStationModeDisconnectedHandler = WiFi.onStationModeDisconnected(onStationModeDisconnected);
    _onStationModeGotIPHandler        = WiFi.onStationModeGotIP(onStationModeGotIP);
#endif
}

#ifdef ESP32
void EMSESPStatusService::onStationModeDisconnected(WiFiEvent_t event, WiFiEventInfo_t info) {
    EMSESP::logger().debug(F("WiFi Disconnected. Reason code=%d"), info.disconnected.reason);
}
void EMSESPStatusService::onStationModeGotIP(WiFiEvent_t event, WiFiEventInfo_t info) {
    EMSESP::logger().debug(F("WiFi Connected with IP=%s, hostname=%s"), WiFi.localIP().toString().c_str(), WiFi.getHostname());
    EMSESP::system_.send_heartbeat(); // send out heartbeat MQTT as soon as we have a connection
}
#elif defined(ESP8266)
void EMSESPStatusService::onStationModeDisconnected(const WiFiEventStationModeDisconnected & event) {
    EMSESP::logger().debug(F("WiFi Disconnected. Reason code=%d"), event.reason);
}
void EMSESPStatusService::onStationModeGotIP(const WiFiEventStationModeGotIP & event) {
    EMSESP::logger().debug(F("WiFi Connected with IP=%s, hostname=%s"), event.ip.toString().c_str(), WiFi.hostname().c_str());
    EMSESP::system_.send_heartbeat(); // send out heartbeat MQTT as soon as we have a connection
}
#endif

void EMSESPStatusService::emsespStatusService(AsyncWebServerRequest * request) {
    AsyncJsonResponse * response = new AsyncJsonResponse(false, MAX_EMSESP_STATUS_SIZE);
    JsonObject          root     = response->getRoot();

    root["status"]      = EMSESP::bus_status(); // 0, 1 or 2
    root["rx_received"] = EMSESP::rxservice_.telegram_count();
    root["tx_sent"]     = EMSESP::txservice_.telegram_read_count() + EMSESP::txservice_.telegram_write_count();
    root["crc_errors"]  = EMSESP::rxservice_.telegram_error_count();
    root["tx_errors"]   = EMSESP::txservice_.telegram_fail_count();

    response->setLength();
    request->send(response);
}

} // namespace emsesp