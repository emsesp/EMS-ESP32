#include "APStatus.h"

#include <emsesp.h>

APStatus::APStatus(AsyncWebServer * server, SecurityManager * securityManager, APSettingsService * apSettingsService)
    : _apSettingsService(apSettingsService) {
    securityManager->addEndpoint(server, AP_STATUS_SERVICE_PATH, AuthenticationPredicates::IS_AUTHENTICATED, [this](AsyncWebServerRequest * request) {
        apStatus(request);
    });
}

void APStatus::apStatus(AsyncWebServerRequest * request) {
    auto *     response = new emsesp::PsramAsyncJsonResponse(false);
    JsonObject root     = response->getRoot();

    root["status"]      = _apSettingsService->getAPNetworkStatus();
    root["ip_address"]  = emsesp::EMSESP::network_.getAPIP();
    root["mac_address"] = emsesp::EMSESP::network_.getAPMacAddress();
    root["station_num"] = emsesp::EMSESP::network_.getAPStationNum();

    response->setLength();
    request->send(response);
}
