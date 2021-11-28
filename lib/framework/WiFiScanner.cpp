#include <WiFiScanner.h>

using namespace std::placeholders; // for `_1` etc

WiFiScanner::WiFiScanner(AsyncWebServer * server, SecurityManager * securityManager) {
    server->on(SCAN_NETWORKS_SERVICE_PATH,
               HTTP_GET,
               securityManager->wrapRequest(std::bind(&WiFiScanner::scanNetworks, this, _1), AuthenticationPredicates::IS_ADMIN));
    server->on(LIST_NETWORKS_SERVICE_PATH,
               HTTP_GET,
               securityManager->wrapRequest(std::bind(&WiFiScanner::listNetworks, this, _1), AuthenticationPredicates::IS_ADMIN));
};

void WiFiScanner::scanNetworks(AsyncWebServerRequest * request) {
    if (WiFi.scanComplete() != -1) {
        WiFi.scanDelete();
        WiFi.scanNetworks(true);
    }
    request->send(202);
}

void WiFiScanner::listNetworks(AsyncWebServerRequest * request) {
    int numNetworks = WiFi.scanComplete();
    if (numNetworks > -1) {
        AsyncJsonResponse * response = new AsyncJsonResponse(false, MAX_WIFI_SCANNER_SIZE);
        JsonObject          root     = response->getRoot();
        JsonArray           networks = root.createNestedArray("networks");
        for (int i = 0; i < numNetworks; i++) {
            JsonObject network         = networks.createNestedObject();
            network["rssi"]            = WiFi.RSSI(i);
            network["ssid"]            = WiFi.SSID(i);
            network["bssid"]           = WiFi.BSSIDstr(i);
            network["channel"]         = WiFi.channel(i);
            network["encryption_type"] = (uint8_t)WiFi.encryptionType(i);
        }
        response->setLength();
        request->send(response);
    } else if (numNetworks == -1) {
        request->send(202);
    } else {
        scanNetworks(request);
    }
}
