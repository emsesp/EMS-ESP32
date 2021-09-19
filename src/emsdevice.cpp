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

#include "emsdevice.h"
#include "emsesp.h"

namespace emsesp {

// mapping of UOM, to match order in DeviceValueUOM enum emsdevice.h
// must be an int of 4 bytes, 32bit aligned
static const __FlashStringHelper * DeviceValueUOM_s[] __attribute__((__aligned__(sizeof(uint32_t)))) PROGMEM = {

    F_(degrees),
    F_(percent),
    F_(lmin),
    F_(kwh),
    F_(wh),
    F_(hours),
    F_(minutes),
    F_(ua),
    F_(bar),
    F_(kw),
    F_(w),
    F_(kb),
    F_(seconds),
    F_(dbm),
    F_(num),
    F_(bool),
    F_(blank)

};

// mapping of TAGs, to match order in DeviceValueTAG enum in emsdevice.h
// must be an int of 4 bytes, 32bit aligned
static const __FlashStringHelper * const DeviceValueTAG_s[] PROGMEM = {

    F_(tag_none),            // ""
    F_(tag_heartbeat),       // ""
    F_(tag_boiler_data),     // ""
    F_(tag_boiler_data_ww),  // "ww"
    F_(tag_thermostat_data), // ""
    F_(tag_hc1),             // "hc1"
    F_(tag_hc2),             // "hc2"
    F_(tag_hc3),             // "hc3"
    F_(tag_hc4),             // "hc4"
    F_(tag_wwc1),            // "wwc1"
    F_(tag_wwc2),            // "Wwc2"
    F_(tag_wwc3),            // "wwc3"
    F_(tag_wwc4),            // "wwc4"
    F_(tag_hs1),             // "hs1"
    F_(tag_hs2),             // "hs2"
    F_(tag_hs3),             // "hs3"
    F_(tag_hs4),             // "hs4"
    F_(tag_hs5),             // "hs5"
    F_(tag_hs6),             // "hs6"
    F_(tag_hs7),             // "hs7"
    F_(tag_hs8),             // "hs8"
    F_(tag_hs9),             // "hs9"
    F_(tag_hs10),            // "hs10"
    F_(tag_hs11),            // "hs11"
    F_(tag_hs12),            // "hs12"
    F_(tag_hs13),            // "hs13"
    F_(tag_hs14),            // "hs14"
    F_(tag_hs15),            // "hs15"
    F_(tag_hs16)             // "hs16"

};

// MQTT topics derived from tags
static const __FlashStringHelper * const DeviceValueTAG_mqtt[] PROGMEM = {

    F_(tag_none),                // ""
    F_(heartbeat),               // "heartbeat"
    F_(tag_boiler_data_mqtt),    // ""
    F_(tag_boiler_data_ww_mqtt), // "ww"
    F_(tag_thermostat_data),     // ""
    F_(tag_hc1),                 // "hc1"
    F_(tag_hc2),                 // "hc2"
    F_(tag_hc3),                 // "hc3"
    F_(tag_hc4),                 // "hc4"
    F_(tag_wwc1),                // "wwc1"
    F_(tag_wwc2),                // "Wwc2"
    F_(tag_wwc3),                // "wwc3"
    F_(tag_wwc4),                // "wwc4"
    F_(tag_hs1),                 // "hs1"
    F_(tag_hs2),                 // "hs2"
    F_(tag_hs3),                 // "hs3"
    F_(tag_hs4),                 // "hs4"
    F_(tag_hs5),                 // "hs5"
    F_(tag_hs6),                 // "hs6"
    F_(tag_hs7),                 // "hs7"
    F_(tag_hs8),                 // "hs8"
    F_(tag_hs9),                 // "hs9"
    F_(tag_hs10),                // "hs10"
    F_(tag_hs11),                // "hs11"
    F_(tag_hs12),                // "hs12"
    F_(tag_hs13),                // "hs13"
    F_(tag_hs14),                // "hs14"
    F_(tag_hs15),                // "hs15"
    F_(tag_hs16)                 // "hs16"

};

const std::string EMSdevice::tag_to_string(uint8_t tag) {
    return uuid::read_flash_string(DeviceValueTAG_s[tag]);
}

const std::string EMSdevice::tag_to_mqtt(uint8_t tag) {
    return uuid::read_flash_string(DeviceValueTAG_mqtt[tag]);
}

const std::string EMSdevice::uom_to_string(uint8_t uom) {
    if (uom == DeviceValueUOM::NONE) {
        return std::string{};
    }
    return uuid::read_flash_string(DeviceValueUOM_s[uom - 1]); // offset by 1 to account for NONE
}

const std::vector<EMSdevice::DeviceValue> EMSdevice::devicevalues() const {
    return devicevalues_;
}

const std::string EMSdevice::brand_to_string() const {
    switch (brand_) {
    case EMSdevice::Brand::BOSCH:
        return read_flash_string(F("Bosch"));
        break;
    case EMSdevice::Brand::JUNKERS:
        return read_flash_string(F("Junkers"));
        break;
    case EMSdevice::Brand::BUDERUS:
        return read_flash_string(F("Buderus"));
        break;
    case EMSdevice::Brand::NEFIT:
        return read_flash_string(F("Nefit"));
        break;
    case EMSdevice::Brand::SIEGER:
        return read_flash_string(F("Sieger"));
        break;
    case EMSdevice::Brand::WORCESTER:
        return read_flash_string(F("Worcester"));
        break;
    case EMSdevice::Brand::IVT:
        return read_flash_string(F("IVT"));
        break;
    case EMSdevice::Brand::NO_BRAND:
    default:
        return read_flash_string(F(""));
        break;
    }

    return std::string{};
}

// returns the name of the MQTT topic to use for a specific device, without the base
const std::string EMSdevice::device_type_2_device_name(const uint8_t device_type) {
    switch (device_type) {
    case DeviceType::SYSTEM:
        return read_flash_string(F_(system));
        break;

    case DeviceType::BOILER:
        return read_flash_string(F_(boiler));
        break;

    case DeviceType::THERMOSTAT:
        return read_flash_string(F_(thermostat));
        break;

    case DeviceType::HEATPUMP:
        return read_flash_string(F_(heatpump));
        break;

    case DeviceType::SOLAR:
        return read_flash_string(F_(solar));
        break;

    case DeviceType::CONNECT:
        return read_flash_string(F_(connect));
        break;

    case DeviceType::MIXER:
        return read_flash_string(F_(mixer));
        break;

    case DeviceType::DALLASSENSOR:
        return read_flash_string(F_(dallassensor));
        break;

    case DeviceType::CONTROLLER:
        return read_flash_string(F_(controller));
        break;

    case DeviceType::SWITCH:
        return read_flash_string(F_(switch));
        break;

    case DeviceType::GATEWAY:
        return read_flash_string(F_(gateway));
        break;

    default:
        return read_flash_string(F_(unknown));
        break;
    }
}

// returns device_type from a string
uint8_t EMSdevice::device_name_2_device_type(const char * topic) {
    // convert topic to lowercase and compare
    char lowtopic[20];
    strlcpy(lowtopic, topic, sizeof(lowtopic));
    for (char * p = lowtopic; *p; p++) {
        *p = tolower(*p);
    }

    if (!strcmp(lowtopic, reinterpret_cast<PGM_P>(F_(boiler)))) {
        return DeviceType::BOILER;
    }

    if (!strcmp(lowtopic, reinterpret_cast<PGM_P>(F_(thermostat)))) {
        return DeviceType::THERMOSTAT;
    }

    if (!strcmp(lowtopic, reinterpret_cast<PGM_P>(F_(system)))) {
        return DeviceType::SYSTEM;
    }

    if (!strcmp(lowtopic, reinterpret_cast<PGM_P>(F_(heatpump)))) {
        return DeviceType::HEATPUMP;
    }

    if (!strcmp(lowtopic, reinterpret_cast<PGM_P>(F_(solar)))) {
        return DeviceType::SOLAR;
    }

    if (!strcmp(lowtopic, reinterpret_cast<PGM_P>(F_(mixer)))) {
        return DeviceType::MIXER;
    }

    if (!strcmp(lowtopic, reinterpret_cast<PGM_P>(F_(dallassensor)))) {
        return DeviceType::DALLASSENSOR;
    }

    return DeviceType::UNKNOWN;
}

// return name of the device type, capitalized
const std::string EMSdevice::device_type_name() const {
    std::string s = device_type_2_device_name(device_type_);
    s[0]          = toupper(s[0]);
    return s;
}

// 0=unknown, 1=bosch, 2=junkers, 3=buderus, 4=nefit, 5=sieger, 11=worcester
uint8_t EMSdevice::decode_brand(uint8_t value) {
    switch (value) {
    case 1:
        return EMSdevice::Brand::BOSCH;
        break;
    case 2:
        return EMSdevice::Brand::JUNKERS;
        break;
    case 3:
        return EMSdevice::Brand::BUDERUS;
        break;
    case 4:
        return EMSdevice::Brand::NEFIT;
        break;
    case 5:
        return EMSdevice::Brand::SIEGER;
        break;
    case 11:
        return EMSdevice::Brand::WORCESTER;
        break;
    case 13:
        return EMSdevice::Brand::IVT;
        break;
    case 0:
    default:
        return EMSdevice::Brand::NO_BRAND;
        break;
    }
}

// returns string of a human friendly description of the EMS device
const std::string EMSdevice::to_string() const {
    std::string str(160, '\0');

    // for devices that haven't been lookup yet, don't show all details
    if (product_id_ == 0) {
        snprintf(&str[0], str.capacity() + 1, "%s (DeviceID:0x%02X)", name_.c_str(), device_id_);
        return str;
    }

    if (brand_ == Brand::NO_BRAND) {
        snprintf(&str[0], str.capacity() + 1, "%s (DeviceID:0x%02X, ProductID:%d, Version:%s)", name_.c_str(), device_id_, product_id_, version_.c_str());
    } else {
        snprintf(&str[0],
                 str.capacity() + 1,
                 "%s %s (DeviceID:0x%02X ProductID:%d, Version:%s)",
                 brand_to_string().c_str(),
                 name_.c_str(),
                 device_id_,
                 product_id_,
                 version_.c_str());
    }

    return str;
}

// returns out brand + device name
const std::string EMSdevice::to_string_short() const {
    std::string str(160, '\0');
    if (brand_ == Brand::NO_BRAND) {
        snprintf(&str[0], str.capacity() + 1, "%s: %s", device_type_name().c_str(), name_.c_str());
    } else {
        snprintf(&str[0], str.capacity() + 1, "%s: %s %s", device_type_name().c_str(), brand_to_string().c_str(), name_.c_str());
    }
    return str;
}

// for each telegram that has the fetch value set (true) do a read request
void EMSdevice::fetch_values() {
    EMSESP::logger().debug(F("Fetching values for device ID 0x%02X"), device_id());

    for (const auto & tf : telegram_functions_) {
        if (tf.fetch_) {
            read_command(tf.telegram_type_id_);
        }
    }
}

// toggle on/off automatic fetch for a telegram id
void EMSdevice::toggle_fetch(uint16_t telegram_id, bool toggle) {
    EMSESP::logger().debug(F("Toggling fetch for device ID 0x%02X, telegram ID 0x%02X to %d"), device_id(), telegram_id, toggle);

    for (auto & tf : telegram_functions_) {
        if (tf.telegram_type_id_ == telegram_id) {
            tf.fetch_ = toggle;
        }
    }
}

// get status of automatic fetch for a telegram id
bool EMSdevice::is_fetch(uint16_t telegram_id) {
    for (auto & tf : telegram_functions_) {
        if (tf.telegram_type_id_ == telegram_id) {
            return tf.fetch_;
        }
    }
    return false;
}

// list of registered device entries, adding the HA entity if it exists
void EMSdevice::list_device_entries(JsonObject & json) {
    for (const auto & dv : devicevalues_) {
        if (dv.full_name && dv.type != DeviceValueType::CMD) {
            // if we have a tag prefix it
            char key[50];
            if (!EMSdevice::tag_to_string(dv.tag).empty()) {
                snprintf(key, 51, "%s.%s", EMSdevice::tag_to_string(dv.tag).c_str(), uuid::read_flash_string(dv.short_name).c_str());
            } else {
                snprintf(key, 51, "%s", uuid::read_flash_string(dv.short_name).c_str());
            }

            JsonArray details = json.createNestedArray(key);

            // add the full name description
            details.add(dv.full_name);

            // add uom
            if (!uom_to_string(dv.uom).empty() && uom_to_string(dv.uom) != " ") {
                details.add(EMSdevice::uom_to_string(dv.uom));
            }

            // add ha sensor name
            if (Mqtt::ha_enabled()) {
                details.add(device_entity_ha(dv));
            }
        }
    }
}

// creates the HA sensor entity from a device value
const std::string EMSdevice::device_entity_ha(DeviceValue const & dv) {
    std::string entity_name(50, '\0');
    if (EMSdevice::tag_to_string(dv.tag).empty()) {
        snprintf(&entity_name[0], entity_name.capacity() + 1, "sensor.%s %s", this->device_type_name().c_str(), uuid::read_flash_string(dv.full_name).c_str());
    } else {
        snprintf(&entity_name[0],
                 entity_name.capacity() + 1,
                 "sensor.%s %s %s",
                 this->device_type_name().c_str(),
                 EMSdevice::tag_to_string(dv.tag).c_str(),
                 uuid::read_flash_string(dv.full_name).c_str());
    }
    std::replace(entity_name.begin(), entity_name.end(), ' ', '_');
    return Helpers::toLower(entity_name);
}

// list all the telegram type IDs for this device
void EMSdevice::show_telegram_handlers(uuid::console::Shell & shell) {
    if (telegram_functions_.size() == 0) {
        return;
    }

    shell.printf(F(" This %s will respond to telegram type IDs: "), device_type_name().c_str());
    for (const auto & tf : telegram_functions_) {
        shell.printf(F("0x%02X "), tf.telegram_type_id_);
    }
    shell.println();
}

// list all the telegram type IDs for this device, outputting to a string (max size 200)
char * EMSdevice::show_telegram_handlers(char * result) {
    uint8_t size = telegram_functions_.size();

    strlcpy(result, "", 200);

    if (!size) {
        return result;
    }

    char    str[10];
    uint8_t i = 0;
    for (const auto & tf : telegram_functions_) {
        snprintf(str, sizeof(str), "0x%02X", tf.telegram_type_id_);
        strlcat(result, str, 200);
        if (++i < size) {
            strlcat(result, " ", 200);
        }
    }

    return result;
}

// list all the mqtt handlers for this device
void EMSdevice::show_mqtt_handlers(uuid::console::Shell & shell) {
    Mqtt::show_topic_handlers(shell, device_type_);
}

void EMSdevice::register_mqtt_topic(const std::string & topic, const mqtt_sub_function_p f) {
    Mqtt::subscribe(device_type_, topic, f);
}

// register a callback function for a specific telegram type
void EMSdevice::register_telegram_type(const uint16_t telegram_type_id, const __FlashStringHelper * telegram_type_name, bool fetch, const process_function_p f) {
    telegram_functions_.emplace_back(telegram_type_id, telegram_type_name, fetch, f);
}

// add to device value library
// arguments are:
//  tag: to be used to group mqtt together, either as separate topics as a nested object
//  value: pointer to the value from the .h file
//  type: one of DeviceValueType
//  options: options for enum or a divider for int (e.g. F("10"))
//  short_name: used in Mqtt as keys
//  full_name: used in Web and Console unless empty (nullptr)
//  uom: unit of measure from DeviceValueUOM
void EMSdevice::register_device_value(uint8_t                             tag,
                                      void *                              value_p,
                                      uint8_t                             type,
                                      const __FlashStringHelper * const * options,
                                      const __FlashStringHelper *         short_name,
                                      const __FlashStringHelper *         full_name,
                                      uint8_t                             uom,
                                      bool                                has_cmd,
                                      int32_t                             min,
                                      uint32_t                            max) {
    // init the value depending on it's type
    if (type == DeviceValueType::TEXT) {
        *(char *)(value_p) = {'\0'};
    } else if (type == DeviceValueType::INT) {
        *(int8_t *)(value_p) = EMS_VALUE_INT_NOTSET;
    } else if (type == DeviceValueType::SHORT) {
        *(int16_t *)(value_p) = EMS_VALUE_SHORT_NOTSET;
    } else if (type == DeviceValueType::USHORT) {
        *(uint16_t *)(value_p) = EMS_VALUE_USHORT_NOTSET;
    } else if ((type == DeviceValueType::ULONG) || (type == DeviceValueType::TIME)) {
        *(uint32_t *)(value_p) = EMS_VALUE_ULONG_NOTSET;
    } else if (type == DeviceValueType::BOOL) {
        *(int8_t *)(value_p) = EMS_VALUE_BOOL_NOTSET; // bool is uint8_t, but other initial value
    } else {
        *(uint8_t *)(value_p) = EMS_VALUE_UINT_NOTSET; // enums behave as uint8_t
    }

    // count #options
    uint8_t options_size = 0;
    if (options != nullptr) {
        uint8_t i = 0;
        while (options[i++]) {
            options_size++;
        };
    }

    devicevalues_.emplace_back(device_type_, tag, value_p, type, options, options_size, short_name, full_name, uom, 0, has_cmd, min, max);
}

// function with min and max values
// adds a new command to the command list
void EMSdevice::register_device_value(uint8_t                             tag,
                                      void *                              value_p,
                                      uint8_t                             type,
                                      const __FlashStringHelper * const * options,
                                      const __FlashStringHelper * const * name,
                                      uint8_t                             uom,
                                      const cmd_function_p                f,
                                      int32_t                             min,
                                      uint32_t                            max) {
    register_device_value(tag, value_p, type, options, name[0], name[1], uom, (f != nullptr), min, max);

    // add a new command if it has a function attached
    if (f == nullptr) {
        return;
    }

    if (tag >= TAG_HC1 && tag <= TAG_HC4) {
        Command::add(device_type_, name[0], f, name[1], CommandFlag::MQTT_SUB_FLAG_HC | CommandFlag::ADMIN_ONLY);
    } else if (tag >= TAG_WWC1 && tag <= TAG_WWC4) {
        Command::add(device_type_, name[0], f, name[1], CommandFlag::MQTT_SUB_FLAG_WWC | CommandFlag::ADMIN_ONLY);
    } else {
        Command::add(device_type_, name[0], f, name[1], CommandFlag::MQTT_SUB_FLAG_NORMAL | CommandFlag::ADMIN_ONLY);
    }
}

// function with no min and max values
void EMSdevice::register_device_value(uint8_t                             tag,
                                      void *                              value_p,
                                      uint8_t                             type,
                                      const __FlashStringHelper * const * options,
                                      const __FlashStringHelper * const * name,
                                      uint8_t                             uom,
                                      const cmd_function_p                f) {
    register_device_value(tag, value_p, type, options, name, uom, f, 0, 0);
}

// no command function
void EMSdevice::register_device_value(uint8_t                             tag,
                                      void *                              value_p,
                                      uint8_t                             type,
                                      const __FlashStringHelper * const * options,
                                      const __FlashStringHelper * const * name,
                                      uint8_t                             uom) {
    register_device_value(tag, value_p, type, options, name, uom, nullptr, 0, 0);
}

// looks up the uom (suffix) for a given key from the device value table
const std::string EMSdevice::get_value_uom(const char * key) {
    // the key may have a suffix at the start which is between brackets. remove it.
    char new_key[80];
    strlcpy(new_key, key, sizeof(new_key));
    char * p = new_key;
    if (key[0] == '(') {
        while ((*p++ != ')') && (*p != '\0'))
            ;
        p++;
    }

    // find the key (p) in the name
    for (const auto & dv : devicevalues_) {
        if (dv.full_name != nullptr) {
            if (uuid::read_flash_string(dv.full_name) == p) {
                // ignore TIME since "minutes" is already added to the string value
                if ((dv.uom == DeviceValueUOM::NONE) || (dv.uom == DeviceValueUOM::MINUTES)) {
                    break;
                }
                return EMSdevice::uom_to_string(dv.uom);
            }
        }
    }

    return std::string{}; // not found
}

// prepare array of device values used for the WebUI
// v = value, u=uom, n=name, c=cmd
void EMSdevice::generate_values_json_web(JsonObject & json) {
    json["name"]   = to_string_short();
    JsonArray data = json.createNestedArray("data");

    for (const auto & dv : devicevalues_) {
        // ignore if full_name empty and also commands
        if ((dv.full_name != nullptr) && (dv.type != DeviceValueType::CMD)) {
            JsonObject obj; // create the object, if needed

            // handle Booleans (true, false)
            if ((dv.type == DeviceValueType::BOOL) && Helpers::hasValue(*(uint8_t *)(dv.value_p), EMS_VALUE_BOOL)) {
                obj      = data.createNestedObject();
                obj["v"] = *(bool *)(dv.value_p);
            }

            // handle TEXT strings
            else if ((dv.type == DeviceValueType::TEXT) && (Helpers::hasValue((char *)(dv.value_p)))) {
                obj      = data.createNestedObject();
                obj["v"] = (char *)(dv.value_p);
            }

            // handle ENUMs
            else if ((dv.type == DeviceValueType::ENUM) && Helpers::hasValue(*(uint8_t *)(dv.value_p))) {
                if (*(uint8_t *)(dv.value_p) < dv.options_size) {
                    obj      = data.createNestedObject();
                    obj["v"] = dv.options[*(uint8_t *)(dv.value_p)];
                }
            }

            // // handle commands without value
            // else if (dv.type == DeviceValueType::CMD) {
            //     obj      = data.createNestedObject();
            //     obj["v"] = "";
            // }

            else {
                // handle Integers and Floats
                // If a divider is specified, do the division to 2 decimals places and send back as double/float
                // otherwise force as an integer whole
                // the nested if's is necessary due to the way the ArduinoJson templates are pre-processed by the compiler
                uint8_t divider = (dv.options_size == 1) ? Helpers::atoint(uuid::read_flash_string(dv.options[0]).c_str()) : 0;

                if ((dv.type == DeviceValueType::INT) && Helpers::hasValue(*(int8_t *)(dv.value_p))) {
                    obj      = data.createNestedObject();
                    obj["v"] = (divider) ? Helpers::round2(*(int8_t *)(dv.value_p), divider) : *(int8_t *)(dv.value_p);
                } else if ((dv.type == DeviceValueType::UINT) && Helpers::hasValue(*(uint8_t *)(dv.value_p))) {
                    obj      = data.createNestedObject();
                    obj["v"] = (divider) ? Helpers::round2(*(uint8_t *)(dv.value_p), divider) : *(uint8_t *)(dv.value_p);
                } else if ((dv.type == DeviceValueType::SHORT) && Helpers::hasValue(*(int16_t *)(dv.value_p))) {
                    obj      = data.createNestedObject();
                    obj["v"] = (divider) ? Helpers::round2(*(int16_t *)(dv.value_p), divider) : *(int16_t *)(dv.value_p);
                } else if ((dv.type == DeviceValueType::USHORT) && Helpers::hasValue(*(uint16_t *)(dv.value_p))) {
                    obj      = data.createNestedObject();
                    obj["v"] = (divider) ? Helpers::round2(*(uint16_t *)(dv.value_p), divider) : *(uint16_t *)(dv.value_p);
                } else if ((dv.type == DeviceValueType::ULONG) && Helpers::hasValue(*(uint32_t *)(dv.value_p))) {
                    obj      = data.createNestedObject();
                    obj["v"] = (divider) ? Helpers::round2(*(uint32_t *)(dv.value_p), divider) : *(uint32_t *)(dv.value_p);
                } else if ((dv.type == DeviceValueType::TIME) && Helpers::hasValue(*(uint32_t *)(dv.value_p))) {
                    uint32_t time_value = *(uint32_t *)(dv.value_p);
                    obj                 = data.createNestedObject();
                    obj["v"]            = (divider) ? time_value / divider : time_value; // sometimes we need to divide by 60
                }
            }

            // check if we've added a data element then add the remaining elements
            if (obj.containsKey("v")) {
                // add the unit of measure (uom)
                obj["u"] = dv.uom;

                // add name, prefixing the tag if it exists
                if ((dv.tag == DeviceValueTAG::TAG_NONE) || tag_to_string(dv.tag).empty()) {
                    obj["n"] = dv.full_name;
                } else {
                    char name[50];
                    snprintf(name, sizeof(name), "%s %s", tag_to_string(dv.tag).c_str(), uuid::read_flash_string(dv.full_name).c_str());
                    obj["n"] = name;
                }

                // add the name of the Command function if it exists
                if (dv.has_cmd) {
                    if (dv.tag >= DeviceValueTAG::TAG_HC1) {
                        obj["c"] = tag_to_string(dv.tag) + "/" + uuid::read_flash_string(dv.short_name);
                    } else {
                        obj["c"] = dv.short_name;
                    }
                } else {
                    obj["c"] = "";
                }

                // add enum and text option settings
                if ((dv.uom == DeviceValueUOM::LIST) && dv.has_cmd) {
                    JsonArray l = obj.createNestedArray("l");
                    for (uint8_t i = 0; i < dv.options_size; i++) {
                        if (!uuid::read_flash_string(dv.options[i]).empty()) {
                            l.add(uuid::read_flash_string(dv.options[i]));
                        }
                    }
                }
            }
        }
    }
}

// builds json with specific device value information
// e.g. http://ems-esp/api?device=thermostat&cmd=seltemp
bool EMSdevice::get_value_info(JsonObject & root, const char * cmd, const int8_t id) {
    JsonObject json = root;
    int8_t     tag  = id;

    // check if we have hc or wwc
    if (id >= 1 && id <= 4) {
        tag = DeviceValueTAG::TAG_HC1 + id - 1;
    } else if (id >= 8 && id <= 11) {
        tag = DeviceValueTAG::TAG_WWC1 + id - 8;
    } else if (id != -1) {
        return false;
    }

    // search device value with this tag
    for (auto & dv : devicevalues_) {
        if (strcmp(cmd, Helpers::toLower(uuid::read_flash_string(dv.short_name)).c_str()) == 0 && (tag <= 0 || tag == dv.tag)) {
            uint8_t      divider = (dv.options_size == 1) ? Helpers::atoint(uuid::read_flash_string(dv.options[0]).c_str()) : 0;
            const char * type    = "type";
            const char * min     = "min";
            const char * max     = "max";
            const char * value   = "value";

            json["name"] = dv.short_name;
            // prefix tag if it's included
            if (dv.full_name != nullptr) {
                if ((dv.tag == DeviceValueTAG::TAG_NONE) || tag_to_string(dv.tag).empty()) {
                    json["fullname"] = dv.full_name;
                } else {
                    json["fullname"] = tag_to_string(dv.tag) + " " + uuid::read_flash_string(dv.full_name);
                }
            }

            if (!tag_to_mqtt(dv.tag).empty()) {
                json["circuit"] = tag_to_mqtt(dv.tag);
            }

            switch (dv.type) {
            case DeviceValueType::ENUM: {
                if (*(uint8_t *)(dv.value_p) < dv.options_size) {
                    if (EMSESP::enum_format() == ENUM_FORMAT_NUMBER) {
                        json[value] = (uint8_t)(*(uint8_t *)(dv.value_p));
                    } else {
                        json[value] = dv.options[*(uint8_t *)(dv.value_p)]; // text
                    }
                }
                json[type] = F_(enum);
                // uint8_t min_    = (uuid::read_flash_string(dv.options[0]) == "") ? 1 : 0;
                // json[min]       = min_;
                // json[max]       = dv.options_size - 1;
                JsonArray enum_ = json.createNestedArray(F_(enum));
                for (uint8_t i = 0; i < dv.options_size; i++) {
                    enum_.add(dv.options[i]);
                }
                break;
            }

            case DeviceValueType::USHORT:
                if (Helpers::hasValue(*(uint16_t *)(dv.value_p))) {
                    json[value] = Helpers::round2(*(uint16_t *)(dv.value_p), divider);
                }
                json[type] = F_(number);
                json[min]  = 0;
                json[max]  = divider ? EMS_VALUE_USHORT_NOTSET / divider : EMS_VALUE_USHORT_NOTSET;
                break;

            case DeviceValueType::UINT:
                if (Helpers::hasValue(*(uint8_t *)(dv.value_p))) {
                    json[value] = Helpers::round2(*(uint8_t *)(dv.value_p), divider);
                }
                json[type] = F_(number);
                json[min]  = 0;
                if (dv.uom == DeviceValueUOM::PERCENT) {
                    json[max] = 100;
                } else {
                    json[max] = divider ? EMS_VALUE_UINT_NOTSET / divider : EMS_VALUE_UINT_NOTSET;
                }
                break;

            case DeviceValueType::SHORT:
                if (Helpers::hasValue(*(int16_t *)(dv.value_p))) {
                    json[value] = Helpers::round2(*(int16_t *)(dv.value_p), divider);
                }
                json[type] = F_(number);
                json[min]  = divider ? -EMS_VALUE_SHORT_NOTSET / divider : -EMS_VALUE_SHORT_NOTSET;
                json[max]  = divider ? EMS_VALUE_SHORT_NOTSET / divider : EMS_VALUE_SHORT_NOTSET;
                break;

            case DeviceValueType::INT:
                if (Helpers::hasValue(*(int8_t *)(dv.value_p))) {
                    json[value] = Helpers::round2(*(int8_t *)(dv.value_p), divider);
                }
                json[type] = F_(number);
                if (dv.uom == DeviceValueUOM::PERCENT) {
                    json[min] = -100;
                    json[max] = 100;
                } else {
                    json[min] = divider ? -EMS_VALUE_INT_NOTSET / divider : -EMS_VALUE_INT_NOTSET;
                    json[max] = divider ? EMS_VALUE_INT_NOTSET / divider : EMS_VALUE_INT_NOTSET;
                }
                break;

            case DeviceValueType::ULONG:
                if (Helpers::hasValue(*(uint32_t *)(dv.value_p))) {
                    json[value] = Helpers::round2(*(uint32_t *)(dv.value_p), divider);
                }
                json[type] = F_(number);
                json[min]  = 0;
                json[max]  = divider ? EMS_VALUE_ULONG_NOTSET / divider : EMS_VALUE_ULONG_NOTSET;
                break;

            case DeviceValueType::BOOL: {
                if (Helpers::hasValue(*(uint8_t *)(dv.value_p), EMS_VALUE_BOOL)) {
                    uint8_t bool_format = EMSESP::bool_format();
                    if (bool_format == BOOL_FORMAT_ONOFF) {
                        json[value] = (bool)(*(uint8_t *)(dv.value_p)) ? F_(on) : F_(off);
                    } else if (bool_format == BOOL_FORMAT_ONOFF_CAP) {
                        json[value] = (bool)(*(uint8_t *)(dv.value_p)) ? F_(ON) : F_(OFF);
                    } else if (bool_format == BOOL_FORMAT_TRUEFALSE) {
                        json[value] = (bool)(*(uint8_t *)(dv.value_p)) ? true : false;
                    } else {
                        json[value] = (bool)(*(uint8_t *)(dv.value_p)) ? 1 : 0;
                    }
                }
                json[type] = F("boolean");
                break;
            }

            case DeviceValueType::TIME:
                if (Helpers::hasValue(*(uint32_t *)(dv.value_p))) {
                    json[value] = (divider) ? *(uint32_t *)(dv.value_p) / divider : *(uint32_t *)(dv.value_p);
                }
                json[type] = F_(number);
                json[min]  = 0;
                json[max]  = divider ? EMS_VALUE_ULONG_NOTSET / divider : EMS_VALUE_ULONG_NOTSET;
                break;

            case DeviceValueType::TEXT:
                if (Helpers::hasValue((char *)(dv.value_p))) {
                    json[value] = (char *)(dv.value_p);
                }
                json[type] = F_(text);
                break;

            case DeviceValueType::CMD:
                json[type] = F_(command);
                break;

            default:
                json[type] = F_(unknown);
                break;
            }

            // add uom if it's not a " " (single space)
            if (!uom_to_string(dv.uom).empty() && uom_to_string(dv.uom) != " ") {
                json["uom"] = EMSdevice::uom_to_string(dv.uom);
            }

            json["writeable"] = dv.has_cmd;
            // if we have individual limits, overwrite the common limits
            if (dv.min != 0 || dv.max != 0) {
                json[min] = dv.min;
                json[max] = dv.max;
            }

            // show the HA entity name if available
            if (Mqtt::ha_enabled()) {
                json["ha_entityname"] = device_entity_ha(dv);
            }

            // if there is no value, mention it
            if (!json.containsKey("value")) {
                json[value] = "not set";
            }

            return true;
        }
    }

    return false;
}

// For each value in the device create the json object pair and add it to given json
// return false if empty
// this is used to create both the MQTT payloads and Console messages (console = true)
bool EMSdevice::generate_values_json(JsonObject & root, const uint8_t tag_filter, const bool nested, const bool console) {
    bool       has_values = false; // to see if we've added a value. it's faster than doing a json.size() at the end
    uint8_t    old_tag    = 255;   // NAN
    JsonObject json       = root;

    for (auto & dv : devicevalues_) {
        bool has_value = false;
        // only show if tag is either empty (TAG_NONE) or matches a value
        // and don't show if full_name is empty unless we're outputing for mqtt payloads
        // for nested we use all values, dont show command only (have_cmd and no fullname)
        if (((nested) || tag_filter == DeviceValueTAG::TAG_NONE || (tag_filter == dv.tag)) && (dv.full_name != nullptr || !console)
            && !(dv.full_name == nullptr && dv.has_cmd)) {
            // we have a tag if it matches the filter given, and that the tag name is not empty/""
            bool have_tag = ((dv.tag != tag_filter) && !tag_to_string(dv.tag).empty());

            char name[80];
            if (console) {
                // prefix the tag in brackets, unless it's Boiler because we're naughty and use tag for the MQTT topic
                if (have_tag) {
                    snprintf(name, 80, "%s %s", tag_to_string(dv.tag).c_str(), uuid::read_flash_string(dv.full_name).c_str());
                } else {
                    strcpy(name, uuid::read_flash_string(dv.full_name).c_str()); // use full name
                }
            } else {
                strcpy(name, uuid::read_flash_string(dv.short_name).c_str()); // use short name

                // if we have a tag, and its different to the last one create a nested object
                if (dv.tag != old_tag) {
                    old_tag = dv.tag;
                    if (nested && have_tag && dv.tag >= DeviceValueTAG::TAG_HC1) { // no nests for boiler tags
                        json = root.createNestedObject(tag_to_string(dv.tag));
                    }
                }
            }

            // handle Booleans (true, false)
            if ((dv.type == DeviceValueType::BOOL) && Helpers::hasValue(*(uint8_t *)(dv.value_p), EMS_VALUE_BOOL)) {
                // see how to render the value depending on the setting
                // when in console mode we always use on and off
                uint8_t bool_format = EMSESP::bool_format();
                if ((bool_format == BOOL_FORMAT_ONOFF) || console) {
                    json[name] = *(uint8_t *)(dv.value_p) ? F_(on) : F_(off);
                } else if (bool_format == BOOL_FORMAT_ONOFF_CAP) {
                    json[name] = *(uint8_t *)(dv.value_p) ? F_(ON) : F_(OFF);
                } else if (bool_format == BOOL_FORMAT_TRUEFALSE) {
                    json[name] = (bool)(*(uint8_t *)(dv.value_p)) ? true : false;
                } else {
                    json[name] = (uint8_t)(*(uint8_t *)(dv.value_p)) ? 1 : 0;
                }
                has_value = true;
            }

            // handle TEXT strings
            else if ((dv.type == DeviceValueType::TEXT) && (Helpers::hasValue((char *)(dv.value_p)))) {
                json[name] = (char *)(dv.value_p);
                has_value  = true;
            }

            // handle ENUMs
            else if ((dv.type == DeviceValueType::ENUM) && Helpers::hasValue(*(uint8_t *)(dv.value_p))) {
                if (*(uint8_t *)(dv.value_p) < dv.options_size) {
                    // check for numeric enum-format, but "hamode" always as text
                    if ((EMSESP::enum_format() == ENUM_FORMAT_NUMBER) && (dv.short_name != FL_(hamode)[0])) {
                        json[name] = (uint8_t)(*(uint8_t *)(dv.value_p));
                    } else {
                        json[name] = dv.options[*(uint8_t *)(dv.value_p)];
                    }
                    has_value = true;
                }
            }

            // handle Integers and Floats
            else {
                // If a divider is specified, do the division to 2 decimals places and send back as double/float
                // otherwise force as an integer whole
                // the nested if's is necessary due to the way the ArduinoJson templates are pre-processed by the compiler
                uint8_t divider = (dv.options_size == 1) ? Helpers::atoint(uuid::read_flash_string(dv.options[0]).c_str()) : 0;

                // INT
                if ((dv.type == DeviceValueType::INT) && Helpers::hasValue(*(int8_t *)(dv.value_p))) {
                    if (divider) {
                        json[name] = Helpers::round2(*(int8_t *)(dv.value_p), divider);
                    } else {
                        json[name] = *(int8_t *)(dv.value_p);
                    }
                    has_value = true;
                } else if ((dv.type == DeviceValueType::UINT) && Helpers::hasValue(*(uint8_t *)(dv.value_p))) {
                    if (divider) {
                        json[name] = Helpers::round2(*(uint8_t *)(dv.value_p), divider);
                    } else {
                        json[name] = *(uint8_t *)(dv.value_p);
                    }
                    has_value = true;
                } else if ((dv.type == DeviceValueType::SHORT) && Helpers::hasValue(*(int16_t *)(dv.value_p))) {
                    if (divider) {
                        json[name] = Helpers::round2(*(int16_t *)(dv.value_p), divider);
                    } else {
                        json[name] = *(int16_t *)(dv.value_p);
                    }
                    has_value = true;
                } else if ((dv.type == DeviceValueType::USHORT) && Helpers::hasValue(*(uint16_t *)(dv.value_p))) {
                    if (divider) {
                        json[name] = Helpers::round2(*(uint16_t *)(dv.value_p), divider);
                    } else {
                        json[name] = *(uint16_t *)(dv.value_p);
                    }
                    has_value = true;
                } else if ((dv.type == DeviceValueType::ULONG) && Helpers::hasValue(*(uint32_t *)(dv.value_p))) {
                    if (divider) {
                        json[name] = Helpers::round2(*(uint32_t *)(dv.value_p), divider);
                    } else {
                        json[name] = *(uint32_t *)(dv.value_p);
                    }
                    has_value = true;
                } else if ((dv.type == DeviceValueType::TIME) && Helpers::hasValue(*(uint32_t *)(dv.value_p))) {
                    uint32_t time_value = *(uint32_t *)(dv.value_p);
                    time_value          = (divider) ? time_value / divider : time_value; // sometimes we need to divide by 60
                    if (console) {
                        char time_s[40];
                        snprintf(time_s, sizeof(time_s), "%d days %d hours %d minutes", (time_value / 1440), ((time_value % 1440) / 60), (time_value % 60));
                        json[name] = time_s;
                    } else {
                        json[name] = time_value;
                    }
                    has_value = true;
                }
            }
        }
        dv.ha |= has_value ? DeviceValueHA::HA_VALUE : DeviceValueHA::HA_NONE;
        has_values |= has_value;
    }

    return has_values;
}

// create the Home Assistant configs for each value
// this is called when an MQTT publish is done via an EMS Device
void EMSdevice::publish_mqtt_ha_sensor() {
    for (auto & dv : devicevalues_) {
        if (dv.ha == DeviceValueHA::HA_VALUE) {
            Mqtt::publish_ha_sensor(dv.type, dv.tag, dv.full_name, device_type_, dv.short_name, dv.uom);
            dv.ha |= DeviceValueHA::HA_DONE;
        }
    }

    if (!ha_config_done()) {
        bool ok = publish_ha_config();
        ha_config_done(ok); // see if it worked
    }
}

void EMSdevice::ha_config_clear() {
    for (auto & dv : devicevalues_) {
        dv.ha = DeviceValueHA::HA_NONE; // also wait for new value
    }
    ha_config_done(false);
}

// return the name of the telegram type
const std::string EMSdevice::telegram_type_name(std::shared_ptr<const Telegram> telegram) {
    // see if it's one of the common ones, like Version
    if (telegram->type_id == EMS_TYPE_VERSION) {
        return read_flash_string(F("Version"));
    } else if (telegram->type_id == EMS_TYPE_UBADevices) {
        return read_flash_string(F("UBADevices"));
    }

    for (const auto & tf : telegram_functions_) {
        if ((tf.telegram_type_id_ == telegram->type_id) && (telegram->type_id != 0xFF)) {
            return uuid::read_flash_string(tf.telegram_type_name_);
        }
    }

    return std::string{};
}

// take a telegram_type_id and call the matching handler
// return true if match found
bool EMSdevice::handle_telegram(std::shared_ptr<const Telegram> telegram) {
    for (const auto & tf : telegram_functions_) {
        if (tf.telegram_type_id_ == telegram->type_id) {
            // if the data block is empty, assume that this telegram is not recognized by the bus master
            // so remove it from the automatic fetch list
            if (telegram->message_length == 0 && telegram->offset == 0) {
                EMSESP::logger().debug(F("This telegram (%s) is not recognized by the EMS bus"), uuid::read_flash_string(tf.telegram_type_name_).c_str());
                toggle_fetch(tf.telegram_type_id_, false);
                return false;
            }

            if (telegram->message_length > 0) {
                tf.process_function_(telegram);
            }

            return true;
        }
    }
    return false; // type not found
}

// send Tx write with a data block
void EMSdevice::write_command(const uint16_t type_id, const uint8_t offset, uint8_t * message_data, const uint8_t message_length, const uint16_t validate_typeid) {
    EMSESP::send_write_request(type_id, device_id(), offset, message_data, message_length, validate_typeid);
}

// send Tx write with a single value
void EMSdevice::write_command(const uint16_t type_id, const uint8_t offset, const uint8_t value, const uint16_t validate_typeid) {
    EMSESP::send_write_request(type_id, device_id(), offset, value, validate_typeid);
}

// send Tx write with a single value, with no post validation
void EMSdevice::write_command(const uint16_t type_id, const uint8_t offset, const uint8_t value) {
    EMSESP::send_write_request(type_id, device_id(), offset, value, 0);
}

// send Tx read command to the device
void EMSdevice::read_command(const uint16_t type_id, const uint8_t offset, const uint8_t length) {
    EMSESP::send_read_request(type_id, device_id(), offset, length);
}

} // namespace emsesp
