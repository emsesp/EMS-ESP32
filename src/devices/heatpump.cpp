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

    // device values
    register_device_value(TAG_NONE, &id_, DeviceValueType::UINT, nullptr, F("id"), nullptr); // empty full name to prevent being shown in web or console
    register_device_value(TAG_NONE, &airHumidity_, DeviceValueType::UINT, FL_(div2), F("airHumidity"), F("relative air humidity"));
    register_device_value(TAG_NONE, &dewTemperature_, DeviceValueType::UINT, nullptr, F("dewTemperature"), F("dew point temperature"));

    id_ = product_id;
}

// publish HA config
bool Heatpump::publish_ha_config() {
    StaticJsonDocument<EMSESP_JSON_SIZE_HA_CONFIG> doc;
    doc["uniq_id"] = F_(heatpump);
    doc["ic"]      = F_(iconheatpump);

    char stat_t[Mqtt::MQTT_TOPIC_MAX_SIZE];
    snprintf_P(stat_t, sizeof(stat_t), PSTR("%s/heatpump_data"), Mqtt::base().c_str());
    doc["stat_t"] = stat_t;

    doc["name"]    = FJSON("ID");
    doc["val_tpl"] = FJSON("{{value_json.id}}");
    JsonObject dev = doc.createNestedObject("dev");
    dev["name"]    = FJSON("EMS-ESP Heat Pump");
    dev["sw"]      = EMSESP_APP_VERSION;
    dev["mf"]      = brand_to_string();
    dev["mdl"]     = this->name();
    JsonArray ids  = dev.createNestedArray("ids");
    ids.add("ems-esp-heatpump");

    char topic[Mqtt::MQTT_TOPIC_MAX_SIZE];
    snprintf_P(topic, sizeof(topic), PSTR("homeassistant/sensor/%s/heatpump/config"), Mqtt::base().c_str());
    Mqtt::publish_ha(topic, doc.as<JsonObject>()); // publish the config payload with retain flag

    return true;
}

/*
 * Type 0x47B - HeatPump Monitor 2
 * e.g. "38 10 FF 00 03 7B 08 24 00 4B"
 */
void Heatpump::process_HPMonitor2(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram->read_value(dewTemperature_, 0));
    has_update(telegram->read_value(airHumidity_, 1));
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