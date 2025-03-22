/*
 * EMS-ESP - https://github.com/emsesp/EMS-ESP
 * Copyright 2020-2024  emsesp.org - proddy, MichaelDvP
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

// returns number of visible device values (entities) for this device, for the Devices page
// this includes commands since they can also be entities and visible in the web UI
uint8_t EMSdevice::count_entities() {
    uint8_t count = 0;
    for (const auto & dv : devicevalues_) {
        // if (!dv.has_state(DeviceValueState::DV_WEB_EXCLUDE) && dv.hasValue()) {
        if (dv.hasValue()) {
            count++;
        }
    }
    return count;
}

// count favorites, used in Dashboard
uint8_t EMSdevice::count_entities_fav() {
    uint8_t count = 0;
    for (const auto & dv : devicevalues_) {
        count += dv.has_state(DeviceValueState::DV_FAVORITE) && dv.hasValue();
    }
    return count;
}

// see if there are customized entities, excluding any commands
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
const char * EMSdevice::tag_to_string(int8_t tag, const bool translate) {
    int8_t tag_n = tag > DeviceValue::NUM_TAGS ? 0 : tag;
    return (translate ? Helpers::translated_word(DeviceValue::DeviceValueTAG_s[tag_n]) : DeviceValue::DeviceValueTAG_s[tag_n][0]);
}

const char * EMSdevice::tag_to_mqtt(int8_t tag) {
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

// returns the short name of the device, used in MQTT and console commands, all lowercase, no translated
const char * EMSdevice::device_type_2_device_name(const uint8_t device_type) {
    switch (device_type) {
    case DeviceType::SYSTEM:
        return F_(system);
    case DeviceType::TEMPERATURESENSOR:
        return F_(temperaturesensor);
    case DeviceType::ANALOGSENSOR:
        return F_(analogsensor);
    case DeviceType::SCHEDULER:
        return F_(scheduler);
    case DeviceType::CUSTOM:
        return F_(custom);
    case DeviceType::BOILER:
        return F_(boiler);
    case DeviceType::THERMOSTAT:
        return F_(thermostat);
    case DeviceType::MIXER:
        return F_(mixer);
    case DeviceType::SOLAR:
        return F_(solar);
    case DeviceType::HEATPUMP:
        return F_(heatpump);
    case DeviceType::GATEWAY:
        return F_(gateway);
    case DeviceType::SWITCH:
        return F_(switcher);
    case DeviceType::CONTROLLER:
        return F_(controller);
    case DeviceType::CONNECT:
        return F_(connect);
    case DeviceType::ALERT:
        return F_(alert);
    case DeviceType::EXTENSION:
        return F_(extension);
    case DeviceType::GENERIC:
        return F_(generic);
    case DeviceType::HEATSOURCE:
        return F_(heatsource);
    case DeviceType::VENTILATION:
        return F_(ventilation);
    case DeviceType::WATER:
        return F_(water);
    case DeviceType::POOL:
        return F_(pool);
    default:
        return Helpers::translated_word(FL_(unknown), true);
    }
}

// returns the translated name of a specific EMS device
// excludes temperaturesensor, analogsensor and system
const char * EMSdevice::device_type_2_device_name_translated() {
    switch (device_type_) {
    case DeviceType::BOILER:
        if (flags_ == EMS_DEVICE_FLAG_HEATPUMP) {
            return Helpers::translated_word(FL_(boiler_hp_device));
        }
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
    case DeviceType::EXTENSION:
        return Helpers::translated_word(FL_(extension_device));
    case DeviceType::HEATSOURCE:
        return Helpers::translated_word(FL_(heatsource_device));
    case DeviceType::VENTILATION:
        return Helpers::translated_word(FL_(ventilation_device));
    case DeviceType::WATER:
        return Helpers::translated_word(FL_(water_device));
    case DeviceType::POOL:
        return Helpers::translated_word(FL_(pool_device));
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
    if (!strcmp(lowtopic, F_(switcher))) {
        return DeviceType::SWITCH;
    }
    if (!strcmp(lowtopic, F_(gateway))) {
        return DeviceType::GATEWAY;
    }
    if (!strcmp(lowtopic, F_(alert))) {
        return DeviceType::ALERT;
    }
    if (!strcmp(lowtopic, F_(extension))) {
        return DeviceType::EXTENSION;
    }
    if (!strcmp(lowtopic, F_(heatsource))) {
        return DeviceType::HEATSOURCE;
    }
    if (!strcmp(lowtopic, F_(ventilation))) {
        return DeviceType::VENTILATION;
    }
    if (!strcmp(lowtopic, F_(water))) {
        return DeviceType::WATER;
    }
    if (!strcmp(lowtopic, F_(pool))) {
        return DeviceType::POOL;
    }

    // non EMS
    if (!strcmp(lowtopic, F_(custom))) {
        return DeviceType::CUSTOM;
    }
    if (!strcmp(lowtopic, F_(temperaturesensor))) {
        return DeviceType::TEMPERATURESENSOR;
    }
    if (!strcmp(lowtopic, F_(analogsensor))) {
        return DeviceType::ANALOGSENSOR;
    }
    if (!strcmp(lowtopic, F_(scheduler))) {
        return DeviceType::SCHEDULER;
    }
    if (!strcmp(lowtopic, F_(system))) {
        return DeviceType::SYSTEM;
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
std::string EMSdevice::to_string() {
    // for devices that haven't been lookup yet, don't show all details
    if (product_id_ == 0) {
        return std::string(name()) + " (DeviceID:" + Helpers::hextoa(device_id_) + ")";
    }

    if (brand_ == Brand::NO_BRAND) {
        return std::string(name()) + " (DeviceID:" + Helpers::hextoa(device_id_) + ", ProductID:" + Helpers::itoa(product_id_) + ", Version:" + version_ + ")";
    }

    return std::string(brand_to_char()) + " " + name() + " (DeviceID:" + Helpers::hextoa(device_id_) + ", ProductID:" + Helpers::itoa(product_id_)
           + ", Version:" + version_ + ")";
}

// returns out brand + device name
// translated
std::string EMSdevice::to_string_short() {
    if (brand_ == Brand::NO_BRAND) {
        return std::string(device_type_2_device_name_translated()) + ": " + name();
    }

    return std::string(device_type_2_device_name_translated()) + ": " + brand_to_char() + " " + name();
}

// for each telegram that has the fetch value set (true) do a read request
void EMSdevice::fetch_values() {
    if (!active_) {
        return;
    }
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
    EMSESP::logger().debug("Setting fetch to %d for deviceID 0x%02X, telegramID 0x%02X", toggle, device_id(), telegram_id);
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

// get receive status of telegramID
bool EMSdevice::is_received(uint16_t telegram_id) const {
    for (const auto & tf : telegram_functions_) {
        if (tf.telegram_type_id_ == telegram_id) {
            return tf.received_;
        }
    }
    return false;
}

// check for a tag to create a nest
bool EMSdevice::has_tags(const int8_t tag) const {
    for (const auto & dv : devicevalues_) {
        if (dv.tag == tag && tag >= DeviceValueTAG::TAG_HC1) {
            return true;
        }
    }
    return false;
}

// check if the device has a command with this tag.
bool EMSdevice::has_cmd(const char * cmd, const int8_t id) const {
    for (const auto & dv : devicevalues_) {
        if ((id < 1 || dv.tag == id) && dv.has_cmd && strcmp(dv.short_name, cmd) == 0) {
            return true;
        }
    }
    return false;
}

// list of registered device entries
// called from the command 'entities'
/*
void EMSdevice::list_device_entries(JsonObject output) const {
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

            JsonArray details = output[key].to<JsonArray>();

            // add the full name description
            details.add(fullname);

            // add uom
            if (dv.uom != DeviceValueUOM::NONE) {
                details.add(EMSdevice::uom_to_string(dv.uom));
            }
        }
    }
}
*/

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
// this function will also apply any customizations to the entity
void EMSdevice::add_device_value(int8_t                tag,              // to be used to group mqtt together, either as separate topics as a nested object
                                 void *                value_p,          // pointer to the value from the .h file
                                 uint8_t               type,             // one of DeviceValueType
                                 const char * const ** options,          // options for enum, which are translated as a list of lists
                                 const char * const *  options_single,   // list of names
                                 int8_t                numeric_operator, // to divide or multiply, see DeviceValueNumOps::
                                 const char * const *  name,             // list of names, including shortname and translations
                                 uint8_t               uom,              // unit of measure from DeviceValueUOM
                                 const cmd_function_p  f,                // command function pointer
                                 int16_t               min,              // min allowed value
                                 uint32_t              max               // max allowed value
) {
    // initialize the device value depending on it's type

    if (type == DeviceValueType::STRING) {
        *(char *)(value_p) = {'\0'}; // this is important for string functions like strlen() to work later
    } else if (type == DeviceValueType::INT8) {
        *(int8_t *)(value_p) = System::test_set_all_active() ? EMS_VALUE_DEFAULT_INT8_DUMMY : EMS_VALUE_DEFAULT_INT8;
    } else if (type == DeviceValueType::UINT8) {
        *(uint8_t *)(value_p) = System::test_set_all_active() ? EMS_VALUE_DEFAULT_UINT8_DUMMY : EMS_VALUE_DEFAULT_UINT8;
    } else if (type == DeviceValueType::INT16) {
        *(int16_t *)(value_p) = System::test_set_all_active() ? EMS_VALUE_DEFAULT_INT16_DUMMY : EMS_VALUE_DEFAULT_INT16;
    } else if (type == DeviceValueType::UINT16) {
        *(uint16_t *)(value_p) = System::test_set_all_active() ? EMS_VALUE_DEFAULT_UINT16_DUMMY : EMS_VALUE_DEFAULT_UINT16;
    } else if ((type == DeviceValueType::UINT24) || (type == DeviceValueType::TIME) || (type == DeviceValueType::UINT32)) {
        *(uint32_t *)(value_p) = System::test_set_all_active() ? EMS_VALUE_DEFAULT_UINT24_DUMMY : EMS_VALUE_DEFAULT_UINT24;
    } else if (type == DeviceValueType::BOOL) {
        *(int8_t *)(value_p) = System::test_set_all_active() ? EMS_VALUE_DEFAULT_BOOL_DUMMY : EMS_VALUE_DEFAULT_BOOL; // bool is uint8_t, but other initial value
    } else if (type == DeviceValueType::ENUM) {
        *(uint8_t *)(value_p) = System::test_set_all_active() ? EMS_VALUE_DEFAULT_ENUM_DUMMY : EMS_VALUE_DEFAULT_ENUM; // enums behave as uint8_t
    } else if (type == DeviceValueType::CMD) {
        if (uom == DeviceValueUOM::NONE) {
            *(uint8_t *)(value_p) = System::test_set_all_active() ? EMS_VALUE_DEFAULT_ENUM_DUMMY : EMS_VALUE_DEFAULT_ENUM; // enums behave as uint8_t
        } else if (uom == DeviceValueUOM::DEGREES) {
            *(int16_t *)(value_p) = System::test_set_all_active() ? EMS_VALUE_DEFAULT_INT16_DUMMY : EMS_VALUE_DEFAULT_INT16;
        } else if (uom == DeviceValueUOM::PERCENT) {
            *(uint8_t *)(value_p) = System::test_set_all_active() ? EMS_VALUE_DEFAULT_UINT8_DUMMY : EMS_VALUE_DEFAULT_UINT8;
        }
    }

    uint8_t state           = DeviceValueState::DV_DEFAULT; // determine state
    auto    custom_fullname = std::string("");              // custom fullname
    auto    short_name      = name[0];                      // entity name
    bool    has_cmd         = (f != nullptr);               // is it a command?
    bool    ignore          = false;                        // ignore this entity?

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
                    strncpy(entity, short_name, sizeof(entity) - 1);
                } else {
                    snprintf(entity, sizeof(entity), "%s/%s", tag_to_mqtt(tag), short_name);
                }

                for (const std::string & entity_id : entityCustomization.entity_ids) {
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
            flags |= CommandFlag::CMD_FLAG_HC;
        } else if (tag >= DeviceValueTAG::TAG_DHW1 && tag <= DeviceValueTAG::TAG_DHW10) {
            flags |= CommandFlag::CMD_FLAG_DHW;
        } else if (tag >= DeviceValueTAG::TAG_HS1 && tag <= DeviceValueTAG::TAG_HS16) {
            flags |= CommandFlag::CMD_FLAG_HS;
        } else if (tag >= DeviceValueTAG::TAG_AHS1 && tag <= DeviceValueTAG::TAG_AHS1) {
            flags |= CommandFlag::CMD_FLAG_AHS;
        }

        // add the command to our library
        Command::add(device_type_, device_id_, short_name, f, fullname, flags);
    }
}

// single list of options
void EMSdevice::register_device_value(int8_t               tag,
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
void EMSdevice::register_device_value(int8_t               tag,
                                      void *               value_p,
                                      uint8_t              type,
                                      const char * const * options_single,
                                      const char * const * name,
                                      uint8_t              uom,
                                      const cmd_function_p f,
                                      int16_t              min,
                                      uint32_t             max) {
    // create a multi-list from the options
    add_device_value(tag, value_p, type, nullptr, options_single, 0, name, uom, f, min, max);
};

void EMSdevice::register_device_value(int8_t               tag,
                                      void *               value_p,
                                      uint8_t              type,
                                      int8_t               numeric_operator,
                                      const char * const * name,
                                      uint8_t              uom,
                                      const cmd_function_p f) {
    add_device_value(tag, value_p, type, nullptr, nullptr, numeric_operator, name, uom, f, 0, 0);
}

void EMSdevice::register_device_value(int8_t               tag,
                                      void *               value_p,
                                      uint8_t              type,
                                      int8_t               numeric_operator,
                                      const char * const * name,
                                      uint8_t              uom,
                                      const cmd_function_p f,
                                      int16_t              min,
                                      uint32_t             max) {
    add_device_value(tag, value_p, type, nullptr, nullptr, numeric_operator, name, uom, f, min, max);
}

// no options, no function
void EMSdevice::register_device_value(int8_t tag, void * value_p, uint8_t type, const char * const * name, uint8_t uom, const cmd_function_p f) {
    add_device_value(tag, value_p, type, nullptr, nullptr, 0, name, uom, f, 0, 0);
};

// no options, with min/max
void EMSdevice::register_device_value(int8_t               tag,
                                      void *               value_p,
                                      uint8_t              type,
                                      const char * const * name,
                                      uint8_t              uom,
                                      const cmd_function_p f,
                                      int16_t              min,
                                      uint32_t             max) {
    add_device_value(tag, value_p, type, nullptr, nullptr, 0, name, uom, f, min, max);
};

// function with min and max values
// adds a new command to the command list
// in this function we separate out the short and long names and take any translations
void EMSdevice::register_device_value(int8_t                tag,
                                      void *                value_p,
                                      uint8_t               type,
                                      const char * const ** options,
                                      const char * const *  name,
                                      uint8_t               uom,
                                      const cmd_function_p  f,
                                      int16_t               min,
                                      uint32_t              max) {
    add_device_value(tag, value_p, type, options, nullptr, 0, name, uom, f, min, max);
}

// function with no min and max values (set to 0)
void EMSdevice::register_device_value(int8_t                tag,
                                      void *                value_p,
                                      uint8_t               type,
                                      const char * const ** options,
                                      const char * const *  name,
                                      uint8_t               uom,
                                      const cmd_function_p  f) {
    add_device_value(tag, value_p, type, options, nullptr, 0, name, uom, f, 0, 0);
}

// no associated command function, or min/max values
void EMSdevice::register_device_value(int8_t tag, void * value_p, uint8_t type, const char * const ** options, const char * const * name, uint8_t uom) {
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
    for (const auto & dv : devicevalues_) {
        // check command name and tag, id -1 is default hc and only checks name
        if (dv.has_cmd && std::string(dv.short_name) == cmd && (dv.tag < DeviceValueTAG::TAG_HC1 || dv.tag == id || id == -1)) {
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

// set min and max
void EMSdevice::set_minmax(const void * value_p, int16_t min, uint32_t max) {
    for (auto & dv : devicevalues_) {
        if (dv.value_p == value_p) {
            dv.min = min;
            dv.max = max;
            dv.set_custom_minmax(); // custom priority
            return;
        }
    }
}

// publish a single value on change
void EMSdevice::publish_value(void * value_p) const {
    // if (!Mqtt::publish_single() || value_p == nullptr) {
    if (value_p == nullptr) {
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
            case DeviceValueType::UINT16:
                Helpers::render_value(payload, *(uint16_t *)(value_p), num_op, fahrenheit);
                break;
            case DeviceValueType::UINT8:
                Helpers::render_value(payload, *(uint8_t *)(value_p), num_op, fahrenheit);
                break;
            case DeviceValueType::INT16:
                Helpers::render_value(payload, *(int16_t *)(value_p), num_op, fahrenheit);
                break;
            case DeviceValueType::INT8:
                Helpers::render_value(payload, *(int8_t *)(value_p), num_op, fahrenheit);
                break;
            case DeviceValueType::UINT24:
            case DeviceValueType::TIME:
            case DeviceValueType::UINT32:
                Helpers::render_value(payload, *(uint32_t *)(value_p), num_op);
                break;
            case DeviceValueType::BOOL:
                Helpers::render_boolean(payload, (bool)*(uint8_t *)(value_p));
                break;
            case DeviceValueType::STRING:
                if (Helpers::hasValue((char *)(value_p))) {
                    strlcpy(payload, (char *)(value_p), sizeof(payload));
                }
                break;
            default:
                break;
            }

            if (Mqtt::publish_single() && payload[0] != '\0') {
                Mqtt::queue_publish(topic, payload);
            }
            // check scheduler for on change
            char cmd[COMMAND_MAX_LENGTH];
            if (dv.tag >= DeviceValueTAG::TAG_HC1) {
                snprintf(cmd, sizeof(cmd), "%s/%s/%s", device_type_2_device_name(device_type_), tag_to_mqtt(dv.tag), dv.short_name);
            } else {
                snprintf(cmd, sizeof(cmd), "%s/%s", device_type_2_device_name(device_type_), (dv.short_name));
            }
            EMSESP::webSchedulerService.onChange(cmd);
        }
    }
}

// looks up the UOM for a given key from the device value table
std::string EMSdevice::get_value_uom(const std::string & shortname) const {
    for (const auto & dv : devicevalues_) {
        if ((!dv.has_state(DeviceValueState::DV_WEB_EXCLUDE)) && (dv.short_name == shortname)) {
            // ignore TIME since "minutes" is already added to the string value
            if ((dv.uom == DeviceValueUOM::NONE) || (dv.uom == DeviceValueUOM::MINUTES)) {
                break;
            }
            return EMSdevice::uom_to_string(dv.uom);
        }
    }

    return std::string{}; // not found
}

bool EMSdevice::export_values(uint8_t device_type, JsonObject output, const int8_t id, const uint8_t output_target) {
    bool has_value = false;

    if (id > 0 || output_target == EMSdevice::OUTPUT_TARGET::API_VERBOSE) {
        for (const auto & emsdevice : EMSESP::emsdevices) {
            if (emsdevice && (emsdevice->device_type() == device_type)) {
                has_value |= emsdevice->generate_values(output, id, (id < 1), output_target); // use nested for id -1 and 0
            }
        }
        return has_value;
    }

    // for nested output add for each tag
    for (int8_t tag = DeviceValueTAG::TAG_DEVICE_DATA; tag <= DeviceValueTAG::TAG_HS16; tag++) {
        JsonObject output_hc    = output;
        bool       nest_created = false;
        for (const auto & emsdevice : EMSESP::emsdevices) {
            if (emsdevice && (emsdevice->device_type() == device_type)) {
                if (!nest_created && emsdevice->has_tags(tag)) {
                    output_hc    = output[EMSdevice::tag_to_mqtt(tag)].to<JsonObject>();
                    nest_created = true;
                }
                has_value |= emsdevice->generate_values(output_hc, tag, true, output_target); // use nested for id -1 and 0
            }
        }
    }
    return has_value;
}

// prepare array of device values used for the WebUI
// this is used for the Dashboard and also the Devices page
// this is loosely based of the function generate_values used for the MQTT and Console
// except additional data is stored in the JSON document needed for the Web UI like the UOM and command
// v=value, u=uom, n=name, c=cmd, h=help string, s=step, m=min, x=max
// see types.ts::DeviceValue for the structure
void EMSdevice::generate_values_web(JsonObject output, const bool is_dashboard) {
    // output["label"] = to_string_short();
    // output["label"] = name_;

    JsonArray nodes = output["nodes"].to<JsonArray>();
    uint8_t   count = 0;

    for (auto & dv : devicevalues_) {
        auto fullname = dv.get_fullname();

        // check conditions:
        //  1. fullname cannot be empty
        //  2. it must have a valid value, unless its a command like 'reset'
        //  3. if is_dashboard then only show favs
        bool matching_states = (is_dashboard) ? dv.has_state(DeviceValueState::DV_FAVORITE) : !dv.has_state(DeviceValueState::DV_WEB_EXCLUDE);

        if (matching_states && !fullname.empty() && (dv.hasValue() || (dv.type == DeviceValueType::CMD))) {
            JsonObject root_obj = nodes.add<JsonObject>(); // create the object, we know there is a value

            JsonObject obj;
            if (is_dashboard) {
                root_obj["id"] = (unique_id() * 100) + count++; // make unique
                obj            = root_obj["dv"].to<JsonObject>();
            } else {
                obj = root_obj;
            }

            uint8_t fahrenheit = 0;

            // handle Booleans (true, false), output as strings according to the user settings
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

                if ((dv.type == DeviceValueType::INT8) && Helpers::hasValue(*(int8_t *)(dv.value_p))) {
                    obj["v"] = Helpers::transformNumFloat(*(int8_t *)(dv.value_p), dv.numeric_operator, fahrenheit);
                } else if ((dv.type == DeviceValueType::UINT8) && Helpers::hasValue(*(uint8_t *)(dv.value_p))) {
                    obj["v"] = Helpers::transformNumFloat(*(uint8_t *)(dv.value_p), dv.numeric_operator, fahrenheit);
                } else if ((dv.type == DeviceValueType::INT16) && Helpers::hasValue(*(int16_t *)(dv.value_p))) {
                    obj["v"] = Helpers::transformNumFloat(*(int16_t *)(dv.value_p), dv.numeric_operator, fahrenheit);
                } else if ((dv.type == DeviceValueType::UINT16) && Helpers::hasValue(*(uint16_t *)(dv.value_p))) {
                    obj["v"] = Helpers::transformNumFloat(*(uint16_t *)(dv.value_p), dv.numeric_operator, fahrenheit);
                } else if ((dv.type == DeviceValueType::UINT24) && Helpers::hasValue(*(uint32_t *)(dv.value_p))) {
                    obj["v"] = dv.numeric_operator > 0 ? *(uint32_t *)(dv.value_p) / dv.numeric_operator : *(uint32_t *)(dv.value_p);
                } else if ((dv.type == DeviceValueType::TIME) && Helpers::hasValue(*(uint32_t *)(dv.value_p))) {
                    obj["v"] = dv.numeric_operator > 0 ? *(uint32_t *)(dv.value_p) / dv.numeric_operator : *(uint32_t *)(dv.value_p);
                } else if ((dv.type == DeviceValueType::UINT32) && Helpers::hasValue(*(uint32_t *)(dv.value_p))) {
                    obj["v"] = dv.numeric_operator > 0 ? *(uint32_t *)(dv.value_p) / dv.numeric_operator : *(uint32_t *)(dv.value_p);
                } else {
                    obj["v"] = ""; // must have a value for sorting to work
                }
            }

            // add the unit of measure (uom)
            obj["u"] = fahrenheit ? (uint8_t)DeviceValueUOM::FAHRENHEIT : dv.uom;

            auto mask = Helpers::hextoa((uint8_t)(dv.state >> 4), false); // create mask to a 2-char string

            // add name, prefixing the tag if it exists. This is the id used in the WebUI table and must be unique
            obj["id"] = dv.has_tag() ? mask + tag_to_string(dv.tag) + " " + fullname : mask + fullname; // suffix tag

            // TAG https://github.com/emsesp/EMS-ESP32/issues/1338
            // obj["id"] = dv.has_tag() ? mask + fullname + " " + tag_to_string(dv.tag) : mask + fullname; // suffix tag

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
                    JsonArray l = obj["l"].to<JsonArray>();
                    for (uint8_t i = 0; i < dv.options_size; i++) {
                        auto enum_str = Helpers::translated_word(dv.options[i]);
                        if (enum_str) {
                            l.add(enum_str);
                        }
                    }
                } else if (dv.type == DeviceValueType::BOOL) {
                    JsonArray l = obj["l"].to<JsonArray>();
                    char      result[12];
                    l.add(Helpers::render_boolean(result, false, true));
                    l.add(Helpers::render_boolean(result, true, true));
                }
                // add command help template
                else if (dv.type == DeviceValueType::STRING || (dv.type == DeviceValueType::CMD && dv.uom == DeviceValueUOM::NONE)) {
                    if (dv.options_size == 1) {
                        obj["h"] = dv.options_single[0]; // NOT translated
                    }
                }
                // handle INTs
                else {
                    // add min and max values, if available
                    int16_t  dv_set_min;
                    uint32_t dv_set_max;
                    if (dv.get_min_max(dv_set_min, dv_set_max)) {
                        obj["m"] = dv_set_min;
                        obj["x"] = dv_set_max;
                        // add steps to numeric values as rendered string to avoid rounding floats in js
                        char s[10];
                        obj["s"] = Helpers::render_value(s, (uint32_t)1, dv.numeric_operator);
                    }
                }
            }
        }
    }
}

// as generate_values_web() but with extra data for WebCustomizationService::device_entities() (rest/deviceEntities?id=n)
// also show commands and entities that have an empty fullname
// see types.ts::DeviceEntity for the structure
void EMSdevice::generate_values_web_customization(JsonArray output) {
    for (auto & dv : devicevalues_) {
        JsonObject obj        = output.add<JsonObject>();
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
                if (dv.type == DeviceValueType::INT8) {
                    obj["v"] = Helpers::transformNumFloat(*(int8_t *)(dv.value_p), dv.numeric_operator, fahrenheit);
                } else if (dv.type == DeviceValueType::UINT8) {
                    obj["v"] = Helpers::transformNumFloat(*(uint8_t *)(dv.value_p), dv.numeric_operator, fahrenheit);
                } else if (dv.type == DeviceValueType::INT16) {
                    obj["v"] = Helpers::transformNumFloat(*(int16_t *)(dv.value_p), dv.numeric_operator, fahrenheit);
                } else if (dv.type == DeviceValueType::UINT16) {
                    obj["v"] = Helpers::transformNumFloat(*(uint16_t *)(dv.value_p), dv.numeric_operator, fahrenheit);
                } else if (dv.type == DeviceValueType::UINT24 || dv.type == DeviceValueType::UINT32) {
                    obj["v"] = dv.numeric_operator > 0 ? *(uint32_t *)(dv.value_p) / dv.numeric_operator : *(uint32_t *)(dv.value_p);
                } else if (dv.type == DeviceValueType::TIME) {
                    obj["v"] = dv.numeric_operator > 0 ? *(uint32_t *)(dv.value_p) / dv.numeric_operator : *(uint32_t *)(dv.value_p);
                }
            }
        }

        // create the id
        // it holds the shortname and must always have a unique value for the WebUI table to work
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
                // obj["n"] = dv.has_tag() ? std::string(tag_to_string(dv.tag)) + " " + fullname : fullname; // prefix tag
                obj["n"] = fullname;
                // TAG https://github.com/emsesp/EMS-ESP32/issues/1338
                // obj["n"] = (dv.has_tag()) ? fullname + " " + tag_to_string(dv.tag) : fullname; // suffix tag
            }

            // add the custom name, is optional
            std::string custom_fullname = dv.get_custom_fullname();
            if (!custom_fullname.empty()) {
                obj["cn"] = custom_fullname;
            }
        } else {
            obj["n"] = "!" + std::string(fullname); // prefix commands with a !
        }
        if (dv.has_tag()) {
            obj["t"] = tag_to_string(dv.tag);
        }

        // the mask state. We're only interested in the high nibble which contains the flags, so shift right
        // 0x80 = 128 = DV_FAVORITE
        // 0x40 = 64  = DV_READONLY
        // 0x20 = 32  = DV_API_MQTT_EXCLUDE
        // 0x10 = 16  = DV_WEB_EXCLUDE
        obj["m"] = dv.state >> 4;

        obj["w"] = dv.has_cmd; // if writable

        if (dv.has_cmd && (obj["v"].is<float>() || obj["v"].is<int>())) {
            // set the min and max values if there are any and if entity has a value
            int16_t  dv_set_min;
            uint32_t dv_set_max;
            if (dv.get_min_max(dv_set_min, dv_set_max)) {
                obj["mi"] = dv_set_min;
                obj["ma"] = dv_set_max;
            }
        }
    }

    // apply and blacklisted/removed entities
    // this is when the mask has it's high bit (0x80) set
    // https://github.com/emsesp/EMS-ESP32/issues/891
    EMSESP::webCustomizationService.read([&](WebCustomization & settings) {
        for (EntityCustomization entityCustomization : settings.entityCustomizations) {
            if (entityCustomization.device_id == device_id()) {
                // entity_ids is a list of all entities with the mask prefixed in the string
                for (const std::string & entity_id : entityCustomization.entity_ids) {
                    uint8_t mask = Helpers::hextoint(entity_id.substr(0, 2).c_str());
                    if (mask & 0x80) {
                        JsonObject obj = output.add<JsonObject>();
                        obj["id"]      = DeviceValue::get_name(entity_id); // set the name, it could be custom following a '|'
                        obj["m"]       = mask;                             // update the mask
                        obj["w"]       = false;                            // not writeable as it won't be shown
                    }
                }
                break;
            }
        }
    });
}

void EMSdevice::set_climate_minmax(int8_t tag, int16_t min, uint32_t max) {
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

void EMSdevice::setValueEnum(const void * value_p, const char * const ** options) {
    for (auto & dv : devicevalues_) {
        if (dv.value_p == value_p) {
            if (dv.options != options && Mqtt::ha_enabled()) {
                dv.remove_state(DeviceValueState::DV_HA_CONFIG_CREATED);
            }
            dv.options      = options;
            dv.options_size = Helpers::count_items(options);
            break;
        }
    }
}

// set mask per device entity based on the id which is prefixed with the 2 char hex mask value
// returns true if the entity has a mask set (not 0 the default)
void EMSdevice::setCustomizationEntity(const std::string & entity_id) {
    for (auto & dv : devicevalues_) {
        char entity_name[70];
        if (dv.tag < DeviceValueTAG::TAG_HC1) {
            strncpy(entity_name, dv.short_name, sizeof(entity_name) - 1);
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

// populate a string vector with entities that have masks set or have a custom entity name
void EMSdevice::getCustomizationEntities(std::vector<std::string> & entity_ids) {
    for (const auto & dv : devicevalues_) {
        char name[100];
        name[0] = '\0';
        if (dv.tag >= DeviceValueTAG::TAG_HC1) {
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

// dumps all entity values in native English
// the code is intended to run only once standalone, outside the ESP32 so not optimized for memory efficiency
// pipe symbols (|) are escaped so they can be converted to Markdown in the Wiki
// format is: device name,device type,product id,shortname,fullname,type [options...] \\| (min/max),uom,writeable,discovery entityid v3.4, discovery entityid
#if defined(EMSESP_STANDALONE)
void EMSdevice::dump_devicevalue_info() {
    for (auto & dv : devicevalues_) {
        if (dv.fullname != nullptr) {
            Serial.print('\"');
            Serial.print(default_name());
            Serial.print('\"');
            Serial.print(',');
            Serial.print(device_type_name());
            Serial.print(',');

            Serial.print(product_id_);
            Serial.print(',');

            // <tag>/<shortname> unless generating for modbus registers - used in scripts/generate_csv_and_headers.sh
#ifndef EMSESP_MODBUS
            if (dv.tag > DeviceValueTAG::TAG_DEVICE_DATA) {
                Serial.print(tag_to_string(dv.tag));
                Serial.print('.');
            }
#endif
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

            case DeviceValueType::UINT16:
                Serial.print("uint16");
                break;

            case DeviceValueType::UINT8:
                Serial.print("uint8");
                break;

            case DeviceValueType::INT16:
                Serial.print("int16");
                break;

            case DeviceValueType::INT8:
                Serial.print("int8");
                break;

            case DeviceValueType::UINT24:
                Serial.print("uint24");
                break;

            case DeviceValueType::UINT32:
                Serial.print("uint32");
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
            uint32_t dv_set_max;
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
                    case DeviceValueType::INT8:
                    case DeviceValueType::UINT8:
                    case DeviceValueType::INT16:
                    case DeviceValueType::UINT16:
                    case DeviceValueType::UINT24:
                    case DeviceValueType::UINT32:
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

            Serial.print(",");

            // modbus specific infos
            Serial.print(device_type()); // modbus unit identifier
            Serial.print(',');

            Serial.print(dv.tag); // modbus block
            Serial.print(',');

            // numeric operator -> modbus scale factor
            if (dv.numeric_operator == 0)
                Serial.print("1");
            else if (dv.numeric_operator > 0)
                Serial.printf("1/%d", dv.numeric_operator);
            else
                Serial.print(-dv.numeric_operator);
            Serial.print(",");

            Serial.printf("%d", EMSESP::modbus_->getRegisterOffset(dv)); // modbus offset
            Serial.print(",");

            Serial.printf("%d", EMSESP::modbus_->getRegisterCount(dv)); // modbus count

            // /modbus specific infos

            Serial.println();
        }
    }
}
#endif

// dumps all telegram details to a new vector
#if defined(EMSESP_STANDALONE)
void EMSdevice::dump_telegram_info(std::vector<TelegramFunctionDump> & telegram_functions_dump) {
    for (auto & tf : telegram_functions_) {
        telegram_functions_dump.emplace_back(tf.telegram_type_id_, tf.telegram_type_name_, tf.fetch_);
    }
}
#endif

// builds json for a specific EMS device value / entity
// cmd is the endpoint or name of the device entity
// returns false if failed, otherwise true
bool EMSdevice::get_value_info(JsonObject output, const char * cmd, const int8_t tag) {
    if (!strcmp(cmd, F_(info))) {
        return export_values(device_type(), output, tag, EMSdevice::OUTPUT_TARGET::API_VERBOSE);
    }
    if (!strcmp(cmd, F_(values))) {
        return export_values(device_type(), output, tag, EMSdevice::OUTPUT_TARGET::API_SHORTNAMES);
    }
    if (!strcmp(cmd, F_(entities))) {
        char name[30];
        for (auto & dv : devicevalues_) {
            if (tag < 0 || tag == dv.tag) {
                strlcpy(name, tag < 0 ? tag_to_mqtt(dv.tag) : "", sizeof(name));
                strlcat(name, tag < 0 && dv.tag > 0 ? "." : "", sizeof(name));
                strlcat(name, dv.short_name, sizeof(name));
                get_value_json(output[name].to<JsonObject>(), dv);
            }
        }
        return true;
    }

    // search device value with this tag
    // make a copy of cmd and split attribute (leave cmd untouched for other devices)
    char cmd_s[COMMAND_MAX_LENGTH];
    strlcpy(cmd_s, cmd, sizeof(cmd_s));
    const char * attribute_s = Command::get_attribute(cmd_s);
    for (auto & dv : devicevalues_) {
        if (cmd_s == Helpers::toLower(dv.short_name) && (tag <= 0 || tag == dv.tag)) {
            get_value_json(output, dv);
            // if we're filtering on an attribute, go find it
            // if we can't find it, maybe it exists but doesn't not have a value assigned yet
            return Command::set_attribute(output, cmd_s, attribute_s);
        }
    }
    return false; // not found, but don't return a message error yet
}

// build the json for a specific entity
void EMSdevice::get_value_json(JsonObject json, DeviceValue & dv) {
    uint8_t fahrenheit = !EMSESP::system_.fahrenheit() ? 0 : (dv.uom == DeviceValueUOM::DEGREES) ? 2 : (dv.uom == DeviceValueUOM::DEGREES_R) ? 1 : 0;

    const char * type  = "type";
    const char * value = "value";

    json["name"] = dv.short_name;

    auto fullname = dv.get_fullname();
    if (!fullname.empty()) {
        json["fullname"] = dv.has_tag() ? fullname + " " + tag_to_string(dv.tag) : fullname; // suffix tag

        // TAG https://github.com/emsesp/EMS-ESP32/issues/1338
        json["fullname"] = dv.has_tag() ? std::string(tag_to_string(dv.tag)) + " " + fullname.c_str() : fullname; // prefix tag
    }

    if (dv.tag != DeviceValueTAG::TAG_NONE) {
        json["circuit"] = tag_to_mqtt(dv.tag);
    }

    char val[20];
    switch (dv.type) {
    case DeviceValueType::ENUM: {
        if (*(uint8_t *)(dv.value_p) < dv.options_size) {
            json["index"] = (uint8_t)(*(uint8_t *)(dv.value_p));
            json["enum"]  = Helpers::translated_word(dv.options[*(uint8_t *)(dv.value_p)]); // text
            if (EMSESP::system_.enum_format() == ENUM_FORMAT_INDEX) {
                json[value] = (uint8_t)(*(uint8_t *)(dv.value_p));
            } else {
                json[value] = Helpers::translated_word(dv.options[*(uint8_t *)(dv.value_p)]); // text
            }
        }
        json[type]      = F_(enum);
        JsonArray enum_ = json[F_(enum)].to<JsonArray>();
        for (uint8_t i = 0; i < dv.options_size; i++) {
            enum_.add(Helpers::translated_word(dv.options[i]));
        }
        break;
    }

    case DeviceValueType::UINT16:
        if (Helpers::hasValue(*(uint16_t *)(dv.value_p))) {
            json[value] = serialized(Helpers::render_value(val, *(uint16_t *)(dv.value_p), dv.numeric_operator, fahrenheit));
        }
        json[type] = F_(number);
        break;

    case DeviceValueType::UINT8:
        if (Helpers::hasValue(*(uint8_t *)(dv.value_p))) {
            json[value] = serialized(Helpers::render_value(val, *(uint8_t *)(dv.value_p), dv.numeric_operator, fahrenheit));
        }
        json[type] = F_(number);
        break;

    case DeviceValueType::INT16:
        if (Helpers::hasValue(*(int16_t *)(dv.value_p))) {
            json[value] = serialized(Helpers::render_value(val, *(int16_t *)(dv.value_p), dv.numeric_operator, fahrenheit));
        }
        json[type] = F_(number);
        break;

    case DeviceValueType::INT8:
        if (Helpers::hasValue(*(int8_t *)(dv.value_p))) {
            json[value] = serialized(Helpers::render_value(val, *(int8_t *)(dv.value_p), dv.numeric_operator, fahrenheit));
        }
        json[type] = F_(number);
        break;

    case DeviceValueType::UINT24:
    case DeviceValueType::UINT32:
        if (Helpers::hasValue(*(uint32_t *)(dv.value_p))) {
            json[value] = serialized(Helpers::render_value(val, *(uint32_t *)(dv.value_p), dv.numeric_operator));
        }
        json[type] = F_(number);
        break;

    case DeviceValueType::BOOL:
        if (Helpers::hasValue(*(uint8_t *)(dv.value_p), EMS_VALUE_BOOL)) {
            auto value_b  = (bool)*(uint8_t *)(dv.value_p);
            json["bool"]  = value_b;
            json["index"] = value_b ? 1 : 0;
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
        if (dv.options_size > 1) {
            json[type]      = F_(enum);
            JsonArray enum_ = json[F_(enum)].to<JsonArray>();
            for (uint8_t i = 0; i < dv.options_size; i++) {
                enum_.add(Helpers::translated_word(dv.options[i]));
            }
        } else if (dv.uom != DeviceValueUOM::NONE) {
            json[type] = F_(number);
        } else {
            json[type] = F_(command);
        }
        break;

    default:
        json[type] = Helpers::translated_word(FL_(unknown));
        break;
    }

    // set the min and max only for commands
    if (dv.has_cmd) {
        int16_t  dv_set_min;
        uint32_t dv_set_max;
        if (dv.get_min_max(dv_set_min, dv_set_max)) {
            json["min"] = dv_set_min;
            json["max"] = dv_set_max;
        }
    }

    // add uom, state class and device class
    Mqtt::add_ha_uom(json, dv.type, dv.uom, dv.short_name, false); // no icon

    json["readable"]  = !dv.has_state(DeviceValueState::DV_API_MQTT_EXCLUDE);
    json["writeable"] = dv.has_cmd && !dv.has_state(DeviceValueState::DV_READONLY);
    json["visible"]   = !dv.has_state(DeviceValueState::DV_WEB_EXCLUDE);
}

// mqtt publish all single values from one device (used for time schedule)
void EMSdevice::publish_all_values() {
    for (const auto & dv : devicevalues_) {
        publish_value(dv.value_p);
    }
}

// For each value in the device create the json object pair and add it to given json
// return false if empty
// this is used to create the MQTT payloads, Console messages and Web API call responses
bool EMSdevice::generate_values(JsonObject output, const int8_t tag_filter, const bool nested, const uint8_t output_target) {
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
                // char short_name[20];
                // if (output_target == OUTPUT_TARGET::CONSOLE) {
                //     snprintf(short_name, sizeof(short_name), "(%s)", dv.short_name);
                // } else {
                //     strcpy(short_name, "");
                // }

                // add tag
                if (have_tag) {
                    snprintf(name, sizeof(name), "%s %s (%s)", tag_to_string(dv.tag), fullname.c_str(), dv.short_name); // prefix tag
                    // TAG https://github.com/emsesp/EMS-ESP32/issues/1338
                    // snprintf(name, sizeof(name), "%s %s (%s)", fullname.c_str(), tag_to_string(dv.tag), dv.short_name); // suffix tag
                } else {
                    snprintf(name, sizeof(name), "%s (%s)", fullname.c_str(), dv.short_name);
                }
            } else {
                strlcpy(name, (dv.short_name), sizeof(name)); // use short name

                // if we have a tag, and its different to the last one create a nested object. only for hc, dhw and hs
                if (dv.tag != old_tag) {
                    old_tag = dv.tag;
                    if (nested && have_tag && dv.tag >= DeviceValueTAG::TAG_HC1) {
                        json = output[tag_to_mqtt(dv.tag)].to<JsonObject>();
                    }
                }
            }

            // do not overwrite
            if (json[name].is<JsonVariantConst>()) {
#if defined(EMSESP_DEBUG)
                EMSESP::logger().debug("warning: double json key: %s", name);
#endif
                continue;
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
                char    val[20]    = {'\0'};
                if (dv.type == DeviceValueType::INT8) {
                    json[name] = serialized(Helpers::render_value(val, *(int8_t *)(dv.value_p), dv.numeric_operator, fahrenheit));
                } else if (dv.type == DeviceValueType::UINT8) {
                    json[name] = serialized(Helpers::render_value(val, *(uint8_t *)(dv.value_p), dv.numeric_operator, fahrenheit));
                } else if (dv.type == DeviceValueType::INT16) {
                    json[name] = serialized(Helpers::render_value(val, *(int16_t *)(dv.value_p), dv.numeric_operator, fahrenheit));
                } else if (dv.type == DeviceValueType::UINT16) {
                    json[name] = serialized(Helpers::render_value(val, *(uint16_t *)(dv.value_p), dv.numeric_operator, fahrenheit));
                } else if (dv.type == DeviceValueType::UINT24 || dv.type == DeviceValueType::UINT32) {
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
                                 "%lu %s %lu %s %lu %s",
                                 (time_value / 1440),
                                 Helpers::translated_word(FL_(days)),
                                 ((time_value % 1440) / 60),
                                 Helpers::translated_word(FL_(hours)),
                                 (time_value % 60),
                                 Helpers::translated_word(FL_(minutes)));
                        json[name] = time_s;
                    } else {
                        json[name] = serialized(Helpers::render_value(val, time_value, 0));
                    }
                }

                // we don't want Commands in Console ('show values')
                else if (dv.type == DeviceValueType::CMD && output_target != EMSdevice::OUTPUT_TARGET::CONSOLE) {
                    if (dv.uom == DeviceValueUOM::NONE) {
                        json[name] = "";
                    } else {
                        json[name] = NAN;
                    }
                }

                // check for value outside min/max range and adapt the limits to avoid HA complains
                // Should this also check for api output?
                if ((output_target == OUTPUT_TARGET::MQTT) && (dv.min != 0 || dv.max != 0)) {
                    int v = Helpers::atoint(val);
                    if (fahrenheit) {
                        v = (v - (32 * (fahrenheit - 1))) / 1.8; // reset to °C
                    }
                    if (v < dv.min) {
                        dv.min = v;
                        dv.remove_state(DeviceValueState::DV_HA_CONFIG_CREATED);
                    } else if (v > 0 && (uint32_t)v > dv.max) {
                        dv.max = v;
                        dv.remove_state(DeviceValueState::DV_HA_CONFIG_CREATED);
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
    bool     create_device_config = !ha_config_done(); // do we need to create the main Discovery device config with this entity?
    uint16_t count                = 0;

    // check the state of each of the device values
    // create the discovery topic if if hasn't already been created, not a command (like reset) and is active and visible
    for (auto & dv : devicevalues_) {
        // create climate when we reach the haclimate entity
        if (!strcmp(dv.short_name, FL_(haclimate)[0]) && !dv.has_state(DeviceValueState::DV_API_MQTT_EXCLUDE) && dv.has_state(DeviceValueState::DV_ACTIVE)) {
            if (*(int8_t *)(dv.value_p) == 1 && (!dv.has_state(DeviceValueState::DV_HA_CONFIG_CREATED) || dv.has_state(DeviceValueState::DV_HA_CLIMATE_NO_RT))) {
                if (Mqtt::publish_ha_climate_config(dv.tag, true, false, dv.min, dv.max)) { // roomTemp
                    dv.remove_state(DeviceValueState::DV_HA_CLIMATE_NO_RT);
                    dv.add_state(DeviceValueState::DV_HA_CONFIG_CREATED);
                    count++;
                }
            } else if (*(int8_t *)(dv.value_p) == 0
                       && (!dv.has_state(DeviceValueState::DV_HA_CONFIG_CREATED) || !dv.has_state(DeviceValueState::DV_HA_CLIMATE_NO_RT))) {
                if (Mqtt::publish_ha_climate_config(dv.tag, false, false, dv.min, dv.max)) { // no roomTemp
                    dv.add_state(DeviceValueState::DV_HA_CLIMATE_NO_RT);
                    dv.add_state(DeviceValueState::DV_HA_CONFIG_CREATED);
                    count++;
                }
            }
        }

        if (!dv.has_state(DeviceValueState::DV_HA_CONFIG_CREATED) && dv.has_state(DeviceValueState::DV_ACTIVE)
            && !dv.has_state(DeviceValueState::DV_API_MQTT_EXCLUDE)) {
            // create_device_config is only done once for the EMS device. It can added to any entity, so we take the first
            if (Mqtt::publish_ha_sensor_config(dv, name().c_str(), brand_to_char(), false, create_device_config)) {
                dv.add_state(DeviceValueState::DV_HA_CONFIG_CREATED);
                create_device_config = false; // only create the main config once
                count++;
            }
#ifndef EMSESP_STANDALONE
            // always create minimum one config
            if (count && (heap_caps_get_free_size(MALLOC_CAP_8BIT) < 65 * 1024)) { // checks free Heap+PSRAM
                break;
            }
#endif
        }
    }
#if defined(EMSESP_DEBUG) || defined(EMSESP_STANDALONE)
    if (count) {
        EMSESP::logger().debug("Created %d HA-%s-configs", count, device_type_name());
    }
#endif
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
            // for telegram destination only read telegram
            if (telegram->dest == device_id_ && telegram->message_length > 0) {
                tf.process_function_(telegram);
                return true;
            }
            // if the data block is empty and we have not received data before, assume that this telegram
            // is not recognized by the bus master. So remove it from the automatic fetch list
            if (telegram->message_length == 0 && telegram->offset == 0 && !tf.received_) {
#if defined(EMSESP_DEBUG)
                EMSESP::logger().debug("This telegram (%s) is not recognized by the EMS bus", tf.telegram_type_name_);
#endif
                // removing fetch after start causes issue: https://github.com/emsesp/EMS-ESP32/issues/1420
                // continue retry the first 5 minutes, then disable (added 15.3.2024)
                if (uuid::get_uptime_sec() > 600) {
                    tf.fetch_ = false;
                }
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

// returns either default or custom name
std::string EMSdevice::name() {
    if (custom_name_.empty()) {
        // return default name prefixed with a model if exists
        if (model().empty()) {
            return default_name();
        }
        return model() + "/" + std::string(default_name());
    }

    return custom_name();
}

// copy a raw value (i.e. without applying the numeric_operator) to the output buffer.
// returns true on success.
int EMSdevice::get_modbus_value(uint8_t tag, const std::string & shortname, std::vector<uint16_t> & result) {
    // find device value by shortname
    // TODO replace linear search which is inefficient
    const auto & it = std::find_if(devicevalues_.begin(), devicevalues_.end(), [&](const DeviceValue & x) { return x.tag == tag && x.short_name == shortname; });
    if (it == devicevalues_.end() && (it->short_name != shortname || it->tag != tag)) {
        return -1;
    }

    auto & dv = *it;

    // check if it exists, there is a value for the entity. Set the flag to ACTIVE
    // not that this will override any previously removed states
    (dv.hasValue()) ? dv.add_state(DeviceValueState::DV_ACTIVE) : dv.remove_state(DeviceValueState::DV_ACTIVE);

    if (!dv.has_state(DeviceValueState::DV_ACTIVE))
        return -2;

    // handle Booleans
    if (dv.type == DeviceValueType::BOOL) {
        if (result.size() != 1)
            return -3;
        auto value_bool = *(uint8_t *)(dv.value_p);
        if (!Helpers::hasValue(value_bool, EMS_VALUE_BOOL))
            return -4;
        result[0] = value_bool ? 1 : 0;
    }

    // handle TEXT strings
    else if (dv.type == DeviceValueType::STRING) {
        auto   value_s           = (char *)dv.value_p;
        size_t length_s          = strlen(value_s) + 1; // length including terminating zero in bytes
        size_t register_length_s = (length_s + 1) / 2;  // length including terminating zero in uint16_t-registers

        if ((long)result.size() < (long)register_length_s) {
            return -5;
        }

        for (size_t i = 0; i < register_length_s; i++) {
            auto hi   = (uint8_t)value_s[2 * i];
            auto lo   = (uint8_t)(2 * i + 1 < length_s ? value_s[2 * i + 1] : 0);
            result[i] = ((uint16_t)hi << 8) | lo;
        }
    }

    // handle ENUMs
    else if (dv.type == DeviceValueType::ENUM) {
        if (result.size() != 1)
            return -6;
        auto value_enum = *(uint8_t *)(dv.value_p);
        if (value_enum >= dv.options_size)
            return -7;
        result[0] = (uint16_t)value_enum;
    }

    // handle Numbers
    else if (dv.type == DeviceValueType::INT8) {
        if (result.size() != 1)
            return -8;
        result[0] = (uint16_t)(int16_t)(*(int8_t *)(dv.value_p));
    } else if (dv.type == DeviceValueType::UINT8) {
        if (result.size() != 1)
            return -9;
        result[0] = (uint16_t)(*(uint8_t *)(dv.value_p));
    } else if (dv.type == DeviceValueType::INT16) {
        if (result.size() != 1)
            return -10;
        result[0] = (uint16_t)(*(int16_t *)(dv.value_p));
    } else if (dv.type == DeviceValueType::UINT16) {
        if (result.size() != 1)
            return -11;
        result[0] = *(uint16_t *)(dv.value_p);
    } else if (dv.type == DeviceValueType::UINT24 || dv.type == DeviceValueType::UINT32 || dv.type == DeviceValueType::TIME) {
        if (result.size() != 2)
            return -12;
        auto value_uint32 = *(uint32_t *)(dv.value_p);
        result[0]         = (uint16_t)(value_uint32 >> 16);
        result[1]         = (uint16_t)(value_uint32 & 0xffff);
    }

    else {
        return -13;
    }

    return 0;
}

int EMSdevice::modbus_value_to_json(uint8_t tag, const std::string & shortname, const std::vector<uint8_t> & modbus_data, JsonObject jsonValue) {
    // LOG_DEBUG("modbus_value_to_json(%d,%s,[%d bytes])\n", tag, shortname.c_str(), modbus_data.size());

    // find device value by shortname
    const auto & it = std::find_if(devicevalues_.begin(), devicevalues_.end(), [&](const DeviceValue & x) { return x.tag == tag && x.short_name == shortname; });
    if (it == devicevalues_.end() && (it->short_name != shortname || it->tag != tag)) {
        return -1;
    }

    auto & dv = *it;

    // handle Booleans
    if (dv.type == DeviceValueType::BOOL) {
        // bools are 1 16 bit register
        if (modbus_data.size() != 2) {
            return -2;
        }
        jsonValue["value"] = modbus_data[0] || modbus_data[1];
    }

    // handle TEXT strings
    else if (dv.type == DeviceValueType::STRING) {
        // text is optionally nul terminated
        // check if the data contains a null char
        auto nul_or_end    = std::find(modbus_data.begin(), modbus_data.end(), 0);
        jsonValue["value"] = std::string(modbus_data.begin(), nul_or_end);
    }

    // handle ENUMs
    else if (dv.type == DeviceValueType::ENUM) {
        // these data types are 1 16 bit register
        if (modbus_data.size() != 2) {
            return -3;
        }

        jsonValue["value"] = (uint16_t)modbus_data[0] << 8 | (uint16_t)modbus_data[1];
    }

    // handle Numbers
    else if (dv.type == DeviceValueType::INT8 || dv.type == DeviceValueType::UINT8 || dv.type == DeviceValueType::INT16 || dv.type == DeviceValueType::UINT16) {
        // these data types are 1 16 bit register
        if (modbus_data.size() != 2) {
            return -4;
        }

        if (dv.type == DeviceValueType::INT8 || dv.type == DeviceValueType::INT16) { // handle signed
            jsonValue["value"] =
                Helpers::numericoperator2scalefactor(dv.numeric_operator) * (float)(int16_t)((uint16_t)modbus_data[0] << 8 | (uint16_t)modbus_data[1]);
        } else {
            jsonValue["value"] = Helpers::numericoperator2scalefactor(dv.numeric_operator) * (float)((uint16_t)modbus_data[0] << 8 | (uint16_t)modbus_data[1]);
        }
    } else if (dv.type == DeviceValueType::UINT24 || dv.type == DeviceValueType::UINT32 || dv.type == DeviceValueType::TIME) {
        // these data types are 2 16 bit register
        if (modbus_data.size() != 4) {
            return -5;
        }

        jsonValue["value"] =
            Helpers::numericoperator2scalefactor(dv.numeric_operator)
            * (float)((uint32_t)modbus_data[0] << 24 | (uint32_t)modbus_data[1] << 16 | (uint32_t)modbus_data[2] << 8 | (uint32_t)modbus_data[3]);
    }

    // handle CMD
    else if (dv.type == DeviceValueType::CMD) {
        if (modbus_data.size() > 4) {
            return -7;
        }

        uint32_t value = 0;
        for (size_t i = 0; i < modbus_data.size(); i++) {
            value += (uint32_t)modbus_data[modbus_data.size() - i - 1] << (i * 8);
        }

        jsonValue["value"] = Helpers::numericoperator2scalefactor(dv.numeric_operator) * (float)value;
    }

    else {
        return -6;
    }

    return 0;
}

} // namespace emsesp
