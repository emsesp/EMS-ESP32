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

// trigger on wifi connects
#ifdef ESP32
    WiFi.onEvent(onStationModeConnected, WiFiEvent_t::SYSTEM_EVENT_STA_CONNECTED);
    WiFi.onEvent(onStationModeDisconnected, WiFiEvent_t::SYSTEM_EVENT_STA_DISCONNECTED);
    WiFi.onEvent(onStationModeGotIP, WiFiEvent_t::SYSTEM_EVENT_STA_GOT_IP);
#elif defined(ESP8266)
    _onStationModeConnectedHandler    = WiFi.onStationModeConnected(onStationModeConnected);
    _onStationModeDisconnectedHandler = WiFi.onStationModeDisconnected(onStationModeDisconnected);
    _onStationModeGotIPHandler        = WiFi.onStationModeGotIP(onStationModeGotIP);
#endif
}

#ifdef ESP32
void EMSESPStatusService::onStationModeConnected(WiFiEvent_t event, WiFiEventInfo_t info) {
    EMSESP::logger().debug(F("Wifi Connected"));
}

void EMSESPStatusService::onStationModeDisconnected(WiFiEvent_t event, WiFiEventInfo_t info) {
    EMSESP::logger().debug(F("WiFi Disconnected. Reason code=%d"), info.disconnected.reason);
}

void EMSESPStatusService::onStationModeGotIP(WiFiEvent_t event, WiFiEventInfo_t info) {
    EMSESP::logger().debug(F("WiFi connected with IP=%s, hostname=%s"), WiFi.localIP().toString().c_str(), WiFi.getHostname());
}
#elif defined(ESP8266)
void EMSESPStatusService::onStationModeConnected(const WiFiEventStationModeConnected & event) {
    EMSESP::logger().debug(F("Wifi connected with SSID %s"), event.ssid.c_str());
}

void EMSESPStatusService::onStationModeDisconnected(const WiFiEventStationModeDisconnected & event) {
    EMSESP::logger().debug(F("WiFi Disconnected. Reason code=%d"), event.reason);
}

void EMSESPStatusService::onStationModeGotIP(const WiFiEventStationModeGotIP & event) {
    EMSESP::logger().debug(F("WiFi connected with IP=%s, hostname=%s"), event.ip.toString().c_str(), WiFi.hostname().c_str());
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