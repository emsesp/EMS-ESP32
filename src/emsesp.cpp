/*
 * EMS-ESP - https://github.com/proddy/EMS-ESP
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
uint32_t           heap_start = ESP.getFreeHeap(); // get initial available heap memory
#endif

AsyncWebServer webServer(80);

#if defined(EMSESP_STANDALONE)
FS                 dummyFS;
ESP8266React       EMSESP::esp8266React(&webServer, &dummyFS);
WebSettingsService EMSESP::webSettingsService = WebSettingsService(&webServer, &dummyFS, EMSESP::esp8266React.getSecurityManager());
#else
ESP8266React       EMSESP::esp8266React(&webServer, &LITTLEFS);
WebSettingsService EMSESP::webSettingsService = WebSettingsService(&webServer, &LITTLEFS, EMSESP::esp8266React.getSecurityManager());
#endif

WebStatusService  EMSESP::webStatusService  = WebStatusService(&webServer, EMSESP::esp8266React.getSecurityManager());
WebDevicesService EMSESP::webDevicesService = WebDevicesService(&webServer, EMSESP::esp8266React.getSecurityManager());
WebAPIService     EMSESP::webAPIService     = WebAPIService(&webServer);

using DeviceFlags = EMSdevice;
using DeviceType  = EMSdevice::DeviceType;

std::vector<std::unique_ptr<EMSdevice>> EMSESP::emsdevices;      // array of all the detected EMS devices
std::vector<EMSESP::Device_record>      EMSESP::device_library_; // library of all our known EMS devices, in heap

uuid::log::Logger EMSESP::logger_{F_(emsesp), uuid::log::Facility::KERN};

// The services
RxService    EMSESP::rxservice_;    // incoming Telegram Rx handler
TxService    EMSESP::txservice_;    // outgoing Telegram Tx handler
Mqtt         EMSESP::mqtt_;         // mqtt handler
System       EMSESP::system_;       // core system services
Console      EMSESP::console_;      // telnet and serial console
DallasSensor EMSESP::dallassensor_; // Dallas sensors
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
uint64_t EMSESP::tx_delay_                 = 0;

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

// clears list of recognized devices
void EMSESP::clear_all_devices() {
    // temporary removed: clearing the list causes a crash, the associated commands and mqtt should also be removed.
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

// scans for new devices
void EMSESP::scan_devices() {
    EMSESP::clear_all_devices();
    EMSESP::send_read_request(EMSdevice::EMS_TYPE_UBADevices, EMSdevice::EMS_DEVICE_ID_BOILER);
}

/**
* if thermostat master is 0x18 it handles only ww and hc1, hc2..hc4 handled by devices 0x19..0x1B
* we send to right device and match all reads to 0x18
*/
uint8_t EMSESP::check_master_device(const uint8_t device_id, const uint16_t type_id, const bool read) {
    if (actual_master_thermostat_ == 0x18) {
        uint16_t mon_ids[4]    = {0x02A5, 0x02A6, 0x02A7, 0x02A8};
        uint16_t set_ids[4]    = {0x02B9, 0x02BA, 0x02BB, 0x02BC};
        uint16_t summer_ids[4] = {0x02AF, 0x02B0, 0x02B1, 0x02B2};
        uint16_t curve_ids[4]  = {0x029B, 0x029C, 0x029D, 0x029E};
        uint16_t master_ids[]  = {0x02F5, 0x031B, 0x031D, 0x031E, 0x023A, 0x0267, 0x0240};
        // look for heating circuits
        for (uint8_t i = 0; i < 4; i++) {
            if (type_id == mon_ids[i] || type_id == set_ids[i] || type_id == summer_ids[i] || type_id == curve_ids[i]) {
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

// to watch both type IDs and device IDs
void EMSESP::watch_id(uint16_t watch_id) {
    watch_id_ = watch_id;
}

// change the tx_mode
// resets all counters and bumps the UART
// this is called when the tx_mode is persisted in the FS either via Web UI or the console
void EMSESP::init_tx() {
    uint8_t tx_mode;
    EMSESP::webSettingsService.read([&](WebSettings & settings) {
        tx_mode   = settings.tx_mode;
        tx_delay_ = settings.tx_delay * 1000;

        EMSuart::stop();
        EMSuart::start(tx_mode, settings.rx_gpio, settings.tx_gpio);
    });

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

    // nothing sent successfully, also no errors - must be ok
    if ((total_sent == 0) && (txservice_.telegram_fail_count() == 0)) {
        return BUS_STATUS_CONNECTED;
    }

    // nothing sent successfully, but have Tx errors
    if ((total_sent == 0) && (txservice_.telegram_fail_count() != 0)) {
        return BUS_STATUS_TX_ERRORS;
    }

    // Tx Failure rate > 5%
    if (((txservice_.telegram_fail_count() * 100) / total_sent) > EMSbus::EMS_TX_ERROR_LIMIT) {
        return BUS_STATUS_TX_ERRORS;
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
        shell.printfln(F("  #telegrams received: %d"), rxservice_.telegram_count());
        shell.printfln(F("  #read requests sent: %d"), txservice_.telegram_read_count());
        shell.printfln(F("  #write requests sent: %d"), txservice_.telegram_write_count());
        shell.printfln(F("  #incomplete telegrams: %d"), rxservice_.telegram_error_count());
        shell.printfln(F("  #tx fails (after %d retries): %d"), TxService::MAXIMUM_TX_RETRIES, txservice_.telegram_fail_count());
        shell.printfln(F("  Rx line quality: %d%%"), rxservice_.quality());
        shell.printfln(F("  Tx line quality: %d%%"), txservice_.quality());
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
// this is the only time generate_values_json is called in verbose mode (set to true)
void EMSESP::show_device_values(uuid::console::Shell & shell) {
    if (emsdevices.empty()) {
        shell.printfln(F("No EMS devices detected. Try using 'scan devices' from the ems menu."));
        shell.println();
        return;
    }

    // do this in the order of factory classes to keep a consistent order when displaying
    for (const auto & device_class : EMSFactory::device_handlers()) {
        for (const auto & emsdevice : emsdevices) {
            if ((emsdevice) && (emsdevice->device_type() == device_class.first)) {
                // print header
                shell.printfln(F("%s: %s"), emsdevice->device_type_name().c_str(), emsdevice->to_string().c_str());

                DynamicJsonDocument doc(EMSESP_JSON_SIZE_XLARGE_DYN); // use max size
                JsonObject          json = doc.to<JsonObject>();
                emsdevice->generate_values_json(json, DeviceValueTAG::TAG_NONE, true, true); // console mode and nested

                // print line
                uint8_t id = 0;
                for (JsonPair p : json) {
                    const char * key = p.key().c_str();
                    shell.printf("  %s: ", key);
                    JsonVariant data = p.value();
                    shell.print(COLOR_BRIGHT_GREEN);
                    if (data.is<char *>()) {
                        shell.print(data.as<char *>());
                    } else if (data.is<int>()) {
                        shell.print(data.as<int>());
                    } else if (data.is<float>()) {
                        char s[10];
                        shell.print(Helpers::render_value(s, (float)data.as<float>(), 1));
                    } else if (data.is<bool>()) {
                        char s[10];
                        // shell.print(Helpers::render_boolean(s, data.as<bool>()));
                        shell.print(data.as<bool>() ? F_(on) : F_(off));
                    }

                    // if there is a uom print it
                    std::string uom = emsdevice->get_value_uom(key);
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

// show Dallas temperature sensors
void EMSESP::show_sensor_values(uuid::console::Shell & shell) {
    if (!have_sensors()) {
        return;
    }

    shell.printfln(F("Dallas temperature sensors:"));
    uint8_t i = 1;
    char    s[7];
    for (const auto & device : sensor_devices()) {
        shell.printfln(F("  Sensor %d, ID: %s, Temperature: %s °C"), i++, device.to_string().c_str(), Helpers::render_value(s, device.temperature_c, 10));
    }
    shell.println();
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
        publish_other_values();
        publish_sensor_values(true);
        system_.send_heartbeat();
    }
}

// on command "publish HA" loop and wait between devices for publishing all sensors
void EMSESP::publish_all_loop() {
    if (!Mqtt::connected() || !publish_all_idx_) {
        return;
    }
    // wait for free queue before sending next message, v3 queues HA-messages
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
        publish_other_values();
        break;
    case 6:
        publish_sensor_values(true, true);
        break;
    case 7:
        if (Mqtt::ha_enabled()) {
            Mqtt::ha_status();
        }
        system_.send_heartbeat();
        shower_.send_mqtt_stat(false, true);
        break;
    default:
        // all finished
        publish_all_idx_ = 0;
    }
}

// force HA to re-create all the devices
void EMSESP::reset_mqtt_ha() {
    if (!Mqtt::ha_enabled()) {
        return;
    }

    for (const auto & emsdevice : emsdevices) {
        emsdevice->ha_config_done(false);
    }
    dallassensor_.reload();
}

// create json doc for the devices values and add to MQTT publish queue
// generate_values_json is called without verbose mode (defaults to false)
void EMSESP::publish_device_values(uint8_t device_type) {
    DynamicJsonDocument doc(EMSESP_JSON_SIZE_XLARGE_DYN); // use max size
    JsonObject          json         = doc.to<JsonObject>();
    bool                need_publish = false;

    bool nested = Mqtt::nested_format();

    for (const auto & emsdevice : emsdevices) {
        if (emsdevice && (emsdevice->device_type() == device_type)) {
            // if we're using HA and it's not already done, send the config topics first. only do this once
            if (Mqtt::ha_enabled() && (!emsdevice->ha_config_done())) {
                emsdevice->publish_mqtt_ha_sensor(); // create the configs for each value as a sensor
            }

            // if its a boiler, generate json for each group and publish it directly
            if (device_type == DeviceType::BOILER) {
                emsdevice->generate_values_json(json, DeviceValueTAG::TAG_BOILER_DATA, false); // not nested
                Mqtt::publish("boiler_data", json);
                json.clear();
                emsdevice->generate_values_json(json, DeviceValueTAG::TAG_BOILER_DATA_WW, false); // not nested
                Mqtt::publish("boiler_data_ww", json);
                json.clear();
                emsdevice->generate_values_json(json, DeviceValueTAG::TAG_BOILER_DATA_INFO, false); // not nested
                Mqtt::publish("boiler_data_info", json);
                need_publish = false;
            }

            // Thermostat
            else if (device_type == DeviceType::THERMOSTAT) {
                // only publish the single master thermostat
                if (emsdevice->device_id() == EMSESP::actual_master_thermostat()) {
                    if (nested) {
                        need_publish |= emsdevice->generate_values_json(json, DeviceValueTAG::TAG_NONE, true); // nested
                    } else {
                        emsdevice->generate_values_json(json, DeviceValueTAG::TAG_NONE, false); // not nested
                        Mqtt::publish("thermostat_data", json);
                        json.clear();

                        for (uint8_t hc_tag = TAG_HC1; hc_tag <= DeviceValueTAG::TAG_HC4; hc_tag++) {
                            emsdevice->generate_values_json(json, hc_tag, false); // not nested
                            char topic[20];
                            snprintf_P(topic, sizeof(topic), PSTR("thermostat_data_%s"), EMSdevice::tag_to_string(hc_tag).c_str());
                            Mqtt::publish(topic, json);
                            json.clear();
                        }
                        need_publish = false;
                    }
                }
            }

            // Mixer
            else if (device_type == DeviceType::MIXER) {
                if (nested) {
                    need_publish |= emsdevice->generate_values_json(json, DeviceValueTAG::TAG_NONE, true); // nested
                } else {
                    for (uint8_t hc_tag = TAG_HC1; hc_tag <= DeviceValueTAG::TAG_WWC4; hc_tag++) {
                        emsdevice->generate_values_json(json, hc_tag, false); // not nested
                        char topic[20];
                        snprintf_P(topic, sizeof(topic), PSTR("mixer_data_%s"), EMSdevice::tag_to_string(hc_tag).c_str());
                        Mqtt::publish(topic, json);
                        json.clear();
                    }
                    need_publish = false;
                }

            } else {
                // for all other devices add the values to the json
                need_publish |= emsdevice->generate_values_json(json, DeviceValueTAG::TAG_NONE, true); // nested
            }
        }
    }

    // publish it under a single topic, only if we have data to publish
    if (need_publish) {
        char topic[20];
        snprintf_P(topic, sizeof(topic), PSTR("%s_data"), EMSdevice::device_type_2_device_name(device_type).c_str());
        Mqtt::publish(topic, json);
    }
}

// call the devices that don't need special attention
void EMSESP::publish_other_values() {
    publish_device_values(EMSdevice::DeviceType::SWITCH);
    publish_device_values(EMSdevice::DeviceType::HEATPUMP);
}

void EMSESP::publish_sensor_values(const bool time, const bool force) {
    if (dallassensor_.updated_values() || time || force) {
        dallassensor_.publish_values(force);
    }
}

// MQTT publish a telegram as raw data
void EMSESP::publish_response(std::shared_ptr<const Telegram> telegram) {
    if (!Mqtt::connected()) {
        return;
    }

    StaticJsonDocument<EMSESP_JSON_SIZE_SMALL> doc;

    char buffer[100];
    doc["src"]    = Helpers::hextoa(buffer, telegram->src);
    doc["dest"]   = Helpers::hextoa(buffer, telegram->dest);
    doc["type"]   = Helpers::hextoa(buffer, telegram->type_id);
    doc["offset"] = Helpers::hextoa(buffer, telegram->offset);
    strcpy(buffer, Helpers::data_to_hex(telegram->message_data, telegram->message_length).c_str());
    doc["data"] = buffer;

    if (telegram->message_length <= 4) {
        uint32_t value = 0;
        for (uint8_t i = 0; i < telegram->message_length; i++) {
            value = (value << 8) + telegram->message_data[i];
        }
        doc["value"] = value;
    }

    Mqtt::publish(F("response"), doc.as<JsonObject>());
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
    std::string src_name;
    std::string dest_name;
    std::string type_name;
    std::string direction;
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

    // check for global/common types like Version
    if (telegram->type_id == EMSdevice::EMS_TYPE_VERSION) {
        type_name = read_flash_string(F("Version"));
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

    std::string str(200, '\0');
    if (offset) {
        snprintf_P(&str[0],
                   str.capacity() + 1,
                   PSTR("%s(0x%02X) %s %s(0x%02X), %s(0x%02X), data: %s (offset %d)"),
                   src_name.c_str(),
                   src,
                   direction.c_str(),
                   dest_name.c_str(),
                   dest,
                   type_name.c_str(),
                   telegram->type_id,
                   telegram->to_string_message().c_str(),
                   offset);
    } else {
        snprintf_P(&str[0],
                   str.capacity() + 1,
                   PSTR("%s(0x%02X) %s %s(0x%02X), %s(0x%02X), data: %s"),
                   src_name.c_str(),
                   src,
                   direction.c_str(),
                   dest_name.c_str(),
                   dest,
                   type_name.c_str(),
                   telegram->type_id,
                   telegram->to_string_message().c_str());
    }

    return str;
}

/*
 * Type 0x07 - UBADevices - shows us the connected EMS devices
 * e.g. 08 00 07 00 0B 80 00 00 00 00 00 00 00 00 00 00 00
 * Junkers has 15 bytes of data
 * each byte is a bitmask for which devices are active
 * byte 1 = 0x08 - 0x0F, byte 2 = 0x10 - 0x17, etc...
 * e.g. in example above 1st byte = x0B = b1011 so we have device ids 0x08, 0x09, 0x011
 * and 2nd byte = x80 = b1000 b0000 = device id 0x17
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
                    // always skip modem device 0x0D, it does not reply to version request
                    // see https://github.com/proddy/EMS-ESP/issues/460#issuecomment-709553012
                    if ((device_id != EMSbus::ems_bus_id()) && !(EMSESP::device_exists(device_id)) && (device_id != 0x0D) && (device_id != 0x0C)) {
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
    uint8_t device_id  = telegram->src;                  // device ID
    uint8_t product_id = telegram->message_data[offset]; // product ID

    // get version as XX.XX
    std::string version(5, '\0');
    snprintf_P(&version[0], version.capacity() + 1, PSTR("%02d.%02d"), telegram->message_data[offset + 1], telegram->message_data[offset + 2]);

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

// find the device object that matches the device ID and see if it has a matching telegram type handler
// but only process if the telegram is sent to us or it's a broadcast (dest=0x00=all)
// We also check for common telgram types, like the Version(0x02)
// returns false if there are none found
bool EMSESP::process_telegram(std::shared_ptr<const Telegram> telegram) {
    // if watching or reading...
    if ((telegram->type_id == read_id_) && (telegram->dest == txservice_.ems_bus_id())) {
        LOG_NOTICE(pretty_telegram(telegram).c_str());
        publish_response(telegram);
        if (!read_next_) {
            read_id_ = WATCH_ID_NONE;
        }
        read_next_ = false;
    } else if (watch() == WATCH_ON) {
        if ((watch_id_ == WATCH_ID_NONE) || (telegram->type_id == watch_id_) || ((watch_id_ < 0x80) && ((telegram->src == watch_id_) || (telegram->dest == watch_id_)))) {
            LOG_NOTICE(pretty_telegram(telegram).c_str());
        } else if (!trace_raw_) {
            LOG_TRACE(pretty_telegram(telegram).c_str());
        }
    } else if (!trace_raw_) {
        LOG_TRACE(pretty_telegram(telegram).c_str());
    }

    // only process broadcast telegrams or ones sent to us on request
    if ((telegram->dest != 0x00) && (telegram->dest != rxservice_.ems_bus_id())) {
        return false;
    }

    // remember if we first get scan results from UBADevices
    static bool first_scan_done_ = false;
    // check for common types, like the Version(0x02)
    if (telegram->type_id == EMSdevice::EMS_TYPE_VERSION) {
        process_version(telegram);
        return true;
    } else if (telegram->type_id == EMSdevice::EMS_TYPE_UBADevices) {
        process_UBADevices(telegram);
        if (telegram->dest == EMSbus::ems_bus_id()) {
            first_scan_done_ = true;
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
            if (emsdevice->is_device_id(telegram->src)) {
                knowndevice = true;
                found       = emsdevice->handle_telegram(telegram);
                // if we correctly processes the telegram follow up with sending it via MQTT if needed
                if (found && Mqtt::connected()) {
                    if ((mqtt_.get_publish_onchange(emsdevice->device_type()) && emsdevice->has_update()) || (telegram->type_id == publish_id_ && telegram->dest == txservice_.ems_bus_id())) {
                        if (telegram->type_id == publish_id_) {
                            publish_id_ = 0;
                        }
                        emsdevice->has_update(false);                    // reset flag
                        publish_device_values(emsdevice->device_type()); // publish to MQTT if we explicitly have too
                    }
                }
                break;
            }
        }
    }

    if (!found) {
        LOG_DEBUG(F("No telegram type handler found for ID 0x%02X (src 0x%02X)"), telegram->type_id, telegram->src);
        if (watch() == WATCH_UNKNOWN) {
            LOG_NOTICE(pretty_telegram(telegram).c_str());
        }
        if (first_scan_done_ && !knowndevice && (telegram->src != EMSbus::ems_bus_id()) && (telegram->src != 0x0B) && (telegram->src != 0x0C) && (telegram->src != 0x0D)) {
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

    // for all device objects from emsdevice.h (UNKNOWN, SYSTEM, BOILER, THERMOSTAT, MIXER, SOLAR, HEATPUMP, GATEWAY, SWITCH, CONTROLLER, CONNECT)
    // so we keep a consistent order
    for (const auto & device_class : EMSFactory::device_handlers()) {
        // shell.printf(F("[factory ID: %d] "), device_class.first);
        for (const auto & emsdevice : emsdevices) {
            if ((emsdevice) && (emsdevice->device_type() == device_class.first)) {
                shell.printf(F("(%d) %s: %s"), emsdevice->unique_id(), emsdevice->device_type_name().c_str(), emsdevice->to_string().c_str());
                if ((emsdevice->device_type() == EMSdevice::DeviceType::THERMOSTAT) && (emsdevice->device_id() == actual_master_thermostat())) {
                    shell.printf(F(" ** master device **"));
                }
                shell.println();
                emsdevice->show_telegram_handlers(shell);

#if defined(EMSESP_DEBUG)
                emsdevice->show_mqtt_handlers(shell);
                shell.println();
                emsdevice->show_device_values_debug(shell);
#endif

                shell.println();
            }
        }
    }
}

// add a new or update existing EMS device to our list of active EMS devices
// if its not in our database, we don't add it
bool EMSESP::add_device(const uint8_t device_id, const uint8_t product_id, std::string & version, const uint8_t brand) {
    // don't add ourselves!
    if (device_id == rxservice_.ems_bus_id()) {
        return false;
    }

    // first check to see if we already have it, if so update the record
    for (const auto & emsdevice : emsdevices) {
        if (emsdevice) {
            if (emsdevice->is_device_id(device_id)) {
                LOG_DEBUG(F("Updating details for already active device ID 0x%02X"), device_id);
                emsdevice->product_id(product_id);
                emsdevice->version(version);
                // only set brand if it doesn't already exist
                if (emsdevice->brand() == EMSdevice::Brand::NO_BRAND) {
                    emsdevice->brand(brand);
                }
                // find the name and flags in our database
                for (const auto & device : device_library_) {
                    if (device.product_id == product_id) {
                        emsdevice->name(std::move(uuid::read_flash_string(device.name)));
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
            // sometimes boilers share the same product id as controllers
            // so only add boilers if the device_id is 0x08, which is fixed for EMS
            if (device.device_type == DeviceType::BOILER) {
                if (device_id == EMSdevice::EMS_DEVICE_ID_BOILER) {
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

    // if we don't recognize the product ID report it and add as a generic device
    if (device_p == nullptr) {
        LOG_NOTICE(F("Unrecognized EMS device (device ID 0x%02X, product ID %d). Please report on GitHub."), device_id, product_id);
        std::string name("unknown");
        emsdevices.push_back(EMSFactory::add(DeviceType::GENERIC, device_id, product_id, version, name, DeviceFlags::EMS_DEVICE_FLAG_NONE, EMSdevice::Brand::NO_BRAND));
        return false; // not found
    }

    auto name        = uuid::read_flash_string(device_p->name);
    auto device_type = device_p->device_type;
    auto flags       = device_p->flags;
    LOG_DEBUG(F("Adding new device %s (device ID 0x%02X, product ID %d, version %s)"), name.c_str(), device_id, product_id, version.c_str());
    emsdevices.push_back(EMSFactory::add(device_type, device_id, product_id, version, name, flags, brand));
    emsdevices.back()->unique_id(++unique_id_count_);

    fetch_device_values(device_id); // go and fetch its data

    // add info command, but not for all devices
    if ((device_type == DeviceType::CONNECT) || (device_type == DeviceType::CONTROLLER) || (device_type == DeviceType::GATEWAY)) {
        return true;
    }

    Command::add_with_json(device_type, F_(info), [device_type](const char * value, const int8_t id, JsonObject & json) { return command_info(device_type, json, id); });

    return true;
}

// export all values to info command
// value is ignored here
bool EMSESP::command_info(uint8_t device_type, JsonObject & json, const int8_t id) {
    bool    has_value = false;
    uint8_t tag;
    if (id >= 1 && id <= 4) {
        tag = DeviceValueTAG::TAG_HC1 + id - 1;
    } else if (id >= 9 && id <= 10) {
        tag = DeviceValueTAG::TAG_WWC1 + id - 9;
    } else {
        tag = DeviceValueTAG::TAG_NONE;
    }

    for (const auto & emsdevice : emsdevices) {
        if (emsdevice && (emsdevice->device_type() == device_type) && ((device_type != DeviceType::THERMOSTAT) || (emsdevice->device_id() == EMSESP::actual_master_thermostat()))) {
            has_value |= emsdevice->generate_values_json(json, tag, (id == -1), true); // console & nested
        }
    }

    return has_value;
}

// send a read request, passing it into to the Tx Service, with offset
void EMSESP::send_read_request(const uint16_t type_id, const uint8_t dest, const uint8_t offset) {
    txservice_.read_request(type_id, dest, offset);
}

// send a read request, passing it into to the Tx Service, with no offset
void EMSESP::send_read_request(const uint16_t type_id, const uint8_t dest) {
    txservice_.read_request(type_id, dest, 0); // 0 = no offset
}

// sends write request
void EMSESP::send_write_request(const uint16_t type_id, const uint8_t dest, const uint8_t offset, uint8_t * message_data, const uint8_t message_length, const uint16_t validate_typeid) {
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
                // if telegram is longer read next part with offset + 25 for ems+
                if (length == 32) {
                    if (txservice_.read_next_tx() == read_id_) {
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
        static uint64_t delayed_tx_start_ = 0;
        if (!rxservice_.bus_connected() && (tx_delay_ > 0)) {
            delayed_tx_start_ = uuid::get_uptime_ms();
            LOG_DEBUG(F("Tx delay started"));
        }
        if ((first_value ^ 0x80 ^ rxservice_.ems_mask()) == txservice_.ems_bus_id()) {
            EMSbus::last_bus_activity(uuid::get_uptime()); // set the flag indication the EMS bus is active
        }
        // first send delayed after connect
        if ((uuid::get_uptime_ms() - delayed_tx_start_) < tx_delay_) {
            return;
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
        // if ht3 poll must be ems_bus_id else if Buderus poll must be (ems_bus_id | 0x80)
        if ((first_value ^ 0x80 ^ rxservice_.ems_mask()) == txservice_.ems_bus_id()) {
            txservice_.send();
        }
        // send remote room temperature if active
        Roomctrl::send(first_value ^ 0x80 ^ rxservice_.ems_mask());
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
    Serial.println();

// start the file system
#ifndef EMSESP_STANDALONE
    if (!LITTLEFS.begin(true)) {
        Serial.println("LITTLEFS Mount Failed");
        return;
    }
#endif

    esp8266React.begin();       // loads system settings (wifi, mqtt, etc)
    webSettingsService.begin(); // load EMS-ESP specific settings

    system_.check_upgrade(); // do any upgrades

    // Load our library of known devices into stack mem. Names are stored in Flash memory (take about 960bytes)
    device_library_ = {
#include "device_library.h"
    };

    console_.start();          // telnet and serial console
    mqtt_.start();             // mqtt init
    system_.start(heap_start); // starts syslog, uart, sets version, initializes LED. Requires pre-loaded settings.
    shower_.start();           // initialize shower timer and shower alert
    dallassensor_.start();     // dallas external sensors
    webServer.begin();         // start web server

    emsdevices.reserve(5); // reserve space for initially 5 devices to avoid mem frag issues

    LOG_INFO(F("EMS Device library loaded with %d records"), device_library_.size());

#if defined(EMSESP_STANDALONE)
    Mqtt::on_connect(); // simulate an MQTT connection
#endif
}

// main loop calling all services
void EMSESP::loop() {
    esp8266React.loop(); // web

    // if we're doing an OTA upload, skip MQTT and EMS
    if (system_.upload_status()) {
        return;
    }

    system_.loop();       // does LED and checks system health, and syslog service
    rxservice_.loop();    // process any incoming Rx telegrams
    shower_.loop();       // check for shower on/off
    dallassensor_.loop(); // this will also send out via MQTT
    publish_all_loop();   // See which topics need publishing to MQTT and queue them
    mqtt_.loop();         // sends out anything in the MQTT queue
    console_.loop();      // telnet/serial console

    // force a query on the EMS devices to fetch latest data at a set interval (1 min)
    if ((uuid::get_uptime() - last_fetch_ > EMS_FETCH_FREQUENCY)) {
        last_fetch_ = uuid::get_uptime();
        fetch_device_values();
    }

    // delay(1); // helps telnet catch up. don't think its needed in ESP32 3.1.0
}

} // namespace emsesp
