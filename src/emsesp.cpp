/*
 * EMS-ESP - https://github.com/emsesp/EMS-ESP
 * Copyright 2020  Paul Derbyshire
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

namespace emsesp {

#if defined(EMSESP_STANDALONE)
uint32_t heap_start = 0;
#else
uint32_t                heap_start = ESP.getFreeHeap(); // get initial available heap memory
#endif

AsyncWebServer webServer(80);

#if defined(EMSESP_STANDALONE)
FS                      dummyFS;
ESP8266React            EMSESP::esp8266React(&webServer, &dummyFS);
WebSettingsService      EMSESP::webSettingsService      = WebSettingsService(&webServer, &dummyFS, EMSESP::esp8266React.getSecurityManager());
WebCustomizationService EMSESP::webCustomizationService = WebCustomizationService(&webServer, &dummyFS, EMSESP::esp8266React.getSecurityManager());
#else
ESP8266React            EMSESP::esp8266React(&webServer, &LITTLEFS);
WebSettingsService      EMSESP::webSettingsService      = WebSettingsService(&webServer, &LITTLEFS, EMSESP::esp8266React.getSecurityManager());
WebCustomizationService EMSESP::webCustomizationService = WebCustomizationService(&webServer, &LITTLEFS, EMSESP::esp8266React.getSecurityManager());
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

// The services
RxService    EMSESP::rxservice_;    // incoming Telegram Rx handler
TxService    EMSESP::txservice_;    // outgoing Telegram Tx handler
Mqtt         EMSESP::mqtt_;         // mqtt handler
System       EMSESP::system_;       // core system services
Console      EMSESP::console_;      // telnet and serial console
DallasSensor EMSESP::dallassensor_; // Dallas sensors
AnalogSensor EMSESP::analogsensor_; // Analog sensors
Shower       EMSESP::shower_;       // Shower logic

// static/common variables
uint8_t  EMSESP::actual_master_thermostat_ = EMSESP_DEFAULT_MASTER_THERMOSTAT; // which thermostat leads when multiple found
uint16_t EMSESP::watch_id_                 = WATCH_ID_NONE;                    // for when log is TRACE. 0 means no trace set
uint8_t  EMSESP::watch_                    = 0;                                // trace off
uint16_t EMSESP::read_id_                  = WATCH_ID_NONE;
bool     EMSESP::read_next_                = false;
uint16_t EMSESP::publish_id_               = 0;
bool     EMSESP::tap_water_active_         = false; // for when Boiler states we having running warm water. used in Shower()
uint32_t EMSESP::last_fetch_               = 0;
uint8_t  EMSESP::publish_all_idx_          = 0;
uint8_t  EMSESP::unique_id_count_          = 0;
bool     EMSESP::trace_raw_                = false;
uint16_t EMSESP::wait_validate_            = 0;
bool     EMSESP::wait_km_                  = true;

// for a specific EMS device go and request data values
// or if device_id is 0 it will fetch from all our known and active devices
void EMSESP::fetch_device_values(const uint8_t device_id) {
    for (const auto & emsdevice : emsdevices) {
        if (emsdevice) {
            if ((device_id == 0) || emsdevice->is_device_id(device_id)) {
                emsdevice->fetch_values();
                if (device_id != 0) {
                    return; // quit, we only want to return the selected device
                }
            }
        }
    }
}

// see if the deviceID exists
bool EMSESP::valid_device(const uint8_t device_id) {
    for (const auto & emsdevice : emsdevices) {
        if (emsdevice) {
            if (emsdevice->is_device_id(device_id)) {
                return true;
            }
        }
    }
    return false; // can't find it
}

// for a specific EMS device type go and request data values
void EMSESP::fetch_device_values_type(const uint8_t device_type) {
    for (const auto & emsdevice : emsdevices) {
        if ((emsdevice) && (emsdevice->device_type() == device_type)) {
            emsdevice->fetch_values();
        }
    }
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

/**
* if thermostat master is 0x18 it handles only ww and hc1, hc2..hc8 handled by devices 0x19..0x1F
* we send to right device and match all reads to 0x18
*/
uint8_t EMSESP::check_master_device(const uint8_t device_id, const uint16_t type_id, const bool read) {
    if (actual_master_thermostat_ == 0x18) {
        uint16_t mon_ids[]     = {0x02A5, 0x02A6, 0x02A7, 0x02A8, 0x02A9, 0x02AA, 0x02AB, 0x02AC};
        uint16_t set_ids[]     = {0x02B9, 0x02BA, 0x02BB, 0x02BC, 0x02BD, 0x02BE, 0x02BF, 0x02C0};
        uint16_t summer_ids[]  = {0x02AF, 0x02B0, 0x02B1, 0x02B2, 0x02B3, 0x02B4, 0x02B5, 0x02B6};
        uint16_t curve_ids[]   = {0x029B, 0x029C, 0x029D, 0x029E, 0x029F, 0x02A0, 0x02A1, 0x02A2};
        uint16_t summer2_ids[] = {0x0471, 0x0472, 0x0473, 0x0474, 0x0475, 0x0476, 0x0477, 0x0478};
        uint16_t master_ids[]  = {0x02F5, 0x031B, 0x031D, 0x031E, 0x023A, 0x0267, 0x0240};
        // look for heating circuits
        for (uint8_t i = 0; i < sizeof(mon_ids) / 2; i++) {
            if (type_id == mon_ids[i] || type_id == set_ids[i] || type_id == summer_ids[i] || type_id == curve_ids[i] || type_id == summer2_ids[i]) {
                if (read) {
                    // receiving telegrams and map all to master thermostat at 0x18 (src manipulated)
                    return 0x18;
                } else {
                    // sending telegrams to the individual thermostats (dst manipulated)
                    return 0x18 + i;
                }
            }
        }
        // look for ids that are only handled by master
        for (uint8_t i = 0; i < sizeof(master_ids) / 2; i++) {
            if (type_id == master_ids[i]) {
                return 0x18;
            }
        }
    }

    return device_id;
}

void EMSESP::actual_master_thermostat(const uint8_t device_id) {
    actual_master_thermostat_ = device_id;
}

uint8_t EMSESP::actual_master_thermostat() {
    return actual_master_thermostat_;
}

// to watch both type IDs and deviceIDs
void EMSESP::watch_id(uint16_t watch_id) {
    watch_id_ = watch_id;
}

// resets all counters and bumps the UART
// this is called when the tx_mode is persisted in the FS either via Web UI or the console
void EMSESP::init_uart() {
    uint8_t tx_mode;
    uint8_t rx_gpio;
    uint8_t tx_gpio;
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
        LOG_WARNING(F("Invalid UART Rx/Tx GPIOs. Check config."));
    }

    txservice_.start(); // sends out request to EMS bus for all devices

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
    if (total_fail < total_sent) {
        if (((total_fail * 100) / total_sent) > EMSbus::EMS_TX_ERROR_LIMIT) {
            return BUS_STATUS_TX_ERRORS;
        }
    }

    return BUS_STATUS_CONNECTED;
}

// show the EMS bus status plus both Rx and Tx queues
void EMSESP::show_ems(uuid::console::Shell & shell) {
    // EMS bus information
    switch (bus_status()) {
    case BUS_STATUS_OFFLINE:
        shell.printfln(F("EMS Bus is disconnected."));
        break;
    case BUS_STATUS_TX_ERRORS:
        shell.printfln(F("EMS Bus is connected, but Tx is not stable."));
        break;
    case BUS_STATUS_CONNECTED:
    default:
        shell.printfln(F("EMS Bus is connected."));
        break;
    }

    shell.println();

    if (bus_status() != BUS_STATUS_OFFLINE) {
        shell.printfln(F("EMS Bus info:"));
        EMSESP::webSettingsService.read([&](WebSettings & settings) { shell.printfln(F("  Tx mode: %d"), settings.tx_mode); });
        shell.printfln(F("  Bus protocol: %s"), EMSbus::is_ht3() ? F("HT3") : F("Buderus"));
        shell.printfln(F("  #recognized EMS devices: %d"), (EMSESP::emsdevices).size());
        shell.printfln(F("  #telegrams received: %d"), rxservice_.telegram_count());
        shell.printfln(F("  #read requests sent: %d"), txservice_.telegram_read_count());
        shell.printfln(F("  #write requests sent: %d"), txservice_.telegram_write_count());
        shell.printfln(F("  #incomplete telegrams: %d"), rxservice_.telegram_error_count());
        shell.printfln(F("  #read fails (after %d retries): %d"), TxService::MAXIMUM_TX_RETRIES, txservice_.telegram_read_fail_count());
        shell.printfln(F("  #write fails (after %d retries): %d"), TxService::MAXIMUM_TX_RETRIES, txservice_.telegram_write_fail_count());
        shell.printfln(F("  Rx line quality: %d%%"), rxservice_.quality());
        shell.printfln(F("  Tx line quality: %d%%"), (txservice_.read_quality() + txservice_.read_quality()) / 2);
        shell.println();
    }

    // Rx queue
    auto rx_telegrams = rxservice_.queue();
    if (rx_telegrams.empty()) {
        shell.printfln(F("Rx Queue is empty"));
    } else {
        shell.printfln(F("Rx Queue (%ld telegram%s):"), rx_telegrams.size(), rx_telegrams.size() == 1 ? "" : "s");
        for (const auto & it : rx_telegrams) {
            shell.printfln(F(" [%02d] %s"), it.id_, pretty_telegram(it.telegram_).c_str());
        }
    }

    shell.println();

    // Tx queue
    auto tx_telegrams = txservice_.queue();
    if (tx_telegrams.empty()) {
        shell.printfln(F("Tx Queue is empty"));
    } else {
        shell.printfln(F("Tx Queue (%ld telegram%s):"), tx_telegrams.size(), tx_telegrams.size() == 1 ? "" : "s");

        std::string op;
        for (const auto & it : tx_telegrams) {
            if ((it.telegram_->operation) == Telegram::Operation::TX_RAW) {
                op = read_flash_string(F("RAW  "));
            } else if ((it.telegram_->operation) == Telegram::Operation::TX_READ) {
                op = read_flash_string(F("READ "));
            } else if ((it.telegram_->operation) == Telegram::Operation::TX_WRITE) {
                op = read_flash_string(F("WRITE"));
            }
            shell.printfln(F(" [%02d%c] %s %s"), it.id_, ((it.retry_) ? '*' : ' '), op.c_str(), pretty_telegram(it.telegram_).c_str());
        }
    }

    shell.println();
}

// show EMS device values to the shell console
// generate_values_json is called in verbose mode
void EMSESP::show_device_values(uuid::console::Shell & shell) {
    if (emsdevices.empty()) {
        shell.printfln(F("No EMS devices detected."));
        shell.println();
        return;
    }

    // do this in the order of factory classes to keep a consistent order when displaying
    for (const auto & device_class : EMSFactory::device_handlers()) {
        for (const auto & emsdevice : emsdevices) {
            if ((emsdevice) && (emsdevice->device_type() == device_class.first)) {
                // print header
                shell.printfln(F("%s: %s (%d)"), emsdevice->device_type_name().c_str(), emsdevice->to_string().c_str(), emsdevice->count_entities());

                DynamicJsonDocument doc(EMSESP_JSON_SIZE_XXLARGE_DYN); // use max size
                JsonObject          json = doc.to<JsonObject>();

                emsdevice->generate_values(json, DeviceValueTAG::TAG_NONE, true, EMSdevice::OUTPUT_TARGET::API_VERBOSE); // verbose mode and nested

                // print line
                uint8_t id = 0;
                for (JsonPair p : json) {
                    const char * key = p.key().c_str();
                    shell.printf("  %s: ", key);
                    JsonVariant data = p.value();
                    shell.print(COLOR_BRIGHT_GREEN);
                    if (data.is<const char *>()) {
                        shell.print(data.as<const char *>());
                    } else if (data.is<int>()) {
                        shell.print(data.as<int>());
                    } else if (data.is<float>()) {
                        char s[10];
                        shell.print(Helpers::render_value(s, (float)data.as<float>(), 1));
                    } else if (data.is<bool>()) {
                        shell.print(data.as<bool>() ? F_(on) : F_(off));
                    }

                    // if there is a uom print it
                    std::string uom = emsdevice->get_value_uom(key);
                    if (uom == "°C" && EMSESP::system_.fahrenheit()) {
                        uom = "°F";
                    }
                    if (!uom.empty()) {
                        shell.print(' ');
                        shell.print(uom);
                    }

                    shell.print(COLOR_RESET);
                    shell.println();
                    id++;
                }
                shell.println();
            }
        }
    }
}

// show Dallas temperature sensors and Analog sensors
void EMSESP::show_sensor_values(uuid::console::Shell & shell) {
    if (dallassensor_.have_sensors()) {
        shell.printfln(F("Temperature sensors:"));
        char    s[10];
        char    s2[10];
        uint8_t fahrenheit = EMSESP::system_.fahrenheit() ? 2 : 0;

        for (const auto & sensor : dallassensor_.sensors()) {
            if (Helpers::hasValue(sensor.temperature_c)) {
                shell.printfln(F("  %s: %s%s °%c%s (offset %s, ID: %s)"),
                               sensor.name().c_str(),
                               COLOR_BRIGHT_GREEN,
                               Helpers::render_value(s, sensor.temperature_c, 10, fahrenheit),
                               (fahrenheit == 0) ? 'C' : 'F',
                               COLOR_RESET,
                               Helpers::render_value(s2, sensor.offset(), 10, fahrenheit),
                               sensor.id_str().c_str());
            } else {
                shell.printfln(F("  %s (offset %s, ID: %s)"),
                               sensor.name().c_str(),
                               Helpers::render_value(s, sensor.offset(), 10, fahrenheit),
                               sensor.id_str().c_str());
            }
        }
        shell.println();
    }

    if (analogsensor_.have_sensors()) {
        char s[10];
        char s2[10];
        shell.printfln(F("Analog sensors:"));
        for (const auto & sensor : analogsensor_.sensors()) {
            switch (sensor.type()) {
            case AnalogSensor::AnalogType::ADC:
                shell.printfln(F("  %s: %s%s %s%s (Type: ADC, Factor: %s, Offset: %d)"),
                               sensor.name().c_str(),
                               COLOR_BRIGHT_GREEN,
                               Helpers::render_value(s, sensor.value(), 2),
                               EMSdevice::uom_to_string(sensor.uom()).c_str(),
                               COLOR_RESET,
                               Helpers::render_value(s2, sensor.factor(), 4),
                               sensor.offset());
                break;
            default:
            case AnalogSensor::AnalogType::DIGITAL_IN:
            case AnalogSensor::AnalogType::COUNTER:
                shell.printfln(F("  %s: %s%d%s (Type: %s)"),
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
        publish_other_values();      // switch and heat pump
        publish_sensor_values(true); // includes dallas and analog sensors
        system_.send_heartbeat();
    }
}

// on command "publish HA" loop and wait between devices for publishing all sensors
void EMSESP::publish_all_loop() {
    if (!Mqtt::connected() || !publish_all_idx_) {
        return;
    }

    // wait for free queue before sending next message, HA-messages are also queued
    if (!Mqtt::is_empty()) {
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
    dallassensor_.reload();
    analogsensor_.reload();
}

// create json doc for the devices values and add to MQTT publish queue
// this will also create the HA /config topic
// generate_values_json is called to build the device value (dv) object array
void EMSESP::publish_device_values(uint8_t device_type) {
    DynamicJsonDocument doc(EMSESP_JSON_SIZE_XLARGE_DYN); // use max size
    JsonObject          json         = doc.to<JsonObject>();
    bool                need_publish = false;

    bool nested = (Mqtt::is_nested());

    // group by device type
    for (const auto & emsdevice : emsdevices) {
        if (emsdevice && (emsdevice->device_type() == device_type)) {
            // specially for HA
            // we may have some RETAINED /config topics that reference fields in the data payloads that no longer exist
            // remove them immediately to prevent HA from complaining
            // we need to do this first before the data payload is published, and only done once!
            if (Mqtt::ha_enabled() && emsdevice->ha_config_firstrun()) {
                emsdevice->ha_config_clear();
                emsdevice->ha_config_firstrun(false);
            }

            // if its a boiler, generate json for each group and publish it directly. not nested
            if (device_type == DeviceType::BOILER) {
                if (emsdevice->generate_values(json, DeviceValueTAG::TAG_BOILER_DATA, false, EMSdevice::OUTPUT_TARGET::MQTT)) {
                    Mqtt::publish(Mqtt::tag_to_topic(device_type, DeviceValueTAG::TAG_BOILER_DATA), json);
                }
                doc.clear();
                if (emsdevice->generate_values(json, DeviceValueTAG::TAG_DEVICE_DATA_WW, false, EMSdevice::OUTPUT_TARGET::MQTT)) {
                    Mqtt::publish(Mqtt::tag_to_topic(device_type, DeviceValueTAG::TAG_DEVICE_DATA_WW), json);
                }
                need_publish = false;
            }

            // Thermostat
            else if (device_type == DeviceType::THERMOSTAT) {
                // only publish the single master thermostat
                if (emsdevice->device_id() == EMSESP::actual_master_thermostat()) {
                    if (nested) {
                        need_publish |= emsdevice->generate_values(json, DeviceValueTAG::TAG_NONE, true, EMSdevice::OUTPUT_TARGET::MQTT); // nested
                    } else {
                        if (emsdevice->generate_values(json, DeviceValueTAG::TAG_THERMOSTAT_DATA, false, EMSdevice::OUTPUT_TARGET::MQTT)) { // not nested
                            Mqtt::publish(Mqtt::tag_to_topic(device_type, DeviceValueTAG::TAG_NONE), json);
                        }
                        doc.clear();
                        for (uint8_t hc_tag = DeviceValueTAG::TAG_HC1; hc_tag <= DeviceValueTAG::TAG_HC8; hc_tag++) {
                            if (emsdevice->generate_values(json, hc_tag, false, EMSdevice::OUTPUT_TARGET::MQTT)) { // not nested
                                Mqtt::publish(Mqtt::tag_to_topic(device_type, hc_tag), json);
                            }
                            doc.clear();
                        }
                        need_publish = false;
                    }
                }
            }

            // Mixer
            else if (device_type == DeviceType::MIXER) {
                if (nested) {
                    need_publish |= emsdevice->generate_values(json, DeviceValueTAG::TAG_NONE, true, EMSdevice::OUTPUT_TARGET::MQTT); // nested
                } else {
                    for (uint8_t hc_tag = DeviceValueTAG::TAG_HC1; hc_tag <= DeviceValueTAG::TAG_WWC4; hc_tag++) {
                        if (emsdevice->generate_values(json, hc_tag, false, EMSdevice::OUTPUT_TARGET::MQTT)) { // not nested
                            Mqtt::publish(Mqtt::tag_to_topic(device_type, hc_tag), json);
                        }
                        doc.clear();
                    }
                    need_publish = false;
                }

            } else {
                // for all other devices add the values to the json
                need_publish |= emsdevice->generate_values(json, DeviceValueTAG::TAG_NONE, true, EMSdevice::OUTPUT_TARGET::MQTT); // nested
            }
        }

        // we want to create the /config topic after the data payload to prevent HA from throwing up a warning
        if (Mqtt::ha_enabled()) {
            emsdevice->publish_mqtt_ha_entity_config();
        }
    }

    // publish it under a single topic, only if we have data to publish
    if (need_publish) {
        if (doc.overflowed()) {
            LOG_WARNING(F("MQTT buffer overflow, please use individual topics"));
        }
        char topic[Mqtt::MQTT_TOPIC_MAX_SIZE];
        snprintf(topic, sizeof(topic), "%s_data", EMSdevice::device_type_2_device_name(device_type).c_str());
        Mqtt::publish(topic, json);
    }
}

// call the devices that don't need special attention
void EMSESP::publish_other_values() {
    publish_device_values(EMSdevice::DeviceType::SWITCH);
    publish_device_values(EMSdevice::DeviceType::HEATPUMP);
}

// publish both the dallas and analog sensor values
void EMSESP::publish_sensor_values(const bool time, const bool force) {
    if (dallas_enabled()) {
        if (dallassensor_.updated_values() || time || force) {
            dallassensor_.publish_values(force);
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
    StaticJsonDocument<EMSESP_JSON_SIZE_SMALL> doc;

    char buffer[100];
    doc["src"]    = Helpers::hextoa(buffer, telegram->src);
    doc["dest"]   = Helpers::hextoa(buffer, telegram->dest);
    doc["type"]   = Helpers::hextoa(buffer, telegram->type_id);
    doc["offset"] = Helpers::hextoa(buffer, telegram->offset);
    strcpy(buffer, Helpers::data_to_hex(telegram->message_data, telegram->message_length).c_str()); // telegram is without crc
    doc["data"] = buffer;

    if (telegram->message_length <= 4) {
        uint32_t value = 0;
        for (uint8_t i = 0; i < telegram->message_length; i++) {
            value = (value << 8) + telegram->message_data[i];
        }
        doc["value"] = value;
    }

    Mqtt::publish(F_(response), doc.as<JsonObject>());
}

// builds json with the detail of each value, for a specific EMS device type or the dallas sensor
bool EMSESP::get_device_value_info(JsonObject & root, const char * cmd, const int8_t id, const uint8_t devicetype) {
    for (const auto & emsdevice : emsdevices) {
        if (emsdevice->device_type() == devicetype) {
            return emsdevice->get_value_info(root, cmd, id);
        }
    }

    // specific for the dallassensor
    if (devicetype == DeviceType::DALLASSENSOR) {
        EMSESP::dallassensor_.get_value_info(root, cmd, id);
        return true;
    }

    // analog sensor
    if (devicetype == DeviceType::ANALOGSENSOR) {
        EMSESP::analogsensor_.get_value_info(root, cmd, id);
        return true;
    }

    return false;
}

// search for recognized device_ids : Me, All, otherwise print hex value
std::string EMSESP::device_tostring(const uint8_t device_id) {
    if ((device_id & 0x7F) == rxservice_.ems_bus_id()) {
        return read_flash_string(F("Me"));
    } else if (device_id == 0x00) {
        return read_flash_string(F("All"));
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
    std::string direction("");
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
        type_name = read_flash_string(F("Version"));
    } else if (telegram->type_id == EMSdevice::EMS_TYPE_UBADevices) {
        type_name = read_flash_string(F("UBADevices"));
    }

    // if we don't know the type show
    if (type_name.empty()) {
        type_name = read_flash_string(F("?"));
    }

    if (telegram->operation == Telegram::Operation::RX_READ) {
        direction = read_flash_string(F("<-"));
    } else {
        direction = read_flash_string(F("->"));
    }

    std::string str;
    str.reserve(200);
    str = src_name + "(" + Helpers::hextoa(src) + ") " + direction + " " + dest_name + "(" + Helpers::hextoa(dest) + "), " + type_name + "("
          + Helpers::hextoa(telegram->type_id) + "), data: " + telegram->to_string_message();

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
                        LOG_DEBUG(F("New EMS device detected with ID 0x%02X. Requesting version information."), device_id);
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
// We also check for common telgram types, like the Version(0x02)
// returns false if there are none found
bool EMSESP::process_telegram(std::shared_ptr<const Telegram> telegram) {
    // if watching or reading...
    if ((telegram->type_id == read_id_) && (telegram->dest == txservice_.ems_bus_id())) {
        LOG_INFO(F("%s"), pretty_telegram(telegram).c_str());
        if (Mqtt::send_response()) {
            publish_response(telegram);
        }

        if (!read_next_) {
            read_id_ = WATCH_ID_NONE;
        }
        read_next_ = false;
    } else if (watch() == WATCH_ON) {
        if ((watch_id_ == WATCH_ID_NONE) || (telegram->type_id == watch_id_)
            || ((watch_id_ < 0x80) && ((telegram->src == watch_id_) || (telegram->dest == watch_id_)))) {
            LOG_NOTICE(F("%s"), pretty_telegram(telegram).c_str());
        } else if (!trace_raw_) {
            LOG_TRACE(F("%s"), pretty_telegram(telegram).c_str());
        }
    } else if (!trace_raw_) {
        LOG_TRACE(F("%s"), pretty_telegram(telegram).c_str());
    }

    // only process broadcast telegrams or ones sent to us on request
    // if ((telegram->dest != 0x00) && (telegram->dest != rxservice_.ems_bus_id())) {
    if (telegram->operation == Telegram::Operation::RX_READ) {
        // LOG_DEBUG(F("read telegram received, not processing"));
        return false;
    }

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
    // after the telegram has been processed, call see if there have been values changed and we need to do a MQTT publish
    bool found       = false;
    bool knowndevice = false;
    for (const auto & emsdevice : emsdevices) {
        if (emsdevice) {
            if (emsdevice->is_device_id(telegram->src) || emsdevice->is_device_id(telegram->dest)) {
                knowndevice = true;
                found       = emsdevice->handle_telegram(telegram);
                if (found && emsdevice->is_device_id(telegram->dest)) {
                    LOG_DEBUG(F("Process setting 0x%02X for device 0x%02X"), telegram->type_id, telegram->dest);
                }
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
                break;
            }
        }
    }

    if (!found) {
        LOG_DEBUG(F("No telegram type handler found for ID 0x%02X (src 0x%02X)"), telegram->type_id, telegram->src);
        if (watch() == WATCH_UNKNOWN) {
            LOG_NOTICE(F("%s"), pretty_telegram(telegram).c_str());
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
        if (emsdevice) {
            if (emsdevice->is_device_id(device_id)) {
                return true;
            }
        }
    }

    return false; // not found
}

// for each associated EMS device go and get its system information
void EMSESP::show_devices(uuid::console::Shell & shell) {
    if (emsdevices.empty()) {
        shell.printfln(F("No EMS devices detected. Try using 'scan devices' from the ems menu."));
        shell.println();
        return;
    }

    shell.printfln(F("These EMS devices are currently active:"));
    shell.println();

    // count the number of thermostats
    uint8_t num_thermostats = 0;
    for (const auto & emsdevice : emsdevices) {
        if ((emsdevice) && (emsdevice->device_type() == DeviceType::THERMOSTAT)) {
            num_thermostats++;
        }
    }

    // for all device objects from emsdevice.h (UNKNOWN, SYSTEM, BOILER, THERMOSTAT, MIXER, SOLAR, HEATPUMP, GATEWAY, SWITCH, CONTROLLER, CONNECT)
    // so we keep a consistent order
    for (const auto & device_class : EMSFactory::device_handlers()) {
        for (const auto & emsdevice : emsdevices) {
            if ((emsdevice) && (emsdevice->device_type() == device_class.first)) {
                shell.printf(F("%s: %s"), emsdevice->device_type_name().c_str(), emsdevice->to_string().c_str());
                if ((num_thermostats > 1) && (emsdevice->device_type() == EMSdevice::DeviceType::THERMOSTAT)
                    && (emsdevice->device_id() == actual_master_thermostat())) {
                    shell.printf(F(" **master device**"));
                }
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
        if (emsdevice) {
            if (emsdevice->is_device_id(device_id)) {
                LOG_DEBUG(F("Updating details for already active deviceID 0x%02X"), device_id);
                emsdevice->product_id(product_id);
                emsdevice->version(version);
                // only set brand if it doesn't already exist
                if (emsdevice->brand() == EMSdevice::Brand::NO_BRAND) {
                    emsdevice->brand(brand);
                }
                // find the name and flags in our database
                for (const auto & device : device_library_) {
                    if (device.product_id == product_id && device.device_type == emsdevice->device_type()) {
                        emsdevice->name(std::move(read_flash_string(device.name)));
                        emsdevice->add_flags(device.flags);
                    }
                }

                return true; // finish up
            }
        }
    }

    // look up the rest of the details using the product_id and create the new device object
    Device_record * device_p = nullptr;
    for (auto & device : device_library_) {
        if (device.product_id == product_id) {
            // sometimes boilers share the same productID as controllers
            // so only add boilers if the device_id is 0x08, which is fixed for EMS
            if (device.device_type == DeviceType::BOILER) {
                if (device_id == EMSdevice::EMS_DEVICE_ID_BOILER
                    || (device_id >= EMSdevice::EMS_DEVICE_ID_BOILER_1 && device_id <= EMSdevice::EMS_DEVICE_ID_BOILER_F)) {
                    device_p = &device;
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
        LOG_NOTICE(F("Unrecognized EMS device (deviceID 0x%02X, productID %d). Please report on GitHub."), device_id, product_id);
        std::string name("unknown");
        emsdevices.push_back(
            EMSFactory::add(DeviceType::GENERIC, device_id, product_id, version, name, DeviceFlags::EMS_DEVICE_FLAG_NONE, EMSdevice::Brand::NO_BRAND));
        return false; // not found
    }

    auto name        = read_flash_string(device_p->name);
    auto device_type = device_p->device_type;
    auto flags       = device_p->flags;

    // empty reply to version, read a generic device from database
    if (product_id == 0) {
        // check for known device IDs
        if (device_id == 0x40) {
            // see: https://github.com/emsesp/EMS-ESP32/issues/103#issuecomment-911717342
            name = "rf room temperature sensor"; // generic
        } else if (device_id == 0x17) {
            name        = "generic thermostat";
            device_type = DeviceType::THERMOSTAT;
            flags       = DeviceFlags::EMS_DEVICE_FLAG_RC10 | DeviceFlags::EMS_DEVICE_FLAG_NO_WRITE;
        } else if (device_id == 0x04) {
            name        = "RS232";
            device_type = DeviceType::CONNECT;
        } else if (device_id == 0x0A) {
            name        = "terminal";
            device_type = DeviceType::CONNECT;
        } else if (device_id == 0x0B) {
            name        = "service key";
            device_type = DeviceType::CONNECT;
        } else if (device_id == 0x0C) {
            name        = "cascade";
            device_type = DeviceType::CONNECT;
        } else if (device_id == 0x0D) {
            // see https://github.com/emsesp/EMS-ESP/issues/460#issuecomment-709553012
            name        = "modem";
            device_type = DeviceType::CONNECT;
        } else if (device_id == 0x0E) {
            name = "converter"; // generic
        } else if (device_id == 0x0F) {
            name = "clock"; // generic
        } else if (device_id == 0x08) {
            name        = "generic boiler";
            device_type = DeviceType::BOILER;
            flags       = DeviceFlags::EMS_DEVICE_FLAG_HEATPUMP;
            LOG_WARNING(F("Unknown EMS boiler. Using generic profile. Please report on GitHub."));
        } else {
            LOG_WARNING(F("Unrecognized EMS device (device ID 0x%02X, no product ID). Please report on GitHub."), device_id);
            return false;
        }
    }

    LOG_DEBUG(F("Adding new device %s (deviceID 0x%02X, productID %d, version %s)"), name.c_str(), device_id, product_id, version);
    emsdevices.push_back(EMSFactory::add(device_type, device_id, product_id, version, name, flags, brand));

    // assign a unique ID. Note that this is not actual unique after a restart as it's dependent on the order that devices are found
    emsdevices.back()->unique_id(++unique_id_count_);

    // sort devices based on type
    std::sort(emsdevices.begin(), emsdevices.end(), [](const std::unique_ptr<emsesp::EMSdevice> & a, const std::unique_ptr<emsesp::EMSdevice> & b) {
        return a->device_type() < b->device_type();
    });

    fetch_device_values(device_id); // go and fetch its data

    // add command commands for all devices, except for connect, controller and gateway
    if ((device_type == DeviceType::CONNECT) || (device_type == DeviceType::CONTROLLER) || (device_type == DeviceType::GATEWAY)) {
        return true;
    }

    Command::add(
        device_type,
        F_(info),
        [device_type](const char * value, const int8_t id, JsonObject & output) {
            return command_info(device_type, output, id, EMSdevice::OUTPUT_TARGET::API_VERBOSE);
        },
        F_(info_cmd));
    Command::add(
        device_type,
        F("values"),
        [device_type](const char * value, const int8_t id, JsonObject & output) {
            return command_info(device_type, output, id, EMSdevice::OUTPUT_TARGET::API_SHORTNAMES); // HIDDEN command showing short names, used in e.g. /api/boiler
        },
        nullptr,
        CommandFlag::HIDDEN); // this command is hidden
    Command::add(
        device_type,
        F_(commands),
        [device_type](const char * value, const int8_t id, JsonObject & output) { return command_commands(device_type, output, id); },
        F_(commands_cmd));
    Command::add(
        device_type,
        F_(entities),
        [device_type](const char * value, const int8_t id, JsonObject & output) { return command_entities(device_type, output, id); },
        F_(entities_cmd));

    // MQTT subscribe to the device e.g. "ems-esp/boiler/#"
    Mqtt::subscribe(device_type, EMSdevice::device_type_2_device_name(device_type) + "/#", nullptr);

    // Print to LOG showing we've added a new device
    LOG_INFO(F("Recognized new %s with deviceID 0x%02X"), EMSdevice::device_type_2_device_name(device_type).c_str(), device_id);

    return true;
}

// list device entities
bool EMSESP::command_entities(uint8_t device_type, JsonObject & output, const int8_t id) {
    JsonObject node;

    for (const auto & emsdevice : emsdevices) {
        if ((emsdevice) && (emsdevice->device_type() == device_type)) {
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

// export all values for a specific device
bool EMSESP::command_info(uint8_t device_type, JsonObject & output, const int8_t id, const uint8_t output_target) {
    bool    has_value = false;
    uint8_t tag;
    if (id >= 1 && id <= 8) {
        tag = DeviceValueTAG::TAG_HC1 + id - 1;
    } else if (id >= 9 && id <= 12) {
        tag = DeviceValueTAG::TAG_WWC1 + id - 9;
    } else if (id == -1 || id == 0) {
        tag = DeviceValueTAG::TAG_NONE;
    } else {
        return false;
    }

    for (const auto & emsdevice : emsdevices) {
        if (emsdevice && (emsdevice->device_type() == device_type)
            && ((device_type != DeviceType::THERMOSTAT) || (emsdevice->device_id() == EMSESP::actual_master_thermostat()))) {
            has_value |= emsdevice->generate_values(output, tag, (id < 1), output_target); // use nested for id -1 and 0
        }
    }

    return has_value;
}

// send a read request, passing it into to the Tx Service, with optional offset and length
void EMSESP::send_read_request(const uint16_t type_id, const uint8_t dest, const uint8_t offset, const uint8_t length) {
    txservice_.read_request(type_id, dest, offset, length);
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
        Roomctrl::check((data[1] ^ 0x80 ^ rxservice_.ems_mask()), data);
#ifdef EMSESP_UART_DEBUG
        // get_uptime is only updated once per loop, does not give the right time
        LOG_TRACE(F("[UART_DEBUG] Echo after %d ms: %s"), ::millis() - rx_time_, Helpers::data_to_hex(data, length).c_str());
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
                LOG_DEBUG(F("Last Tx write successful"));
                txservice_.increment_telegram_write_count(); // last tx/write was confirmed ok
                txservice_.send_poll();                      // close the bus
                publish_id_ = txservice_.post_send_query();  // follow up with any post-read if set
                txservice_.reset_retry_count();
                tx_successful = true;
            } else if (first_value == TxService::TX_WRITE_FAIL) {
                LOG_ERROR(F("Last Tx write rejected by host"));
                txservice_.send_poll(); // close the bus
                txservice_.reset_retry_count();
            }
        } else if (tx_state == Telegram::Operation::TX_READ) {
            // got a telegram with data in it. See if the src/dest matches that from the last one we sent and continue to process it
            uint8_t src  = data[0];
            uint8_t dest = data[1];
            if (txservice_.is_last_tx(src, dest)) {
                LOG_DEBUG(F("Last Tx read successful"));
                txservice_.increment_telegram_read_count();
                txservice_.send_poll(); // close the bus
                txservice_.reset_retry_count();
                tx_successful = true;
                // if telegram is longer read next part with offset +25 for ems+ or +27 for ems1.0
                if (length == 32) {
                    if (txservice_.read_next_tx(data[3]) == read_id_) {
                        read_next_ = true;
                    }
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
            LOG_TRACE(F("[UART_DEBUG] next Poll %s after %d ms"), Helpers::hextoa(s, first_value), ::millis() - rx_time_);
            // time measurement starts here, use millis because get_uptime is only updated once per loop
            rx_time_ = ::millis();
        } else {
            LOG_TRACE(F("[UART_DEBUG] Poll ack %s after %d ms"), Helpers::hextoa(s, first_value), ::millis() - rx_time_);
        }
#endif
        // check for poll to us, if so send top message from Tx queue immediately and quit
        if (poll_id == txservice_.ems_bus_id()) {
            txservice_.send();
        }
        // send remote room temperature if active
        Roomctrl::send(poll_id);
        return;
    } else {
#ifdef EMSESP_UART_DEBUG
        LOG_TRACE(F("[UART_DEBUG] Reply after %d ms: %s"), ::millis() - rx_time_, Helpers::data_to_hex(data, length).c_str());
#endif
        Roomctrl::check((data[1] ^ 0x80 ^ rxservice_.ems_mask()), data); // check if there is a message for the roomcontroller

        rxservice_.add(data, length); // add to RxQueue
    }
}

// sends raw data of bytes along the Tx line
void EMSESP::send_raw_telegram(const char * data) {
    txservice_.send_raw(data);
}

// start all the core services
// the services must be loaded in the correct order
void EMSESP::start() {
    Serial.begin(115200);

// start the file system
#ifndef EMSESP_STANDALONE
    if (!LITTLEFS.begin(true)) {
        Serial.println("LITTLEFS Mount Failed. EMS-ESP stopped.");
        return;
    }
#endif

    esp8266React.begin();                     // loads core system services settings (network, mqtt, ap, ntp etc)
    system_.check_upgrade();                  // do any system upgrades
    webSettingsService.begin();               // load EMS-ESP Application settings...
    system_.get_settings();                   // ...and store some of the settings locally for future reference
    console_.start(system_.telnet_enabled()); // telnet and serial console, from here we can start logging events
    webLogService.start();                    // start web log service
    webCustomizationService.begin();          // load the customizations

    // welcome message
    LOG_INFO(F("Starting EMS-ESP version %s (hostname: %s)"), EMSESP_APP_VERSION, system_.hostname().c_str());
    LOG_INFO(F("Configuring for interface board profile %s"), system_.board_profile().c_str());

    // start all the EMS-ESP services
    mqtt_.start();             // mqtt init
    system_.start(heap_start); // starts commands, led, adc, button, network, syslog & uart
    shower_.start();           // initialize shower timer and shower alert
    dallassensor_.start();     // Dallas external sensors
    analogsensor_.start();     // Analog external sensors
    webServer.begin();         // start the web server
    // emsdevices.reserve(5); // reserve space for initially 5 devices to avoid mem frag issues

    LOG_INFO(F("Last system reset reason Core0: %s, Core1: %s"), system_.reset_reason(0).c_str(), system_.reset_reason(1).c_str());

    // Load our library of known devices into stack mem. Names are stored in Flash memory (takes up about 1kb)
    device_library_ = {
#include "device_library.h"
    };
    LOG_INFO(F("EMS device library loaded with %d records"), device_library_.size());

#if defined(EMSESP_STANDALONE)
    Mqtt::on_connect(); // simulate an MQTT connection
#endif
}

// main loop calling all services
void EMSESP::loop() {
    esp8266React.loop(); // web services
    system_.loop();      // does LED and checks system health, and syslog service

    // if we're doing an OTA upload, skip MQTT and EMS
    if (!system_.upload_status()) {
        webLogService.loop(); // log in Web UI
        rxservice_.loop();    // process any incoming Rx telegrams
        shower_.loop();       // check for shower on/off
        dallassensor_.loop(); // read dallas sensor temperatures
        analogsensor_.loop(); // read analog sensor values
        publish_all_loop();   // with HA messages in parts to avoid flooding the mqtt queue
        mqtt_.loop();         // sends out anything in the MQTT queue

        // force a query on the EMS devices to fetch latest data at a set interval (1 min)
        if ((uuid::get_uptime() - last_fetch_ > EMS_FETCH_FREQUENCY)) {
            last_fetch_ = uuid::get_uptime();
            fetch_device_values();
        }
    }

    console_.loop(); // telnet/serial console

    // https://github.com/emsesp/EMS-ESP32/issues/78#issuecomment-877599145
    // delay(1); // helps telnet catch up. don't think its needed in ESP32 >3.1.0?
}

} // namespace emsesp
