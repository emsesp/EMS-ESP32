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
}

void Switch::device_info_web(JsonArray & root) {
    // fetch the values into a JSON document
    StaticJsonDocument<EMSESP_MAX_JSON_SIZE_SMALL> doc;
    JsonObject                                     json = doc.to<JsonObject>();
    if (export_values(json)) {
        print_value_json(root, F("activated"), nullptr, F_(activated), nullptr, json);
        print_value_json(root, F("flowTemp"), nullptr, F_(flowTemp), F_(degrees), json);
        print_value_json(root, F("status"), nullptr, F_(status), nullptr, json);
    }
}

// display all values into the shell console
void Switch::show_values(uuid::console::Shell & shell) {
    // EMSdevice::show_values(shell); // always call this to show header
    // fetch the values into a JSON document
    StaticJsonDocument<EMSESP_MAX_JSON_SIZE_SMALL> doc;
    JsonObject                                     json = doc.to<JsonObject>();
    if (export_values(json)) {
        print_value_json(shell, F("activated"), nullptr, F_(activated), nullptr, json);
        print_value_json(shell, F("flowTemp"), F_(2spaces), F_(flowTemp), F_(degrees), json);
        print_value_json(shell, F("status"), nullptr, F_(status), nullptr, json);
    }
}

// publish values via MQTT
void Switch::publish_values(JsonObject & json, bool force) {
    if (Mqtt::mqtt_format() == Mqtt::Format::HA) {
        if (!mqtt_ha_config_ || force) {
            register_mqtt_ha_config();
            return;
        }
    }
    StaticJsonDocument<EMSESP_MAX_JSON_SIZE_SMALL> doc;
    JsonObject                                     json_data = doc.to<JsonObject>();
    if (export_values(json_data)) {
        Mqtt::publish(F("switch_data"), doc.as<JsonObject>());
    }
}

// export values to JSON
bool Switch::export_values(JsonObject & json) {

    if (Helpers::hasValue(flowTemp_)) {
        char s[7];
        json["activated"] = Helpers::render_value(s, activated_, EMS_VALUE_BOOL);
    }

    if (Helpers::hasValue(flowTemp_)) {
        json["flowTemp"] = (float)flowTemp_ / 10;
    }

    if (Helpers::hasValue(flowTemp_)) {
        json["status"] = status_;
    }

    return true;
}

// check to see if values have been updated
bool Switch::updated_values() {
    if (changed_) {
        changed_ = false;
        return true;
    }
    return false;
}

// publish config topic for HA MQTT Discovery
void Switch::register_mqtt_ha_config() {
    if (!Mqtt::connected()) {
        return;
    }

    // if we don't have valid values for this HC don't add it ever again
    if (!Helpers::hasValue(flowTemp_)) {
        return;
    }

    // Create the Master device
    StaticJsonDocument<EMSESP_MAX_JSON_SIZE_HA_CONFIG> doc;

    char name[10];
    snprintf_P(name, sizeof(name), PSTR("Switch"));
    doc["name"] = name;

    char uniq_id[10];
    snprintf_P(uniq_id, sizeof(uniq_id), PSTR("switch"));
    doc["uniq_id"] = uniq_id;

    doc["ic"] = F("mdi:home-thermometer-outline");

    char stat_t[50];
    snprintf_P(stat_t, sizeof(stat_t), PSTR("%s/switch_data"), System::hostname().c_str());
    doc["stat_t"] = stat_t;

    doc["val_tpl"] = F("{{value_json.type}}"); // HA needs a single value. We take the type which is wwc or hc

    JsonObject dev = doc.createNestedObject("dev");
    dev["name"]    = F("EMS-ESP Switch");
    dev["sw"]      = EMSESP_APP_VERSION;
    dev["mf"]      = this->brand_to_string();
    dev["mdl"]     = this->name();
    JsonArray ids  = dev.createNestedArray("ids");
    ids.add("ems-esp-switch");

    Mqtt::publish_retain(F("homeassistant/sensor/ems-esp/switch/config"), doc.as<JsonObject>(), true); // publish the config payload with retain flag
    Mqtt::register_mqtt_ha_sensor(PSTR("switch"), nullptr, F_(flowTemp), this->device_type(), "activated", nullptr, nullptr);
    Mqtt::register_mqtt_ha_sensor(PSTR("switch"), nullptr, F_(flowTemp), this->device_type(), "flowTemp", F_(degrees), F_(icontemperature));
    Mqtt::register_mqtt_ha_sensor(PSTR("switch"), nullptr, F_(flowTemp), this->device_type(), "status", nullptr, nullptr);

    mqtt_ha_config_ = true; // done
}

// message 0x9B switch on/off
void Switch::process_WM10SetMessage(std::shared_ptr<const Telegram> telegram){
    changed_ |= telegram->read_value(activated_, 0);
}

// message 0x9C holds flowtemp and unknown statusvalue
void Switch::process_WM10MonitorMessage(std::shared_ptr<const Telegram> telegram){
    changed_ |= telegram->read_value(flowTemp_, 0); // is * 10
    changed_ |= telegram->read_value(status_, 2);
}

} // namespace emsesp