#include <WiFiStatus.h>

WiFiStatus::WiFiStatus(AsyncWebServer* server, SecurityManager* securityManager) {
  server->on(WIFI_STATUS_SERVICE_PATH,
             HTTP_GET,
             securityManager->wrapRequest(std::bind(&WiFiStatus::wifiStatus, this, std::placeholders::_1),
                                          AuthenticationPredicates::IS_AUTHENTICATED));
#ifdef ESP32
  WiFi.onEvent(onStationModeConnected, WiFiEvent_t::SYSTEM_EVENT_STA_CONNECTED);
  WiFi.onEvent(onStationModeDisconnected, WiFiEvent_t::SYSTEM_EVENT_STA_DISCONNECTED);
  WiFi.onEvent(onStationModeGotIP, WiFiEvent_t::SYSTEM_EVENT_STA_GOT_IP);
#elif defined(ESP8266)
  _onStationModeConnectedHandler = WiFi.onStationModeConnected(onStationModeConnected);
  _onStationModeDisconnectedHandler = WiFi.onStationModeDisconnected(onStationModeDisconnected);
  _onStationModeGotIPHandler = WiFi.onStationModeGotIP(onStationModeGotIP);
#endif
}

#ifdef ESP32
void WiFiStatus::onStationModeConnected(WiFiEvent_t event, WiFiEventInfo_t info) {
  Serial.println(F("WiFi Connected."));
}

void WiFiStatus::onStationModeDisconnected(WiFiEvent_t event, WiFiEventInfo_t info) {
  Serial.print(F("WiFi Disconnected. Reason code="));
  Serial.println(info.disconnected.reason);
}

void WiFiStatus::onStationModeGotIP(WiFiEvent_t event, WiFiEventInfo_t info) {
  Serial.printf_P(
      PSTR("WiFi Got IP. localIP=%s, hostName=%s\r\n"), WiFi.localIP().toString().c_str(), WiFi.getHostname());
}
#elif defined(ESP8266)
void WiFiStatus::onStationModeConnected(const WiFiEventStationModeConnected& event) {
  Serial.print(F("WiFi Connected. SSID="));
  Serial.println(event.ssid);
}

void WiFiStatus::onStationModeDisconnected(const WiFiEventStationModeDisconnected& event) {
  Serial.print(F("WiFi Disconnected. Reason code="));
  Serial.println(event.reason);
}

void WiFiStatus::onStationModeGotIP(const WiFiEventStationModeGotIP& event) {
  Serial.printf_P(
      PSTR("WiFi Got IP. localIP=%s, hostName=%s\r\n"), event.ip.toString().c_str(), WiFi.hostname().c_str());
}
#endif

void WiFiStatus::wifiStatus(AsyncWebServerRequest* request) {
  AsyncJsonResponse* response = new AsyncJsonResponse(false, MAX_WIFI_STATUS_SIZE);
  JsonObject root = response->getRoot();
  wl_status_t status = WiFi.status();
  root["status"] = (uint8_t)status;
  if (status == WL_CONNECTED) {
    root["local_ip"] = WiFi.localIP().toString();
    root["mac_address"] = WiFi.macAddress();
    root["rssi"] = WiFi.RSSI();
    root["ssid"] = WiFi.SSID();
    root["bssid"] = WiFi.BSSIDstr();
    root["channel"] = WiFi.channel();
    root["subnet_mask"] = WiFi.subnetMask().toString();
    root["gateway_ip"] = WiFi.gatewayIP().toString();
    IPAddress dnsIP1 = WiFi.dnsIP(0);
    IPAddress dnsIP2 = WiFi.dnsIP(1);
    if (dnsIP1 != INADDR_NONE) {
      root["dns_ip_1"] = dnsIP1.toString();
    }
    if (dnsIP2 != INADDR_NONE) {
      root["dns_ip_2"] = dnsIP2.toString();
    }
  }
  response->setLength();
  request->send(response);
}
