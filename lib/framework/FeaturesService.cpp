#include <FeaturesService.h>
#include "../../src/emsesp_stub.hpp"

FeaturesService::FeaturesService(PsychicHttpServer * server)
    : _server(server) {
}

void FeaturesService::registerURI() {
    // return feature set
    _server->on(FEATURES_SERVICE_PATH, HTTP_GET, [](PsychicRequest * request) {
        PsychicJsonResponse response = PsychicJsonResponse(request, false, 256);
        JsonObject          root     = response.getRoot();

        root["version"]  = EMSESP_APP_VERSION;
        root["platform"] = EMSESP_PLATFORM;

        return response.send();
    });
}
