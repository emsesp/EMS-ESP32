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

#include "heatpump.h"

namespace emsesp {

REGISTER_FACTORY(Heatpump, EMSdevice::DeviceType::HEATPUMP);

uuid::log::Logger Heatpump::logger_{F_(heatpump), uuid::log::Facility::CONSOLE};

Heatpump::Heatpump(uint8_t device_type, uint8_t device_id, uint8_t product_id, const std::string & version, const std::string & name, uint8_t flags, uint8_t brand)
    : EMSdevice(device_type, device_id, product_id, version, name, flags, brand) {
    // telegram handlers
    register_telegram_type(0x042B, F("HP1"), true, MAKE_PF_CB(process_HPMonitor1));
    register_telegram_type(0x047B, F("HP2"), true, MAKE_PF_CB(process_HPMonitor2));

    // device values
    register_device_value(TAG_NONE, &id_, DeviceValueType::UINT, nullptr, FL_(ID), DeviceValueUOM::NONE);
    register_device_value(TAG_NONE, &airHumidity_, DeviceValueType::UINT, FL_(div2), FL_(airHumidity), DeviceValueUOM::PERCENT);
    register_device_value(TAG_NONE, &dewTemperature_, DeviceValueType::UINT, nullptr, FL_(dewTemperature), DeviceValueUOM::DEGREES);

    id_ = product_id;
}

// publish HA config
bool Heatpump::publish_ha_config() {
    StaticJsonDocument<EMSESP_JSON_SIZE_HA_CONFIG> doc;
    doc["uniq_id"] = F_(heatpump);
    doc["ic"]      = F_(icondevice);

    char stat_t[Mqtt::MQTT_TOPIC_MAX_SIZE];
    snprintf(stat_t, sizeof(stat_t), "%s/%s", Mqtt::base().c_str(), Mqtt::tag_to_topic(device_type(), DeviceValueTAG::TAG_NONE).c_str());
    doc["stat_t"] = stat_t;

    char name_s[40];
    snprintf(name_s, sizeof(name_s), FSTR_(productid_fmt), device_type_name().c_str());
    doc["name"] = name_s;

    doc["val_tpl"] = FJSON("{{value_json.id}}");
    JsonObject dev = doc.createNestedObject("dev");
    dev["name"]    = FJSON("EMS-ESP Heat Pump");
    dev["sw"]      = EMSESP_APP_VERSION;
    dev["mf"]      = brand_to_string();
    dev["mdl"]     = this->name();
    JsonArray ids  = dev.createNestedArray("ids");
    ids.add("ems-esp-heatpump");

    char topic[Mqtt::MQTT_TOPIC_MAX_SIZE];
    snprintf(topic, sizeof(topic), "sensor/%s/heatpump/config", Mqtt::base().c_str());
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