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
    F_(bar)

};

// mapping of TAGs, to match order in DeviceValueTAG enum in emsdevice.h
// must be an int of 4 bytes, 32bit aligned
static const __FlashStringHelper * const DeviceValueTAG_s[] PROGMEM = {

    F_(tag_boiler_data),
    F_(tag_boiler_data_ww),
    F_(tag_boiler_data_info),
    F_(tag_hc1),
    F_(tag_hc2),
    F_(tag_hc3),
    F_(tag_hc4),
    F_(tag_wwc1),
    F_(tag_wwc2),
    F_(tag_wwc3),
    F_(tag_wwc4)

};

const std::string EMSdevice::uom_to_string(uint8_t uom) {
    if (uom == DeviceValueUOM::NONE || uom >= DeviceValueUOM::PUMP) {
        return std::string{};
    }
    return uuid::read_flash_string(DeviceValueUOM_s[uom - 1]); // offset by 1 to account for NONE
}

const std::string EMSdevice::tag_to_string(uint8_t tag) {
    if (tag == DeviceValueTAG::TAG_NONE) {
        return std::string{};
    }
    return uuid::read_flash_string(DeviceValueTAG_s[tag - 1]); // offset by 1 to account for NONE
}

const std::vector<EMSdevice::DeviceValue> EMSdevice::devicevalues() const {
    return devicevalues_;
}

std::string EMSdevice::brand_to_string() const {
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
    case EMSdevice::Brand::NO_BRAND:
    default:
        return read_flash_string(F("---"));
        break;
    }

    return std::string{};
}

// returns the name of the MQTT topic to use for a specific device
std::string EMSdevice::device_type_2_device_name(const uint8_t device_type) {
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
    if (!strcmp_P(topic, reinterpret_cast<PGM_P>(F_(boiler)))) {
        return DeviceType::BOILER;
    }

    if (!strcmp_P(topic, reinterpret_cast<PGM_P>(F_(thermostat)))) {
        return DeviceType::THERMOSTAT;
    }

    if (!strcmp_P(topic, reinterpret_cast<PGM_P>(F_(system)))) {
        return DeviceType::SYSTEM;
    }

    if (!strcmp_P(topic, reinterpret_cast<PGM_P>(F_(heatpump)))) {
        return DeviceType::HEATPUMP;
    }

    if (!strcmp_P(topic, reinterpret_cast<PGM_P>(F_(solar)))) {
        return DeviceType::SOLAR;
    }

    if (!strcmp_P(topic, reinterpret_cast<PGM_P>(F_(mixer)))) {
        return DeviceType::MIXER;
    }

    if (!strcmp_P(topic, reinterpret_cast<PGM_P>(F_(dallassensor)))) {
        return DeviceType::DALLASSENSOR;
    }

    return DeviceType::UNKNOWN;
}

// return name of the device type, capitalized
std::string EMSdevice::device_type_name() const {
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
    case 0:
    default:
        return EMSdevice::Brand::NO_BRAND;
        break;
    }
}

// returns string of a human friendly description of the EMS device
std::string EMSdevice::to_string() const {
    std::string str(160, '\0');

    // for devices that haven't been lookup yet, don't show all details
    if (product_id_ == 0) {
        snprintf_P(&str[0], str.capacity() + 1, PSTR("%s (DeviceID:0x%02X)"), name_.c_str(), device_id_);
        return str;
    }

    if (brand_ == Brand::NO_BRAND) {
        snprintf_P(&str[0], str.capacity() + 1, PSTR("%s (DeviceID:0x%02X, ProductID:%d, Version:%s)"), name_.c_str(), device_id_, product_id_, version_.c_str());
    } else {
        snprintf_P(&str[0], str.capacity() + 1, PSTR("%s %s (DeviceID:0x%02X ProductID:%d, Version:%s)"), brand_to_string().c_str(), name_.c_str(), device_id_, product_id_, version_.c_str());
    }

    return str;
}

// returns out brand + device name
std::string EMSdevice::to_string_short() const {
    std::string str(160, '\0');
    if (brand_ == Brand::NO_BRAND) {
        snprintf_P(&str[0], str.capacity() + 1, PSTR("%s: %s"), device_type_name().c_str(), name_.c_str());
    } else {
        snprintf_P(&str[0], str.capacity() + 1, PSTR("%s: %s %s"), device_type_name().c_str(), brand_to_string().c_str(), name_.c_str());
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
bool EMSdevice::get_toggle_fetch(uint16_t telegram_id) {
    for (auto & tf : telegram_functions_) {
        if (tf.telegram_type_id_ == telegram_id) {
            return tf.fetch_;
        }
    }
    return false;
}

// list device values, only for EMSESP_DEBUG mode
void EMSdevice::show_device_values_debug(uuid::console::Shell & shell) {
    size_t  total_s = 0;
    uint8_t count   = 0;
    for (const auto & dv : devicevalues_) {
        size_t s = sizeof(dv);
        if (dv.full_name) {
            shell.printfln("[%s] %d", uuid::read_flash_string(dv.full_name).c_str(), s);
        } else {
            shell.printfln("[%s]* %d", uuid::read_flash_string(dv.short_name).c_str(), s);
        }
        total_s += s;
        count++;
    }
    shell.printfln("Total size of %d elements: %d", count, total_s);
    shell.println();
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
    // for (const auto & tf : *telegram_functions_) {
    for (const auto & tf : telegram_functions_) {
        snprintf_P(str, sizeof(str), PSTR("0x%02X"), tf.telegram_type_id_);
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

void EMSdevice::register_mqtt_topic(const std::string & topic, mqtt_subfunction_p f) {
    Mqtt::subscribe(device_type_, topic, f);
}

// add command to library
void EMSdevice::register_mqtt_cmd(const __FlashStringHelper * cmd, cmdfunction_p f) {
    Command::add(device_type_, cmd, f);
}

// register a call back function for a specific telegram type
void EMSdevice::register_telegram_type(const uint16_t telegram_type_id, const __FlashStringHelper * telegram_type_name, bool fetch, process_function_p f) {
    telegram_functions_.emplace_back(telegram_type_id, telegram_type_name, fetch, f);
}

// add to device value library
// arguments are:
//  tag: to be used to group mqtt together, either as separate topics as a nested object
//  value: pointer to the value from the .h file
//  type: one of DeviceValueType
//  options: options for enum or a divider for int (e.g. F("10"))
//  short_name: used in Mqtt as keys
//  full name: used in Web and Console unless empty (nullptr)
//  uom: unit of measure from DeviceValueUOM
void EMSdevice::register_device_value(uint8_t tag, void * value_p, uint8_t type, const __FlashStringHelper * const * options, const __FlashStringHelper * short_name, const __FlashStringHelper * full_name, uint8_t uom) {
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
    } else {
        *(uint8_t *)(value_p) = EMS_VALUE_UINT_NOTSET; // enums, uint8_t, bool behave as uint8_t
    }

    // count #options
    uint8_t options_size = 0;
    if (options != nullptr) {
        uint8_t i = 0;
        while (options[i++]) {
            options_size++;
        };
    }

    devicevalues_.emplace_back(device_type_, tag, value_p, type, options, options_size, short_name, full_name, uom);
}

// looks up the uom (suffix) for a given key from the device value table
std::string EMSdevice::get_value_uom(const char * key) {
    // the key may have a suffix at the start which is between brackets. remove it.
    char new_key[80];
    strncpy(new_key, key, sizeof(new_key));
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
                // ignore TIME since "minutes" is already included
                if ((dv.uom == DeviceValueUOM::NONE) || (dv.uom == DeviceValueUOM::MINUTES)) {
                    break;
                }
                return EMSdevice::uom_to_string(dv.uom);
            }
        }
    }

    return std::string{}; // not found
}

// prepare array of device values, as 3 elements serialized (name, value, uom) in array to send to Web UI
// returns number of elements
bool EMSdevice::generate_values_json_web(JsonObject & json) {
    json["name"]   = to_string_short();
    JsonArray data = json.createNestedArray("data");

    uint8_t num_elements = 0;

    for (const auto & dv : devicevalues_) {
        // ignore if full_name empty
        if (dv.full_name != nullptr) {
            // handle Booleans (true, false)
            if ((dv.type == DeviceValueType::BOOL) && Helpers::hasValue(*(uint8_t *)(dv.value_p), EMS_VALUE_BOOL)) {
                // see if we have options for the bool's
                if (dv.options_size == 2) {
                    data.add(*(uint8_t *)(dv.value_p) ? dv.options[0] : dv.options[1]);
                } else {
                    // always render booleans as on or off
                    data.add(*(uint8_t *)(dv.value_p) ? F_(on) : F_(off));
                }
            }

            // handle TEXT strings
            else if ((dv.type == DeviceValueType::TEXT) && (Helpers::hasValue((char *)(dv.value_p)))) {
                data.add((char *)(dv.value_p));
            }

            // handle ENUMs
            else if ((dv.type == DeviceValueType::ENUM) && Helpers::hasValue(*(uint8_t *)(dv.value_p))) {
                if (*(uint8_t *)(dv.value_p) < dv.options_size) {
                    data.add(dv.options[*(uint8_t *)(dv.value_p)]);
                }
            }

            else {
                // handle Integers and Floats
                // If a divider is specified, do the division to 2 decimals places and send back as double/float
                // otherwise force as an integer whole
                // the nested if's is necessary due to the way the ArduinoJson templates are pre-processed by the compiler
                uint8_t divider = (dv.options_size == 1) ? Helpers::atoint(uuid::read_flash_string(dv.options[0]).c_str()) : 0;

                // INT
                if ((dv.type == DeviceValueType::INT) && Helpers::hasValue(*(int8_t *)(dv.value_p))) {
                    if (divider) {
                        data.add(Helpers::round2(*(int8_t *)(dv.value_p), divider));
                    } else {
                        data.add(*(int8_t *)(dv.value_p));
                    }
                } else if ((dv.type == DeviceValueType::UINT) && Helpers::hasValue(*(uint8_t *)(dv.value_p))) {
                    if (divider) {
                        data.add(Helpers::round2(*(uint8_t *)(dv.value_p), divider));
                    } else {
                        data.add(*(uint8_t *)(dv.value_p));
                    }
                } else if ((dv.type == DeviceValueType::SHORT) && Helpers::hasValue(*(int16_t *)(dv.value_p))) {
                    if (divider) {
                        data.add(Helpers::round2(*(int16_t *)(dv.value_p), divider));
                    } else {
                        data.add(*(int16_t *)(dv.value_p));
                    }
                } else if ((dv.type == DeviceValueType::USHORT) && Helpers::hasValue(*(uint16_t *)(dv.value_p))) {
                    if (divider) {
                        data.add(Helpers::round2(*(uint16_t *)(dv.value_p), divider));
                    } else {
                        data.add(*(uint16_t *)(dv.value_p));
                    }
                } else if ((dv.type == DeviceValueType::ULONG) && Helpers::hasValue(*(uint32_t *)(dv.value_p))) {
                    if (divider) {
                        data.add(Helpers::round2(*(uint32_t *)(dv.value_p), divider));
                    } else {
                        data.add(*(uint32_t *)(dv.value_p));
                    }
                } else if ((dv.type == DeviceValueType::TIME) && Helpers::hasValue(*(uint32_t *)(dv.value_p))) {
                    uint32_t time_value = *(uint32_t *)(dv.value_p);
                    time_value          = (divider) ? time_value / divider : time_value; // sometimes we need to divide by 60
                    char time_s[40];
                    snprintf_P(time_s, 40, PSTR("%d days %d hours %d minutes"), (time_value / 1440), ((time_value % 1440) / 60), (time_value % 60));
                    data.add(time_s);
                }
            }

            // check if we've added a data element by comparing the size
            // then add the remaining elements
            uint8_t sz = data.size();
            if (sz > num_elements) {
                // add the unit of measure (uom)
                if (dv.uom == DeviceValueUOM::MINUTES) {
                    data.add(nullptr);
                } else {
                    data.add(uom_to_string(dv.uom));
                }

                // add name, prefixing the tag if it exists
                // if we're a boiler, ignore the tag
                if ((dv.tag == DeviceValueTAG::TAG_NONE) || (device_type_ == DeviceType::BOILER)) {
                    data.add(dv.full_name);
                } else {
                    char name[50];
                    snprintf_P(name, sizeof(name), "(%s) %s", tag_to_string(dv.tag).c_str(), uuid::read_flash_string(dv.full_name).c_str());
                    data.add(name);
                }
                num_elements = sz + 2;
            }
        }
    }

    return (num_elements != 0);
}

// For each value in the device create the json object pair and add it to given json
// return false if empty
// this is used to create both the MQTT payloads and Console messages (console = true)
bool EMSdevice::generate_values_json(JsonObject & root, const uint8_t tag_filter, const bool nested, const bool console) {
    bool       has_value = false; // to see if we've added a value. it's faster than doing a json.size() at the end
    uint8_t    old_tag   = 255;   // NAN
    JsonObject json      = root;

    for (const auto & dv : devicevalues_) {
        // only show if tag is either empty (TAG_NONE) or matches a value
        // and don't show if full_name is empty unless we're outputing for mqtt payloads
        // for nested we use all values
        if (((nested) || tag_filter == DeviceValueTAG::TAG_NONE || (tag_filter == dv.tag)) && (dv.full_name != nullptr || !console)) {
            bool have_tag = (dv.tag != tag_filter);

            // EMSESP::logger().info(F("**HERE: console=%d nested=%d tag_filter=%d tag=%d type=%d short=%s"), console, nested, tag_filter, dv.tag, dv.type, uuid::read_flash_string(dv.short_name).c_str());

            char name[80];
            if (console) {
                // prefix the tag in brackets, unless it's Boiler because we're naughty and use tag for the MQTT topic
                if (have_tag) {
                    snprintf_P(name, 80, "(%s) %s", tag_to_string(dv.tag).c_str(), uuid::read_flash_string(dv.full_name).c_str());
                } else {
                    strcpy(name, uuid::read_flash_string(dv.full_name).c_str()); // use full name
                }
            } else {
                strcpy(name, uuid::read_flash_string(dv.short_name).c_str()); // use short name

                // if we have a tag, and its different to the last one create a nested object
                if (dv.tag != old_tag) {
                    old_tag = dv.tag;
                    if (nested && have_tag) {
                        json = root.createNestedObject(tag_to_string(dv.tag));
                    }
                }
            }

            // handle Booleans (true, false)
            if ((dv.type == DeviceValueType::BOOL) && Helpers::hasValue(*(uint8_t *)(dv.value_p), EMS_VALUE_BOOL)) {
                // see if we have options for the bool's
                if (dv.options_size == 2) {
                    json[name] = *(uint8_t *)(dv.value_p) ? dv.options[0] : dv.options[1];
                    has_value  = true;
                } else {
                    // see how to render the value depending on the setting
                    // when in console mode we always use on and off
                    if ((Mqtt::bool_format() == BOOL_FORMAT_ONOFF) || console) {
                        // on or off as strings
                        json[name] = *(uint8_t *)(dv.value_p) ? F_(on) : F_(off);
                        has_value  = true;
                    } else if (Mqtt::bool_format() == BOOL_FORMAT_ONOFF_CAP) {
                        // on or off as strings
                        json[name] = *(uint8_t *)(dv.value_p) ? F_(ON) : F_(OFF);
                        has_value  = true;
                    } else if (Mqtt::bool_format() == BOOL_FORMAT_TRUEFALSE) {
                        // true or false values (as real booleans, not strings)
                        json[name] = (bool)(*(uint8_t *)(dv.value_p)) ? true : false;
                        has_value  = true;
                    } else {
                        // numerical 1 or 0
                        json[name] = (uint8_t)(*(uint8_t *)(dv.value_p)) ? 1 : 0;
                        has_value  = true;
                    }
                }
            }

            // handle TEXT strings
            else if ((dv.type == DeviceValueType::TEXT) && (Helpers::hasValue((char *)(dv.value_p)))) {
                json[name] = (char *)(dv.value_p);
                has_value  = true;
            }

            // handle ENUMs
            else if ((dv.type == DeviceValueType::ENUM) && Helpers::hasValue(*(uint8_t *)(dv.value_p))) {
                if (*(uint8_t *)(dv.value_p) < dv.options_size) {
                    if (Mqtt::bool_format() == BOOL_FORMAT_10) {
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
                        snprintf_P(time_s, sizeof(time_s), PSTR("%d days %d hours %d minutes"), (time_value / 1440), ((time_value % 1440) / 60), (time_value % 60));
                        json[name] = time_s;
                    } else {
                        json[name] = time_value;
                    }
                    has_value = true;
                }
            }
        }
    }

    // for debugging
    // char dump[2000];
    // serializeJson(root, dump);
    // EMSESP::logger().info(F("**json root: %s"), dump);

    return has_value;
}

// create the Home Assistant configs for each value
// this is called when an MQTT publish is done via an EMS Device, and only done once
void EMSdevice::publish_mqtt_ha_sensor() {
    for (const auto & dv : devicevalues_) {
        Mqtt::publish_mqtt_ha_sensor(dv.type, dv.tag, dv.full_name, device_type_, dv.short_name, dv.uom);
    }

    bool ok = publish_ha_config();
    ha_config_done(ok); // see if it worked
}

// return the name of the telegram type
std::string EMSdevice::telegram_type_name(std::shared_ptr<const Telegram> telegram) {
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
void EMSdevice::read_command(const uint16_t type_id) {
    EMSESP::send_read_request(type_id, device_id());
}

} // namespace emsesp
