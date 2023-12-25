#include <RestartService.h>
#include <esp_ota_ops.h>

#include "../../src/emsesp_stub.hpp"

using namespace std::placeholders; // for `_1` etc

RestartService::RestartService(PsychicHttpServer * server, SecurityManager * securityManager)
    : _server(server)
    , _securityManager(securityManager) {
}

void RestartService::registerURI() {
    _server->on(RESTART_SERVICE_PATH, HTTP_POST, _securityManager->wrapRequest(std::bind(&RestartService::restart, this, _1), AuthenticationPredicates::IS_ADMIN));
    _server->on(PARTITION_SERVICE_PATH,
                HTTP_POST,
                _securityManager->wrapRequest(std::bind(&RestartService::partition, this, _1), AuthenticationPredicates::IS_ADMIN));
}

esp_err_t RestartService::restart(PsychicRequest * request) {
    emsesp::EMSESP::system_.store_nvs_values();
    request->reply(200);
    restartNow();
    return ESP_OK;
}

esp_err_t RestartService::partition(PsychicRequest * request) {
    const esp_partition_t * factory_partition = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_FACTORY, NULL);
    if (factory_partition) {
        esp_ota_set_boot_partition(factory_partition);
        emsesp::EMSESP::system_.store_nvs_values();
        request->reply(200);
        restartNow();
        return ESP_OK;
    }

    const esp_partition_t * ota_partition = esp_ota_get_next_update_partition(NULL);
    if (!ota_partition) {
        return request->reply(400); // bad request
    }

    uint64_t buffer;
    esp_partition_read(ota_partition, 0, &buffer, 8);
    if (buffer == 0xFFFFFFFFFFFFFFFF) { // partition empty
        return request->reply(400);     // bad request
    }

    esp_ota_set_boot_partition(ota_partition);
    emsesp::EMSESP::system_.store_nvs_values();
    request->reply(200);
    restartNow();
    return ESP_OK;
}
