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
#include "esp_ota_ops.h"
#endif

static_assert(uuid::thread_safe, "uuid-common must be thread-safe");
static_assert(uuid::log::thread_safe, "uuid-log must be thread-safe");
static_assert(uuid::console::thread_safe, "uuid-console must be thread-safe");

namespace emsesp {

// Static member definitions
std::deque<std::unique_ptr<EMSdevice>> EMSESP::emsdevices;
std::vector<EMSESP::Device_record>     EMSESP::device_library_;
uuid::log::Logger                      EMSESP::logger_{F_(emsesp), uuid::log::Facility::KERN};
uint16_t                               EMSESP::watch_id_         = WATCH_ID_NONE;
uint8_t                                EMSESP::watch_            = 0;
uint16_t                               EMSESP::read_id_          = WATCH_ID_NONE;
bool                                   EMSESP::read_next_        = false;
uint16_t                               EMSESP::publish_id_       = 0;
uint16_t                               EMSESP::response_id_      = 0;
bool                                   EMSESP::tap_water_active_ = false;
uint8_t                                EMSESP::publish_all_idx_  = 0;
uint8_t                                EMSESP::unique_id_count_  = 0;
bool                                   EMSESP::trace_raw_        = false;
uint16_t                               EMSESP::wait_validate_    = 0;
bool                                   EMSESP::wait_km_          = false;
uint32_t                               EMSESP::last_fetch_       = 0;

AsyncWebServer webServer(80);

#if defined(EMSESP_STANDALONE)
FS                      dummyFS;
ESP32React              EMSESP::esp32React(&webServer, &dummyFS);
WebSettingsService      EMSESP::webSettingsService      = WebSettingsService(&webServer, &dummyFS, EMSESP::esp32React.getSecurityManager());
WebCustomizationService EMSESP::webCustomizationService = WebCustomizationService(&webServer, &dummyFS, EMSESP::esp32React.getSecurityManager());
WebSchedulerService     EMSESP::webSchedulerService     = WebSchedulerService(&webServer, &dummyFS, EMSESP::esp32React.getSecurityManager());
WebCustomEntityService  EMSESP::webCustomEntityService  = WebCustomEntityService(&webServer, &dummyFS, EMSESP::esp32React.getSecurityManager());
WebModulesService       EMSESP::webModulesService       = WebModulesService(&webServer, &dummyFS, EMSESP::esp32React.getSecurityManager());
#else
ESP32React              EMSESP::esp32React(&webServer, &LittleFS);
WebSettingsService      EMSESP::webSettingsService      = WebSettingsService(&webServer, &LittleFS, EMSESP::esp32React.getSecurityManager());
WebCustomizationService EMSESP::webCustomizationService = WebCustomizationService(&webServer, &LittleFS, EMSESP::esp32React.getSecurityManager());
WebSchedulerService     EMSESP::webSchedulerService     = WebSchedulerService(&webServer, &LittleFS, EMSESP::esp32React.getSecurityManager());
WebCustomEntityService  EMSESP::webCustomEntityService  = WebCustomEntityService(&webServer, &LittleFS, EMSESP::esp32React.getSecurityManager());
WebModulesService       EMSESP::webModulesService       = WebModulesService(&webServer, &LittleFS, EMSESP::esp32React.getSecurityManager());
#endif

WebActivityService EMSESP::webActivityService = WebActivityService(&webServer, EMSESP::esp32React.getSecurityManager());
WebStatusService   EMSESP::webStatusService   = WebStatusService(&webServer, EMSESP::esp32React.getSecurityManager());
WebDataService     EMSESP::webDataService     = WebDataService(&webServer, EMSESP::esp32React.getSecurityManager());
WebAPIService      EMSESP::webAPIService      = WebAPIService(&webServer, EMSESP::esp32React.getSecurityManager());
WebLogService      EMSESP::webLogService      = WebLogService(&webServer, EMSESP::esp32React.getSecurityManager());

using DeviceFlags = EMSdevice;
using DeviceType  = EMSdevice::DeviceType;

uuid::log::Logger EMSESP::logger() {
    return logger_;
}

#ifndef EMSESP_STANDALONE
uuid::syslog::SyslogService System::syslog_;
#endif

// The services
RxService         EMSESP::rxservice_;         // incoming Telegram Rx handler
TxService         EMSESP::txservice_;         // outgoing Telegram Tx handler
Mqtt              EMSESP::mqtt_;              // mqtt handler
Modbus *          EMSESP::modbus_;            // modbus handler
System            EMSESP::system_;            // core system services
TemperatureSensor EMSESP::temperaturesensor_; // Temperature sensors
AnalogSensor      EMSESP::analogsensor_;      // Analog sensors
Shower            EMSESP::shower_;            // Shower logic
Preferences       EMSESP::nvs_;               // NV Storage

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
    EMSESP::webSettingsService.read([&](WebSettings const & settings) {
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
    // if (tx_mode != 0) {
    // EMSESP::fetch_device_values();
    // }
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
        EMSESP::webSettingsService.read([&](WebSettings const & settings) { shell.printfln("  Tx mode: %d", settings.tx_mode); });
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

// Dump all entities to Serial out - used to create the dump_entities.csv file
// this is intended to run within the OS with lots of available memory!
#if defined(EMSESP_STANDALONE)
void EMSESP::dump_all_entities(uuid::console::Shell & shell) {
    Serial.println();
    Serial.println("---- CSV START ----"); // marker use by py script
    // add header for CSV
    Serial.println("device name,device type,product id,shortname,fullname,type [options...] \\| (min/max),uom,writeable,discovery entityid v3.4,discovery "
                   "entityid,modbus unit identifier,modbus block,modbus scale factor,modbus offset,modbus count");

    for (const auto & device_class : EMSFactory::device_handlers()) {
        // go through each device type so they are sorted
        for (const auto & device : device_library_) {
            if (device_class.first == device.device_type) {
                uint8_t device_id = 0;
                // Water class looks at device_id to determine type and the tag
                if (device.device_type == DeviceType::WATER) {
                    if (device.flags == EMSdevice::EMS_DEVICE_FLAG_MMPLUS) {
                        device_id = 0x28; // dhw 1/2
                    } else if (device.flags == EMSdevice::EMS_DEVICE_FLAG_SM100) {
                        device_id = 0x28; // fix to dhw1, normally SM100 can only use dhw 3
                    } else if (device.flags == EMSdevice::EMS_DEVICE_FLAG_IPM) {
                        device_id = 0x40; // dhw 1, not needed
                    }
                }

                // For a Mixer, fix device_id to 0x20 to give us all the settings
                if (device.device_type == DeviceType::MIXER) {
                    device_id = 0x20; // hc
                }
                // For Heatsource set AM200
                if (device.device_type == DeviceType::HEATSOURCE) {
                    device_id = EMSdevice::EMS_DEVICE_ID_AHS1;
                }

                // add the device and print out all the entities
                // for testing the mixer use ... if (device.product_id == 69) {
                emsdevices.push_back(
                    EMSFactory::add(device.device_type, device_id, device.product_id, "1.0", device.default_name, device.flags, EMSdevice::Brand::NO_BRAND));
                emsdevices.back()->dump_devicevalue_info(); // print out the entities
            }
        }
    }

    Serial.println("---- CSV END ----"); // marker use by py script
}
#endif

// Dump all telegrams to Serial out
// this is intended to run within the OS with lots of available memory!
#if defined(EMSESP_STANDALONE)
void EMSESP::dump_all_telegrams(uuid::console::Shell & shell) {
    std::vector<EMSdevice::TelegramFunctionDump> telegram_functions_dump;

    Serial.println();
    Serial.println("---- CSV START ----"); // marker use by py script
    // add header for CSV
    Serial.println("telegram_type_id,name,is_fetched");

    for (const auto & device_class : EMSFactory::device_handlers()) {
        // go through each device type so they are sorted
        for (const auto & device : device_library_) {
            if (device_class.first == device.device_type) {
                uint8_t device_id = 0;
                // Water class looks at device_id to determine type and the tag
                if (device.device_type == DeviceType::WATER) {
                    if (device.flags == EMSdevice::EMS_DEVICE_FLAG_MMPLUS) {
                        device_id = 0x28; // dhw 1/2
                    } else if (device.flags == EMSdevice::EMS_DEVICE_FLAG_SM100) {
                        device_id = 0x2A; // dhw 3 needed to calculate right telegram numbers
                    } else if (device.flags == EMSdevice::EMS_DEVICE_FLAG_IPM) {
                        device_id = 0x40; // dhw 1, not needed
                    }
                }

                // For a Mixer, fix device_id to 0x20 to give us all the settings
                if (device.device_type == DeviceType::MIXER) {
                    device_id = 0x20; // hc
                }

                // add the device and print out all the entities
                emsdevices.push_back(
                    EMSFactory::add(device.device_type, device_id, device.product_id, "1.0", device.default_name, device.flags, EMSdevice::Brand::NO_BRAND));
                // add to our vector list
                emsdevices.back()->dump_telegram_info(telegram_functions_dump);
            }
        }
    }

    auto num_entries = telegram_functions_dump.size();

    // sort based on typeID
    std::sort(telegram_functions_dump.begin(),
              telegram_functions_dump.end(),
              [](const EMSdevice::TelegramFunctionDump & a, const EMSdevice::TelegramFunctionDump & b) { return a.type_id_ < b.type_id_; });

    // Get the iterator for the modified vector
    auto it = std::unique(telegram_functions_dump.begin(),
                          telegram_functions_dump.end(),
                          [](const EMSdevice::TelegramFunctionDump & a, const EMSdevice::TelegramFunctionDump & b) { return a.type_id_ == b.type_id_; });

    // Use erase method to remove all the duplicates from the vector
    telegram_functions_dump.erase(it, telegram_functions_dump.end());

    for (const auto & tf : telegram_functions_dump) {
        Serial.printf(Helpers::hextoa(tf.type_id_, true).c_str());
        Serial.print(',');
        Serial.print(tf.name_);
        Serial.print(',');
        Serial.print(tf.fetch_ ? "fetched" : " ");
        Serial.println();
    }

    Serial.println("---- CSV END ----"); // marker used by py script

    Serial.printf("Total telegrams = %d, total unique telegrams = %d", num_entries, telegram_functions_dump.size());
    Serial.println();
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

                JsonDocument doc;
                JsonObject   json = doc.to<JsonObject>();

                emsdevice->generate_values(json, DeviceValueTAG::TAG_NONE, true, EMSdevice::OUTPUT_TARGET::CONSOLE);

                // print line
                for (JsonPair p : json) {
                    std::string key = p.key().c_str(); // this will be fullname and the shortname in brackets

                    // extract the shortname from the key, which is in brackets
                    std::size_t first_bracket = key.find_last_of('(');
                    std::size_t last_bracket  = key.find_last_of(')');
                    std::string uom           = emsdevice->get_value_uom(key.substr(first_bracket + 1, last_bracket - first_bracket - 1));

                    shell.printfln("  %s: %s%s %s%s", key.c_str(), COLOR_BRIGHT_GREEN, p.value().as<std::string>().c_str(), uom.c_str(), COLOR_RESET);
                }
                shell.println();
            }
        }
    }

    // show any custom entities
    if (webCustomEntityService.count_entities() > 0) {
        shell.printfln("Custom Entities:");
        JsonDocument custom_doc; // use max size
        JsonObject   custom_output = custom_doc.to<JsonObject>();
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
                               sensor.type() == AnalogSensor::AnalogType::COUNTER ? "Counter" : "Digital In");
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
        publish_device_values(EMSdevice::DeviceType::WATER);
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
        publish_device_values(EMSdevice::DeviceType::WATER);
        break;
    case 6:
        publish_other_values(); // switch and heat pump
        break;
    case 7:
        publish_sensor_values(true, true);
        break;
    case 8:
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
    JsonDocument doc;
    JsonObject   json         = doc.to<JsonObject>();
    bool         need_publish = false;
    bool         nested       = (Mqtt::is_nested());

    // group by device type
    for (int8_t tag = DeviceValueTAG::TAG_DEVICE_DATA; tag <= DeviceValueTAG::TAG_HS16; tag++) {
        JsonObject json_tag     = json;
        bool       nest_created = false;
        for (const auto & emsdevice : emsdevices) {
            if (emsdevice && (emsdevice->device_type() == device_type)) {
                if (nested && !nest_created && emsdevice->has_tags(tag)) {
                    json_tag     = doc[EMSdevice::tag_to_mqtt(tag)].to<JsonObject>();
                    nest_created = true;
                }
                need_publish |= emsdevice->generate_values(json_tag, tag, false, EMSdevice::OUTPUT_TARGET::MQTT);
            }
        }
        if (need_publish && !nested) {
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
    publish_device_values(EMSdevice::DeviceType::POOL);
    // other EMS devices without values yet
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
    static char *   buffer = nullptr;
    static uint8_t  offset = 0;
    static uint16_t type   = 0;
    // restart on mismatch while collecting telegram
    if (buffer && (telegram->offset < offset || telegram->type_id != type)) {
        delete[] buffer;
        buffer = nullptr;
    }
    if (buffer == nullptr) {
        offset = telegram->offset; // store offset from first part
        type   = telegram->type_id;
        buffer = new char[768]; // max 256 hex-codes, 255 spaces, 1 termination
        for (uint16_t i = 0; i < 256; i++) {
            buffer[i * 3]     = '0';
            buffer[i * 3 + 1] = '0';
            buffer[i * 3 + 2] = ' ';
        }
        buffer[267] = '\0';
    }
    if (telegram->message_length) {
        strlcpy(&buffer[(telegram->offset - offset) * 3], Helpers::data_to_hex(telegram->message_data, telegram->message_length).c_str(), 768);
    } else {
        strlcpy(&buffer[(telegram->offset - offset) * 3], "", 768);
    }
    if (response_id_ != 0) {
        buffer[strlen(buffer)] = ' '; // overwrite termination \0
        return;                       // do not delete buffer
    }
    JsonDocument doc;
    char         s[10];
    doc["src"]    = Helpers::hextoa(s, telegram->src);
    doc["dest"]   = Helpers::hextoa(s, telegram->dest);
    doc["type"]   = Helpers::hextoa(s, telegram->type_id);
    doc["offset"] = Helpers::hextoa(s, offset);
    doc["data"]   = buffer;

    if (strlen(buffer) && strlen(buffer) <= 11) {
        uint32_t value = Helpers::hextoint(buffer);
        doc["value"]   = value;
    }
    Mqtt::queue_publish("response", doc.as<JsonObject>());
    delete[] buffer;
    buffer = nullptr;
}

// builds json with the detail of each value, for a given device type
// device type can be EMS devices looking for entities, or a sensor/scheduler/custom entity etc looking for values extracted from the info command
bool EMSESP::get_device_value_info(JsonObject root, const char * cmd, const int8_t id, const uint8_t devicetype) {
    // check first for EMS devices
    bool found_device = false;

    for (const auto & emsdevice : emsdevices) {
        if (emsdevice->device_type() == devicetype) {
            found_device = true;
            // we may have multiple devices of the same type, so we need to check the id (e.g. id could be a hc number)
            if (emsdevice->get_value_info(root, cmd, id)) {
                // if we have no values, keep going traversing the devices, it may be a thermostat with multiple hc's
                if (root.size()) {
                    return true;
                }
            }
        }
    }

    // if the EMS device was valid, but the cmd not found exit. it will be handled upstream.
    if (found_device) {
        return false;
    }

    // check for other non EMS devices...

    // temperature sensor
    if (devicetype == DeviceType::TEMPERATURESENSOR) {
        return temperaturesensor_.get_value_info(root, cmd, id);
    }

    // analog sensor
    if (devicetype == DeviceType::ANALOGSENSOR) {
        return analogsensor_.get_value_info(root, cmd, id);
    }

    // scheduler
    if (devicetype == DeviceType::SCHEDULER) {
        return webSchedulerService.get_value_info(root, cmd);
    }

    // custom entities
    if (devicetype == DeviceType::CUSTOM) {
        return webCustomEntityService.get_value_info(root, cmd);
    }

    // system
    if (devicetype == DeviceType::SYSTEM) {
        return system_.get_value_info(root, cmd);
    }

    return false; // not found
}

// search for recognized device_ids : Me, All, otherwise print hex value
std::string EMSESP::device_tostring(const uint8_t device_id) {
    if ((device_id & 0x7F) == EMSbus::ems_bus_id()) {
        return "me";
    } else if (device_id == 0x00) {
        return "all";
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
        // get src & dest
        if (emsdevice->is_device_id(src)) {
            src_name = emsdevice->device_type_name();
        } else if (emsdevice->is_device_id(dest)) {
            dest_name = emsdevice->device_type_name();
        }
        // get the type name
        if (type_name.empty()) {
            if ((telegram->operation == Telegram::Operation::RX_READ && emsdevice->is_device_id(dest))
                || (telegram->operation != Telegram::Operation::RX_READ && dest == 0 && emsdevice->is_device_id(src))
                || (telegram->operation != Telegram::Operation::RX_READ && src == EMSbus::ems_bus_id() && emsdevice->is_device_id(dest))) {
                type_name = emsdevice->telegram_type_name(telegram);
            }
        }
    }
    if (type_name.empty()) {
        // fallback, get the type name from src
        for (const auto & emsdevice : emsdevices) {
            if (telegram->operation != Telegram::Operation::RX_READ && emsdevice->is_device_id(src)) {
                type_name = emsdevice->telegram_type_name(telegram);
                break;
            }
        }
    }

    if (type_name.empty()) {
        // 2nd fallback, get the type name from dest
        for (const auto & emsdevice : emsdevices) {
            if (telegram->operation != Telegram::Operation::RX_READ && emsdevice->is_device_id(dest)) {
                type_name = emsdevice->telegram_type_name(telegram);
                break;
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

    // if we don't know the type show
    if (type_name.empty()) {
        // check for global/common types like Version & UBADevices
        switch (telegram->type_id) {
        case EMSdevice::EMS_TYPE_NAME:
            type_name = "DeviceName";
            break;
        case EMSdevice::EMS_TYPE_VERSION:
            type_name = "Version";
            break;
        case EMSdevice::EMS_TYPE_UBADevices:
            type_name = "UBADevices";
            break;
        case EMSdevice::EMS_TYPE_DEVICEERROR:
            type_name = "DeviceError";
            break;
        case EMSdevice::EMS_TYPE_SYSTEMERROR:
            type_name = "SystemError";
            break;
        case EMSdevice::EMS_TYPE_MENUCONFIG:
            type_name = "MenuConfig";
            break;
        case EMSdevice::EMS_TYPE_VALUECONFIG:
            type_name = "ValueConfig";
            break;
        default:
            type_name = "?";
        }
    }

    std::string str;
    str.reserve(200);
    if (telegram->operation == Telegram::Operation::RX_READ) {
        str = src_name + "(" + Helpers::hextoa(src) + ") R " + dest_name + "(" + Helpers::hextoa(dest) + "), " + type_name + "("
              + Helpers::hextoa(telegram->type_id) + "), length: " + Helpers::itoa(telegram->message_data[0])
              + ((telegram->message_length > 1) ? ", data: " + Helpers::data_to_hex(telegram->message_data + 1, telegram->message_length - 1) : "");
    } else if (telegram->dest == 0) {
        str = src_name + "(" + Helpers::hextoa(src) + ") B " + dest_name + "(" + Helpers::hextoa(dest) + "), " + type_name + "("
              + Helpers::hextoa(telegram->type_id) + "), data: " + telegram->to_string_message();
    } else {
        str = src_name + "(" + Helpers::hextoa(src) + ") W " + dest_name + "(" + Helpers::hextoa(dest) + "), " + type_name + "("
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
        for (uint8_t bit = 0; bit < 8; bit++) {
            uint8_t device_id = ((data_byte + 1) * 8) + bit;
            EMSESP::device_active(device_id, next_byte & 0x01);
            if (next_byte & 0x01) {
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

// read deviceName from telegram 0x01 offset 27 and set it to custom name
void EMSESP::process_deviceName(std::shared_ptr<const Telegram> telegram) {
    // exit if only part of name fields
    if (telegram->offset > 27 || (telegram->offset + telegram->message_length) < 29) {
        return;
    }
    char    name[16];
    uint8_t len = telegram->offset + telegram->message_length - 27;
    strlcpy(name, (const char *)&telegram->message_data[27 - telegram->offset], len < 16 ? len : 16);
    char * c = name;
    while (isprint(*c)) {
        c++;
    };
    *c = '\0';
    if (strlen(name) > 2) { // https://github.com/emsesp/EMS-ESP32/issues/2166#issuecomment-2454488657
        LOG_DEBUG("Model name received for device 0x%02X: %s", telegram->src, name);
        for (const auto & emsdevice : emsdevices) {
            if (emsdevice->is_device_id(telegram->src)) {
                emsdevice->model(name);
                break;
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

    // request the deviceName from telegram 0x01
    send_read_request(EMSdevice::EMS_TYPE_NAME, device_id, 27);
}

// find the device object that matches the deviceID and see if it has a matching telegram type handler
// but only process if the telegram is sent to us or it's a broadcast (dest=0x00=all)
// We also check for common telegram types, like the Version(0x02)
// returns false if there are none found
bool EMSESP::process_telegram(std::shared_ptr<const Telegram> telegram) {
    // if watching or reading...
    if ((telegram->type_id == read_id_ || telegram->type_id == response_id_) && (telegram->dest == EMSbus::ems_bus_id())) {
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
    // if ((telegram->dest != 0x00) && (telegram->dest != EMSbus::ems_bus_id())) {
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
    } else if (telegram->type_id == EMSdevice::EMS_TYPE_NAME) {
        process_deviceName(telegram);
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
    bool    telegram_found = false;
    uint8_t device_found   = 0;
    // broadcast or send to us
    for (const auto & emsdevice : emsdevices) {
        if (emsdevice->is_device_id(telegram->src) && (telegram->dest == 0 || telegram->dest == EMSbus::ems_bus_id())) {
            telegram_found = emsdevice->handle_telegram(telegram);
            device_found   = emsdevice->unique_id();
            break;
        }
    }
    if (!telegram_found) {
        // check for command to the device
        for (const auto & emsdevice : emsdevices) {
            if (emsdevice->is_device_id(telegram->dest) && telegram->src != EMSbus::ems_bus_id()) {
                telegram_found = emsdevice->handle_telegram(telegram);
                device_found   = emsdevice->unique_id();
                break;
            }
        }
    }
    if (!telegram_found) {
        // check for sends to master thermostat
        for (const auto & emsdevice : emsdevices) {
            if (emsdevice->is_device_id(telegram->src) && telegram->dest == 0x10) {
                telegram_found = emsdevice->handle_telegram(telegram);
                device_found   = emsdevice->unique_id();
                break;
            }
        }
    }
    for (const auto & emsdevice : emsdevices) {
        if (emsdevice->unique_id() == device_found) {
            if (!telegram_found && telegram->message_length > 0) {
                emsdevice->add_handlers_ignored(telegram->type_id);
            }
            if (wait_validate_ == telegram->type_id) {
                wait_validate_ = 0;
            }
            if (Mqtt::connected() && telegram_found
                && ((mqtt_.get_publish_onchange(emsdevice->device_type()) && emsdevice->has_update())
                    || (telegram->type_id == publish_id_ && telegram->dest == EMSbus::ems_bus_id()))) {
                if (telegram->type_id == publish_id_) {
                    publish_id_ = 0;
                }
                emsdevice->has_update(false); // reset flag
                if (!Mqtt::publish_single()) {
                    publish_device_values(emsdevice->device_type()); // publish to MQTT if we explicitly have too
                }
            }
            break;
        }
    }
    // handle unknown broadcasted telegrams (or send to us)
    if (!telegram_found && (telegram->dest == 0 || telegram->dest == EMSbus::ems_bus_id())) {
        LOG_DEBUG("No telegram type handler found for ID 0x%02X (src 0x%02X)", telegram->type_id, telegram->src);
        if (watch() == WATCH_UNKNOWN) {
            LOG_NOTICE("%s", pretty_telegram(telegram).c_str());
        }
        if (!wait_km_ && !device_found && (telegram->src != EMSbus::ems_bus_id()) && (telegram->message_length > 0)) {
            send_read_request(EMSdevice::EMS_TYPE_VERSION, telegram->src);
        }
    }

    return telegram_found;
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

void EMSESP::device_active(const uint8_t device_id, const bool active) {
    for (auto & emsdevice : emsdevices) {
        if (emsdevice && emsdevice->is_device_id(device_id)) {
            emsdevice->active(active);
            return;
        }
    }
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
                // print header, with device type translated
                shell.printfln("%s: %s (%d)", emsdevice->device_type_2_device_name_translated(), emsdevice->to_string().c_str(), emsdevice->count_entities());
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
    if (device_id == EMSbus::ems_bus_id()) {
        return false;
    }

    // first check to see if we already have it, if so update the record
    auto it = emsdevices.begin();
    for (const auto & emsdevice : emsdevices) {
        if (emsdevice && emsdevice->is_device_id(device_id)) {
            if (product_id == 0 || emsdevice->product_id() != 0) { // update only with valid product_id
                return true;
            }
            emsdevices.erase(it); // erase the old device without product_id and re detect
            break;
        }
        it++;
    }

    // look up the rest of the details using the product_id and create the new device object
    Device_record * device_p = nullptr;
    for (auto & device : device_library_) {
        if (device.product_id == product_id) {
            // sometimes boilers share the same productID as controllers
            // so only add boilers if the device_id is 0x08
            // cascaded boilers with 0x70..., map to heatsources
            if (device.device_type == DeviceType::BOILER) {
                if (device_id == EMSdevice::EMS_DEVICE_ID_BOILER) {
                    device_p = &device;
                    break;
                }
                if (device_id >= EMSdevice::EMS_DEVICE_ID_HS1 && device_id <= EMSdevice::EMS_DEVICE_ID_HS16) {
                    device_p              = &device;
                    device_p->device_type = DeviceType::HEATSOURCE;
                    break;
                }
            } else {
                // it's not a boiler, but we still have a match
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

    auto default_name = device_p->default_name;
    auto device_type  = device_p->device_type;
    auto flags        = device_p->flags;

    // check for integrated modules with same product id, but different function (device_id)
    // and force set the correct device type and hardcoded name
    if (device_type == DeviceType::HEATPUMP) {
        if (device_id == EMSdevice::EMS_DEVICE_ID_MODEM) {
            device_type  = DeviceType::GATEWAY;
            default_name = "WiFi module";
        } else if (device_id == EMSdevice::EMS_DEVICE_ID_RFBASE) {
            device_type  = DeviceType::CONNECT;
            default_name = "Wireless sensor base";
        }
    }

    if (device_id >= EMSdevice::EMS_DEVICE_ID_DHW1 && device_id <= EMSdevice::EMS_DEVICE_ID_DHW8) {
        device_type = DeviceType::WATER;
    }

    // set MH210 with id 0x20 as mixer, see https://github.com/emsesp/EMS-ESP32/discussions/2138
    if (product_id == 248 && device_id >= EMSdevice::EMS_DEVICE_ID_MIXER1 && device_id <= EMSdevice::EMS_DEVICE_ID_MIXER8) {
        default_name = "MH210";
        device_type  = DeviceType::MIXER;
        flags        = DeviceFlags::EMS_DEVICE_FLAG_MMPLUS;
    }

    // CR120 have version 22.xx, RC400/CW100 uses version 42.xx, see https://github.com/emsesp/EMS-ESP32/discussions/1779
    if (product_id == 157 && version[0] == '2') {
        flags        = DeviceFlags::EMS_DEVICE_FLAG_CR120;
        default_name = "CR120";
    }

    // empty reply to version, read a generic device from database
    if (product_id == 0) {
        // check for known device IDs
        if (device_id == EMSdevice::EMS_DEVICE_ID_RFSENSOR) {
            // see: https://github.com/emsesp/EMS-ESP32/issues/103#issuecomment-911717342 and https://github.com/emsesp/EMS-ESP32/issues/624
            default_name = "RF room temperature sensor";
            device_type  = DeviceType::THERMOSTAT;
        } else if (device_id == EMSdevice::EMS_DEVICE_ID_ROOMTHERMOSTAT || device_id == EMSdevice::EMS_DEVICE_ID_TADO_OLD) {
            // see https://github.com/emsesp/EMS-ESP32/issues/174
            default_name = "Generic Thermostat";
            device_type  = DeviceType::THERMOSTAT;
            flags        = DeviceFlags::EMS_DEVICE_FLAG_RC10 | DeviceFlags::EMS_DEVICE_FLAG_NO_WRITE;
        } else if (device_id == EMSdevice::EMS_DEVICE_ID_RS232) {
            default_name = "RS232";
            device_type  = DeviceType::CONNECT;
        } else if (device_id == EMSdevice::EMS_DEVICE_ID_TERMINAL) {
            default_name = "Terminal";
            device_type  = DeviceType::CONNECT;
        } else if (device_id == EMSdevice::EMS_DEVICE_ID_SERVICEKEY) {
            default_name = "Service Key";
            device_type  = DeviceType::CONNECT;
        } else if (device_id == EMSdevice::EMS_DEVICE_ID_CASCADE) {
            default_name = "Cascade";
            device_type  = DeviceType::CONNECT;
        } else if (device_id == EMSdevice::EMS_DEVICE_ID_EASYCOM
                   || (device_id >= EMSdevice::EMS_DEVICE_ID_MODEM && device_id <= EMSdevice::EMS_DEVICE_ID_MODEM + 5)) {
            // see https://github.com/emsesp/EMS-ESP/issues/460#issuecomment-709553012
            default_name = "Modem";
            device_type  = DeviceType::CONNECT;
        } else if (device_id == EMSdevice::EMS_DEVICE_ID_CONVERTER) {
            default_name = "Converter"; // generic
        } else if (device_id == EMSdevice::EMS_DEVICE_ID_CLOCK) {
            default_name = "Clock"; // generic
            device_type  = DeviceType::CONTROLLER;
        } else if (device_id == EMSdevice::EMS_DEVICE_ID_CONTROLLER) {
            default_name = "Generic Controller";
            device_type  = DeviceType::CONTROLLER;
        } else if (device_id == EMSdevice::EMS_DEVICE_ID_BOILER) {
            default_name = "Generic Boiler";
            device_type  = DeviceType::BOILER;
            flags        = DeviceFlags::EMS_DEVICE_FLAG_HEATPUMP;
            LOG_WARNING("Unknown EMS boiler. Using generic profile. Please report on GitHub.");
        } else if (device_id >= 0x68 && device_id <= 0x6F) {
            // test for https://github.com/emsesp/EMS-ESP32/issues/882
            default_name = "Cascaded Controller";
            device_type  = DeviceType::CONTROLLER;
        } else {
            LOG_WARNING("Unrecognized EMS device (deviceID 0x%02X, no productID). Please report on GitHub.", device_id);
            return false;
        }
    }

    LOG_DEBUG("Adding new device %s (deviceID 0x%02X, productID %d, version %s)", default_name, device_id, product_id, version);
    emsdevices.push_back(EMSFactory::add(device_type, device_id, product_id, version, default_name, flags, brand));

    // see if we have a custom device name in our Customizations list, and if so set it
    webCustomizationService.read([&](WebCustomization const & settings) {
        for (EntityCustomization e : settings.entityCustomizations) {
            if ((e.device_id == device_id) && (e.product_id == product_id)) {
                LOG_DEBUG("Have customizations for %s with deviceID 0x%02X productID %d", e.custom_name.c_str(), device_id, product_id);
                emsdevices.back()->custom_name(e.custom_name);
                break;
            }
        }
    });

    // assign a unique ID. Note that this is not actual remain unique after a restart as it's dependent on the order that devices are found
    // can't be 0 otherwise web won't work
    emsdevices.back()->unique_id(++unique_id_count_);

    // sort devices based on type
    std::sort(emsdevices.begin(), emsdevices.end(), [](const std::unique_ptr<EMSdevice> & a, const std::unique_ptr<EMSdevice> & b) {
        return a->device_type() < b->device_type();
    });

    fetch_device_values(device_id); // go and fetch its device entity data

    // Print to LOG showing we've added a new device
    LOG_INFO("Detected EMS device: %s (0x%02X)", EMSdevice::device_type_2_device_name(device_type), device_id);

    // register the MQTT subscribe topic for this device
    // except for connect, controller and gateway
    if ((device_type == DeviceType::CONNECT) || (device_type == DeviceType::CONTROLLER) || (device_type == DeviceType::GATEWAY)) {
        return true;
    }

    // MQTT subscribe to the device e.g. "ems-esp/boiler/#"
    auto topic = std::string(EMSdevice::device_type_2_device_name(device_type)) + "/#";
    Mqtt::subscribe(device_type, topic, nullptr);

    return true;
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
    if (((first_value & 0x7F) == EMSbus::ems_bus_id()) && (length > 1)) {
        // if we ask ourself at roomcontrol for version e.g. 0B 98 02 00 20
        Roomctrl::check(data[1], data, length);
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
                txservice_.reset_retry_count();
                tx_successful = true; // no retries
            } else {
                LOG_ERROR("Last Tx write host reply: 0x%02X", first_value);
            }
        } else if (tx_state == Telegram::Operation::TX_READ && length > 1) {
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
                if ((response_id_ == 0 || read_id_ > 0) && (txservice_.read_next_tx(data[3], length) > 0)) {
                    read_next_ = true;
                    txservice_.send(); // read next part withing same poll or:
                    // txservice_.send_poll(); // close the bus, next request in new poll
                } else {
                    read_next_ = false;
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
        if (poll_id == EMSbus::ems_bus_id()) {
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
        } else {
            // send remote room temperature if active
            Roomctrl::send(poll_id);
        }

        return;
    } else {
#ifdef EMSESP_UART_DEBUG
        LOG_TRACE("[UART_DEBUG] Reply after %d ms: %s", ::millis() - rx_time_, Helpers::data_to_hex(data, length).c_str());
#endif
        Roomctrl::check(data[1], data, length); // check if there is a message for the roomcontroller

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

// EMSESP main class

EMSESP::EMSESP()
#ifndef EMSESP_STANDALONE
    : telnet_([this](Stream & stream, const IPAddress & addr, uint16_t port) -> std::shared_ptr<uuid::console::Shell> {
        return std::make_shared<EMSESPConsole>(*this, stream, addr, port);
    })
#endif
{
}

// start all the core services
// the services must be loaded in the correct order
void EMSESP::start() {
#ifndef EMSESP_STANDALONE
    system_.PSram(ESP.getPsramSize());
#endif

    serial_console_.begin(SERIAL_CONSOLE_BAUD_RATE);

    // always start a serial console if we're running standalone, except if we're running unit tests
#if defined(EMSESP_STANDALONE) || defined(EMSESP_DEBUG)
#ifndef EMSESP_UNITY
    start_serial_console();
#endif
#endif

// start the file system
#ifndef EMSESP_STANDALONE
    if (!LittleFS.begin(true)) {
        LOG_ERROR("LittleFS Mount Failed");
        return;
    }
#endif

// do a quick scan of the filesystem to see if we a settings file in the /config folder
// so we know if this is a new factory install or not
#ifndef EMSESP_STANDALONE
    File root             = LittleFS.open(EMSESP_SETTINGS_FILE);
    bool factory_settings = !root;
    if (!root) {
        LOG_WARNING("No settings found on filesystem. Using factory settings.");
    }
    root.close();
#else
    bool factory_settings = false;
#endif

    // start web log service. now we can start capturing logs to the web log
    webLogService.begin();

    // loads core system services settings (network, mqtt, ap, ntp etc)
    esp32React.begin();

#ifndef EMSESP_STANDALONE
    LOG_INFO("EMS-ESP version %s", EMSESP_APP_VERSION);
    LOG_DEBUG("Boot partition %s, Active partition %s", esp_ota_get_boot_partition()->label, esp_ota_get_running_partition()->label);
#else
    LOG_INFO("EMS-ESP version %s", EMSESP_APP_VERSION);
#endif
    LOG_DEBUG("System is running in Debug mode");
    LOG_INFO("Last system reset reason Core0: %s, Core1: %s", system_.reset_reason(0).c_str(), system_.reset_reason(1).c_str());

// see if we're restoring a settings file
#ifndef EMSESP_STANDALONE
    if (system_.check_restore()) {
        LOG_WARNING("EMS-ESP will restart to apply new settings. Please wait.");
        system_.system_restart();
    };
#endif

    if (!nvs_.begin("ems-esp", false, "nvs1")) { // try bigger nvs partition on 16M flash first
        nvs_.begin("ems-esp", false, "nvs");     // fallback to small nvs
    }

    LOG_DEBUG("NVS device information: %s", system_.getBBQKeesGatewayDetails().isEmpty() ? "not set" : system_.getBBQKeesGatewayDetails().c_str());

    webSettingsService.begin(); // load EMS-ESP Application settings

    // do any system upgrades
    if (system_.check_upgrade(factory_settings)) {
        LOG_WARNING("System needs a restart to apply new settings. Please wait.");
        system_.system_restart();
    };

    // Load our library of known devices into stack mem. Names are stored in Flash memory
    device_library_ = {
#include "device_library.h"
    };
    LOG_INFO("Loaded EMS device library (%d entries)", device_library_.size());

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

    // start services
    if (system_.modbus_enabled()) {
        modbus_ = new Modbus;
        modbus_->start(1, system_.modbus_port(), system_.modbus_max_clients(), system_.modbus_timeout() * 1000);
    }

    mqtt_.start();              // mqtt init
    system_.start();            // starts commands, led, adc, button, network (sets hostname), syslog & uart
    shower_.start();            // initialize shower timer and shower alert
    temperaturesensor_.start(); // Temperature external sensors
    analogsensor_.start();      // Analog external sensors

    // start web services
    webLogService.start();     // apply settings to weblog service
    webModulesService.begin(); // setup the external library modules
    webServer.begin();         // start the web server
    LOG_INFO("Starting Web Server");
}

void EMSESP::start_serial_console() {
    shell_ = std::make_shared<EMSESPConsole>(*this, serial_console_, true);
    shell_->maximum_log_messages(100);
    shell_->start();
#if defined(EMSESP_DEBUG)
    shell_->log_level(uuid::log::Level::DEBUG);
    shell_->add_flags(CommandFlags::ADMIN); // always start in su/admin mode when compiled with debug
#else
    shell_->log_level(uuid::log::Level::TRACE);
#endif
}

void EMSESP::shell_prompt() {
#ifndef EMSESP_STANDALONE
    serial_console_.println();
    serial_console_.printf("EMS-ESP %s: press CTRL-D to activate this serial console", EMSESP_APP_VERSION);
    serial_console_.println();
#endif
}

// main loop calling all services
void EMSESP::loop() {
    esp32React.loop(); // web services
    system_.loop();    // does LED and checks system health, and syslog service

    // run the loop, unless we're in the middle of an OTA upload
    if (EMSESP::system_.systemStatus() == SYSTEM_STATUS::SYSTEM_STATUS_NORMAL) {
        webLogService.loop();       // log in Web UI
        rxservice_.loop();          // process any incoming Rx telegrams
        shower_.loop();             // check for shower on/off
        temperaturesensor_.loop();  // read sensor temperatures
        analogsensor_.loop();       // read analog sensor values
        publish_all_loop();         // with HA messages in parts to avoid flooding the MQTT queue
        mqtt_.loop();               // sends out anything in the MQTT queue
        webModulesService.loop();   // loop through the external library modules
        if (system_.PSram() == 0) { // run non-async if there is no PSRAM available
            webSchedulerService.loop();
        }
        scheduled_fetch_values(); // force a query on the EMS devices to fetch latest data at a set interval (1 min)
    }

    if (EMSESP::system_.systemStatus() == SYSTEM_STATUS::SYSTEM_STATUS_PENDING_UPLOAD) {
        // start an upload from a URL, assuming the URL exists and set from a previous pass
        // Note this next call is synchronous and blocking.
        if (!system_.uploadFirmwareURL()) {
            // upload failed, send a "reset" to return back to normal
            Shell::loop_all(); // flush log buffers so latest error message are shown in console
            system_.uploadFirmwareURL("reset");
            EMSESP::system_.systemStatus(SYSTEM_STATUS::SYSTEM_STATUS_ERROR_UPLOAD);
        }
    }

    uuid::loop();

#ifndef EMSESP_STANDALONE
    if (system_.telnet_enabled()) {
        telnet_.loop();
    }
#endif

    Shell::loop_all();

    static bool show_prompt = true;

    // user has to CTRL-D to create a serial console stream, exit command will close it
    // this saves around 2kb of heap memory
    if (shell_) {
        if (!shell_->running()) {
            shell_.reset();
#ifdef EMSESP_STANDALONE
            ::exit(0); // kill session
#endif
        }
    } else {
        if (show_prompt) {
            shell_prompt();
            show_prompt = false; // only show it once
        }
        int c = serial_console_.read();
        if (c != -1) {
            show_prompt = true;
        }
        // https://daleswanson.org/ascii.htm#:~:text=0
        // CTRL-D to open
        if (c == '\x04') {
            start_serial_console();
        }
    }
}

} // namespace emsesp
