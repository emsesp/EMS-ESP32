#include <RestartService.h>

using namespace std::placeholders; // for `_1` etc

RestartService::RestartService(AsyncWebServer * server, SecurityManager * securityManager) {
    server->on(RESTART_SERVICE_PATH, HTTP_POST, securityManager->wrapRequest(std::bind(&RestartService::restart, this, _1), AuthenticationPredicates::IS_ADMIN));
}

void RestartService::restart(AsyncWebServerRequest * request) {
    request->onDisconnect(RestartService::restartNow);
    request->send(200);
}
