/*
 * EMS-ESP - https://github.com/emsesp/EMS-ESP
 * Copyright 2020-2025  emsesp.org
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
#include "httpClient.h"

namespace emsesp {

WebStatusService::WebStatusService(AsyncWebServer * server, SecurityManager * securityManager)
    : _securityManager(securityManager) {
    // GET
    securityManager->addEndpoint(server, EMSESP_SYSTEM_STATUS_SERVICE_PATH, AuthenticationPredicates::IS_AUTHENTICATED, [this](AsyncWebServerRequest * request) {
        systemStatus(request);
    });

    // POST - generic action handler, handles both GET and POST
    securityManager->addEndpoint(
        server,
        EMSESP_ACTION_SERVICE_PATH,
        AuthenticationPredicates::IS_AUTHENTICATED,
        [this](AsyncWebServerRequest * request, JsonVariant json) { action(request, json); },
        HTTP_ANY);
}

// /rest/systemStatus
// This contains both system & hardware Status to avoid having multiple costly endpoints
// This is also used for polling during the SystemMonitor to see if EMS-ESP is alive
void WebStatusService::systemStatus(AsyncWebServerRequest * request) {
    EMSESP::system_.refreshHeapMem(); // refresh free heap and max alloc heap

    auto *     response = new PsramAsyncJsonResponse(false);
    JsonObject root     = response->getRoot();

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
    uint8_t ntp_status = 0; // 0=disabled, 1=enabled, 2=connected
    if (esp_sntp_enabled()) {
        ntp_status = (EMSESP::system_.ntp_connected()) ? 2 : 1;
    }
    root["ntp_status"] = ntp_status;
    if (ntp_status == 2) {
        // send back actual time if NTP enabled and active
        time_t now = time(nullptr);
        if (now > 1500000000L) {
            char t[25];
            strftime(t, sizeof(t), "%FT%T", localtime(&now));
            root["ntp_time"] = t; // optional string
        }
    }
#endif

    root["ap_status"] = EMSESP::network_.ap_connected();

    if (EMSESP::network_.ethernet_connected()) {
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
    root["partition"]        = (const char *)esp_ota_get_running_partition()->label; // active partition
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
    root["board"] = EMSESP::system_.getBBQKeesGatewayDetails(FUSE_VALUE::BOARD);
#if CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32S2
    root["temperature"] = (int)Helpers::transformNumFloat(EMSESP::system_.temperature(), 0, EMSESP::system_.fahrenheit() ? 2 : 0); // only 2 decimal places
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

    // get the partition info for each partition, including the running one
    // the partition data is gathered once in System::start() and stored in partition_info_
    // install_date is stored as a UTC epoch and formatted to local time here so it honors
    // the current TZ (which may not have been set yet when System::start() ran).
    JsonArray partitions = root["partitions"].to<JsonArray>();
    for (const auto & partition : EMSESP::system_.partition_info_) {
        // Skip partition if it has no version, or it's size is 0
        if (partition.second.version.empty() || partition.second.size == 0) {
            continue;
        }
        JsonObject part   = partitions.add<JsonObject>();
        part["partition"] = partition.first;
        part["version"]   = partition.second.version;
        part["size"]      = partition.second.size;
        if (partition.second.install_date > 0) {
            char   time_string[25];
            time_t d = partition.second.install_date;
            strftime(time_string, sizeof(time_string), "%FT%T", localtime(&d));
            part["install_date"] = time_string;
        } else {
            part["install_date"] = "";
        }
    }

    root["developer_mode"] = EMSESP::system_.developer_mode();
    root["disable_reset"]  = EMSESP::system_.disable_reset();

    // Also used in SystemMonitor.tsx
    root["status"] = EMSESP::system_.systemStatus(); // send the status. See System.h for status codes
    if (EMSESP::system_.systemStatus() == SYSTEM_STATUS::SYSTEM_STATUS_PENDING_RESTART) {
        // we're ready to do the actual restart ASAP
        EMSESP::system_.systemStatus(SYSTEM_STATUS::SYSTEM_STATUS_RESTART_REQUESTED);
    }
    if (EMSESP::system_.systemStatus() == SYSTEM_STATUS::SYSTEM_STATUS_ERROR_UPLOAD) {
        // error is reported, back to normal state
        EMSESP::system_.systemStatus(SYSTEM_STATUS::SYSTEM_STATUS_NORMAL);
    }

#endif

    response->setLength();
    request->send(response);
}

// generic action handler - as a POST
void WebStatusService::action(AsyncWebServerRequest * request, JsonVariant json) {
    auto *     response = new PsramAsyncJsonResponse();
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
    bool        ok     = true;
    std::string action = json["action"];

    if (action == "getVersions") {
        getVersions(root);
    } else if (action == "setPartition") {
        ok = EMSESP::system_.set_partition(param.c_str());
    } else if (action == "export") {
        if (has_param) {
            ok = exportData(root, param);
        }
    } else if (action == "getCustomSupport") {
        ok = getCustomSupport(root);
    } else if (action == "uploadURL" && is_admin) {
        ok = uploadURL(param.c_str());
    } else if (action == "systemStatus" && is_admin) {
        ok = setSystemStatus(param.c_str());
    } else if (action == "resetMQTT" && is_admin) {
        EMSESP::mqtt_.reset_mqtt();
    } else if (action == "upgradeImportantMessages") {
        root["upgradeImportantMessageType"] = upgradeImportantMessages(param);
    } else if (action == "executeCommand") {
        ok = EMSESP::webCommandService.dispatchCommand(param.c_str()); // command worker task (ok = dispatched); fast internal commands run inline (ok = real result)
    }

#if defined(EMSESP_STANDALONE) && !defined(EMSESP_UNITY)
    Serial.printf("%sweb output: %s[%s]", COLOR_WHITE, COLOR_BRIGHT_CYAN, request->url().c_str());
    Serial.printf(" %s(%d)%s ", ok ? COLOR_BRIGHT_GREEN : COLOR_BRIGHT_RED, ok ? 200 : 400, COLOR_YELLOW);
    serializeJson(root, Serial);
    Serial.println(COLOR_RESET);
#endif

    // check for error
    if (!ok) {
        EMSESP::logger().err("Action '%s' failed", action.c_str());
        request->send(400); // bad request
        delete response;
        return;
    }

    // send response
    response->setLength();
    request->send(response);
}

// action = upgradeImportantMessages
// returns the type of upgrade important message to display in the UI
//  0 = no message (if just a minor version upgrade)
//  1 = special message for upgrades (no longer used)
//  2 = major version upgrade
// version can be like 3.8.2 or a filename like EMS-ESP-3_8_2-dev_13-ESP32-16MB+.bin
uint8_t WebStatusService::upgradeImportantMessages(std::string & version) {
    if (version.empty()) {
        return 0;
    }

    // it's a filename with a .bin or .md extension, try and extract the version from it
    // e.g. EMS-ESP-3_8_2-dev_13-ESP32-16MB+.bin -> major=3 minor=8 patch=2
    FirmwareVersion latest_version;
    if ((version.find(".bin") != std::string::npos) || (version.find(".md") != std::string::npos)) {
        std::string filename = version;
        auto        pos      = filename.find("EMS-ESP-");
        if (pos == std::string::npos) {
            EMSESP::logger().err("Invalid version string: %s", version.c_str());
            return 0;
        }

        pos += 8; // skip past "EMS-ESP-"
        auto underscore1 = filename.find('_', pos);
        auto underscore2 = filename.find('_', underscore1 + 1);
        auto dash        = filename.find('-', underscore2 + 1);
        if (underscore1 == std::string::npos || underscore2 == std::string::npos || dash == std::string::npos) {
            EMSESP::logger().err("Invalid version string: %s", version.c_str());
            return 0;
        }

        std::string major_version = filename.substr(pos, underscore1 - pos);
        std::string minor_version = filename.substr(underscore1 + 1, underscore2 - underscore1 - 1);
        std::string patch_version = filename.substr(underscore2 + 1, dash - underscore2 - 1);
        latest_version            = FirmwareVersion(major_version + "." + minor_version + "." + patch_version);
    } else {
        // if it's .json file exit
        if (version.find(".json") != std::string::npos) {
            return 0;
        } else {
            // treat it like a version string like "3.9.0"
            latest_version = FirmwareVersion(version);
        }
    }

    FirmwareVersion current_version(current_version_s); // get current version

    if (!(latest_version > current_version)) {
        return 0; // no upgrade (same version or downgrade)
    }

    // case "1" is no longer used
    // if (current_version < FirmwareVersion("3.9.0-dev.0") && latest_version.major() == 3 && latest_version.minor() == 9) {
    //     return 1; // upgrading to 3.9.x from anything older - new partition layout warning
    // }

    if (current_version.major() < latest_version.major()) {
        return 2; // major version upgrade
    }

    return 0; // minor or patch upgrade, no special message
}

// action = getVersions
// returns the device's current version for dev and stable, plus the system name
// The remote fetch runs from the main loop task via WebStatusService::loop() so that we never block the AsyncTCP callback
void WebStatusService::getVersions(JsonObject root) {
    FirmwareVersion current_version(current_version_s);
    bool            is_dev = current_version.prerelease().find("dev") != std::string::npos;

    // the WebUI shows this in the menu drawer. It's included here because this action is called once after login
    root["system_name"] = EMSESP::system_.system_name();

    JsonObject current     = root["current"].to<JsonObject>();
    current["version"]     = current_version_s;
    current["type"]        = is_dev ? "dev" : "stable";
    current["date"]        = "";
    current["upgradeable"] = current_upgradeable(); // false if cache not valid yet

#ifndef EMSESP_STANDALONE
    // pull the install_date for the running partition (if known)
    const esp_partition_t * running = esp_ota_get_running_partition();
    if (running != nullptr) {
        const auto & info = EMSESP::system_.partition_info_;
        auto         it   = info.find(running->label);
        if (it != info.end() && it->second.install_date > 0) {
            char   time_string[25];
            time_t d = it->second.install_date;
            strftime(time_string, sizeof(time_string), "%FT%T", localtime(&d));
            current["date"] = time_string;
        }
    }

    // take a snapshot, since the loop task can be rewriting the cache while we run on AsyncTCP
    VersionInfo stable;
    VersionInfo dev;
    bool        cache_valid;
    {
        std::lock_guard<std::mutex> lock{versions_mutex_};
        cache_valid = versions_cache_valid_;
        stable      = versions_stable_;
        dev         = versions_dev_;
    }

    versions_fetch_requested_ = true;

    if (!cache_valid) {
        // no successful fetch yet (no network, fetch pending, or parse error)
        return;
    }

    // copies a cached entry into root[key]
    auto add_section = [&](const char * key, const VersionInfo & info) {
        if (info.version.empty()) {
            return;
        }
        JsonObject out     = root[key].to<JsonObject>();
        out["version"]     = info.version;
        out["date"]        = info.date;
        out["upgradeable"] = info.upgradeable;
    };

    add_section("stable", stable);
    add_section("dev", dev);
#else
    // standalone/test build: provide deterministic dummy data
    JsonObject stable_out     = root["stable"].to<JsonObject>();
    stable_out["version"]     = "3.8.3";
    stable_out["date"]        = "2026-08-01";
    stable_out["upgradeable"] = FirmwareVersion("3.8.3") > current_version;

    JsonObject dev_out     = root["dev"].to<JsonObject>();
    dev_out["version"]     = "3.9.0-dev.1";
    dev_out["date"]        = "2026-08-02";
    dev_out["upgradeable"] = FirmwareVersion("3.9.0-dev.1") > current_version;
#endif
}

// schedule the next versions.json fetch a few seconds out so the network stack has time to settle
// (DHCP completion, default-netif assignment and DNS server propagation through lwip)
void WebStatusService::schedule_versions_refresh() {
    versions_failures_ = 0; // a fresh link is a fresh chance, so start again from the short retry delay
    uint32_t next      = uuid::get_uptime() + VERSIONS_INITIAL_FETCH_DELAY_MS;
    if (next == 0) {
        next = 1; // 0 is the "idle" sentinel — never let the wrap land there
    }
    versions_next_fetch_ms_ = next;
}

// refresh of the cached versions.json, at most once a day
void WebStatusService::loop() {
#ifndef EMSESP_STANDALONE
    // with auto_fw_check off we only reach out to emsesp.org when the WebUI asks for version
    // information, so an unattended system never contacts it at all
    if (!EMSESP::system_.auto_fw_check() && !versions_fetch_requested_) {
        return;
    }

    // need a network
    if (!EMSESP::network_.network_connected()) {
        return;
    }

    // 0 = idle, nothing scheduled
    if (versions_next_fetch_ms_ == 0) {
        return;
    }

    // not time yet (signed difference handles uint32 wrap)
    if ((int32_t)(uuid::get_uptime() - versions_next_fetch_ms_) < 0) {
        return;
    }

    versions_fetch_requested_ = false;

    uint32_t interval;
    if (refresh_versions_cache()) {
        versions_failures_ = 0;
        interval           = VERSIONS_REFRESH_INTERVAL_MS;
    } else {
        if (versions_failures_ < VERSIONS_MAX_BACKOFF_STEPS) {
            versions_failures_++;
        }
        interval = VERSIONS_RETRY_INTERVAL_MS << (versions_failures_ - 1);
    }

    uint32_t next = uuid::get_uptime() + interval;
    if (next == 0) {
        next = 1;
    }
    versions_next_fetch_ms_ = next;
#endif
}

// runs on the main loop task — never call this from an AsyncWebServer handler
bool WebStatusService::refresh_versions_cache() {
#ifdef EMSESP_STANDALONE
    return false;
#else
    std::string  result;
    JsonDocument doc;
    auto         http_code = HttpClient::request(VERSIONS_URL, "GET", "", doc.as<JsonObjectConst>(), result);
    if (http_code != 200) {
        EMSESP::logger().warning("Unable to retrieve online version information (HTTP error code %d)", http_code);
        return false;
    }
    DeserializationError err = deserializeJson(doc, result);
    if (err) {
#if defined(EMSESP_DEBUG)
        EMSESP::logger().debug("refresh_versions_cache() parse error");
#endif
        return false;
    }

    FirmwareVersion current_version(current_version_s);

    auto read_section = [&doc, &current_version](const char * key, VersionInfo & out) {
        JsonObjectConst section = doc[key];
        if (section.isNull()) {
            out = {};
            return;
        }
        out.version     = section["version"] | "";
        out.date        = section["date"] | "";
        out.upgradeable = !out.version.empty() && FirmwareVersion(out.version) > current_version;
    };

    // parse into locals first, then publish in one go so the AsyncTCP task never sees a half-written cache
    VersionInfo stable;
    VersionInfo dev;
    read_section("stable", stable);
    read_section("dev", dev);

    {
        std::lock_guard<std::mutex> lock{versions_mutex_};
        versions_stable_      = stable;
        versions_dev_         = dev;
        versions_cache_valid_ = true;
    }

#if defined(EMSESP_DEBUG)
    EMSESP::logger().debug("Fetched from %s: stable=%s, dev=%s, current=%s", VERSIONS_URL, stable.version.c_str(), dev.version.c_str(), current_version_s.c_str());
#endif
    return true;
#endif
}

// returns if current dev/stable is upgradeable
bool WebStatusService::current_upgradeable() const {
    FirmwareVersion             current_version(current_version_s);
    bool                        is_dev = current_version.prerelease().find("dev") != std::string::npos;
    std::lock_guard<std::mutex> lock{versions_mutex_};
    if (!versions_cache_valid_) {
        return false;
    }
    return is_dev ? versions_dev_.upgradeable : versions_stable_.upgradeable;
}

// action = allvalues
// output all the devices and their values, including custom entities, scheduler and sensors
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
    if (type == "settings") {
        root["type"] = type; // add settings as a group
        System::exportSettings(type, NETWORK_SETTINGS_FILE, root);
        System::exportSettings(type, AP_SETTINGS_FILE, root);
        System::exportSettings(type, MQTT_SETTINGS_FILE, root);
        System::exportSettings(type, NTP_SETTINGS_FILE, root);
        System::exportSettings(type, SECURITY_SETTINGS_FILE, root);
        System::exportSettings(type, EMSESP_SETTINGS_FILE, root);
    } else if (type == "schedule") {
        System::exportSettings(type, EMSESP_SCHEDULER_FILE, root);
    } else if (type == "customizations") {
        System::exportSettings(type, EMSESP_CUSTOMIZATION_FILE, root);
    } else if (type == "entities") {
        System::exportSettings(type, EMSESP_CUSTOMENTITY_FILE, root);
    } else if (type == "allvalues") {
        allvalues(root);
    } else if (type == "systembackup") {
        System::exportSystemBackup(root);
    } else {
        return false; // error
    }

    return true;
}

// action = getCustomSupport
// reads any upload customSupport.json file and sends to to Help page to be shown as Guest
bool WebStatusService::getCustomSupport(JsonObject root) {
    JsonDocument doc(PSRAM_DOC);

#if defined(EMSESP_STANDALONE)
    // dummy test data for "test api3"
    deserializeJson(doc, "{\"type\":\"customSupport\",\"Support\":{\"html\":[\"html code\",\"here\"], \"img_url\": \"https://emsesp.org/media/images/designer.png\"}");
#else
    // check if we have custom support file uploaded
    File file = LittleFS.open(EMSESP_CUSTOMSUPPORT_FILE, "r");
    if (!file) {
        // there is no custom file, return empty object
#if defined(EMSESP_DEBUG)
        EMSESP::logger().debug("No custom support file found");
#endif
        return true;
    }

    // read the contents of the file into a json doc. We can't do this direct to object since 7.2.1
    DeserializationError error = deserializeJson(doc, file);
    if (error) {
        EMSESP::logger().err("Failed to read custom support file");
        return false;
    }

    file.close();
#endif

#if defined(EMSESP_DEBUG)
    EMSESP::logger().debug("Sending custom support page");
#endif

    root.set(doc.as<JsonObject>()); // add to web response root object

    return true;
}

// action = uploadURL
// uploads a firmware file from a URL
bool WebStatusService::uploadURL(const char * url) {
    // this will keep a copy of the URL, but won't initiate the download yet
    EMSESP::system_.uploadFirmwareURL(url);
    return true;
}

// action = systemStatus
// sets the system status
bool WebStatusService::setSystemStatus(const char * status) {
    EMSESP::system_.systemStatus(Helpers::atoint(status));
    return true;
}

} // namespace emsesp
