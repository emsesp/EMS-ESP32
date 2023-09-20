#include <RestartService.h>
#include <esp_ota_ops.h>

#include "../../src/emsesp_stub.hpp"

using namespace std::placeholders; // for `_1` etc

RestartService::RestartService(AsyncWebServer * server, SecurityManager * securityManager) {
    server->on(RESTART_SERVICE_PATH, HTTP_POST, securityManager->wrapRequest(std::bind(&RestartService::restart, this, _1), AuthenticationPredicates::IS_ADMIN));
    server->on(PARTITION_SERVICE_PATH,
               HTTP_POST,
               securityManager->wrapRequest(std::bind(&RestartService::partition, this, _1), AuthenticationPredicates::IS_ADMIN));
}

void RestartService::restart(AsyncWebServerRequest * request) {
    emsesp::EMSESP::system_.store_nvs_values();
    request->onDisconnect(RestartService::restartNow);
    request->send(200);
}

void RestartService::partition(AsyncWebServerRequest * request) {
    const esp_partition_t * factory_partition = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_FACTORY, NULL);
    if (factory_partition) {
        esp_ota_set_boot_partition(factory_partition);
        emsesp::EMSESP::system_.store_nvs_values();
        request->onDisconnect(RestartService::restartNow);
        request->send(200);
        return;
    }
    const esp_partition_t * ota_partition = esp_ota_get_next_update_partition(NULL);
    if (!ota_partition) {
        request->send(400); // bad request
        return;
    }
    uint64_t buffer;
    esp_partition_read(ota_partition, 0, &buffer, 8);
    if (buffer == 0xFFFFFFFFFFFFFFFF) { // partition empty
        request->send(400);             // bad request
        return;
    }
    esp_ota_set_boot_partition(ota_partition);
    emsesp::EMSESP::system_.store_nvs_values();
    request->onDisconnect(RestartService::restartNow);
    request->send(200);
}
