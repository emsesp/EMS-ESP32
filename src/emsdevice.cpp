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
// also maps to DeviceValueUOM in interface/src/project/types.ts for the Web UI
// must be an int of 4 bytes, 32bit aligned
static const __FlashStringHelper * DeviceValueUOM_s[] __attribute__((__aligned__(sizeof(uint32_t)))) PROGMEM = {

    F_(blank), F_(degrees), F_(degrees), F_(percent), F_(lmin),    F_(kwh), F_(wh),         F_(hours), F_(minutes), F_(ua),
    F_(bar),   F_(kw),      F_(w),       F_(kb),      F_(seconds), F_(dbm), F_(fahrenheit), F_(mv),    F_(sqm),     F_(oclock)};


// mapping of TAGs, to match order in DeviceValueTAG enum in emsdevice.h
// must be an int of 4 bytes, 32bit aligned
static const __FlashStringHelper * const DeviceValueTAG_s[] PROGMEM = {

    F_(tag_none),            // ""
    F_(tag_heartbeat),       // ""
    F_(tag_boiler_data),     // ""
    F_(tag_device_data_ww),  // "ww"
    F_(tag_thermostat_data), // ""
    F_(tag_hc1),             // "hc1"
    F_(tag_hc2),             // "hc2"
    F_(tag_hc3),             // "hc3"
    F_(tag_hc4),             // "hc4"
    F_(tag_hc5),             // "hc5"
    F_(tag_hc6),             // "hc6"
    F_(tag_hc7),             // "hc7"
    F_(tag_hc8),             // "hc8"
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
    F_(tag_device_data_ww_mqtt), // "ww"
    F_(tag_thermostat_data),     // ""
    F_(tag_hc1),                 // "hc1"
    F_(tag_hc2),                 // "hc2"
    F_(tag_hc3),                 // "hc3"
    F_(tag_hc4),                 // "hc4"
    F_(tag_hc5),                 // "hc5"
    F_(tag_hc6),                 // "hc6"
    F_(tag_hc7),                 // "hc7"
    F_(tag_hc8),                 // "hc8"
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

// returns number of visible device values (entries) for this device
// this includes commands since they can also be entities and visible in the web UI
uint8_t EMSdevice::count_entities() {
    uint8_t count = 0;
    for (auto & dv : devicevalues_) {
        if (dv.has_state(DeviceValueState::DV_VISIBLE) && dv.hasvalue()) {
            count++;
        }
    }
    return count;
}

// see if there are entities, excluding any commands
bool EMSdevice::has_entities() {
    for (const auto & dv : devicevalues_) {
        if (dv.type != DeviceValueType::CMD) {
            return true;
        }
    }
    return false;
}

const std::string EMSdevice::tag_to_string(uint8_t tag) {
    return read_flash_string(DeviceValueTAG_s[tag]);
}

const std::string EMSdevice::tag_to_mqtt(uint8_t tag) {
    return read_flash_string(DeviceValueTAG_mqtt[tag]);
}

const std::string EMSdevice::uom_to_string(uint8_t uom) {
    return read_flash_string(DeviceValueUOM_s[uom]);
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

    case DeviceType::ANALOGSENSOR:
        return read_flash_string(F_(analogsensor));
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
    if (!topic) {
        return DeviceType::UNKNOWN; // nullptr
    }

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

    if (!strcmp(lowtopic, reinterpret_cast<PGM_P>(F_(analogsensor)))) {
        return DeviceType::ANALOGSENSOR;
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
    // for devices that haven't been lookup yet, don't show all details
    if (product_id_ == 0) {
        return name_ + " (DeviceID:" + Helpers::hextoa(device_id_) + ")";
    }

    if (brand_ == Brand::NO_BRAND) {
        return name_ + " (DeviceID:" + Helpers::hextoa(device_id_) + ", ProductID:" + Helpers::itoa(product_id_) + ", Version:" + version_ + ")";
    }

    return brand_to_string() + " " + name_ + " (DeviceID:" + Helpers::hextoa(device_id_) + ", ProductID:" + Helpers::itoa(product_id_) + ", Version:" + version_
           + ")";
}

// returns out brand + device name
const std::string EMSdevice::to_string_short() const {
    if (brand_ == Brand::NO_BRAND) {
        return device_type_name() + ": " + name_;
    }

    return device_type_name() + ": " + brand_to_string() + " " + name_;
}

// for each telegram that has the fetch value set (true) do a read request
void EMSdevice::fetch_values() {
    EMSESP::logger().debug(F("Fetching values for deviceID 0x%02X"), device_id());

    for (const auto & tf : telegram_functions_) {
        if (tf.fetch_) {
            read_command(tf.telegram_type_id_);
        }
    }
}

// toggle on/off automatic fetch for a telegramID
void EMSdevice::toggle_fetch(uint16_t telegram_id, bool toggle) {
    EMSESP::logger().debug(F("Toggling fetch for deviceID 0x%02X, telegramID 0x%02X to %d"), device_id(), telegram_id, toggle);

    for (auto & tf : telegram_functions_) {
        if (tf.telegram_type_id_ == telegram_id) {
            tf.fetch_ = toggle;
        }
    }
}

// get status of automatic fetch for a telegramID
bool EMSdevice::is_fetch(uint16_t telegram_id) {
    for (auto & tf : telegram_functions_) {
        if (tf.telegram_type_id_ == telegram_id) {
            return tf.fetch_;
        }
    }
    return false;
}

// list of registered device entries
// called from the command 'entities'
void EMSdevice::list_device_entries(JsonObject & output) {
    for (const auto & dv : devicevalues_) {
        if (dv.has_state(DeviceValueState::DV_VISIBLE) && dv.type != DeviceValueType::CMD) {
            // if we have a tag prefix it
            char key[50];
            if (!EMSdevice::tag_to_mqtt(dv.tag).empty()) {
                snprintf(key, sizeof(key), "%s.%s", EMSdevice::tag_to_mqtt(dv.tag).c_str(), read_flash_string(dv.short_name).c_str());
            } else {
                snprintf(key, sizeof(key), "%s", read_flash_string(dv.short_name).c_str());
            }

            JsonArray details = output.createNestedArray(key);

            // add the full name description
            details.add(dv.full_name);

            // add uom
            if (!uom_to_string(dv.uom).empty() && uom_to_string(dv.uom) != " ") {
                if (EMSESP::system_.fahrenheit() && (dv.uom == DeviceValueUOM::DEGREES || dv.uom == DeviceValueUOM::DEGREES_R)) {
                    details.add(EMSdevice::uom_to_string(DeviceValueUOM::FAHRENHEIT));
                } else {
                    details.add(EMSdevice::uom_to_string(dv.uom));
                }
            }
        }
    }
}

// list all the telegram type IDs for this device
void EMSdevice::show_telegram_handlers(uuid::console::Shell & shell) {
    if (telegram_functions_.size() == 0) {
        return;
    }

    shell.printf(F(" This %s will listen to telegram type IDs: "), device_type_name().c_str());
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

// register a callback function for a specific telegram type
void EMSdevice::register_telegram_type(const uint16_t telegram_type_id, const __FlashStringHelper * telegram_type_name, bool fetch, const process_function_p f) {
    telegram_functions_.emplace_back(telegram_type_id, telegram_type_name, fetch, false, f);
}

// add to device value library, also know now as a "device entity"
// arguments are:
//  tag: to be used to group mqtt together, either as separate topics as a nested object
//  value_p: pointer to the value from the .h file
//  type: one of DeviceValueType
//  options: options for enum or a divider for int (e.g. F("10"))
//  short_name: used in Mqtt as keys
//  full_name: used in Web and Console unless empty (nullptr)
//  uom: unit of measure from DeviceValueUOM
//  has_cmd: true if this is an associated command
//  min: min allowed value
//  max: max allowed value
void EMSdevice::register_device_value(uint8_t                             tag,
                                      void *                              value_p,
                                      uint8_t                             type,
                                      const __FlashStringHelper * const * options,
                                      const __FlashStringHelper *         short_name,
                                      const __FlashStringHelper *         full_name,
                                      uint8_t                             uom,
                                      bool                                has_cmd,
                                      int16_t                             min,
                                      uint16_t                            max) {
    // initialize the device value depending on it's type
    if (type == DeviceValueType::STRING) {
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

    // this is the unique id set for the device entity. it's a simple sequence number
    uint8_t dv_id = get_next_dv_id();

    // determine state
    uint8_t state = DeviceValueState::DV_VISIBLE; // default to visible
    if (!full_name) {
        state = DeviceValueState::DV_DEFAULT; // don't show if the full_name is empty
    } else {
        // scan through customizations to see if it's on the exclusion list by matching the productID and deviceID
        EMSESP::webCustomizationService.read([&](WebCustomization & settings) {
            for (EntityCustomization entityCustomization : settings.entityCustomizations) {
                if ((entityCustomization.product_id == product_id()) && (entityCustomization.device_id == device_id())) {
                    for (uint8_t entity_id : entityCustomization.entity_ids) {
                        if (entity_id == dv_id) {
                            state = DeviceValueState::DV_DEFAULT; // it's on the list, exclude it by making it not visible
                            break;
                        }
                    }
                }
            }
        });
    }

    // add the device
    devicevalues_.emplace_back(device_type_, tag, value_p, type, options, options_size, short_name, full_name, uom, 0, has_cmd, min, max, state, dv_id);
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
                                      int16_t                             min,
                                      uint16_t                            max) {
    auto short_name = name[0];
    auto full_name  = name[1];

    register_device_value(tag, value_p, type, options, short_name, full_name, uom, (f != nullptr), min, max);

    // add a new command if it has a function attached
    if (f == nullptr) {
        return;
    }

    uint8_t flags = CommandFlag::ADMIN_ONLY; // executing commands require admin privileges

    if (tag >= DeviceValueTAG::TAG_HC1 && tag <= DeviceValueTAG::TAG_HC8) {
        flags |= CommandFlag::MQTT_SUB_FLAG_HC;
    } else if (tag >= DeviceValueTAG::TAG_WWC1 && tag <= DeviceValueTAG::TAG_WWC4) {
        flags |= CommandFlag::MQTT_SUB_FLAG_WWC;
    } else if (tag == DeviceValueTAG::TAG_DEVICE_DATA_WW) {
        flags |= CommandFlag::MQTT_SUB_FLAG_WW;
    }

    // add the command to our library
    // cmd is the short_name and the description is the full_name
    Command::add(device_type_, short_name, f, full_name, flags);
}

// function with no min and max values (set to 0)
void EMSdevice::register_device_value(uint8_t                             tag,
                                      void *                              value_p,
                                      uint8_t                             type,
                                      const __FlashStringHelper * const * options,
                                      const __FlashStringHelper * const * name,
                                      uint8_t                             uom,
                                      const cmd_function_p                f) {
    register_device_value(tag, value_p, type, options, name, uom, f, 0, 0);
}

// no associated command function, or min/max values
void EMSdevice::register_device_value(uint8_t                             tag,
                                      void *                              value_p,
                                      uint8_t                             type,
                                      const __FlashStringHelper * const * options,
                                      const __FlashStringHelper * const * name,
                                      uint8_t                             uom) {
    register_device_value(tag, value_p, type, options, name, uom, nullptr, 0, 0);
}

// publish a single value on change
void EMSdevice::publish_value(void * value_p) {
    if (!Mqtt::publish_single() || value_p == nullptr) {
        return;
    }
    for (auto & dv : devicevalues_) {
        if (dv.value_p == value_p) {
            char topic[Mqtt::MQTT_TOPIC_MAX_SIZE];
            if ((dv.tag >= DeviceValueTAG::TAG_HC1 && dv.tag <= DeviceValueTAG::TAG_HC8)
                || (dv.tag >= DeviceValueTAG::TAG_WWC1 && dv.tag <= DeviceValueTAG::TAG_WWC4)) {
                snprintf(topic,
                         sizeof(topic),
                         "%s/%s/%s",
                         device_type_2_device_name(device_type_).c_str(),
                         tag_to_mqtt(dv.tag).c_str(),
                         read_flash_string(dv.short_name).c_str());
            } else {
                snprintf(topic, sizeof(topic), "%s/%s", device_type_2_device_name(device_type_).c_str(), read_flash_string(dv.short_name).c_str());
            }

            int8_t  divider     = (dv.options_size == 1) ? Helpers::atoint(read_flash_string(dv.options[0]).c_str()) : 0;
            char    payload[30] = {'\0'};
            uint8_t fahrenheit  = !EMSESP::system_.fahrenheit() ? 0 : (dv.uom == DeviceValueUOM::DEGREES) ? 2 : (dv.uom == DeviceValueUOM::DEGREES_R) ? 1 : 0;

            switch (dv.type) {
            case DeviceValueType::CMD: // publish a dummy value to show subscription in mqtt
                strlcpy(payload, "-", 2);
                break;
            case DeviceValueType::ENUM: {
                if ((*(uint8_t *)(value_p)) < dv.options_size) {
                    if (EMSESP::system_.enum_format() == ENUM_FORMAT_INDEX) {
                        Helpers::render_value(payload, *(uint8_t *)(value_p), 0);
                    } else {
                        strlcpy(payload, read_flash_string(dv.options[*(uint8_t *)(value_p)]).c_str(), sizeof(payload));
                    }
                }
                break;
            }

            case DeviceValueType::USHORT:
                Helpers::render_value(payload, *(uint16_t *)(value_p), divider, fahrenheit);
                break;
            case DeviceValueType::UINT:
                Helpers::render_value(payload, *(uint8_t *)(value_p), divider, fahrenheit);
                break;
            case DeviceValueType::SHORT:
                Helpers::render_value(payload, *(int16_t *)(value_p), divider, fahrenheit);
                break;
            case DeviceValueType::INT:
                Helpers::render_value(payload, *(int8_t *)(value_p), divider, fahrenheit);
                break;
            case DeviceValueType::ULONG:
                Helpers::render_value(payload, *(uint32_t *)(value_p), divider, fahrenheit);
                break;
            case DeviceValueType::BOOL: {
                Helpers::render_boolean(payload, (bool)(*(uint8_t *)(value_p)));
                break;
            }
            case DeviceValueType::TIME:
                Helpers::render_value(payload, *(uint32_t *)(value_p), divider);
                break;
            case DeviceValueType::STRING:
            default:
                if (Helpers::hasValue((char *)(value_p))) {
                    strlcpy(payload, (char *)(value_p), sizeof(payload));
                }
                break;
            }

            if (payload[0] != '\0') {
                Mqtt::publish(topic, payload);
            }
        }
    }
}

// looks up the UOM for a given key from the device value table
const std::string EMSdevice::get_value_uom(const char * key) {
    // the key may have a TAG string prefixed at the beginning. If so, remove it
    char new_key[80];
    strlcpy(new_key, key, sizeof(new_key));
    char * key_p = new_key;

    size_t sz = sizeof(DeviceValueTAG_s) / sizeof(__FlashStringHelper *);
    for (uint8_t i = 0; i < sz; i++) {
        auto tag = read_flash_string(DeviceValueTAG_s[i]);
        if (!tag.empty()) {
            std::string key2 = key; // copy char to a std::string
            if ((key2.find(tag) != std::string::npos) && (key[tag.length()] == ' ')) {
                key_p += tag.length() + 1; // remove the tag
                break;
            }
        }
    }

    // look up key in our device value list
    for (const auto & dv : devicevalues_) {
        if (dv.has_state(DeviceValueState::DV_VISIBLE)) {
            if (read_flash_string(dv.full_name) == key_p) {
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
// this is loosely based of the function generate_values used for the MQTT and Console
// except additional data is stored in the JSON document needed for the Web UI like the UOM and command
// v = value, u=uom, n=name, c=cmd
void EMSdevice::generate_values_web(JsonObject & output) {
    output["label"] = to_string_short();
    JsonArray data  = output.createNestedArray("data");

    for (auto & dv : devicevalues_) {
        // check conditions:
        //  1. full_name cannot be empty
        //  2. it must have a valid value, if it is not a command like 'reset'

        if (dv.has_state(DeviceValueState::DV_VISIBLE) && (dv.hasvalue() || (dv.type == DeviceValueType::CMD))) {
            JsonObject obj        = data.createNestedObject(); // create the object, we know there is a value
            uint8_t    fahrenheit = 0;

            // handle Booleans (true, false)
            if (dv.type == DeviceValueType::BOOL) {
                bool value_b = *(bool *)(dv.value_p);
                if ((EMSESP::system_.bool_format() == BOOL_FORMAT_TRUEFALSE)) {
                    obj["v"] = value_b ? "true" : "false";
                } else if ((EMSESP::system_.bool_format() == BOOL_FORMAT_10)) {
                    obj["v"] = value_b ? 1 : 0;
                } else {
                    char s[7];
                    obj["v"] = Helpers::render_boolean(s, value_b);
                }
            }

            // handle TEXT strings
            else if (dv.type == DeviceValueType::STRING) {
                obj["v"] = (char *)(dv.value_p);
            }

            // handle ENUMs
            else if ((dv.type == DeviceValueType::ENUM) && (*(uint8_t *)(dv.value_p) < dv.options_size)) {
                obj["v"] = dv.options[*(uint8_t *)(dv.value_p)];
            }

            // handle commands without value
            // else if (dv.type == DeviceValueType::CMD) {
            //     obj["v"] = "-";
            // }

            else {
                // If a divider is specified, do the division to 2 decimals places and send back as double/float
                // otherwise force as an integer whole
                // the nested if's is necessary due to the way the ArduinoJson templates are pre-processed by the compiler
                int8_t divider = (dv.options_size == 1) ? Helpers::atoint(read_flash_string(dv.options[0]).c_str()) : 0;
                fahrenheit     = !EMSESP::system_.fahrenheit() ? 0 : (dv.uom == DeviceValueUOM::DEGREES) ? 2 : (dv.uom == DeviceValueUOM::DEGREES_R) ? 1 : 0;

                if (dv.type == DeviceValueType::INT) {
                    obj["v"] = Helpers::round2(*(int8_t *)(dv.value_p), divider, fahrenheit);
                } else if ((dv.type == DeviceValueType::UINT) && Helpers::hasValue(*(uint8_t *)(dv.value_p))) {
                    obj["v"] = Helpers::round2(*(uint8_t *)(dv.value_p), divider, fahrenheit);
                } else if ((dv.type == DeviceValueType::SHORT) && Helpers::hasValue(*(int16_t *)(dv.value_p))) {
                    obj["v"] = Helpers::round2(*(int16_t *)(dv.value_p), divider, fahrenheit);
                } else if ((dv.type == DeviceValueType::USHORT) && Helpers::hasValue(*(uint16_t *)(dv.value_p))) {
                    obj["v"] = Helpers::round2(*(uint16_t *)(dv.value_p), divider, fahrenheit);
                } else if ((dv.type == DeviceValueType::ULONG) && Helpers::hasValue(*(uint32_t *)(dv.value_p))) {
                    obj["v"] = Helpers::round2(*(uint32_t *)(dv.value_p), divider, fahrenheit);
                } else if ((dv.type == DeviceValueType::TIME) && Helpers::hasValue(*(uint32_t *)(dv.value_p))) {
                    uint32_t time_value = *(uint32_t *)(dv.value_p);
                    obj["v"]            = (divider > 0) ? time_value / divider : time_value; // sometimes we need to divide by 60
                }
            }

            // add the unit of measure (uom)
            obj["u"] = fahrenheit ? (uint8_t)DeviceValueUOM::FAHRENHEIT : dv.uom;

            // add name, prefixing the tag if it exists
            if ((dv.tag == DeviceValueTAG::TAG_NONE) || tag_to_string(dv.tag).empty()) {
                obj["n"] = dv.full_name;
            } else if (dv.tag < DeviceValueTAG::TAG_HC1) {
                obj["n"] = tag_to_string(dv.tag) + " " + read_flash_string(dv.full_name);
            } else {
                obj["n"] = tag_to_string(dv.tag) + " " + read_flash_string(dv.full_name);
            }

            // add commands and options
            if (dv.has_cmd) {
                // add the name of the Command function
                if (dv.tag >= DeviceValueTAG::TAG_HC1) {
                    obj["c"] = tag_to_mqtt(dv.tag) + "/" + read_flash_string(dv.short_name);
                } else {
                    obj["c"] = dv.short_name;
                }
                // add the Command options
                if (dv.type == DeviceValueType::ENUM || (dv.type == DeviceValueType::CMD && dv.options_size > 1)) {
                    JsonArray l = obj.createNestedArray("l");
                    for (uint8_t i = 0; i < dv.options_size; i++) {
                        if (!read_flash_string(dv.options[i]).empty()) {
                            l.add(read_flash_string(dv.options[i]));
                        }
                    }
                }
                if (dv.type == DeviceValueType::BOOL) {
                    JsonArray l = obj.createNestedArray("l");
                    l.add("off");
                    l.add("on");
                }
                // add command template
                if ((dv.type == DeviceValueType::STRING || dv.type == DeviceValueType::CMD) && dv.options_size == 1) {
                    obj["o"] = dv.options[0];
                }
            }
        }
    }
}

// disable/exclude a device entity based on its unique id
void EMSdevice::exclude_entity(uint8_t id) {
    for (auto & dv : devicevalues_) {
        if (dv.id == id) {
            dv.remove_state(DeviceValueState::DV_VISIBLE); // this will remove from MQTT payloads and showing in web & console
            dv.remove_state(DeviceValueState::DV_ACTIVE);  // this will ensure it'll be removed from any HA MQTT /config topics
            return;
        }
    }
}

// as generate_values_web() but stripped down to only show all entities and their state
void EMSdevice::generate_values_web_all(JsonArray & output) {
    for (auto & dv : devicevalues_) {
        // ignore commands and entities that have an empty full name
        if ((dv.type != DeviceValueType::CMD) && (dv.full_name)) {
            JsonObject obj = output.createNestedObject();

            // create the value
            if (dv.hasvalue()) {
                // handle Booleans (true, false)
                if (dv.type == DeviceValueType::BOOL) {
                    bool value_b = *(bool *)(dv.value_p);
                    if ((EMSESP::system_.bool_format() == BOOL_FORMAT_TRUEFALSE)) {
                        obj["v"] = value_b;
                    } else if ((EMSESP::system_.bool_format() == BOOL_FORMAT_10)) {
                        obj["v"] = value_b ? 1 : 0;
                    } else {
                        char s[7];
                        obj["v"] = Helpers::render_boolean(s, value_b);
                    }
                }

                // handle TEXT strings
                else if (dv.type == DeviceValueType::STRING) {
                    obj["v"] = (char *)(dv.value_p);
                }

                // handle ENUMs
                else if ((dv.type == DeviceValueType::ENUM) && (*(uint8_t *)(dv.value_p) < dv.options_size)) {
                    obj["v"] = dv.options[*(uint8_t *)(dv.value_p)];
                }

                // handle Integers and Floats
                else {
                    // If a divider is specified, do the division to 2 decimals places and send back as double/float
                    // otherwise force as an integer whole
                    // the nested if's is necessary due to the way the ArduinoJson templates are pre-processed by the compiler
                    uint8_t divider = 0;
                    uint8_t factor  = 1;
                    if (dv.options_size == 1) {
                        const char * s = read_flash_string(dv.options[0]).c_str();
                        if (s[0] == '*') {
                            factor = Helpers::atoint(&s[1]);
                        } else {
                            divider = Helpers::atoint(s);
                        }
                    }

                    if (dv.type == DeviceValueType::INT) {
                        obj["v"] = (divider) ? Helpers::round2(*(int8_t *)(dv.value_p), divider) : *(int8_t *)(dv.value_p) * factor;
                    } else if (dv.type == DeviceValueType::UINT) {
                        obj["v"] = (divider) ? Helpers::round2(*(uint8_t *)(dv.value_p), divider) : *(uint8_t *)(dv.value_p) * factor;
                    } else if (dv.type == DeviceValueType::SHORT) {
                        obj["v"] = (divider) ? Helpers::round2(*(int16_t *)(dv.value_p), divider) : *(int16_t *)(dv.value_p) * factor;
                    } else if (dv.type == DeviceValueType::USHORT) {
                        obj["v"] = (divider) ? Helpers::round2(*(uint16_t *)(dv.value_p), divider) : *(uint16_t *)(dv.value_p) * factor;
                    } else if (dv.type == DeviceValueType::ULONG) {
                        obj["v"] = divider ? Helpers::round2(*(uint32_t *)(dv.value_p), divider) : *(uint32_t *)(dv.value_p) * factor;
                    } else if (dv.type == DeviceValueType::TIME) {
                        uint32_t time_value = *(uint32_t *)(dv.value_p);
                        obj["v"]            = (divider > 0) ? time_value / divider : time_value * factor; // sometimes we need to divide by 60
                    }
                }
            }

            // add name, prefixing the tag if it exists
            if ((dv.tag == DeviceValueTAG::TAG_NONE) || tag_to_string(dv.tag).empty()) {
                obj["n"] = dv.full_name;
            } else {
                char name[50];
                snprintf(name, sizeof(name), "%s %s", tag_to_string(dv.tag).c_str(), read_flash_string(dv.full_name).c_str());
                obj["n"] = name;
            }

            // shortname
            if (dv.tag >= DeviceValueTAG::TAG_HC1) {
                obj["s"] = tag_to_string(dv.tag) + "/" + read_flash_string(dv.short_name);
            } else {
                obj["s"] = dv.short_name;
            }

            // is it marked as excluded?
            obj["x"] = !dv.has_state(DeviceValueState::DV_VISIBLE);

            // add the unique ID
            obj["i"] = dv.id;
        }
    }
}

// builds json for a specific device value / entity
// cmd is the endpoint or name of the device entity
// returns false if failed, otherwise true
bool EMSdevice::get_value_info(JsonObject & output, const char * cmd, const int8_t id) {
    JsonObject json = output;
    int8_t     tag  = id;

    // check if we have hc or wwc
    if (id >= 1 && id <= 8) {
        tag = DeviceValueTAG::TAG_HC1 + id - 1;
    } else if (id >= 9 && id <= 12) {
        tag = DeviceValueTAG::TAG_WWC1 + id - 9;
    } else if (id != -1) {
        return false; // error
    }

    // search device value with this tag
    for (auto & dv : devicevalues_) {
        if (strcmp(cmd, Helpers::toLower(read_flash_string(dv.short_name)).c_str()) == 0 && (tag <= 0 || tag == dv.tag)) {
            int8_t       divider = (dv.options_size == 1) ? Helpers::atoint(read_flash_string(dv.options[0]).c_str()) : 0;
            const char * type    = "type";
            const char * min     = "min";
            const char * max     = "max";
            const char * value   = "value";
            uint8_t fahrenheit   = !EMSESP::system_.fahrenheit() ? 0 : (dv.uom == DeviceValueUOM::DEGREES) ? 2 : (dv.uom == DeviceValueUOM::DEGREES_R) ? 1 : 0;

            json["name"] = dv.short_name;

            if (dv.full_name != nullptr) {
                const char * fullname = "fullname";
                if ((dv.tag == DeviceValueTAG::TAG_NONE) || tag_to_string(dv.tag).empty()) {
                    json[fullname] = dv.full_name;
                } else {
                    json[fullname] = tag_to_string(dv.tag) + " " + read_flash_string(dv.full_name);
                }
            }

            if (!tag_to_mqtt(dv.tag).empty()) {
                json["circuit"] = tag_to_mqtt(dv.tag);
            }

            switch (dv.type) {
            case DeviceValueType::ENUM: {
                if (*(uint8_t *)(dv.value_p) < dv.options_size) {
                    if (EMSESP::system_.enum_format() == ENUM_FORMAT_INDEX) {
                        json[value] = (uint8_t)(*(uint8_t *)(dv.value_p));
                    } else {
                        json[value] = dv.options[*(uint8_t *)(dv.value_p)]; // text
                    }
                }
                json[type]      = F_(enum);
                JsonArray enum_ = json.createNestedArray(F_(enum));
                for (uint8_t i = 0; i < dv.options_size; i++) {
                    enum_.add(dv.options[i]);
                }
                break;
            }

            case DeviceValueType::USHORT:
                if (Helpers::hasValue(*(uint16_t *)(dv.value_p))) {
                    json[value] = Helpers::round2(*(uint16_t *)(dv.value_p), divider, fahrenheit);
                }
                json[type] = F_(number);
                json[min]  = 0;
                json[max]  = Helpers::round2(EMS_VALUE_USHORT_NOTSET, divider);
                break;

            case DeviceValueType::UINT:
                if (Helpers::hasValue(*(uint8_t *)(dv.value_p))) {
                    json[value] = Helpers::round2(*(uint8_t *)(dv.value_p), divider, fahrenheit);
                }
                json[type] = F_(number);
                json[min]  = 0;
                if (dv.uom == DeviceValueUOM::PERCENT) {
                    json[max] = 100;
                } else {
                    json[max] = Helpers::round2(EMS_VALUE_UINT_NOTSET, divider);
                }
                break;

            case DeviceValueType::SHORT:
                if (Helpers::hasValue(*(int16_t *)(dv.value_p))) {
                    json[value] = Helpers::round2(*(int16_t *)(dv.value_p), divider, fahrenheit);
                }
                json[type] = F_(number);
                json[min]  = Helpers::round2(-EMS_VALUE_SHORT_NOTSET, divider);
                json[max]  = Helpers::round2(EMS_VALUE_SHORT_NOTSET, divider);
                break;

            case DeviceValueType::INT:
                if (Helpers::hasValue(*(int8_t *)(dv.value_p))) {
                    json[value] = Helpers::round2(*(int8_t *)(dv.value_p), divider, fahrenheit);
                }
                json[type] = F_(number);
                if (dv.uom == DeviceValueUOM::PERCENT) {
                    json[min] = -100;
                    json[max] = 100;
                } else {
                    json[min] = Helpers::round2(-EMS_VALUE_INT_NOTSET, divider);
                    json[max] = Helpers::round2(EMS_VALUE_INT_NOTSET, divider);
                }
                break;

            case DeviceValueType::ULONG:
                if (Helpers::hasValue(*(uint32_t *)(dv.value_p))) {
                    json[value] = Helpers::round2(*(uint32_t *)(dv.value_p), divider, fahrenheit);
                }
                json[type] = F_(number);
                json[min]  = 0;
                json[max]  = Helpers::round2(EMS_VALUE_ULONG_NOTSET, divider);
                break;

            case DeviceValueType::BOOL:
                if (Helpers::hasValue(*(uint8_t *)(dv.value_p), EMS_VALUE_BOOL)) {
                    bool value_b = (bool)(*(uint8_t *)(dv.value_p));
                    if ((EMSESP::system_.bool_format() == BOOL_FORMAT_TRUEFALSE)) {
                        json[value] = value_b;
                    } else if ((EMSESP::system_.bool_format() == BOOL_FORMAT_10)) {
                        json[value] = value_b ? 1 : 0;
                    } else {
                        char s[7];
                        json[value] = Helpers::render_boolean(s, value_b);
                    }
                }
                json[type] = F("boolean");
                break;

            case DeviceValueType::TIME:
                if (Helpers::hasValue(*(uint32_t *)(dv.value_p))) {
                    json[value] = Helpers::round2(*(uint32_t *)(dv.value_p), divider);
                }
                json[type] = F_(number);
                json[min]  = 0;
                json[max]  = Helpers::round2(EMS_VALUE_ULONG_NOTSET, divider);
                break;

            case DeviceValueType::STRING:
                if (Helpers::hasValue((char *)(dv.value_p))) {
                    json[value] = (char *)(dv.value_p);
                }
                json[type] = F("string");
                break;

            case DeviceValueType::CMD:
                json[type] = F_(command);
                if (dv.options_size > 1) {
                    JsonArray enum_ = json.createNestedArray(F_(enum));
                    for (uint8_t i = 0; i < dv.options_size; i++) {
                        enum_.add(dv.options[i]);
                    }
                }
                break;

            default:
                json[type] = F_(unknown);
                break;
            }

            // add uom if it's not a " " (single space)
            if (!uom_to_string(dv.uom).empty() && uom_to_string(dv.uom) != " ") {
                json["uom"] = fahrenheit ? "°F" : uom_to_string(dv.uom);
            }

            json["writeable"] = dv.has_cmd;
            // if we have individual limits, overwrite the common limits
            // limits are not scaled with divider and temperatures are °C
            if (dv.min != 0 || dv.max != 0) {
                json[min] = Helpers::round2(dv.min, 0, fahrenheit);
                json[max] = Helpers::round2(dv.max, 0, fahrenheit);
            }

            json["visible"] = dv.has_state(DeviceValue::DV_VISIBLE);

            // if there is no value, mention it
            if (!json.containsKey(value)) {
                json[value] = "not set";
            }

            return true;
        }
    }

    char error[100];
    snprintf(error, sizeof(error), "cannot find values for entity '%s'", cmd);
    json["message"] = error;

    return false;
}

// mqtt publish all single values from one device (used for time schedule)
void EMSdevice::publish_all_values() {
    for (auto & dv : devicevalues_) {
        publish_value(dv.value_p);
    }
}

// For each value in the device create the json object pair and add it to given json
// return false if empty
// this is used to create both the MQTT payloads, Console messages and Web API calls
bool EMSdevice::generate_values(JsonObject & output, const uint8_t tag_filter, const bool nested, const uint8_t output_target) {
    bool       has_values = false; // to see if we've added a value. it's faster than doing a json.size() at the end
    uint8_t    old_tag    = 255;   // NAN
    JsonObject json       = output;

    for (auto & dv : devicevalues_) {
        // check if it exists, there is a value for the entity. Set the flag to ACTIVE
        if (dv.hasvalue()) {
            dv.add_state(DeviceValueState::DV_ACTIVE);
        } else {
            dv.remove_state(DeviceValueState::DV_ACTIVE);
        }

        // check conditions:
        //  1. it must have a valid value (state is active)
        //  2. it must have a visible, unless the output_target is MQTT
        //  3. it must match the given tag filter or have an empty tag
        bool conditions = ((tag_filter == DeviceValueTAG::TAG_NONE) || (tag_filter == dv.tag)) && dv.has_state(DeviceValueState::DV_ACTIVE);
        //  4. for MQTT we want to always show the special HA entities (they have an empty fullname)
        bool visible = ((dv.has_state(DeviceValueState::DV_VISIBLE)) || ((output_target == OUTPUT_TARGET::MQTT) && (!dv.full_name)));
        conditions &= visible;

        if (conditions) {
            has_values = true; // flagged if we actually have data

            // we have a tag if it matches the filter given, and that the tag name is not empty/""
            bool have_tag = ((dv.tag != tag_filter) && !tag_to_string(dv.tag).empty());

            // create the name for the JSON key
            char name[80];
            if (output_target == OUTPUT_TARGET::API_VERBOSE) {
                if (have_tag) {
                    snprintf(name, 80, "%s %s", tag_to_string(dv.tag).c_str(), read_flash_string(dv.full_name).c_str()); // prefix the tag
                } else {
                    strcpy(name, read_flash_string(dv.full_name).c_str()); // use full name
                }
            } else {
                strcpy(name, read_flash_string(dv.short_name).c_str()); // use short name

                // if we have a tag, and its different to the last one create a nested object. only for hc, wwc and hs
                if (dv.tag != old_tag) {
                    old_tag = dv.tag;
                    if (nested && have_tag && dv.tag >= DeviceValueTAG::TAG_HC1) {
                        json = output.createNestedObject(tag_to_string(dv.tag));
                    }
                }
            }

            // handle Booleans
            if (dv.type == DeviceValueType::BOOL && Helpers::hasValue(*(uint8_t *)(dv.value_p), EMS_VALUE_BOOL)) {
                // see how to render the value depending on the setting
                bool value_b = (bool)*(uint8_t *)(dv.value_p);
                if (Mqtt::ha_enabled()) {
                    char s[7];
                    json[name] = Helpers::render_boolean(s, value_b); // for HA always render as string
                } else {
                    if ((EMSESP::system_.bool_format() == BOOL_FORMAT_TRUEFALSE)) {
                        json[name] = value_b;
                    } else if ((EMSESP::system_.bool_format() == BOOL_FORMAT_10)) {
                        json[name] = value_b ? 1 : 0;
                    } else {
                        char s[7];
                        json[name] = Helpers::render_boolean(s, value_b);
                    }
                }
            }

            // handle TEXT strings
            else if (dv.type == DeviceValueType::STRING) {
                json[name] = (char *)(dv.value_p);
            }

            // handle ENUMs
            else if ((dv.type == DeviceValueType::ENUM) && (*(uint8_t *)(dv.value_p) < dv.options_size)) {
                // check for numeric enum-format
                if ((EMSESP::system_.enum_format() == ENUM_FORMAT_INDEX)) {
                    json[name] = (uint8_t)(*(uint8_t *)(dv.value_p));
                } else {
                    json[name] = dv.options[*(uint8_t *)(dv.value_p)];
                }
            }

            // handle Numbers
            // If a divider is specified, do the division to 2 decimals places and send back as double/float
            // otherwise force as a whole integer
            // note: the strange nested if's is necessary due to the way the ArduinoJson templates are pre-processed by the compiler
            else {
                // If a divider is specified, do the division to 2 decimals places and send back as double/float
                // otherwise force as an integer whole
                uint8_t divider = 0;
                uint8_t factor  = 1;
                if (dv.options_size == 1) {
                    const char * s = read_flash_string(dv.options[0]).c_str();
                    if (s[0] == '*') {
                        factor = Helpers::atoint(&s[1]);
                    } else {
                        divider = Helpers::atoint(s);
                    }
                }

                // fahrenheit, 0 is no converstion other 1 or 2. not sure why?
                uint8_t fahrenheit = !EMSESP::system_.fahrenheit()           ? 0
                                     : (dv.uom == DeviceValueUOM::DEGREES)   ? 2
                                     : (dv.uom == DeviceValueUOM::DEGREES_R) ? 1
                                                                             : 0;

                // always convert temperatures to floats with 1 decimal place
                bool make_float = (divider || (dv.uom == DeviceValueUOM::DEGREES) || (dv.uom == DeviceValueUOM::DEGREES_R));

                if (dv.type == DeviceValueType::INT) {
                    if (make_float) {
                        json[name] = Helpers::round2(*(int8_t *)(dv.value_p), divider, fahrenheit);
                    } else {
                        json[name] = *(int8_t *)(dv.value_p) * factor;
                    }
                } else if (dv.type == DeviceValueType::UINT) {
                    if (make_float) {
                        json[name] = Helpers::round2(*(uint8_t *)(dv.value_p), divider, fahrenheit);
                    } else {
                        json[name] = *(uint8_t *)(dv.value_p) * factor;
                    }
                } else if (dv.type == DeviceValueType::SHORT) {
                    if (make_float) {
                        json[name] = Helpers::round2(*(int16_t *)(dv.value_p), divider, fahrenheit);
                    } else {
                        json[name] = *(int16_t *)(dv.value_p) * factor;
                    }
                } else if (dv.type == DeviceValueType::USHORT) {
                    if (make_float) {
                        json[name] = Helpers::round2(*(uint16_t *)(dv.value_p), divider, fahrenheit);
                    } else {
                        json[name] = *(uint16_t *)(dv.value_p) * factor;
                    }
                } else if (dv.type == DeviceValueType::ULONG) {
                    if (make_float) {
                        json[name] = Helpers::round2(*(uint32_t *)(dv.value_p), divider, fahrenheit);
                    } else {
                        json[name] = *(uint32_t *)(dv.value_p) * factor;
                    }
                } else if ((dv.type == DeviceValueType::TIME) && Helpers::hasValue(*(uint32_t *)(dv.value_p))) {
                    uint32_t time_value = *(uint32_t *)(dv.value_p);
                    time_value          = Helpers::round2(time_value, divider); // sometimes we need to divide by 60
                    if (output_target == EMSdevice::OUTPUT_TARGET::API_VERBOSE) {
                        char time_s[40];
                        snprintf(time_s,
                                 sizeof(time_s),
                                 "%d %s %d %s %d %s",
                                 (time_value / 1440),
                                 read_flash_string(F_(days)).c_str(),
                                 ((time_value % 1440) / 60),
                                 read_flash_string(F_(hours)).c_str(),
                                 (time_value % 60),
                                 read_flash_string(F_(minutes)).c_str());
                        json[name] = time_s;
                    } else {
                        json[name] = time_value;
                    }
                }
                // commenting out - we don't want Commands in MQTT or Console
                //  else if (dv.type == DeviceValueType::CMD && output_target != EMSdevice::OUTPUT_TARGET::MQTT) {
                //     json[name] = "";
                // }
            }
        }
    }

    return has_values;
}

// create the Home Assistant configs for each device value / entity
// this is called when an MQTT publish is done via an EMS Device in emsesp.cpp
void EMSdevice::publish_mqtt_ha_entity_config() {
    // create the main device config if not already done, per device type
    bool create_device_config = !ha_config_done();

    for (auto & dv : devicevalues_) {
        if (dv.has_state(DeviceValueState::DV_ACTIVE)) {
            // add it if not already done and if it's visible (not on the exclusion list)
            // don't do this for commands (like reset)
            if (!dv.has_state(DeviceValueState::DV_HA_CONFIG_CREATED) && dv.has_state(DeviceValueState::DV_VISIBLE) && dv.type != DeviceValueType::CMD) {
                Mqtt::publish_ha_sensor_config(dv, name(), brand_to_string(), false, create_device_config);
                dv.add_state(DeviceValueState::DV_HA_CONFIG_CREATED);
                if (create_device_config) {
                    create_device_config = false;
                }
            }
        } else {
            if (dv.has_state(DeviceValueState::DV_HA_CONFIG_CREATED)) {
                // if the HA config has already been created and now the value has gone dormant, delete the config
                // https://github.com/emsesp/EMS-ESP32/issues/196
                Mqtt::publish_ha_sensor_config(dv, name(), brand_to_string(), true, create_device_config); // remove /config
                dv.remove_state(DeviceValueState::DV_HA_CONFIG_CREATED);
            }
        }
    }

    ha_config_done(true); // assume we've created the config
}

// remove all config topics in HA
void EMSdevice::ha_config_clear() {
    for (auto & dv : devicevalues_) {
        Mqtt::publish_ha_sensor_config(dv, "", "", true); // delete topic (remove = true)
        dv.remove_state(DeviceValueState::DV_HA_CONFIG_CREATED);
    }

    ha_config_done(false); // this will force the recreation of the main HA device config
}

bool EMSdevice::has_telegram_id(uint16_t id) {
    for (const auto & tf : telegram_functions_) {
        if (tf.telegram_type_id_ == id) {
            return true;
        }
    }
    return false;
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
            return read_flash_string(tf.telegram_type_name_);
        }
    }

    return std::string{};
}

// take a telegram_type_id and call the matching handler
// return true if match found
bool EMSdevice::handle_telegram(std::shared_ptr<const Telegram> telegram) {
    for (auto & tf : telegram_functions_) {
        if (tf.telegram_type_id_ == telegram->type_id) {
            // if the data block is empty and we have not received data before, assume that this telegram
            // is not recognized by the bus master. So remove it from the automatic fetch list
            if (telegram->message_length == 0 && telegram->offset == 0 && !tf.received_) {
                EMSESP::logger().debug(F("This telegram (%s) is not recognized by the EMS bus"), read_flash_string(tf.telegram_type_name_).c_str());
                tf.fetch_ = false;
                return false;
            }
            if (telegram->message_length > 0) {
                tf.received_ = true;
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
