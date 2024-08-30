#include "RestartService.h"

#include <esp_ota_ops.h>

#include "../../src/emsesp_stub.hpp"

RestartService::RestartService(AsyncWebServer * server, SecurityManager * securityManager) {
    server->on(RESTART_SERVICE_PATH,
               HTTP_POST,
               securityManager->wrapRequest([this](AsyncWebServerRequest * request) { restart(request); }, AuthenticationPredicates::IS_ADMIN));
}

void RestartService::restartNow() {
    emsesp::EMSESP::system_.restart_requested(true); // will be handled by the main loop
}

void RestartService::restart(AsyncWebServerRequest * request) {
    request->onDisconnect(RestartService::restartNow);
    request->send(200);
}
