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

#include "switch.h"

namespace emsesp {

REGISTER_FACTORY(Switch, EMSdevice::DeviceType::SWITCH);

uuid::log::Logger Switch::logger_ {
    F_(switch), uuid::log::Facility::CONSOLE
};

Switch::Switch(uint8_t device_type, uint8_t device_id, uint8_t product_id, const std::string & version, const std::string & name, uint8_t flags, uint8_t brand)
    : EMSdevice(device_type, device_id, product_id, version, name, flags, brand) {
    LOG_DEBUG(F("Adding new Switch with device ID 0x%02X"), device_id);

    register_telegram_type(0x9C, F("WM10MonitorMessage"), false, MAKE_PF_CB(process_WM10MonitorMessage));
    register_telegram_type(0x9D, F("WM10SetMessage"), false, MAKE_PF_CB(process_WM10SetMessage));
    register_telegram_type(0x1E, F("WM10TempMessage"), false, MAKE_PF_CB(process_WM10TempMessage));

    register_device_value(TAG_NONE, &id_, DeviceValueType::UINT, nullptr, FL_(ID), DeviceValueUOM::NONE);
    register_device_value(TAG_NONE, &activated_, DeviceValueType::BOOL, nullptr, FL_(activated), DeviceValueUOM::BOOLEAN);
    register_device_value(TAG_NONE, &flowTempHc_, DeviceValueType::USHORT, FL_(div10), FL_(flowTempHc), DeviceValueUOM::DEGREES);
    register_device_value(TAG_NONE, &status_, DeviceValueType::INT, nullptr, FL_(status), DeviceValueUOM::NUM);
    id_ = product_id;
}

// publish HA config
bool Switch::publish_ha_config() {
    // if we don't have valid values don't add it ever again
    if (!Helpers::hasValue(flowTempHc_)) {
        return false;
    }

    StaticJsonDocument<EMSESP_JSON_SIZE_HA_CONFIG> doc;
    doc["uniq_id"] = F_(switch);
    doc["ic"]      = F_(icondevice);

    char stat_t[Mqtt::MQTT_TOPIC_MAX_SIZE];
    snprintf(stat_t, sizeof(stat_t), "%s/%s", Mqtt::base().c_str(), Mqtt::tag_to_topic(device_type(), DeviceValueTAG::TAG_NONE).c_str());
    doc["stat_t"] = stat_t;

    char name_s[40];
    snprintf(name_s, sizeof(name_s), FSTR_(productid_fmt), device_type_name().c_str());
    doc["name"] = name_s;

    doc["val_tpl"] = FJSON("{{value_json.id}}");
    JsonObject dev = doc.createNestedObject("dev");
    dev["name"]    = FJSON("EMS-ESP Switch");
    dev["sw"]      = EMSESP_APP_VERSION;
    dev["mf"]      = brand_to_string();
    dev["mdl"]     = this->name();
    JsonArray ids  = dev.createNestedArray("ids");
    ids.add("ems-esp-switch");

    char topic[Mqtt::MQTT_TOPIC_MAX_SIZE];
    snprintf(topic, sizeof(topic), "sensor/%s/switch/config", Mqtt::base().c_str());
    Mqtt::publish_ha(topic, doc.as<JsonObject>()); // publish the config payload with retain flag

    return true;
}

// message 0x9D switch on/off
// Thermostat(0x10) -> Switch(0x11), ?(0x9D), data: 00
void Switch::process_WM10SetMessage(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram->read_value(activated_, 0));
}

// message 0x9C holds flowtemp and unknown status value
// Switch(0x11) -> All(0x00), ?(0x9C), data: 01 BA 00 01 00
void Switch::process_WM10MonitorMessage(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram->read_value(flowTempHc_, 0)); // is * 10
    has_update(telegram->read_value(status_, 2));
    // has_update(telegram->read_value(status2_, 3)); // unknown
}

// message 0x1E flow temperature, same as in 9C, published often, republished also by boiler UBAFast 0x18
// Switch(0x11) -> Boiler(0x08), ?(0x1E), data: 01 BA
void Switch::process_WM10TempMessage(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram->read_value(flowTempHc_, 0)); // is * 10
}

} // namespace emsesp