#include <RestartService.h>
#include <esp_ota_ops.h>

using namespace std::placeholders; // for `_1` etc

RestartService::RestartService(AsyncWebServer * server, SecurityManager * securityManager) {
    server->on(RESTART_SERVICE_PATH, HTTP_POST, securityManager->wrapRequest(std::bind(&RestartService::restart, this, _1), AuthenticationPredicates::IS_ADMIN));
    server->on(PARTITION_SERVICE_PATH,
               HTTP_POST,
               securityManager->wrapRequest(std::bind(&RestartService::partition, this, _1), AuthenticationPredicates::IS_ADMIN));
}

void RestartService::restart(AsyncWebServerRequest * request) {
    request->onDisconnect(RestartService::restartNow);
    request->send(200);
}

void RestartService::partition(AsyncWebServerRequest * request) {
    const esp_partition_t * factory_partition = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_FACTORY, NULL);
    if (!factory_partition) {
        request->send(400); // bad request
        return;
    }
    esp_ota_set_boot_partition(factory_partition);
    request->onDisconnect(RestartService::restartNow);
    request->send(200);
}
