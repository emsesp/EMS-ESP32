#include <WiFiScanner.h>

using namespace std::placeholders; // for `_1` etc

WiFiScanner::WiFiScanner(PsychicHttpServer * server, SecurityManager * securityManager)
    : _server(server)
    , _securityManager(securityManager){};

void WiFiScanner::registerURI() {
    _server->on(SCAN_NETWORKS_SERVICE_PATH,
                HTTP_GET,
                _securityManager->wrapRequest(std::bind(&WiFiScanner::scanNetworks, this, _1), AuthenticationPredicates::IS_ADMIN));
    _server->on(LIST_NETWORKS_SERVICE_PATH,
                HTTP_GET,
                _securityManager->wrapRequest(std::bind(&WiFiScanner::listNetworks, this, _1), AuthenticationPredicates::IS_ADMIN));
};


esp_err_t WiFiScanner::scanNetworks(PsychicRequest * request) {
    if (WiFi.scanComplete() != -1) {
        WiFi.scanDelete();
        WiFi.scanNetworks(true);
    }

    return request->reply(202); // special code to indicate scan in progress
}

esp_err_t WiFiScanner::listNetworks(PsychicRequest * request) {
    int numNetworks = WiFi.scanComplete();
    if (numNetworks > -1) {
        PsychicJsonResponse response = PsychicJsonResponse(request, false, MAX_WIFI_SCANNER_SIZE);
        JsonObject          root     = response.getRoot();

        JsonArray networks = root.createNestedArray("networks");
        for (int i = 0; i < numNetworks; i++) {
            JsonObject network         = networks.createNestedObject();
            network["rssi"]            = WiFi.RSSI(i);
            network["ssid"]            = WiFi.SSID(i);
            network["bssid"]           = WiFi.BSSIDstr(i);
            network["channel"]         = WiFi.channel(i);
            network["encryption_type"] = (uint8_t)WiFi.encryptionType(i);
        }

        return response.send();

    } else if (numNetworks == -1) {
        return request->reply(202); // special code to indicate scan in progress
    } else {
        return scanNetworks(request);
    }
}
