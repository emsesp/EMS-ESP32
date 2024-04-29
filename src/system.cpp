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

#include "system.h"
#include "emsesp.h" // for send_raw_telegram() command

#ifndef EMSESP_STANDALONE
#include "esp_ota_ops.h"
#endif

#include <semver200.h>

#if defined(EMSESP_TEST)
#include "test/test.h"
#endif

#ifndef EMSESP_STANDALONE
#ifdef ESP_IDF_VERSION_MAJOR // IDF 4+
#if CONFIG_IDF_TARGET_ESP32  // ESP32/PICO-D4
#include "../esp32/rom/rtc.h"
#elif CONFIG_IDF_TARGET_ESP32S2
#include "../esp32s2/rom/rtc.h"
#elif CONFIG_IDF_TARGET_ESP32C3
#include "../esp32c3/rom/rtc.h"
#elif CONFIG_IDF_TARGET_ESP32S3
#include "../esp32s3/rom/rtc.h"
#else
#error Target CONFIG_IDF_TARGET is not supported
#endif
#else // ESP32 Before IDF 4.0
#include "../rom/rtc.h"
#endif
#endif

namespace emsesp {

// Languages supported. Note: the order is important and must match locale_translations.h
#if defined(EMSESP_TEST) || defined(EMSESP_EN_ONLY)
// in Debug mode use one language (en) to save flash memory needed for the tests
const char * const languages[] = {EMSESP_LOCALE_EN};
#elif defined(EMSESP_DE_ONLY)
const char * const languages[] = {EMSESP_LOCALE_DE};
#else
const char * const languages[] = {EMSESP_LOCALE_EN,
                                  EMSESP_LOCALE_DE,
                                  EMSESP_LOCALE_NL,
                                  EMSESP_LOCALE_SV,
                                  EMSESP_LOCALE_PL,
                                  EMSESP_LOCALE_NO,
                                  EMSESP_LOCALE_FR,
                                  EMSESP_LOCALE_TR,
                                  EMSESP_LOCALE_IT,
                                  EMSESP_LOCALE_SK};
#endif

static constexpr uint8_t NUM_LANGUAGES = sizeof(languages) / sizeof(const char *);

uuid::log::Logger System::logger_{F_(system), uuid::log::Facility::KERN};

// init statics
PButton  System::myPButton_;
bool     System::restart_requested_   = false;
bool     System::test_set_all_active_ = false;
uint32_t System::max_alloc_mem_;
uint32_t System::heap_mem_;

// find the index of the language
// 0 = EN, 1 = DE, etc...
uint8_t System::language_index() {
    for (uint8_t i = 0; i < NUM_LANGUAGES; i++) {
        if (languages[i] == locale()) {
            return i;
        }
    }
    return 0; // EN
}

// send raw to ems
bool System::command_send(const char * value, const int8_t id) {
    return EMSESP::txservice_.send_raw(value); // ignore id
}

bool System::command_response(const char * value, const int8_t id, JsonObject output) {
    JsonDocument doc;
    if (DeserializationError::Ok == deserializeJson(doc, Mqtt::get_response())) {
        for (JsonPair p : doc.as<JsonObject>()) {
            output[p.key()] = p.value();
        }
    } else {
        output["response"] = Mqtt::get_response();
    }
    return true;
}

// output all the EMS devices and their values, plus the sensors and any custom entities
// not scheduler as these are records with no output data
bool System::command_allvalues(const char * value, const int8_t id, JsonObject output) {
    JsonDocument doc;
    JsonObject   device_output;

    for (const auto & emsdevice : EMSESP::emsdevices) {
        std::string title = emsdevice->device_type_2_device_name_translated() + std::string(" ") + emsdevice->to_string();
        device_output     = output[title].to<JsonObject>();
        emsdevice->generate_values(device_output, DeviceValueTAG::TAG_NONE, true, EMSdevice::OUTPUT_TARGET::API_VERBOSE); // use nested for id -1 and 0
    }

    // Custom Entities
    device_output = output["Custom Entities"].to<JsonObject>();
    EMSESP::webCustomEntityService.get_value_info(device_output, "");

    // Sensors
    device_output = output["Analog Sensors"].to<JsonObject>();
    EMSESP::analogsensor_.get_value_info(device_output, "values");
    device_output = output["Temperature Sensors"].to<JsonObject>();
    EMSESP::temperaturesensor_.get_value_info(device_output, "values");

    return true;
}

// fetch device values
bool System::command_fetch(const char * value, const int8_t id) {
    std::string value_s;
    if (Helpers::value2string(value, value_s)) {
        if (value_s == "all") {
            LOG_INFO("Requesting data from EMS devices");
            EMSESP::fetch_device_values();
            return true;
        } else if (value_s == (F_(boiler))) {
            EMSESP::fetch_device_values_type(EMSdevice::DeviceType::BOILER);
            return true;
        } else if (value_s == (F_(thermostat))) {
            EMSESP::fetch_device_values_type(EMSdevice::DeviceType::THERMOSTAT);
            return true;
        } else if (value_s == (F_(solar))) {
            EMSESP::fetch_device_values_type(EMSdevice::DeviceType::SOLAR);
            return true;
        } else if (value_s == (F_(mixer))) {
            EMSESP::fetch_device_values_type(EMSdevice::DeviceType::MIXER);
            return true;
        }
    }

    EMSESP::fetch_device_values(); // default if no name or id is given
    return true;
}

// mqtt publish
bool System::command_publish(const char * value, const int8_t id) {
    std::string value_s;
    if (Helpers::value2string(value, value_s)) {
        if (value_s == "ha") {
            EMSESP::publish_all(true); // includes HA
            LOG_INFO("Publishing all data to MQTT, including HA configs");
            return true;
        } else if (value_s == (F_(boiler))) {
            EMSESP::publish_device_values(EMSdevice::DeviceType::BOILER);
            return true;
        } else if (value_s == (F_(thermostat))) {
            EMSESP::publish_device_values(EMSdevice::DeviceType::THERMOSTAT);
            return true;
        } else if (value_s == (F_(solar))) {
            EMSESP::publish_device_values(EMSdevice::DeviceType::SOLAR);
            return true;
        } else if (value_s == (F_(mixer))) {
            EMSESP::publish_device_values(EMSdevice::DeviceType::MIXER);
            return true;
        } else if (value_s == (F_(water))) {
            EMSESP::publish_device_values(EMSdevice::DeviceType::WATER);
            return true;
        } else if (value_s == "other") {
            EMSESP::publish_other_values(); // switch and heat pump
            return true;
        } else if ((value_s == (F_(temperaturesensor))) || (value_s == (F_(analogsensor)))) {
            EMSESP::publish_sensor_values(true);
            return true;
        }
    }

    LOG_INFO("Publishing all data to MQTT");
    EMSESP::publish_all();

    return true;
}

// syslog level
// commenting this out - don't see the point on having an API service to change the syslog level
/*
bool System::command_syslog_level(const char * value, const int8_t id) {
    uint8_t s = 0xff;
    if (Helpers::value2enum(value, s, FL_(list_syslog_level))) {
        bool changed = false;
        EMSESP::webSettingsService.update(
            [&](WebSettings & settings) {
                if (settings.syslog_level != (int8_t)s - 1) {
                    settings.syslog_level = (int8_t)s - 1;
                    changed               = true;
                }
                return StateUpdateResult::CHANGED;
            });
        if (changed) {
            EMSESP::system_.syslog_init();
        }
        return true;
    }
    return false;
}
*/

// watch
bool System::command_watch(const char * value, const int8_t id) {
    uint8_t  w = 0xff;
    uint16_t i = Helpers::hextoint(value);
    if (Helpers::value2enum(value, w, FL_(list_watch))) {
        if (w == 0 || EMSESP::watch() == EMSESP::Watch::WATCH_OFF) {
            EMSESP::watch_id(0);
        }
        if (Mqtt::publish_single() && w != EMSESP::watch()) {
            if (Mqtt::publish_single2cmd()) {
                Mqtt::queue_publish("system/watch", EMSESP::system_.enum_format() == ENUM_FORMAT_INDEX ? Helpers::itoa(w) : (FL_(list_watch)[w]));
            } else {
                Mqtt::queue_publish("system_data/watch", EMSESP::system_.enum_format() == ENUM_FORMAT_INDEX ? Helpers::itoa(w) : (FL_(list_watch)[w]));
            }
        }
        EMSESP::watch(w);
        return true;
    } else if (i) {
        if (Mqtt::publish_single() && i != EMSESP::watch_id()) {
            if (Mqtt::publish_single2cmd()) {
                Mqtt::queue_publish("system/watch", Helpers::hextoa(i));
            } else {
                Mqtt::queue_publish("system_data/watch", Helpers::hextoa(i));
            }
        }
        EMSESP::watch_id(i);
        if (EMSESP::watch() == EMSESP::Watch::WATCH_OFF) {
            EMSESP::watch(EMSESP::Watch::WATCH_ON);
        }
        return true;
    }
    return false;
}

void System::store_nvs_values() {
    if (Command::find_command(EMSdevice::DeviceType::BOILER, 0, "nompower") != nullptr) {
        Command::call(EMSdevice::DeviceType::BOILER, "nompower", "-1"); // trigger a write
    }
    EMSESP::analogsensor_.store_counters();
    EMSESP::nvs_.end();
}

// restart EMS-ESP
void System::system_restart(const char * partitionname) {
#ifndef EMSESP_STANDALONE
    if (partitionname != nullptr) {
        const esp_partition_t * partition = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_FACTORY, NULL);
        if (partition && strcmp(partition->label, partitionname) == 0) {
            esp_ota_set_boot_partition(partition);
        } else if (strcmp(esp_ota_get_running_partition()->label, partitionname) != 0) {
            partition = esp_ota_get_next_update_partition(NULL);
            if (!partition) {
                LOG_ERROR("Partition '%s' not found", partitionname);
                return;
            }
            if (strcmp(partition->label, partitionname) != 0 && strcmp(partitionname, "boot") != 0) {
                partition = esp_ota_get_next_update_partition(partition);
                if (!partition || strcmp(partition->label, partitionname)) {
                    LOG_ERROR("Partition '%s' not found", partitionname);
                    return;
                }
            }
            uint64_t buffer;
            esp_partition_read(partition, 0, &buffer, 8);
            if (buffer == 0xFFFFFFFFFFFFFFFF) { // partition empty
                LOG_ERROR("Partition '%s' is empty, not bootable", partition->label);
                return;
            }
            esp_ota_set_boot_partition(partition);
        }
        LOG_INFO("Restarting EMS-ESP from partition '%s'", partitionname);
    } else {
        LOG_INFO("Restarting EMS-ESP...");
    }
    store_nvs_values();
    Shell::loop_all();
    delay(1000); // wait a second
    ESP.restart();
#endif
}

// saves all settings
void System::wifi_reconnect() {
    EMSESP::esp8266React.getNetworkSettingsService()->read(
        [](NetworkSettings & networkSettings) { LOG_INFO("WiFi reconnecting to SSID '%s'...", networkSettings.ssid.c_str()); });
    Shell::loop_all();
    delay(1000);                                                            // wait a second
    EMSESP::webSettingsService.save();                                      // save local settings
    EMSESP::esp8266React.getNetworkSettingsService()->callUpdateHandlers(); // in case we've changed ssid or password
}

// format the FS. Wipes everything.
void System::format(uuid::console::Shell & shell) {
    auto msg = ("Formatting file system. This will reset all settings to their defaults");
    shell.logger().warning(msg);
    EMSuart::stop();

#ifndef EMSESP_STANDALONE
    LittleFS.format();
#endif

    System::system_restart();
}

void System::syslog_init() {
    EMSESP::webSettingsService.read([&](WebSettings & settings) {
        syslog_enabled_       = settings.syslog_enabled;
        syslog_level_         = settings.syslog_level;
        syslog_mark_interval_ = settings.syslog_mark_interval;
        syslog_host_          = settings.syslog_host;
        syslog_port_          = settings.syslog_port;
    });
#ifndef EMSESP_STANDALONE
    if (syslog_enabled_) {
        // start & configure syslog
        EMSESP::logger().info("Starting Syslog service");
        syslog_.start();

        syslog_.log_level((uuid::log::Level)syslog_level_);
        syslog_.mark_interval(syslog_mark_interval_);
        syslog_.destination(syslog_host_.c_str(), syslog_port_);
        syslog_.hostname(hostname().c_str());

        // removed in 3.6.0
        // Command::add(EMSdevice::DeviceType::SYSTEM, F_(syslog), System::command_syslog_level, FL_(changeloglevel_cmd), CommandFlag::ADMIN_ONLY);

    } else if (syslog_.started()) {
        // in case service is still running, this flushes the queue
        // https://github.com/emsesp/EMS-ESP/issues/496
        EMSESP::logger().info("Stopping Syslog");
        syslog_.log_level((uuid::log::Level)-1); // stop server
        syslog_.mark_interval(0);
        syslog_.destination("");
    }
    if (Mqtt::publish_single()) {
        if (Mqtt::publish_single2cmd()) {
            Mqtt::queue_publish("system/syslog", syslog_enabled_ ? (FL_(list_syslog_level)[syslog_level_ + 1]) : "off");
            if (EMSESP::watch_id() == 0 || EMSESP::watch() == 0) {
                Mqtt::queue_publish("system/watch",
                                    EMSESP::system_.enum_format() == ENUM_FORMAT_INDEX ? Helpers::itoa(EMSESP::watch()) : (FL_(list_watch)[EMSESP::watch()]));
            } else {
                Mqtt::queue_publish("system/watch", Helpers::hextoa(EMSESP::watch_id()));
            }

        } else {
            Mqtt::queue_publish("system_data/syslog", syslog_enabled_ ? (FL_(list_syslog_level)[syslog_level_ + 1]) : "off");
            if (EMSESP::watch_id() == 0 || EMSESP::watch() == 0) {
                Mqtt::queue_publish("system_data/watch",
                                    EMSESP::system_.enum_format() == ENUM_FORMAT_INDEX ? Helpers::itoa(EMSESP::watch()) : (FL_(list_watch)[EMSESP::watch()]));
            } else {
                Mqtt::queue_publish("system_data/watch", Helpers::hextoa(EMSESP::watch_id()));
            }
        }
    }
#endif
}

// read some specific system settings to store locally for faster access
void System::reload_settings() {
    EMSESP::webSettingsService.read([&](WebSettings & settings) {
        version_ = settings.version;

        pbutton_gpio_   = settings.pbutton_gpio;
        analog_enabled_ = settings.analog_enabled;
        low_clock_      = settings.low_clock;
        hide_led_       = settings.hide_led;
        led_gpio_       = settings.led_gpio;
        board_profile_  = settings.board_profile;
        telnet_enabled_ = settings.telnet_enabled;

        rx_gpio_     = settings.rx_gpio;
        tx_gpio_     = settings.tx_gpio;
        dallas_gpio_ = settings.dallas_gpio;

        syslog_enabled_       = settings.syslog_enabled;
        syslog_level_         = settings.syslog_level;
        syslog_mark_interval_ = settings.syslog_mark_interval;
        syslog_host_          = settings.syslog_host;
        syslog_port_          = settings.syslog_port;

        fahrenheit_     = settings.fahrenheit;
        bool_format_    = settings.bool_format;
        bool_dashboard_ = settings.bool_dashboard;
        enum_format_    = settings.enum_format;
        readonly_mode_  = settings.readonly_mode;

        phy_type_       = settings.phy_type;
        eth_power_      = settings.eth_power;
        eth_phy_addr_   = settings.eth_phy_addr;
        eth_clock_mode_ = settings.eth_clock_mode;

        locale_ = settings.locale;
    });
}

// check for valid ESP32 pins. This is very dependent on which ESP32 board is being used.
// Typically you can't use 1, 6-11, 20, 24, 28-31 and 40+
// we allow 0 as it has a special function on the NodeMCU apparently
// See https://diyprojects.io/esp32-how-to-use-gpio-digital-io-arduino-code/#.YFpVEq9KhjG
// and https://nodemcu.readthedocs.io/en/dev-esp32/modules/gpio/
bool System::is_valid_gpio(uint8_t pin, bool has_psram) {
#if CONFIG_IDF_TARGET_ESP32 || EMSESP_STANDALONE
    if ((pin == 1) || (pin >= 6 && pin <= 11) || (pin == 20) || (pin == 24) || (pin >= 28 && pin <= 31) || (pin > 40)
        || ((EMSESP::system_.PSram() > 0 || has_psram) && pin >= 16 && pin <= 17)) {
#elif CONFIG_IDF_TARGET_ESP32S2
    if ((pin >= 19 && pin <= 20) || (pin >= 22 && pin <= 32) || (pin > 40)) {
#elif CONFIG_IDF_TARGET_ESP32C3
    // https://www.wemos.cc/en/latest/c3/c3_mini.html
    if ((pin >= 11 && pin <= 19) || (pin > 21)) {
#elif CONFIG_IDF_TARGET_ESP32S3
    if ((pin >= 19 && pin <= 20) || (pin >= 22 && pin <= 37) || (pin >= 39 && pin <= 42) || (pin > 48)) {
#endif
        return false; // bad pin
    }
    return true;
}

// Starts up the UART Serial bridge
void System::start() {
#ifndef EMSESP_STANDALONE
    // disable bluetooth module
    // periph_module_disable(PERIPH_BT_MODULE);
    if (low_clock_) {
#if CONFIG_IDF_TARGET_ESP32C3
        setCpuFrequencyMhz(80);
#else
        setCpuFrequencyMhz(160);
#endif
    }

    // get current memory values
    fstotal_ = LittleFS.totalBytes() / 1024; // read only once, it takes 500 ms to read
    psram_   = ESP.getPsramSize() / 1024;
    appused_ = ESP.getSketchSize() / 1024;
    appfree_ = esp_ota_get_running_partition()->size / 1024 - appused_;
    refreshHeapMem(); // refresh free heap and max alloc heap
#endif

    EMSESP::esp8266React.getNetworkSettingsService()->read([&](NetworkSettings & networkSettings) {
        hostname(networkSettings.hostname.c_str()); // sets the hostname
    });

    commands_init();     // console & api commands
    led_init(false);     // init LED
    button_init(false);  // the special button
    network_init(false); // network
    EMSESP::uart_init(); // start UART
    syslog_init();       // start syslog
}

// button single click
void System::button_OnClick(PButton & b) {
    LOG_NOTICE("Button pressed - single click - show settings folders");

#if defined(EMSESP_TEST)
#ifndef EMSESP_STANDALONE
    Test::listDir(LittleFS, FS_CONFIG_DIRECTORY, 3);
#endif
#endif
}

// button double click
void System::button_OnDblClick(PButton & b) {
    LOG_NOTICE("Button pressed - double click - wifi reconnect");
    EMSESP::system_.wifi_reconnect();
}

// button long press
void System::button_OnLongPress(PButton & b) {
    LOG_NOTICE("Button pressed - long press - restart other partition");
    EMSESP::system_.system_restart("boot");
}

// button indefinite press
void System::button_OnVLongPress(PButton & b) {
    LOG_NOTICE("Button pressed - very long press - factory reset");
#ifndef EMSESP_STANDALONE
    EMSESP::esp8266React.factoryReset();
#endif
}

// push button
void System::button_init(bool refresh) {
    if (refresh) {
        reload_settings();
    }

    if (!is_valid_gpio(pbutton_gpio_)) {
        LOG_WARNING("Invalid button GPIO. Check config.");
        myPButton_.init(255, HIGH); // disable
        return;
    }
    if (!myPButton_.init(pbutton_gpio_, HIGH)) {
        LOG_WARNING("Multi-functional button not detected");
        return;
    }
    LOG_DEBUG("Multi-functional button enabled");

    myPButton_.onClick(BUTTON_Debounce, button_OnClick);
    myPButton_.onDblClick(BUTTON_DblClickDelay, button_OnDblClick);
    myPButton_.onLongPress(BUTTON_LongPressDelay, button_OnLongPress);
    myPButton_.onVLongPress(BUTTON_VLongPressDelay, button_OnVLongPress);
}

// set the LED to on or off when in normal operating mode
void System::led_init(bool refresh) {
    if (refresh) {
        reload_settings();
    }

    if ((led_gpio_ != 0) && is_valid_gpio(led_gpio_)) {
#if defined(ARDUINO_LOLIN_C3_MINI) && !defined(BOARD_C3_MINI_V1)
        // rgb LED WS2812B, use Adafruit Neopixel
        neopixelWrite(led_gpio_, 0, 0, 0);
#else
        pinMode(led_gpio_, OUTPUT);       // 0 means disabled
        digitalWrite(led_gpio_, !LED_ON); // start with LED off
#endif
    }
}

// returns true if OTA is uploading
bool System::upload_status() {
#if defined(EMSESP_STANDALONE)
    return false;
#else
    return upload_status_ || Update.isRunning();
#endif
}

void System::upload_status(bool in_progress) {
    // if we've just started an upload
    if (!upload_status_ && in_progress) {
        EMSuart::stop();
    }
    upload_status_ = in_progress;
}

// checks system health and handles LED flashing wizardry
void System::loop() {
    // check if we're supposed to do a reset/restart
    if (restart_requested()) {
        this->system_restart();
    }

#ifndef EMSESP_STANDALONE
    myPButton_.check(); // check button press

    if (syslog_enabled_) {
        syslog_.loop();
    }

    led_monitor();  // check status and report back using the LED
    system_check(); // check system health
    send_info_mqtt();
#endif
}

// send MQTT info topic appended with the version information as JSON, as a retained flag
void System::send_info_mqtt() {
    static uint8_t _connection = 0;
    uint8_t        connection  = (ethernet_connected() ? 1 : 0) + ((WiFi.status() == WL_CONNECTED) ? 2 : 0) + (ntp_connected_ ? 4 : 0) + (has_ipv6_ ? 8 : 0);
    // check if connection status has changed
    if (!Mqtt::connected() || connection == _connection) {
        return;
    }
    _connection = connection;
    JsonDocument doc;
    doc["event"]   = "connected";
    doc["version"] = EMSESP_APP_VERSION;

    // if NTP is enabled send the boot_time in local time in ISO 8601 format (eg: 2022-11-15 20:46:38)
    // https://github.com/emsesp/EMS-ESP32/issues/751
    if (ntp_connected_) {
        char   time_string[25];
        time_t now = time(nullptr) - uuid::get_uptime_sec();
        strftime(time_string, 25, "%FT%T%z", localtime(&now));
        doc["boot time"] = time_string;
    }

#ifndef EMSESP_STANDALONE
    if (EMSESP::system_.ethernet_connected()) {
        doc["network"]  = "ethernet";
        doc["hostname"] = ETH.getHostname();
        /*
        doc["MAC"]             = ETH.macAddress();
        doc["IPv4 address"]    = uuid::printable_to_string(ETH.localIP()) + "/" + uuid::printable_to_string(ETH.subnetMask());
        doc["IPv4 gateway"]    = uuid::printable_to_string(ETH.gatewayIP());
        doc["IPv4 nameserver"] = uuid::printable_to_string(ETH.dnsIP());
        if (ETH.localIPv6().toString() != "0000:0000:0000:0000:0000:0000:0000:0000" && ETH.localIPv6().toString() != "::") {
            doc["IPv6 address"] = uuid::printable_to_string(ETH.localIPv6());
    }
            */

    } else if (WiFi.status() == WL_CONNECTED) {
        doc["network"]         = "wifi";
        doc["hostname"]        = WiFi.getHostname();
        doc["SSID"]            = WiFi.SSID();
        doc["BSSID"]           = WiFi.BSSIDstr();
        doc["MAC"]             = WiFi.macAddress();
        doc["IPv4 address"]    = uuid::printable_to_string(WiFi.localIP()) + "/" + uuid::printable_to_string(WiFi.subnetMask());
        doc["IPv4 gateway"]    = uuid::printable_to_string(WiFi.gatewayIP());
        doc["IPv4 nameserver"] = uuid::printable_to_string(WiFi.dnsIP());

#if ESP_IDF_VERSION_MAJOR < 5
        if (WiFi.localIPv6().toString() != "0000:0000:0000:0000:0000:0000:0000:0000" && WiFi.localIPv6().toString() != "::") {
            doc["IPv6 address"] = uuid::printable_to_string(WiFi.localIPv6());
        }
#else
        if (WiFi.linkLocalIPv6().toString() != "0000:0000:0000:0000:0000:0000:0000:0000" && WiFi.linkLocalIPv6().toString() != "::") {
            doc["IPv6 address"] = uuid::printable_to_string(WiFi.linkLocalIPv6());
        }

#endif
    }
#endif
    Mqtt::queue_publish_retain(F_(info), doc.as<JsonObject>(), true); // topic called "info" and it's Retained
}

// create the json for heartbeat
void System::heartbeat_json(JsonObject output) {
    switch (EMSESP::bus_status()) {
    case EMSESP::BUS_STATUS_OFFLINE:
        output["bus_status"] = "connecting"; // EMS-ESP is booting...
        break;
    case EMSESP::BUS_STATUS_TX_ERRORS:
        output["bus_status"] = "txerror";
        break;
    case EMSESP::BUS_STATUS_CONNECTED:
        output["bus_status"] = "connected";
        break;
    default:
        output["bus_status"] = "disconnected";
        break;
    }

    output["uptime"]     = uuid::log::format_timestamp_ms(uuid::get_uptime_ms(), 3);
    output["uptime_sec"] = uuid::get_uptime_sec();
    bool value_b         = ntp_connected();
    if (EMSESP::system_.bool_format() == BOOL_FORMAT_TRUEFALSE) {
        output["ntp_status"] = value_b;
    } else if (EMSESP::system_.bool_format() == BOOL_FORMAT_10) {
        output["ntp_status"] = value_b ? 1 : 0;
    } else {
        char s[12];
        output["ntp_status"] = Helpers::render_boolean(s, value_b);
    }
    output["rxreceived"] = EMSESP::rxservice_.telegram_count();
    output["rxfails"]    = EMSESP::rxservice_.telegram_error_count();
    output["txreads"]    = EMSESP::txservice_.telegram_read_count();
    output["txwrites"]   = EMSESP::txservice_.telegram_write_count();
    output["txfails"]    = EMSESP::txservice_.telegram_read_fail_count() + EMSESP::txservice_.telegram_write_fail_count();

    if (Mqtt::enabled()) {
        output["mqttcount"]    = Mqtt::publish_count();
        output["mqttfails"]    = Mqtt::publish_fails();
        output["mqttconnects"] = Mqtt::connect_count();
    }
    output["apicalls"] = WebAPIService::api_count(); // + WebAPIService::api_fails();
    output["apifails"] = WebAPIService::api_fails();

    if (EMSESP::sensor_enabled() || EMSESP::analog_enabled()) {
        output["sensorreads"] = EMSESP::temperaturesensor_.reads() + EMSESP::analogsensor_.reads();
        output["sensorfails"] = EMSESP::temperaturesensor_.fails() + EMSESP::analogsensor_.fails();
    }

#ifndef EMSESP_STANDALONE
    output["freemem"]   = getHeapMem();
    output["max_alloc"] = getMaxAllocMem();
#endif

#ifndef EMSESP_STANDALONE
    if (!ethernet_connected_) {
        int8_t rssi            = WiFi.RSSI();
        output["rssi"]         = rssi;
        output["wifistrength"] = wifi_quality(rssi);
    }
#endif
}

// send periodic MQTT message with system information
void System::send_heartbeat() {
    // don't send heartbeat if WiFi or MQTT is not connected
    if (!Mqtt::connected()) {
        return;
    }

    refreshHeapMem(); // refresh free heap and max alloc heap

    JsonDocument doc;
    JsonObject   json = doc.to<JsonObject>();

    heartbeat_json(json);
    Mqtt::queue_publish(F_(heartbeat), json); // send to MQTT with retain off. This will add to MQTT queue.
}

// initializes network
void System::network_init(bool refresh) {
    if (refresh) {
        reload_settings();
    }

    last_system_check_ = 0; // force the LED to go from fast flash to pulse

#if CONFIG_IDF_TARGET_ESP32
    bool disableEth;
    EMSESP::esp8266React.getNetworkSettingsService()->read([&](NetworkSettings & settings) { disableEth = settings.ssid.length() > 0; });

    // no ethernet present or disabled
    if (phy_type_ == PHY_type::PHY_TYPE_NONE || disableEth) {
        return;
    } // no ethernet present

    // configure Ethernet
    int            mdc      = 23;            // Pin# of the I²C clock signal for the Ethernet PHY - hardcoded
    int            mdio     = 18;            // Pin# of the I²C IO signal for the Ethernet PHY - hardcoded
    uint8_t        phy_addr = eth_phy_addr_; // I²C-address of Ethernet PHY (0 or 1 for LAN8720, 31 for TLK110)
    int8_t         power    = eth_power_;    // Pin# of the enable signal for the external crystal oscillator (-1 to disable for internal APLL source)
    eth_phy_type_t type     = (phy_type_ == PHY_type::PHY_TYPE_LAN8720) ? ETH_PHY_LAN8720 : ETH_PHY_TLK110; // Type of the Ethernet PHY (LAN8720 or TLK110)
    // clock mode:
    //  ETH_CLOCK_GPIO0_IN   = 0  RMII clock input to GPIO0
    //  ETH_CLOCK_GPIO0_OUT  = 1  RMII clock output from GPIO0
    //  ETH_CLOCK_GPIO16_OUT = 2  RMII clock output from GPIO16
    //  ETH_CLOCK_GPIO17_OUT = 3  RMII clock output from GPIO17, for 50hz inverted clock
    auto clock_mode = (eth_clock_mode_t)eth_clock_mode_;

#if ESP_IDF_VERSION_MAJOR < 5
    eth_present_ = ETH.begin(phy_addr, power, mdc, mdio, type, clock_mode);
#else
    eth_present_ = ETH.begin(type, phy_addr, mdc, mdio, power, clock_mode);
#endif
#endif
}

// check health of system, done every 5 seconds
void System::system_check() {
    if (!last_system_check_ || ((uint32_t)(uuid::get_uptime() - last_system_check_) >= SYSTEM_CHECK_FREQUENCY)) {
        last_system_check_ = uuid::get_uptime();

#ifdef EMSESP_PINGTEST
        static uint64_t ping_count = 0;
        LOG_NOTICE("Ping test, #%d", ping_count++);
#endif

        // check if we have a valid network connection
        if (!ethernet_connected() && (WiFi.status() != WL_CONNECTED)) {
            healthcheck_ |= HEALTHCHECK_NO_NETWORK;
        } else {
            healthcheck_ &= ~HEALTHCHECK_NO_NETWORK;
        }

        // check if we have a bus connection
        if (!EMSbus::bus_connected()) {
            healthcheck_ |= HEALTHCHECK_NO_BUS;
        } else {
            healthcheck_ &= ~HEALTHCHECK_NO_BUS;
        }

        // see if the healthcheck state has changed
        static uint8_t last_healthcheck_ = 0;
        if (healthcheck_ != last_healthcheck_) {
            last_healthcheck_ = healthcheck_;

            EMSESP::system_.send_heartbeat(); // send MQTT heartbeat immediately when connected

            // see if we're better now
            if (healthcheck_ == 0) {
                // everything is healthy, show LED permanently on or off depending on setting
                if (led_gpio_) {
#if defined(ARDUINO_LOLIN_C3_MINI) && !defined(BOARD_C3_MINI_V1)
                    neopixelWrite(led_gpio_, 0, hide_led_ ? 0 : 128, 0);
#else
                    digitalWrite(led_gpio_, hide_led_ ? !LED_ON : LED_ON);
#endif
                }
            } else {
                // turn off LED so we're ready to the flashes
                if (led_gpio_) {
#if defined(ARDUINO_LOLIN_C3_MINI) && !defined(BOARD_C3_MINI_V1)
                    neopixelWrite(led_gpio_, 0, 0, 0);
#else
                    digitalWrite(led_gpio_, !LED_ON);
#endif
                }
            }
        }
    }
}

// commands - takes static function pointers
// can be called via Console using 'call system <cmd>'
void System::commands_init() {
    Command::add(EMSdevice::DeviceType::SYSTEM, F_(send), System::command_send, FL_(send_cmd), CommandFlag::ADMIN_ONLY);
    Command::add(EMSdevice::DeviceType::SYSTEM, F_(fetch), System::command_fetch, FL_(fetch_cmd), CommandFlag::ADMIN_ONLY);

    // restart and watch (and test) are also exposed as Console commands
    Command::add(EMSdevice::DeviceType::SYSTEM, F_(restart), System::command_restart, FL_(restart_cmd), CommandFlag::ADMIN_ONLY);
    Command::add(EMSdevice::DeviceType::SYSTEM, F_(watch), System::command_watch, FL_(watch_cmd));

#if defined(EMSESP_TEST)
    Command::add(EMSdevice::DeviceType::SYSTEM, ("test"), System::command_test, FL_(test_cmd));
#endif

    // these commands will return data in JSON format
    Command::add(EMSdevice::DeviceType::SYSTEM, F_(info), System::command_info, FL_(system_info_cmd));
    Command::add(EMSdevice::DeviceType::SYSTEM, F_(commands), System::command_commands, FL_(commands_cmd));
    Command::add(EMSdevice::DeviceType::SYSTEM, F("response"), System::command_response, FL_(commands_response));
    Command::add(EMSdevice::DeviceType::SYSTEM, F("allvalues"), System::command_allvalues, FL_(allvalues_cmd));


    // MQTT subscribe "ems-esp/system/#"
    Mqtt::subscribe(EMSdevice::DeviceType::SYSTEM, "system/#", nullptr); // use empty function callback
}

// uses LED to show system health
void System::led_monitor() {
    // we only need to run the LED healthcheck if there are errors
    if (!healthcheck_ || !led_gpio_) {
        return; // all good
    }

    static uint32_t led_long_timer_  = 1; // 1 will kick it off immediately
    static uint32_t led_short_timer_ = 0;
    static uint8_t  led_flash_step_  = 0; // 0 means we're not in the short flash timer
    auto            current_time     = uuid::get_uptime();

    // first long pause before we start flashing
    if (led_long_timer_ && (uint32_t)(current_time - led_long_timer_) >= HEALTHCHECK_LED_LONG_DUARATION) {
        // Serial.println("starting the flash check");
        led_short_timer_ = current_time; // start the short timer
        led_long_timer_  = 0;            // stop long timer
        led_flash_step_  = 1;            // enable the short flash timer
    }

    // the flash timer which starts after the long pause
    if (led_flash_step_ && (uint32_t)(current_time - led_short_timer_) >= HEALTHCHECK_LED_FLASH_DUARATION) {
        led_long_timer_     = 0; // stop the long timer
        led_short_timer_    = current_time;
        static bool led_on_ = false;

        if (++led_flash_step_ == 8) {
            // reset the whole sequence
            // Serial.println("resetting flash check");
            led_long_timer_ = uuid::get_uptime();
            led_flash_step_ = 0;
#if defined(ARDUINO_LOLIN_C3_MINI) && !defined(BOARD_C3_MINI_V1)
            neopixelWrite(led_gpio_, 0, 0, 0);
#else
            digitalWrite(led_gpio_, !LED_ON); // LED off
#endif
        } else if (led_flash_step_ % 2) {
            // handle the step events (on odd numbers 3,5,7,etc). see if we need to turn on a LED
            //  1 flash is the EMS bus is not connected
            //  2 flashes if the network (wifi or ethernet) is not connected
            //  3 flashes is both the bus and the network are not connected. Then you know you're truly f*cked.

#if defined(ARDUINO_LOLIN_C3_MINI) && !defined(BOARD_C3_MINI_V1)
            if (led_flash_step_ == 3) {
                if ((healthcheck_ & HEALTHCHECK_NO_NETWORK) == HEALTHCHECK_NO_NETWORK) {
                    neopixelWrite(led_gpio_, 128, 0, 0); // red
                } else if ((healthcheck_ & HEALTHCHECK_NO_BUS) == HEALTHCHECK_NO_BUS) {
                    neopixelWrite(led_gpio_, 0, 0, 128); // blue
                }
            }
            if (led_flash_step_ == 5 && (healthcheck_ & HEALTHCHECK_NO_NETWORK) == HEALTHCHECK_NO_NETWORK) {
                neopixelWrite(led_gpio_, 128, 0, 0); // red
            }
            if ((led_flash_step_ == 7) && ((healthcheck_ & HEALTHCHECK_NO_NETWORK) == HEALTHCHECK_NO_NETWORK)
                && ((healthcheck_ & HEALTHCHECK_NO_BUS) == HEALTHCHECK_NO_BUS)) {
                neopixelWrite(led_gpio_, 0, 0, 128); // blue
            }
#else

            if ((led_flash_step_ == 3)
                && (((healthcheck_ & HEALTHCHECK_NO_NETWORK) == HEALTHCHECK_NO_NETWORK) || ((healthcheck_ & HEALTHCHECK_NO_BUS) == HEALTHCHECK_NO_BUS))) {
                led_on_ = true;
            }

            if ((led_flash_step_ == 5) && ((healthcheck_ & HEALTHCHECK_NO_NETWORK) == HEALTHCHECK_NO_NETWORK)) {
                led_on_ = true;
            }

            if ((led_flash_step_ == 7) && ((healthcheck_ & HEALTHCHECK_NO_NETWORK) == HEALTHCHECK_NO_NETWORK)
                && ((healthcheck_ & HEALTHCHECK_NO_BUS) == HEALTHCHECK_NO_BUS)) {
                led_on_ = true;
            }

            if (led_on_) {
                digitalWrite(led_gpio_, LED_ON); // LED off
            }
#endif
        } else {
            // turn the led off after the flash, on even number count
            if (led_on_) {
#if defined(ARDUINO_LOLIN_C3_MINI) && !defined(BOARD_C3_MINI_V1)
                neopixelWrite(led_gpio_, 0, 0, 0);
#else
                digitalWrite(led_gpio_, !LED_ON); // LED off
#endif
                led_on_ = false;
            }
        }
    }
}

// Return the quality (Received Signal Strength Indicator) of the WiFi network as a %
//  High quality: 90% ~= -55dBm
//  Medium quality: 50% ~= -75dBm
//  Low quality: 30% ~= -85dBm
//  Unusable quality: 8% ~= -96dBm
int8_t System::wifi_quality(int8_t dBm) {
    if (dBm <= -100) {
        return 0;
    }

    if (dBm >= -50) {
        return 100;
    }
    return 2 * (dBm + 100);
}

// print users to console
void System::show_users(uuid::console::Shell & shell) {
    shell.printfln("Users:");

#ifndef EMSESP_STANDALONE
    EMSESP::esp8266React.getSecuritySettingsService()->read([&](SecuritySettings & securitySettings) {
        for (const User & user : securitySettings.users) {
            shell.printfln(" username: %s, password: %s, is_admin: %s", user.username.c_str(), user.password.c_str(), user.admin ? ("yes") : ("no"));
        }
    });
#endif

    shell.println();
}

void System::show_system(uuid::console::Shell & shell) {
    refreshHeapMem(); // refresh free heap and max alloc heap

    shell.println("System:");
    shell.printfln(" Version: %s", EMSESP_APP_VERSION);
    shell.printfln(" Language: %s", locale().c_str());
    shell.printfln(" Board profile: %s", board_profile().c_str());
    shell.printfln(" Uptime: %s", uuid::log::format_timestamp_ms(uuid::get_uptime_ms(), 3).c_str());
#ifndef EMSESP_STANDALONE
    shell.printfln(" SDK version: %s", ESP.getSdkVersion());
    shell.printfln(" CPU frequency: %lu MHz", ESP.getCpuFreqMHz());
    shell.printfln(" Free heap/Max alloc: %lu KB / %lu KB", getHeapMem(), getMaxAllocMem());
    shell.printfln(" App used/free: %lu KB / %lu KB", appUsed(), appFree());
    uint32_t FSused = LittleFS.usedBytes() / 1024;
    shell.printfln(" FS used/free: %lu KB / %lu KB", FSused, FStotal() - FSused);
    shell.println();

    shell.println("Network:");

    // show ethernet mac address if we have an eth controller present
    if (eth_present_) {
        shell.printfln(" Ethernet MAC address: %s", ETH.macAddress().c_str());
    }

    switch (WiFi.status()) {
    case WL_IDLE_STATUS:
        shell.printfln(" Status: Idle");
        break;

    case WL_NO_SSID_AVAIL:
        shell.printfln(" Status: Network not found");
        break;

    case WL_SCAN_COMPLETED:
        shell.printfln(" Status: Network scan complete");
        break;

    case WL_CONNECTED:
        shell.printfln(" Status: WiFi connected");
        shell.printfln(" SSID: %s", WiFi.SSID().c_str());
        shell.printfln(" BSSID: %s", WiFi.BSSIDstr().c_str());
        shell.printfln(" RSSI: %d dBm (%d %%)", WiFi.RSSI(), wifi_quality(WiFi.RSSI()));
        char result[10];
        shell.printfln(" TxPower: %s dBm", emsesp::Helpers::render_value(result, (double)(WiFi.getTxPower() / 4), 1));
        shell.printfln(" MAC address: %s", WiFi.macAddress().c_str());
        shell.printfln(" Hostname: %s", WiFi.getHostname());
        shell.printfln(" IPv4 address: %s/%s", uuid::printable_to_string(WiFi.localIP()).c_str(), uuid::printable_to_string(WiFi.subnetMask()).c_str());
        shell.printfln(" IPv4 gateway: %s", uuid::printable_to_string(WiFi.gatewayIP()).c_str());
        shell.printfln(" IPv4 nameserver: %s", uuid::printable_to_string(WiFi.dnsIP()).c_str());
#if ESP_IDF_VERSION_MAJOR < 5
        if (WiFi.localIPv6().toString() != "0000:0000:0000:0000:0000:0000:0000:0000" && WiFi.localIPv6().toString() != "::") {
            shell.printfln(" IPv6 address: %s", uuid::printable_to_string(WiFi.localIPv6()).c_str());
        }
#else
        if (WiFi.linkLocalIPv6().toString() != "0000:0000:0000:0000:0000:0000:0000:0000" && WiFi.linkLocalIPv6().toString() != "::") {
            shell.printfln(" IPv6 address: %s", uuid::printable_to_string(WiFi.linkLocalIPv6()).c_str());
        }
#endif

        break;

    case WL_CONNECT_FAILED:
        shell.printfln(" WiFi Network: Connection failed");
        break;

    case WL_CONNECTION_LOST:
        shell.printfln(" WiFi Network: Connection lost");
        break;

    case WL_DISCONNECTED:
        shell.printfln(" WiFi Network: Disconnected");
        break;

    case WL_NO_SHIELD:
    default:
        shell.printfln(" WiFi MAC address: %s", WiFi.macAddress().c_str());
        shell.printfln(" WiFi Network: not connected");
        break;
    }

    // show Ethernet if connected
    if (ethernet_connected_) {
        shell.println();
        shell.printfln(" Status: Ethernet connected");
        shell.printfln(" Ethernet MAC address: %s", ETH.macAddress().c_str());
        shell.printfln(" Hostname: %s", ETH.getHostname());
        shell.printfln(" IPv4 address: %s/%s", uuid::printable_to_string(ETH.localIP()).c_str(), uuid::printable_to_string(ETH.subnetMask()).c_str());
        shell.printfln(" IPv4 gateway: %s", uuid::printable_to_string(ETH.gatewayIP()).c_str());
        shell.printfln(" IPv4 nameserver: %s", uuid::printable_to_string(ETH.dnsIP()).c_str());
#if ESP_IDF_VERSION_MAJOR < 5
        if (ETH.localIPv6().toString() != "0000:0000:0000:0000:0000:0000:0000:0000" && ETH.localIPv6().toString() != "::") {
            shell.printfln(" IPv6 address: %s", uuid::printable_to_string(ETH.localIPv6()).c_str());
        }
#else
        if (ETH.linkLocalIPv6().toString() != "0000:0000:0000:0000:0000:0000:0000:0000" && ETH.linkLocalIPv6().toString() != "::") {
            shell.printfln(" IPv6 address: %s", uuid::printable_to_string(ETH.linkLocalIPv6()).c_str());
        }
#endif
    }
    shell.println();

    shell.println("Syslog:");
    if (!syslog_enabled_) {
        shell.printfln(" Syslog: disabled");
    } else {
        shell.printfln(" Syslog: %s", syslog_.started() ? "started" : "stopped");
        shell.print(" ");
        shell.printfln(F_(host_fmt), !syslog_host_.isEmpty() ? syslog_host_.c_str() : (F_(unset)));
        shell.printfln(" IP: %s", uuid::printable_to_string(syslog_.ip()).c_str());
        shell.print(" ");
        shell.printfln(F_(port_fmt), syslog_port_);
        shell.print(" ");
        shell.printfln(F_(log_level_fmt), uuid::log::format_level_lowercase(static_cast<uuid::log::Level>(syslog_level_)));
        shell.print(" ");
        shell.printfln(F_(mark_interval_fmt), syslog_mark_interval_);
        shell.printfln(" Queued: %d", syslog_.queued());
    }

#endif
}

// see if there is a restore of an older settings file that needs to be applied
bool System::check_restore() {
    bool reboot_required = false;

#ifndef EMSESP_STANDALONE
    // see if we have a temp file, if so try and read it
    File new_file = LittleFS.open(TEMP_FILENAME_PATH);
    if (new_file) {
        JsonDocument         jsonDocument;
        DeserializationError error = deserializeJson(jsonDocument, new_file);
        if (error == DeserializationError::Ok && jsonDocument.is<JsonObject>()) {
            JsonObject input = jsonDocument.as<JsonObject>();
            // see what type of file it is, either settings or customization. anything else is ignored
            std::string settings_type = input["type"];
            if (settings_type == "settings") {
                // It's a settings file. Parse each section separately. If it's system related it will require a reboot
                reboot_required = saveSettings(NETWORK_SETTINGS_FILE, "Network", input);
                reboot_required |= saveSettings(AP_SETTINGS_FILE, "AP", input);
                reboot_required |= saveSettings(MQTT_SETTINGS_FILE, "MQTT", input);
                reboot_required |= saveSettings(NTP_SETTINGS_FILE, "NTP", input);
                reboot_required |= saveSettings(SECURITY_SETTINGS_FILE, "Security", input);
                reboot_required |= saveSettings(EMSESP_SETTINGS_FILE, "Settings", input);
                reboot_required |= saveSettings(OTA_SETTINGS_FILE, "OTA", input);
            } else if (settings_type == "customizations") {
                // it's a customization file, just replace it and there's no need to reboot
                saveSettings(EMSESP_CUSTOMIZATION_FILE, "Customizations", input);
            } else if (settings_type == "schedule") {
                // it's a schedule file, just replace it and there's no need to reboot
                saveSettings(EMSESP_SCHEDULER_FILE, "Schedule", input);
            } else if (settings_type == "entities") {
                // it's a entity file, just replace it and there's no need to reboot
                saveSettings(EMSESP_CUSTOMENTITY_FILE, "Entities", input);
            } else {
                LOG_ERROR("Unrecognized file uploaded");
            }
        } else {
            LOG_ERROR("Unrecognized file uploaded, not json");
        }

        // close (just in case) and remove the temp file
        new_file.close();
        LittleFS.remove(TEMP_FILENAME_PATH);
    }
#endif

    return reboot_required;
}

// handle upgrades from previous versions
// this function will not be called on a clean install, with no settings files yet created
// returns true if we need a reboot
bool System::check_upgrade(bool factory_settings) {
    bool        missing_version = true;
    std::string settingsVersion{EMSESP_APP_VERSION}; // default setting version

    if (!factory_settings) {
        // fetch current version from settings file
        EMSESP::webSettingsService.read([&](WebSettings & settings) { settingsVersion = settings.version.c_str(); });

        // see if we're missing a version, will be < 3.5.0b13 from Dec 23 2022
        missing_version = (settingsVersion.empty() || (settingsVersion.length() < 5));
        if (missing_version) {
            LOG_WARNING("No version information found (%s)", settingsVersion.c_str());
            settingsVersion = "3.5.0"; // this was the last stable version without version info
        }
    }

    version::Semver200_version settings_version(settingsVersion);

    if (!missing_version) {
        LOG_DEBUG("Checking version upgrade (settings file is v%d.%d.%d-%s)",
                  settings_version.major(),
                  settings_version.minor(),
                  settings_version.patch(),
                  settings_version.prerelease().c_str());
    }

    if (factory_settings) {
        return false; // fresh install, do nothing
    }

    version::Semver200_version this_version(EMSESP_APP_VERSION);

    bool save_version = true;

    // compare versions
    if (this_version > settings_version) {
        // need upgrade
        LOG_NOTICE("Upgrading to version %d.%d.%d-%s", this_version.major(), this_version.minor(), this_version.patch(), this_version.prerelease().c_str());

        // if we're coming from 3.4.4 or 3.5.0b14 which had no version stored then we need to apply new settings
        if (missing_version) {
            LOG_INFO("Setting MQTT Entity ID format to v3.4 format");
            EMSESP::esp8266React.getMqttSettingsService()->update([&](MqttSettings & mqttSettings) {
                mqttSettings.entity_format = 0; // use old Entity ID format from v3.4
                return StateUpdateResult::CHANGED;
            });
        } else if (settings_version.major() == 3 && settings_version.minor() <= 6) {
            LOG_INFO("Setting MQTT Entity ID format to v3.6 format");
            EMSESP::esp8266React.getMqttSettingsService()->update([&](MqttSettings & mqttSettings) {
                if (mqttSettings.entity_format == 1) {
                    mqttSettings.entity_format = 3; // use old Entity ID format from v3.6
                    return StateUpdateResult::CHANGED;
                } else if (mqttSettings.entity_format == 2) {
                    mqttSettings.entity_format = 4; // use old Entity ID format from v3.6
                    return StateUpdateResult::CHANGED;
                }
                return StateUpdateResult::UNCHANGED;
            });
        }

        // Network Settings Wifi tx_power is now using the value * 4.
        EMSESP::esp8266React.getNetworkSettingsService()->update([&](NetworkSettings & networkSettings) {
            if (networkSettings.tx_power == 20) {
                networkSettings.tx_power = WIFI_POWER_19_5dBm; // use 19.5 as we don't have 20 anymore
                LOG_INFO("Setting WiFi TX Power to Auto");
                return StateUpdateResult::CHANGED;
            }
            return StateUpdateResult::UNCHANGED;
        });

    } else if (this_version < settings_version) {
        // need downgrade
        LOG_NOTICE("Downgrading to version %d.%d.%d-%s", this_version.major(), this_version.minor(), this_version.patch(), this_version.prerelease().c_str());
    } else {
        // same version, do nothing
        save_version = false;
    }

    // if we did a change, set the new version and reboot
    if (save_version) {
        EMSESP::webSettingsService.update([&](WebSettings & settings) {
            settings.version = EMSESP_APP_VERSION;
            return StateUpdateResult::CHANGED;
        });
        return true; // need reboot
    }

    return false;
}

// list commands
bool System::command_commands(const char * value, const int8_t id, JsonObject output) {
    return Command::list(EMSdevice::DeviceType::SYSTEM, output);
}

// convert settings file into json object
void System::extractSettings(const char * filename, const char * section, JsonObject output) {
#ifndef EMSESP_STANDALONE
    File settingsFile = LittleFS.open(filename);
    if (settingsFile) {
        JsonDocument         jsonDocument;
        DeserializationError error = deserializeJson(jsonDocument, settingsFile);
        if (error == DeserializationError::Ok && jsonDocument.is<JsonObject>()) {
            JsonObject jsonObject = jsonDocument.as<JsonObject>();
            JsonObject node       = output[section].to<JsonObject>();
            for (JsonPair kvp : jsonObject) {
                node[kvp.key()] = kvp.value();
            }
        }
    }
    settingsFile.close();
#endif
}

// save settings file using input from a json object
bool System::saveSettings(const char * filename, const char * section, JsonObject input) {
#ifndef EMSESP_STANDALONE
    JsonObject section_json = input[section];
    if (section_json) {
        File section_file = LittleFS.open(filename, "w");
        if (section_file) {
            LOG_INFO("Applying new %s", section);
            serializeJson(section_json, section_file);
            section_file.close();
            return true; // reboot required
        }
    }
#endif
    return false; // not found
}

// export status information including the device information
// http://ems-esp/api/system/info
bool System::command_info(const char * value, const int8_t id, JsonObject output) {
    JsonObject node;

    // System
    node                     = output["System Info"].to<JsonObject>();
    node["version"]          = EMSESP_APP_VERSION;
    node["uptime"]           = uuid::log::format_timestamp_ms(uuid::get_uptime_ms(), 3);
    node["uptime (seconds)"] = uuid::get_uptime_sec();
#ifndef EMSESP_STANDALONE
    node["platform"]  = EMSESP_PLATFORM;
    node["arduino"]   = ARDUINO_VERSION;
    node["sdk"]       = ESP.getSdkVersion();
    node["free mem"]  = getHeapMem();
    node["max alloc"] = getMaxAllocMem();
    node["free caps"] = heap_caps_get_free_size(MALLOC_CAP_8BIT) / 1024; // includes heap and psram
    node["used app"]  = EMSESP::system_.appUsed();                       // kilobytes
    node["free app"]  = EMSESP::system_.appFree();                       // kilobytes
    node["partition"] = esp_ota_get_running_partition()->label;
#endif
    node["reset reason"] = EMSESP::system_.reset_reason(0) + " / " + EMSESP::system_.reset_reason(1);

#ifndef EMSESP_STANDALONE
    // Network Status
    node = output["Network Info"].to<JsonObject>();
    if (EMSESP::system_.ethernet_connected()) {
        node["network"]  = "Ethernet";
        node["hostname"] = ETH.getHostname();
        // node["MAC"]             = ETH.macAddress();
        // node["IPv4 address"]    = uuid::printable_to_string(ETH.localIP()) + "/" + uuid::printable_to_string(ETH.subnetMask());
        // node["IPv4 gateway"]    = uuid::printable_to_string(ETH.gatewayIP());
        // node["IPv4 nameserver"] = uuid::printable_to_string(ETH.dnsIP());
        // if (ETH.localIPv6().toString() != "0000:0000:0000:0000:0000:0000:0000:0000") {
        //     node["IPv6 address"] = uuid::printable_to_string(ETH.localIPv6());
        // }
    } else if (WiFi.status() == WL_CONNECTED) {
        node["network"]  = "WiFi";
        node["hostname"] = WiFi.getHostname();
        node["RSSI"]     = WiFi.RSSI();
        // node["MAC"]             = WiFi.macAddress();
        // node["IPv4 address"]    = uuid::printable_to_string(WiFi.localIP()) + "/" + uuid::printable_to_string(WiFi.subnetMask());
        // node["IPv4 gateway"]    = uuid::printable_to_string(WiFi.gatewayIP());
        // node["IPv4 nameserver"] = uuid::printable_to_string(WiFi.dnsIP());
        // if (WiFi.localIPv6().toString() != "0000:0000:0000:0000:0000:0000:0000:0000") {
        //     node["IPv6 address"] = uuid::printable_to_string(WiFi.localIPv6());
        // }
    }
#endif
    EMSESP::esp8266React.getNetworkSettingsService()->read([&](NetworkSettings & settings) {
        if (WiFi.status() == WL_CONNECTED && !settings.bssid.isEmpty()) {
            node["BSSID"] = "set";
        }
        node["TxPower setting"]  = settings.tx_power;
        node["static ip config"] = settings.staticIPConfig;
        node["enable IPv6"]      = settings.enableIPv6;
        node["low bandwidth"]    = settings.bandwidth20;
        node["disable sleep"]    = settings.nosleep;
        node["enable MDNS"]      = settings.enableMDNS;
        node["enable CORS"]      = settings.enableCORS;
        if (settings.enableCORS) {
            node["CORS origin"] = settings.CORSOrigin;
        }
    });
#ifndef EMSESP_STANDALONE
    EMSESP::esp8266React.getAPSettingsService()->read([&](APSettings & settings) {
        const char * pM[]         = {"always", "disconnected", "never"};
        node["AP provision mode"] = pM[settings.provisionMode];
        node["AP security"]       = settings.password.length() ? "wpa2" : "open";
        node["AP ssid"]           = settings.ssid;
    });
#endif

    // NTP status
    node = output["NTP Info"].to<JsonObject>();
#ifndef EMSESP_STANDALONE
    node["NTP status"] = EMSESP::system_.ntp_connected() ? "connected" : "disconnected";
    EMSESP::esp8266React.getNTPSettingsService()->read([&](NTPSettings & settings) {
        node["enabled"]  = settings.enabled;
        node["server"]   = settings.server;
        node["tz label"] = settings.tzLabel;
        // node["tz format"] = settings.tzFormat;
    });

    // OTA status
    node = output["OTA Info"].to<JsonObject>();
    EMSESP::esp8266React.getOTASettingsService()->read([&](OTASettings & settings) {
        node["enabled"] = settings.enabled;
        node["port"]    = settings.port;
    });
#endif

    // MQTT Status
    node                = output["MQTT Info"].to<JsonObject>();
    node["MQTT status"] = Mqtt::connected() ? F_(connected) : F_(disconnected);
    if (Mqtt::enabled()) {
        node["MQTT publishes"]     = Mqtt::publish_count();
        node["MQTT queued"]        = Mqtt::publish_queued();
        node["MQTT publish fails"] = Mqtt::publish_fails();
        node["MQTT connects"]      = Mqtt::connect_count();
    }
    EMSESP::esp8266React.getMqttSettingsService()->read([&](MqttSettings & settings) {
        node["enabled"]                 = settings.enabled;
        node["client id"]               = settings.clientId;
        node["keep alive"]              = settings.keepAlive;
        node["clean session"]           = settings.cleanSession;
        node["entity format"]           = settings.entity_format;
        node["base"]                    = settings.base;
        node["discovery prefix"]        = settings.discovery_prefix;
        node["discovery type"]          = settings.discovery_type;
        node["nested format"]           = settings.nested_format;
        node["ha enabled"]              = settings.ha_enabled;
        node["mqtt qos"]                = settings.mqtt_qos;
        node["mqtt retain"]             = settings.mqtt_retain;
        node["publish time heartbeat"]  = settings.publish_time_heartbeat;
        node["publish time boiler"]     = settings.publish_time_boiler;
        node["publish time thermostat"] = settings.publish_time_thermostat;
        node["publish time solar"]      = settings.publish_time_solar;
        node["publish time mixer"]      = settings.publish_time_mixer;
        node["publish time water"]      = settings.publish_time_water;
        node["publish time other"]      = settings.publish_time_other;
        node["publish time sensor"]     = settings.publish_time_sensor;
        node["publish single"]          = settings.publish_single;
        node["publish2command"]         = settings.publish_single2cmd;
        node["send response"]           = settings.send_response;
    });

    // Syslog Status
    node            = output["Syslog Info"].to<JsonObject>();
    node["enabled"] = EMSESP::system_.syslog_enabled_;
#ifndef EMSESP_STANDALONE
    if (EMSESP::system_.syslog_enabled_) {
        node["syslog started"] = syslog_.started();
        node["syslog level"]   = FL_(list_syslog_level)[syslog_.log_level() + 1];
        node["syslog ip"]      = syslog_.ip();
        node["syslog queue"]   = syslog_.queued();
    }
#endif

    // Sensor Status
    node = output["Sensor Info"].to<JsonObject>();
    if (EMSESP::sensor_enabled()) {
        node["temperature sensors"]      = EMSESP::temperaturesensor_.no_sensors();
        node["temperature sensor reads"] = EMSESP::temperaturesensor_.reads();
        node["temperature sensor fails"] = EMSESP::temperaturesensor_.fails();
    }
    if (EMSESP::analog_enabled()) {
        node["analog sensors"]      = EMSESP::analogsensor_.no_sensors();
        node["analog sensor reads"] = EMSESP::analogsensor_.reads();
        node["analog sensor fails"] = EMSESP::analogsensor_.fails();
    }

    // API Status
    node              = output["API Info"].to<JsonObject>();
    node["API calls"] = WebAPIService::api_count();
    node["API fails"] = WebAPIService::api_fails();

    // EMS Bus Status
    node = output["Bus Info"].to<JsonObject>();
    switch (EMSESP::bus_status()) {
    case EMSESP::BUS_STATUS_OFFLINE:
        node["bus status"] = "disconnected";
        break;
    case EMSESP::BUS_STATUS_TX_ERRORS:
        node["bus status"] = "connected, tx issues - try a different Tx Mode";
        break;
    case EMSESP::BUS_STATUS_CONNECTED:
        node["bus status"] = "connected";
        break;
    default:
        node["bus status"] = "unknown";
        break;
    }
    // if (EMSESP::bus_status() != EMSESP::BUS_STATUS_OFFLINE) {
    node["bus protocol"]                = EMSbus::is_ht3() ? "HT3" : "Buderus";
    node["bus telegrams received (rx)"] = EMSESP::rxservice_.telegram_count();
    node["bus reads (tx)"]              = EMSESP::txservice_.telegram_read_count();
    node["bus writes (tx)"]             = EMSESP::txservice_.telegram_write_count();
    node["bus incomplete telegrams"]    = EMSESP::rxservice_.telegram_error_count();
    node["bus reads failed"]            = EMSESP::txservice_.telegram_read_fail_count();
    node["bus writes failed"]           = EMSESP::txservice_.telegram_write_fail_count();
    node["bus rx line quality"]         = EMSESP::rxservice_.quality();
    node["bus tx line quality"]         = (EMSESP::txservice_.read_quality() + EMSESP::txservice_.read_quality()) / 2;
    // }

    // Settings
    node = output["Settings"].to<JsonObject>();
    EMSESP::webSettingsService.read([&](WebSettings & settings) {
        node["board profile"] = settings.board_profile;
        node["locale"]        = settings.locale;
        node["tx mode"]       = settings.tx_mode;
        node["ems bus id"]    = settings.ems_bus_id;
        node["shower timer"]  = settings.shower_timer;
        node["shower alert"]  = settings.shower_alert;
        if (settings.shower_alert) {
            node["shower alert coldshot"] = settings.shower_alert_coldshot; // seconds
            node["shower alert trigger"]  = settings.shower_alert_trigger;  // minutes
        }
        if (settings.board_profile == "CUSTOM") {
            node["phy type"] = settings.phy_type;
            if (settings.phy_type != PHY_type::PHY_TYPE_NONE) {
                node["eth power"]      = settings.eth_power;
                node["eth phy addr"]   = settings.eth_phy_addr;
                node["eth clock_mode"] = settings.eth_clock_mode;
            }
            node["rx gpio"]      = settings.rx_gpio;
            node["tx gpio"]      = settings.tx_gpio;
            node["dallas gpio"]  = settings.dallas_gpio;
            node["pbutton gpio"] = settings.pbutton_gpio;
            node["led gpio"]     = settings.led_gpio;
        }
        node["hide led"]           = settings.hide_led;
        node["notoken api"]        = settings.notoken_api;
        node["readonly mode"]      = settings.readonly_mode;
        node["fahrenheit"]         = settings.fahrenheit;
        node["dallas parasite"]    = settings.dallas_parasite;
        node["bool format"]        = settings.bool_format;
        node["bool dashboard"]     = settings.bool_dashboard;
        node["enum format"]        = settings.enum_format;
        node["analog enabled"]     = settings.analog_enabled;
        node["telnet enabled"]     = settings.telnet_enabled;
        node["max web log buffer"] = settings.weblog_buffer;
        node["web log buffer"]     = EMSESP::webLogService.num_log_messages();
    });

    // Devices - show EMS devices if we have any
    if (!EMSESP::emsdevices.empty()) {
        JsonArray devices = output["Devices"].to<JsonArray>();
        for (const auto & device_class : EMSFactory::device_handlers()) {
            for (const auto & emsdevice : EMSESP::emsdevices) {
                if (emsdevice && (emsdevice->device_type() == device_class.first)) {
                    JsonObject obj    = devices.add<JsonObject>();
                    obj["type"]       = emsdevice->device_type_name(); // non translated name
                    obj["name"]       = emsdevice->name();
                    obj["device id"]  = Helpers::hextoa(emsdevice->device_id());
                    obj["product id"] = emsdevice->product_id();
                    obj["version"]    = emsdevice->version();
                    obj["entities"]   = emsdevice->count_entities();
                    char result[300];
                    (void)emsdevice->show_telegram_handlers(result, sizeof(result), EMSdevice::Handlers::RECEIVED);
                    if (result[0] != '\0') {
                        obj["handlers received"] = result; // don't show handlers if there aren't any
                    }
                    (void)emsdevice->show_telegram_handlers(result, sizeof(result), EMSdevice::Handlers::FETCHED);
                    if (result[0] != '\0') {
                        obj["handlers fetched"] = result;
                    }
                    (void)emsdevice->show_telegram_handlers(result, sizeof(result), EMSdevice::Handlers::PENDING);
                    if (result[0] != '\0') {
                        obj["handlers pending"] = result;
                    }
                    (void)emsdevice->show_telegram_handlers(result, sizeof(result), EMSdevice::Handlers::IGNORED);
                    if (result[0] != '\0') {
                        obj["handlers ignored"] = result;
                    }
                }
            }
        }
    }

    return true;
}

#if defined(EMSESP_TEST)
// run a test, e.g. http://ems-esp/api?device=system&cmd=test&data=boiler
bool System::command_test(const char * value, const int8_t id) {
    return Test::test(value, id);
}
#endif

// takes a board profile and populates a data array with GPIO configurations
// returns false if profile is unknown
//
// data = led, dallas, rx, tx, button, phy_type, eth_power, eth_phy_addr, eth_clock_mode
//
// clock modes:
//  0 = RMII clock input to GPIO0
//  1 = RMII clock output from GPIO0
//  2 = RMII clock output from GPIO16
//  3 = RMII clock output from GPIO17, for 50hz inverted clock
bool System::load_board_profile(std::vector<int8_t> & data, const std::string & board_profile) {
    if (board_profile == "S32") {
        data = {2, 18, 23, 5, 0, PHY_type::PHY_TYPE_NONE, 0, 0, 0}; // BBQKees Gateway S32
    } else if (board_profile == "E32") {
        data = {2, 4, 5, 17, 33, PHY_type::PHY_TYPE_LAN8720, 16, 1, 0}; // BBQKees Gateway E32
    } else if (board_profile == "E32V2") {
        data = {2, 14, 4, 5, 34, PHY_type::PHY_TYPE_LAN8720, 15, 0, 1}; // BBQKees Gateway E32 V2
    } else if (board_profile == "MH-ET") {
        data = {2, 18, 23, 5, 0, PHY_type::PHY_TYPE_NONE, 0, 0, 0}; // MH-ET Live D1 Mini
    } else if (board_profile == "NODEMCU") {
        data = {2, 18, 23, 5, 0, PHY_type::PHY_TYPE_NONE, 0, 0, 0}; // NodeMCU 32S
    } else if (board_profile == "LOLIN") {
        data = {2, 18, 17, 16, 0, PHY_type::PHY_TYPE_NONE, 0, 0, 0}; // Lolin D32
    } else if (board_profile == "OLIMEX") {
        data = {0, 0, 36, 4, 34, PHY_type::PHY_TYPE_LAN8720, -1, 0, 0}; // Olimex ESP32-EVB (uses U1TXD/U1RXD/BUTTON, no LED or Temperature sensor)
    } else if (board_profile == "OLIMEXPOE") {
        data = {0, 0, 36, 4, 34, PHY_type::PHY_TYPE_LAN8720, 12, 0, 3}; // Olimex ESP32-POE
    } else if (board_profile == "C3MINI") {
        data = {7, 1, 4, 5, 9, PHY_type::PHY_TYPE_NONE, 0, 0, 0}; // Lolin C3 Mini
    } else if (board_profile == "S2MINI") {
        data = {15, 7, 11, 12, 0, PHY_type::PHY_TYPE_NONE, 0, 0, 0}; // Lolin S2 Mini
    } else if (board_profile == "S3MINI") {
        data = {17, 18, 8, 5, 0, PHY_type::PHY_TYPE_NONE, 0, 0, 0}; // Liligo S3
    } else if (board_profile == "S32S3") {
        data = {2, 18, 5, 17, 0, PHY_type::PHY_TYPE_NONE, 0, 0, 0}; // BBQKees Gateway S3
    } else if (board_profile == "CUSTOM") {
        // send back current values
        data = {(int8_t)EMSESP::system_.led_gpio_,
                (int8_t)EMSESP::system_.dallas_gpio_,
                (int8_t)EMSESP::system_.rx_gpio_,
                (int8_t)EMSESP::system_.tx_gpio_,
                (int8_t)EMSESP::system_.pbutton_gpio_,
                (int8_t)EMSESP::system_.phy_type_,
                EMSESP::system_.eth_power_,
                (int8_t)EMSESP::system_.eth_phy_addr_,
                (int8_t)EMSESP::system_.eth_clock_mode_};
    } else {
        // unknown, return false
        return false;
    }

    return true;
}

// restart command - perform a hard reset
bool System::command_restart(const char * value, const int8_t id) {
    if (value != nullptr && value[0] != '\0') {
        EMSESP::system_.system_restart(value);
    } else {
        EMSESP::system_.system_restart();
    }
    return true;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch"

std::string System::reset_reason(uint8_t cpu) const {
#ifndef EMSESP_STANDALONE
    switch (rtc_get_reset_reason(cpu)) {
    case 1:
        return ("Power on reset");
    // case 2 :reset pin not on esp32
    case 3:
        return ("Software reset");
    case 4: // not on S2, C3
        return ("Legacy watch dog reset");
    case 5:
        return ("Deep sleep reset");
    case 6: // not on S2, C3
        return ("Reset by SDIO");
    case 7:
        return ("Timer group0 watch dog reset");
    case 8:
        return ("Timer group1 watch dog reset");
    case 9:
        return ("RTC watch dog reset");
    case 10:
        return ("Intrusion reset CPU");
    case 11:
        return ("Timer group reset CPU");
    case 12:
        return ("Software reset CPU");
    case 13:
        return ("RTC watch dog reset: CPU");
    case 14: // not on S2, C3
        return ("APP CPU reset by PRO CPU");
    case 15:
        return ("Brownout reset");
    case 16:
        return ("RTC watch dog reset: CPU+RTC");
    default:
        break;
    }
#endif
    return ("Unknown");
}
#pragma GCC diagnostic pop

// set NTP status
void System::ntp_connected(bool b) {
    if (b != ntp_connected_ && !b) {
        LOG_WARNING("NTP disconnected"); // if turned off report it
    }

    ntp_connected_  = b;
    ntp_last_check_ = b ? uuid::get_uptime_sec() : 0;
}

// get NTP status
bool System::ntp_connected() {
    // timeout 2 hours, ntp sync is normally every hour.
    if ((uuid::get_uptime_sec() - ntp_last_check_ > 7201) && ntp_connected_) {
        ntp_connected(false);
    }

    return ntp_connected_;
}

} // namespace emsesp
