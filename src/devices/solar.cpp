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

namespace emsesp {

REGISTER_FACTORY(Solar, EMSdevice::DeviceType::SOLAR);

uuid::log::Logger Solar::logger_{F_(solar), uuid::log::Facility::CONSOLE};

Solar::Solar(uint8_t device_type, uint8_t device_id, uint8_t product_id, const std::string & version, const std::string & name, uint8_t flags, uint8_t brand)
    : EMSdevice(device_type, device_id, product_id, version, name, flags, brand) {
    LOG_DEBUG(F("Adding new Solar module with device ID 0x%02X"), device_id);

    // telegram handlers
    if (flags == EMSdevice::EMS_DEVICE_FLAG_SM10) {
        register_telegram_type(0x0097, F("SM10Monitor"), true, [&](std::shared_ptr<const Telegram> t) { process_SM10Monitor(t); });
    }

    if (flags == EMSdevice::EMS_DEVICE_FLAG_SM100) {
        register_telegram_type(0x0362, F("SM100Monitor"), true, [&](std::shared_ptr<const Telegram> t) { process_SM100Monitor(t); });
        register_telegram_type(0x0363, F("SM100Monitor2"), true, [&](std::shared_ptr<const Telegram> t) { process_SM100Monitor2(t); });
        register_telegram_type(0x0366, F("SM100Config"), true, [&](std::shared_ptr<const Telegram> t) { process_SM100Config(t); });

        register_telegram_type(0x0364, F("SM100Status"), false, [&](std::shared_ptr<const Telegram> t) { process_SM100Status(t); });
        register_telegram_type(0x036A, F("SM100Status2"), false, [&](std::shared_ptr<const Telegram> t) { process_SM100Status2(t); });
        register_telegram_type(0x038E, F("SM100Energy"), true, [&](std::shared_ptr<const Telegram> t) { process_SM100Energy(t); });
    }

    if (flags == EMSdevice::EMS_DEVICE_FLAG_ISM) {
        register_telegram_type(0x0103, F("ISM1StatusMessage"), true, [&](std::shared_ptr<const Telegram> t) { process_ISM1StatusMessage(t); });
        register_telegram_type(0x0101, F("ISM1Set"), false, [&](std::shared_ptr<const Telegram> t) { process_ISM1Set(t); });
    }

    // API call
    Command::add_with_json(this->device_type(), F("info"), [&](const char * value, const int8_t id, JsonObject & object) {
        return command_info(value, id, object);
    });
}

bool Solar::command_info(const char * value, const int8_t id, JsonObject & output) {
    return (export_values(output));
}

// print to web
void Solar::device_info_web(JsonArray & root) {
    render_value_json(root, "", F("Collector temperature (TS1)"), collectorTemp_, F_(degrees), 10);
    render_value_json(root, "", F("Tank bottom temperature (TS2)"), tankBottomTemp_, F_(degrees), 10);
    render_value_json(root, "", F("Tank bottom temperature (TS5)"), tankBottomTemp2_, F_(degrees), 10);
    render_value_json(root, "", F("Heat exchanger temperature (TS6)"), heatExchangerTemp_, F_(degrees), 10);
    render_value_json(root, "", F("Solar pump modulation (PS1)"), solarPumpModulation_, F_(percent));
    render_value_json(root, "", F("Cylinder pump modulation (PS5)"), cylinderPumpModulation_, F_(percent));
    render_value_json(root, "", F("Valve (VS2) status"), valveStatus_, nullptr, EMS_VALUE_BOOL);
    render_value_json(root, "", F("Solar Pump (PS1) active"), solarPump_, nullptr, EMS_VALUE_BOOL);

    if (Helpers::hasValue(pumpWorkMin_)) {
        JsonObject dataElement;
        dataElement         = root.createNestedObject();
        dataElement["name"] = F("Pump working time");
        std::string time_str(60, '\0');
        snprintf_P(&time_str[0], time_str.capacity() + 1, PSTR("%d days %d hours %d minutes"), pumpWorkMin_ / 1440, (pumpWorkMin_ % 1440) / 60, pumpWorkMin_ % 60);
        dataElement["value"] = time_str;
    }

    render_value_json(root, "", F("Tank Heated"), tankHeated_, nullptr, EMS_VALUE_BOOL);
    render_value_json(root, "", F("Collector shutdown"), collectorShutdown_, nullptr, EMS_VALUE_BOOL);

    render_value_json(root, "", F("Energy last hour"), energyLastHour_, F_(wh), 10);
    render_value_json(root, "", F("Energy today"), energyToday_, F_(wh));
    render_value_json(root, "", F("Energy total"), energyTotal_, F_(kwh), 10);
}

// display all values into the shell console
void Solar::show_values(uuid::console::Shell & shell) {
    EMSdevice::show_values(shell); // always call this to show header

    print_value(shell, 2, F("Collector temperature (TS1)"), collectorTemp_, F_(degrees), 10);
    print_value(shell, 2, F("Bottom temperature (TS2)"), tankBottomTemp_, F_(degrees), 10);
    print_value(shell, 2, F("Bottom temperature (TS5)"), tankBottomTemp2_, F_(degrees), 10);
    print_value(shell, 2, F("Heat exchanger temperature (TS6)"), heatExchangerTemp_, F_(degrees), 10);
    print_value(shell, 2, F("Solar pump modulation (PS1)"), solarPumpModulation_, F_(percent));
    print_value(shell, 2, F("Cylinder pump modulation (PS5)"), cylinderPumpModulation_, F_(percent));
    print_value(shell, 2, F("Valve (VS2) status"), valveStatus_, nullptr, EMS_VALUE_BOOL);
    print_value(shell, 2, F("Solar Pump (PS1) active"), solarPump_, nullptr, EMS_VALUE_BOOL);

    if (Helpers::hasValue(pumpWorkMin_)) {
        shell.printfln(F("  Pump working time: %d days %d hours %d minutes"), pumpWorkMin_ / 1440, (pumpWorkMin_ % 1440) / 60, pumpWorkMin_ % 60);
    }

    print_value(shell, 2, F("Tank Heated"), tankHeated_, nullptr, EMS_VALUE_BOOL);
    print_value(shell, 2, F("Collector shutdown"), collectorShutdown_, nullptr, EMS_VALUE_BOOL);

    print_value(shell, 2, F("Energy last hour"), energyLastHour_, F_(wh), 10);
    print_value(shell, 2, F("Energy today"), energyToday_, F_(wh));
    print_value(shell, 2, F("Energy total"), energyTotal_, F_(kwh), 10);
}

// publish values via MQTT
void Solar::publish_values() {
    StaticJsonDocument<EMSESP_MAX_JSON_SIZE_MEDIUM> doc;
    JsonObject                                      output = doc.to<JsonObject>();
    if (export_values(output)) {
        Mqtt::publish(F("solar_data"), doc.as<JsonObject>());
    }

    // if we're using Home Assistant and haven't created the MQTT Discovery topics, do it now
    if ((Mqtt::mqtt_format() == Mqtt::Format::HA) && (!ha_created_)) {
        register_mqtt_ha_config();
    }
}

// publish config topic for HA MQTT Discovery
void Solar::register_mqtt_ha_config() {
    Mqtt::register_mqtt_ha_sensor(F("Collector temperature (TS1)"), this->device_type(), "collectorTemp", "°C", "");
    Mqtt::register_mqtt_ha_sensor(F("Bottom temperature (TS2)"), this->device_type(), "tankBottomTemp", "°C", "");
    Mqtt::register_mqtt_ha_sensor(F("Bottom temperature (TS5)"), this->device_type(), "tankBottomTemp2", "°C", "");
    Mqtt::register_mqtt_ha_sensor(F("Heat exchanger temperature (TS6)"), this->device_type(), "heatExchangerTemp", "°C", "");
    Mqtt::register_mqtt_ha_sensor(F("Solar pump modulation (PS1)"), this->device_type(), "solarPumpModulation", "%", "");
    Mqtt::register_mqtt_ha_sensor(F("Cylinder pump modulation (PS5)"), this->device_type(), "cylinderPumpModulation", "%", "");
    Mqtt::register_mqtt_ha_sensor(F("Pump working time"), this->device_type(), "pumpWorkMin", "", "");
    Mqtt::register_mqtt_ha_sensor(F("Energy last hour"), this->device_type(), "energyLastHour", "", "");
    Mqtt::register_mqtt_ha_sensor(F("Energy today"), this->device_type(), "energyToday", "", "");
    Mqtt::register_mqtt_ha_sensor(F("Energy total"), this->device_type(), "energyTotal", "", "");
    Mqtt::register_mqtt_ha_sensor(F("Solar Pump (PS1) active"), this->device_type(), "solarPump", "", "");
    Mqtt::register_mqtt_ha_sensor(F("Valve (VS2) status"), this->device_type(), "valveStatus", "", "");
    Mqtt::register_mqtt_ha_sensor(F("Tank Heated"), this->device_type(), "tankHeated", "", "");
    Mqtt::register_mqtt_ha_sensor(F("Collector shutdown"), this->device_type(), "collectorShutdown", "", "");

    ha_created_ = true;
}

// creates JSON doc from values
// returns false if empty
bool Solar::export_values(JsonObject & output) {
    char s[10]; // for formatting strings

    if (Helpers::hasValue(collectorTemp_)) {
        output["collectorTemp"] = (float)collectorTemp_ / 10;
    }

    if (Helpers::hasValue(tankBottomTemp_)) {
        output["tankBottomTemp"] = (float)tankBottomTemp_ / 10;
    }

    if (Helpers::hasValue(tankBottomTemp2_)) {
        output["tankBottomTemp2"] = (float)tankBottomTemp2_ / 10;
    }

    if (Helpers::hasValue(heatExchangerTemp_)) {
        output["heatExchangerTemp"] = (float)heatExchangerTemp_ / 10;
    }

    if (Helpers::hasValue(solarPumpModulation_)) {
        output["solarPumpModulation"] = solarPumpModulation_;
    }

    if (Helpers::hasValue(cylinderPumpModulation_)) {
        output["cylinderPumpModulation"] = cylinderPumpModulation_;
    }

    if (Helpers::hasValue(solarPump_, EMS_VALUE_BOOL)) {
        output["solarPump"] = Helpers::render_value(s, solarPump_, EMS_VALUE_BOOL);
    }

    if (Helpers::hasValue(valveStatus_, EMS_VALUE_BOOL)) {
        output["valveStatus"] = Helpers::render_value(s, valveStatus_, EMS_VALUE_BOOL);
    }

    if (Helpers::hasValue(pumpWorkMin_)) {
        output["pumpWorkMin"] = pumpWorkMin_;
    }

    if (Helpers::hasValue(tankHeated_, EMS_VALUE_BOOL)) {
        output["tankHeated"] = Helpers::render_value(s, tankHeated_, EMS_VALUE_BOOL);
    }

    if (Helpers::hasValue(collectorShutdown_, EMS_VALUE_BOOL)) {
        output["collectorShutdown"] = Helpers::render_value(s, collectorShutdown_, EMS_VALUE_BOOL);
    }

    if (Helpers::hasValue(energyLastHour_)) {
        output["energyLastHour"] = (float)energyLastHour_ / 10;
    }

    if (Helpers::hasValue(energyToday_)) {
        output["energyToday"] = energyToday_;
    }

    if (Helpers::hasValue(energyTotal_)) {
        output["energyTotal"] = (float)energyTotal_ / 10;
    }

    return output.size();
}

// check to see if values have been updated
bool Solar::updated_values() {
    if (changed_) {
        changed_ = false;
        return true;
    }
    return false;
}

// SM10Monitor - type 0x97
void Solar::process_SM10Monitor(std::shared_ptr<const Telegram> telegram) {
    changed_ |= telegram->read_value(collectorTemp_, 2);       // collector temp from SM10, is *10
    changed_ |= telegram->read_value(tankBottomTemp_, 5);      // bottom temp from SM10, is *10
    changed_ |= telegram->read_value(solarPumpModulation_, 4); // modulation solar pump
    changed_ |= telegram->read_bitvalue(solarPump_, 7, 1);
    changed_ |= telegram->read_value(pumpWorkMin_, 8);
}

/*
 * SM100Monitor - type 0x0362 EMS+ - for MS/SM100 and MS/SM200
 * e.g. B0 0B FF 00 02 62 00 77 01 D4 80 00 80 00 80 00 80 00 80 00 80 00 80 00 80 00 00 F9 80 00 80 9E - for heat exchanger temp
 * e.g, 30 00 FF 00 02 62 01 AC
 *      30 00 FF 18 02 62 80 00
 *      30 00 FF 00 02 62 01 A1 - for bottom temps
 * bytes 0+1 = TS1 Temperature sensor for collector
 * bytes 2+3 = TS2 Temperature sensor 1 cylinder, bottom
 * bytes 16+17 = TS5 Temperature sensor 2 cylinder, bottom, or swimming pool
 * bytes 20+21 = TS6 Temperature sensor external heat exchanger
 */
void Solar::process_SM100Monitor(std::shared_ptr<const Telegram> telegram) {
    changed_ |= telegram->read_value(collectorTemp_, 0);      // is *10 - TS1: Temperature sensor for collector array 1
    changed_ |= telegram->read_value(tankBottomTemp_, 2);     // is *10 - TS2: Temperature sensor 1 cylinder, bottom
    changed_ |= telegram->read_value(tankBottomTemp2_, 16);   // is *10 - TS5: Temperature sensor 2 cylinder, bottom, or swimming pool
    changed_ |= telegram->read_value(heatExchangerTemp_, 20); // is *10 - TS6: Heat exchanger temperature sensor
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
    changed_ |= telegram->read_value(availabilityFlag_, 0);
    changed_ |= telegram->read_value(configFlag_, 1);
    changed_ |= telegram->read_value(userFlag_, 2);
}

/*
 * SM100Status - type 0x0364 EMS+ for pump modulations - for MS/SM100 and MS/SM200
 - PS1: Solar circuit pump for collector array 1
 - PS5: Cylinder primary pump when using an external heat exchanger
 * e.g. 30 00 FF 09 02 64 64 = 100%
 */
void Solar::process_SM100Status(std::shared_ptr<const Telegram> telegram) {
    uint8_t solarpumpmod    = solarPumpModulation_;
    uint8_t cylinderpumpmod = cylinderPumpModulation_;
    changed_ |= telegram->read_value(cylinderPumpModulation_, 8);
    changed_ |= telegram->read_value(solarPumpModulation_, 9);

    if (solarpumpmod == 0 && solarPumpModulation_ == 100) { // mask out boosts
        solarPumpModulation_ = 15;                          // set to minimum
    }

    if (cylinderpumpmod == 0 && cylinderPumpModulation_ == 100) { // mask out boosts
        cylinderPumpModulation_ = 15;                             // set to minimum
    }
    changed_ |= telegram->read_bitvalue(tankHeated_, 3, 1);        // issue #422
    changed_ |= telegram->read_bitvalue(collectorShutdown_, 3, 0); // collector shutdown
}

/*
 * SM100Status2 - type 0x036A EMS+ for pump on/off at offset 0x0A - for SM100 and SM200
 * e.g. B0 00 FF 00 02 6A 03 03 03 03 01 03 03 03 03 03 01 03
 * byte 4 = VS2 3-way valve for cylinder 2 : test=01, on=04 and off=03
 * byte 10 = PS1 Solar circuit pump for collector array 1: test=b0001(1), on=b0100(4) and off=b0011(3)
 */
void Solar::process_SM100Status2(std::shared_ptr<const Telegram> telegram) {
    changed_ |= telegram->read_bitvalue(valveStatus_, 4, 2); // on if bit 2 set
    changed_ |= telegram->read_bitvalue(solarPump_, 10, 2);  // on if bit 2 set
}

/*
 * SM100Energy - type 0x038E EMS+ for energy readings
 * e.g. 30 00 FF 00 02 8E 00 00 00 00 00 00 06 C5 00 00 76 35
 */
void Solar::process_SM100Energy(std::shared_ptr<const Telegram> telegram) {
    changed_ |= telegram->read_value(energyLastHour_, 0); // last hour / 10 in Wh
    changed_ |= telegram->read_value(energyToday_, 4);    // todays in Wh
    changed_ |= telegram->read_value(energyTotal_, 8);    // total / 10 in kWh
}

/*
 * Junkers ISM1 Solar Module - type 0x0103 EMS+ for energy readings
 *  e.g. B0 00 FF 00 00 03 32 00 00 00 00 13 00 D6 00 00 00 FB D0 F0
 */
void Solar::process_ISM1StatusMessage(std::shared_ptr<const Telegram> telegram) {
    changed_ |= telegram->read_value(collectorTemp_, 4);  // Collector Temperature
    changed_ |= telegram->read_value(tankBottomTemp_, 6); // Temperature Bottom of Solar Boiler
    uint16_t Wh = 0xFFFF;
    changed_ |= telegram->read_value(Wh, 2); // Solar Energy produced in last hour only ushort, is not * 10

    if (Wh != 0xFFFF) {
        energyLastHour_ = Wh * 10; // set to *10
    }

    changed_ |= telegram->read_bitvalue(solarPump_, 8, 0);         // PS1 Solar pump on (1) or off (0)
    changed_ |= telegram->read_value(pumpWorkMin_, 10, 3);         // force to 3 bytes
    changed_ |= telegram->read_bitvalue(collectorShutdown_, 9, 0); // collector shutdown on/off
    changed_ |= telegram->read_bitvalue(tankHeated_, 9, 2);        // tank full
}

/*
 * Junkers ISM1 Solar Module - type 0x0101 EMS+ for setting values
 */
void Solar::process_ISM1Set(std::shared_ptr<const Telegram> telegram) {
    changed_ |= telegram->read_value(setpoint_maxBottomTemp_, 6);
}

} // namespace emsesp
