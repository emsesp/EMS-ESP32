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
        if (device_id == 0x2A) {
            register_telegram_type(0x07D6, F("SM100wwTemperature"), false, [&](std::shared_ptr<const Telegram> t) { process_SM100wwTemperature(t); });
            register_telegram_type(0x07AA, F("SM100wwStatus"), false, [&](std::shared_ptr<const Telegram> t) { process_SM100wwStatus(t); });
            register_telegram_type(0x07AB, F("SM100wwCommand"), false, [&](std::shared_ptr<const Telegram> t) { process_SM100wwCommand(t); });
        } else {
            register_telegram_type(0x0362, F("SM100Monitor"), true, [&](std::shared_ptr<const Telegram> t) { process_SM100Monitor(t); });
            register_telegram_type(0x0363, F("SM100Monitor2"), true, [&](std::shared_ptr<const Telegram> t) { process_SM100Monitor2(t); });
            register_telegram_type(0x0366, F("SM100Config"), true, [&](std::shared_ptr<const Telegram> t) { process_SM100Config(t); });
            register_telegram_type(0x0364, F("SM100Status"), false, [&](std::shared_ptr<const Telegram> t) { process_SM100Status(t); });
            register_telegram_type(0x036A, F("SM100Status2"), false, [&](std::shared_ptr<const Telegram> t) { process_SM100Status2(t); });
            register_telegram_type(0x038E, F("SM100Energy"), true, [&](std::shared_ptr<const Telegram> t) { process_SM100Energy(t); });
        }
    }

    if (flags == EMSdevice::EMS_DEVICE_FLAG_ISM) {
        register_telegram_type(0x0103, F("ISM1StatusMessage"), true, [&](std::shared_ptr<const Telegram> t) { process_ISM1StatusMessage(t); });
        register_telegram_type(0x0101, F("ISM1Set"), false, [&](std::shared_ptr<const Telegram> t) { process_ISM1Set(t); });
    }
}

// print to web
void Solar::device_info_web(JsonArray & root) {
    // fetch the values into a JSON document
    StaticJsonDocument<EMSESP_MAX_JSON_SIZE_MEDIUM> doc;
    JsonObject                                      json = doc.to<JsonObject>();
    if (!export_values(json)) {
        return; // empty
    }

    print_value_json(root, F("collectorTemp"), nullptr, F_(collectorTemp), F_(degrees), json);
    print_value_json(root, F("tankBottomTemp"), nullptr, F_(tankBottomTemp), F_(degrees), json);
    print_value_json(root, F("tankBottomTemp2"), nullptr, F_(tankBottomTemp2), F_(degrees), json);
    print_value_json(root, F("heatExchangerTemp"), nullptr, F_(heatExchangerTemp), F_(degrees), json);
    print_value_json(root, F("solarPumpModulation"), nullptr, F_(solarPumpModulation), F_(percent), json);
    print_value_json(root, F("cylinderPumpModulation"), nullptr, F_(cylinderPumpModulation), F_(percent), json);
    print_value_json(root, F("valveStatus"), nullptr, F_(valveStatus), nullptr, json);
    print_value_json(root, F("solarPump"), nullptr, F_(solarPump), nullptr, json);
    print_value_json(root, F("tankHeated"), nullptr, F_(tankHeated), nullptr, json);
    print_value_json(root, F("collectorShutdown"), nullptr, F_(collectorShutdown), nullptr, json);
    print_value_json(root, F("energyLastHour"), nullptr, F_(energyLastHour), F_(wh), json);
    print_value_json(root, F("energyToday"), nullptr, F_(energyToday), F_(wh), json);
    print_value_json(root, F("energyTotal"), nullptr, F_(energyTotal), F_(kwh), json);

    if (Helpers::hasValue(pumpWorkMin_)) {
        JsonObject dataElement = root.createNestedObject();
        dataElement["n"]       = F_(pumpWorkMin);
        char time_str[60];
        snprintf_P(time_str, sizeof(time_str), PSTR("%d days %d hours %d minutes"), pumpWorkMin_ / 1440, (pumpWorkMin_ % 1440) / 60, pumpWorkMin_ % 60);
        dataElement["v"] = time_str;
    }
}

// display all values into the shell console
void Solar::show_values(uuid::console::Shell & shell) {
    EMSdevice::show_values(shell); // always call this to show header

    // fetch the values into a JSON document
    StaticJsonDocument<EMSESP_MAX_JSON_SIZE_MEDIUM> doc;
    JsonObject                                      json = doc.to<JsonObject>();
    if (!export_values(json)) {
        return; // empty
    }

    print_value_json(shell, F("collectorTemp"), nullptr, F_(collectorTemp), F_(degrees), json);
    print_value_json(shell, F("tankBottomTemp"), nullptr, F_(tankBottomTemp), F_(degrees), json);
    print_value_json(shell, F("tankBottomTemp2"), nullptr, F_(tankBottomTemp2), F_(degrees), json);
    print_value_json(shell, F("heatExchangerTemp"), nullptr, F_(heatExchangerTemp), F_(degrees), json);
    print_value_json(shell, F("solarPumpModulation"), nullptr, F_(solarPumpModulation), F_(percent), json);
    print_value_json(shell, F("cylinderPumpModulation"), nullptr, F_(cylinderPumpModulation), F_(percent), json);
    print_value_json(shell, F("valveStatus"), nullptr, F_(valveStatus), nullptr, json);
    print_value_json(shell, F("solarPump"), nullptr, F_(solarPump), nullptr, json);
    print_value_json(shell, F("tankHeated"), nullptr, F_(tankHeated), nullptr, json);
    print_value_json(shell, F("collectorShutdown"), nullptr, F_(collectorShutdown), nullptr, json);
    print_value_json(shell, F("energyLastHour"), nullptr, F_(energyLastHour), F_(wh), json);
    print_value_json(shell, F("energyToday"), nullptr, F_(energyToday), F_(wh), json);
    print_value_json(shell, F("energyTotal"), nullptr, F_(energyTotal), F_(kwh), json);

    if (Helpers::hasValue(pumpWorkMin_)) {
        shell.printfln(F("  %s: %d days %d hours %d minutes"),
                       uuid::read_flash_string(F_(pumpWorkMin)).c_str(),
                       pumpWorkMin_ / 1440,
                       (pumpWorkMin_ % 1440) / 60,
                       pumpWorkMin_ % 60);
    }
}

// publish values via MQTT
void Solar::publish_values(JsonObject & json, bool force) {
    // handle HA first
    if (Mqtt::mqtt_format() == Mqtt::Format::HA) {
        register_mqtt_ha_config(force);
    }

    StaticJsonDocument<EMSESP_MAX_JSON_SIZE_MEDIUM> doc;
    JsonObject                                      json_payload = doc.to<JsonObject>();
    if (export_values(json_payload)) {
        if (device_id() == 0x2A) {
            Mqtt::publish(F("ww_data"), doc.as<JsonObject>());
        } else {
            Mqtt::publish(F("solar_data"), doc.as<JsonObject>());
        }
    }
}

// publish config topic for HA MQTT Discovery
void Solar::register_mqtt_ha_config(bool force) {
    if ((mqtt_ha_config_ && !force)) {
        return;
    }

    if (!Mqtt::connected()) {
        return;
    }

    // Create the Master device
    StaticJsonDocument<EMSESP_MAX_JSON_SIZE_MEDIUM> doc;
    doc["name"]    = F_(EMSESP);
    doc["uniq_id"] = F_(solar);
    doc["ic"]      = F_(iconthermostat);

    char stat_t[50];
    snprintf_P(stat_t, sizeof(stat_t), PSTR("%s/solar_data"), System::hostname().c_str());
    doc["stat_t"] = stat_t;

    doc["val_tpl"] = F("{{value_json.solarPump}}");
    JsonObject dev = doc.createNestedObject("dev");
    dev["name"]    = F("EMS-ESP Solar");
    dev["sw"]      = EMSESP_APP_VERSION;
    dev["mf"]      = this->brand_to_string();
    dev["mdl"]     = this->name();
    JsonArray ids  = dev.createNestedArray("ids");
    ids.add("ems-esp-solar");
    Mqtt::publish_retain(F("homeassistant/sensor/ems-esp/solar/config"), doc.as<JsonObject>(), true); // publish the config payload with retain flag

    Mqtt::register_mqtt_ha_sensor(nullptr, nullptr, F_(collectorTemp), this->device_type(), "collectorTemp", F_(degrees), nullptr);
    Mqtt::register_mqtt_ha_sensor(nullptr, nullptr, F_(tankBottomTemp), this->device_type(), "tankBottomTemp", F_(degrees), nullptr);
    Mqtt::register_mqtt_ha_sensor(nullptr, nullptr, F_(tankBottomTemp2), this->device_type(), "tankBottomTemp2", F_(degrees), nullptr);
    Mqtt::register_mqtt_ha_sensor(nullptr, nullptr, F_(heatExchangerTemp), this->device_type(), "heatExchangerTemp", F_(degrees), nullptr);
    Mqtt::register_mqtt_ha_sensor(nullptr, nullptr, F_(solarPumpModulation), this->device_type(), "solarPumpModulation", F_(percent), nullptr);
    Mqtt::register_mqtt_ha_sensor(nullptr, nullptr, F_(cylinderPumpModulation), this->device_type(), "cylinderPumpModulation", F_(percent), nullptr);
    Mqtt::register_mqtt_ha_sensor(nullptr, nullptr, F_(pumpWorkMin), this->device_type(), "pumpWorkMin", nullptr, nullptr);
    Mqtt::register_mqtt_ha_sensor(nullptr, nullptr, F_(energyLastHour), this->device_type(), "energyLastHour", nullptr, nullptr);
    Mqtt::register_mqtt_ha_sensor(nullptr, nullptr, F_(energyToday), this->device_type(), "energyToday", nullptr, nullptr);
    Mqtt::register_mqtt_ha_sensor(nullptr, nullptr, F_(energyTotal), this->device_type(), "energyTotal", nullptr, nullptr);
    Mqtt::register_mqtt_ha_sensor(nullptr, nullptr, F_(solarPump), this->device_type(), "solarPump", nullptr, nullptr);
    Mqtt::register_mqtt_ha_sensor(nullptr, nullptr, F_(valveStatus), this->device_type(), "valveStatus", nullptr, nullptr);
    Mqtt::register_mqtt_ha_sensor(nullptr, nullptr, F_(tankHeated), this->device_type(), "tankHeated", nullptr, nullptr);
    Mqtt::register_mqtt_ha_sensor(nullptr, nullptr, F_(collectorShutdown), this->device_type(), "collectorShutdown", nullptr, nullptr);

    mqtt_ha_config_ = true; // done
}

// creates JSON doc from values
// returns false if empty
bool Solar::export_values(JsonObject & json) {
    char s[10]; // for formatting strings

    if (Helpers::hasValue(collectorTemp_)) {
        json["collectorTemp"] = (float)collectorTemp_ / 10;
    }

    if (Helpers::hasValue(tankBottomTemp_)) {
        json["tankBottomTemp"] = (float)tankBottomTemp_ / 10;
    }

    if (Helpers::hasValue(tankBottomTemp2_)) {
        json["tankBottomTemp2"] = (float)tankBottomTemp2_ / 10;
    }

    if (Helpers::hasValue(heatExchangerTemp_)) {
        json["heatExchangerTemp"] = (float)heatExchangerTemp_ / 10;
    }

    if (Helpers::hasValue(solarPumpModulation_)) {
        json["solarPumpModulation"] = solarPumpModulation_;
    }

    if (Helpers::hasValue(cylinderPumpModulation_)) {
        json["cylinderPumpModulation"] = cylinderPumpModulation_;
    }

    if (Helpers::hasValue(solarPump_, EMS_VALUE_BOOL)) {
        json["solarPump"] = Helpers::render_value(s, solarPump_, EMS_VALUE_BOOL);
    }

    if (Helpers::hasValue(valveStatus_, EMS_VALUE_BOOL)) {
        json["valveStatus"] = Helpers::render_value(s, valveStatus_, EMS_VALUE_BOOL);
    }

    if (Helpers::hasValue(pumpWorkMin_)) {
        json["pumpWorkMin"] = pumpWorkMin_;
    }

    if (Helpers::hasValue(tankHeated_, EMS_VALUE_BOOL)) {
        json["tankHeated"] = Helpers::render_value(s, tankHeated_, EMS_VALUE_BOOL);
    }

    if (Helpers::hasValue(collectorShutdown_, EMS_VALUE_BOOL)) {
        json["collectorShutdown"] = Helpers::render_value(s, collectorShutdown_, EMS_VALUE_BOOL);
    }

    if (Helpers::hasValue(energyLastHour_)) {
        json["energyLastHour"] = (float)energyLastHour_ / 10;
    }

    if (Helpers::hasValue(energyToday_)) {
        json["energyToday"] = energyToday_;
    }

    if (Helpers::hasValue(energyTotal_)) {
        json["energyTotal"] = (float)energyTotal_ / 10;
    }

    return json.size();
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
    changed_ |= telegram->read_value(pumpWorkMin_, 8, 3);
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

// SM100wwTemperatur - 0x07D6
// Solar Module(0x2A) -> (0x00), (0x7D6), data: 01 C1 00 00 02 5B 01 AF 01 AD 80 00 01 90
void Solar::process_SM100wwTemperature(std::shared_ptr<const Telegram> telegram) {
    // changed_ |= telegram->read_value(wwTemp_1_, 0);
    // changed_ |= telegram->read_value(wwTemp_3_, 4);
    // changed_ |= telegram->read_value(wwTemp_4_, 6);
    // changed_ |= telegram->read_value(wwTemp_5_, 8);
    // changed_ |= telegram->read_value(wwTemp_7_, 12);
}
// SM100wwStatus - 0x07AA
// Solar Module(0x2A) -> (0x00), (0x7AA), data: 64 00 04 00 03 00 28 01 0F
void Solar::process_SM100wwStatus(std::shared_ptr<const Telegram> telegram) {
    // changed_ |= telegram->read_value(wwPump_, 0);
}
// SM100wwCommand - 0x07AB
// Thermostat(0x10) -> Solar Module(0x2A), (0x7AB), data: 01 00 01
void Solar::process_SM100wwCommand(std::shared_ptr<const Telegram> telegram) {
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
