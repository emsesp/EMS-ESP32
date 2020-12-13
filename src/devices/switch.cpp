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

#include "switch.h"

namespace emsesp {

REGISTER_FACTORY(Switch, EMSdevice::DeviceType::SWITCH);

uuid::log::Logger Switch::logger_ {
    F_(switch), uuid::log::Facility::CONSOLE
};

Switch::Switch(uint8_t device_type, uint8_t device_id, uint8_t product_id, const std::string & version, const std::string & name, uint8_t flags, uint8_t brand)
    : EMSdevice(device_type, device_id, product_id, version, name, flags, brand) {
    LOG_DEBUG(F("Adding new Switch with device ID 0x%02X"), device_id);

    register_telegram_type(0x9C, F("WM10MonitorMessage"), false, [&](std::shared_ptr<const Telegram> t) { process_WM10MonitorMessage(t); });
    register_telegram_type(0x9B, F("WM10SetMessage"), false, [&](std::shared_ptr<const Telegram> t) { process_WM10SetMessage(t); });

    std::string empty("");
    register_device_value(empty, &activated_, DeviceValueType::BOOL, {}, F("activated"), F("Activated"), DeviceValueUOM::NONE);
    register_device_value(
        empty, &flowTemp_, DeviceValueType::USHORT, flash_string_vector{F("10")}, F("flowTemp"), F("Current flow temperature"), DeviceValueUOM::DEGREES);
    register_device_value(empty, &status_, DeviceValueType::INT, {}, F("status"), F("Status"), DeviceValueUOM::NONE);
}

// publish HA config
bool Switch::publish_ha_config() {
    // if we don't have valid values don't add it ever again
    if (!Helpers::hasValue(flowTemp_)) {
        return false;
    }

    StaticJsonDocument<EMSESP_JSON_SIZE_HA_CONFIG> doc;
    doc["uniq_id"] = F_(switch);

    char stat_t[50];
    snprintf_P(stat_t, sizeof(stat_t), PSTR("%s/switch_data"), System::hostname().c_str());
    doc["stat_t"] = stat_t;

    doc["name"]    = FJSON("Type");
    doc["val_tpl"] = FJSON("{{value_json.type}}"); // HA needs a single value. We take the type which is wwc or hc
    JsonObject dev = doc.createNestedObject("dev");
    dev["name"]    = FJSON("EMS-ESP Switch");
    dev["sw"]      = EMSESP_APP_VERSION;
    dev["mf"]      = brand_to_string();
    dev["mdl"]     = this->name();
    JsonArray ids  = dev.createNestedArray("ids");
    ids.add("ems-esp-switch");

    char topic[100];
    snprintf_P(topic, sizeof(topic), PSTR("homeassistant/sensor/%s/switch/config"), System::hostname().c_str());
    Mqtt::publish_ha(topic, doc.as<JsonObject>()); // publish the config payload with retain flag

    return true;
}

// message 0x9B switch on/off
void Switch::process_WM10SetMessage(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram->read_value(activated_, 0));
}

// message 0x9C holds flowtemp and unknown status value
void Switch::process_WM10MonitorMessage(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram->read_value(flowTemp_, 0)); // is * 10
    has_update(telegram->read_value(status_, 2));
}

} // namespace emsesp