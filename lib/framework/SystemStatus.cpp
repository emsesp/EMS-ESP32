#include <SystemStatus.h>
#include <esp_ota_ops.h>

#include "../../src/emsesp_stub.hpp"

using namespace std::placeholders; // for `_1` etc

SystemStatus::SystemStatus(PsychicHttpServer * server, SecurityManager * securityManager)
    : _server(server)
    , _securityManager(securityManager) {
}

void SystemStatus::registerURI() {
    emsesp::EMSESP::system_.refreshHeapMem(); // refresh free heap and max alloc heap

    _server->on(SYSTEM_STATUS_SERVICE_PATH,
                HTTP_GET,
                _securityManager->wrapRequest(std::bind(&SystemStatus::systemStatus, this, _1), AuthenticationPredicates::IS_AUTHENTICATED));
}

esp_err_t SystemStatus::systemStatus(PsychicRequest * request) {
    PsychicJsonResponse response = PsychicJsonResponse(request, false, MAX_ESP_STATUS_SIZE);
    JsonObject          root     = response.getRoot();

    root["emsesp_version"]   = EMSESP_APP_VERSION;
    root["esp_platform"]     = EMSESP_PLATFORM;
    root["cpu_type"]         = ESP.getChipModel();
    root["cpu_rev"]          = ESP.getChipRevision();
    root["cpu_cores"]        = ESP.getChipCores();
    root["cpu_freq_mhz"]     = ESP.getCpuFreqMHz();
    root["max_alloc_heap"]   = emsesp::EMSESP::system_.getMaxAllocMem();
    root["free_heap"]        = emsesp::EMSESP::system_.getHeapMem();
    root["arduino_version"]  = ARDUINO_VERSION;
    root["sdk_version"]      = ESP.getSdkVersion();
    root["partition"]        = esp_ota_get_running_partition()->label;
    root["flash_chip_size"]  = ESP.getFlashChipSize() / 1024;
    root["flash_chip_speed"] = ESP.getFlashChipSpeed();
    root["app_used"]         = emsesp::EMSESP::system_.appUsed();
    root["app_free"]         = emsesp::EMSESP::system_.appFree();
    uint32_t FSused          = LittleFS.usedBytes() / 1024;
    root["fs_used"]          = FSused;
    root["fs_free"]          = emsesp::EMSESP::system_.FStotal() - FSused;
    root["uptime"]           = uuid::log::format_timestamp_ms(uuid::get_uptime_ms(), 3);

    // TODO add buid time with __TIME__ and __DATE__

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
            root["has_loader"]              = (buffer != 0xFFFFFFFFFFFFFFFF && running->size != partition->size);
        }
    }

    return response.send();
}
