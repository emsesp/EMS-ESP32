/*
 * EMS-ESP - https://github.com/emsesp/EMS-ESP
 * Copyright 2020-2024  Paul Derbyshire
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "emsesp.h"

#ifndef EMSESP_STANDALONE
#include <esp_ota_ops.h>
#endif

namespace emsesp {

// /rest/HardwareStatus
WebStatusService::WebStatusService(AsyncWebServer * server, SecurityManager * securityManager) {
    server->on(HARDWARE_STATUS_SERVICE_PATH, HTTP_GET, [this](AsyncWebServerRequest * request) { HardwareStatus(request); });
    server->on(SYSTEM_STATUS_SERVICE_PATH, HTTP_GET, [this](AsyncWebServerRequest * request) { systemStatus(request); });
}

void WebStatusService::systemStatus(AsyncWebServerRequest * request) {
    EMSESP::system_.refreshHeapMem(); // refresh free heap and max alloc heap

    auto *     response = new AsyncJsonResponse(false);
    JsonObject root     = response->getRoot();

#ifdef EMSESP_DEBUG
    root["emsesp_version"] = std::string(EMSESP_APP_VERSION) + " (DEBUG)";
#else
#ifdef EMSESP_TEST
    root["emsesp_version"] = std::string(EMSESP_APP_VERSION) + " (TEST)";
#else
    root["emsesp_version"] = EMSESP_APP_VERSION;
#endif
#endif

    root["esp_platform"] = EMSESP_PLATFORM;      // from default_settings.h: ESP32, ESP32-C3, ESP32-S2, ESP32-S3
    root["status"]       = EMSESP::bus_status(); // 0, 1 or 2
    root["bus_uptime"]   = EMSbus::bus_uptime();
    root["num_devices"]  = EMSESP::count_devices();
    root["num_sensors"]  = EMSESP::temperaturesensor_.no_sensors();
    root["num_analogs"]  = EMSESP::analogsensor_.no_sensors();
    root["free_heap"]    = EMSESP::system_.getHeapMem();
    root["uptime"]       = uuid::get_uptime_sec();
    root["mqtt_status"]  = EMSESP::mqtt_.connected();

#ifndef EMSESP_STANDALONE
    root["ntp_status"] = [] {
        if (esp_sntp_enabled()) {
            if (emsesp::EMSESP::system_.ntp_connected()) {
                return 2;
            } else {
                return 1;
            }
        }
        return 0;
    }();
#endif

    root["ap_status"] = EMSESP::esp8266React.apStatus();

    if (emsesp::EMSESP::system_.ethernet_connected()) {
        root["network_status"] = 10; // custom code #10 - ETHERNET_STATUS_CONNECTED
        root["wifi_rssi"]      = 0;
    } else {
        root["network_status"] = static_cast<uint8_t>(WiFi.status());
#ifndef EMSESP_STANDALONE
        root["wifi_rssi"] = WiFi.RSSI();
#endif
    }

    response->setLength();
    request->send(response);
}

void WebStatusService::HardwareStatus(AsyncWebServerRequest * request) {
    EMSESP::system_.refreshHeapMem(); // refresh free heap and max alloc heap

    auto *     response = new AsyncJsonResponse(false);
    JsonObject root     = response->getRoot();

#ifdef EMSESP_DEBUG
    root["emsesp_version"] = std::string(EMSESP_APP_VERSION) + " (DEBUG)";
#else
#ifdef EMSESP_TEST
    root["emsesp_version"] = std::string(EMSESP_APP_VERSION) + " (TEST)";
#else
    root["emsesp_version"] = EMSESP_APP_VERSION;
#endif
#endif
    root["esp_platform"] = EMSESP_PLATFORM;

#ifndef EMSESP_STANDALONE
    root["cpu_type"]         = ESP.getChipModel();
    root["cpu_rev"]          = ESP.getChipRevision();
    root["cpu_cores"]        = ESP.getChipCores();
    root["cpu_freq_mhz"]     = ESP.getCpuFreqMHz();
    root["max_alloc_heap"]   = EMSESP::system_.getMaxAllocMem();
    root["free_heap"]        = EMSESP::system_.getHeapMem();
    root["arduino_version"]  = ARDUINO_VERSION;
    root["sdk_version"]      = ESP.getSdkVersion();
    root["partition"]        = esp_ota_get_running_partition()->label;
    root["flash_chip_size"]  = ESP.getFlashChipSize() / 1024;
    root["flash_chip_speed"] = ESP.getFlashChipSpeed();
    root["app_used"]         = EMSESP::system_.appUsed();
    root["app_free"]         = EMSESP::system_.appFree();
    uint32_t FSused          = LittleFS.usedBytes() / 1024;
    root["fs_used"]          = FSused;
    root["fs_free"]          = EMSESP::system_.FStotal() - FSused;
    root["free_caps"]        = heap_caps_get_free_size(MALLOC_CAP_8BIT) / 1024; // includes heap and psram

    if (EMSESP::system_.PSram()) {
        root["psram_size"] = EMSESP::system_.PSram();
        root["free_psram"] = ESP.getFreePsram() / 1024;
    }

    const esp_partition_t * partition = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_FACTORY, nullptr);
    if (partition != NULL) { // factory partition found
        root["has_loader"] = true;
    } else { // check for not empty, smaller OTA partition
        partition = esp_ota_get_next_update_partition(nullptr);
        if (partition) {
            uint64_t buffer;
            esp_partition_read(partition, 0, &buffer, 8);
            const esp_partition_t * running = esp_ota_get_running_partition();
            root["has_loader"]              = (buffer != 0xFFFFFFFFFFFFFFFF && running->size != partition->size);
        }
    }

    root["model"] = EMSESP::system_.getBBQKeesGatewayDetails();

#endif

    response->setLength();
    request->send(response);
}

} // namespace emsesp
