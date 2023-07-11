#include <FeaturesService.h>
#include "../../src/emsesp_stub.hpp" // proddy added

using namespace std::placeholders;   // for `_1` etc

FeaturesService::FeaturesService(AsyncWebServer * server) {
    server->on(FEATURES_SERVICE_PATH, HTTP_GET, std::bind(&FeaturesService::features, this, _1));
}

void FeaturesService::features(AsyncWebServerRequest * request) {
    AsyncJsonResponse * response = new AsyncJsonResponse(false, MAX_FEATURES_SIZE);
    JsonObject          root     = response->getRoot();

    root["version"]  = EMSESP_APP_VERSION;
    root["platform"] = EMSESP_PLATFORM;

    response->setLength();
    request->send(response);
}
