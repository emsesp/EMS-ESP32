#include <SystemStatus.h>

SystemStatus::SystemStatus(AsyncWebServer * server, SecurityManager * securityManager) {
    server->on(SYSTEM_STATUS_SERVICE_PATH,
               HTTP_GET,
               securityManager->wrapRequest(std::bind(&SystemStatus::systemStatus, this, std::placeholders::_1), AuthenticationPredicates::IS_AUTHENTICATED));
}

void SystemStatus::systemStatus(AsyncWebServerRequest * request) {
    AsyncJsonResponse * response = new AsyncJsonResponse(false, MAX_ESP_STATUS_SIZE);
    JsonObject          root     = response->getRoot();
#ifdef ESP32
    root["esp_platform"]   = "esp32";
    root["max_alloc_heap"] = ESP.getMaxAllocHeap();
    root["psram_size"]     = ESP.getPsramSize();
    root["free_psram"]     = ESP.getFreePsram();
#elif defined(ESP8266)
    root["esp_platform"]       = "esp8266";
    root["max_alloc_heap"]     = ESP.getMaxFreeBlockSize();
    root["heap_fragmentation"] = ESP.getHeapFragmentation();
#endif
    root["cpu_freq_mhz"]      = ESP.getCpuFreqMHz();
    root["free_heap"]         = ESP.getFreeHeap();
    root["sketch_size"]       = ESP.getSketchSize();
    root["free_sketch_space"] = ESP.getFreeSketchSpace();
    root["sdk_version"]       = ESP.getSdkVersion();
    root["flash_chip_size"]   = ESP.getFlashChipSize();
    root["flash_chip_speed"]  = ESP.getFlashChipSpeed();

// TODO - Ideally this class will take an *FS and extract the file system information from there.
// ESP8266 and ESP32 do not have feature parity in FS.h which currently makes that difficult.
#ifdef ESP32
    root["fs_total"] = SPIFFS.totalBytes();
    root["fs_used"]  = SPIFFS.usedBytes();
#elif defined(ESP8266)
    FSInfo fs_info;
    LittleFS.info(fs_info); // // proddy added
    root["fs_total"] = fs_info.totalBytes;
    root["fs_used"]  = fs_info.usedBytes;
#endif

    root["uptime"]   = uuid::log::format_timestamp_ms(uuid::get_uptime_ms(), 3); // proddy added
    root["free_mem"] = emsesp::System::free_mem(); // proddy added

    response->setLength();
    request->send(response);
}
