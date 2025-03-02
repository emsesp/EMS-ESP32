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

#include "system.h"
#include "emsesp.h" // for send_raw_telegram() command

#ifndef EMSESP_STANDALONE
#include "esp_ota_ops.h"
#endif

#include <semver200.h>

#if defined(EMSESP_TEST)
#include "../test/test.h"
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
#include <esp_mac.h>
#endif

#include <HTTPClient.h>

namespace emsesp {

// Languages supported. Note: the order is important
// and must match locale_translations.h and common.h
#if defined(EMSESP_TEST)
// in Test mode use two languages (en & de) to save flash memory needed for the tests
const char * const languages[] = {EMSESP_LOCALE_EN, EMSESP_LOCALE_DE};
#elif defined(EMSESP_EN_ONLY)
// EN only
const char * const languages[] = {EMSESP_LOCALE_EN};
#elif defined(EMSESP_DE_ONLY)
// EN + DE
const char * const languages[] = {EMSESP_LOCALE_EN, EMSESP_LOCALE_DE};
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
                                  EMSESP_LOCALE_SK,
                                  EMSESP_LOCALE_CZ};
#endif

static constexpr uint8_t NUM_LANGUAGES = sizeof(languages) / sizeof(const char *);

uuid::log::Logger System::logger_{F_(system), uuid::log::Facility::KERN};

// init statics
PButton  System::myPButton_;
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
    return 0; // EN only
}

// send raw to ems
bool System::command_send(const char * value, const int8_t id) {
    return EMSESP::txservice_.send_raw(value); // ignore id
}

// returns last response from MQTT
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

// fetch device values
bool System::command_fetch(const char * value, const int8_t id) {
    std::string value_s;
    if (Helpers::value2string(value, value_s)) {
        if (value_s == "all") {
            LOG_INFO("Requesting data from EMS devices");
            EMSESP::fetch_device_values();
        } else if (value_s == F_(boiler)) {
            EMSESP::fetch_device_values_type(EMSdevice::DeviceType::BOILER);
        } else if (value_s == F_(thermostat)) {
            EMSESP::fetch_device_values_type(EMSdevice::DeviceType::THERMOSTAT);
        } else if (value_s == F_(solar)) {
            EMSESP::fetch_device_values_type(EMSdevice::DeviceType::SOLAR);
        } else if (value_s == F_(mixer)) {
            EMSESP::fetch_device_values_type(EMSdevice::DeviceType::MIXER);
        }
    } else {
        EMSESP::fetch_device_values(); // default if no name or id is given
    }

    return true; // always true
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

// send message - to log and MQTT
bool System::command_message(const char * value, const int8_t id) {
    if (value == nullptr || value[0] == '\0') {
        return false; // must have a string value
    }

    LOG_INFO("Message: %s", value);
    Mqtt::queue_publish(F_(message), value);

    return true;
}

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
    if (Command::find_command(EMSdevice::DeviceType::BOILER, 0, "nompower", 0) != nullptr) {
        Command::call(EMSdevice::DeviceType::BOILER, "nompower", "-1"); // trigger a write
    }
    EMSESP::analogsensor_.store_counters();
    EMSESP::nvs_.end();
}

// restart EMS-ESP
// app0 or app1
// on 16MB we have the additional boot and factory partitions
void System::system_restart(const char * partitionname) {
#ifndef EMSESP_STANDALONE
    // see if we are forcing a partition to use
    if (partitionname != nullptr) {
        // Factory partition - label will be "factory"
        const esp_partition_t * partition = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_FACTORY, NULL);
        if (partition && strcmp(partition->label, partitionname) == 0) {
            esp_ota_set_boot_partition(partition);
        } else
            // try and find the partition by name
            if (strcmp(esp_ota_get_running_partition()->label, partitionname) != 0) {
                partition = esp_ota_get_next_update_partition(nullptr);
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
                // check if partition is empty
                uint64_t buffer;
                esp_partition_read(partition, 0, &buffer, 8);
                if (buffer == 0xFFFFFFFFFFFFFFFF) {
                    LOG_ERROR("Partition '%s' is empty, not bootable", partition->label);
                    return;
                }
                // set the boot partition
                esp_ota_set_boot_partition(partition);
            }
        LOG_INFO("Restarting EMS-ESP from %s partition", partitionname);
    } else {
        LOG_INFO("Restarting EMS-ESP...");
    }

    // make sure it's only executed once
    EMSESP::system_.systemStatus(SYSTEM_STATUS::SYSTEM_STATUS_NORMAL);

    store_nvs_values(); // save any NVS values
    Shell::loop_all();  // flush log to output
    Mqtt::disconnect(); // gracefully disconnect MQTT, needed for QOS1
    delay(1000);        // wait 1 second
    ESP.restart();
#else
    EMSESP::system_.systemStatus(SYSTEM_STATUS::SYSTEM_STATUS_NORMAL);
    if (partitionname != nullptr) {
        LOG_INFO("Restarting EMS-ESP from %s partition", partitionname);
    } else {
        LOG_INFO("Restarting EMS-ESP...");
    }
#endif
}

// saves all settings
void System::wifi_reconnect() {
    EMSESP::esp32React.getNetworkSettingsService()->read(
        [](NetworkSettings & networkSettings) { LOG_INFO("WiFi reconnecting to SSID '%s'...", networkSettings.ssid.c_str()); });
    delay(500);                                                           // wait
    EMSESP::webSettingsService.save();                                    // save local settings
    EMSESP::esp32React.getNetworkSettingsService()->callUpdateHandlers(); // in case we've changed ssid or password
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
        led_type_       = settings.led_type;
        led_gpio_       = settings.led_gpio;
        board_profile_  = settings.board_profile;
        telnet_enabled_ = settings.telnet_enabled;

        modbus_enabled_     = settings.modbus_enabled;
        modbus_port_        = settings.modbus_port;
        modbus_max_clients_ = settings.modbus_max_clients;
        modbus_timeout_     = settings.modbus_timeout;

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

        locale_         = settings.locale;
        developer_mode_ = settings.developer_mode;
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

    // extra check for pins 21 and 22 (I2C) when ethernet is onboard
    if ((EMSESP::system_.ethernet_connected() || EMSESP::system_.phy_type_ != PHY_type::PHY_TYPE_NONE) && (pin >= 21 && pin <= 22)) {
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
    appused_ = ESP.getSketchSize() / 1024;
    appfree_ = esp_ota_get_running_partition()->size / 1024 - appused_;
    refreshHeapMem(); // refresh free heap and max alloc heap
#if CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32S2
#if ESP_IDF_VERSION_MAJOR < 5
    temp_sensor_config_t temp_sensor = TSENS_CONFIG_DEFAULT();
    temp_sensor_get_config(&temp_sensor);
    temp_sensor.dac_offset = TSENS_DAC_DEFAULT; // DEFAULT: range:-10℃ ~  80℃, error < 1℃.
    temp_sensor_set_config(temp_sensor);
    temp_sensor_start();
    temp_sensor_read_celsius(&temperature_);
#else
    temperature_sensor_config_t temp_sensor_config = TEMPERATURE_SENSOR_CONFIG_DEFAULT(-10, 80);
    temperature_sensor_install(&temp_sensor_config, &temperature_handle_);
    temperature_sensor_enable(temperature_handle_);
    temperature_sensor_get_celsius(temperature_handle_, &temperature_);
#endif
#endif
#endif

    EMSESP::esp32React.getNetworkSettingsService()->read([&](NetworkSettings & networkSettings) {
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
    LOG_NOTICE("Button pressed - single click");

#if defined(EMSESP_TEST)
#ifndef EMSESP_STANDALONE
    // show filesystem
    Test::listDir(LittleFS, "/", 3);
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
    LOG_NOTICE("Button pressed - long press - restart from factory/boot partition");
    EMSESP::system_.system_restart("boot"); // this is default when first installed. it may contain the bootloader code.
}

// button indefinite press - do nothing for now
void System::button_OnVLongPress(PButton & b) {
    LOG_NOTICE("Button pressed - very long press");
}

// push button
void System::button_init(bool refresh) {
    if (refresh) {
        reload_settings();
    }

#ifndef EMSESP_STANDALONE
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
#endif
}

// set the LED to on or off when in normal operating mode
void System::led_init(bool refresh) {
    if (refresh) {
        // disabled old led port before setting new one
        if ((led_gpio_ != 0) && is_valid_gpio(led_gpio_)) {
            led_type_ ? neopixelWrite(led_gpio_, 0, 0, 0) : digitalWrite(led_gpio_, !LED_ON);
            pinMode(led_gpio_, INPUT);
        }
        reload_settings();
    }

    if ((led_gpio_ != 0) && is_valid_gpio(led_gpio_)) { // 0 means disabled
        if (led_type_) {
            // rgb LED WS2812B, use Neopixel
            neopixelWrite(led_gpio_, 0, 0, 0);
        } else {
            pinMode(led_gpio_, OUTPUT);
            digitalWrite(led_gpio_, !LED_ON); // start with LED off
        }
    }
}

// checks system health and handles LED flashing wizardry
void System::loop() {
    // check if we're supposed to do a reset/restart
    if (systemStatus() == SYSTEM_STATUS::SYSTEM_STATUS_RESTART_REQUESTED) {
        system_restart();
    }

#ifndef EMSESP_STANDALONE
    myPButton_.check(); // check button press

    // syslog
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
    // doc["event"]   = "connected";
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

    output["rxreceived"] = EMSESP::rxservice_.telegram_count();
    output["rxfails"]    = EMSESP::rxservice_.telegram_error_count();
    output["txreads"]    = EMSESP::txservice_.telegram_read_count();
    output["txwrites"]   = EMSESP::txservice_.telegram_write_count();
    output["txfails"]    = EMSESP::txservice_.telegram_read_fail_count() + EMSESP::txservice_.telegram_write_fail_count();

    if (Mqtt::enabled()) {
        output["mqttcount"]      = Mqtt::publish_count();
        output["mqttfails"]      = Mqtt::publish_fails();
        output["mqttreconnects"] = Mqtt::connect_count();
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
#if CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32
    output["temperature"] = (int)temperature_;
#endif
#endif

#ifndef EMSESP_STANDALONE
    if (!ethernet_connected_) {
        int8_t rssi              = WiFi.RSSI();
        output["rssi"]           = rssi;
        output["wifistrength"]   = wifi_quality(rssi);
        output["wifireconnects"] = EMSESP::esp32React.getWifiReconnects();
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
    EMSESP::esp32React.getNetworkSettingsService()->read([&](NetworkSettings & settings) { disableEth = settings.ssid.length() > 0; });

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

    // reset power and add a delay as ETH doesn't not always start up correctly after a warm boot
    if (eth_power_ != -1) {
        pinMode(eth_power_, OUTPUT);
        digitalWrite(eth_power_, LOW);
        delay(500);
        digitalWrite(eth_power_, HIGH);
    }

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

#ifndef EMSESP_STANDALONE
#if defined(CONFIG_IDF_TARGET_ESP32)
        uint8_t raw  = temprature_sens_read();
        temperature_ = (raw - 32) / 1.8f; // convert to Celsius
#elif CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32S2
#if ESP_IDF_VERSION_MAJOR < 5
        temp_sensor_read_celsius(&temperature_);
#else
        temperature_sensor_get_celsius(temperature_handle_, &temperature_);
#endif
#endif
#endif

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
                    led_type_ ? neopixelWrite(led_gpio_, 0, hide_led_ ? 0 : 128, 0) : digitalWrite(led_gpio_, hide_led_ ? !LED_ON : LED_ON);
                }
            } else {
                // turn off LED so we're ready to the flashes
                if (led_gpio_) {
                    led_type_ ? neopixelWrite(led_gpio_, 0, 0, 0) : digitalWrite(led_gpio_, !LED_ON);
                }
            }
        }
    }
}

// commands - takes static function pointers
// can be called via Console using 'call system <cmd>'
void System::commands_init() {
    Command::add(EMSdevice::DeviceType::SYSTEM, F_(read), System::command_read, FL_(read_cmd), CommandFlag::ADMIN_ONLY);
    Command::add(EMSdevice::DeviceType::SYSTEM, F_(send), System::command_send, FL_(send_cmd), CommandFlag::ADMIN_ONLY);
    Command::add(EMSdevice::DeviceType::SYSTEM, F_(fetch), System::command_fetch, FL_(fetch_cmd), CommandFlag::ADMIN_ONLY);
    Command::add(EMSdevice::DeviceType::SYSTEM, F_(restart), System::command_restart, FL_(restart_cmd), CommandFlag::ADMIN_ONLY);
    Command::add(EMSdevice::DeviceType::SYSTEM, F_(format), System::command_format, FL_(format_cmd), CommandFlag::ADMIN_ONLY);
    Command::add(EMSdevice::DeviceType::SYSTEM, F_(watch), System::command_watch, FL_(watch_cmd));
    Command::add(EMSdevice::DeviceType::SYSTEM, F_(message), System::command_message, FL_(message_cmd));
#if defined(EMSESP_TEST)
    Command::add(EMSdevice::DeviceType::SYSTEM, ("test"), System::command_test, FL_(test_cmd));
#endif

    // these commands will return data in JSON format
    Command::add(EMSdevice::DeviceType::SYSTEM, F("response"), System::command_response, FL_(commands_response));

    // MQTT subscribe "ems-esp/system/#"
    Mqtt::subscribe(EMSdevice::DeviceType::SYSTEM, "system/#", nullptr); // use empty function callback
}

// uses LED to show system health
// 1 x flash = the EMS bus is not connected
// 2 x flash = the network (wifi or ethernet) is not connected
// 3 x flash = both EMS bus and network are failing. This is a critical error!
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
            led_long_timer_ = uuid::get_uptime();
            led_flash_step_ = 0;
            led_type_ ? neopixelWrite(led_gpio_, 0, 0, 0) : digitalWrite(led_gpio_, !LED_ON); // LED off
        } else if (led_flash_step_ % 2) {
            // handle the step events (on odd numbers 3,5,7,etc). see if we need to turn on a LED
            //  1 flash is the EMS bus is not connected
            //  2 flashes if the network (wifi or ethernet) is not connected
            //  3 flashes is both the bus and the network are not connected. Then you know you're truly f*cked.

            if (led_type_) {
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
            } else {
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
            }
        } else {
            // turn the led off after the flash, on even number count
            if (led_on_) {
                led_type_ ? neopixelWrite(led_gpio_, 0, 0, 0) : digitalWrite(led_gpio_, !LED_ON);
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
    EMSESP::esp32React.getSecuritySettingsService()->read([&](SecuritySettings & securitySettings) {
        for (const User & user : securitySettings.users) {
            shell.printfln(" username: %s, password: %s, is_admin: %s", user.username.c_str(), user.password.c_str(), user.admin ? ("yes") : ("no"));
        }
    });
#endif

    shell.println();
}

// shell command 'show system'
void System::show_system(uuid::console::Shell & shell) {
    refreshHeapMem(); // refresh free heap and max alloc heap

    shell.println();
    shell.println("System:");
    shell.printfln(" Version: %s", EMSESP_APP_VERSION);
#ifndef EMSESP_STANDALONE
    shell.printfln(" Platform: %s (%s)", EMSESP_PLATFORM, ESP.getChipModel());
    shell.printfln(" Model: %s", getBBQKeesGatewayDetails().c_str());
    shell.printfln(" Partition: %s", esp_ota_get_running_partition()->label);
#endif
    shell.printfln(" Language: %s", locale().c_str());
    shell.printfln(" Board profile: %s", board_profile().c_str());
    shell.printfln(" Uptime: %s", uuid::log::format_timestamp_ms(uuid::get_uptime_ms(), 3).c_str());
#ifndef EMSESP_STANDALONE
    // https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/misc_system_api.html
    unsigned char mac_base[6] = {0};
    esp_efuse_mac_get_default(mac_base);
    esp_read_mac(mac_base, ESP_MAC_WIFI_STA);
    shell.printfln(" Base MAC Address: %02X:%02X:%02X:%02X:%02X:%02X", mac_base[0], mac_base[1], mac_base[2], mac_base[3], mac_base[4], mac_base[5]);

    shell.printfln(" SDK version: %s", ESP.getSdkVersion());
    shell.printfln(" CPU frequency: %lu MHz", ESP.getCpuFreqMHz());
#if CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32S2
    shell.printfln(" CPU temperature: %d °C", (int)temperature());
#endif
    shell.printfln(" Free heap/Max alloc: %lu KB / %lu KB", getHeapMem(), getMaxAllocMem());
    shell.printfln(" App used/free: %lu KB / %lu KB", appUsed(), appFree());
    uint32_t FSused = LittleFS.usedBytes() / 1024;
    shell.printfln(" FS used/free: %lu KB / %lu KB", FSused, FStotal() - FSused);
    shell.printfln(" Flash size: %lu KB", ESP.getFlashChipSize() / 1024);
    if (PSram()) {
        shell.printfln(" PSRAM size/free: %lu KB / %lu KB", PSram(), ESP.getFreePsram() / 1024);
    } else {
        shell.printfln(" PSRAM: not available");
    }

    shell.println();
    shell.println("Network:");

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

    // case WL_NO_SHIELD:
    default:
        shell.printfln(" WiFi MAC address: %s", WiFi.macAddress().c_str());
        shell.printfln(" WiFi Network: not connected");
        break;
    }

    // show Ethernet if connected
    if (ethernet_connected_) {
        shell.println();
        shell.printfln(" Ethernet Status: connected");
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
        shell.printfln(F_(host_fmt), !syslog_host_.isEmpty() ? syslog_host_.c_str() : F_(unset));
        shell.printfln(" IP: %s", uuid::printable_to_string(syslog_.ip()).c_str());
        shell.print(" ");
        shell.printfln(F_(port_fmt), syslog_port_);
        shell.print(" ");
        shell.printfln(F_(log_level_fmt), uuid::log::format_level_lowercase(static_cast<uuid::log::Level>(syslog_level_)));
        shell.print(" ");
        shell.printfln(F_(mark_interval_fmt), syslog_mark_interval_);
        shell.printfln(" Queued: %d", syslog_.queued());
    }

    shell.println();

#endif
}

// see if there is a restore of an older settings file that needs to be applied
// note there can be only one file at a time
bool System::check_restore() {
    bool reboot_required = false; // true if we need to reboot

#ifndef EMSESP_STANDALONE
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
            } else if (settings_type == "customizations") {
                // it's a customization file, just replace it and there's no need to reboot
                saveSettings(EMSESP_CUSTOMIZATION_FILE, "Customizations", input);
            } else if (settings_type == "schedule") {
                // it's a schedule file, just replace it and there's no need to reboot
                saveSettings(EMSESP_SCHEDULER_FILE, "Schedule", input);
            } else if (settings_type == "entities") {
                // it's a entity file, just replace it and there's no need to reboot
                saveSettings(EMSESP_CUSTOMENTITY_FILE, "Entities", input);
            } else if (settings_type == "customSupport") {
                // it's a custom support file - save it to /config
                new_file.close();
                if (LittleFS.rename(TEMP_FILENAME_PATH, EMSESP_CUSTOMSUPPORT_FILE)) {
                    LOG_INFO("Custom support file stored");
                    return false; // no need to reboot
                } else {
                    LOG_ERROR("Failed to save custom support file");
                }
            } else {
                LOG_ERROR("Unrecognized file uploaded");
            }
        } else {
            LOG_ERROR("Unrecognized file uploaded, not json.");
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
    std::string settingsVersion;

    if (!factory_settings) {
        // fetch current version from settings file
        EMSESP::webSettingsService.read([&](WebSettings const & settings) { settingsVersion = settings.version.c_str(); });

        // see if we're missing a version, will be < 3.5.0b13 from Dec 23 2022
        missing_version = (settingsVersion.empty() || (settingsVersion.length() < 5));
        if (missing_version) {
            LOG_WARNING("No version information found");
            settingsVersion = "3.5.0"; // this was the last stable version without version info
        }
    } else {
        settingsVersion = EMSESP_APP_VERSION; // use the current version
    }

    version::Semver200_version settings_version(settingsVersion);

    if (!missing_version) {
        LOG_DEBUG("Checking for version upgrades (settings file has v%d.%d.%d-%s)",
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
        // we need to do an upgrade
        if (missing_version) {
            LOG_NOTICE("Upgrading to version %d.%d.%d-%s", this_version.major(), this_version.minor(), this_version.patch(), this_version.prerelease().c_str());
        } else {
            LOG_NOTICE("Upgrading from version %d.%d.%d-%s to %d.%d.%d-%s",
                       settings_version.major(),
                       settings_version.minor(),
                       settings_version.patch(),
                       settings_version.prerelease().c_str(),
                       this_version.major(),
                       this_version.minor(),
                       this_version.patch(),
                       this_version.prerelease().c_str());
        }

        // if we're coming from 3.4.4 or 3.5.0b14 which had no version stored then we need to apply new settings
        if (missing_version) {
            LOG_INFO("Upgrade: Setting MQTT Entity ID format to older v3.4 format (0)");
            EMSESP::esp32React.getMqttSettingsService()->update([&](MqttSettings & mqttSettings) {
                mqttSettings.entity_format = Mqtt::entityFormat::SINGLE_LONG; // use old Entity ID format from v3.4
                return StateUpdateResult::CHANGED;
            });
        } else if (settings_version.major() == 3 && settings_version.minor() <= 6) {
            EMSESP::esp32React.getMqttSettingsService()->update([&](MqttSettings & mqttSettings) {
                if (mqttSettings.entity_format == 1) {
                    mqttSettings.entity_format = Mqtt::entityFormat::SINGLE_OLD; // use old Entity ID format from v3.6
                    LOG_INFO("Upgrade: Setting MQTT Entity ID format to v3.6 format (3)");
                    return StateUpdateResult::CHANGED;
                } else if (mqttSettings.entity_format == 2) {
                    mqttSettings.entity_format = Mqtt::entityFormat::MULTI_OLD; // use old Entity ID format from v3.6
                    LOG_INFO("Upgrade: Setting MQTT Entity ID format to v3.6 format (4)");
                    return StateUpdateResult::CHANGED;
                }
                return StateUpdateResult::UNCHANGED;
            });
        }

        // changes to Network
        EMSESP::esp32React.getNetworkSettingsService()->update([&](NetworkSettings & networkSettings) {
            // Network Settings Wifi tx_power is now using the value * 4.
            if (networkSettings.tx_power == 20) {
                networkSettings.tx_power = WIFI_POWER_19_5dBm; // use 19.5 as we don't have 20 anymore
                LOG_INFO("Upgrade: Setting WiFi TX Power to Auto");
            }

            // force WiFi sleep to off (was default on < 3.7.0-dev-33)
            networkSettings.nosleep = true;
            LOG_INFO("Upgrade: Disabling WiFi nosleep");

            return StateUpdateResult::CHANGED;
        });

        // changes to application settings
        EMSESP::webSettingsService.update([&](WebSettings & settings) {
            // force web buffer to 25 for those boards without psram
            if (EMSESP::system_.PSram() == 0) {
                settings.weblog_buffer = 25;
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

    // if we did a change, set the new version and save it, then reboot
    if (save_version) {
        EMSESP::webSettingsService.update([&](WebSettings & settings) {
            settings.version = EMSESP_APP_VERSION;
            return StateUpdateResult::CHANGED;
        });
        // LOG_INFO("Upgrade: Setting version to %s", EMSESP_APP_VERSION);
        return true; // need reboot
    }

    return false;
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
            LOG_INFO("Applying new uploaded %s data", section);
            serializeJson(section_json, section_file);
            section_file.close();
            return true; // reboot required
        }
    }
#endif
    return false; // not found
}

// set a entity of services 'network', 'settings', 'mqtt', etc.
bool System::command_service(const char * cmd, const char * value) {
    bool ok = false;
    bool b;
    if (Helpers::value2bool(value, b)) {
        if (!strcmp(cmd, "settings/showertimer")) {
            EMSESP::webSettingsService.update([&](WebSettings & settings) {
                settings.shower_timer = b;
                return StateUpdateResult::CHANGED;
            });
            EMSESP::shower_.shower_timer(b);
            ok = true;
        } else if (!strcmp(cmd, "settings/showeralert")) {
            EMSESP::webSettingsService.update([&](WebSettings & settings) {
                settings.shower_alert = b;
                return StateUpdateResult::CHANGED;
            });
            EMSESP::shower_.shower_alert(b);
            ok = true;
        } else if (!strcmp(cmd, "settings/hideled")) {
            EMSESP::webSettingsService.update([&](WebSettings & settings) {
                settings.hide_led = b;
                return StateUpdateResult::CHANGED;
            });
            EMSESP::system_.hide_led(b);
            ok = true;
        } else if (!strcmp(cmd, "settings/analogenabled")) {
            EMSESP::webSettingsService.update([&](WebSettings & settings) {
                settings.analog_enabled = b;
                return StateUpdateResult::CHANGED;
            });
            EMSESP::system_.analog_enabled(b);
            ok = true;
        } else if (!strcmp(cmd, "mqtt/enabled")) {
            EMSESP::esp32React.getMqttSettingsService()->update([&](MqttSettings & Settings) {
                Settings.enabled = b;
                return StateUpdateResult::CHANGED;
            });
            ok = true;
        } else if (!strcmp(cmd, "ap/enabled")) {
            EMSESP::esp32React.getAPSettingsService()->update([&](APSettings & Settings) {
                Settings.provisionMode = b ? 0 : 2;
                return StateUpdateResult::CHANGED;
            });
            ok = true;
        } else if (!strcmp(cmd, "ntp/enabled")) {
            EMSESP::esp32React.getNTPSettingsService()->update([&](NTPSettings & Settings) {
                Settings.enabled = b;
                return StateUpdateResult::CHANGED;
            });
            ok = true;
        } else if (!strcmp(cmd, "syslog/enabled")) {
            EMSESP::webSettingsService.update([&](WebSettings & settings) {
                settings.syslog_enabled = b;
                return StateUpdateResult::CHANGED;
            });
            EMSESP::system_.syslog_enabled_ = b;
            EMSESP::system_.syslog_init();
            ok = true;
        }
    }

    if (ok) {
        LOG_INFO("System command '%s' with value '%s'", cmd, value);
    }
    return ok;
}

// return back a system value
bool System::get_value_info(JsonObject output, const char * cmd) {
    if (cmd == nullptr || strlen(cmd) == 0) {
        LOG_ERROR("empty system command");
        return false;
    }

    // check for hardcoded "info"/"value"
    if (!strcmp(cmd, F_(info)) || !strcmp(cmd, F_(values))) {
        return command_info("", 0, output);
    }

    // fetch all the data from the system in a different json
    JsonDocument doc;
    JsonObject   root = doc.to<JsonObject>();
    (void)command_info("", 0, root);

    // list all entities
    if (!strcmp(cmd, F_(entities))) {
        for (JsonPair p : root) {
            if (p.value().is<JsonObject>()) {
                for (JsonPair p1 : p.value().as<JsonObject>()) {
                    JsonObject entity = output[std::string(p.key().c_str()) + "." + p1.key().c_str()].to<JsonObject>();
                    get_value_json(entity, p.key().c_str(), p1.key().c_str(), p1.value());
                }
            }
        }
        return true;
    }

    char * val = (char *)strstr(cmd, "/value");
    if (val) {
        *val = '\0';
    }

    char * slash = (char *)strchr(cmd, '/');
    if (slash) {
        *slash = '\0';
        slash++;
    }

    // list values for a jsonObject in system, e.g. /api/system/network
    if (!slash || !strcmp(slash, F_(info)) || !strcmp(slash, F_(values))) {
        for (JsonPair p : root) {
            if (Helpers::toLower(p.key().c_str()) == cmd && p.value().is<JsonObject>()) {
                for (JsonPair p1 : p.value().as<JsonObject>()) {
                    output[p1.key().c_str()] = p1.value().as<std::string>();
                }
                return true;
            }
        }
        return false;
    }

    // value info or api_data for a single value
    // Loop through all the key-value pairs in root to find the key, case independent
    if (slash) { // search the top level first
        for (JsonPair p : root) {
            if (p.value().is<JsonObject>() && Helpers::toLower(p.key().c_str()) == cmd) {
                for (JsonPair p1 : p.value().as<JsonObject>()) {
                    if (Helpers::toLower(p1.key().c_str()) == slash && !p1.value().is<JsonObject>()) {
                        if (val) {
                            output["api_data"] = p1.value().as<std::string>();
                            return true;
                        }
                        get_value_json(output, p.key().c_str(), p1.key().c_str(), p1.value());
                        return true;
                    }
                }
            } // else skipt, but we don't have value pairs in system root
        }
    }
    return false;
}

void System::get_value_json(JsonObject output, const std::string & circuit, const std::string & name, JsonVariant val) {
    output["name"] = name;
    if (circuit.length()) {
        output["circuit"] = circuit;
    }
    output["readable"]  = true;
    output["writeable"] = (name == "showerTimer" || name == "showerAlert" || name == "enabled" || name == "hideLed" || name == "analogEnabled");
    output["visible"]   = true;
    if (val.is<bool>()) {
        output["value"] = val.as<bool>();
        output["type"]  = "boolean";
    } else if (val.is<String>() || val.is<const char *>() || val.is<std::string>()) {
        output["value"] = val.as<std::string>();
        output["type"]  = "string";
    } else {
        output["value"] = val.as<float>();
        output["type"]  = "number";
    }
}

// export status information including the device information
// http://ems-esp/api/system/info
bool System::command_info(const char * value, const int8_t id, JsonObject output) {
    JsonObject node;

    // System
    node = output["system"].to<JsonObject>();
// prevent false-negatives in Unity tests every time the version changes
#if defined(EMSESP_UNITY)
    node["version"] = "dev";
#else
    node["version"] = EMSESP_APP_VERSION;
#endif
    node["uptime"]    = uuid::log::format_timestamp_ms(uuid::get_uptime_ms(), 3);
    node["uptimeSec"] = uuid::get_uptime_sec();
#ifndef EMSESP_STANDALONE
    node["platform"]        = EMSESP_PLATFORM;
    node["cpuType"]         = ESP.getChipModel();
    node["arduino"]         = ARDUINO_VERSION;
    node["sdk"]             = ESP.getSdkVersion();
    node["freeMem"]         = getHeapMem();
    node["maxAlloc"]        = getMaxAllocMem();
    node["freeCaps"]        = heap_caps_get_free_size(MALLOC_CAP_8BIT) / 1024; // includes heap and psram
    node["usedApp"]         = EMSESP::system_.appUsed();                       // kilobytes
    node["freeApp"]         = EMSESP::system_.appFree();                       // kilobytes
    node["partition"]       = esp_ota_get_running_partition()->label;          // active partition
    node["flash_chip_size"] = ESP.getFlashChipSize() / 1024;                   // kilobytes
#endif
    node["resetReason"] = EMSESP::system_.reset_reason(0) + " / " + EMSESP::system_.reset_reason(1);
#ifndef EMSESP_STANDALONE
    node["psram"] = (EMSESP::system_.PSram() > 0); // make boolean
    if (EMSESP::system_.PSram()) {
        node["psramSize"] = EMSESP::system_.PSram();
        node["freePsram"] = ESP.getFreePsram() / 1024;
    }
    node["model"] = EMSESP::system_.getBBQKeesGatewayDetails();
#if CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32S2
    node["temperature"] = EMSESP::system_.temperature();
#endif

#endif

    // Network Status
    node = output["network"].to<JsonObject>();
#ifndef EMSESP_STANDALONE
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
        node["network"]        = "WiFi";
        node["hostname"]       = WiFi.getHostname();
        node["RSSI"]           = WiFi.RSSI();
        node["WIFIReconnects"] = EMSESP::esp32React.getWifiReconnects();
        // node["MAC"]             = WiFi.macAddress();
        // node["IPv4 address"]    = uuid::printable_to_string(WiFi.localIP()) + "/" + uuid::printable_to_string(WiFi.subnetMask());
        // node["IPv4 gateway"]    = uuid::printable_to_string(WiFi.gatewayIP());
        // node["IPv4 nameserver"] = uuid::printable_to_string(WiFi.dnsIP());
        // if (WiFi.localIPv6().toString() != "0000:0000:0000:0000:0000:0000:0000:0000") {
        //     node["IPv6 address"] = uuid::printable_to_string(WiFi.localIPv6());
        // }
    }
#else
    // for testing
    node["network"]  = "WiFi";
    node["hostname"] = "ems-esp";
    node["RSSI"]     = -23;
#endif
    EMSESP::esp32React.getNetworkSettingsService()->read([&](NetworkSettings & settings) {
        if (WiFi.status() == WL_CONNECTED && !settings.bssid.isEmpty()) {
            node["BSSID"] = "set"; // we don't disclose the name
        }
        node["TxPowerSetting"] = settings.tx_power;
        node["staticIP"]       = settings.staticIPConfig;
        node["lowBandwidth"]   = settings.bandwidth20;
        node["disableSleep"]   = settings.nosleep;
        node["enableMDNS"]     = settings.enableMDNS;
        node["enableCORS"]     = settings.enableCORS;
        if (settings.enableCORS) {
            node["CORSOrigin"] = settings.CORSOrigin;
        }
    });

#ifndef EMSESP_STANDALONE
    EMSESP::esp32React.getAPSettingsService()->read([&](const APSettings & settings) {
        const char * pM[]       = {"always", "disconnected", "never"};
        node["APProvisionMode"] = pM[settings.provisionMode];
        node["APSecurity"]      = settings.password.length() ? "wpa2" : "open";
        node["APSSID"]          = settings.ssid;
    });
#endif

    // NTP status
    node = output["ntp"].to<JsonObject>();
#ifndef EMSESP_STANDALONE
    node["NTPStatus"] = EMSESP::system_.ntp_connected() ? "connected" : "disconnected";
    EMSESP::esp32React.getNTPSettingsService()->read([&](const NTPSettings & settings) {
        node["enabled"] = settings.enabled;
        node["server"]  = settings.server;
        node["tzLabel"] = settings.tzLabel;
    });
#endif

    // MQTT Status
    node               = output["mqtt"].to<JsonObject>();
    node["MQTTStatus"] = Mqtt::connected() ? F_(connected) : F_(disconnected);
    if (Mqtt::enabled()) {
        node["MQTTPublishes"]    = Mqtt::publish_count();
        node["MQTTQueued"]       = Mqtt::publish_queued();
        node["MQTTPublishFails"] = Mqtt::publish_fails();
        node["MQTTReconnects"]   = Mqtt::connect_count();
    }
    EMSESP::esp32React.getMqttSettingsService()->read([&](const MqttSettings & settings) {
        node["enabled"]               = settings.enabled;
        node["clientID"]              = settings.clientId;
        node["keepAlive"]             = settings.keepAlive;
        node["cleanSession"]          = settings.cleanSession;
        node["entityFormat"]          = settings.entity_format;
        node["base"]                  = settings.base;
        node["discoveryPrefix"]       = settings.discovery_prefix;
        node["discoveryType"]         = settings.discovery_type;
        node["nestedFormat"]          = settings.nested_format;
        node["haEnabled"]             = settings.ha_enabled;
        node["mqttQos"]               = settings.mqtt_qos;
        node["mqttRetain"]            = settings.mqtt_retain;
        node["publishTimeHeartbeat"]  = settings.publish_time_heartbeat;
        node["publishTimeBoiler"]     = settings.publish_time_boiler;
        node["publishTimeThermostat"] = settings.publish_time_thermostat;
        node["publishTimeSolar"]      = settings.publish_time_solar;
        node["publishTimeMixer"]      = settings.publish_time_mixer;
        node["publishTimeWater"]      = settings.publish_time_water;
        node["publishTimeOther"]      = settings.publish_time_other;
        node["publishTimeSensor"]     = settings.publish_time_sensor;
        node["publishSingle"]         = settings.publish_single;
        node["publish2command"]       = settings.publish_single2cmd;
        node["sendResponse"]          = settings.send_response;
    });

    // Syslog Status
    node            = output["syslog"].to<JsonObject>();
    node["enabled"] = EMSESP::system_.syslog_enabled_;
#ifndef EMSESP_STANDALONE
    if (EMSESP::system_.syslog_enabled_) {
        node["syslogStarted"] = syslog_.started();
        node["syslogLevel"]   = FL_(list_syslog_level)[syslog_.log_level() + 1];
        node["syslogIP"]      = syslog_.ip();
        node["syslogQueue"]   = syslog_.queued();
    }
#endif

    // Sensor Status
    node = output["sensor"].to<JsonObject>();
    if (EMSESP::sensor_enabled()) {
        node["temperatureSensors"]     = EMSESP::temperaturesensor_.count_entities();
        node["temperatureSensorReads"] = EMSESP::temperaturesensor_.reads();
        node["temperatureSensorFails"] = EMSESP::temperaturesensor_.fails();
    }
    if (EMSESP::analog_enabled()) {
        node["analogSensors"]     = EMSESP::analogsensor_.count_entities();
        node["analogSensorReads"] = EMSESP::analogsensor_.reads();
        node["analogSensorFails"] = EMSESP::analogsensor_.fails();
    }

    // API Status
    node = output["api"].to<JsonObject>();

// if we're generating test data for Unit Tests we dont want to count these API calls as it will pollute the data response
#if defined(EMSESP_UNITY)
    node["APICalls"] = 0;
    node["APIFails"] = 0;
#else
    node["APICalls"] = WebAPIService::api_count();
    node["APIFails"] = WebAPIService::api_fails();
#endif

    // EMS Bus Status
    node = output["bus"].to<JsonObject>();
    switch (EMSESP::bus_status()) {
    case EMSESP::BUS_STATUS_OFFLINE:
        node["busStatus"] = "disconnected";
        break;
    case EMSESP::BUS_STATUS_TX_ERRORS:
        node["busStatus"] = "connected, tx issues - try a different Tx Mode";
        break;
    case EMSESP::BUS_STATUS_CONNECTED:
        node["busStatus"] = "connected";
        break;
    default:
        node["busStatus"] = "unknown";
        break;
    }
    node["busProtocol"]            = EMSbus::is_ht3() ? "HT3" : "Buderus";
    node["busTelegramsReceived"]   = EMSESP::rxservice_.telegram_count();
    node["busReads"]               = EMSESP::txservice_.telegram_read_count();
    node["busWrites"]              = EMSESP::txservice_.telegram_write_count();
    node["busIncompleteTelegrams"] = EMSESP::rxservice_.telegram_error_count();
    node["busReadsFailed"]         = EMSESP::txservice_.telegram_read_fail_count();
    node["busWritesFailed"]        = EMSESP::txservice_.telegram_write_fail_count();
    node["busRxLineQuality"]       = EMSESP::rxservice_.quality();
    node["busTxLineQuality"]       = (EMSESP::txservice_.read_quality() + EMSESP::txservice_.read_quality()) / 2;

    // Settings
    node = output["settings"].to<JsonObject>();
    EMSESP::webSettingsService.read([&](const WebSettings & settings) {
        node["boardProfile"]      = settings.board_profile;
        node["locale"]            = settings.locale;
        node["txMode"]            = settings.tx_mode;
        node["emsBusID"]          = settings.ems_bus_id;
        node["showerTimer"]       = settings.shower_timer;
        node["showerMinDuration"] = settings.shower_min_duration; // seconds
        node["showerAlert"]       = settings.shower_alert;
        if (settings.shower_alert) {
            node["showerAlertColdshot"] = settings.shower_alert_coldshot; // seconds
            node["showerAlertTrigger"]  = settings.shower_alert_trigger;  // minutes
        }
        if (settings.board_profile == "CUSTOM") {
            node["phyType"] = settings.phy_type;
            if (settings.phy_type != PHY_type::PHY_TYPE_NONE) {
                node["ethPower"]      = settings.eth_power;
                node["ethPhyAddr"]    = settings.eth_phy_addr;
                node["ethClockMmode"] = settings.eth_clock_mode;
            }
            node["rxGPIO"]      = settings.rx_gpio;
            node["txGPIO"]      = settings.tx_gpio;
            node["dallasGPIO"]  = settings.dallas_gpio;
            node["pbuttonGPIO"] = settings.pbutton_gpio;
            node["ledGPIO"]     = settings.led_gpio;
            node["ledType"]     = settings.led_type;
        }
        node["hideLed"]         = settings.hide_led;
        node["noTokenApi"]      = settings.notoken_api;
        node["readonlyMode"]    = settings.readonly_mode;
        node["fahrenheit"]      = settings.fahrenheit;
        node["dallasParasite"]  = settings.dallas_parasite;
        node["boolFormat"]      = settings.bool_format;
        node["boolDashboard"]   = settings.bool_dashboard;
        node["enumFormat"]      = settings.enum_format;
        node["analogEnabled"]   = settings.analog_enabled;
        node["telnetEnabled"]   = settings.telnet_enabled;
        node["maxWebLogBuffer"] = settings.weblog_buffer;

        /*
#if defined(EMSESP_UNITY)
        node["webLogBuffer"] = 0;
#else
        node["webLogBuffer"] = EMSESP::webLogService.num_log_messages();
#endif
*/
        node["modbusEnabled"]   = settings.modbus_enabled;
        node["forceHeatingOff"] = settings.boiler_heatingoff;
        node["developerMode"]   = settings.developer_mode;
    });

    // Devices - show EMS devices if we have any
    JsonArray devices = output["devices"].to<JsonArray>();
    if (!EMSESP::emsdevices.empty()) {
        for (const auto & device_class : EMSFactory::device_handlers()) {
            for (const auto & emsdevice : EMSESP::emsdevices) {
                if (emsdevice && (emsdevice->device_type() == device_class.first)) {
                    JsonObject obj   = devices.add<JsonObject>();
                    obj["type"]      = emsdevice->device_type_name(); // non translated name
                    obj["name"]      = emsdevice->name();             // custom name
                    obj["deviceID"]  = Helpers::hextoa(emsdevice->device_id());
                    obj["productID"] = emsdevice->product_id();
                    obj["brand"]     = emsdevice->brand_to_char();
                    obj["version"]   = emsdevice->version();
                    obj["entities"]  = emsdevice->count_entities();
                    char result[500];
                    (void)emsdevice->show_telegram_handlers(result, sizeof(result), EMSdevice::Handlers::RECEIVED);
                    if (result[0] != '\0') {
                        obj["handlersReceived"] = result; // don't show handlers if there aren't any
                    }
                    (void)emsdevice->show_telegram_handlers(result, sizeof(result), EMSdevice::Handlers::FETCHED);
                    if (result[0] != '\0') {
                        obj["handlersFetched"] = result;
                    }
                    (void)emsdevice->show_telegram_handlers(result, sizeof(result), EMSdevice::Handlers::PENDING);
                    if (result[0] != '\0') {
                        obj["handlersPending"] = result;
                    }
                    (void)emsdevice->show_telegram_handlers(result, sizeof(result), EMSdevice::Handlers::IGNORED);
                    if (result[0] != '\0') {
                        obj["handlersIgnored"] = result;
                    }
                }
            }
        }
    }
    // Also show EMSESP devices if we have any
    if (EMSESP::temperaturesensor_.count_entities()) {
        JsonObject obj  = devices.add<JsonObject>();
        obj["type"]     = F_(temperaturesensor);
        obj["name"]     = F_(temperaturesensor);
        obj["entities"] = EMSESP::temperaturesensor_.count_entities();
    }
    if (EMSESP::analogsensor_.count_entities()) {
        JsonObject obj  = devices.add<JsonObject>();
        obj["type"]     = F_(analogsensor);
        obj["name"]     = F_(analogsensor);
        obj["entities"] = EMSESP::analogsensor_.count_entities();
    }
    if (EMSESP::webSchedulerService.count_entities()) {
        JsonObject obj  = devices.add<JsonObject>();
        obj["type"]     = F_(scheduler);
        obj["name"]     = F_(scheduler);
        obj["entities"] = EMSESP::webSchedulerService.count_entities();
    }
    if (EMSESP::webCustomEntityService.count_entities()) {
        JsonObject obj  = devices.add<JsonObject>();
        obj["type"]     = F_(custom);
        obj["name"]     = F_(custom);
        obj["entities"] = EMSESP::webCustomEntityService.count_entities();
    }

    return true; // this function always returns true!
}

#if defined(EMSESP_TEST)
// run a test, e.g. http://ems-esp/api?device=system&cmd=test&data=boiler
bool System::command_test(const char * value, const int8_t id) {
    if (value) {
        return Test::test(value, id);
    } else {
        return false;
    }
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
        data = {2, 18, 23, 5, 0, PHY_type::PHY_TYPE_NONE, 0, 0, 0, 0}; // BBQKees Gateway S32
    } else if (board_profile == "E32") {
        data = {2, 4, 5, 17, 33, PHY_type::PHY_TYPE_LAN8720, 16, 1, 0, 0}; // BBQKees Gateway E32
    } else if (board_profile == "E32V2") {
        data = {2, 14, 4, 5, 34, PHY_type::PHY_TYPE_LAN8720, 15, 0, 1, 0}; // BBQKees Gateway E32 V2
    } else if (board_profile == "MH-ET") {
        data = {2, 18, 23, 5, 0, PHY_type::PHY_TYPE_NONE, 0, 0, 0, 0}; // MH-ET Live D1 Mini
    } else if (board_profile == "NODEMCU") {
        data = {2, 18, 23, 5, 0, PHY_type::PHY_TYPE_NONE, 0, 0, 0, 0}; // NodeMCU 32S
    } else if (board_profile == "LOLIN") {
        data = {2, 18, 17, 16, 0, PHY_type::PHY_TYPE_NONE, 0, 0, 0, 0}; // Lolin D32
    } else if (board_profile == "OLIMEX") {
        data = {0, 0, 36, 4, 34, PHY_type::PHY_TYPE_LAN8720, -1, 0, 0, 0}; // Olimex ESP32-EVB (uses U1TXD/U1RXD/BUTTON, no LED or Temperature sensor)
    } else if (board_profile == "OLIMEXPOE") {
        data = {0, 0, 36, 4, 34, PHY_type::PHY_TYPE_LAN8720, 12, 0, 3, 0}; // Olimex ESP32-POE
    } else if (board_profile == "C3MINI") {
#if defined(BOARD_C3_MINI_V1)
        data = {7, 1, 4, 5, 9, PHY_type::PHY_TYPE_NONE, 0, 0, 0, 0}; // Lolin C3 Mini V1
#else
        data = {7, 1, 4, 5, 9, PHY_type::PHY_TYPE_NONE, 0, 0, 0, 1}; // Lolin C3 Mini with RGB Led
#endif
    } else if (board_profile == "S2MINI") {
        data = {15, 7, 11, 12, 0, PHY_type::PHY_TYPE_NONE, 0, 0, 0, 0}; // Lolin S2 Mini
    } else if (board_profile == "S3MINI") {
        data = {17, 18, 8, 5, 0, PHY_type::PHY_TYPE_NONE, 0, 0, 0, 0}; // Liligo S3
    } else if (board_profile == "S32S3") {
        data = {2, 18, 5, 17, 0, PHY_type::PHY_TYPE_NONE, 0, 0, 0, 0}; // BBQKees Gateway S3
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
                (int8_t)EMSESP::system_.eth_clock_mode_,
                (int8_t)EMSESP::system_.led_type_};
    } else {
        LOG_DEBUG("Couldn't identify board profile %s", board_profile.c_str());
        return false; // unknown, return false
    }

    // LOG_DEBUG("Found data for board profile %s", board_profile.c_str());
    return true;
}

// format command - factory reset, removing all config fi`les
bool System::command_format(const char * value, const int8_t id) {
    LOG_INFO("Removing all config files");
#ifndef EMSESP_STANDALONE
    // TODO To replaced with LittleFS.rmdir(FS_CONFIG_DIRECTORY) now we're using IDF 4.2+
    File root = LittleFS.open(EMSESP_FS_CONFIG_DIRECTORY);
    File file;
    while ((file = root.openNextFile())) {
        String path = file.path();
        file.close();
        LittleFS.remove(path);
    }
#endif

    // restart will be handled by the main loop
    EMSESP::system_.systemStatus(SYSTEM_STATUS::SYSTEM_STATUS_RESTART_REQUESTED);
    return true;
}

// restart command - perform a hard reset (system reboot)
bool System::command_restart(const char * value, const int8_t id) {
    if (id == 0) {
        // if it has an id then it's a web call and we need to queue the restart
        // default id is -1 when calling /api/system/restart directly for example
        LOG_INFO("Preparing to restart system");
        EMSESP::system_.systemStatus(SYSTEM_STATUS::SYSTEM_STATUS_PENDING_RESTART);
        return true;
    }

    LOG_INFO("Restarting system immediately");
    // restart will be handled by the main loop
    EMSESP::system_.systemStatus(SYSTEM_STATUS::SYSTEM_STATUS_RESTART_REQUESTED);
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
    return "Unknown";
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

// see if its a BBQKees Gateway by checking the nvs values
String System::getBBQKeesGatewayDetails() {
#ifndef EMSESP_STANDALONE
    if (!EMSESP::nvs_.isKey("mfg")) {
        return "";
    }

    // mfg can be either "BBQKees" or "BBQKees Electronics"
    auto mfg = EMSESP::nvs_.getString("mfg");
    if (mfg) {
        if (!mfg.startsWith("BBQKees")) {
            return "";
        }
    }

    return "BBQKees Gateway Model " + EMSESP::nvs_.getString("model") + " v" + EMSESP::nvs_.getString("hwrevision") + "/" + EMSESP::nvs_.getString("batch");
#else
    return "";
#endif
}

// Stream from an URL and send straight to OTA uploader service.
//
// This function needs to be called twice, 1st pass once with a url to persist it, 2nd pass with no arguments to start the upload
// This is to avoid timeouts in callback functions, like calling from a web hook.
bool System::uploadFirmwareURL(const char * url) {
#ifndef EMSESP_STANDALONE

    static String saved_url;

    if (url && strlen(url) > 0) {
        // if the passed URL is "reset" abort the current upload. This is called when an error happens during OTA
        if (strncmp(url, "reset", 5) == 0) {
            LOG_DEBUG("Firmware upload - resetting");
            saved_url.clear();
            return true;
        }

        // given a URL to download from, save it ready for the 2nd pass
        saved_url = url;
        LOG_INFO("Firmware location: %s", saved_url.c_str());
        EMSESP::system_.systemStatus(SYSTEM_STATUS::SYSTEM_STATUS_PENDING_UPLOAD); // we're ready to start the upload
        return true;
    }

    // check we have a valid URL from the 1st pass
    if (saved_url.isEmpty()) {
        LOG_ERROR("Firmware upload failed - invalid URL");
        return false; // error
    }

    Shell::loop_all(); // flush log buffers so latest messages are shown in console

    // Configure temporary client
    HTTPClient http;
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS); // important for GitHub 302's
    http.setTimeout(8000);
    http.useHTTP10(true); // use HTTP/1.0 for update since the update handler does not support any transfer Encoding
    http.begin(saved_url);

    // start a connection, returns -1 if fails
    int httpCode = http.GET();
    if (httpCode != HTTP_CODE_OK) {
        LOG_ERROR("Firmware upload failed - HTTP code %d", httpCode);
        http.end();
        return false; // error
    }

    // check we have enough space for the upload in the ota partition
    int firmware_size = http.getSize();
    LOG_INFO("Firmware uploading (size: %d bytes). Please wait...", firmware_size);
    if (!Update.begin(firmware_size)) {
        LOG_ERROR("Firmware upload failed - no space");
        http.end();
        return false; // error
    }

    Shell::loop_all(); // flush log buffers so latest messages are shown in console

    // we're about to start the upload, set the status so the Web System Monitor spots it
    EMSESP::system_.systemStatus(SYSTEM_STATUS::SYSTEM_STATUS_UPLOADING);
    // TODO do we need to stop the UART first with EMSuart::stop() ?

    // set a callback so we can monitor progress in the WebUI
    Update.onProgress([](size_t progress, size_t total) { EMSESP::system_.systemStatus(SYSTEM_STATUS::SYSTEM_STATUS_UPLOADING + (progress * 100 / total)); });

    // get tcp stream and send it to Updater
    WiFiClient * stream = http.getStreamPtr();
    if (Update.writeStream(*stream) != firmware_size) {
        LOG_ERROR("Firmware upload failed - size differences");
        http.end();
        EMSESP::system_.systemStatus(SYSTEM_STATUS::SYSTEM_STATUS_ERROR_UPLOAD);
        return false; // error
    }

    if (!Update.end(true)) {
        LOG_ERROR("Firmware upload failed - general error");
        http.end();
        EMSESP::system_.systemStatus(SYSTEM_STATUS::SYSTEM_STATUS_ERROR_UPLOAD);
        return false; // error
    }

    // finished with upload
    http.end();
    saved_url.clear(); // prevent from downloading again
    LOG_INFO("Firmware uploaded successfully. Restarting...");
    EMSESP::system_.systemStatus(SYSTEM_STATUS::SYSTEM_STATUS_PENDING_RESTART);
#endif

    return true; // OK
}

// read command, e.g. read <deviceID> <type ID> [offset] [length] from console or API
// from Console use quotes so: call system read "<deviceID> <type ID> [offset] [length]"
bool System::readCommand(const char * data) {
    if (!data) {
        return false;
    }

    // extract <deviceID> <type ID> [offset] [length] from string
    char * p;
    char   value[11];

    // make a copy so we can iterate, max 15 chars (XX XXXX XX XX)
    char data_args[15];
    strlcpy(data_args, data, sizeof(data_args));

    uint8_t  device_id = 0; // is in hex
    uint16_t type_id   = 0; // is in hex
    uint8_t  length    = 0;
    uint8_t  offset    = 0;

    // first check deviceID
    if ((p = strtok(data_args, " ,"))) {               // delimiter comma or space
        strlcpy(value, p, 10);                         // get string
        device_id = (uint8_t)Helpers::hextoint(value); // convert hex to int
        if (!EMSESP::valid_device(device_id)) {
            LOG_ERROR("Invalid device ID (0x%02X) in read command", device_id);
            return false; // invalid device
        }
    }

    // iterate until end
    uint8_t num_args = 0;
    while (p != 0) {
        if ((p = strtok(nullptr, " ,"))) { // delimiter comma or space
            strlcpy(value, p, 10);         // get string
            if (num_args == 0) {
                type_id = (uint16_t)Helpers::hextoint(value); // convert hex to int
            } else if (num_args == 1) {
                offset = Helpers::atoint(value); // decimal
            } else if (num_args == 2) {
                length = Helpers::atoint(value); // decimal
            }
            num_args++;
        }
    }

    if (num_args == 0) {
        return false; // invalid number of arguments
    }

    EMSESP::send_read_request(type_id, device_id, offset, length, true);
    EMSESP::set_read_id(type_id);

    return true;
}

// system read command
bool System::command_read(const char * value, const int8_t id) {
    return readCommand(value);
}

// set the system status code - SYSTEM_STATUS in system.h
void System::systemStatus(uint8_t status_code) {
    systemStatus_ = status_code;
    // LOG_DEBUG("Setting System status code %d", status_code);
}

uint8_t System::systemStatus() {
    return systemStatus_;
}

} // namespace emsesp
