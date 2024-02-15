#include "APStatus.h"

APStatus::APStatus(AsyncWebServer * server, SecurityManager * securityManager, APSettingsService * apSettingsService)
    : _apSettingsService(apSettingsService) {
    server->on(AP_STATUS_SERVICE_PATH,
               HTTP_GET,
               securityManager->wrapRequest([this](AsyncWebServerRequest * request) { apStatus(request); }, AuthenticationPredicates::IS_AUTHENTICATED));
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
