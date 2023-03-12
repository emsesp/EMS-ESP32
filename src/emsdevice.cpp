/*
 * EMS-ESP - https://github.com/emsesp/EMS-ESP
 * Copyright 2020-2023  Paul Derbyshire
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

// returns number of visible device values (entries) for this device
// this includes commands since they can also be entities and visible in the web UI
uint8_t EMSdevice::count_entities() {
    uint8_t count = 0;
    for (const auto & dv : devicevalues_) {
        if (!dv.has_state(DeviceValueState::DV_WEB_EXCLUDE) && dv.hasValue()) {
            count++;
        }
    }
    return count;
}

// see if there are entities, excluding any commands
bool EMSdevice::has_entities() const {
    for (const auto & dv : devicevalues_) {
        if (dv.type != DeviceValueType::CMD) {
            return true;
        }
    }
    bool found = false;
    EMSESP::webCustomizationService.read([&](WebCustomization & settings) {
        for (EntityCustomization entityCustomization : settings.entityCustomizations) {
            if (entityCustomization.device_id == device_id() && entityCustomization.entity_ids.size()) {
                found = true;
                break;
            }
        }
    });
    return found;
}

// return translated tag name based on tag id
const char * EMSdevice::tag_to_string(uint8_t tag, const bool translate) {
    uint8_t tag_n = tag > DeviceValue::NUM_TAGS ? 0 : tag;
    return (translate ? Helpers::translated_word(DeviceValue::DeviceValueTAG_s[tag_n]) : DeviceValue::DeviceValueTAG_s[tag_n][0]);
}

const char * EMSdevice::tag_to_mqtt(uint8_t tag) {
    return (DeviceValue::DeviceValueTAG_mqtt[tag > DeviceValue::NUM_TAGS ? 0 : tag]);
}

// convert UOM to a char string - translating only for hours/minutes/seconds
const char * EMSdevice::uom_to_string(uint8_t uom) {
    switch (uom) {
    case DeviceValueUOM::DEGREES:
    case DeviceValueUOM::DEGREES_R:
        return EMSESP::system_.fahrenheit() ? DeviceValue::DeviceValueUOM_s[DeviceValueUOM::FAHRENHEIT] : DeviceValue::DeviceValueUOM_s[uom];
    case DeviceValueUOM::HOURS:
        return Helpers::translated_word(FL_(hours));
    case DeviceValueUOM::MINUTES:
        return Helpers::translated_word(FL_(minutes));
    case DeviceValueUOM::SECONDS:
        return Helpers::translated_word(FL_(seconds));
    default:
        return DeviceValue::DeviceValueUOM_s[uom];
    }
}

const char * EMSdevice::brand_to_char() {
    switch (brand_) {
    case EMSdevice::Brand::BOSCH:
        return F_(bosch);
    case EMSdevice::Brand::JUNKERS:
        return F_(junkers);
    case EMSdevice::Brand::BUDERUS:
        return F_(buderus);
    case EMSdevice::Brand::NEFIT:
        return F_(nefit);
    case EMSdevice::Brand::SIEGER:
        return F_(sieger);
    case EMSdevice::Brand::WORCESTER:
        return F_(worcester);
    case EMSdevice::Brand::IVT:
        return F_(ivt);
    default:
        return F_(no_brand);
    }
}

// returns the short name of the device, used in MQTT and console commands, all lowercase
const char * EMSdevice::device_type_2_device_name(const uint8_t device_type) {
    switch (device_type) {
    case DeviceType::SYSTEM:
        return F_(system);
    case DeviceType::SCHEDULER:
        return F_(scheduler);
    case DeviceType::BOILER:
        return F_(boiler);
    case DeviceType::THERMOSTAT:
        return F_(thermostat);
    case DeviceType::HEATPUMP:
        return F_(heatpump);
    case DeviceType::SOLAR:
        return F_(solar);
    case DeviceType::CONNECT:
        return F_(connect);
    case DeviceType::MIXER:
        return F_(mixer);
    case DeviceType::DALLASSENSOR:
        return F_(dallassensor);
    case DeviceType::ANALOGSENSOR:
        return F_(analogsensor);
    case DeviceType::CONTROLLER:
        return F_(controller);
    case DeviceType::SWITCH:
        return F_(switch);
    case DeviceType::GATEWAY:
        return F_(gateway);
    case DeviceType::ALERT:
        return F_(alert);
    case DeviceType::PUMP:
        return F_(pump);
    case DeviceType::HEATSOURCE:
        return F_(heatsource);
    default:
        return Helpers::translated_word(FL_(unknown), true);
    }
}

// returns the translated name of a specific EMS device
// excludes dallassensor, analogsensor and system
const char * EMSdevice::device_type_2_device_name_translated() {
    switch (device_type_) {
    case DeviceType::BOILER:
        return Helpers::translated_word(FL_(boiler_device));
    case DeviceType::THERMOSTAT:
        return Helpers::translated_word(FL_(thermostat_device));
    case DeviceType::HEATPUMP:
        return Helpers::translated_word(FL_(heatpump_device));
    case DeviceType::SOLAR:
        return Helpers::translated_word(FL_(solar_device));
    case DeviceType::CONNECT:
        return Helpers::translated_word(FL_(connect_device));
    case DeviceType::MIXER:
        return Helpers::translated_word(FL_(mixer_device));
    case DeviceType::CONTROLLER:
        return Helpers::translated_word(FL_(controller_device));
    case DeviceType::SWITCH:
        return Helpers::translated_word(FL_(switch_device));
    case DeviceType::GATEWAY:
        return Helpers::translated_word(FL_(gateway_device));
    case DeviceType::ALERT:
        return Helpers::translated_word(FL_(alert_device));
    case DeviceType::PUMP:
        return Helpers::translated_word(FL_(pump_device));
    case DeviceType::HEATSOURCE:
        return Helpers::translated_word(FL_(heatsource_device));
    default:
        break;
    }
    return Helpers::translated_word(FL_(unknown_device));
}

// returns device_type from a non-translated EN string
uint8_t EMSdevice::device_name_2_device_type(const char * topic) {
    if (!topic) {
        return DeviceType::UNKNOWN;
    }

    // convert topic to lowercase and compare
    char lowtopic[20];
    strlcpy(lowtopic, topic, sizeof(lowtopic));
    for (char * p = lowtopic; *p; p++) {
        *p = tolower(*p);
    }

    if (!strcmp(lowtopic, F_(boiler))) {
        return DeviceType::BOILER;
    }
    if (!strcmp(lowtopic, F_(thermostat))) {
        return DeviceType::THERMOSTAT;
    }
    if (!strcmp(lowtopic, F_(system))) {
        return DeviceType::SYSTEM;
    }
    if (!strcmp(lowtopic, F_(scheduler))) {
        return DeviceType::SCHEDULER;
    }
    if (!strcmp(lowtopic, F_(heatpump))) {
        return DeviceType::HEATPUMP;
    }
    if (!strcmp(lowtopic, F_(solar))) {
        return DeviceType::SOLAR;
    }
    if (!strcmp(lowtopic, F_(mixer))) {
        return DeviceType::MIXER;
    }
    if (!strcmp(lowtopic, F_(dallassensor))) {
        return DeviceType::DALLASSENSOR;
    }
    if (!strcmp(lowtopic, F_(analogsensor))) {
        return DeviceType::ANALOGSENSOR;
    }
    if (!strcmp(lowtopic, F_(switch))) {
        return DeviceType::SWITCH;
    }
    if (!strcmp(lowtopic, F_(gateway))) {
        return DeviceType::GATEWAY;
    }
    if (!strcmp(lowtopic, F_(alert))) {
        return DeviceType::ALERT;
    }
    if (!strcmp(lowtopic, F_(pump))) {
        return DeviceType::PUMP;
    }
    if (!strcmp(lowtopic, F_(heatsource))) {
        return DeviceType::HEATSOURCE;
    }

    return DeviceType::UNKNOWN;
}

// return name of the device type, not translated
const char * EMSdevice::device_type_name() {
    return device_type_2_device_name(device_type_);
}

// 0=unknown, 1=bosch, 2=junkers, 3=buderus, 4=nefit, 5=sieger, 11=worcester
uint8_t EMSdevice::decode_brand(uint8_t value) {
    switch (value) {
    case 1:
        return EMSdevice::Brand::BOSCH;
    case 2:
        return EMSdevice::Brand::JUNKERS;
    case 3:
        return EMSdevice::Brand::BUDERUS;
    case 4:
        return EMSdevice::Brand::NEFIT;
    case 5:
        return EMSdevice::Brand::SIEGER;
    case 11:
        return EMSdevice::Brand::WORCESTER;
    case 13:
        return EMSdevice::Brand::IVT;
    default:
        return EMSdevice::Brand::NO_BRAND;
    }
}

// returns string of a human friendly description of the EMS device
const std::string EMSdevice::to_string() {
    // for devices that haven't been lookup yet, don't show all details
    if (product_id_ == 0) {
        return std::string(name_) + " (DeviceID:" + Helpers::hextoa(device_id_) + ")";
    }

    if (brand_ == Brand::NO_BRAND) {
        return std::string(name_) + " (DeviceID:" + Helpers::hextoa(device_id_) + ", ProductID:" + Helpers::itoa(product_id_) + ", Version:" + version_ + ")";
    }

    return std::string(brand_to_char()) + " " + name_ + " (DeviceID:" + Helpers::hextoa(device_id_) + ", ProductID:" + Helpers::itoa(product_id_)
           + ", Version:" + version_ + ")";
}

// returns out brand + device name
// translated
const std::string EMSdevice::to_string_short() {
    if (brand_ == Brand::NO_BRAND) {
        return std::string(device_type_2_device_name_translated()) + ": " + name_;
    }

    return std::string(device_type_2_device_name_translated()) + ": " + brand_to_char() + " " + name_;
}

// for each telegram that has the fetch value set (true) do a read request
void EMSdevice::fetch_values() {
#if defined(EMSESP_DEBUG)
    EMSESP::logger().debug("Fetching values for deviceID 0x%02X", device_id());
#endif

    for (const auto & tf : telegram_functions_) {
        if (tf.fetch_) {
            read_command(tf.telegram_type_id_);
        }
    }
}

// toggle on/off automatic fetch for a telegramID
void EMSdevice::toggle_fetch(uint16_t telegram_id, bool toggle) {
#if defined(EMSESP_DEBUG)
    EMSESP::logger().debug("Toggling fetch for deviceID 0x%02X, telegramID 0x%02X to %d", device_id(), telegram_id, toggle);
#endif

    for (auto & tf : telegram_functions_) {
        if (tf.telegram_type_id_ == telegram_id) {
            tf.fetch_ = toggle;
        }
    }
}

// get status of automatic fetch for a telegramID
bool EMSdevice::is_fetch(uint16_t telegram_id) const {
    for (const auto & tf : telegram_functions_) {
        if (tf.telegram_type_id_ == telegram_id) {
            return tf.fetch_;
        }
    }
    return false;
}

// check for a tag to create a nest
bool EMSdevice::has_tags(const uint8_t tag) const {
    for (const auto & dv : devicevalues_) {
        if (dv.tag == tag && tag >= DeviceValueTAG::TAG_HC1) {
            return true;
        }
    }
    return false;
}

// check if the device has a command with this tag.
bool EMSdevice::has_cmd(const char * cmd, const int8_t id) const {
    uint8_t tag = DeviceValueTAG::TAG_HC1 + id - 1;
    for (const auto & dv : devicevalues_) {
        if ((id < 1 || dv.tag == tag) && dv.has_cmd && strcmp(dv.short_name, cmd) == 0) {
            return true;
        }
    }
    return false;
}

// list of registered device entries
// called from the command 'entities'
void EMSdevice::list_device_entries(JsonObject & output) const {
    for (const auto & dv : devicevalues_) {
        auto fullname = dv.get_fullname();
        if (!dv.has_state(DeviceValueState::DV_WEB_EXCLUDE) && dv.type != DeviceValueType::CMD && !fullname.empty()) {
            // if we have a tag prefix it
            char key[50];
            auto tag_s = EMSdevice::tag_to_mqtt(dv.tag);
            if (strlen(tag_s)) {
                snprintf(key, sizeof(key), "%s.%s", tag_s, dv.short_name);
            } else {
                snprintf(key, sizeof(key), "%s", dv.short_name);
            }

            JsonArray details = output.createNestedArray(key);

            // add the full name description
            details.add(fullname);

            // add uom
            if (dv.uom != DeviceValueUOM::NONE) {
                details.add(EMSdevice::uom_to_string(dv.uom));
            }
        }
    }
}

// list all the telegram type IDs for this device
void EMSdevice::show_telegram_handlers(uuid::console::Shell & shell) const {
    if (telegram_functions_.empty()) {
        return;
    }
    /*
	// colored list of type-ids
    shell.printf(" This %s will listen to telegram type IDs: ", device_type_name().c_str());
    for (const auto & tf : telegram_functions_) {
        if (tf.received_ && !tf.fetch_) {
            shell.printf(COLOR_BRIGHT_GREEN);
        } else if (tf.received_) {
            shell.printf(COLOR_YELLOW);
        } else {
            shell.printf(COLOR_BRIGHT_RED);
        }
        shell.printf("0x%02X ", tf.telegram_type_id_);
    }
    shell.printf(COLOR_RESET);
    */
    shell.printf(" Received telegram type IDs: ");
    for (const auto & tf : telegram_functions_) {
        if (tf.received_ && !tf.fetch_) {
            shell.printf("0x%02X ", tf.telegram_type_id_);
        }
    }
    shell.println();
    shell.printf(" Fetched telegram type IDs: ");
    for (const auto & tf : telegram_functions_) {
        if (tf.fetch_) {
            shell.printf("0x%02X ", tf.telegram_type_id_);
        }
    }
    shell.println();
    shell.printf(" Pending telegram type IDs: ");
    for (const auto & tf : telegram_functions_) {
        if (!tf.received_ && !tf.fetch_) {
            shell.printf("0x%02X ", tf.telegram_type_id_);
        }
    }
    shell.println();
    shell.printf(" Ignored telegram type IDs: ");
    for (auto handlers : handlers_ignored_) {
        shell.printf("0x%02X ", handlers);
    }
    shell.println();
}

// list all the telegram type IDs for this device, outputting to a string (max size 200)
char * EMSdevice::show_telegram_handlers(char * result, const size_t len, const uint8_t handlers) {
    strlcpy(result, "", len);

    uint8_t i = 0;
    for (const auto & tf : telegram_functions_) {
        if (handlers == Handlers::ALL || (handlers == Handlers::RECEIVED && tf.received_ && !tf.fetch_)
            || (handlers == Handlers::FETCHED && tf.received_ && tf.fetch_) || (handlers == Handlers::PENDING && !tf.received_ && !tf.fetch_)) {
            if (i++ > 0) {
                strlcat(result, " ", len);
            }
            strlcat(result, Helpers::hextoa(tf.telegram_type_id_, true).c_str(), len);
        }
    }

    if (handlers == Handlers::ALL || handlers == Handlers::IGNORED) {
        i = 0;
        for (auto h : handlers_ignored_) {
            if (i++ > 0) {
                strlcat(result, " ", len);
            }
            strlcat(result, Helpers::hextoa(h).c_str(), len);
        }
    }

    return result;
}

void EMSdevice::add_handlers_ignored(const uint16_t handler) {
    for (auto handlers : handlers_ignored_) {
        if (handler == handlers) {
            return;
        }
    }
    handlers_ignored_.push_back(handler);
}

// list all the mqtt handlers for this device
void EMSdevice::show_mqtt_handlers(uuid::console::Shell & shell) const {
    Mqtt::show_topic_handlers(shell, device_type_);
}

// register a callback function for a specific telegram type
void EMSdevice::register_telegram_type(const uint16_t telegram_type_id, const char * telegram_type_name, bool fetch, const process_function_p f) {
    telegram_functions_.emplace_back(telegram_type_id, telegram_type_name, fetch, false, f);
}

// add to device value library, also know now as a "device entity"
void EMSdevice::add_device_value(uint8_t               tag,              // to be used to group mqtt together, either as separate topics as a nested object
                                 void *                value_p,          // pointer to the value from the .h file
                                 uint8_t               type,             // one of DeviceValueType
                                 const char * const ** options,          // options for enum, which are translated as a list of lists
                                 const char * const *  options_single,   // list of names
                                 int8_t                numeric_operator, // to divide or multiply, see DeviceValueNumOps::
                                 const char * const *  name,             // list of names, including shortname and translations
                                 uint8_t               uom,              // unit of measure from DeviceValueUOM
                                 const cmd_function_p  f,                // command function pointer
                                 int16_t               min,              // min allowed value
                                 uint16_t              max               // max allowed value
) {
    // initialize the device value depending on it's type
    // ignoring DeviceValueType::CMD and DeviceValueType::TIME

    if (type == DeviceValueType::STRING) {
        *(char *)(value_p) = {'\0'}; // this is important for string functions like strlen() to work later
    } else if (type == DeviceValueType::INT) {
        *(int8_t *)(value_p) = System::test_set_all_active() ? EMS_VALUE_DEFAULT_INT_DUMMY : EMS_VALUE_DEFAULT_INT;
    } else if (type == DeviceValueType::UINT) {
        *(uint8_t *)(value_p) = System::test_set_all_active() ? EMS_VALUE_DEFAULT_UINT_DUMMY : EMS_VALUE_DEFAULT_UINT;
    } else if (type == DeviceValueType::SHORT) {
        *(int16_t *)(value_p) = System::test_set_all_active() ? EMS_VALUE_DEFAULT_SHORT_DUMMY : EMS_VALUE_DEFAULT_SHORT;
    } else if (type == DeviceValueType::USHORT) {
        *(uint16_t *)(value_p) = System::test_set_all_active() ? EMS_VALUE_DEFAULT_USHORT_DUMMY : EMS_VALUE_DEFAULT_USHORT;
    } else if ((type == DeviceValueType::ULONG) || (type == DeviceValueType::TIME)) {
        *(uint32_t *)(value_p) = System::test_set_all_active() ? EMS_VALUE_DEFAULT_ULONG_DUMMY : EMS_VALUE_DEFAULT_ULONG;
    } else if (type == DeviceValueType::BOOL) {
        *(int8_t *)(value_p) = System::test_set_all_active() ? EMS_VALUE_DEFAULT_BOOL_DUMMY : EMS_VALUE_DEFAULT_BOOL; // bool is uint8_t, but other initial value
    } else if (type == DeviceValueType::ENUM) {
        *(uint8_t *)(value_p) = System::test_set_all_active() ? EMS_VALUE_DEFAULT_ENUM_DUMMY : EMS_VALUE_DEFAULT_ENUM; // enums behave as uint8_t
    }

    uint8_t     state           = DeviceValueState::DV_DEFAULT; // determine state
    std::string custom_fullname = std::string("");              // custom fullname
    auto        short_name      = name[0];                      // entity name
    bool        has_cmd         = (f != nullptr);               // is it a command?
    bool        ignore          = false;                        // ignore this entity?

    // get fullname, getting translation if it exists
    const char * const * fullname;
    if (Helpers::count_items(name) == 1) {
        fullname = nullptr; // no translations available, use empty
    } else {
        fullname = &name[1]; // translations start at index 1
    }

    // scan through customizations to see if it's on the exclusion list by matching the productID and deviceID
    EMSESP::webCustomizationService.read([&](WebCustomization & settings) {
        for (EntityCustomization entityCustomization : settings.entityCustomizations) {
            if ((entityCustomization.product_id == product_id()) && (entityCustomization.device_id == device_id())) {
                char entity[70];
                if (tag < DeviceValueTAG::TAG_HC1) {
                    strncpy(entity, short_name, sizeof(entity));
                } else {
                    snprintf(entity, sizeof(entity), "%s/%s", tag_to_mqtt(tag), short_name);
                }

                for (std::string entity_id : entityCustomization.entity_ids) {
                    // if there is an appended custom name, strip it to get the true entity name
                    // and extract the new custom name
                    auto        custom_name_pos = entity_id.find('|');
                    bool        has_custom_name = (custom_name_pos != std::string::npos);
                    std::string shortname       = has_custom_name ? entity_id.substr(2, custom_name_pos - 2) : entity_id.substr(2);

                    // we found the device entity
                    if (shortname == entity) {
                        // get Mask
                        uint8_t mask = Helpers::hextoint(entity_id.substr(0, 2).c_str());
                        state        = mask << 4;             // set state high bits to flag, turn off active and ha flags
                        ignore       = (mask & 0x80) == 0x80; // do not register
                        // see if there is a custom name in the entity string
                        if (has_custom_name) {
                            custom_fullname = entity_id.substr(custom_name_pos + 1);
                        }
                        break;
                    }
                }
            }
        }
    });
    if (ignore) {
        return;
    }

    // add the device entity
    devicevalues_.emplace_back(
        device_type_, tag, value_p, type, options, options_single, numeric_operator, short_name, fullname, custom_fullname, uom, has_cmd, min, max, state);

    // add a new command if it has a function attached
    if (has_cmd) {
        uint8_t flags = CommandFlag::ADMIN_ONLY; // executing commands require admin privileges

        if (tag >= DeviceValueTAG::TAG_HC1 && tag <= DeviceValueTAG::TAG_HC8) {
            flags |= CommandFlag::MQTT_SUB_FLAG_HC;
        } else if (tag >= DeviceValueTAG::TAG_WWC1 && tag <= DeviceValueTAG::TAG_WWC10) {
            flags |= CommandFlag::MQTT_SUB_FLAG_WWC;
        } else if (tag == DeviceValueTAG::TAG_DEVICE_DATA_WW || tag == DeviceValueTAG::TAG_BOILER_DATA_WW) {
            flags |= CommandFlag::MQTT_SUB_FLAG_WW;
        }

        // add the command to our library
        Command::add(device_type_, device_id_, short_name, f, fullname, flags);
    }
}

// single list of options
void EMSdevice::register_device_value(uint8_t              tag,
                                      void *               value_p,
                                      uint8_t              type,
                                      const char * const * options_single,
                                      const char * const * name,
                                      uint8_t              uom,
                                      const cmd_function_p f) {
    // create a multi-list from the options
    add_device_value(tag, value_p, type, nullptr, options_single, 0, name, uom, f, 0, 0);
};

// single list of options, with no translations, with min and max
void EMSdevice::register_device_value(uint8_t              tag,
                                      void *               value_p,
                                      uint8_t              type,
                                      const char * const * options_single,
                                      const char * const * name,
                                      uint8_t              uom,
                                      const cmd_function_p f,
                                      int16_t              min,
                                      uint16_t             max) {
    // create a multi-list from the options
    add_device_value(tag, value_p, type, nullptr, options_single, 0, name, uom, f, min, max);
};

void EMSdevice::register_device_value(uint8_t              tag,
                                      void *               value_p,
                                      uint8_t              type,
                                      int8_t               numeric_operator,
                                      const char * const * name,
                                      uint8_t              uom,
                                      const cmd_function_p f) {
    add_device_value(tag, value_p, type, nullptr, nullptr, numeric_operator, name, uom, f, 0, 0);
}

void EMSdevice::register_device_value(uint8_t              tag,
                                      void *               value_p,
                                      uint8_t              type,
                                      int8_t               numeric_operator,
                                      const char * const * name,
                                      uint8_t              uom,
                                      const cmd_function_p f,
                                      int16_t              min,
                                      uint16_t             max) {
    add_device_value(tag, value_p, type, nullptr, nullptr, numeric_operator, name, uom, f, min, max);
}

// no options, no function
void EMSdevice::register_device_value(uint8_t tag, void * value_p, uint8_t type, const char * const * name, uint8_t uom, const cmd_function_p f) {
    add_device_value(tag, value_p, type, nullptr, nullptr, 0, name, uom, f, 0, 0);
};

// no options, with min/max
void EMSdevice::register_device_value(uint8_t              tag,
                                      void *               value_p,
                                      uint8_t              type,
                                      const char * const * name,
                                      uint8_t              uom,
                                      const cmd_function_p f,
                                      int16_t              min,
                                      uint16_t             max) {
    add_device_value(tag, value_p, type, nullptr, nullptr, 0, name, uom, f, min, max);
};

// function with min and max values
// adds a new command to the command list
// in this function we separate out the short and long names and take any translations
void EMSdevice::register_device_value(uint8_t               tag,
                                      void *                value_p,
                                      uint8_t               type,
                                      const char * const ** options,
                                      const char * const *  name,
                                      uint8_t               uom,
                                      const cmd_function_p  f,
                                      int16_t               min,
                                      uint16_t              max) {
    add_device_value(tag, value_p, type, options, nullptr, 0, name, uom, f, min, max);
}

// function with no min and max values (set to 0)
void EMSdevice::register_device_value(uint8_t               tag,
                                      void *                value_p,
                                      uint8_t               type,
                                      const char * const ** options,
                                      const char * const *  name,
                                      uint8_t               uom,
                                      const cmd_function_p  f) {
    add_device_value(tag, value_p, type, options, nullptr, 0, name, uom, f, 0, 0);
}

// no associated command function, or min/max values
void EMSdevice::register_device_value(uint8_t tag, void * value_p, uint8_t type, const char * const ** options, const char * const * name, uint8_t uom) {
    add_device_value(tag, value_p, type, options, nullptr, 0, name, uom, nullptr, 0, 0);
}

// check if value is readable via mqtt/api
bool EMSdevice::is_readable(const void * value_p) const {
    for (const auto & dv : devicevalues_) {
        if (dv.value_p == value_p) {
            return !dv.has_state(DeviceValueState::DV_API_MQTT_EXCLUDE);
        }
    }
    return false;
}

// check if value/command is readonly
// matches valid tags too
bool EMSdevice::is_readonly(const std::string & cmd, const int8_t id) const {
    uint8_t tag = id > 0 ? DeviceValueTAG::TAG_HC1 + id - 1 : DeviceValueTAG::TAG_NONE;
    for (const auto & dv : devicevalues_) {
        // check command name and tag, id -1 is default hc and only checks name
        if (dv.has_cmd && std::string(dv.short_name) == cmd && (dv.tag < DeviceValueTAG::TAG_HC1 || dv.tag == tag || id == -1)) {
            return dv.has_state(DeviceValueState::DV_READONLY);
        }
    }
    return true; // not found, no write
}

// check if value has a registered command
bool EMSdevice::has_command(const void * value_p) const {
    for (const auto & dv : devicevalues_) {
        if (dv.value_p == value_p) {
            return dv.has_cmd && !dv.has_state(DeviceValueState::DV_READONLY);
        }
    }
    return false;
}

// publish a single value on change
void EMSdevice::publish_value(void * value_p) const {
    if (!Mqtt::publish_single() || value_p == nullptr) {
        return;
    }

    for (const auto & dv : devicevalues_) {
        if (dv.value_p == value_p && !dv.has_state(DeviceValueState::DV_API_MQTT_EXCLUDE)) {
            char topic[Mqtt::MQTT_TOPIC_MAX_SIZE];
            if (Mqtt::publish_single2cmd()) {
                if (dv.tag >= DeviceValueTAG::TAG_HC1) {
                    snprintf(topic, sizeof(topic), "%s/%s/%s", device_type_2_device_name(device_type_), tag_to_mqtt(dv.tag), dv.short_name);
                } else {
                    snprintf(topic, sizeof(topic), "%s/%s", device_type_2_device_name(device_type_), (dv.short_name));
                }
            } else if (Mqtt::is_nested() && dv.tag >= DeviceValueTAG::TAG_HC1) {
                snprintf(topic, sizeof(topic), "%s/%s/%s", Mqtt::tag_to_topic(device_type_, dv.tag).c_str(), tag_to_mqtt(dv.tag), dv.short_name);
            } else {
                snprintf(topic, sizeof(topic), "%s/%s", Mqtt::tag_to_topic(device_type_, dv.tag).c_str(), dv.short_name);
            }

            int8_t num_op = dv.numeric_operator;

            char    payload[55] = {'\0'};
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
                        auto enum_str = Helpers::translated_word(dv.options[*(uint8_t *)(value_p)]);
                        strlcpy(payload, enum_str, sizeof(payload));
                    }
                }
                break;
            }
            case DeviceValueType::USHORT:
                Helpers::render_value(payload, *(uint16_t *)(value_p), num_op, fahrenheit);
                break;
            case DeviceValueType::UINT:
                Helpers::render_value(payload, *(uint8_t *)(value_p), num_op, fahrenheit);
                break;
            case DeviceValueType::SHORT:
                Helpers::render_value(payload, *(int16_t *)(value_p), num_op, fahrenheit);
                break;
            case DeviceValueType::INT:
                Helpers::render_value(payload, *(int8_t *)(value_p), num_op, fahrenheit);
                break;
            case DeviceValueType::ULONG:
                Helpers::render_value(payload, *(uint32_t *)(value_p), num_op, fahrenheit);
                break;
            case DeviceValueType::BOOL:
                Helpers::render_boolean(payload, (bool)*(uint8_t *)(value_p));
                break;
            case DeviceValueType::TIME:
                Helpers::render_value(payload, *(uint32_t *)(value_p), num_op);
                break;
            case DeviceValueType::STRING:
                if (Helpers::hasValue((char *)(value_p))) {
                    strlcpy(payload, (char *)(value_p), sizeof(payload));
                }
                break;
            default:
                break;
            }

            if (payload[0] != '\0') {
                Mqtt::queue_publish(topic, payload);
            }
        }
    }
}

// looks up the UOM for a given key from the device value table
// key is the fullname
std::string EMSdevice::get_value_uom(const char * key) const {
    // the key may have a TAG string prefixed at the beginning. If so, remove it
    char new_key[80];
    strlcpy(new_key, key, sizeof(new_key));
    char * key_p = new_key;

    for (uint8_t i = 0; i < DeviceValue::NUM_TAGS; i++) {
        auto tag = Helpers::translated_word(DeviceValue::DeviceValueTAG_s[i]);
        if (tag) {
            std::string key2   = key; // copy string to a std::string so we can use the find function
            uint8_t     length = strlen(tag);
            if ((key2.find(tag) != std::string::npos) && (key[length] == ' ')) {
                key_p += length + 1; // remove the tag
                break;
            }
        }
    }

    // look up key in our device value list
    for (const auto & dv : devicevalues_) {
        auto fullname = dv.get_fullname();
        if ((!dv.has_state(DeviceValueState::DV_WEB_EXCLUDE) && !fullname.empty()) && (fullname == key_p)) {
            // ignore TIME since "minutes" is already added to the string value
            if ((dv.uom == DeviceValueUOM::NONE) || (dv.uom == DeviceValueUOM::MINUTES)) {
                break;
            }
            return EMSdevice::uom_to_string(dv.uom);
        }
    }

    return std::string{}; // not found
}

// prepare array of device values used for the WebUI
// this is loosely based of the function generate_values used for the MQTT and Console
// except additional data is stored in the JSON document needed for the Web UI like the UOM and command
// v=value, u=uom, n=name, c=cmd, h=help string, s=step, m=min, x=max
void EMSdevice::generate_values_web(JsonObject & output) {
    output["label"] = to_string_short();
    JsonArray data  = output.createNestedArray("data");

    for (auto & dv : devicevalues_) {
        auto fullname = dv.get_fullname();

        // check conditions:
        //  1. fullname cannot be empty
        //  2. it must have a valid value, if it is not a command like 'reset'
        //  3. show favorites first
        if (!dv.has_state(DeviceValueState::DV_WEB_EXCLUDE) && !fullname.empty() && (dv.hasValue() || (dv.type == DeviceValueType::CMD))) {
            JsonObject obj        = data.createNestedObject(); // create the object, we know there is a value
            uint8_t    fahrenheit = 0;

            // handle Booleans (true, false), use strings, no native true/false)
            if (dv.type == DeviceValueType::BOOL) {
                auto value_b = (bool)*(uint8_t *)(dv.value_p);
                char s[12];
                obj["v"] = Helpers::render_boolean(s, value_b, true);
            }

            // handle TEXT strings
            else if (dv.type == DeviceValueType::STRING) {
                obj["v"] = (char *)(dv.value_p);
            }

            // handle ENUMs
            else if ((dv.type == DeviceValueType::ENUM) && (*(uint8_t *)(dv.value_p) < dv.options_size)) {
                obj["v"] = Helpers::translated_word(dv.options[*(uint8_t *)(dv.value_p)]);
            }

            // handle numbers
            else {
                // note, the nested if's is necessary due to the way the ArduinoJson templates are pre-processed by the compiler
                fahrenheit = !EMSESP::system_.fahrenheit() ? 0 : (dv.uom == DeviceValueUOM::DEGREES) ? 2 : (dv.uom == DeviceValueUOM::DEGREES_R) ? 1 : 0;

                if ((dv.type == DeviceValueType::INT) && Helpers::hasValue(*(int8_t *)(dv.value_p))) {
                    obj["v"] = Helpers::transformNumFloat(*(int8_t *)(dv.value_p), dv.numeric_operator, fahrenheit);
                } else if ((dv.type == DeviceValueType::UINT) && Helpers::hasValue(*(uint8_t *)(dv.value_p))) {
                    obj["v"] = Helpers::transformNumFloat(*(uint8_t *)(dv.value_p), dv.numeric_operator, fahrenheit);
                } else if ((dv.type == DeviceValueType::SHORT) && Helpers::hasValue(*(int16_t *)(dv.value_p))) {
                    obj["v"] = Helpers::transformNumFloat(*(int16_t *)(dv.value_p), dv.numeric_operator, fahrenheit);
                } else if ((dv.type == DeviceValueType::USHORT) && Helpers::hasValue(*(uint16_t *)(dv.value_p))) {
                    obj["v"] = Helpers::transformNumFloat(*(uint16_t *)(dv.value_p), dv.numeric_operator, fahrenheit);
                } else if ((dv.type == DeviceValueType::ULONG) && Helpers::hasValue(*(uint32_t *)(dv.value_p))) {
                    obj["v"] = dv.numeric_operator > 0 ? *(uint32_t *)(dv.value_p) / dv.numeric_operator : *(uint32_t *)(dv.value_p);
                } else if ((dv.type == DeviceValueType::TIME) && Helpers::hasValue(*(uint32_t *)(dv.value_p))) {
                    obj["v"] = dv.numeric_operator > 0 ? *(uint32_t *)(dv.value_p) / dv.numeric_operator : *(uint32_t *)(dv.value_p);
                } else {
                    obj["v"] = ""; // must have a value for sorting to work
                }
            }

            // add the unit of measure (uom)
            obj["u"] = fahrenheit ? (uint8_t)DeviceValueUOM::FAHRENHEIT : dv.uom;

            auto mask = Helpers::hextoa((uint8_t)(dv.state >> 4), false); // create mask to a 2-char string

            // add name, prefixing the tag if it exists. This is the id used in the WebUI table and must be unique
            if (dv.has_tag()) {
                obj["id"] = mask + tag_to_string(dv.tag) + " " + fullname;
            } else {
                obj["id"] = mask + fullname;
            }

            // add commands and options
            if (dv.has_cmd && !dv.has_state(DeviceValueState::DV_READONLY)) {
                // add the name of the Command function
                if (dv.tag >= DeviceValueTAG::TAG_HC1) {
                    char c_s[50];
                    snprintf(c_s, sizeof(c_s), "%s/%s", tag_to_mqtt(dv.tag), dv.short_name);
                    obj["c"] = c_s;
                } else {
                    obj["c"] = dv.short_name;
                }

                // add the Command options
                if (dv.type == DeviceValueType::ENUM || (dv.type == DeviceValueType::CMD && dv.options_size > 1)) {
                    JsonArray l = obj.createNestedArray("l");
                    for (uint8_t i = 0; i < dv.options_size; i++) {
                        auto enum_str = Helpers::translated_word(dv.options[i]);
                        if (enum_str) {
                            l.add(enum_str);
                        }
                    }
                } else if (dv.type == DeviceValueType::BOOL) {
                    JsonArray l = obj.createNestedArray("l");
                    char      result[12];
                    l.add(Helpers::render_boolean(result, false, true));
                    l.add(Helpers::render_boolean(result, true, true));
                }
                // add command help template
                else if (dv.type == DeviceValueType::STRING || dv.type == DeviceValueType::CMD) {
                    if (dv.options_size == 1) {
                        obj["h"] = dv.options_single[0]; // NOT translated
                    }
                }
                // handle INTs
                // add steps to numeric values with numeric_operator
                else {
                    char s[10];
                    if (dv.numeric_operator > 0) {
                        obj["s"] = Helpers::render_value(s, (float)1 / dv.numeric_operator, 1);
                    } else if (dv.numeric_operator < 0) {
                        obj["s"] = Helpers::render_value(s, (float)(-1) * dv.numeric_operator, 0);
                    }

                    int16_t  dv_set_min;
                    uint16_t dv_set_max;
                    if (dv.get_min_max(dv_set_min, dv_set_max)) {
                        obj["m"] = Helpers::render_value(s, dv_set_min, 0);
                        obj["x"] = Helpers::render_value(s, dv_set_max, 0);
                    }
                }
            }
        }
    }
}

// as generate_values_web() but stripped down to only show all entities and their state
// this is used only for WebCustomizationService::device_entities()
void EMSdevice::generate_values_web_customization(JsonArray & output) {
    for (auto & dv : devicevalues_) {
        // also show commands and entities that have an empty full name
        JsonObject obj        = output.createNestedObject();
        uint8_t    fahrenheit = !EMSESP::system_.fahrenheit() ? 0 : (dv.uom == DeviceValueUOM::DEGREES) ? 2 : (dv.uom == DeviceValueUOM::DEGREES_R) ? 1 : 0;

        // create the value
        if (dv.hasValue()) {
            // handle Booleans (true, false), use strings, no native true/false)
            if (dv.type == DeviceValueType::BOOL) {
                auto value_b = (bool)*(uint8_t *)(dv.value_p);
                char s[12];
                obj["v"] = Helpers::render_boolean(s, value_b, true);
            }

            // handle TEXT strings
            else if (dv.type == DeviceValueType::STRING) {
                obj["v"] = (char *)(dv.value_p);
            }

            // handle ENUMs
            else if ((dv.type == DeviceValueType::ENUM) && (*(uint8_t *)(dv.value_p) < dv.options_size)) {
                obj["v"] = Helpers::translated_word(dv.options[*(uint8_t *)(dv.value_p)]);
            }

            // handle Integers and Floats
            else {
                if (dv.type == DeviceValueType::INT) {
                    obj["v"] = Helpers::transformNumFloat(*(int8_t *)(dv.value_p), dv.numeric_operator, fahrenheit);
                } else if (dv.type == DeviceValueType::UINT) {
                    obj["v"] = Helpers::transformNumFloat(*(uint8_t *)(dv.value_p), dv.numeric_operator, fahrenheit);
                } else if (dv.type == DeviceValueType::SHORT) {
                    obj["v"] = Helpers::transformNumFloat(*(int16_t *)(dv.value_p), dv.numeric_operator, fahrenheit);
                } else if (dv.type == DeviceValueType::USHORT) {
                    obj["v"] = Helpers::transformNumFloat(*(uint16_t *)(dv.value_p), dv.numeric_operator, fahrenheit);
                } else if (dv.type == DeviceValueType::ULONG) {
                    obj["v"] = dv.numeric_operator > 0 ? *(uint32_t *)(dv.value_p) / dv.numeric_operator : *(uint32_t *)(dv.value_p);
                } else if (dv.type == DeviceValueType::TIME) {
                    obj["v"] = dv.numeric_operator > 0 ? *(uint32_t *)(dv.value_p) / dv.numeric_operator : *(uint32_t *)(dv.value_p);
                }
            }
        }

        // id holds the shortname and must always have a value for the WebUI table to work
        if (dv.tag >= DeviceValueTAG::TAG_HC1) {
            char id_s[50];
            snprintf(id_s, sizeof(id_s), "%s/%s", tag_to_mqtt(dv.tag), dv.short_name);
            obj["id"] = id_s;
        } else {
            obj["id"] = dv.short_name;
        }

        // n is the fullname, and can be optional
        // don't add the fullname if its a command
        auto fullname = Helpers::translated_word(dv.fullname);
        if (dv.type != DeviceValueType::CMD) {
            if (fullname) {
                if (dv.has_tag()) {
                    char name[50];
                    snprintf(name, sizeof(name), "%s %s", tag_to_string(dv.tag), fullname);
                    obj["n"] = name;
                } else {
                    obj["n"] = fullname;
                }
            }

            // add the custom name, is optional
            std::string custom_fullname = dv.get_custom_fullname();
            if (!custom_fullname.empty()) {
                obj["cn"] = custom_fullname;
            }
        } else {
            obj["n"] = "!" + std::string(fullname); // prefix commands with a !
        }

        obj["m"] = dv.state >> 4; // send back the mask state. We're only interested in the high nibble
        obj["w"] = dv.has_cmd;    // if writable

        if (dv.has_cmd && (obj["v"].is<float>() || obj["v"].is<int>())) {
            // set the min and max values if there are any and if entity has a value
            int16_t  dv_set_min;
            uint16_t dv_set_max;
            if (dv.get_min_max(dv_set_min, dv_set_max)) {
                char s[10];
                obj["mi"] = Helpers::render_value(s, dv_set_min, 0, fahrenheit);
                obj["ma"] = Helpers::render_value(s, dv_set_max, 0, fahrenheit);
            }
        }
    }

    EMSESP::webCustomizationService.read([&](WebCustomization & settings) {
        for (EntityCustomization entityCustomization : settings.entityCustomizations) {
            if (entityCustomization.device_id == device_id()) {
                for (std::string entity_id : entityCustomization.entity_ids) {
                    uint8_t mask = Helpers::hextoint(entity_id.substr(0, 2).c_str());
                    if (mask & 0x80) {
                        JsonObject obj = output.createNestedObject();
                        obj["id"]      = DeviceValue::get_name(entity_id);
                        obj["m"]       = mask;
                        obj["w"]       = false;
                    }
                }
                break;
            }
        }
    });
}

void EMSdevice::set_climate_minmax(uint8_t tag, int16_t min, uint16_t max) {
    for (auto & dv : devicevalues_) {
        if (dv.tag == tag && (strcmp(dv.short_name, FL_(haclimate[0])) == 0)) {
            if (dv.min != min || dv.max != max) {
                dv.min = min;
                dv.max = max;
                dv.remove_state(DeviceValueState::DV_HA_CONFIG_CREATED);
            }
            return;
        }
    }
}

// set mask per device entity based on the id which is prefixed with the 2 char hex mask value
// returns true if the entity has a mask set (not 0 the default)
void EMSdevice::setCustomEntity(const std::string & entity_id) {
    for (auto & dv : devicevalues_) {
        char entity_name[70];
        if (dv.tag < DeviceValueTAG::TAG_HC1) {
            strncpy(entity_name, dv.short_name, sizeof(entity_name));
        } else {
            snprintf(entity_name, sizeof(entity_name), "%s/%s", tag_to_mqtt(dv.tag), dv.short_name);
        }

        // extra shortname
        auto        custom_name_pos = entity_id.find('|');
        bool        has_custom_name = (custom_name_pos != std::string::npos);
        std::string shortname       = has_custom_name ? entity_id.substr(2, custom_name_pos - 2) : entity_id.substr(2);

        if (entity_name == shortname) {
            // check the masks
            uint8_t current_mask = dv.state >> 4;
            uint8_t new_mask     = Helpers::hextoint(entity_id.substr(0, 2).c_str()); // first character contains mask flags

            // if it's a new mask, reconfigure HA
            if (Mqtt::ha_enabled() && (has_custom_name || ((current_mask ^ new_mask) & (DeviceValueState::DV_READONLY >> 4)))) {
                // remove ha config on change of dv_readonly flag
                dv.remove_state(DeviceValueState::DV_HA_CONFIG_CREATED);
                Mqtt::publish_ha_sensor_config(dv, "", "", true); // delete topic (remove = true)
            }

            // always write the mask
            dv.state = ((dv.state & 0x0F) | (new_mask << 4)); // set state high bits to flag

            // set the custom name if it has one, or clear it
            if (has_custom_name) {
                dv.custom_fullname = entity_id.substr(custom_name_pos + 1);
            } else {
                dv.custom_fullname = "";
            }

            auto min = dv.min;
            auto max = dv.max;

            // set the min / max
            dv.set_custom_minmax();

            if (Mqtt::ha_enabled() && dv.short_name == FL_(seltemp)[0] && (min != dv.min || max != dv.max)) {
                set_climate_minmax(dv.tag, dv.min, dv.max);
            }

            return;
        }
    }
}

// populate a string vector with entities that have masks set or have a custom name
void EMSdevice::getCustomEntities(std::vector<std::string> & entity_ids) {
    for (const auto & dv : devicevalues_) {
        char name[100];
        name[0] = '\0';
        if (dv.has_tag()) {
            // prefix tag
            strcpy(name, tag_to_mqtt(dv.tag));
            strcat(name, "/");
        }
        strcat(name, dv.short_name);
        std::string entity_name = name;

        uint8_t mask   = dv.state >> 4;
        bool    is_set = false;
        for (auto & eid : entity_ids) {
            if (DeviceValue::get_name(eid) == entity_name) {
                is_set = true;
                break;
            }
        }
        if (!is_set && (mask || !dv.custom_fullname.empty())) {
            if (dv.custom_fullname.empty()) {
                entity_ids.push_back(Helpers::hextoa(mask, false) + entity_name);
            } else {
                entity_ids.push_back(Helpers::hextoa(mask, false) + entity_name + "|" + dv.custom_fullname);
            }
        }
    }
}

#if defined(EMSESP_STANDALONE)
// dumps all entity values in native English
// the code is intended to run only once standalone, outside the ESP32 so not optimized for memory efficiency
// pipe symbols (|) are escaped so they can be converted to Markdown in the Wiki
// format is: device name,device type,product id,shortname,fullname,type [options...] \\| (min/max),uom,writeable,discovery entityid v3.4, discovery entityid
void EMSdevice::dump_value_info() {
    for (auto & dv : devicevalues_) {
        if (dv.fullname != nullptr) {
            Serial.print(name_);
            Serial.print(',');
            Serial.print(device_type_name());
            Serial.print(',');

            Serial.print(product_id_);
            Serial.print(',');

            Serial.print(dv.short_name);
            Serial.print(',');

            Serial.print(dv.fullname[0]);
            Serial.print(',');

            // per type
            switch (dv.type) {
            case DeviceValueType::ENUM:
            case DeviceValueType::CMD:
                if (dv.type == DeviceValueType::ENUM) {
                    Serial.print("enum");
                } else {
                    Serial.print("cmd");
                }

                Serial.print(" [");
                for (uint8_t i = 0; i < dv.options_size; i++) {
                    Serial.print(dv.options[i][0]);
                    if (i < dv.options_size - 1) {
                        Serial.print("\\|");
                    }
                }
                Serial.print(']');
                break;

            case DeviceValueType::USHORT:
                Serial.print("ushort");
                break;

            case DeviceValueType::UINT:
                Serial.print("uint");
                break;

            case DeviceValueType::SHORT:
                Serial.print("short");
                break;

            case DeviceValueType::INT:
                Serial.print("int");
                break;

            case DeviceValueType::ULONG:
                Serial.print("ulong");
                break;

            case DeviceValueType::BOOL:
                Serial.print("boolean");
                break;

            case DeviceValueType::TIME:
                Serial.print("time");
                break;

            case DeviceValueType::STRING:
                Serial.print("string");
                break;

            default:
                break;
            }

            // min/max range
            int16_t  dv_set_min;
            uint16_t dv_set_max;
            if (dv.get_min_max(dv_set_min, dv_set_max)) {
                Serial.print(" (>=");
                Serial.print(dv_set_min);
                Serial.print("<=");
                Serial.print(dv_set_max);
                Serial.print(")");
            }
            Serial.print(",");

            // uom
            if (dv.uom == DeviceValue::DeviceValueUOM::DEGREES || dv.uom == DeviceValue::DeviceValueUOM::DEGREES_R) {
                Serial.print('C'); // the degrees symbol doesn't print nicely in XLS
            } else {
                Serial.print(DeviceValue::DeviceValueUOM_s[dv.uom]);
            }
            Serial.print(",");

            // writeable flag
            Serial.print(dv.has_cmd ? "true" : "false");
            Serial.print(",");

            // MQTT Discovery entity name
            // do this twice for the old and new formats
            char entity_with_tag[200];
            char entityid[500];
            char entity_name[100];

            for (uint8_t count = 0; count < 2; count++) {
                if (count) {
                    // new name, comes as last
                    Serial.print(",");
                    strcpy(entity_name, dv.short_name);
                } else {
                    // old format, comes first
                    char uniq_s[100];
                    strlcpy(uniq_s, dv.fullname[0], sizeof(uniq_s));
                    Helpers::replace_char(uniq_s, ' ', '_');
                    strcpy(entity_name, uniq_s);
                }

                if (dv.tag >= DeviceValueTAG::TAG_HC1) {
                    snprintf(entity_with_tag,
                             sizeof(entity_with_tag),
                             "%s_%s_%s",
                             device_type_2_device_name(device_type_),
                             EMSdevice::tag_to_mqtt(dv.tag),
                             entity_name);
                } else {
                    snprintf(entity_with_tag, sizeof(entity_with_tag), "%s_%s", device_type_2_device_name(device_type_), entity_name);
                }

                if (dv.has_cmd) {
                    switch (dv.type) {
                    case DeviceValueType::INT:
                    case DeviceValueType::UINT:
                    case DeviceValueType::SHORT:
                    case DeviceValueType::USHORT:
                    case DeviceValueType::ULONG:
                        snprintf(entityid, sizeof(entityid), "number.%s", entity_with_tag);
                        break;
                    case DeviceValueType::BOOL:
                        snprintf(entityid, sizeof(entityid), "switch.%s", entity_with_tag);
                        break;
                    case DeviceValueType::ENUM:
                        snprintf(entityid, sizeof(entityid), "select.%s", entity_with_tag);
                        break;
                    default:
                        snprintf(entityid, sizeof(entityid), "sensor.%s", entity_with_tag);
                        break;
                    }
                } else {
                    if (dv.type == DeviceValueType::BOOL) {
                        snprintf(entityid, sizeof(entityid), "binary_sensor.%s", entity_with_tag); // binary sensor (for booleans)
                    } else {
                        snprintf(entityid, sizeof(entityid), "sensor.%s", entity_with_tag); // normal HA sensor
                    }
                }

                Serial.print(entityid);
            }

            Serial.println();
        }
    }
}
#endif

// builds json for a specific device value / entity
// cmd is the endpoint or name of the device entity
// returns false if failed, otherwise true
bool EMSdevice::get_value_info(JsonObject & output, const char * cmd, const int8_t id) {
    JsonObject json = output;
    int8_t     tag  = id;

    // check if we have hc or wwc or hs
    if (id >= 1 && id <= (1 + DeviceValueTAG::TAG_HS16 - DeviceValueTAG::TAG_HC1)) {
        tag = DeviceValueTAG::TAG_HC1 + id - 1;
    }

    // make a copy of the string command for parsing
    char command_s[30];
    strlcpy(command_s, cmd, sizeof(command_s));
    char * attribute_s = nullptr;

    // check specific attribute to fetch instead of the complete record
    char * breakp = strchr(command_s, '/');
    if (breakp) {
        *breakp     = '\0';
        attribute_s = breakp + 1;
    }

    // search device value with this tag
    for (auto & dv : devicevalues_) {
        if (Helpers::toLower(command_s) == Helpers::toLower(dv.short_name) && (tag <= 0 || tag == dv.tag)) {
            uint8_t fahrenheit = !EMSESP::system_.fahrenheit() ? 0 : (dv.uom == DeviceValueUOM::DEGREES) ? 2 : (dv.uom == DeviceValueUOM::DEGREES_R) ? 1 : 0;

            const char * type  = "type";
            const char * value = "value";

            json["name"] = dv.short_name;

            auto fullname = dv.get_fullname();
            if (!fullname.empty()) {
                if (dv.has_tag()) {
                    char name[50];
                    snprintf(name, sizeof(name), "%s %s", tag_to_string(dv.tag), fullname.c_str());
                    json["name"] = name;
                } else {
                    json["fullname"] = fullname;
                }
            }

            if (dv.tag != DeviceValueTAG::TAG_NONE) {
                json["circuit"] = tag_to_mqtt(dv.tag);
            }

            char val[10];
            switch (dv.type) {
            case DeviceValueType::ENUM: {
                if (*(uint8_t *)(dv.value_p) < dv.options_size) {
                    if (EMSESP::system_.enum_format() == ENUM_FORMAT_INDEX) {
                        json[value] = (uint8_t)(*(uint8_t *)(dv.value_p));
                    } else {
                        json[value] = Helpers::translated_word(dv.options[*(uint8_t *)(dv.value_p)]); // text
                    }
                }
                json[type]      = F_(enum);
                JsonArray enum_ = json.createNestedArray(F_(enum));
                for (uint8_t i = 0; i < dv.options_size; i++) {
                    enum_.add(Helpers::translated_word(dv.options[i]));
                }
                break;
            }

            case DeviceValueType::USHORT:
                if (Helpers::hasValue(*(uint16_t *)(dv.value_p))) {
                    json[value] = serialized(Helpers::render_value(val, *(uint16_t *)(dv.value_p), dv.numeric_operator, fahrenheit));
                }
                json[type] = F_(number);
                break;

            case DeviceValueType::UINT:
                if (Helpers::hasValue(*(uint8_t *)(dv.value_p))) {
                    json[value] = serialized(Helpers::render_value(val, *(uint8_t *)(dv.value_p), dv.numeric_operator, fahrenheit));
                }
                json[type] = F_(number);
                break;

            case DeviceValueType::SHORT:
                if (Helpers::hasValue(*(int16_t *)(dv.value_p))) {
                    json[value] = serialized(Helpers::render_value(val, *(int16_t *)(dv.value_p), dv.numeric_operator, fahrenheit));
                }
                json[type] = F_(number);
                break;

            case DeviceValueType::INT:
                if (Helpers::hasValue(*(int8_t *)(dv.value_p))) {
                    json[value] = serialized(Helpers::render_value(val, *(int8_t *)(dv.value_p), dv.numeric_operator, fahrenheit));
                }
                json[type] = F_(number);
                break;

            case DeviceValueType::ULONG:
                if (Helpers::hasValue(*(uint32_t *)(dv.value_p))) {
                    json[value] = serialized(Helpers::render_value(val, *(uint32_t *)(dv.value_p), dv.numeric_operator));
                }
                json[type] = F_(number);
                break;

            case DeviceValueType::BOOL:
                if (Helpers::hasValue(*(uint8_t *)(dv.value_p), EMS_VALUE_BOOL)) {
                    auto value_b = (bool)*(uint8_t *)(dv.value_p);
                    if (EMSESP::system_.bool_format() == BOOL_FORMAT_TRUEFALSE) {
                        json[value] = value_b;
                    } else if (EMSESP::system_.bool_format() == BOOL_FORMAT_10) {
                        json[value] = value_b ? 1 : 0;
                    } else {
                        char s[12];
                        json[value] = Helpers::render_boolean(s, value_b);
                    }
                }
                json[type] = ("boolean");
                break;

            case DeviceValueType::TIME:
                if (Helpers::hasValue(*(uint32_t *)(dv.value_p))) {
                    json[value] = serialized(Helpers::render_value(val, *(uint32_t *)(dv.value_p), dv.numeric_operator));
                }
                json[type] = F_(number);
                break;

            case DeviceValueType::STRING:
                if (Helpers::hasValue((char *)(dv.value_p))) {
                    json[value] = (char *)(dv.value_p);
                }
                json[type] = ("string");
                break;

            case DeviceValueType::CMD:
                json[type] = F_(command);
                if (dv.options_size > 1) {
                    JsonArray enum_ = json.createNestedArray(F_(enum));
                    for (uint8_t i = 0; i < dv.options_size; i++) {
                        enum_.add(Helpers::translated_word(dv.options[i]));
                    }
                }
                break;

            default:
                json[type] = Helpers::translated_word(FL_(unknown));
                break;
            }

            // set the min and max only for commands
            if (dv.has_cmd) {
                int16_t  dv_set_min;
                uint16_t dv_set_max;
                if (dv.get_min_max(dv_set_min, dv_set_max)) {
                    json["min"] = dv_set_min;
                    json["max"] = dv_set_max;
                }
            }

            // add uom if it's not a " " (single space)
            if (dv.uom != DeviceValueUOM::NONE) {
                json["uom"] = uom_to_string(dv.uom);
            }

            json["readable"]  = !dv.has_state(DeviceValueState::DV_API_MQTT_EXCLUDE);
            json["writeable"] = dv.has_cmd && !dv.has_state(DeviceValueState::DV_READONLY);
            json["visible"]   = !dv.has_state(DeviceValueState::DV_WEB_EXCLUDE);

            // if there is no value, mention it
            if (!json.containsKey(value)) {
                json[value] = "not set";
            }

            // if we're filtering on an attribute, go find it
            if (attribute_s) {
#if defined(EMSESP_DEBUG)
                EMSESP::logger().debug("Attribute '%s'", attribute_s);
#endif
                if (json.containsKey(attribute_s)) {
                    JsonVariant data = json[attribute_s];
                    output.clear();
                    output["api_data"] = data;
                    return true;
                } else {
                    return false;
                }
            }

            return true;
        }
    }

    return false;
}

// mqtt publish all single values from one device (used for time schedule)
void EMSdevice::publish_all_values() {
    for (const auto & dv : devicevalues_) {
        publish_value(dv.value_p);
    }
}

// For each value in the device create the json object pair and add it to given json
// return false if empty
// this is used to create the MQTT payloads, Console messages and Web API calls
bool EMSdevice::generate_values(JsonObject & output, const uint8_t tag_filter, const bool nested, const uint8_t output_target) {
    bool       has_values = false; // to see if we've added a value. it's faster than doing a json.size() at the end
    uint8_t    old_tag    = 255;   // NAN
    JsonObject json       = output;

    for (auto & dv : devicevalues_) {
        // check if it exists, there is a value for the entity. Set the flag to ACTIVE
        // not that this will override any previously removed states
        (dv.hasValue()) ? dv.add_state(DeviceValueState::DV_ACTIVE) : dv.remove_state(DeviceValueState::DV_ACTIVE);

        auto fullname = dv.get_fullname();

        // check conditions:
        //  1. it must have a valid value (state is active)
        //  2. it must have a visible flag
        //  3. it must match the given tag filter or have an empty tag
        //  4. it must not have the exclude flag set or outputs to console
        if (dv.has_state(DeviceValueState::DV_ACTIVE) && !fullname.empty() && (tag_filter == DeviceValueTAG::TAG_NONE || tag_filter == dv.tag)
            && (output_target == OUTPUT_TARGET::CONSOLE || !dv.has_state(DeviceValueState::DV_API_MQTT_EXCLUDE))) {
            has_values = true; // flagged if we actually have data

            // we have a tag if it matches the filter given, and that the tag name is not empty/""
            bool have_tag = ((dv.tag != tag_filter) && dv.has_tag());

            // create the name for the JSON key
            char name[80];

            if (output_target == OUTPUT_TARGET::API_VERBOSE || output_target == OUTPUT_TARGET::CONSOLE) {
                char short_name[20];
                if (output_target == OUTPUT_TARGET::CONSOLE) {
                    snprintf(short_name, sizeof(short_name), " (%s)", dv.short_name);
                } else {
                    strcpy(short_name, "");
                }
                if (have_tag) {
                    snprintf(name, sizeof(name), "%s %s%s", tag_to_string(dv.tag), fullname.c_str(), short_name); // prefix the tag
                } else {
                    snprintf(name, sizeof(name), "%s%s", fullname.c_str(), short_name);
                }
            } else {
                strlcpy(name, (dv.short_name), sizeof(name)); // use short name

                // if we have a tag, and its different to the last one create a nested object. only for hc, wwc and hs
                if (dv.tag != old_tag) {
                    old_tag = dv.tag;
                    if (nested && have_tag && dv.tag >= DeviceValueTAG::TAG_HC1) {
                        json = output.createNestedObject(tag_to_mqtt(dv.tag));
                    }
                }
            }

            // handle Booleans
            if (dv.type == DeviceValueType::BOOL && Helpers::hasValue(*(uint8_t *)(dv.value_p), EMS_VALUE_BOOL)) {
                // see how to render the value depending on the setting
                auto value_b = (bool)*(uint8_t *)(dv.value_p);
                if (output_target == OUTPUT_TARGET::CONSOLE) {
                    char s[12];
                    json[name] = Helpers::render_boolean(s, value_b, true); // console use web settings
                } else if (EMSESP::system_.bool_format() == BOOL_FORMAT_TRUEFALSE) {
                    json[name] = value_b;
                } else if (EMSESP::system_.bool_format() == BOOL_FORMAT_10) {
                    json[name] = value_b ? 1 : 0;
                } else {
                    char s[12];
                    json[name] = Helpers::render_boolean(s, value_b);
                }
            }

            // handle TEXT strings
            else if (dv.type == DeviceValueType::STRING) {
                json[name] = (char *)(dv.value_p);
            }

            // handle ENUMs
            else if ((dv.type == DeviceValueType::ENUM) && (*(uint8_t *)(dv.value_p) < dv.options_size)) {
                // check for numeric enum-format, console use text format
                if (EMSESP::system_.enum_format() == ENUM_FORMAT_INDEX && output_target != OUTPUT_TARGET::CONSOLE) {
                    json[name] = (uint8_t)(*(uint8_t *)(dv.value_p));
                } else {
                    json[name] = Helpers::translated_word(dv.options[*(uint8_t *)(dv.value_p)]);
                }
            }

            // handle Numbers
            else {
                // fahrenheit, 0 is no conversion other 1 or 2. not sure why?
                uint8_t fahrenheit = !EMSESP::system_.fahrenheit()           ? 0
                                     : (dv.uom == DeviceValueUOM::DEGREES)   ? 2
                                     : (dv.uom == DeviceValueUOM::DEGREES_R) ? 1
                                                                             : 0;
                char    val[10];
                if (dv.type == DeviceValueType::INT) {
                    json[name] = serialized(Helpers::render_value(val, *(int8_t *)(dv.value_p), dv.numeric_operator, fahrenheit));
                } else if (dv.type == DeviceValueType::UINT) {
                    json[name] = serialized(Helpers::render_value(val, *(uint8_t *)(dv.value_p), dv.numeric_operator, fahrenheit));
                } else if (dv.type == DeviceValueType::SHORT) {
                    json[name] = serialized(Helpers::render_value(val, *(int16_t *)(dv.value_p), dv.numeric_operator, fahrenheit));
                } else if (dv.type == DeviceValueType::USHORT) {
                    json[name] = serialized(Helpers::render_value(val, *(uint16_t *)(dv.value_p), dv.numeric_operator, fahrenheit));
                } else if (dv.type == DeviceValueType::ULONG) {
                    json[name] = serialized(Helpers::render_value(val, *(uint32_t *)(dv.value_p), dv.numeric_operator));
                } else if ((dv.type == DeviceValueType::TIME) && Helpers::hasValue(*(uint32_t *)(dv.value_p))) {
                    uint32_t time_value = *(uint32_t *)(dv.value_p);
                    if (dv.numeric_operator == DeviceValueNumOp::DV_NUMOP_DIV60) {
                        time_value /= 60; // sometimes we need to divide by 60
                    }
                    if (output_target == OUTPUT_TARGET::API_VERBOSE || output_target == OUTPUT_TARGET::CONSOLE) {
                        char time_s[60];
                        snprintf(time_s,
                                 sizeof(time_s),
                                 "%d %s %d %s %d %s",
                                 (time_value / 1440),
                                 Helpers::translated_word(FL_(days)),
                                 ((time_value % 1440) / 60),
                                 Helpers::translated_word(FL_(hours)),
                                 (time_value % 60),
                                 Helpers::translated_word(FL_(minutes)));
                        json[name] = time_s;
                    } else {
                        json[name] = time_value;
                    }
                }

                // commenting out - we don't want Commands in MQTT or Console
                //  else if (dv.type == DeviceValueType::CMD && output_target != EMSdevice::OUTPUT_TARGET::MQTT) {
                //     json[name] = "";
                // }

                // check for value outside min/max range and adapt the limits to avoid HA complains
                // Should this also check for api output?
                if ((output_target == OUTPUT_TARGET::MQTT) && (dv.min != 0 || dv.max != 0)) {
                    if (json[name].is<float>() || json[name].is<int>()) {
                        int v = json[name];
                        if (fahrenheit) {
                            v = (v - (32 * (fahrenheit - 1))) / 1.8; // reset to °C
                        }
                        if (v < dv.min) {
                            dv.min = v;
                            dv.remove_state(DeviceValueState::DV_HA_CONFIG_CREATED);
                        } else if (v > dv.max) {
                            dv.max = v;
                            dv.remove_state(DeviceValueState::DV_HA_CONFIG_CREATED);
                        }
                    }
                }
            }
        }
    }

    return has_values;
}

// create the Home Assistant configs for each device value / entity
// this is called when an MQTT publish is done via an EMS Device in emsesp.cpp::publish_device_values()
void EMSdevice::mqtt_ha_entity_config_create() {
    bool create_device_config = !ha_config_done(); // do we need to create the main Discovery device config with this entity?

    // check the state of each of the device values
    // create climate if roomtemp is visible
    // create the discovery topic if if hasn't already been created, not a command (like reset) and is active and visible
    for (auto & dv : devicevalues_) {
        if (!strcmp(dv.short_name, FL_(haclimate)[0]) && !dv.has_state(DeviceValueState::DV_API_MQTT_EXCLUDE) && dv.has_state(DeviceValueState::DV_ACTIVE)) {
            if (*(int8_t *)(dv.value_p) == 1 && (!dv.has_state(DeviceValueState::DV_HA_CONFIG_CREATED) || dv.has_state(DeviceValueState::DV_HA_CLIMATE_NO_RT))) {
                dv.remove_state(DeviceValueState::DV_HA_CLIMATE_NO_RT);
                dv.add_state(DeviceValueState::DV_HA_CONFIG_CREATED);
                Mqtt::publish_ha_climate_config(dv.tag, true, false, dv.min, dv.max); // roomTemp
            } else if (*(int8_t *)(dv.value_p) == 0
                       && (!dv.has_state(DeviceValueState::DV_HA_CONFIG_CREATED) || !dv.has_state(DeviceValueState::DV_HA_CLIMATE_NO_RT))) {
                dv.add_state(DeviceValueState::DV_HA_CLIMATE_NO_RT);
                dv.add_state(DeviceValueState::DV_HA_CONFIG_CREATED);
                Mqtt::publish_ha_climate_config(dv.tag, false, false, dv.min, dv.max); // no roomTemp
            }
        }

        if (!dv.has_state(DeviceValueState::DV_HA_CONFIG_CREATED) && (dv.type != DeviceValueType::CMD) && dv.has_state(DeviceValueState::DV_ACTIVE)
            && !dv.has_state(DeviceValueState::DV_API_MQTT_EXCLUDE)) {
            // create_device_config is only done once for the EMS device. It can added to any entity, so we take the first
            Mqtt::publish_ha_sensor_config(dv, name(), brand_to_char(), false, create_device_config);
            dv.add_state(DeviceValueState::DV_HA_CONFIG_CREATED);
            create_device_config = false; // only create the main config once
        }

#ifndef EMSESP_STANDALONE
        if (ESP.getFreeHeap() < (65 * 1024)) {
            break;
        }
#endif
    }

    ha_config_done(!create_device_config);
}

// remove all config topics in HA
void EMSdevice::ha_config_clear() {
    for (auto & dv : devicevalues_) {
        dv.remove_state(DeviceValueState::DV_HA_CONFIG_CREATED);
    }

    ha_config_done(false); // this will force the recreation of the main HA device config
}

bool EMSdevice::has_telegram_id(uint16_t id) const {
    for (const auto & tf : telegram_functions_) {
        if (tf.telegram_type_id_ == id) {
            return true;
        }
    }
    return false;
}

// return the name of the telegram type
const char * EMSdevice::telegram_type_name(std::shared_ptr<const Telegram> telegram) {
    // see if it's one of the common ones, like Version
    if (telegram->type_id == EMS_TYPE_VERSION) {
        return "Version";
    } else if (telegram->type_id == EMS_TYPE_UBADevices) {
        return "UBADevices";
    }

    for (const auto & tf : telegram_functions_) {
        if ((tf.telegram_type_id_ == telegram->type_id) && (telegram->type_id != 0xFF)) {
            return tf.telegram_type_name_;
        }
    }

    return "";
}

// take a telegram_type_id and call the matching handler
// return true if match found
bool EMSdevice::handle_telegram(std::shared_ptr<const Telegram> telegram) {
    for (auto & tf : telegram_functions_) {
        if (tf.telegram_type_id_ == telegram->type_id) {
            // if the data block is empty and we have not received data before, assume that this telegram
            // is not recognized by the bus master. So remove it from the automatic fetch list
            if (telegram->message_length == 0 && telegram->offset == 0 && !tf.received_) {
#if defined(EMSESP_DEBUG)
                EMSESP::logger().debug("This telegram (%s) is not recognized by the EMS bus", tf.telegram_type_name_);
#endif
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
void EMSdevice::write_command(const uint16_t type_id, const uint8_t offset, uint8_t * message_data, const uint8_t message_length, const uint16_t validate_typeid) const {
    EMSESP::send_write_request(type_id, device_id(), offset, message_data, message_length, validate_typeid);
}

// send Tx write with a single value
void EMSdevice::write_command(const uint16_t type_id, const uint8_t offset, const uint8_t value, const uint16_t validate_typeid) const {
    EMSESP::send_write_request(type_id, device_id(), offset, value, validate_typeid);
}

// send Tx write with a single value, with no post validation
void EMSdevice::write_command(const uint16_t type_id, const uint8_t offset, const uint8_t value) const {
    EMSESP::send_write_request(type_id, device_id(), offset, value, 0);
}

// send Tx read command to the device
void EMSdevice::read_command(const uint16_t type_id, const uint8_t offset, const uint8_t length) const {
    EMSESP::send_read_request(type_id, device_id(), offset, length);
}

} // namespace emsesp
