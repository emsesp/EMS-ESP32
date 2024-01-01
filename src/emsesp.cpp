/*
 * EMS-ESP - https://github.com/emsesp/EMS-ESP
 * Copyright 2020-2023  Paul Derbyshire
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

static_assert(uuid::thread_safe, "uuid-common must be thread-safe");
static_assert(uuid::log::thread_safe, "uuid-log must be thread-safe");
static_assert(uuid::console::thread_safe, "uuid-console must be thread-safe");

#ifndef EMSESP_STANDALONE
#include <WWWData.h>
#endif

namespace emsesp {

PsychicHttpServer webServer; // web server is static and global

#if defined(EMSESP_STANDALONE)
FS                      dummyFS;
ESP8266React            EMSESP::esp8266React(&webServer, &dummyFS);
WebSettingsService      EMSESP::webSettingsService      = WebSettingsService(&webServer, &dummyFS, EMSESP::esp8266React.getSecurityManager());
WebCustomizationService EMSESP::webCustomizationService = WebCustomizationService(&webServer, &dummyFS, EMSESP::esp8266React.getSecurityManager());
WebSchedulerService     EMSESP::webSchedulerService     = WebSchedulerService(&webServer, &dummyFS, EMSESP::esp8266React.getSecurityManager());
WebCustomEntityService  EMSESP::webCustomEntityService  = WebCustomEntityService(&webServer, &dummyFS, EMSESP::esp8266React.getSecurityManager());
#else
ESP8266React            EMSESP::esp8266React(&webServer, &LittleFS);
WebSettingsService      EMSESP::webSettingsService      = WebSettingsService(&webServer, &LittleFS, EMSESP::esp8266React.getSecurityManager());
WebCustomizationService EMSESP::webCustomizationService = WebCustomizationService(&webServer, &LittleFS, EMSESP::esp8266React.getSecurityManager());
WebSchedulerService     EMSESP::webSchedulerService     = WebSchedulerService(&webServer, &LittleFS, EMSESP::esp8266React.getSecurityManager());
WebCustomEntityService  EMSESP::webCustomEntityService  = WebCustomEntityService(&webServer, &LittleFS, EMSESP::esp8266React.getSecurityManager());
#endif

WebStatusService EMSESP::webStatusService = WebStatusService(&webServer, EMSESP::esp8266React.getSecurityManager());
WebDataService   EMSESP::webDataService   = WebDataService(&webServer, EMSESP::esp8266React.getSecurityManager());
WebAPIService    EMSESP::webAPIService    = WebAPIService(&webServer, EMSESP::esp8266React.getSecurityManager());
WebLogService    EMSESP::webLogService    = WebLogService(&webServer, EMSESP::esp8266React.getSecurityManager());

using DeviceFlags = EMSdevice;
using DeviceType  = EMSdevice::DeviceType;

std::vector<std::unique_ptr<EMSdevice>> EMSESP::emsdevices;      // array of all the detected EMS devices
std::vector<EMSESP::Device_record>      EMSESP::device_library_; // library of all our known EMS devices, in heap

uuid::log::Logger EMSESP::logger_{F_(emsesp), uuid::log::Facility::KERN};
uuid::log::Logger EMSESP::logger() {
    return logger_;
}

#ifndef EMSESP_STANDALONE
uuid::syslog::SyslogService System::syslog_;
#endif

// Core EMS-ESP services
RxService         EMSESP::rxservice_;         // incoming Telegram Rx handler
TxService         EMSESP::txservice_;         // outgoing Telegram Tx handler
Mqtt              EMSESP::mqtt_;              // mqtt handler
System            EMSESP::system_;            // core system services
TemperatureSensor EMSESP::temperaturesensor_; // Temperature sensors
AnalogSensor      EMSESP::analogsensor_;      // Analog sensors
Shower            EMSESP::shower_;            // Shower logic

// NV Storage
Preferences EMSESP::nvs_;

// static/common variables
uint16_t EMSESP::watch_id_         = WATCH_ID_NONE; // for when log is TRACE. 0 means no trace set
uint8_t  EMSESP::watch_            = 0;             // trace off
uint16_t EMSESP::read_id_          = WATCH_ID_NONE;
bool     EMSESP::read_next_        = false;
uint16_t EMSESP::publish_id_       = 0;
uint16_t EMSESP::response_id_      = 0;
bool     EMSESP::tap_water_active_ = false; // for when Boiler states we having running warm water. used in Shower()
uint32_t EMSESP::last_fetch_       = 0;
uint8_t  EMSESP::publish_all_idx_  = 0;
uint8_t  EMSESP::unique_id_count_  = 0;
bool     EMSESP::trace_raw_        = false;
uint16_t EMSESP::wait_validate_    = 0;
bool     EMSESP::wait_km_          = true;

// for a specific EMS device go and request data values
// or if device_id is 0 it will fetch from all our known and active devices
void EMSESP::fetch_device_values(const uint8_t device_id) {
    for (const auto & emsdevice : emsdevices) {
        if ((device_id == 0) || emsdevice->is_device_id(device_id)) {
            emsdevice->fetch_values();
            if (device_id != 0) {
                return; // quit, we only want to return the selected device
            }
        }
    }
}

// see if the deviceID exists
bool EMSESP::valid_device(const uint8_t device_id) {
    for (const auto & emsdevice : emsdevices) {
        if (emsdevice && emsdevice->is_device_id(device_id)) {
            return true;
        }
    }
    return false; // can't find it
}

// for a specific EMS device type go and request data values
void EMSESP::fetch_device_values_type(const uint8_t device_type) {
    for (const auto & emsdevice : emsdevices) {
        if (emsdevice && (emsdevice->device_type() == device_type)) {
            emsdevice->fetch_values();
        }
    }
}

bool EMSESP::cmd_is_readonly(const uint8_t device_type, const uint8_t device_id, const char * cmd, const int8_t id) {
    for (const auto & emsdevice : emsdevices) {
        if (emsdevice && (emsdevice->device_type() == device_type) && (!device_id || emsdevice->device_id() == device_id)) {
            return emsdevice->is_readonly(cmd, id);
        }
    }
    return false;
}

uint8_t EMSESP::device_id_from_cmd(const uint8_t device_type, const char * cmd, const int8_t id) {
    for (const auto & emsdevice : emsdevices) {
        if (emsdevice && emsdevice->device_type() == device_type && emsdevice->has_cmd(cmd, id)) {
            return emsdevice->device_id();
        }
    }
    return 0;
}

// clears list of recognized devices
void EMSESP::clear_all_devices() {
    // temporarily removed: clearing the list causes a crash, the associated commands and mqtt should also be removed.
    // emsdevices.clear(); // remove entries, but doesn't delete actual devices
}

// return number of devices of a known type
uint8_t EMSESP::count_devices(const uint8_t device_type) {
    uint8_t count = 0;
    for (const auto & emsdevice : emsdevices) {
        if (emsdevice) {
            count += (emsdevice->device_type() == device_type);
        }
    }
    return count;
}

// return total number of devices excluding the Controller
uint8_t EMSESP::count_devices() {
    uint8_t count = 0;
    for (const auto & emsdevice : emsdevices) {
        if (emsdevice) {
            count += (emsdevice->device_type() != EMSdevice::DeviceType::CONTROLLER);
        }
    }
    return count;
}

// returns the index of a device if there are more of the same type
// or 0 if there is only one or none
uint8_t EMSESP::device_index(const uint8_t device_type, const uint8_t unique_id) {
    if (count_devices(device_type) <= 1) {
        return 0; // none or only 1 device exists
    }
    uint8_t index = 1;
    for (const auto & emsdevice : emsdevices) {
        if (emsdevice->device_type() == device_type) {
            // did we find it?
            if (emsdevice->unique_id() == unique_id) {
                return index;
            }
            index++;
        }
    }
    return 0; // didn't find it
}

// scans for new devices
void EMSESP::scan_devices() {
    EMSESP::clear_all_devices();
    EMSESP::send_read_request(EMSdevice::EMS_TYPE_UBADevices, EMSdevice::EMS_DEVICE_ID_BOILER);
}

// to watch both type IDs and deviceIDs
void EMSESP::watch_id(uint16_t watch_id) {
    watch_id_ = watch_id;
}

// resets all counters and bumps the UART
// this is called when the tx_mode is persisted in the FS either via Web UI or the console
void EMSESP::uart_init() {
    uint8_t tx_mode = 0;
    uint8_t rx_gpio = 0;
    uint8_t tx_gpio = 0;
    EMSESP::webSettingsService.read([&](WebSettings & settings) {
        tx_mode = settings.tx_mode;
        rx_gpio = settings.rx_gpio;
        tx_gpio = settings.tx_gpio;
    });

    EMSuart::stop();

    // don't start UART if we have invalid GPIOs
    if (System::is_valid_gpio(rx_gpio) && System::is_valid_gpio(tx_gpio)) {
        EMSuart::start(tx_mode, rx_gpio, tx_gpio); // start UART
    } else {
        LOG_WARNING("Invalid UART Rx/Tx GPIOs. Check config.");
    }

    txservice_.start(); // sends out request to EMS bus for all devices
    txservice_.tx_mode(tx_mode);

    // force a fetch for all new values, unless Tx is set to off
    if (tx_mode != 0) {
        EMSESP::fetch_device_values();
    }
}

// return status of bus: connected (0), connected but Tx is broken (1), disconnected (2)
uint8_t EMSESP::bus_status() {
    if (!rxservice_.bus_connected()) {
        return BUS_STATUS_OFFLINE;
    }

    // check if we have Tx issues.
    uint32_t total_sent = txservice_.telegram_read_count() + txservice_.telegram_write_count();
    uint32_t total_fail = txservice_.telegram_read_fail_count() + txservice_.telegram_write_fail_count();

    // nothing sent and also no errors - must be ok
    if ((total_sent == 0) && (total_fail == 0)) {
        return BUS_STATUS_CONNECTED;
    }

    // nothing sent, but have Tx errors
    if ((total_sent == 0) && (total_fail != 0)) {
        return BUS_STATUS_TX_ERRORS;
    }

    // Tx Failure rate > 10%
    if ((total_fail < total_sent) && (((total_fail * 100) / total_sent) > EMSbus::EMS_TX_ERROR_LIMIT)) {
        return BUS_STATUS_TX_ERRORS;
    }

    return BUS_STATUS_CONNECTED;
}

// show the EMS bus status plus both Rx and Tx queues
void EMSESP::show_ems(uuid::console::Shell & shell) {
    // EMS bus information
    switch (bus_status()) {
    case BUS_STATUS_OFFLINE:
        shell.printfln("EMS Bus is disconnected.");
        break;
    case BUS_STATUS_TX_ERRORS:
        shell.printfln("EMS Bus is connected, but Tx is not stable.");
        break;
    default:
        shell.printfln("EMS Bus is connected.");
        break;
    }

    shell.println();

    if (bus_status() != BUS_STATUS_OFFLINE) {
        shell.printfln("EMS Bus info:");
        EMSESP::webSettingsService.read([&](WebSettings & settings) { shell.printfln("  Tx mode: %d", settings.tx_mode); });
        shell.printfln("  Bus protocol: %s", EMSbus::is_ht3() ? "HT3" : "Buderus");
        shell.printfln("  #recognized EMS devices: %d", EMSESP::emsdevices.size());
        shell.printfln("  #telegrams received: %d", rxservice_.telegram_count());
        shell.printfln("  #read requests sent: %d", txservice_.telegram_read_count());
        shell.printfln("  #write requests sent: %d", txservice_.telegram_write_count());
        shell.printfln("  #incomplete telegrams: %d", rxservice_.telegram_error_count());
        shell.printfln("  #read fails (after %d retries): %d", TxService::MAXIMUM_TX_RETRIES, txservice_.telegram_read_fail_count());
        shell.printfln("  #write fails (after %d retries): %d", TxService::MAXIMUM_TX_RETRIES, txservice_.telegram_write_fail_count());
        shell.printfln("  Rx line quality: %d%%", rxservice_.quality());
        shell.printfln("  Tx line quality: %d%%", (txservice_.read_quality() + txservice_.read_quality()) / 2);
        shell.println();
    }

    // Rx queue
    auto rx_telegrams = rxservice_.queue();
    if (rx_telegrams.empty()) {
        shell.printfln("Rx Queue is empty");
    } else {
        shell.printfln("Rx Queue (%ld telegram%s):", rx_telegrams.size(), rx_telegrams.size() == 1 ? "" : "s");
        for (const auto & it : rx_telegrams) {
            shell.printfln(" [%02d] %s", it.id_, pretty_telegram(it.telegram_).c_str());
        }
    }

    shell.println();

    // Tx queue
    auto tx_telegrams = txservice_.queue();
    if (tx_telegrams.empty()) {
        shell.printfln("Tx Queue is empty");
    } else {
        shell.printfln("Tx Queue (%ld telegram%s):", tx_telegrams.size(), tx_telegrams.size() == 1 ? "" : "s");

        std::string op;
        for (const auto & it : tx_telegrams) {
            if ((it.telegram_->operation) == Telegram::Operation::TX_RAW) {
                op = "RAW  ";
            } else if ((it.telegram_->operation) == Telegram::Operation::TX_READ) {
                op = "READ ";
            } else if ((it.telegram_->operation) == Telegram::Operation::TX_WRITE) {
                op = "WRITE";
            }
            shell.printfln(" [%02d%c] %s %s", it.id_, ((it.retry_) ? '*' : ' '), op.c_str(), pretty_telegram(it.telegram_).c_str());
        }
    }

    shell.println();
}

// Dump all entities to Serial out
// this is intended to run within the OS with lots of available memory!
#if defined(EMSESP_STANDALONE)
void EMSESP::dump_all_values(uuid::console::Shell & shell) {
    Serial.println("---- CSV START ----"); // marker use by py script
    // add header for CSV
    Serial.print(
        "device name,device type,product id,shortname,fullname,type [options...] \\| (min/max),uom,writeable,discovery entityid v3.4, discovery entityid");
    Serial.println();

    for (const auto & device_class : EMSFactory::device_handlers()) {
        // go through each device type so they are sorted
        for (const auto & device : device_library_) {
            if (device_class.first == device.device_type) {
                uint8_t device_id = 0;
                // Mixer class looks at device_id to determine type and the tag
                // so fixing to 0x28 which will give all the settings except flowSetTemp
                if (device.device_type == DeviceType::MIXER) {
                    if (device.flags == EMSdevice::EMS_DEVICE_FLAG_MMPLUS) {
                        if (device.product_id == 160) { // MM100
                            device_id = 0x28;           // wwc
                        } else {
                            device_id = 0x20; // hc
                        }
                    } else {
                        device_id = 0x20; // should cover all the other device types
                    }
                }

                // add the device and print out all the entities

                // if (device.product_id == 69) { // only for testing mixer
                emsdevices.push_back(
                    EMSFactory::add(device.device_type, device_id, device.product_id, "1.0", device.name, device.flags, EMSdevice::Brand::NO_BRAND));
                emsdevices.back()->dump_value_info();
                // } // only for testing mixer
            }
        }
    }

    Serial.println("---- CSV END ----"); // marker use by py script
}
#endif

// show EMS device values to the shell console
void EMSESP::show_device_values(uuid::console::Shell & shell) {
    if (emsdevices.empty()) {
        shell.printfln("No EMS devices detected.");
        shell.println();
        return;
    }

    // do this in the order of factory classes to keep a consistent order when displaying
    for (const auto & device_class : EMSFactory::device_handlers()) {
        for (const auto & emsdevice : emsdevices) {
            if (emsdevice && (emsdevice->device_type() == device_class.first)) {
                // print header, with device type translated
                shell.printfln("%s: %s (%d)", emsdevice->device_type_2_device_name_translated(), emsdevice->to_string().c_str(), emsdevice->count_entities());

                DynamicJsonDocument doc(EMSESP_JSON_SIZE_XXXLARGE); // use max size
                JsonObject          json = doc.to<JsonObject>();

                emsdevice->generate_values(json, DeviceValueTAG::TAG_NONE, true, EMSdevice::OUTPUT_TARGET::CONSOLE);

                // print line
                for (JsonPair p : json) {
                    std::string key = p.key().c_str(); // this will be fullname and the shortname in brackets

                    // extract the shortname from the key, which is in brackets
                    std::size_t first_bracket = key.find_last_of('(');
                    std::size_t last_bracket  = key.find_last_of(')');
                    std::string shortname     = key.substr(first_bracket + 1, last_bracket - first_bracket - 1);
                    std::string uom           = emsdevice->get_value_uom(key.substr(first_bracket + 1, last_bracket - first_bracket - 1));

                    shell.printfln("  %s: %s%s %s%s", key.c_str(), COLOR_BRIGHT_GREEN, p.value().as<std::string>().c_str(), uom.c_str(), COLOR_RESET);
                }
                shell.println();
            }
        }
    }

    // show any custom entities
    if (webCustomEntityService.count_entities() > 0) {
        shell.printfln("Custom entities:");
        StaticJsonDocument<EMSESP_JSON_SIZE_MEDIUM> custom_doc; // use max size
        JsonObject                                  custom_output = custom_doc.to<JsonObject>();
        webCustomEntityService.show_values(custom_output);
        for (JsonPair p : custom_output) {
            shell.printfln("  %s: %s%s%s", p.key().c_str(), COLOR_BRIGHT_GREEN, p.value().as<std::string>().c_str(), COLOR_RESET);
        }
        shell.println();
    }
}

// show temperature sensors and Analog sensors
void EMSESP::show_sensor_values(uuid::console::Shell & shell) {
    if (temperaturesensor_.have_sensors()) {
        shell.printfln("Temperature sensors:");
        char    s[10];
        char    s2[10];
        uint8_t fahrenheit = EMSESP::system_.fahrenheit() ? 2 : 0;

        for (const auto & sensor : temperaturesensor_.sensors()) {
            if (Helpers::hasValue(sensor.temperature_c)) {
                shell.printfln("  %s: %s%s °%c%s (offset %s, ID: %s)",
                               sensor.name().c_str(),
                               COLOR_BRIGHT_GREEN,
                               Helpers::render_value(s, sensor.temperature_c, 10, fahrenheit),
                               (fahrenheit == 0) ? 'C' : 'F',
                               COLOR_RESET,
                               Helpers::render_value(s2, sensor.offset(), 10, fahrenheit),
                               sensor.id().c_str());
            } else {
                shell.printfln("  %s (offset %s, ID: %s)", sensor.name().c_str(), Helpers::render_value(s, sensor.offset(), 10, fahrenheit), sensor.id().c_str());
            }
        }
        shell.println();
    }

    if (analogsensor_.have_sensors()) {
        char s[10];
        char s2[10];
        shell.printfln("Analog sensors:");
        for (const auto & sensor : analogsensor_.sensors()) {
            switch (sensor.type()) {
            case AnalogSensor::AnalogType::ADC:
                shell.printfln("  %s: %s%s %s%s (Type: ADC, Factor: %s, Offset: %d)",
                               sensor.name().c_str(),
                               COLOR_BRIGHT_GREEN,
                               Helpers::render_value(s, sensor.value(), 2),
                               EMSdevice::uom_to_string(sensor.uom()),
                               COLOR_RESET,
                               Helpers::render_value(s2, sensor.factor(), 4),
                               sensor.offset());
                break;
            default:
                // case AnalogSensor::AnalogType::DIGITAL_IN:
                // case AnalogSensor::AnalogType::COUNTER:
                shell.printfln("  %s: %s%d%s (Type: %s)",
                               sensor.name().c_str(),
                               COLOR_BRIGHT_GREEN,
                               (uint16_t)sensor.value(), // as int
                               COLOR_RESET,
                               sensor.type() == AnalogSensor::AnalogType::COUNTER ? "Counter" : "Digital in");
                break;
            }
        }
        shell.println();
    }
}

// MQTT publish everything, immediately
void EMSESP::publish_all(bool force) {
    if (force) {
        publish_all_idx_ = 1;
        reset_mqtt_ha();
        return;
    }

    if (Mqtt::connected()) {
        publish_device_values(EMSdevice::DeviceType::BOILER);
        publish_device_values(EMSdevice::DeviceType::THERMOSTAT);
        publish_device_values(EMSdevice::DeviceType::SOLAR);
        publish_device_values(EMSdevice::DeviceType::MIXER);
        publish_other_values();      // switch and heat pump, ...
        publish_sensor_values(true); // includes temperature and analog sensors
        system_.send_heartbeat();
    }
}

// loop and wait between devices for publishing all values
void EMSESP::publish_all_loop() {
    if (!Mqtt::connected() || !publish_all_idx_) {
        return;
    }

    // wait for free queue before sending next message, HA-messages are also queued
    if (Mqtt::publish_queued() > 0) {
        return;
    }

    switch (publish_all_idx_++) {
    case 1:
        publish_device_values(EMSdevice::DeviceType::BOILER);
        break;
    case 2:
        publish_device_values(EMSdevice::DeviceType::THERMOSTAT);
        break;
    case 3:
        publish_device_values(EMSdevice::DeviceType::SOLAR);
        break;
    case 4:
        publish_device_values(EMSdevice::DeviceType::MIXER);
        break;
    case 5:
        publish_other_values(); // switch and heat pump
        break;
    case 6:
        publish_sensor_values(true, true);
        break;
    case 7:
        if (Mqtt::ha_enabled()) {
            Mqtt::ha_status();
        }
        system_.send_heartbeat();
        break;
    default:
        // all finished
        publish_all_idx_ = 0;
    }
}

// force HA to re-create all the devices next time they are detected
// also removes the old HA topics
void EMSESP::reset_mqtt_ha() {
    if (!Mqtt::ha_enabled()) {
        return;
    }

    for (const auto & emsdevice : emsdevices) {
        emsdevice->ha_config_clear();
    }

    // force the re-creating of the temperature and analog sensor topics (for HA)
    temperaturesensor_.reload();
    analogsensor_.reload();
}

// create json doc for the devices values and add to MQTT publish queue
// this will also create the HA /config topic for each device value
// generate_values_json is called to build the device value (dv) object array
void EMSESP::publish_device_values(uint8_t device_type) {
    DynamicJsonDocument doc(EMSESP_JSON_SIZE_XXLARGE);
    JsonObject          json         = doc.to<JsonObject>();
    bool                need_publish = false;
    bool                nested       = (Mqtt::is_nested());

    // group by device type
    for (uint8_t tag = DeviceValueTAG::TAG_BOILER_DATA_WW; tag <= DeviceValueTAG::TAG_HS16; tag++) {
        JsonObject json_hc      = json;
        bool       nest_created = false;
        for (const auto & emsdevice : emsdevices) {
            if (emsdevice && (emsdevice->device_type() == device_type)) {
                if (nested && !nest_created && emsdevice->has_tags(tag)) {
                    json_hc      = doc.createNestedObject(EMSdevice::tag_to_mqtt(tag));
                    nest_created = true;
                }
                need_publish |= emsdevice->generate_values(json_hc, tag, false, EMSdevice::OUTPUT_TARGET::MQTT);
            }
        }
        if (need_publish && ((!nested && tag >= DeviceValueTAG::TAG_DEVICE_DATA_WW) || (tag == DeviceValueTAG::TAG_BOILER_DATA_WW))) {
            Mqtt::queue_publish(Mqtt::tag_to_topic(device_type, tag), json);
            json         = doc.to<JsonObject>();
            need_publish = false;
        }
    }

    if (need_publish) {
        if (doc.overflowed()) {
            LOG_WARNING("MQTT buffer overflow, please use individual topics");
        }
        Mqtt::queue_publish(Mqtt::tag_to_topic(device_type, DeviceValueTAG::TAG_NONE), json);
    }

    // we want to create the /config topic after the data payload to prevent HA from throwing up a warning
    if (Mqtt::ha_enabled()) {
        for (const auto & emsdevice : emsdevices) {
            if (emsdevice && (emsdevice->device_type() == device_type)) {
                emsdevice->mqtt_ha_entity_config_create();
            }
        }
    }
}

// call the devices that don't need special attention
void EMSESP::publish_other_values() {
    publish_device_values(EMSdevice::DeviceType::SWITCH);
    publish_device_values(EMSdevice::DeviceType::HEATPUMP);
    publish_device_values(EMSdevice::DeviceType::HEATSOURCE);
    publish_device_values(EMSdevice::DeviceType::VENTILATION);
    publish_device_values(EMSdevice::DeviceType::EXTENSION);
    publish_device_values(EMSdevice::DeviceType::ALERT);
    // other devices without values yet
    // publish_device_values(EMSdevice::DeviceType::GATEWAY);
    // publish_device_values(EMSdevice::DeviceType::CONNECT);
    // publish_device_values(EMSdevice::DeviceType::GENERIC);
    webSchedulerService.publish();
    webCustomEntityService.publish();
}

// publish both the temperature and analog sensor values
void EMSESP::publish_sensor_values(const bool time, const bool force) {
    if (sensor_enabled()) {
        if (temperaturesensor_.updated_values() || time || force) {
            temperaturesensor_.publish_values(force);
        }
    }

    if (analog_enabled()) {
        if (analogsensor_.updated_values() || time || force) {
            analogsensor_.publish_values(force);
        }
    }
}

// MQTT publish a telegram as raw data to the topic 'response'
void EMSESP::publish_response(std::shared_ptr<const Telegram> telegram) {
    static char *  buffer = nullptr;
    static uint8_t offset;
    if (buffer == nullptr) {
        offset    = telegram->offset; // store offset from first part
        buffer    = new char[768];    // max 256 hex-codes, 255 spaces, 1 termination
        buffer[0] = '\0';
    }
    strlcat(buffer, Helpers::data_to_hex(telegram->message_data, telegram->message_length).c_str(), 768);
    if (response_id_ != 0) {
        strlcat(buffer, " ", 768);
        return;
    }
    DynamicJsonDocument doc(EMSESP_JSON_SIZE_LARGE);
    char                s[10];
    doc["src"]    = Helpers::hextoa(s, telegram->src);
    doc["dest"]   = Helpers::hextoa(s, telegram->dest);
    doc["type"]   = Helpers::hextoa(s, telegram->type_id);
    doc["offset"] = Helpers::hextoa(s, offset);
    doc["data"]   = buffer;

    if (telegram->message_length <= 4 && strlen(buffer) <= 11) {
        uint32_t value = 0;
        for (uint8_t i = 0; i < telegram->message_length; i++) {
            value = (value << 8) + telegram->message_data[i];
        }
        doc["value"] = value;
    }
    Mqtt::queue_publish("response", doc.as<JsonObject>());
    delete[] buffer;
    buffer = nullptr;
}

// builds json with the detail of each value,
// for a specific EMS device type or the sensors, scheduler and custom entities
bool EMSESP::get_device_value_info(JsonObject & root, const char * cmd, const int8_t id, const uint8_t devicetype) {
    for (const auto & emsdevice : emsdevices) {
        if (emsdevice->device_type() == devicetype) {
            if (emsdevice->get_value_info(root, cmd, id)) {
                return true;
            }
        }
    }

    // specific for the temperaturesensor
    if (devicetype == DeviceType::TEMPERATURESENSOR) {
        return EMSESP::temperaturesensor_.get_value_info(root, cmd, id);
    }

    // analog sensor
    if (devicetype == DeviceType::ANALOGSENSOR) {
        return EMSESP::analogsensor_.get_value_info(root, cmd, id);
    }

    // scheduler
    if (devicetype == DeviceType::SCHEDULER) {
        return EMSESP::webSchedulerService.get_value_info(root, cmd);
    }

    // own entities
    if (devicetype == DeviceType::CUSTOM) {
        return EMSESP::webCustomEntityService.get_value_info(root, cmd);
    }

    char error[100];
    snprintf(error, sizeof(error), "cannot find values for entity '%s'", cmd);
    root["message"] = error;

    return false;
}

// search for recognized device_ids : Me, All, otherwise print hex value
std::string EMSESP::device_tostring(const uint8_t device_id) {
    if ((device_id & 0x7F) == rxservice_.ems_bus_id()) {
        return "Me";
    } else if (device_id == 0x00) {
        return "All";
    } else {
        char buffer[5];
        return Helpers::hextoa(buffer, device_id);
    }
}

// created a pretty print telegram as a text string
// e.g. Boiler(0x08) -> Me(0x0B), Version(0x02), data: 7B 06 01 00 00 00 00 00 00 04 (offset 1)
std::string EMSESP::pretty_telegram(std::shared_ptr<const Telegram> telegram) {
    uint8_t src    = telegram->src & 0x7F;
    uint8_t dest   = telegram->dest & 0x7F;
    uint8_t offset = telegram->offset;

    // find name for src and dest by looking up known devices
    std::string src_name("");
    std::string dest_name("");
    std::string type_name("");
    for (const auto & emsdevice : emsdevices) {
        if (emsdevice) {
            // get src & dest
            if (emsdevice->is_device_id(src)) {
                src_name = emsdevice->device_type_name();
            } else if (emsdevice->is_device_id(dest)) {
                dest_name = emsdevice->device_type_name();
            }
            // get the type name, any match will do
            if (type_name.empty()) {
                type_name = emsdevice->telegram_type_name(telegram);
            }
        }
    }

    // if we can't find names for the devices, use their hex values
    if (src_name.empty()) {
        src_name = device_tostring(src);
    }

    if (dest_name.empty()) {
        dest_name = device_tostring(dest);
    }

    // check for global/common types like Version & UBADevices
    if (telegram->type_id == EMSdevice::EMS_TYPE_VERSION) {
        type_name = "Version";
    } else if (telegram->type_id == EMSdevice::EMS_TYPE_UBADevices) {
        type_name = "UBADevices";
    }

    // if we don't know the type show
    if (type_name.empty()) {
        type_name = "?";
    }

    std::string str;
    str.reserve(200);
    if (telegram->operation == Telegram::Operation::RX_READ) {
        str = src_name + "(" + Helpers::hextoa(src) + ") -R-> " + dest_name + "(" + Helpers::hextoa(dest) + "), " + type_name + "("
              + Helpers::hextoa(telegram->type_id) + "), length: " + Helpers::hextoa(telegram->message_data[0]);
    } else if (telegram->dest == 0) {
        str = src_name + "(" + Helpers::hextoa(src) + ") -B-> " + dest_name + "(" + Helpers::hextoa(dest) + "), " + type_name + "("
              + Helpers::hextoa(telegram->type_id) + "), data: " + telegram->to_string_message();
    } else {
        str = src_name + "(" + Helpers::hextoa(src) + ") -W-> " + dest_name + "(" + Helpers::hextoa(dest) + "), " + type_name + "("
              + Helpers::hextoa(telegram->type_id) + "), data: " + telegram->to_string_message();
    }

    if (offset) {
        str += " (offset " + Helpers::itoa(offset) + ")";
    }

    return str;
}

/*
 * Type 0x07 - UBADevices - shows us the connected EMS devices
 * e.g. 08 00 07 00 0B 80 00 00 00 00 00 00 00 00 00 00 00
 * Junkers has 15 bytes of data
 * each byte is a bitmask for which devices are active
 * byte 1 = 0x08 - 0x0F, byte 2 = 0x10 - 0x17, etc...
 * e.g. in example above 1st byte = x0B = b1011 so we have deviceIDs 0x08, 0x09, 0x011
 * and 2nd byte = x80 = b1000 b0000 = deviceID 0x17
 */
void EMSESP::process_UBADevices(std::shared_ptr<const Telegram> telegram) {
    // exit it length is incorrect (must be 13 or 15 bytes long)
    if (telegram->message_length > 15) {
        return;
    }

    // for each byte, check the bits and determine the device_id
    for (uint8_t data_byte = 0; data_byte < telegram->message_length; data_byte++) {
        uint8_t next_byte = telegram->message_data[data_byte];

        if (next_byte) {
            for (uint8_t bit = 0; bit < 8; bit++) {
                if (next_byte & 0x01) {
                    uint8_t device_id = ((data_byte + 1) * 8) + bit;
                    // if we haven't already detected this device, request it's version details, unless its us (EMS-ESP)
                    // when the version info is received, it will automagically add the device
                    if ((device_id != EMSbus::ems_bus_id()) && !(EMSESP::device_exists(device_id))) {
                        LOG_DEBUG("New EMS device detected with ID 0x%02X. Requesting version information.", device_id);
                        send_read_request(EMSdevice::EMS_TYPE_VERSION, device_id);
                    }
                }
                next_byte = next_byte >> 1; // advance 1 bit
            }
        }
    }
}

// process the Version telegram (type 0x02), which is a common type
// e.g. 09 0B 02 00 PP V1 V2
void EMSESP::process_version(std::shared_ptr<const Telegram> telegram) {
    // check for valid telegram, just in case
    if (telegram->message_length < 3) {
        // for empty telegram add device with empty product, version and brand
        if (!telegram->message_length) {
            (void)add_device(telegram->src, 0, "00.00", 0);
        }
        return;
    }

    // check for 2nd subscriber, e.g. 18 0B 02 00 00 00 00 5E 02 01
    uint8_t offset = 0;
    if (telegram->message_data[0] == 0x00) {
        // see if we have a 2nd subscriber
        if (telegram->message_data[3] != 0x00) {
            offset = 3;
        } else {
            return; // ignore whole telegram
        }
    }

    // extra details from the telegram
    uint8_t device_id  = telegram->src;                  // deviceID
    uint8_t product_id = telegram->message_data[offset]; // productID

    // get version as XX.XX
    char version[8];
    snprintf(version, sizeof(version), "%02d.%02d", telegram->message_data[offset + 1], telegram->message_data[offset + 2]);

    // some devices store the protocol type (HT3, Buderus) in the last byte
    uint8_t brand;
    if (telegram->message_length >= 10) {
        brand = EMSdevice::decode_brand(telegram->message_data[9]);
    } else {
        brand = EMSdevice::Brand::NO_BRAND; // unknown
    }

    // add it - will be overwritten if device already exists
    (void)add_device(device_id, product_id, version, brand);
}

// find the device object that matches the deviceID and see if it has a matching telegram type handler
// but only process if the telegram is sent to us or it's a broadcast (dest=0x00=all)
// We also check for common telegram types, like the Version(0x02)
// returns false if there are none found
bool EMSESP::process_telegram(std::shared_ptr<const Telegram> telegram) {
    // if watching or reading...
    if ((telegram->type_id == read_id_ || telegram->type_id == response_id_) && (telegram->dest == txservice_.ems_bus_id())) {
        if (telegram->type_id == response_id_) {
            if (!trace_raw_) {
                LOG_TRACE("%s", pretty_telegram(telegram).c_str());
            }
            if (!read_next_) {
                response_id_ = 0;
            }
            publish_response(telegram);
        } else {
            LOG_NOTICE("%s", pretty_telegram(telegram).c_str());
        }
        // check if read is finished or gives more parts
        if (!read_next_) {
            read_id_ = WATCH_ID_NONE;
        }
        read_next_ = false;
    } else if (watch() == WATCH_ON) {
        if ((watch_id_ == WATCH_ID_NONE) || (telegram->type_id == watch_id_)
            || ((watch_id_ < 0x80) && ((telegram->src == watch_id_) || (telegram->dest == watch_id_)))) {
            LOG_NOTICE("%s", pretty_telegram(telegram).c_str());
        } else if (!trace_raw_) {
            LOG_TRACE("%s", pretty_telegram(telegram).c_str());
        }
    } else if (!trace_raw_) {
        LOG_TRACE("%s", pretty_telegram(telegram).c_str());
    }

    // only process broadcast telegrams or ones sent to us on request
    // if ((telegram->dest != 0x00) && (telegram->dest != rxservice_.ems_bus_id())) {
    if (telegram->operation == Telegram::Operation::RX_READ) {
        // LOG_DEBUG("read telegram received, not processing");
        return false;
    }

    // Check for custom entities reding this telegram
    webCustomEntityService.get_value(telegram);

    // check for common types, like the Version(0x02)
    if (telegram->type_id == EMSdevice::EMS_TYPE_VERSION) {
        process_version(telegram);
        return true;
    } else if (telegram->type_id == EMSdevice::EMS_TYPE_UBADevices) {
        // do not flood tx-queue with version requests while waiting for km200
        if (!wait_km_) {
            process_UBADevices(telegram);
        }
        return true;
    }

    // match device_id and type_id
    // calls the associated process function for that EMS device
    // returns false if the device_id doesn't recognize it
    // after the telegram has been processed, see if there have been values changed and we need to do a MQTT publish
    bool found       = false;
    bool knowndevice = false;
    for (const auto & emsdevice : emsdevices) {
        if (emsdevice->is_device_id(telegram->src)) {
            knowndevice = true;
            found       = emsdevice->handle_telegram(telegram);
            // if we correctly processed the telegram then follow up with sending it via MQTT (if enabled)
            if (found && Mqtt::connected()) {
                if ((mqtt_.get_publish_onchange(emsdevice->device_type()) && emsdevice->has_update())
                    || (telegram->type_id == publish_id_ && telegram->dest == txservice_.ems_bus_id())) {
                    if (telegram->type_id == publish_id_) {
                        publish_id_ = 0;
                    }
                    emsdevice->has_update(false); // reset flag
                    if (!Mqtt::publish_single()) {
                        publish_device_values(emsdevice->device_type()); // publish to MQTT if we explicitly have too
                    }
                }
            }
            if (wait_validate_ == telegram->type_id) {
                wait_validate_ = 0;
            }
            if (!found && telegram->message_length > 0) {
                emsdevice->add_handlers_ignored(telegram->type_id);
            }
            break;
        } else if (emsdevice->is_device_id(telegram->dest)) {
            emsdevice->handle_telegram(telegram);
        }
    }

    // handle unknown broadcasted telegrams
    if (!found && telegram->dest == 0) {
        LOG_DEBUG("No telegram type handler found for ID 0x%02X (src 0x%02X)", telegram->type_id, telegram->src);
        if (watch() == WATCH_UNKNOWN) {
            LOG_NOTICE("%s", pretty_telegram(telegram).c_str());
        }
        if (!wait_km_ && !knowndevice && (telegram->src != EMSbus::ems_bus_id()) && (telegram->message_length > 0)) {
            send_read_request(EMSdevice::EMS_TYPE_VERSION, telegram->src);
        }
    }

    return found;
}

// return true if we have this device already registered
bool EMSESP::device_exists(const uint8_t device_id) {
    for (const auto & emsdevice : emsdevices) {
        if (emsdevice && emsdevice->is_device_id(device_id)) {
            return true;
        }
    }

    return false; // not found
}

// for each associated EMS device go and get its system information
void EMSESP::show_devices(uuid::console::Shell & shell) {
    if (emsdevices.empty()) {
        shell.printfln("No EMS devices detected");
        shell.println();
        return;
    }

    shell.printfln("These EMS devices are currently active:");
    shell.println();

    // count the number of thermostats
    /*
    uint8_t num_thermostats = 0;
    for (const auto & emsdevice : emsdevices) {
        if (emsdevice && (emsdevice->device_type() == DeviceType::THERMOSTAT)) {
            num_thermostats++;
        }
    }
    */

    // for all device objects from emsdevice.h
    // so we keep a consistent order
    // don't translate the device type name
    for (const auto & device_class : EMSFactory::device_handlers()) {
        for (const auto & emsdevice : emsdevices) {
            if (emsdevice && (emsdevice->device_type() == device_class.first)) {
                shell.printf("%s: %s", emsdevice->device_type_name(), emsdevice->to_string().c_str());
                shell.println();
                emsdevice->show_telegram_handlers(shell);

#if defined(EMSESP_DEBUG)
                emsdevice->show_mqtt_handlers(shell);
#endif
                shell.println();
            }
        }
    }
}

// add a new or update existing EMS device to our list of active EMS devices
// if its not in our database, we don't add it
bool EMSESP::add_device(const uint8_t device_id, const uint8_t product_id, const char * version, const uint8_t brand) {
    // don't add ourselves!
    if (device_id == rxservice_.ems_bus_id()) {
        return false;
    }

    // first check to see if we already have it, if so update the record
    for (const auto & emsdevice : emsdevices) {
        if (emsdevice && emsdevice->is_device_id(device_id)) {
            if (product_id == 0) { // update only with valid product_id
                return true;
            }
            LOG_DEBUG("Updating details for already active deviceID 0x%02X", device_id);
            emsdevice->product_id(product_id);
            emsdevice->version(version);
            // only set brand if it doesn't already exist
            if (emsdevice->brand() == EMSdevice::Brand::NO_BRAND) {
                emsdevice->brand(brand);
            }
            // find the name and flags in our database
            for (const auto & device : device_library_) {
                if (device.product_id == product_id && device.device_type == emsdevice->device_type()) {
                    emsdevice->name(device.name);
                    emsdevice->add_flags(device.flags);
                }
            }

            return true; // finish up
        }
    }

    // look up the rest of the details using the product_id and create the new device object
    Device_record * device_p = nullptr;
    for (auto & device : device_library_) {
        if (device.product_id == product_id) {
            // sometimes boilers share the same productID as controllers
            // so only add boilers if the device_id is 0x08
            // cascaded boilers with 0x70.., map to heatsources
            if (device.device_type == DeviceType::BOILER) {
                if (device_id == EMSdevice::EMS_DEVICE_ID_BOILER) {
                    device_p = &device;
                    break;
                }
                if ((device_id >= EMSdevice::EMS_DEVICE_ID_HS1 && device_id <= EMSdevice::EMS_DEVICE_ID_HS16)) {
                    device_p              = &device;
                    device_p->device_type = DeviceType::HEATSOURCE;
                    break;
                }
            } else {
                // it's not a boiler, but we have a match
                device_p = &device;
                break;
            }
        }
    }

    // if we don't recognize the productID report it and add as a generic device
    if (device_p == nullptr) {
        LOG_NOTICE("Unrecognized EMS device (deviceID 0x%02X, productID %d). Please report on GitHub.", device_id, product_id);
        emsdevices.push_back(
            EMSFactory::add(DeviceType::GENERIC, device_id, product_id, version, "unknown", DeviceFlags::EMS_DEVICE_FLAG_NONE, EMSdevice::Brand::NO_BRAND));
        return false; // not found
    }

    auto name        = device_p->name;
    auto device_type = device_p->device_type;
    auto flags       = device_p->flags;

    // check for integrated modules with same product id
    if (device_type == DeviceType::HEATPUMP) {
        if (device_id == EMSdevice::EMS_DEVICE_ID_MODEM) {
            device_type = DeviceType::GATEWAY;
            name        = "WiFi module";
        } else if (device_id == EMSdevice::EMS_DEVICE_ID_RFBASE) {
            device_type = DeviceType::CONNECT;
            name        = "Wireless sensor base";
        }
    }

    // empty reply to version, read a generic device from database
    if (product_id == 0) {
        // check for known device IDs
        if (device_id == EMSdevice::EMS_DEVICE_ID_RFSENSOR) {
            // see: https://github.com/emsesp/EMS-ESP32/issues/103#issuecomment-911717342 and https://github.com/emsesp/EMS-ESP32/issues/624
            name        = "RF room temperature sensor";
            device_type = DeviceType::THERMOSTAT;
        } else if (device_id == EMSdevice::EMS_DEVICE_ID_ROOMTHERMOSTAT || device_id == EMSdevice::EMS_DEVICE_ID_TADO_OLD) {
            // see https://github.com/emsesp/EMS-ESP32/issues/174
            name        = "Generic thermostat";
            device_type = DeviceType::THERMOSTAT;
            flags       = DeviceFlags::EMS_DEVICE_FLAG_RC10 | DeviceFlags::EMS_DEVICE_FLAG_NO_WRITE;
        } else if (device_id == EMSdevice::EMS_DEVICE_ID_RS232) {
            name        = "RS232";
            device_type = DeviceType::CONNECT;
        } else if (device_id == EMSdevice::EMS_DEVICE_ID_TERMINAL) {
            name        = "Terminal";
            device_type = DeviceType::CONNECT;
        } else if (device_id == EMSdevice::EMS_DEVICE_ID_SERVICEKEY) {
            name        = "Service Key";
            device_type = DeviceType::CONNECT;
        } else if (device_id == EMSdevice::EMS_DEVICE_ID_CASCADE) {
            name        = "Cascade";
            device_type = DeviceType::CONNECT;
        } else if (device_id == EMSdevice::EMS_DEVICE_ID_EASYCOM
                   || (device_id >= EMSdevice::EMS_DEVICE_ID_MODEM && device_id <= EMSdevice::EMS_DEVICE_ID_MODEM + 5)) {
            // see https://github.com/emsesp/EMS-ESP/issues/460#issuecomment-709553012
            name        = "Modem";
            device_type = DeviceType::CONNECT;
        } else if (device_id == EMSdevice::EMS_DEVICE_ID_CONVERTER) {
            name = "Converter"; // generic
        } else if (device_id == EMSdevice::EMS_DEVICE_ID_CLOCK) {
            name        = "Clock"; // generic
            device_type = DeviceType::CONTROLLER;
        } else if (device_id == EMSdevice::EMS_DEVICE_ID_CONTROLLER) {
            name        = "Generic Controller";
            device_type = DeviceType::CONTROLLER;
        } else if (device_id == EMSdevice::EMS_DEVICE_ID_BOILER) {
            name        = "Generic Boiler";
            device_type = DeviceType::BOILER;
            flags       = DeviceFlags::EMS_DEVICE_FLAG_HEATPUMP;
            LOG_WARNING("Unknown EMS boiler. Using generic profile. Please report on GitHub.");
        } else if (device_id >= 0x68 && device_id <= 0x6F) {
            // test for https://github.com/emsesp/EMS-ESP32/issues/882
            name        = "Cascaded Controller";
            device_type = DeviceType::CONTROLLER;
        } else {
            LOG_WARNING("Unrecognized EMS device (device ID 0x%02X, no product ID). Please report on GitHub.", device_id);
            return false;
        }
    }

    LOG_DEBUG("Adding new device %s (deviceID 0x%02X, productID %d, version %s)", name, device_id, product_id, version);
    emsdevices.push_back(EMSFactory::add(device_type, device_id, product_id, version, name, flags, brand));

    // assign a unique ID. Note that this is not actual unique after a restart as it's dependent on the order that devices are found
    // can't be 0 otherwise web won't work
    emsdevices.back()->unique_id(++unique_id_count_);

    // sort devices based on type
    std::sort(emsdevices.begin(), emsdevices.end(), [](const std::unique_ptr<EMSdevice> & a, const std::unique_ptr<EMSdevice> & b) {
        return a->device_type() < b->device_type();
    });

    fetch_device_values(device_id); // go and fetch its data

    // Print to LOG showing we've added a new device
    LOG_INFO("Recognized new %s with deviceID 0x%02X", EMSdevice::device_type_2_device_name(device_type), device_id);

    // add command commands for all devices, except for connect, controller and gateway
    if ((device_type == DeviceType::CONNECT) || (device_type == DeviceType::CONTROLLER) || (device_type == DeviceType::GATEWAY)) {
        return true;
    }

    Command::add(
        device_type,
        F_(info),
        [device_type](const char * value, const int8_t id, JsonObject & output) {
            return EMSdevice::export_values(device_type, output, id, EMSdevice::OUTPUT_TARGET::API_VERBOSE);
        },
        FL_(info_cmd));
    Command::add(
        device_type,
        F_(values),
        [device_type](const char * value, const int8_t id, JsonObject & output) {
            return EMSdevice::export_values(device_type,
                                            output,
                                            id,
                                            EMSdevice::OUTPUT_TARGET::API_SHORTNAMES); // HIDDEN command showing short names, used in e.g. /api/boiler
        },
        nullptr,
        CommandFlag::HIDDEN); // this command is hidden
    Command::add(
        device_type,
        F_(commands),
        [device_type](const char * value, const int8_t id, JsonObject & output) { return command_commands(device_type, output, id); },
        FL_(commands_cmd));
    Command::add(
        device_type,
        F_(entities),
        [device_type](const char * value, const int8_t id, JsonObject & output) { return command_entities(device_type, output, id); },
        FL_(entities_cmd));

    // MQTT subscribe to the device e.g. "ems-esp/boiler/#"
    auto topic = std::string(EMSdevice::device_type_2_device_name(device_type)) + "/#";
    Mqtt::subscribe(device_type, topic, nullptr);

    return true;
}

// list device entities
bool EMSESP::command_entities(uint8_t device_type, JsonObject & output, const int8_t id) {
    JsonObject node;

    for (const auto & emsdevice : emsdevices) {
        if (emsdevice && (emsdevice->device_type() == device_type)) {
            emsdevice->list_device_entries(output);
            return true;
        }
    }

    return false;
}

// list all available commands, return as json
bool EMSESP::command_commands(uint8_t device_type, JsonObject & output, const int8_t id) {
    return Command::list(device_type, output);
}

// send a read request, passing it into to the Tx Service, with optional offset and length
void EMSESP::send_read_request(const uint16_t type_id, const uint8_t dest, const uint8_t offset, const uint8_t length, const bool front) {
    txservice_.read_request(type_id, dest, offset, length, front);
}

// sends write request
void EMSESP::send_write_request(const uint16_t type_id,
                                const uint8_t  dest,
                                const uint8_t  offset,
                                uint8_t *      message_data,
                                const uint8_t  message_length,
                                const uint16_t validate_typeid) {
    txservice_.add(Telegram::Operation::TX_WRITE, dest, type_id, offset, message_data, message_length, validate_typeid, true);
}

void EMSESP::send_write_request(const uint16_t type_id, const uint8_t dest, const uint8_t offset, const uint8_t value) {
    send_write_request(type_id, dest, offset, value, 0);
}

// send Tx write with a single value
void EMSESP::send_write_request(const uint16_t type_id, const uint8_t dest, const uint8_t offset, const uint8_t value, const uint16_t validate_typeid) {
    uint8_t message_data[1];
    message_data[0] = value;
    EMSESP::send_write_request(type_id, dest, offset, message_data, 1, validate_typeid);
}

// this is main entry point when data is received on the Rx line, via emsuart library
// we check if its a complete telegram or just a single byte (which could be a poll or a return status)
// the CRC check is not done here, only when it's added to the Rx queue with add()
void EMSESP::incoming_telegram(uint8_t * data, const uint8_t length) {
#ifdef EMSESP_UART_DEBUG
    static uint32_t rx_time_ = 0;
#endif
    // check first for echo
    uint8_t first_value = data[0];
    if (((first_value & 0x7F) == txservice_.ems_bus_id()) && (length > 1)) {
        // if we ask ourself at roomcontrol for version e.g. 0B 98 02 00 20
        Roomctrl::check((data[1] ^ 0x80 ^ rxservice_.ems_mask()), data, length);
#ifdef EMSESP_UART_DEBUG
        // get_uptime is only updated once per loop, does not give the right time
        LOG_TRACE("[UART_DEBUG] Echo after %d ms: %s", ::millis() - rx_time_, Helpers::data_to_hex(data, length).c_str());
#endif
        // add to RxQueue for log/watch
        rxservice_.add(data, length);
        return; // it's an echo
    }

    // are we waiting for a response from a recent Tx Read or Write?
    uint8_t tx_state = EMSbus::tx_state();
    if (tx_state != Telegram::Operation::NONE) {
        bool tx_successful = false;
        EMSbus::tx_state(Telegram::Operation::NONE); // reset Tx wait state

        // if we're waiting on a Write operation, we want a single byte 1 or 4
        if ((tx_state == Telegram::Operation::TX_WRITE) && (length == 1)) {
            if (first_value == TxService::TX_WRITE_SUCCESS) {
                LOG_DEBUG("Last Tx write successful");
                txservice_.increment_telegram_write_count(); // last tx/write was confirmed ok
                txservice_.send_poll();                      // close the bus
                publish_id_ = txservice_.post_send_query();  // follow up with any post-read if set
                txservice_.reset_retry_count();
                tx_successful = true;
            } else if (first_value == TxService::TX_WRITE_FAIL) {
                LOG_ERROR("Last Tx write rejected by host");
                txservice_.send_poll(); // close the bus
            }
        } else if (tx_state == Telegram::Operation::TX_READ && length == 1) {
            EMSbus::tx_state(Telegram::Operation::TX_READ); // reset Tx wait state
            return;
        } else if (tx_state == Telegram::Operation::TX_READ) {
            // got a telegram with data in it. See if the src/dest matches that from the last one we sent and continue to process it
            uint8_t src  = data[0];
            uint8_t dest = data[1];
            if (txservice_.is_last_tx(src, dest)) {
                LOG_DEBUG("Last Tx read successful");
                txservice_.increment_telegram_read_count();
                txservice_.reset_retry_count();
                tx_successful = true;

                // if telegram is longer read next part with offset +25 for ems+ or +27 for ems1.0
                // not for response to raw send commands without read_id set
                if ((response_id_ == 0 || read_id_ > 0) && (length >= 31) && (txservice_.read_next_tx(data[3], length) == read_id_)) {
                    read_next_ = true;
                    txservice_.send();
                } else {
                    txservice_.send_poll(); // close the bus
                }
            }
        }

        // if Tx wasn't successful, retry or just give up
        if (!tx_successful) {
            txservice_.retry_tx(tx_state, data, length);
            return;
        }
    }

    // check for poll
    if (length == 1) {
        // if ht3 poll must be ems_bus_id else if Buderus poll must be (ems_bus_id | 0x80)
        uint8_t         poll_id      = (first_value ^ 0x80 ^ rxservice_.ems_mask());
        static uint32_t connect_time = 0;
        if (!rxservice_.bus_connected()) {
            wait_km_     = true;
            connect_time = uuid::get_uptime_sec();
        }
        if (poll_id == txservice_.ems_bus_id()) {
            EMSbus::last_bus_activity(uuid::get_uptime()); // set the flag indication the EMS bus is active
        }
        if (wait_km_) {
            if (poll_id != 0x48 && (uuid::get_uptime_sec() - connect_time) < EMS_WAIT_KM_TIMEOUT) {
                return;
            }
            wait_km_ = false; // KM200 is polled, from now on it is safe to send
        }

#ifdef EMSESP_UART_DEBUG
        char s[4];
        if (first_value & 0x80) {
            LOG_TRACE("[UART_DEBUG] next Poll %s after %d ms", Helpers::hextoa(s, first_value), ::millis() - rx_time_);
            // time measurement starts here, use millis because get_uptime is only updated once per loop
            rx_time_ = ::millis();
        } else {
            LOG_TRACE("[UART_DEBUG] Poll ack %s after %d ms", Helpers::hextoa(s, first_value), ::millis() - rx_time_);
        }
#endif
        // check for poll to us, if so send top message from Tx queue immediately and quit
        if (poll_id == txservice_.get_send_id()) {
            txservice_.send();
        }
        // send remote room temperature if active
        Roomctrl::send(poll_id);
        return;
    } else {
#ifdef EMSESP_UART_DEBUG
        LOG_TRACE("[UART_DEBUG] Reply after %d ms: %s", ::millis() - rx_time_, Helpers::data_to_hex(data, length).c_str());
#endif
        Roomctrl::check((data[1] ^ 0x80 ^ rxservice_.ems_mask()), data, length); // check if there is a message for the roomcontroller

        rxservice_.add(data, length); // add to RxQueue
    }
}

// fetch devices one by one
void EMSESP::scheduled_fetch_values() {
    static uint8_t no = 0;
    if (no || (uuid::get_uptime() - last_fetch_ > EMS_FETCH_FREQUENCY)) {
        if (!no) {
            last_fetch_ = uuid::get_uptime();
            no          = 1;
        }

        if (txservice_.tx_queue_empty()) {
            uint8_t i = 0;
            for (const auto & emsdevice : emsdevices) {
                if (++i >= no) {
                    emsdevice->fetch_values();
                    no++;
                    return;
                }
            }
            webCustomEntityService.fetch();
            no = 0;
        }
    }
}

EMSESP::EMSESP()
#ifndef EMSESP_STANDALONE
    : telnet_([this](Stream & stream, const IPAddress & addr, uint16_t port) -> std::shared_ptr<uuid::console::Shell> {
        return std::make_shared<EMSESPConsole>(*this, stream, addr, port);
    })
#endif
{
}

// add web server endpoint
void EMSESP::handler(const char * uri, const char * contentType, const uint8_t * content, size_t len) {
    PsychicHttpRequestCallback fn = [contentType, content, len](PsychicRequest * request) {
        PsychicResponse response(request);
        response.setCode(200);
        response.setContentType(contentType);
        response.addHeader("Content-Encoding", "gzip");
        // response.addHeader("Content-Encoding", "br"); // Brotli - only works over HTTPS
        response.setContent(content, len);
        return response.send();
    };

    PsychicWebHandler * handler = new PsychicWebHandler();
    handler->onRequest(fn);
    webServer.on(uri, HTTP_GET, handler);

    // Set default end-point for all non matching requests
    // this is easier than using webServer.onNotFound()
    if (strcmp(uri, "/index.html") == 0) {
        webServer.defaultEndpoint->setHandler(handler);
    }
};

// configure web server
// this can be only be done after the network has been initialized
void EMSESP::setupWeb() {
    // set maximum number of uri handlers (.on() calls). default was 8
    //   WWWData has 19 (in registerRoutes(handler)
    //   esp8266React services has 13
    //   custom projects has around 23
    webServer.config.max_uri_handlers = 80;

    // TODO remove experimental stuff
    // webServer.config.uri_match_fn     = NULL; // don't use wildcards
    // webServer.config.task_priority = uxTaskPriorityGet(nullptr); // seems to make it slightly slower

    // TODO add support for https
    webServer.listen(80); // start the web server

    DefaultHeaders::Instance().addHeader("Server", "EMS-ESP");

#ifndef EMSESP_STANDALONE
    WWWData::registerRoutes(handler); // add webServer.on() endpoints from the generated web code
#endif

    esp8266React.registerURI(); // load up the core system web endpoints

    // init EMS-ESP web endpoints
    webDataService.registerURI();          // /rest{deviceData,coreData,sensorData...}
    webStatusService.registerURI();        // /rest/status
    webSettingsService.registerURI();      // /rest/{settings,boardprofile}
    webAPIService.registerURI();           // /api/*, /rest/{getSettings,getSchedule,getCustomizations,getEntities}
    webLogService.registerURI();           // /rest/logSettings and EventSource
    webCustomizationService.registerURI(); // /rest{customization,customizationEntities,resetCustomizations,devices,deviceEntities}
    webSchedulerService.registerURI();     // /rest/schedule
    webCustomEntityService.registerURI();  // /rest/customentities

    // Add CORS if specified in the network settings
    esp8266React.getNetworkSettingsService()->read([&](NetworkSettings & networkSettings) {
        if (networkSettings.enableCORS) {
            DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", networkSettings.CORSOrigin);
            DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "Accept, Content-Type, Authorization");
            DefaultHeaders::Instance().addHeader("Access-Control-Allow-Credentials", "true");
        }
    });
}

// start all the core services
// the services must be loaded in the correct order
void EMSESP::start() {
    serial_console_.begin(SERIAL_CONSOLE_BAUD_RATE);

    shell_ = std::make_shared<EMSESPConsole>(*this, serial_console_, true);
    shell_->maximum_log_messages(100);
    shell_->start();
#if defined(EMSESP_DEBUG)
    shell_->log_level(uuid::log::Level::DEBUG);
#else
    shell_->log_level(uuid::log::Level::TRACE);
#endif

#if defined(EMSESP_STANDALONE)
    shell_->add_flags(CommandFlags::ADMIN); // always start in su/admin mode when running tests
#endif

// start the file system
#ifndef EMSESP_STANDALONE
    if (!LittleFS.begin(true)) {
        Serial.println("LittleFS Mount Failed. Using default settings.");
        return;
    }
#endif

// do a quick scan of the filesystem to see if we have a /config folder
// so we know if this is a new install or not
#ifndef EMSESP_STANDALONE
    File root             = LittleFS.open("/config"); // FS_CONFIG_DIRECTORY
    bool factory_settings = !root;
    if (!root) {
#if defined(EMSESP_DEBUG)
        Serial.println("No config found, assuming factory settings");
#endif
    }
    root.close();
#else
    bool factory_settings = false;
#endif

    esp8266React.begin(); // loads core system services settings (network, mqtt, ap, ntp etc)

    setupWeb(); // configure web server, now that the network has been initialized

    webLogService.begin(); // start web log service. now we can start capturing logs to the web log

    // start Prefences, the internal persistance storage
    nvs_.begin("ems-esp", false, "nvs");

    LOG_INFO("Starting EMS-ESP version %s", EMSESP_APP_VERSION); // welcome message
    LOG_DEBUG("System is running in Debug mode");
    LOG_INFO("Last system reset reason Core0: %s, Core1: %s", system_.reset_reason(0).c_str(), system_.reset_reason(1).c_str());

    // see if we're restoring a settings file
    if (system_.check_restore()) {
        LOG_WARNING("System needs a restart to apply new settings. Please wait.");
        system_.system_restart();
    };

    webSettingsService.begin(); // load EMS-ESP Application settings...

    // do any system upgrades
    if (system_.check_upgrade(factory_settings)) {
        LOG_WARNING("System needs a restart to apply new settings. Please wait.");
        system_.system_restart();
    };

    system_.reload_settings(); // ... and store some of the settings locally

    webCustomizationService.begin(); // load the customizations
    webSchedulerService.begin();     // load the scheduler events
    webCustomEntityService.begin();  // load the custom telegram reads

    // start telnet service if it's enabled
    // default idle is 10 minutes, default write timeout is 0 (automatic)
    // note, this must be started after the network/wifi for ESP32 otherwise it'll crash
    if (system_.telnet_enabled()) {
#ifndef EMSESP_STANDALONE
        telnet_.start();
        telnet_.initial_idle_timeout(3600);  // in sec, one hour idle timeout
        telnet_.default_write_timeout(1000); // in ms, socket timeout 1 second
#endif
    }

    mqtt_.start();              // mqtt init
    system_.start();            // starts commands, led, adc, button, network (sets hostname), syslog & uart
    shower_.start();            // initialize shower timer and shower alert
    temperaturesensor_.start(); // Temperature external sensors
    analogsensor_.start();      // Analog external sensors
    webLogService.start();      // apply settings to weblog service

    // Load our library of known devices into stack mem. Names are stored in Flash memory
    device_library_ = {
#include "device_library.h"
    };
    LOG_INFO("Loaded EMS device library (%d records)", device_library_.size());

#if defined(EMSESP_STANDALONE)
    Mqtt::on_connect(); // simulate an MQTT connection
#endif
}

// main loop calling all services


void EMSESP::loop() {
    esp8266React.loop(); // web services
    system_.loop();      // does LED and checks system health, and syslog service

    // if we're doing an OTA upload, skip everything except from console refresh
    if (!system_.upload_status()) {
        // service loops
        webLogService.loop();       // log in Web UI
        rxservice_.loop();          // process any incoming Rx telegrams
        shower_.loop();             // check for shower on/off
        temperaturesensor_.loop();  // read sensor temperatures
        analogsensor_.loop();       // read analog sensor values
        publish_all_loop();         // with HA messages in parts to avoid flooding the mqtt queue
        mqtt_.loop();               // sends out anything in the MQTT queue
        webSchedulerService.loop(); // handle any scheduled jobs

        // force a query on the EMS devices to fetch latest data at a set interval (1 min)
        scheduled_fetch_values();
    }

    uuid::loop();

#ifndef EMSESP_STANDALONE
    if (system_.telnet_enabled()) {
        telnet_.loop();
    }

#else
    if (!shell_->running()) {
        ::exit(0);
    }
#endif

    Shell::loop_all();
}

} // namespace emsesp
