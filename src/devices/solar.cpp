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

#include "solar.h"

MAKE_PSTR(kwh, "kWh")
MAKE_PSTR(wh, "Wh")

namespace emsesp {

REGISTER_FACTORY(Solar, EMSdevice::DeviceType::SOLAR);

MAKE_PSTR(logger_name, "solar")
uuid::log::Logger Solar::logger_{F_(logger_name), uuid::log::Facility::CONSOLE};

Solar::Solar(uint8_t device_type, uint8_t device_id, uint8_t product_id, const std::string & version, const std::string & name, uint8_t flags, uint8_t brand)
    : EMSdevice(device_type, device_id, product_id, version, name, flags, brand) {
    LOG_DEBUG(F("Registering new Solar module with device ID 0x%02X"), device_id);

    // telegram handlers
    if (flags == EMSdevice::EMS_DEVICE_FLAG_SM10) {
        register_telegram_type(0x0097, F("SM10Monitor"), true, std::bind(&Solar::process_SM10Monitor, this, _1));
    }
    if (flags == EMSdevice::EMS_DEVICE_FLAG_SM100) {
        register_telegram_type(0x0362, F("SM100Monitor"), true, std::bind(&Solar::process_SM100Monitor, this, _1));
        register_telegram_type(0x0363, F("SM100Monitor2"), true, std::bind(&Solar::process_SM100Monitor2, this, _1));
        register_telegram_type(0x0366, F("SM100Config"), true, std::bind(&Solar::process_SM100Config, this, _1));

        register_telegram_type(0x0364, F("SM100Status"), false, std::bind(&Solar::process_SM100Status, this, _1));
        register_telegram_type(0x036A, F("SM100Status2"), false, std::bind(&Solar::process_SM100Status2, this, _1));
        register_telegram_type(0x038E, F("SM100Energy"), true, std::bind(&Solar::process_SM100Energy, this, _1));
    }
    if (flags == EMSdevice::EMS_DEVICE_FLAG_ISM) {
        register_telegram_type(0x0103, F("ISM1StatusMessage"), true, std::bind(&Solar::process_ISM1StatusMessage, this, _1));
        register_telegram_type(0x0101, F("ISM1Set"), false, std::bind(&Solar::process_ISM1Set, this, _1));
    }

    // MQTT callbacks
    // register_mqtt_topic("cmd", std::bind(&Solar::cmd, this, _1));
}

// context submenu
void Solar::add_context_menu() {
}

// display all values into the shell console
void Solar::show_values(uuid::console::Shell & shell) {
    EMSdevice::show_values(shell); // always call this to show header

    print_value(shell, 2, F("Collector temperature (TS1)"), collectorTemp_, F_(degrees), 10);
    print_value(shell, 2, F("Bottom temperature (TS2)"), bottomTemp_, F_(degrees), 10);
    print_value(shell, 2, F("Bottom temperature (TS5)"), bottomTemp2_, F_(degrees), 10);
    print_value(shell, 2, F("Pump modulation"), pumpModulation_, F_(percent));
    print_value(shell, 2, F("Valve (VS2) status"), valveStatus_, nullptr, EMS_VALUE_BOOL);
    print_value(shell, 2, F("Pump (PS1) active"), pump_, nullptr, EMS_VALUE_BOOL);

    if (Helpers::hasValue(pumpWorkMin_)) {
        shell.printfln(F("  Pump working time: %d days %d hours %d minutes"), pumpWorkMin_ / 1440, (pumpWorkMin_ % 1440) / 60, pumpWorkMin_ % 60);
    }

    print_value(shell, 2, F("Tank Heated"), tankHeated_, nullptr, EMS_VALUE_BOOL);
    print_value(shell, 2, F("Collector"), collectorOnOff_, nullptr, EMS_VALUE_BOOL);

    print_value(shell, 2, F("Energy last hour"), energyLastHour_, F_(wh), 10);
    print_value(shell, 2, F("Energy today"), energyToday_, F_(wh));
    print_value(shell, 2, F("Energy total"), energyTotal_, F_(kwh), 10);
}

// publish values via MQTT
void Solar::publish_values() {
    DynamicJsonDocument doc(EMSESP_MAX_JSON_SIZE_MEDIUM);

    char s[10]; // for formatting strings

    if (Helpers::hasValue(collectorTemp_)) {
        doc["collectortemp"] = (float)collectorTemp_ / 10;
    }

    if (Helpers::hasValue(bottomTemp_)) {
        doc["bottomtemp"] = (float)bottomTemp_ / 10;
    }

    if (Helpers::hasValue(bottomTemp2_)) {
        doc["bottomtemp2"] = (float)bottomTemp2_ / 10;
    }

    if (Helpers::hasValue(pumpModulation_)) {
        doc["pumpmodulation"] = pumpModulation_;
    }

    if (Helpers::hasValue(pump_, true)) {
        doc["pump"] = Helpers::render_value(s, pump_, EMS_VALUE_BOOL);
    }

    if (Helpers::hasValue(valveStatus_, true)) {
        doc["valvestatus"] = Helpers::render_value(s, valveStatus_, EMS_VALUE_BOOL);
    }

    if (Helpers::hasValue(pumpWorkMin_)) {
        doc["pumpWorkMin"] = (float)pumpWorkMin_;
    }

    if (Helpers::hasValue(tankHeated_, true)) {
        doc["tankHeated"] = Helpers::render_value(s, tankHeated_, EMS_VALUE_BOOL);
    }

    if (Helpers::hasValue(collectorOnOff_, true)) {
        doc["collectorOnOff"] = Helpers::render_value(s, collectorOnOff_, EMS_VALUE_BOOL);
    }

    if (Helpers::hasValue(energyLastHour_)) {
        doc["energylasthour"] = (float)energyLastHour_ / 10;
    }

    if (Helpers::hasValue(energyToday_)) {
        doc["energytoday"] = energyToday_;
    }

    if (Helpers::hasValue(energyTotal_)) {
        doc["energytotal"] = (float)energyTotal_ / 10;
    }

    Mqtt::publish("sm_data", doc);
}

// check to see if values have been updated
bool Solar::updated_values() {
    return false;
}

// add console commands
void Solar::console_commands() {
}

// SM10Monitor - type 0x97
void Solar::process_SM10Monitor(std::shared_ptr<const Telegram> telegram) {
    telegram->read_value(collectorTemp_, 2);  // collector temp from SM10, is *10
    telegram->read_value(bottomTemp_, 5);     // bottom temp from SM10, is *10
    telegram->read_value(pumpModulation_, 4); // modulation solar pump
    telegram->read_bitvalue(pump_, 7, 1);
    telegram->read_value(pumpWorkMin_, 8);
}

/*
 * SM100Monitor - type 0x0362 EMS+ - for SM100 and SM200
 * e.g. B0 0B FF 00 02 62 00 44 02 7A 80 00 80 00 80 00 80 00 80 00 80 00 00 7C 80 00 80 00 80 00 80
 * e.g, 30 00 FF 00 02 62 01 AC
 *      30 00 FF 18 02 62 80 00
 *      30 00 FF 00 02 62 01 A1 - for bottom temps
 * bytes 0+1 = TS1 Temperature sensor for collector
 * bytes 2+3 = TS2 Temperature sensor bottom cylinder 1
 * bytes 16+17 = TS5 Temperature sensor bottom cylinder 2
 */
void Solar::process_SM100Monitor(std::shared_ptr<const Telegram> telegram) {
    telegram->read_value(collectorTemp_, 0); // is *10
    telegram->read_value(bottomTemp_, 2);    // is *10
    telegram->read_value(bottomTemp2_, 16);  // is *10
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

// SM100Monitor2 - 0x0363
// e.g. B0 00 FF 00 02 63 80 00 80 00 00 00 80 00 80 00 80 00 00 80 00 5A
void Solar::process_SM100Monitor2(std::shared_ptr<const Telegram> telegram) {
    // not implemented yet
}

#pragma GCC diagnostic pop

// SM100Config - 0x0366
// e.g. B0 00 FF 00 02 66     01 62 00 13 40 14
void Solar::process_SM100Config(std::shared_ptr<const Telegram> telegram) {
    telegram->read_value(availabilityFlag_, 0);
    telegram->read_value(configFlag_, 1);
    telegram->read_value(userFlag_, 2);
}

/*
 * SM100Status - type 0x0364 EMS+ for pump modulation - for SM100 and SM200
 * e.g. 30 00 FF 09 02 64 64 = 100%
 *      30 00 FF 09 02 64 1E = 30%
 */
void Solar::process_SM100Status(std::shared_ptr<const Telegram> telegram) {
    uint8_t pumpmod = pumpModulation_;
    telegram->read_value(pumpModulation_, 9);
    if (pumpmod == 0 && pumpModulation_ == 100) { // mask out boosts
        pumpModulation_ = 15;                     // set to minimum
    }

    telegram->read_bitvalue(tankHeated_, 3, 1); // issue #422
    telegram->read_bitvalue(collectorOnOff_, 3, 0);
}

/*
 * SM100Status2 - type 0x036A EMS+ for pump on/off at offset 0x0A - for SM100 and SM200
 * e.g. B0 00 FF 00 02 6A 03 03 03 03 01 03 03 03 03 03 01 03 
 * byte 4 = VS2 3-way valve for cylinder 2 : test=01, on=04 and off=03
 * byte 10 = PS1 Solar circuit pump for collector array 1: test=b0001(1), on=b0100(4) and off=b0011(3)
 */
void Solar::process_SM100Status2(std::shared_ptr<const Telegram> telegram) {
    telegram->read_bitvalue(valveStatus_, 4, 2); // on if bit 2 set
    telegram->read_bitvalue(pump_, 10, 2);       // on if bit 2 set
}

/*
 * SM100Energy - type 0x038E EMS+ for energy readings
 * e.g. 30 00 FF 00 02 8E 00 00 00 00 00 00 06 C5 00 00 76 35
 */
void Solar::process_SM100Energy(std::shared_ptr<const Telegram> telegram) {
    telegram->read_value(energyLastHour_, 0); // last hour / 10 in Wh
    telegram->read_value(energyToday_, 4);    //  todays in Wh
    telegram->read_value(energyTotal_, 8);    //  total / 10 in kWh
}

/*
 * Junkers ISM1 Solar Module - type 0x0103 EMS+ for energy readings
 *  e.g. B0 00 FF 00 00 03 32 00 00 00 00 13 00 D6 00 00 00 FB D0 F0
 */
void Solar::process_ISM1StatusMessage(std::shared_ptr<const Telegram> telegram) {
    telegram->read_value(collectorTemp_, 4); // Collector Temperature
    telegram->read_value(bottomTemp_, 6);    // Temperature Bottom of Solar Boiler
    uint16_t Wh = 0xFFFF;
    telegram->read_value(Wh, 2); // Solar Energy produced in last hour only ushort, is not * 10
    if (Wh != 0xFFFF) {
        energyLastHour_ = Wh * 10; // set to *10
    }
    telegram->read_bitvalue(collectorOnOff_, 9, 0); // collector on/off
    telegram->read_bitvalue(tankHeated_, 9, 2);     // tank full
}

/*
 * e.g. 90 30 FF 06 00 01 50
 */
void Solar::process_ISM1Set(std::shared_ptr<const Telegram> telegram) {
    telegram->read_value(setpoint_maxBottomTemp_, 6);
}

} // namespace emsesp
