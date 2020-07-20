/*
 * EMS-ESP - https://github.com/proddy/EMS-ESP
 * Copyright 2019  Paul Derbyshire
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

MAKE_PSTR(logger_name, "emsesp")

namespace emsesp {

using DeviceFlags = emsesp::EMSdevice;
using DeviceType  = emsesp::EMSdevice::DeviceType;

AsyncWebServer webServer(80);

#if defined(ESP32)
ESP8266React          EMSESP::esp8266React(&webServer, &SPIFFS);
EMSESPSettingsService EMSESP::emsespSettingsService = EMSESPSettingsService(&webServer, &SPIFFS, EMSESP::esp8266React.getSecurityManager());
#elif defined(ESP8266)
ESP8266React          EMSESP::esp8266React(&webServer, &LittleFS);
EMSESPSettingsService EMSESP::emsespSettingsService = EMSESPSettingsService(&webServer, &LittleFS, EMSESP::esp8266React.getSecurityManager());
#elif defined(EMSESP_STANDALONE)
FS                    dummyFS;
ESP8266React          EMSESP::esp8266React(&webServer, &dummyFS);
EMSESPSettingsService EMSESP::emsespSettingsService = EMSESPSettingsService(&webServer, &dummyFS, EMSESP::esp8266React.getSecurityManager());
#endif

EMSESPStatusService      EMSESP::emsespStatusService      = EMSESPStatusService(&webServer, EMSESP::esp8266React.getSecurityManager());
EMSESPDevicesService     EMSESP::emsespDevicesService     = EMSESPDevicesService(&webServer, EMSESP::esp8266React.getSecurityManager());
EMSESPScanDevicesService EMSESP::emsespScanDevicesService = EMSESPScanDevicesService(&webServer, EMSESP::esp8266React.getSecurityManager());

std::vector<std::unique_ptr<EMSdevice>>    EMSESP::emsdevices;      // array of all the detected EMS devices
std::vector<emsesp::EMSESP::Device_record> EMSESP::device_library_; // libary of all our known EMS devices so far

uuid::log::Logger EMSESP::logger_{FPSTR(__pstr__logger_name), uuid::log::Facility::KERN};

// The services
RxService EMSESP::rxservice_; // incoming Telegram Rx handler
TxService EMSESP::txservice_; // outgoing Telegram Tx handler
Mqtt      EMSESP::mqtt_;      // mqtt handler
System    EMSESP::system_;    // core system services
Console   EMSESP::console_;   // telnet and serial console
Sensors   EMSESP::sensors_;   // Dallas sensors
Shower    EMSESP::shower_;    // Shower logic

// static/common variables
uint8_t  EMSESP::actual_master_thermostat_ = EMSESP_DEFAULT_MASTER_THERMOSTAT; // which thermostat leads when multiple found
uint16_t EMSESP::watch_id_                 = WATCH_ID_NONE;                    // for when log is TRACE. 0 means no trace set
uint8_t  EMSESP::watch_                    = 0;                                // trace off
bool     EMSESP::tap_water_active_         = false;                            // for when Boiler states we having running warm water. used in Shower()
uint32_t EMSESP::last_fetch_               = 0;

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

void EMSESP::actual_master_thermostat(const uint8_t device_id) {
    actual_master_thermostat_ = device_id;
}

uint8_t EMSESP::actual_master_thermostat() {
    return actual_master_thermostat_;
}

// to watch both type IDs and device IDs
void EMSESP::watch_id(uint16_t watch_id) {
    // if it's a device ID, which is a single byte, remove the MSB so to support both Buderus and HT3 protocols
    if (watch_id <= 0xFF) {
        watch_id_ = (watch_id & 0x7F);
    } else {
        watch_id_ = watch_id;
    }
}

// change the tx_mode
// resets all counters and bumps the UART
void EMSESP::reset_tx(uint8_t const tx_mode) {
    txservice_.telegram_read_count(0);
    txservice_.telegram_write_count(0);
    txservice_.telegram_fail_count(0);
    if (tx_mode) {
        EMSuart::stop();
        EMSuart::start(tx_mode);
        EMSESP::fetch_device_values();
    }
}

// return status of bus: connected, connected but Tx is broken, disconnected
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
    if (rxservice_.bus_connected()) {
        uint8_t success_rate = 0;
        if (rxservice_.telegram_error_count()) {
            success_rate = ((float)rxservice_.telegram_error_count() / (float)rxservice_.telegram_count()) * 100;
        }

        shell.printfln(F("EMS Bus info:"));
        EMSESP::emsespSettingsService.read([&](EMSESPSettings & settings) { shell.printfln(F("  Tx mode: %d"), settings.tx_mode); });
        shell.printfln(F("  Bus protocol: %s"), EMSbus::is_ht3() ? F("HT3") : F("Buderus"));
        shell.printfln(F("  #telegrams received: %d"), rxservice_.telegram_count());
        shell.printfln(F("  #read requests sent: %d"), txservice_.telegram_read_count());
        shell.printfln(F("  #write requests sent: %d"), txservice_.telegram_write_count());
        shell.printfln(F("  #corrupted telegrams: %d (%d%%)"), rxservice_.telegram_error_count(), success_rate);
        shell.printfln(F("  #tx fails (after %d retries): %d"), TxService::MAXIMUM_TX_RETRIES, txservice_.telegram_fail_count());
    } else {
        shell.printfln(F("EMS Bus is disconnected."));
    }

    shell.println();

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
            shell.printfln(F(" [%02d%c] %s %s (offset %d)"),
                           it.id_,
                           ((it.retry_) ? '*' : ' '),
                           op.c_str(),
                           pretty_telegram(it.telegram_).c_str(),
                           it.telegram_->offset);
        }
    }

    shell.println();
}

// show EMS device values
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
                emsdevice->show_values(shell);
                shell.println();
            }
        }
    }
    shell.println();
}

// show Dallas sensors
void EMSESP::show_sensor_values(uuid::console::Shell & shell) {
    if (sensor_devices().empty()) {
        return;
    }

    char valuestr[8] = {0}; // for formatting temp
    shell.printfln(F("External temperature sensors:"));
    for (const auto & device : sensor_devices()) {
        shell.printfln(F("  Sensor ID %s: %s°C"), device.to_string().c_str(), Helpers::render_value(valuestr, device.temperature_c_, 2));
    }
    shell.println();
}

// publish all values from each EMS device to MQTT
// plus the heartbeat and sensor if activated
void EMSESP::publish_all_values() {
    if (Mqtt::connected()) {
        // Dallas sensors first
        sensors_.publish_values();

        // all the connected EMS devices we known about
        for (const auto & emsdevice : emsdevices) {
            if (emsdevice) {
                emsdevice->publish_values();
            }
        }
    }
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
// e.g. Boiler(0x08) -> Me(0x0B), Version(0x02), data: 7B 06 01 00 00 00 00 00 00 04 (#data=10)
std::string EMSESP::pretty_telegram(std::shared_ptr<const Telegram> telegram) {
    uint8_t src    = telegram->src & 0x7F;
    uint8_t dest   = telegram->dest & 0x7F;
    uint8_t offset = telegram->offset;

    // find name for src and dest by looking up known devices
    std::string src_name;
    std::string dest_name;
    std::string type_name;
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

    std::string str(200, '\0');

    if (offset) {
        snprintf_P(&str[0],
                   str.capacity() + 1,
                   PSTR("%s(0x%02X) -> %s(0x%02X), %s(0x%02X), data: %s @offset %d"),
                   src_name.c_str(),
                   src,
                   dest_name.c_str(),
                   dest,
                   type_name.c_str(),
                   telegram->type_id,
                   telegram->to_string().c_str(),
                   offset);
    } else {
        snprintf_P(&str[0],
                   str.capacity() + 1,
                   PSTR("%s(0x%02X) -> %s(0x%02X), %s(0x%02X), data: %s"),
                   src_name.c_str(),
                   src,
                   dest_name.c_str(),
                   dest,
                   type_name.c_str(),
                   telegram->type_id,
                   telegram->to_string().c_str());
    }

    return str;
}

/*
 * Type 0x07 - UBADevices - shows us the connected EMS devices
 * e.g. 08 00 07 00 0B 80 00 00 00 00 00 00 00 00 00 00 00
 * Junkers has 15 bytes of data
 * each byte is a bitmask for which devices are active
 * byte 1 = range 0x08 - 0x0F, byte 2=0x10 - 0x17 etc...
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
    // if watching...
    if (watch() == WATCH_ON) {
        if ((watch_id_ == WATCH_ID_NONE) || (telegram->src == watch_id_) || (telegram->dest == watch_id_) || (telegram->type_id == watch_id_)) {
            LOG_NOTICE(pretty_telegram(telegram).c_str());
        }
    }

    // only process broadcast telegrams or ones sent to us on request
    if ((telegram->dest != 0x00) && (telegram->dest != rxservice_.ems_bus_id())) {
        return false;
    }

    // check for common types, like the Version(0x02)
    if (telegram->type_id == EMSdevice::EMS_TYPE_VERSION) {
        process_version(telegram);
        return true;
    } else if (telegram->type_id == EMSdevice::EMS_TYPE_UBADevices) {
        process_UBADevices(telegram);
        return true;
    }

    // match device_id and type_id
    // calls the associated process function for that EMS device
    // returns false if the device_id doesn't recognize it
    // after the telegram has been processed, call the updated_values() function to see if we need to force an MQTT publish
    bool found = false;
    for (const auto & emsdevice : emsdevices) {
        if (emsdevice) {
            if (emsdevice->is_device_id(telegram->src)) {
                found = emsdevice->handle_telegram(telegram);
                // check to see if we need to follow up after the telegram has been processed
                if (found) {
                    if (emsdevice->updated_values()) {
                        emsdevice->publish_values(); // publish to MQTT if we explicitly have too
                    }
                }
                break;
            }
        }
    }

    if (!found) {
        LOG_DEBUG(F("No telegram type handler found for ID 0x%02X (src 0x%02X)"), telegram->type_id, telegram->src);
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

// for each device add its context menu for the console
void EMSESP::add_context_menus() {
    for (const auto & emsdevice : emsdevices) {
        if (emsdevice) {
            emsdevice->add_context_menu();
        }
    }
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

    // for all device objects from emsdevice.h (UNKNOWN, SERVICEKEY, BOILER, THERMOSTAT, MIXING, SOLAR, HEATPUMP, GATEWAY, SWITCH, CONTROLLER, CONNECT)
    // so we keep a consistent order
    for (const auto & device_class : EMSFactory::device_handlers()) {
        // shell.printf(F("[factory ID: %d] "), device_class.first);
        for (const auto & emsdevice : emsdevices) {
            if ((emsdevice) && (emsdevice->device_type() == device_class.first)) {
                shell.printf(F("%s: %s"), emsdevice->device_type_name().c_str(), emsdevice->to_string().c_str());
                if ((emsdevice->device_type() == EMSdevice::DeviceType::THERMOSTAT) && (emsdevice->device_id() == actual_master_thermostat())) {
                    shell.printf(F(" ** master device **"));
                }
                shell.println();
                emsdevice->show_telegram_handlers(shell);
                emsdevice->show_mqtt_handlers(shell);
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
                LOG_DEBUG(F("Updating details to already existing device ID 0x%02X"), device_id);
                emsdevice->product_id(product_id);
                emsdevice->version(version);
                // only set brand if it doesn't already exist
                if (emsdevice->brand() == EMSdevice::Brand::NO_BRAND) {
                    emsdevice->brand(brand);
                }
                // find the name and flags in our database
                for (const auto & device : device_library_) {
                    if (device.product_id == product_id) {
                        emsdevice->name(uuid::read_flash_string(device.name));
                        emsdevice->flags(device.flags);
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

    // if we don't recognize the product ID report it, but don't add it.
    if (device_p == nullptr) {
        LOG_NOTICE(F("Unrecognized EMS device with device ID 0x%02X with product ID %d. Please report on GitHub."), device_id, product_id);
        return false; // not found
    } else {
        emsdevices.push_back(
            EMSFactory::add(device_p->device_type, device_id, device_p->product_id, version, uuid::read_flash_string(device_p->name), device_p->flags, brand));
        LOG_DEBUG(F("Adding new device with device ID 0x%02X with product ID %d and version %s"), device_id, product_id, version.c_str());
        // go and fetch its data,
        fetch_device_values(device_id);
    }

    return true;
}

// send a read request, passing it into to the Tx Service, with no offset
void EMSESP::send_read_request(const uint16_t type_id, const uint8_t dest) {
    txservice_.read_request(type_id, dest, 0); // 0 = no offset
}

// sends write request
void EMSESP::send_write_request(const uint16_t type_id,
                                const uint8_t  dest,
                                const uint8_t  offset,
                                uint8_t *      message_data,
                                const uint8_t  message_length,
                                const uint16_t validate_typeid) {
    txservice_.add(Telegram::Operation::TX_WRITE, dest, type_id, offset, message_data, message_length);

    txservice_.set_post_send_query(validate_typeid); // store which type_id to send Tx read after a write
}

// this is main entry point when data is received on the Rx line, via emsuart library
// we check if its a complete telegram or just a single byte (which could be a poll or a return status)
// the CRC check is not done here, only when it's added to the Rx queue with add()
void EMSESP::incoming_telegram(uint8_t * data, const uint8_t length) {
#ifdef EMSESP_DEBUG
    static uint32_t rx_time_ = 0;
#endif
    // check first for echo
    uint8_t first_value = data[0];
    if (((first_value & 0x7F) == txservice_.ems_bus_id()) && (length > 1)) {
        // if we ask ourself at roomcontrol for version e.g. 0B 98 02 00 20
        Roomctrl::check((data[1] ^ 0x80 ^ rxservice_.ems_mask()), data);
#ifdef EMSESP_DEBUG
        // get_uptime is only updated once per loop, does not give the right time
        LOG_TRACE(F("[DEBUG] Echo after %d ms: %s"), ::millis() - rx_time_, Helpers::data_to_hex(data, length).c_str());
#endif
        return; // it's an echo
    }

    // are we waiting for a response from a recent Tx Read or Write?
    uint8_t op = EMSbus::tx_waiting();
    if (op != Telegram::Operation::NONE) {
        bool tx_successful = false;
        EMSbus::tx_waiting(Telegram::Operation::NONE); // reset Tx wait state
        // txservice_.print_last_tx();

        // if we're waiting on a Write operation, we want a single byte 1 or 4
        if ((op == Telegram::Operation::TX_WRITE) && (length == 1)) {
            if (first_value == TxService::TX_WRITE_SUCCESS) {
                LOG_DEBUG(F("Last Tx write successful"));
                txservice_.increment_telegram_write_count(); // last tx/write was confirmed ok
                txservice_.send_poll();                      // close the bus
                txservice_.post_send_query();                // follow up with any post-read
                txservice_.reset_retry_count();
                tx_successful = true;
            } else if (first_value == TxService::TX_WRITE_FAIL) {
                LOG_ERROR(F("Last Tx write rejected by host"));
                txservice_.send_poll(); // close the bus
                txservice_.reset_retry_count();
            }
        } else if (op == Telegram::Operation::TX_READ) {
            // got a telegram with data in it. See if the src/dest matches that from the last one we sent and continue to process it
            uint8_t src  = data[0];
            uint8_t dest = data[1];
            if (txservice_.is_last_tx(src, dest)) {
                LOG_DEBUG(F("Last Tx read successful"));
                txservice_.increment_telegram_read_count();
                txservice_.send_poll(); // close the bus
                txservice_.reset_retry_count();
                tx_successful = true;
            }
        }

        // if Tx wasn't successful, retry or just give up
        if (!tx_successful) {
            txservice_.retry_tx(op, data, length);
            return;
        }
    }

    // check for poll
    if (length == 1) {
#ifdef EMSESP_DEBUG
        char s[4];
        if (first_value & 0x80) {
            LOG_TRACE(F("[DEBUG] next Poll %s after %d ms"), Helpers::hextoa(s, first_value), ::millis() - rx_time_);
            // time measurement starts here, use millis because get_uptime is only updated once per loop
            rx_time_ = ::millis();
        } else {
            LOG_TRACE(F("[DEBUG] Poll ack %s after %d ms"), Helpers::hextoa(s, first_value), ::millis() - rx_time_);
        }
#endif
        // check for poll to us, if so send top message from Tx queue immediately and quit
        // if ht3 poll must be ems_bus_id else if Buderus poll must be (ems_bus_id | 0x80)
        if ((first_value ^ 0x80 ^ rxservice_.ems_mask()) == txservice_.ems_bus_id()) {
            EMSbus::last_bus_activity(uuid::get_uptime()); // set the flag indication the EMS bus is active
            txservice_.send();
        }
        // send remote room temperature if active
        Roomctrl::send(first_value ^ 0x80 ^ rxservice_.ems_mask());
        return;
    } else {
#ifdef EMSESP_DEBUG
        LOG_TRACE(F("[DEBUG] Reply after %d ms: %s"), ::millis() - rx_time_, Helpers::data_to_hex(data, length).c_str());
#endif
        // check if there is a message for the roomcontroller
        Roomctrl::check((data[1] ^ 0x80 ^ rxservice_.ems_mask()), data);
        // add to RxQueue, what ever it is.
        // in add() the CRC will be checked
        rxservice_.add(data, length);
    }
}

// sends raw data of bytes along the Tx line
void EMSESP::send_raw_telegram(const char * data) {
    txservice_.send_raw(data);
}

// kick off the party, start all the services
void EMSESP::start() {
    // Load our library of known devices
    device_library_ = {
#include "device_library.h"
    };

#ifdef ESP32
    SPIFFS.begin(true);
#elif defined(ESP8266)
    LittleFS.begin();
#endif

    esp8266React.begin();                              // starts wifi, ap, ota, security, mqtt services
    emsespSettingsService.begin();                     // load EMS-ESP specific settings from LittleFS
    console_.start();                                  // telnet and serial console
    system_.start();                                   // starts syslog, uart, sets version, initializes LED. Requires pre-loaded settings.
    mqtt_.start(EMSESP::esp8266React.getMqttClient()); // mqtt init
    sensors_.start();                                  // dallas external sensors
    shower_.start();                                   // initialize shower timer and shower alert
    txservice_.start();                                // sets bus ID, sends out request for EMS devices
    webServer.begin();                                 // start web server
}

// loop de loop
void EMSESP::loop() {
    esp8266React.loop(); // web
    system_.loop();      // does LED and checks system health, and syslog service
    mqtt_.loop();        // starts mqtt, and sends out anything in the queue
    rxservice_.loop();   // process what ever is in the rx queue
    txservice_.loop();   // check that the Tx is all ok
    shower_.loop();      // check for shower on/off
    sensors_.loop();     // this will also send out via MQTT
    console_.loop();     // telnet/serial console

    // force a query on the EMS devices to fetch latest data at a set interval (1 min)
    if ((uuid::get_uptime() - last_fetch_ > EMS_FETCH_FREQUENCY)) {
        last_fetch_ = uuid::get_uptime();
        fetch_device_values();
    }

#if defined(ESP32)
    delay(1);
#endif
}

} // namespace emsesp
