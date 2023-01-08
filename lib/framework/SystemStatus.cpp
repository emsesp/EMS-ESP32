#include <SystemStatus.h>
#include <esp_ota_ops.h>

#include "../../src/emsesp_stub.hpp" // proddy added

using namespace std::placeholders; // for `_1` etc

SystemStatus::SystemStatus(AsyncWebServer * server, SecurityManager * securityManager) {
    server->on(SYSTEM_STATUS_SERVICE_PATH,
               HTTP_GET,
               securityManager->wrapRequest(std::bind(&SystemStatus::systemStatus, this, _1), AuthenticationPredicates::IS_AUTHENTICATED));
}

void SystemStatus::systemStatus(AsyncWebServerRequest * request) {
    AsyncJsonResponse * response = new AsyncJsonResponse(false, MAX_ESP_STATUS_SIZE);
    JsonObject          root     = response->getRoot();
    root["emsesp_version"]   = EMSESP_APP_VERSION;
    root["esp_platform"]     = EMSESP_PLATFORM;
    root["cpu_freq_mhz"]     = ESP.getCpuFreqMHz();
    root["max_alloc_heap"]   = ESP.getMaxAllocHeap() / 1024;
    root["free_heap"]        = ESP.getFreeHeap() / 1024;
    root["sdk_version"]      = ESP.getSdkVersion();
    root["flash_chip_size"]  = ESP.getFlashChipSize() / 1024;
    root["flash_chip_speed"] = ESP.getFlashChipSpeed();
    root["app_used"]         = emsesp::EMSESP::system_.appUsed();
    root["app_free"]         = emsesp::EMSESP::system_.appFree();
    uint32_t FSused          = LittleFS.usedBytes() / 1024;
    root["fs_used"]          = FSused;
    root["fs_free"]          = emsesp::EMSESP::system_.FStotal() - FSused;
    root["uptime"]           = uuid::log::format_timestamp_ms(uuid::get_uptime_ms(), 3);
    if (emsesp::EMSESP::system_.PSram()) {
        root["psram_size"] = emsesp::EMSESP::system_.PSram();
        root["free_psram"] = ESP.getFreePsram() / 1024;
    }
    const esp_partition_t * partition = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_FACTORY, NULL);
    if (partition != NULL) { // factory partition found
        root["has_loader"] = true;
    } else { // check for not empty, smaller OTA partition
        partition = esp_ota_get_next_update_partition(NULL);
        if (partition) {
            uint64_t buffer;
            esp_partition_read(partition, 0, &buffer, 8);
            const esp_partition_t * running = esp_ota_get_running_partition();
            root["has_loader"] = (buffer != 0xFFFFFFFFFFFFFFFF && running->size != partition->size);
        }
    }

    response->setLength();
    request->send(response);
}
