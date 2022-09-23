#include <SystemStatus.h>

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
    root["emsesp_version"]       = EMSESP_APP_VERSION;
    root["esp_platform"]         = "ESP32";
    root["max_alloc_heap"]       = ESP.getMaxAllocHeap() / 1024;
#if defined(BOARD_HAS_PSRAM)
    root["psram_size"] = ESP.getPsramSize() / 1024;
    root["free_psram"] = ESP.getFreePsram() / 1024;
#endif
    root["cpu_freq_mhz"]     = ESP.getCpuFreqMHz();
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

    response->setLength();
    request->send(response);
}
