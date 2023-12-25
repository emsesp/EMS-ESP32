#include <APStatus.h>

using namespace std::placeholders; // for `_1` etc

APStatus::APStatus(PsychicHttpServer * server, SecurityManager * securityManager, APSettingsService * apSettingsService)
    : _apSettingsService(apSettingsService)
    , _server(server)
    , _securityManager(securityManager) {
}

void APStatus::registerURI() {
    _server->on(AP_STATUS_SERVICE_PATH,
                HTTP_GET,
                _securityManager->wrapRequest(std::bind(&APStatus::apStatus, this, _1), AuthenticationPredicates::IS_AUTHENTICATED));
}

esp_err_t APStatus::apStatus(PsychicRequest * request) {
    PsychicJsonResponse response = PsychicJsonResponse(request, false, MAX_AP_STATUS_SIZE);
    JsonObject          root     = response.getRoot();

    root["status"]      = _apSettingsService->getAPNetworkStatus();
    root["ip_address"]  = WiFi.softAPIP().toString();
    root["mac_address"] = WiFi.softAPmacAddress();
    root["station_num"] = WiFi.softAPgetStationNum();

    return response.send();
}
