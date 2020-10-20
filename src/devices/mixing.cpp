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

#include "mixing.h"

namespace emsesp {

REGISTER_FACTORY(Mixing, EMSdevice::DeviceType::MIXING);

uuid::log::Logger Mixing::logger_{F_(mixing), uuid::log::Facility::CONSOLE};

Mixing::Mixing(uint8_t device_type, uint8_t device_id, uint8_t product_id, const std::string & version, const std::string & name, uint8_t flags, uint8_t brand)
    : EMSdevice(device_type, device_id, product_id, version, name, flags, brand) {
    LOG_DEBUG(F("Adding new Mixing module with device ID 0x%02X"), device_id);

    if (flags == EMSdevice::EMS_DEVICE_FLAG_MMPLUS) {
        if (device_id <= 0x27) {
            // telegram handlers 0x20 - 0x27 for HC
            register_telegram_type(device_id - 0x20 + 0x02D7, F("MMPLUSStatusMessage_HC"), true, [&](std::shared_ptr<const Telegram> t) {
                process_MMPLUSStatusMessage_HC(t);
            });
        } else {
            // telegram handlers for warm water/DHW 0x28, 0x29
            register_telegram_type(device_id - 0x28 + 0x0331, F("MMPLUSStatusMessage_WWC"), true, [&](std::shared_ptr<const Telegram> t) {
                process_MMPLUSStatusMessage_WWC(t);
            });
        }
    }

    // EMS 1.0
    if (flags == EMSdevice::EMS_DEVICE_FLAG_MM10) {
        register_telegram_type(0x00AA, F("MMConfigMessage"), false, [&](std::shared_ptr<const Telegram> t) { process_MMConfigMessage(t); });
        register_telegram_type(0x00AB, F("MMStatusMessage"), true, [&](std::shared_ptr<const Telegram> t) { process_MMStatusMessage(t); });
        register_telegram_type(0x00AC, F("MMSetMessage"), false, [&](std::shared_ptr<const Telegram> t) { process_MMSetMessage(t); });
    }

    // HT3
    if (flags == EMSdevice::EMS_DEVICE_FLAG_IPM) {
        register_telegram_type(0x010C, F("IPMSetMessage"), false, [&](std::shared_ptr<const Telegram> t) { process_IPMStatusMessage(t); });
    }

    // API call
    Command::add_with_json(this->device_type(), F("info"), [&](const char * value, const int8_t id, JsonObject & object) {
        return command_info(value, id, object);
    });
}

// output json to web UI
void Mixing::device_info_web(JsonArray & root) {
    if (type() == Type::NONE) {
        return; // don't have any values yet
    }

    // fetch the values into a JSON document
    StaticJsonDocument<EMSESP_MAX_JSON_SIZE_MEDIUM> doc;
    JsonObject                                      output = doc.to<JsonObject>();
    if (!export_values(Mqtt::Format::SINGLE, output)) {
        return; // empty
    }

    char prefix_str[10];
    if (type() == Type::HC) {
        snprintf_P(prefix_str, sizeof(prefix_str), PSTR("(hc %d) "), hc_);
        print_value_json(root, F("flowTemp"), FPSTR(prefix_str), F_(flowTemp), F_(degrees), output);
        print_value_json(root, F("flowSetTemp"), FPSTR(prefix_str), F_(flowSetTemp), F_(degrees), output);
        print_value_json(root, F("pumpStatus"), FPSTR(prefix_str), F_(pumpStatus), nullptr, output);
        print_value_json(root, F("valveStatus"), FPSTR(prefix_str), F_(valveStatus), F_(percent), output);
    } else {
        snprintf_P(prefix_str, sizeof(prefix_str), PSTR("(wwc %d) "), hc_);
        print_value_json(root, F("wwTemp"), FPSTR(prefix_str), F_(wwTemp), F_(degrees), output);
        print_value_json(root, F("pumpStatus"), FPSTR(prefix_str), F_(pumpStatus), nullptr, output);
        print_value_json(root, F("tempStatus"), FPSTR(prefix_str), F_(tempStatus), nullptr, output);
    }
}

// check to see if values have been updated
bool Mixing::updated_values() {
    if (changed_) {
        changed_ = false;
        return true;
    }
    return false;
}

// display all values into the shell console
void Mixing::show_values(uuid::console::Shell & shell) {
    EMSdevice::show_values(shell); // always call this to show header

    if (type() == Type::NONE) {
        return; // don't have any values yet
    }

    // fetch the values into a JSON document
    StaticJsonDocument<EMSESP_MAX_JSON_SIZE_MEDIUM> doc;
    JsonObject                                      output = doc.to<JsonObject>();
    if (!export_values(Mqtt::Format::SINGLE, output)) {
        return; // empty
    }

    if (type() == Type::HC) {
        shell.printfln(F_(hc), hc_);
        print_value_json(shell, F("flowTemp"), F_(2spaces), F_(flowTemp), F_(degrees), output);
        print_value_json(shell, F("flowSetTemp"), F_(2spaces), F_(flowSetTemp), F_(degrees), output);
        print_value_json(shell, F("pumpStatus"), F_(2spaces), F_(pumpStatus), nullptr, output);
        print_value_json(shell, F("valveStatus"), F_(2spaces), F_(valveStatus), F_(percent), output);
    } else {
        shell.printfln(F_(ww_hc), hc_);
        print_value_json(shell, F("wwTemp"), F_(2spaces), F_(wwTemp), F_(degrees), output);
        print_value_json(shell, F("pumpStatus"), F_(2spaces), F_(pumpStatus), nullptr, output);
        print_value_json(shell, F("tempStatus"), F_(2spaces), F_(tempStatus), nullptr, output);
    }

    shell.println();
}

// export all values to info command
bool Mixing::command_info(const char * value, const int8_t id, JsonObject & output) {
    if (id != (device_id() - 0x20 + 1) && id > 0) { // defaults to first hc if no id
        return false;
    }
    return (export_values(Mqtt::Format::NESTED, output));
}

// publish values via MQTT
// topic is mixing_data<id>
void Mixing::publish_values(JsonObject & data, bool force) {
    // handle HA first
    if (Mqtt::mqtt_format() == Mqtt::Format::HA) {
        register_mqtt_ha_config(force);
    }

    if (Mqtt::mqtt_format() == Mqtt::Format::SINGLE) {
        StaticJsonDocument<EMSESP_MAX_JSON_SIZE_SMALL> doc;
        JsonObject                                     output = doc.to<JsonObject>();
        if (export_values(Mqtt::mqtt_format(), output)) {
            char topic[30];
            char s[5];
            strlcpy(topic, "mixing_data", 30);
            strlcat(topic, Helpers::itoa(s, device_id() - 0x20 + 1), 30); // append device_id to topic
            Mqtt::publish(topic, doc.as<JsonObject>());
        }
    } else {
        // format is HA or Nested. This is bundled together and sent in emsesp.cpp
        (void)export_values(Mqtt::mqtt_format(), data);
    }
}

// publish config topic for HA MQTT Discovery
void Mixing::register_mqtt_ha_config(bool force) {
    if ((mqtt_ha_config_ && !force)) {
        return;
    }

    if (!Mqtt::connected()) {
        return;
    }

    // Create the Master device
    StaticJsonDocument<EMSESP_MAX_JSON_SIZE_MEDIUM> doc;

    char str1[20];
    snprintf_P(str1, sizeof(str1), PSTR("Mixing %d"), device_id() - 0x20 + 1);
    doc["name"] = str1;

    char str2[20];
    snprintf_P(str2, sizeof(str2), PSTR("mixing %d"), device_id() - 0x20 + 1);
    doc["uniq_id"] = str2;

    doc["ic"] = F("mdi:home-thermometer-outline");

    char stat_t[50];
    snprintf_P(stat_t, sizeof(stat_t), PSTR("%s/mixing_data"), System::hostname().c_str());
    doc["stat_t"] = stat_t;

    doc["val_tpl"] = F("{{value_json.type}}"); // HA needs a single value. We take the type which is wwc or hc
    JsonObject dev = doc.createNestedObject("dev");
    dev["name"]    = F("EMS-ESP Mixing");
    dev["sw"]      = EMSESP_APP_VERSION;
    dev["mf"]      = this->brand_to_string();
    dev["mdl"]     = this->name();
    JsonArray ids  = dev.createNestedArray("ids");
    ids.add("ems-esp-mixing");

    std::string topic(100, '\0');
    if (this->type() == Type::HC) {
        snprintf_P(&topic[0], topic.capacity() + 1, PSTR("homeassistant/climate/ems-esp/mixing_hc%d/config"), hc_);
        Mqtt::publish_retain(topic, doc.as<JsonObject>(), true); // publish the config payload with retain flag
        char hc_name[10];
        snprintf_P(hc_name, sizeof(hc_name), PSTR("hc%d"), hc_);
        Mqtt::register_mqtt_ha_sensor(hc_name, nullptr, F_(flowTemp), this->device_type(), "flowTemp", F_(degrees), F_(icontemperature));
        Mqtt::register_mqtt_ha_sensor(hc_name, nullptr, F_(flowSetTemp), this->device_type(), "flowSetTemp", F_(degrees), F_(icontemperature));
        Mqtt::register_mqtt_ha_sensor(hc_name, nullptr, F_(pumpStatus), this->device_type(), "pumpStatus", nullptr, nullptr);
        Mqtt::register_mqtt_ha_sensor(hc_name, nullptr, F_(valveStatus), this->device_type(), "valveStatus", nullptr, nullptr);
    } else {
        // WWC
        snprintf_P(&topic[0], topic.capacity() + 1, PSTR("homeassistant/climate/ems-esp/mixing_wwc%d/config"), hc_);
        Mqtt::publish_retain(topic, doc.as<JsonObject>(), true); // publish the config payload with retain flag
        char wwc_name[10];
        snprintf_P(wwc_name, sizeof(wwc_name), PSTR("wwc%d"), hc_);
        Mqtt::register_mqtt_ha_sensor(wwc_name, nullptr, F_(wwTemp), this->device_type(), "wwTemp", F_(degrees), F_(icontemperature));
        Mqtt::register_mqtt_ha_sensor(wwc_name, nullptr, F_(pumpStatus), this->device_type(), "pumpStatus", nullptr, nullptr);
        Mqtt::register_mqtt_ha_sensor(wwc_name, nullptr, F_(tempStatus), this->device_type(), "tempStatus", nullptr, nullptr);
    }

    mqtt_ha_config_ = true; // done
}

// creates JSON doc from values
// returns false if empty
bool Mixing::export_values(uint8_t mqtt_format, JsonObject & output) {
    JsonObject output_hc;
    char       hc_name[10]; // hc{1-4}

    if (this->type() == Type::HC) {
        snprintf_P(hc_name, sizeof(hc_name), PSTR("hc%d"), hc_);
        if (mqtt_format == Mqtt::Format::NESTED || mqtt_format == Mqtt::Format::HA) {
            output_hc = output.createNestedObject(hc_name);
        } else {
            output_hc      = output;
            output["type"] = F("hc");
        }
        if (Helpers::hasValue(flowTemp_)) {
            output_hc["flowTemp"] = (float)flowTemp_ / 10;
        }
        if (Helpers::hasValue(flowSetTemp_)) {
            output_hc["flowSetTemp"] = flowSetTemp_;
        }
        if (Helpers::hasValue(pumpStatus_)) {
            char s[5];
            output_hc["pumpStatus"] = Helpers::render_value(s, pumpStatus_, EMS_VALUE_BOOL);
        }
        if (Helpers::hasValue(status_)) {
            output_hc["valveStatus"] = status_;
        }
    } else {
        snprintf_P(hc_name, sizeof(hc_name), PSTR("wwc%d"), hc_);
        if (mqtt_format == Mqtt::Format::NESTED || mqtt_format == Mqtt::Format::HA) {
            output_hc = output.createNestedObject(hc_name);
        } else {
            output_hc      = output;
            output["type"] = F("wwc");
        }
        if (Helpers::hasValue(flowTemp_)) {
            output_hc["wwTemp"] = (float)flowTemp_ / 10;
        }
        if (Helpers::hasValue(pumpStatus_)) {
            char s[5];
            output_hc["pumpStatus"] = Helpers::render_value(s, pumpStatus_, EMS_VALUE_BOOL);
        }
        if (Helpers::hasValue(status_)) {
            output_hc["tempStatus"] = status_;
        }
    }

    return output.size();
}

// heating circuits 0x02D7, 0x02D8 etc...
// e.g.  A0 00 FF 00 01 D7 00 00 00 80 00 00 00 00 03 C5
//       A0 0B FF 00 01 D7 00 00 00 80 00 00 00 00 03 80
void Mixing::process_MMPLUSStatusMessage_HC(std::shared_ptr<const Telegram> telegram) {
    type(Type::HC);
    hc_ = telegram->type_id - 0x02D7 + 1;           // determine which circuit this is
    changed_ |= telegram->read_value(flowTemp_, 3); // is * 10
    changed_ |= telegram->read_value(flowSetTemp_, 5);
    changed_ |= telegram->read_bitvalue(pumpStatus_, 2, 0);
    changed_ |= telegram->read_value(status_, 2); // valve status
}

// Mixing module warm water loading/DHW - 0x0331, 0x0332
// e.g. A9 00 FF 00 02 32 02 6C 00 3C 00 3C 3C 46 02 03 03 00 3C // on 0x28
//      A8 00 FF 00 02 31 02 35 00 3C 00 3C 3C 46 02 03 03 00 3C // in 0x29
void Mixing::process_MMPLUSStatusMessage_WWC(std::shared_ptr<const Telegram> telegram) {
    type(Type::WWC);
    hc_ = telegram->type_id - 0x0331 + 1;           // determine which circuit this is. There are max 2.
    changed_ |= telegram->read_value(flowTemp_, 0); // is * 10
    changed_ |= telegram->read_bitvalue(pumpStatus_, 2, 0);
    changed_ |= telegram->read_value(status_, 11); // temp status
}

// Mixing IMP - 0x010C
// e.g.  A0 00 FF 00 00 0C 01 00 00 00 00 00 54
//       A1 00 FF 00 00 0C 02 04 00 01 1D 00 82
void Mixing::process_IPMStatusMessage(std::shared_ptr<const Telegram> telegram) {
    type(Type::HC);
    hc_             = device_id() - 0x20 + 1;
    uint8_t ismixed = 0;
    changed_ |= telegram->read_value(ismixed, 0); // check if circuit is active, 0-off, 1-unmixed, 2-mixed
    if (ismixed == 0) {
        return;
    }

    if (ismixed == 2) {                                 // we have a mixed circuit
        changed_ |= telegram->read_value(flowTemp_, 3); // is * 10
        changed_ |= telegram->read_value(flowSetTemp_, 5);
        changed_ |= telegram->read_value(status_, 2); // valve status
    }

    changed_ |= telegram->read_bitvalue(pumpStatus_, 1, 0); // pump is also in unmixed circuits
}

// Mixing on a MM10 - 0xAB
// e.g. Mixing Module -> All, type 0xAB, telegram: 21 00 AB 00 2D 01 BE 64 04 01 00 (CRC=15) #data=7
// see also https://github.com/proddy/EMS-ESP/issues/386
void Mixing::process_MMStatusMessage(std::shared_ptr<const Telegram> telegram) {
    type(Type::HC);

    // the heating circuit is determine by which device_id it is, 0x20 - 0x23
    // 0x21 is position 2. 0x20 is typically reserved for the WM10 switch module
    // see https://github.com/proddy/EMS-ESP/issues/270 and https://github.com/proddy/EMS-ESP/issues/386#issuecomment-629610918
    hc_ = device_id() - 0x20 + 1;
    changed_ |= telegram->read_value(flowTemp_, 1);         // is * 10
    changed_ |= telegram->read_bitvalue(pumpStatus_, 3, 2); // is 0 or 0x64 (100%), check only bit 2
    changed_ |= telegram->read_value(flowSetTemp_, 0);
    changed_ |= telegram->read_value(status_, 4); // valve status -100 to 100
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

// Mixing on a MM10 - 0xAA
// e.g. Thermostat -> Mixing Module, type 0xAA, telegram: 10 21 AA 00 FF 0C 0A 11 0A 32 xx
void Mixing::process_MMConfigMessage(std::shared_ptr<const Telegram> telegram) {
    hc_ = device_id() - 0x20 + 1;
    // pos 0: active FF = on
    // pos 1: valve runtime 0C = 120 sec in units of 10 sec
}

// Mixing on a MM10 - 0xAC
// e.g. Thermostat -> Mixing Module, type 0xAC, telegram: 10 21 AC 00 1E 64 01 AB
void Mixing::process_MMSetMessage(std::shared_ptr<const Telegram> telegram) {
    hc_ = device_id() - 0x20 + 1;
    // pos 0: flowtemp setpoint 1E = 30°C
    // pos 1: position in %
}

#pragma GCC diagnostic pop

} // namespace emsesp