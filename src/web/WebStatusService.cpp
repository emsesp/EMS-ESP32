/*
 * EMS-ESP - https://github.com/emsesp/EMS-ESP
 * Copyright 2020-2024  emsesp.org - proddy, MichaelDvP
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

WebStatusService::WebStatusService(AsyncWebServer * server, SecurityManager * securityManager)
    : _securityManager(securityManager) {
    // GET
    server->on(EMSESP_SYSTEM_STATUS_SERVICE_PATH, HTTP_GET, [this](AsyncWebServerRequest * request) { systemStatus(request); });

    // POST - generic action handler
    server->on(EMSESP_ACTION_SERVICE_PATH, [this](AsyncWebServerRequest * request, JsonVariant json) { action(request, json); });
}

// /rest/systemStatus
// This contains both system & hardware Status to avoid having multiple costly endpoints
// This is also used for polling during the RestartMonitor to see if EMS-ESP is alive
void WebStatusService::systemStatus(AsyncWebServerRequest * request) {
    EMSESP::system_.refreshHeapMem(); // refresh free heap and max alloc heap

    auto *     response = new AsyncJsonResponse(false);
    JsonObject root     = response->getRoot();

    root["emsesp_version"] = EMSESP_APP_VERSION;

    //
    // System Status
    //
    root["emsesp_version"] = EMSESP_APP_VERSION;
    root["bus_status"]     = EMSESP::bus_status(); // 0, 1 or 2
    root["bus_uptime"]     = EMSbus::bus_uptime();
    root["num_devices"]    = EMSESP::count_devices();
    root["num_sensors"]    = EMSESP::temperaturesensor_.count_entities();
    root["num_analogs"]    = EMSESP::analogsensor_.count_entities();
    root["free_heap"]      = EMSESP::system_.getHeapMem();
    root["uptime"]         = uuid::get_uptime_sec();
    root["mqtt_status"]    = EMSESP::mqtt_.connected();

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

#if defined(EMSESP_DEBUG)
#ifdef EMSESP_TEST
    root["build_flags"] = "DEBUG,TEST";
#else
    root["build_flags"] = "DEBUG";
#endif
#elif defined(EMSESP_TEST)
    root["build_flags"] = "TEST";
#endif

    //
    // Hardware Status
    //
    root["esp_platform"] = EMSESP_PLATFORM;
#ifndef EMSESP_STANDALONE
    root["cpu_type"]         = ESP.getChipModel();
    root["cpu_rev"]          = ESP.getChipRevision();
    root["cpu_cores"]        = ESP.getChipCores();
    root["cpu_freq_mhz"]     = ESP.getCpuFreqMHz();
    root["max_alloc_heap"]   = EMSESP::system_.getMaxAllocMem();
    root["arduino_version"]  = ARDUINO_VERSION;
    root["sdk_version"]      = ESP.getSdkVersion();
    root["partition"]        = esp_ota_get_running_partition()->label; // active partition
    root["flash_chip_size"]  = ESP.getFlashChipSize() / 1024;
    root["flash_chip_speed"] = ESP.getFlashChipSpeed();
    root["app_used"]         = EMSESP::system_.appUsed();
    root["app_free"]         = EMSESP::system_.appFree();
    uint32_t FSused          = LittleFS.usedBytes() / 1024;
    root["fs_used"]          = FSused;
    root["fs_free"]          = EMSESP::system_.FStotal() - FSused;
    root["free_caps"]        = heap_caps_get_free_size(MALLOC_CAP_8BIT) / 1024; // includes heap and psram
    root["psram"]            = (EMSESP::system_.PSram() > 0);                   // boolean
    if (EMSESP::system_.PSram()) {
        root["psram_size"] = EMSESP::system_.PSram();
        root["free_psram"] = ESP.getFreePsram() / 1024;
    }
    root["model"] = EMSESP::system_.getBBQKeesGatewayDetails();
#if CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32S2
    root["temperature"] = EMSESP::system_.temperature();
#endif

    // check for a factory partition first
    const esp_partition_t * partition = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_FACTORY, nullptr);
    root["has_loader"]                = partition != NULL && partition != esp_ota_get_running_partition();
    partition                         = esp_ota_get_next_update_partition(nullptr);
    if (partition) {
        uint64_t buffer;
        esp_partition_read(partition, 0, &buffer, 8);
        root["has_partition"] = (buffer != 0xFFFFFFFFFFFFFFFF);
    } else {
        root["has_partition"] = false;
    }

    // Matches status codes in RestartMonitor.tsx
    if (EMSESP::system_.restart_pending()) {
        root["status"] = "restarting";
        EMSESP::system_.restart_requested(true); // tell emsesp loop to start restart
    } else {
        root["status"] = EMSESP::system_.upload_isrunning() ? "uploading" : "ready";
    }

#endif

    response->setLength();
    request->send(response);
}

// generic action handler - as a POST
void WebStatusService::action(AsyncWebServerRequest * request, JsonVariant json) {
    auto *     response = new AsyncJsonResponse();
    JsonObject root     = response->getRoot();

    // param is optional - https://arduinojson.org/news/2024/09/18/arduinojson-7-2/
    std::string param;
    bool        has_param      = false;
    JsonVariant param_optional = json["param"];
    if (json["param"].is<const char *>()) {
        param     = param_optional.as<std::string>();
        has_param = true;
    } else {
        has_param = false;
    }

    // check if we're authenticated for admin tasks, some actions are only for admins
    Authentication authentication = _securityManager->authenticateRequest(request);
    bool           is_admin       = AuthenticationPredicates::IS_ADMIN(authentication);

    // call action command
    bool        ok     = false;
    std::string action = json["action"];

    if (action == "checkUpgrade") {
        ok = checkUpgrade(root, param); // param could be empty, if so only send back version
    } else if (action == "export") {
        if (has_param) {
            ok = exportData(root, param);
        }
    } else if (action == "customSupport") {
        ok = customSupport(root);
    } else if (action == "uploadURL" && is_admin) {
        ok = uploadURL(param.c_str());
    }

#if defined(EMSESP_STANDALONE) && !defined(EMSESP_UNITY)
    Serial.printf("%sweb output: %s[%s]", COLOR_WHITE, COLOR_BRIGHT_CYAN, request->url().c_str());
    Serial.printf(" %s(%d)%s ", ok ? COLOR_BRIGHT_GREEN : COLOR_BRIGHT_RED, ok ? 200 : 400, COLOR_YELLOW);
    serializeJson(root, Serial);
    Serial.println(COLOR_RESET);
#endif

    // send response
    if (!ok) {
        request->send(400);
        return;
    }

    response->setLength();
    request->send(response);
}

// action = checkUpgrade
// versions holds the latest development version and stable version in one string, comma separated
bool WebStatusService::checkUpgrade(JsonObject root, std::string & versions) {
    std::string current_version_s;
#ifndef EMSESP_STANDALONE
    current_version_s = EMSESP_APP_VERSION;
#else
    // for testing only - see api3 test in test.cpp
    // current_version_s = "3.6.5";
    current_version_s = "3.7.1-dev.8";
#endif

    if (!versions.empty()) {
        version::Semver200_version current_version(current_version_s);
        bool using_dev_version = !current_version.prerelease().find("dev"); // look for dev in the name to determine if we're using dev version
        version::Semver200_version latest_version(using_dev_version ? versions.substr(0, versions.find(',')) : versions.substr(versions.find(',') + 1));
        bool                       upgradeable = (latest_version > current_version);

#if defined(EMSESP_DEBUG)
        emsesp::EMSESP::logger()
            .debug("Checking Version upgrade. Using %s release branch. current version=%d.%d.%d-%s, latest version=%d.%d.%d-%s (%s upgradeable)",
                   (using_dev_version ? "dev" : "stable"),
                   current_version.major(),
                   current_version.minor(),
                   current_version.patch(),
                   current_version.prerelease().c_str(),
                   latest_version.major(),
                   latest_version.minor(),
                   latest_version.patch(),
                   latest_version.prerelease().c_str(),
                   upgradeable ? "IS" : "NOT");
#endif

        root["upgradeable"] = upgradeable;
    }

    root["emsesp_version"] = current_version_s; // always send back current version

    return true;
}

// action = allvalues
// output all the devices and the values
void WebStatusService::allvalues(JsonObject output) {
    JsonObject device_output;
    auto       value = F_(values);

    // EMS-Device Entities
    for (const auto & emsdevice : EMSESP::emsdevices) {
        std::string title = emsdevice->device_type_2_device_name_translated() + std::string(" ") + emsdevice->to_string();
        device_output     = output[title].to<JsonObject>();
        emsdevice->get_value_info(device_output, value, DeviceValueTAG::TAG_NONE);
    }

    // Custom Entities
    device_output = output["Custom Entities"].to<JsonObject>();
    EMSESP::webCustomEntityService.get_value_info(device_output, value);

    // Scheduler
    device_output = output["Scheduler"].to<JsonObject>();
    EMSESP::webSchedulerService.get_value_info(device_output, value);

    // Sensors
    device_output = output["Analog Sensors"].to<JsonObject>();
    EMSESP::analogsensor_.get_value_info(device_output, value);
    device_output = output["Temperature Sensors"].to<JsonObject>();
    EMSESP::temperaturesensor_.get_value_info(device_output, value);
}

// action = export
// returns data for a specific feature/settings as a json object
bool WebStatusService::exportData(JsonObject root, std::string & type) {
    root["type"] = type;

    if (type == "settings") {
        JsonObject node = root["System"].to<JsonObject>();
        node["version"] = EMSESP_APP_VERSION;
        System::extractSettings(NETWORK_SETTINGS_FILE, "Network", root);
        System::extractSettings(AP_SETTINGS_FILE, "AP", root);
        System::extractSettings(MQTT_SETTINGS_FILE, "MQTT", root);
        System::extractSettings(NTP_SETTINGS_FILE, "NTP", root);
        System::extractSettings(SECURITY_SETTINGS_FILE, "Security", root);
        System::extractSettings(EMSESP_SETTINGS_FILE, "Settings", root);
    } else if (type == "schedule") {
        System::extractSettings(EMSESP_SCHEDULER_FILE, "Schedule", root);
    } else if (type == "customizations") {
        System::extractSettings(EMSESP_CUSTOMIZATION_FILE, "Customizations", root);
    } else if (type == "entities") {
        System::extractSettings(EMSESP_CUSTOMENTITY_FILE, "Entities", root);
    } else if (type == "allvalues") {
        root.clear(); // don't need the "type" key added to the output
        allvalues(root);
    } else {
        return false; // error
    }

    return true;
}

// action = customSupport
// reads any upload customSupport.json file and sends to to Help page to be shown as Guest
bool WebStatusService::customSupport(JsonObject root) {
#ifndef EMSESP_STANDALONE
    // check if we have custom support file uploaded
    File file = LittleFS.open(EMSESP_CUSTOMSUPPORT_FILE, "r");
    if (!file) {
        // there is no custom file, return empty object
        return true;
    }

    // read the contents of the file into the root output json object
    DeserializationError error = deserializeJson(root, file);
    if (error) {
        emsesp::EMSESP::logger().err("Failed to read custom support file");
        return false;
    }

    file.close();
#endif
    return true;
}

// action = uploadURL
// uploads a firmware file from a URL
bool WebStatusService::uploadURL(const char * url) {
    // this will keep a copy of the URL, but won't initiate the download yet
    emsesp::EMSESP::system_.uploadFirmwareURL(url);
    return true;
}

} // namespace emsesp
