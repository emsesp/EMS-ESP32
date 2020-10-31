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

#include "heatpump.h"

namespace emsesp {

REGISTER_FACTORY(Heatpump, EMSdevice::DeviceType::HEATPUMP);

uuid::log::Logger Heatpump::logger_{F_(heatpump), uuid::log::Facility::CONSOLE};

Heatpump::Heatpump(uint8_t device_type, uint8_t device_id, uint8_t product_id, const std::string & version, const std::string & name, uint8_t flags, uint8_t brand)
    : EMSdevice(device_type, device_id, product_id, version, name, flags, brand) {
    LOG_DEBUG(F("Adding new Heat Pump module with device ID 0x%02X"), device_id);

    // telegram handlers
    register_telegram_type(0x042B, F("HP1"), true, [&](std::shared_ptr<const Telegram> t) { process_HPMonitor1(t); });
    register_telegram_type(0x047B, F("HP2"), true, [&](std::shared_ptr<const Telegram> t) { process_HPMonitor2(t); });
}

// creates JSON doc from values
// returns false if empty
bool Heatpump::export_values(JsonObject & json) {
    if (Helpers::hasValue(airHumidity_)) {
        json["airHumidity"] = (float)airHumidity_ / 2;
    }

    if (Helpers::hasValue(dewTemperature_)) {
        json["dewTemperature"] = dewTemperature_;
    }

    return json.size();
}

void Heatpump::device_info_web(JsonArray & root) {
    // fetch the values into a JSON document
    StaticJsonDocument<EMSESP_MAX_JSON_SIZE_MEDIUM> doc;
    JsonObject                                      json = doc.to<JsonObject>();
    if (!export_values(json)) {
        return; // empty
    }

    print_value_json(root, F("airHumidity"), nullptr, F_(airHumidity), F_(percent), json);
    print_value_json(root, F("dewTemperature"), nullptr, F_(dewTemperature), F_(degrees), json);
}

// display all values into the shell console
void Heatpump::show_values(uuid::console::Shell & shell) {
    EMSdevice::show_values(shell); // always call this to show header

    // fetch the values into a JSON document
    StaticJsonDocument<EMSESP_MAX_JSON_SIZE_MEDIUM> doc;
    JsonObject                                      json = doc.to<JsonObject>();
    if (!export_values(json)) {
        return; // empty
    }

    print_value_json(shell, F("airHumidity"), nullptr, F_(airHumidity), F_(percent), json);
    print_value_json(shell, F("dewTemperature"), nullptr, F_(dewTemperature), F_(degrees), json);
}

// publish values via MQTT
void Heatpump::publish_values(JsonObject & json, bool force) {
    // handle HA first
    if (Mqtt::mqtt_format() == Mqtt::Format::HA) {
        register_mqtt_ha_config(force);
    }

    StaticJsonDocument<EMSESP_MAX_JSON_SIZE_MEDIUM> doc;
    JsonObject                                      json_data = doc.to<JsonObject>();
    if (export_values(json_data)) {
        Mqtt::publish(F("heatpump_data"), doc.as<JsonObject>());
    }
}

void Heatpump::register_mqtt_ha_config(bool force) {
    if ((mqtt_ha_config_ && !force)) {
        return;
    }

    if (!Mqtt::connected()) {
        return;
    }

    // Create the Master device
    StaticJsonDocument<EMSESP_MAX_JSON_SIZE_MEDIUM> doc;
    doc["name"]    = F_(EMSESP);
    doc["uniq_id"] = F_(heatpump);
    doc["ic"]      = F_(iconheatpump);

    char stat_t[50];
    snprintf_P(stat_t, sizeof(stat_t), PSTR("%s/heatpump_data"), System::hostname().c_str());
    doc["stat_t"] = stat_t;

    doc["val_tpl"] = F("{{value_json.airHumidity}}");

    JsonObject dev = doc.createNestedObject("dev");
    dev["name"]    = F("EMS-ESP Heat Pump");
    dev["sw"]      = EMSESP_APP_VERSION;
    dev["mf"]      = this->brand_to_string();
    dev["mdl"]     = this->name();
    JsonArray ids  = dev.createNestedArray("ids");
    ids.add("ems-esp-heatpump");
    Mqtt::publish_retain(F("homeassistant/sensor/ems-esp/heatpump/config"), doc.as<JsonObject>(), true); // publish the config payload with retain flag

    Mqtt::register_mqtt_ha_sensor(nullptr, nullptr, F_(airHumidity), this->device_type(), "airHumidity", F_(percent), nullptr);
    Mqtt::register_mqtt_ha_sensor(nullptr, nullptr, F_(dewTemperature), this->device_type(), "dewTemperature", F_(degrees), nullptr);

    mqtt_ha_config_ = true; // done
}

// check to see if values have been updated
bool Heatpump::updated_values() {
    if (changed_) {
        changed_ = false;
        return true;
    }
    return false;
}

/*
 * Type 0x47B - HeatPump Monitor 2
 * e.g. "38 10 FF 00 03 7B 08 24 00 4B"
 */
void Heatpump::process_HPMonitor2(std::shared_ptr<const Telegram> telegram) {
    changed_ |= telegram->read_value(dewTemperature_, 0);
    changed_ |= telegram->read_value(airHumidity_, 1);
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

/*
 * Type 0x42B- HeatPump Monitor 1
 * e.g. "38 10 FF 00 03 2B 00 D1 08 2A 01"
 */
void Heatpump::process_HPMonitor1(std::shared_ptr<const Telegram> telegram) {
    // still to implement
}

#pragma GCC diagnostic pop

} // namespace emsesp