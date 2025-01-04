#include "APStatus.h"

APStatus::APStatus(AsyncWebServer * server, SecurityManager * securityManager, APSettingsService * apSettingsService)
    : _apSettingsService(apSettingsService) {
    securityManager->addEndpoint(server, AP_STATUS_SERVICE_PATH, AuthenticationPredicates::IS_AUTHENTICATED, [this](AsyncWebServerRequest * request) {
        apStatus(request);
    });
}

void APStatus::apStatus(AsyncWebServerRequest * request) {
    auto *     response = new AsyncJsonResponse(false);
    JsonObject root     = response->getRoot();

    root["status"]      = _apSettingsService->getAPNetworkStatus();
    root["ip_address"]  = WiFi.softAPIP().toString();
    root["mac_address"] = WiFi.softAPmacAddress();
    root["station_num"] = WiFi.softAPgetStationNum();

    response->setLength();
    request->send(response);
}
