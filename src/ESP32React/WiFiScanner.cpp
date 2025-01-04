#include "WiFiScanner.h"

WiFiScanner::WiFiScanner(AsyncWebServer * server, SecurityManager * securityManager) {
    securityManager->addEndpoint(server, SCAN_NETWORKS_SERVICE_PATH, AuthenticationPredicates::IS_ADMIN, [this](AsyncWebServerRequest * request) {
        scanNetworks(request);
    });

    securityManager->addEndpoint(server, LIST_NETWORKS_SERVICE_PATH, AuthenticationPredicates::IS_ADMIN, [this](AsyncWebServerRequest * request) {
        listNetworks(request);
    });
};

void WiFiScanner::scanNetworks(AsyncWebServerRequest * request) {
    request->send(202); // special code to indicate scan in progress

    if (WiFi.scanComplete() != -1) {
        WiFi.scanDelete();
        WiFi.scanNetworks(true);
    }
}

void WiFiScanner::listNetworks(AsyncWebServerRequest * request) {
    const int numNetworks = WiFi.scanComplete();
    if (numNetworks > -1) {
        auto *     response = new AsyncJsonResponse(false);
        JsonObject root     = response->getRoot();
        JsonArray  networks = root["networks"].to<JsonArray>();
        for (uint8_t i = 0; i < numNetworks; i++) {
            JsonObject network         = networks.add<JsonObject>();
            network["rssi"]            = WiFi.RSSI(i);
            network["ssid"]            = WiFi.SSID(i);
            network["bssid"]           = WiFi.BSSIDstr(i);
            network["channel"]         = WiFi.channel(i);
            network["encryption_type"] = static_cast<uint8_t>(WiFi.encryptionType(i));
        }
        response->setLength();
        request->send(response);
    } else if (numNetworks == -1) {
        request->send(202); // special code to indicate scan in progress
    } else {
        scanNetworks(request);
    }
}
