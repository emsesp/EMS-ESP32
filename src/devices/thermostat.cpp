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

#include "thermostat.h"

namespace emsesp {

REGISTER_FACTORY(Thermostat, EMSdevice::DeviceType::THERMOSTAT);

uuid::log::Logger Thermostat::logger_{F_(thermostat), uuid::log::Facility::CONSOLE};

Thermostat::Thermostat(uint8_t device_type, uint8_t device_id, uint8_t product_id, const std::string & version, const std::string & name, uint8_t flags, uint8_t brand)
    : EMSdevice(device_type, device_id, product_id, version, name, flags, brand) {
    uint8_t actual_master_thermostat = EMSESP::actual_master_thermostat(); // what we're actually using
    uint8_t master_thermostat        = EMSESP_DEFAULT_MASTER_THERMOSTAT;
    EMSESP::webSettingsService.read([&](WebSettings & settings) {
        master_thermostat = settings.master_thermostat; // what the user has defined
    });

    uint8_t model = this->model();

    // if we're on auto mode, register this thermostat if it has a device id of 0x10, 0x17 or 0x18
    // or if its the master thermostat we defined
    // see https://github.com/emsesp/EMS-ESP/issues/362#issuecomment-629628161
    if ((master_thermostat == device_id)
        || ((master_thermostat == EMSESP_DEFAULT_MASTER_THERMOSTAT) && (device_id < 0x19)
            && ((actual_master_thermostat == EMSESP_DEFAULT_MASTER_THERMOSTAT) || (device_id < actual_master_thermostat)))) {
        EMSESP::actual_master_thermostat(device_id);
        actual_master_thermostat = device_id;
        reserve_telgram_functions(25); // reserve some space for the telegram registries, to avoid memory fragmentation

        // common telegram handlers
        register_telegram_type(EMS_TYPE_RCOutdoorTemp, F("RCOutdoorTemp"), false, MAKE_PF_CB(process_RCOutdoorTemp));
        register_telegram_type(EMS_TYPE_RCTime, F("RCTime"), false, MAKE_PF_CB(process_RCTime));
        register_telegram_type(0xA2, F("RCError"), false, MAKE_PF_CB(process_RCError));
        register_telegram_type(0x12, F("RCErrorMessage"), false, MAKE_PF_CB(process_RCErrorMessage));
    }
    // RC10
    if (model == EMSdevice::EMS_DEVICE_FLAG_RC10) {
        monitor_typeids = {0xB1};
        set_typeids     = {0xB0};
        for (uint8_t i = 0; i < monitor_typeids.size(); i++) {
            register_telegram_type(monitor_typeids[i], F("RC10Monitor"), false, MAKE_PF_CB(process_RC10Monitor));
            register_telegram_type(set_typeids[i], F("RC10Set"), false, MAKE_PF_CB(process_RC10Set));
        }

        // RC35
    } else if ((model == EMSdevice::EMS_DEVICE_FLAG_RC35) || (model == EMSdevice::EMS_DEVICE_FLAG_RC30_N)) {
        monitor_typeids = {0x3E, 0x48, 0x52, 0x5C};
        set_typeids     = {0x3D, 0x47, 0x51, 0x5B};
        timer_typeids   = {0x3F, 0x49, 0x53, 0x5D};
        for (uint8_t i = 0; i < monitor_typeids.size(); i++) {
            register_telegram_type(monitor_typeids[i], F("RC35Monitor"), false, MAKE_PF_CB(process_RC35Monitor));
            register_telegram_type(set_typeids[i], F("RC35Set"), false, MAKE_PF_CB(process_RC35Set));
            register_telegram_type(timer_typeids[i], F("RC35Timer"), false, MAKE_PF_CB(process_RC35Timer));
        }
        register_telegram_type(EMS_TYPE_IBASettings, F("IBASettings"), true, MAKE_PF_CB(process_IBASettings));
        register_telegram_type(EMS_TYPE_wwSettings, F("WWSettings"), true, MAKE_PF_CB(process_RC35wwSettings));

        // RC20
    } else if (model == EMSdevice::EMS_DEVICE_FLAG_RC20) {
        monitor_typeids = {0x91};
        set_typeids     = {0xA8};
        if (actual_master_thermostat == device_id) {
            for (uint8_t i = 0; i < monitor_typeids.size(); i++) {
                register_telegram_type(monitor_typeids[i], F("RC20Monitor"), false, MAKE_PF_CB(process_RC20Monitor));
                register_telegram_type(set_typeids[i], F("RC20Set"), false, MAKE_PF_CB(process_RC20Set));
            }
        } else {
            register_telegram_type(0xAF, F("RC20Remote"), false, MAKE_PF_CB(process_RC20Remote));
        }
        // RC20 newer
    } else if (model == EMSdevice::EMS_DEVICE_FLAG_RC20_N) {
        monitor_typeids = {0xAE};
        set_typeids     = {0xAD};
        if (actual_master_thermostat == device_id) {
            for (uint8_t i = 0; i < monitor_typeids.size(); i++) {
                register_telegram_type(monitor_typeids[i], F("RC20Monitor"), false, MAKE_PF_CB(process_RC20Monitor_2));
                register_telegram_type(set_typeids[i], F("RC20Set"), false, MAKE_PF_CB(process_RC20Set_2));
            }
        } else {
            register_telegram_type(0xAF, F("RC20Remote"), false, MAKE_PF_CB(process_RC20Remote));
        }
        // RC30
    } else if (model == EMSdevice::EMS_DEVICE_FLAG_RC30) {
        monitor_typeids = {0x41};
        set_typeids     = {0xA7};
        for (uint8_t i = 0; i < monitor_typeids.size(); i++) {
            register_telegram_type(monitor_typeids[i], F("RC30Monitor"), false, MAKE_PF_CB(process_RC30Monitor));
            register_telegram_type(set_typeids[i], F("RC30Set"), false, MAKE_PF_CB(process_RC30Set));
        }

        // EASY
    } else if (model == EMSdevice::EMS_DEVICE_FLAG_EASY) {
        monitor_typeids = {0x0A};
        set_typeids     = {};
        register_telegram_type(monitor_typeids[0], F("EasyMonitor"), true, MAKE_PF_CB(process_EasyMonitor));

    } else if (model == EMSdevice::EMS_DEVICE_FLAG_CRF) {
        monitor_typeids = {0x02A5, 0x02A6, 0x02A7, 0x02A8};
        set_typeids     = {};
        for (uint8_t i = 0; i < monitor_typeids.size(); i++) {
            register_telegram_type(monitor_typeids[i], F("CRFMonitor"), true, MAKE_PF_CB(process_CRFMonitor));
        }

        // RC300/RC100
    } else if ((model == EMSdevice::EMS_DEVICE_FLAG_RC300) || (model == EMSdevice::EMS_DEVICE_FLAG_RC100)) {
        monitor_typeids = {0x02A5, 0x02A6, 0x02A7, 0x02A8};
        set_typeids     = {0x02B9, 0x02BA, 0x02BB, 0x02BC};
        summer_typeids  = {0x02AF, 0x02B0, 0x02B1, 0x02B2};
        curve_typeids   = {0x029B, 0x029C, 0x029D, 0x029E};
        summer2_typeids = {0x0471, 0x0472, 0x0473, 0x0474};
        for (uint8_t i = 0; i < monitor_typeids.size(); i++) {
            register_telegram_type(monitor_typeids[i], F("RC300Monitor"), false, MAKE_PF_CB(process_RC300Monitor));
            register_telegram_type(set_typeids[i], F("RC300Set"), false, MAKE_PF_CB(process_RC300Set));
            register_telegram_type(summer_typeids[i], F("RC300Summer"), false, MAKE_PF_CB(process_RC300Summer));
            register_telegram_type(curve_typeids[i], F("RC300Curves"), false, MAKE_PF_CB(process_RC300Curve));
            register_telegram_type(summer2_typeids[i], F("RC300Summer2"), true, MAKE_PF_CB(process_RC300Summer2));
        }
        register_telegram_type(0x2F5, F("RC300WWmode"), true, MAKE_PF_CB(process_RC300WWmode));
        register_telegram_type(0x31B, F("RC300WWtemp"), true, MAKE_PF_CB(process_RC300WWtemp));
        register_telegram_type(0x31D, F("RC300WWmode2"), false, MAKE_PF_CB(process_RC300WWmode2));
        register_telegram_type(0x31E, F("RC300WWmode2"), false, MAKE_PF_CB(process_RC300WWmode2));
        register_telegram_type(0x23A, F("RC300OutdoorTemp"), true, MAKE_PF_CB(process_RC300OutdoorTemp));
        register_telegram_type(0x267, F("RC300Floordry"), false, MAKE_PF_CB(process_RC300Floordry));
        register_telegram_type(0x240, F("RC300Settings"), true, MAKE_PF_CB(process_RC300Settings));

        // JUNKERS/HT3
    } else if (model == EMSdevice::EMS_DEVICE_FLAG_JUNKERS) {
        monitor_typeids = {0x016F, 0x0170, 0x0171, 0x0172};
        for (uint8_t i = 0; i < monitor_typeids.size(); i++) {
            register_telegram_type(monitor_typeids[i], F("JunkersMonitor"), false, MAKE_PF_CB(process_JunkersMonitor));
        }

        if (has_flags(EMS_DEVICE_FLAG_JUNKERS_OLD)) {
            // FR120, FR100
            set_typeids = {0x0179, 0x017A, 0x017B, 0x017C};
            for (uint8_t i = 0; i < monitor_typeids.size(); i++) {
                register_telegram_type(set_typeids[i], F("JunkersSet"), false, MAKE_PF_CB(process_JunkersSet2));
            }
        } else {
            set_typeids = {0x0165, 0x0166, 0x0167, 0x0168};
            for (uint8_t i = 0; i < monitor_typeids.size(); i++) {
                register_telegram_type(set_typeids[i], F("JunkersSet"), false, MAKE_PF_CB(process_JunkersSet));
            }
        }
    }

    // reserve some memory for the heating circuits (max 4 to start with)
    heating_circuits_.reserve(4);

    if (actual_master_thermostat != device_id) {
        LOG_DEBUG(F("Adding new thermostat with device ID 0x%02X"), device_id);
        return; // don't fetch data if more than 1 thermostat
    }

    //
    // this next section is only for the master thermostat....
    //
    LOG_DEBUG(F("Adding new thermostat with device ID 0x%02X (as master)"), device_id);

    // register device values for common values (not heating circuit)
    register_device_values();

    // only for for the master-thermostat, go a query all the heating circuits. This is only done once.
    // The automatic fetch will from now on only update the active heating circuits
    for (uint8_t i = 0; i < monitor_typeids.size(); i++) {
        EMSESP::send_read_request(monitor_typeids[i], device_id);
    }

    // HA will report an error in the climate component if it doesn't have the setpoint temp, current temperatures and the mode (e.g. auto)
    // The mode always comes later (1 minute) so we force a read request to suppress HA errors.
    // https://github.com/emsesp/EMS-ESP/commit/9d5bd11d268a61bbba8dcabacdb96db48b8829ac#commitcomment-48028642
    for (uint8_t i = 0; i < set_typeids.size(); i++) {
        EMSESP::send_read_request(set_typeids[i], device_id);
    }

    EMSESP::send_read_request(EMS_TYPE_RCTime, device_id);
    EMSESP::send_read_request(0x12, device_id); // read last error (only published on errors)
    EMSESP::send_read_request(0xA2, device_id); // read errorCode (only published on errors)
}

// publish HA config
bool Thermostat::publish_ha_config() {
    StaticJsonDocument<EMSESP_JSON_SIZE_HA_CONFIG> doc;
    doc["uniq_id"] = F_(thermostat);
    doc["ic"]      = F_(icondevice);

    char stat_t[Mqtt::MQTT_TOPIC_MAX_SIZE];
    snprintf_P(stat_t, sizeof(stat_t), "%s/%s", Mqtt::base().c_str(), Mqtt::tag_to_topic(device_type(), DeviceValueTAG::TAG_NONE).c_str());
    doc["stat_t"] = stat_t;

    char name_s[40];
    snprintf(name_s, sizeof(name_s), FSTR_(productid_fmt), device_type_name().c_str());
    doc["name"] = name_s;

    doc["val_tpl"] = FJSON("{{value_json.id}}");
    JsonObject dev = doc.createNestedObject("dev");
    dev["name"]    = FJSON("EMS-ESP Thermostat");
    dev["sw"]      = EMSESP_APP_VERSION;
    dev["mf"]      = brand_to_string();
    dev["mdl"]     = name();
    JsonArray ids  = dev.createNestedArray("ids");
    ids.add("ems-esp-thermostat");

    char topic[Mqtt::MQTT_TOPIC_MAX_SIZE];
    snprintf_P(topic, sizeof(topic), "sensor/%s/thermostat/config", Mqtt::base().c_str());
    Mqtt::publish_ha(topic, doc.as<JsonObject>()); // publish the config payload with retain flag

    return true;
}

// returns the heating circuit object based on the hc number
// of nullptr if it doesn't exist yet
std::shared_ptr<Thermostat::HeatingCircuit> Thermostat::heating_circuit(const uint8_t hc_num) {
    // if hc_num is 0 then return the first existing hc in the list
    if (hc_num == AUTO_HEATING_CIRCUIT) {
        for (const auto & heating_circuit : heating_circuits_) {
            if (heating_circuit->is_active()) {
                return heating_circuit;
            }
        }
    }

    // otherwise find a match
    for (const auto & heating_circuit : heating_circuits_) {
        if ((heating_circuit->hc_num() == hc_num) && heating_circuit->is_active()) {
            return heating_circuit;
        }
    }

    return nullptr; // not found
}

// determine which heating circuit the type ID is referring too
// returns pointer to the HeatingCircuit or nullptr if it can't be found
// if its a new one, the object will be created and also the fetch flags set
std::shared_ptr<Thermostat::HeatingCircuit> Thermostat::heating_circuit(std::shared_ptr<const Telegram> telegram) {
    // look through the Monitor and Set arrays to see if there is a match
    uint8_t hc_num  = 0;
    bool    toggle_ = false;
    // search monitor message types
    for (uint8_t i = 0; i < monitor_typeids.size(); i++) {
        if (monitor_typeids[i] == telegram->type_id) {
            hc_num  = i + 1;
            toggle_ = true;
            break;
        }
    }

    // not found, search status message/set types
    if (hc_num == 0) {
        for (uint8_t i = 0; i < set_typeids.size(); i++) {
            if (set_typeids[i] == telegram->type_id) {
                hc_num = i + 1;
                break;
            }
        }
    }

    // not found, search summer message types
    if (hc_num == 0) {
        for (uint8_t i = 0; i < summer_typeids.size(); i++) {
            if (summer_typeids[i] == telegram->type_id) {
                hc_num = i + 1;
                break;
            }
        }
    }

    // not found, search summer message types
    if (hc_num == 0) {
        for (uint8_t i = 0; i < summer2_typeids.size(); i++) {
            if (summer2_typeids[i] == telegram->type_id) {
                hc_num = i + 1;
                break;
            }
        }
    }

    // not found, search heating_curve message types
    if (hc_num == 0) {
        for (uint8_t i = 0; i < curve_typeids.size(); i++) {
            if (curve_typeids[i] == telegram->type_id) {
                hc_num = i + 1;
                break;
            }
        }
    }

    // not found, search timer message types
    if (hc_num == 0) {
        for (uint8_t i = 0; i < timer_typeids.size(); i++) {
            if (timer_typeids[i] == telegram->type_id) {
                hc_num = i + 1;
                break;
            }
        }
    }

    // not found, search device-id types for remote thermostats
    if (telegram->src >= 0x18 && telegram->src <= 0x1B) {
        hc_num  = telegram->src - 0x17;
        toggle_ = true;
    }

    // still didn't recognize it, ignore it
    if (hc_num == 0) {
        return nullptr;
    }

    // if we have the heating circuit already present, returns its object
    // otherwise create a new object and add it
    for (const auto & heating_circuit : heating_circuits_) {
        if (heating_circuit->hc_num() == hc_num) {
            return heating_circuit;
        }
    }
    // register new heatingcircuits only on active monitor telegrams
    if (!toggle_) {
        return nullptr;
    }
    /*
     * at this point we have discovered a new heating circuit
     */

    // if it's the first set the status flag
    if (heating_circuits_.size() == 0) {
        strlcpy(status_, "online", sizeof(status_));
        id_ = this->product_id();
    }

    // create a new heating circuit object
    auto new_hc = std::make_shared<Thermostat::HeatingCircuit>(hc_num, model());
    heating_circuits_.push_back(new_hc);

    // sort based on hc number so there's a nice order when displaying
    std::sort(heating_circuits_.begin(), heating_circuits_.end());

    // register the device values
    register_device_values_hc(new_hc);

    // now create the HA topics to send to MQTT for each sensor
    if (Mqtt::ha_enabled()) {
        publish_ha_config_hc(hc_num);
    }

    // set the flag saying we want its data during the next auto fetch
    toggle_fetch(monitor_typeids[hc_num - 1], toggle_);

    if (set_typeids.size()) {
        toggle_fetch(set_typeids[hc_num - 1], toggle_);
    }
    if (summer_typeids.size()) {
        toggle_fetch(summer_typeids[hc_num - 1], toggle_);
    }
    if (curve_typeids.size()) {
        toggle_fetch(curve_typeids[hc_num - 1], toggle_);
    }
    if (timer_typeids.size()) {
        toggle_fetch(timer_typeids[hc_num - 1], toggle_);
    }

    return heating_circuits_.back(); // even after sorting, this should still point back to the newly created HC
}

// publish config topic for HA MQTT Discovery for each of the heating circuit
// e.g. homeassistant/climate/ems-esp/thermostat_hc1/config
void Thermostat::publish_ha_config_hc(uint8_t hc_num) {
    StaticJsonDocument<EMSESP_JSON_SIZE_HA_CONFIG> doc;

    char str1[20];
    snprintf_P(str1, sizeof(str1), "Thermostat hc%d", hc_num);

    char str2[20];
    snprintf_P(str2, sizeof(str2), "thermostat_hc%d", hc_num);

    char str3[25];
    snprintf_P(str3, sizeof(str3), "~/%s", str2);
    doc["mode_cmd_t"] = str3;
    doc["temp_cmd_t"] = str3;
    doc["name"]       = str1;
    doc["uniq_id"]    = str2;
    doc["mode_cmd_t"] = str3;
    doc["temp_cmd_t"] = str3;
    doc["~"]          = Mqtt::base(); // ems-esp

    char topic_t[Mqtt::MQTT_TOPIC_MAX_SIZE];
    if (Mqtt::nested_format() == 1) {
        snprintf_P(topic_t, sizeof(topic_t), "~/%s", Mqtt::tag_to_topic(EMSdevice::DeviceType::THERMOSTAT, DeviceValueTAG::TAG_NONE).c_str());

        char mode_str_tpl[40];
        snprintf_P(mode_str_tpl, sizeof(mode_str_tpl), "{{value_json.hc%d.hamode}}", hc_num);
        doc["mode_stat_tpl"] = mode_str_tpl;

        char seltemp_str[30];
        snprintf_P(seltemp_str, sizeof(seltemp_str), "{{value_json.hc%d.seltemp}}", hc_num);
        doc["temp_stat_tpl"] = seltemp_str;

        char currtemp_str[30];
        snprintf_P(currtemp_str, sizeof(currtemp_str), "{{value_json.hc%d.hatemp}}", hc_num);
        doc["curr_temp_tpl"] = currtemp_str;


    } else {
        snprintf_P(topic_t, sizeof(topic_t), "~/%s", Mqtt::tag_to_topic(EMSdevice::DeviceType::THERMOSTAT, DeviceValueTAG::TAG_HC1 + hc_num - 1).c_str());

        doc["mode_stat_tpl"] = FJSON("{{value_json.hamode}}");
        doc["temp_stat_tpl"] = FJSON("{{value_json.seltemp}}");
        doc["curr_temp_tpl"] = FJSON("{{value_json.hatemp}}");
    }
    doc["mode_stat_t"] = topic_t;
    doc["temp_stat_t"] = topic_t;
    doc["curr_temp_t"] = topic_t;

    doc["min_temp"]  = FJSON("5");
    doc["max_temp"]  = FJSON("30");
    doc["temp_step"] = FJSON("0.5");

    // the HA climate component only responds to auto, heat and off
    JsonArray modes = doc.createNestedArray("modes");
    modes.add("auto");
    modes.add("heat");
    modes.add("off");

    JsonObject dev = doc.createNestedObject("dev");
    dev["name"]    = FJSON("EMS-ESP Thermostat");
    dev["sw"]      = EMSESP_APP_VERSION;
    dev["mf"]      = brand_to_string();
    dev["mdl"]     = name();
    JsonArray ids  = dev.createNestedArray("ids");
    ids.add("ems-esp-thermostat");

    char topic[Mqtt::MQTT_TOPIC_MAX_SIZE];
    snprintf_P(topic, sizeof(topic), "climate/%s/thermostat_hc%d/config", Mqtt::base().c_str(), hc_num);
    Mqtt::publish_ha(topic, doc.as<JsonObject>()); // publish the config payload with retain flag

    // enable the a special "thermostat_hc<n>" topic to take both mode strings and floats for each of the heating circuits
    std::string topic2(Mqtt::MQTT_TOPIC_MAX_SIZE, '\0');
    snprintf_P(&topic2[0], topic2.capacity() + 1, "thermostat_hc%d", hc_num);
    register_mqtt_topic(topic2, [=](const char * m) { return thermostat_ha_cmd(m, hc_num); });
}

// for HA specifically when receiving over MQTT in the thermostat topic
// e.g. thermostat_hc1
// it could be either a 'mode' or a float value for a temperature. we try brute force both and see which one works.
// return true if it parses the message correctly
bool Thermostat::thermostat_ha_cmd(const char * message, uint8_t hc_num) {
    // check if it's json. We know the message isn't empty
    if (message[0] == '{') {
        return false;
    }

    // check for mode first, which is a string
    if (message[0] >= 'A') {
        if (set_mode(message, hc_num)) {
            return true;
        }
    }
    if ((message[0] >= '0' && message[0] <= '9') || message[0] == '-') {
        // otherwise handle as a numerical temperature value and set the setpoint temp
        float f = strtof((char *)message, 0);
        set_temperature(f, HeatingCircuit::Mode::AUTO, hc_num);
        return true;
    }

    return false;
}

// decodes the thermostat mode for the heating circuit based on the thermostat type
// modes are off, manual, auto, day, night and holiday
uint8_t Thermostat::HeatingCircuit::get_mode() const {
    uint8_t model = get_model();

    if (!Helpers::hasValue(mode)) {
        return HeatingCircuit::Mode::UNKNOWN;
    }

    if (model == EMSdevice::EMS_DEVICE_FLAG_RC20) {
        if (mode == 0) {
            return HeatingCircuit::Mode::OFF;
        } else if (mode == 1) {
            return HeatingCircuit::Mode::MANUAL;
        } else if (mode == 2) {
            return HeatingCircuit::Mode::AUTO;
        }
    } else if (model == EMSdevice::EMS_DEVICE_FLAG_CRF) {
        if (mode == 0) {
            return HeatingCircuit::Mode::AUTO;
        } else if (mode == 1) {
            return HeatingCircuit::Mode::OFF;
        }
    } else if ((model == EMSdevice::EMS_DEVICE_FLAG_RC300) || (model == EMSdevice::EMS_DEVICE_FLAG_RC100)) {
        if (mode == 0) {
            return HeatingCircuit::Mode::MANUAL;
        } else if (mode == 1) {
            return HeatingCircuit::Mode::AUTO;
        }
    } else if (model == EMSdevice::EMS_DEVICE_FLAG_JUNKERS) {
        if (mode == 1) {
            return HeatingCircuit::Mode::MANUAL;
        } else if (mode == 2) {
            return HeatingCircuit::Mode::AUTO;
        } else if (mode == 3) {
            return HeatingCircuit::Mode::HOLIDAY;
        }
    } else { // default for all other thermostats
        if (mode == 0) {
            return HeatingCircuit::Mode::NIGHT;
        } else if (mode == 1) {
            return HeatingCircuit::Mode::DAY;
        } else if (mode == 2) {
            return HeatingCircuit::Mode::AUTO;
        }
    }

    return HeatingCircuit::Mode::UNKNOWN;
}

// figures out the thermostat day/night mode depending on the thermostat type
// mode types are day, night, eco, comfort
uint8_t Thermostat::HeatingCircuit::get_mode_type() const {
    uint8_t model = get_model();

    if (model == EMS_DEVICE_FLAG_JUNKERS) {
        if (modetype == 3) {
            return HeatingCircuit::Mode::HEAT;
        } else if (modetype == 2) {
            return HeatingCircuit::Mode::ECO;
        } else if (modetype == 1) {
            return HeatingCircuit::Mode::NOFROST;
        }
    } else if ((model == EMS_DEVICE_FLAG_RC35) || (model == EMS_DEVICE_FLAG_RC30_N)) {
        if (modetype == 0) {
            return HeatingCircuit::Mode::NIGHT;
        } else if (modetype == 1) {
            return HeatingCircuit::Mode::DAY;
        }
    } else if (model == EMS_DEVICE_FLAG_CRF) {
        if (modetype == 0) {
            return HeatingCircuit::Mode::OFF;
        } else if (modetype == 1) {
            return HeatingCircuit::Mode::ON;
        }
    } else if (model == EMS_DEVICE_FLAG_RC300) {
        if (modetype == 0) {
            return HeatingCircuit::Mode::ECO;
        } else if (modetype == 1) {
            return HeatingCircuit::Mode::COMFORT;
        }
    } else if (model == EMS_DEVICE_FLAG_RC100) {
        return HeatingCircuit::Mode::DAY; // no other modes on these devices
    }

    return HeatingCircuit::Mode::DAY;
}

// decodes the thermostat mode based on the thermostat type
// works with both modes and mode_types
std::string Thermostat::mode_tostring(uint8_t mode) {
    switch (mode) {
    case HeatingCircuit::Mode::OFF:
        return read_flash_string(F_(off));
        break;
    case HeatingCircuit::Mode::MANUAL:
        return read_flash_string(F_(manual));
        break;
    case HeatingCircuit::Mode::DAY:
        return read_flash_string(F_(day));
        break;
    case HeatingCircuit::Mode::NIGHT:
        return read_flash_string(F_(night));
        break;
    case HeatingCircuit::Mode::ECO:
        return read_flash_string(F_(eco));
        break;
    case HeatingCircuit::Mode::COMFORT:
        return read_flash_string(F_(comfort));
        break;
    case HeatingCircuit::Mode::HEAT:
        return read_flash_string(F_(heat));
        break;
    case HeatingCircuit::Mode::HOLIDAY:
        return read_flash_string(F_(holiday));
        break;
    case HeatingCircuit::Mode::NOFROST:
        return read_flash_string(F_(nofrost));
        break;
    case HeatingCircuit::Mode::AUTO:
        return read_flash_string(F_(auto));
        break;
    case HeatingCircuit::Mode::SUMMER:
        return read_flash_string(F_(summer));
        break;
    case HeatingCircuit::Mode::OFFSET:
        return read_flash_string(F_(offset));
        break;
    case HeatingCircuit::Mode::DESIGN:
        return read_flash_string(F_(design));
        break;
    case HeatingCircuit::Mode::MINFLOW:
        return read_flash_string(F_(minflow));
        break;
    case HeatingCircuit::Mode::MAXFLOW:
        return read_flash_string(F_(maxflow));
        break;
    case HeatingCircuit::Mode::ROOMINFLUENCE:
        return read_flash_string(F_(roominfluence[0]));
        break;
    case HeatingCircuit::Mode::FLOWOFFSET:
        return read_flash_string(F_(flowtempoffset[0]));
        break;
    case HeatingCircuit::Mode::TEMPAUTO:
        return read_flash_string(F_(tempauto));
        break;
    case HeatingCircuit::Mode::NOREDUCE:
        return read_flash_string(F_(noreduce));
        break;
    default:
    case HeatingCircuit::Mode::UNKNOWN:
        return read_flash_string(F_(unknown));
        break;
    }
}

// 0xA8 - for reading the mode from the RC20 thermostat (0x17)
void Thermostat::process_RC20Set(std::shared_ptr<const Telegram> telegram) {
    std::shared_ptr<Thermostat::HeatingCircuit> hc = heating_circuit(telegram);
    if (hc == nullptr) {
        return;
    }
    has_update(telegram->read_value(hc->mode, 23));
    hc->hamode = hc->mode; // set special HA mode
}

// type 0xAE - data from the RC20 thermostat (0x17) - not for RC20's
// 17 00 AE 00 80 12 2E 00 D0 00 00 64 (#data=8)
// https://github.com/emsesp/EMS-ESP/issues/361
void Thermostat::process_RC20Monitor_2(std::shared_ptr<const Telegram> telegram) {
    std::shared_ptr<Thermostat::HeatingCircuit> hc = heating_circuit(telegram);
    if (hc == nullptr) {
        return;
    }
    // has_update(telegram->read_bitvalue(hc->modetype, 0, 7));       // day/night-mode MSB 7th bit is day
    // modes byte 0,1: day: 8002, night: 0000, auto-day:0402, auto-night:0400
    has_update(telegram->read_bitvalue(hc->modetype, 1, 1));       // day/night
    has_update(telegram->read_value(hc->setpoint_roomTemp, 2, 1)); // is * 2, force as single byte
    has_update(telegram->read_value(hc->curr_roomTemp, 3));        // is * 10
    // RC25 extension:
    has_update(telegram->read_bitvalue(hc->summermode, 1, 0));
}

// 0xAD - for reading the mode from the RC20/ES72 thermostat (0x17)
// see https://github.com/emsesp/EMS-ESP/issues/334#issuecomment-611698259
// offset: 01-nighttemp, 02-daytemp, 03-mode, 0B-program(1-9), 0D-setpoint_roomtemp(temporary)
// RC25(0x17) -> All(0x00), ?(0xAD), data: 01 27 2D 00 44 05 01 FF 28 19 0A 07 00 00 F6 12 5A 11 00 28 05 05 00
void Thermostat::process_RC20Set_2(std::shared_ptr<const Telegram> telegram) {
    std::shared_ptr<Thermostat::HeatingCircuit> hc = heating_circuit(telegram);
    if (hc == nullptr) {
        return;
    }
    has_update(telegram->read_value(hc->nighttemp, 1)); // is * 2,
    has_update(telegram->read_value(hc->daytemp, 2));   // is * 2,
    has_update(telegram->read_value(hc->mode, 3));
    hc->hamode = hc->mode;                             // set special HA mode
    has_update(telegram->read_value(hc->program, 11)); // 1 .. 9 predefined programs
    // RC25 extension:
    has_update(telegram->read_value(ibaMinExtTemperature_, 14));
    has_update(telegram->read_value(hc->minflowtemp, 15));
    has_update(telegram->read_value(hc->maxflowtemp, 16));
    has_update(telegram->read_value(hc->summertemp, 17));
}

// 0xAF - for reading the roomtemperature from the RC20/ES72 thermostat (0x18, 0x19, ..)
void Thermostat::process_RC20Remote(std::shared_ptr<const Telegram> telegram) {
    std::shared_ptr<Thermostat::HeatingCircuit> hc = heating_circuit(telegram);
    if (hc == nullptr) {
        return;
    }
    has_update(telegram->read_value(hc->curr_roomTemp, 0));
}

// type 0xB1 - data from the RC10 thermostat (0x17)
void Thermostat::process_RC10Monitor(std::shared_ptr<const Telegram> telegram) {
    std::shared_ptr<Thermostat::HeatingCircuit> hc = heating_circuit(telegram);
    if (hc == nullptr) {
        return;
    }

    has_update(telegram->read_value(hc->setpoint_roomTemp, 1, 1)); // is * 2, force as single byte
    has_update(telegram->read_value(hc->curr_roomTemp, 2));        // is * 10
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

// type 0xB0 - for reading the mode from the RC10 thermostat (0x17)
void Thermostat::process_RC10Set(std::shared_ptr<const Telegram> telegram) {
    // mode not implemented yet
}
#pragma GCC diagnostic pop

// type 0x0165, ff
void Thermostat::process_JunkersSet(std::shared_ptr<const Telegram> telegram) {
    std::shared_ptr<Thermostat::HeatingCircuit> hc = heating_circuit(telegram);
    if (hc == nullptr) {
        return;
    }

    has_update(telegram->read_value(hc->daytemp, 17));     // is * 2
    has_update(telegram->read_value(hc->nighttemp, 16));   // is * 2
    has_update(telegram->read_value(hc->nofrosttemp, 15)); // is * 2
    has_update(telegram->read_enumvalue(hc->mode, 14, 1)); // 0 = nofrost, 1 = eco, 2 = heat, 3 = auto
    hc->hamode = hc->mode ? hc->mode - 1 : 0;              // set special HA mode: off, on, auto
}

// type 0x0179, ff
void Thermostat::process_JunkersSet2(std::shared_ptr<const Telegram> telegram) {
    std::shared_ptr<Thermostat::HeatingCircuit> hc = heating_circuit(telegram);
    if (hc == nullptr) {
        return;
    }

    has_update(telegram->read_value(hc->daytemp, 7));     // is * 2
    has_update(telegram->read_value(hc->nighttemp, 6));   // is * 2
    has_update(telegram->read_value(hc->nofrosttemp, 5)); // is * 2
    has_update(telegram->read_enumvalue(hc->mode, 4, 1)); // 0 = nofrost, 1 = eco, 2 = heat, 3 = auto
    hc->hamode = hc->mode ? hc->mode - 1 : 0;             // set special HA mode: off, on, auto
}

// type 0xA3 - for external temp settings from the the RC* thermostats (e.g. RC35)
void Thermostat::process_RCOutdoorTemp(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram->read_value(dampedoutdoortemp_, 0));
    has_update(telegram->read_value(tempsensor1_, 3)); // sensor 1 - is * 10
    has_update(telegram->read_value(tempsensor2_, 5)); // sensor 2 - is * 10
}

// 0x91 - data from the RC20 thermostat (0x17) - 15 bytes long
void Thermostat::process_RC20Monitor(std::shared_ptr<const Telegram> telegram) {
    std::shared_ptr<Thermostat::HeatingCircuit> hc = heating_circuit(telegram);
    if (hc == nullptr) {
        return;
    }

    has_update(telegram->read_value(hc->setpoint_roomTemp, 1, 1)); // is * 2, force as single byte
    has_update(telegram->read_value(hc->curr_roomTemp, 2));        // is * 10
}

// type 0x0A - data from the Nefit Easy/TC100 thermostat (0x18) - 31 bytes long
void Thermostat::process_EasyMonitor(std::shared_ptr<const Telegram> telegram) {
    std::shared_ptr<Thermostat::HeatingCircuit> hc = heating_circuit(telegram);
    if (hc == nullptr) {
        return;
    }

    has_update(telegram->read_value(hc->curr_roomTemp, 8));      // is * 100
    has_update(telegram->read_value(hc->setpoint_roomTemp, 10)); // is * 100

    hc->hamode = 1; // fixed to heat
}

// Settings Parameters - 0xA5 - RC30_1
void Thermostat::process_IBASettings(std::shared_ptr<const Telegram> telegram) {
    // 22 - display line on RC35

    // display on Thermostat: 0 int. temp, 1 int. setpoint, 2 ext. temp., 3 burner temp., 4 ww temp, 5 functioning mode, 6 time, 7 data, 8 smoke temp
    has_update(telegram->read_value(ibaMainDisplay_, 0));
    has_update(telegram->read_value(ibaLanguage_, 1));          // language on Thermostat: 0 german, 1 dutch, 2 french, 3 italian
    has_update(telegram->read_value(ibaCalIntTemperature_, 2)); // offset int. temperature sensor, by * 0.1 Kelvin
    has_update(telegram->read_value(ibaBuildingType_, 6));      // building type: 0 = light, 1 = medium, 2 = heavy
    has_update(telegram->read_value(ibaMinExtTemperature_, 5)); // min ext temp for heating curve, in deg., 0xF6=-10, 0x0 = 0, 0xFF=-1
    has_update(telegram->read_value(ibaClockOffset_, 12));      // offset (in sec) to clock, 0xff = -1 s, 0x02 = 2 s
    has_update(telegram->read_value(ibaDamping_, 21));          // damping 0-off, 0xff-on
}

// Settings WW 0x37 - RC35
void Thermostat::process_RC35wwSettings(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram->read_value(wwProgMode_, 0));     // 0-like hc, 0xFF own prog
    has_update(telegram->read_value(wwCircProg_, 1));     // 0-like hc, 0xFF own prog
    has_update(telegram->read_value(wwMode_, 2));         // 0 off, 1-on, 2-auto
    has_update(telegram->read_value(wwCircMode_, 3));     // 0 off, 1-on, 2-auto
    has_update(telegram->read_value(wwDisinfect_, 4));    // 0-off, 0xFF on
    has_update(telegram->read_value(wwDisinfectDay_, 5)); // 0-6 Day of week, 7 every day
    has_update(telegram->read_value(wwDisinfectHour_, 6));
    has_update(telegram->read_value(wwMaxTemp_, 8));    // Limiter 60 degrees
    has_update(telegram->read_value(wwOneTimeKey_, 9)); // 0-off, 0xFF on
}

// type 0x6F - FR10/FR50/FR100/FR110/FR120 Junkers
void Thermostat::process_JunkersMonitor(std::shared_ptr<const Telegram> telegram) {
    // ignore single byte telegram messages
    if (telegram->message_length <= 1) {
        return;
    }

    std::shared_ptr<Thermostat::HeatingCircuit> hc = heating_circuit(telegram);
    if (hc == nullptr) {
        return;
    }

    has_update(telegram->read_value(hc->curr_roomTemp, 4));     // value is * 10
    has_update(telegram->read_value(hc->setpoint_roomTemp, 2)); // value is * 10

    has_update(telegram->read_enumvalue(hc->modetype, 0, 1)); // 1 = nofrost, 2 = eco, 3 = heat
}

// type 0x02A5 - data from Worchester CRF200
void Thermostat::process_CRFMonitor(std::shared_ptr<const Telegram> telegram) {
    std::shared_ptr<Thermostat::HeatingCircuit> hc = heating_circuit(telegram);
    if (hc == nullptr) {
        return;
    }
    has_update(telegram->read_value(hc->curr_roomTemp, 0)); // is * 10
    has_update(telegram->read_bitvalue(hc->modetype, 2, 0));
    has_update(telegram->read_bitvalue(hc->mode, 2, 4));           // bit 4, mode (auto=0 or off=1)
    hc->hamode = 2 - 2 * hc->mode;                                 // set special HA mode
    has_update(telegram->read_value(hc->setpoint_roomTemp, 6, 1)); // is * 2, force as single byte
    has_update(telegram->read_value(hc->targetflowtemp, 4));
}

// type 0x02A5 - data from the Nefit RC1010/3000 thermostat (0x18) and RC300/310s on 0x10
void Thermostat::process_RC300Monitor(std::shared_ptr<const Telegram> telegram) {
    std::shared_ptr<Thermostat::HeatingCircuit> hc = heating_circuit(telegram);
    if (hc == nullptr) {
        return;
    }

    has_update(telegram->read_value(hc->curr_roomTemp, 0)); // is * 10
    has_update(telegram->read_bitvalue(hc->modetype, 10, 1));
    has_update(telegram->read_bitvalue(hc->mode, 10, 0)); // bit 1, mode (auto=1 or manual=0)
    hc->hamode = hc->mode + 1;                            // set special HA mode

    // if manual, take the current setpoint temp at pos 6
    // if auto, take the next setpoint temp at pos 7
    // pos 3 is the current target temp and sometimes can be 0
    // see https://github.com/emsesp/EMS-ESP/issues/256#issuecomment-585171426
    // pos 3 actual setpoint (optimized), i.e. changes with temporary change, summer/holiday-modes
    // pos 6 actual setpoint according to programmed changes eco/comfort
    // pos 7 next setpoint in the future, time to next setpoint in pos 8/9
    has_update(telegram->read_value(hc->setpoint_roomTemp, 3, 1)); // is * 2, force as single byte
    has_update(telegram->read_bitvalue(hc->summermode, 2, 4));
    has_update(telegram->read_value(hc->targetflowtemp, 4));
}

// type 0x02B9 EMS+ for reading from RC300/RC310 thermostat
void Thermostat::process_RC300Set(std::shared_ptr<const Telegram> telegram) {
    std::shared_ptr<Thermostat::HeatingCircuit> hc = heating_circuit(telegram);
    if (hc == nullptr) {
        return;
    }

    // NOTE when setting the room temp we pick from two values, hopefully one is correct!
    // manual is position 10
    // comfort is position 2, there are 3 levels in pos 3, 2, 1
    // eco is position 4
    // auto is position 8, temporary until next switch
    // actual setpoint taken from RC300Monitor (Michael 12.06.2020)
    // has_update(telegram->read_value(hc->setpoint_roomTemp, 8, 1);  // single byte conversion, value is * 2 - auto?
    // has_update(telegram->read_value(hc->setpoint_roomTemp, 10, 1); // single byte conversion, value is * 2 - manual

    // check why mode is both in the Monitor and Set for the RC300. It'll be read twice!
    // has_update(telegram->read_value(hc->mode, 0); // Auto = xFF, Manual = x00 eg. 10 00 FF 08 01 B9 FF
    has_update(telegram->read_value(hc->daytemp, 2));   // is * 2
    has_update(telegram->read_value(hc->nighttemp, 4)); // is * 2
    has_update(telegram->read_value(hc->tempautotemp, 8));
    has_update(telegram->read_value(hc->manualtemp, 10)); // is * 2
    has_update(telegram->read_value(hc->program, 11));    // timer program 1 or 2
}

// types 0x2AF ff
void Thermostat::process_RC300Summer(std::shared_ptr<const Telegram> telegram) {
    std::shared_ptr<Thermostat::HeatingCircuit> hc = heating_circuit(telegram);
    if (hc == nullptr) {
        return;
    }

    has_update(telegram->read_value(hc->roominfluence, 0));
    has_update(telegram->read_value(hc->offsettemp, 2));
    // dont use these values if we have telegram 0x471 ff
    if (!is_fetch(summer2_typeids[hc->hc_num() - 1])) {
        has_update(telegram->read_value(hc->summertemp, 6));
        has_update(telegram->read_value(hc->summer_setmode, 7));
    }

    if (hc->heatingtype < 3) {
        has_update(telegram->read_value(hc->designtemp, 4));
    } else {
        has_update(telegram->read_value(hc->designtemp, 5));
    }

    has_update(telegram->read_value(hc->minflowtemp, 8));
    has_update(telegram->read_value(hc->fastHeatupFactor, 10));
}

// types 0x471 ff
void Thermostat::process_RC300Summer2(std::shared_ptr<const Telegram> telegram) {
    std::shared_ptr<Thermostat::HeatingCircuit> hc = heating_circuit(telegram);
    if (hc == nullptr) {
        return;
    }
    has_update(telegram->read_value(hc->summer_setmode, 0));
    has_update(telegram->read_value(hc->summertemp, 1));
}

// types 0x29B ff
void Thermostat::process_RC300Curve(std::shared_ptr<const Telegram> telegram) {
    std::shared_ptr<Thermostat::HeatingCircuit> hc = heating_circuit(telegram);
    if (hc == nullptr) {
        return;
    }

    has_update(telegram->read_value(hc->controlmode, 0)); // 1-outdoor, 2-simple, 3-MPC, 4-room, 5-power, 6-const
    has_update(telegram->read_value(hc->heatingtype, 1)); // 1=radiator, 2=convector, 3=floor
    has_update(telegram->read_value(hc->nofrosttemp, 6));

    if (hc->heatingtype < 3) {
        has_update(telegram->read_value(hc->maxflowtemp, 8));
    } else {
        has_update(telegram->read_value(hc->maxflowtemp, 7));
    }
}

// types 0x31B (and 0x31C?)
void Thermostat::process_RC300WWtemp(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram->read_value(wwSetTemp_, 0));
    has_update(telegram->read_value(wwSetTempLow_, 1));
}

// type 02F5
// RC300WWmode(0x2F5), data: 01 FF 04 00 00 00 08 05 00 08 04 00 00 00 00 00 00 00 00 00 01
void Thermostat::process_RC300WWmode(std::shared_ptr<const Telegram> telegram) {
    // circulation pump see: https://github.com/Th3M3/buderus_ems-wiki/blob/master/Einstellungen%20der%20Bedieneinheit%20RC310.md
    has_update(telegram->read_value(wwCircPump_, 1)); // FF=off, 0=on ?

    has_update(telegram->read_value(wwMode_, 2));            // 0=off, 1=low, 2=high, 3=auto, 4=own prog
    has_update(telegram->read_value(wwCircMode_, 3));        // 0=off, 1=on, 2=auto, 4=own?
    has_update(telegram->read_value(wwChargeDuration_, 10)); // value in steps of 15 min
    has_update(telegram->read_value(wwCharge_, 11));
}

// types 0x31D and 0x31E
// RC300WWmode2(0x31D), data: 00 00 09 07
void Thermostat::process_RC300WWmode2(std::shared_ptr<const Telegram> telegram) {
    // 0x31D for WW system 1, 0x31E for WW system 2
    // pos 1 = holiday mode
    // pos 2 = current status of DHW setpoint
    // pos 3 = current status of DHW circulation pump
    if (telegram->type_id == 0x031D) {
        has_update(telegram->read_value(wwExtra1_, 0)); // 0=no, 1=yes
    } else {
        has_update(telegram->read_value(wwExtra2_, 0)); // 0=no, 1=yes
    }
}

// 0x23A damped outdoor temp
void Thermostat::process_RC300OutdoorTemp(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram->read_value(dampedoutdoortemp2_, 0)); // is *10
}

// 0x240 RC300 parameter
void Thermostat::process_RC300Settings(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram->read_enumvalue(ibaBuildingType_, 9, 1)); // 1=light, 2=medium, 3=heavy
    has_update(telegram->read_value(ibaMinExtTemperature_, 10));
}

// 0x267 RC300 floordrying
void Thermostat::process_RC300Floordry(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram->read_value(floordrystatus_, 0));
    has_update(telegram->read_value(floordrytemp_, 1));
}

// type 0x41 - data from the RC30 thermostat(0x10) - 14 bytes long
void Thermostat::process_RC30Monitor(std::shared_ptr<const Telegram> telegram) {
    std::shared_ptr<Thermostat::HeatingCircuit> hc = heating_circuit(telegram);
    if (hc == nullptr) {
        return;
    }

    has_update(telegram->read_value(hc->setpoint_roomTemp, 1, 1)); // is * 2, force as single byte
    has_update(telegram->read_value(hc->curr_roomTemp, 2));
}

// type 0xA7 - for reading the mode from the RC30 thermostat (0x10)
void Thermostat::process_RC30Set(std::shared_ptr<const Telegram> telegram) {
    std::shared_ptr<Thermostat::HeatingCircuit> hc = heating_circuit(telegram);
    if (hc == nullptr) {
        return;
    }

    has_update(telegram->read_value(hc->mode, 23));
    hc->hamode = hc->mode; // set special HA mode
}

// type 0x3E (HC1), 0x48 (HC2), 0x52 (HC3), 0x5C (HC4) - data from the RC35 thermostat (0x10) - 16 bytes
void Thermostat::process_RC35Monitor(std::shared_ptr<const Telegram> telegram) {
    // exit if the 15th byte (second from last) is 0x00, which I think is calculated flow setpoint temperature
    // with weather controlled RC35s this value is >=5, otherwise can be zero and our setpoint temps will be incorrect
    // see https://github.com/emsesp/EMS-ESP/issues/373#issuecomment-627907301
    if (telegram->offset > 0 || telegram->message_length < 15) {
        return;
    }

    if (telegram->message_data[14] == 0x00) {
        return;
    }

    std::shared_ptr<Thermostat::HeatingCircuit> hc = heating_circuit(telegram);
    if (hc == nullptr) {
        return;
    }

    has_update(telegram->read_value(hc->setpoint_roomTemp, 2, 1)); // is * 2, force to single byte, is 0 in summermode
    has_update(telegram->read_value(hc->curr_roomTemp, 3));        // is * 10 - or 0x7D00 if thermostat is mounted on boiler

    has_update(telegram->read_bitvalue(hc->modetype, 1, 1));
    has_update(telegram->read_bitvalue(hc->summermode, 1, 0));
    has_update(telegram->read_bitvalue(hc->holidaymode, 0, 5));

    has_update(telegram->read_value(hc->targetflowtemp, 14));
}

// type 0x3D (HC1), 0x47 (HC2), 0x51 (HC3), 0x5B (HC4) - Working Mode Heating - for reading the mode from the RC35 thermostat (0x10)
void Thermostat::process_RC35Set(std::shared_ptr<const Telegram> telegram) {
    // check to see we have a valid type. heating: 1 radiator, 2 convectors, 3 floors, 4 room supply
    if (telegram->offset == 0 && telegram->message_data[0] == 0x00) {
        return;
    }

    std::shared_ptr<Thermostat::HeatingCircuit> hc = heating_circuit(telegram);
    if (hc == nullptr) {
        return;
    }

    has_update(telegram->read_value(hc->heatingtype, 0));   // 0- off, 1-radiator, 2-convector, 3-floor
    has_update(telegram->read_value(hc->nighttemp, 1));     // is * 2
    has_update(telegram->read_value(hc->daytemp, 2));       // is * 2
    has_update(telegram->read_value(hc->holidaytemp, 3));   // is * 2
    has_update(telegram->read_value(hc->roominfluence, 4)); // is * 1
    has_update(telegram->read_value(hc->offsettemp, 6));    // is * 2
    has_update(telegram->read_value(hc->mode, 7));          // night, day, auto
    hc->hamode = hc->mode;                                  // set special HA mode

    has_update(telegram->read_value(hc->wwprio, 21));         // 0xFF for on
    has_update(telegram->read_value(hc->summertemp, 22));     // is * 1
    has_update(telegram->read_value(hc->nofrosttemp, 23));    // is * 1
    has_update(telegram->read_value(hc->flowtempoffset, 24)); // is * 1, only in mixed circuits
    has_update(telegram->read_value(hc->reducemode, 25));     // 0-nofrost, 1-reduce, 2-roomhold, 3-outdoorhold
    has_update(telegram->read_value(hc->control, 26));        // 0-off, 1-RC20 (remote), 2-RC35
    has_update(telegram->read_value(hc->controlmode, 33));    // 0-outdoortemp, 1-roomtemp
    has_update(telegram->read_value(hc->tempautotemp, 37));
    has_update(telegram->read_value(hc->noreducetemp, 38)); // outdoor temperature for no reduce
    has_update(telegram->read_value(hc->minflowtemp, 16));
    if (hc->heatingtype == 3) {
        has_update(telegram->read_value(hc->designtemp, 36));  // is * 1
        has_update(telegram->read_value(hc->maxflowtemp, 35)); // is * 1
    } else {
        has_update(telegram->read_value(hc->designtemp, 17));  // is * 1
        has_update(telegram->read_value(hc->maxflowtemp, 15)); // is * 1
    }
}

// type 0x3F (HC1), 0x49 (HC2), 0x53 (HC3), 0x5D (HC4) - timer setting
void Thermostat::process_RC35Timer(std::shared_ptr<const Telegram> telegram) {
    std::shared_ptr<Thermostat::HeatingCircuit> hc = heating_circuit(telegram);
    if (hc == nullptr) {
        return;
    }

    has_update(telegram->read_value(hc->program, 84)); // 0 .. 10, 0-userprogram 1, 10-userprogram 2
    has_update(telegram->read_value(hc->pause, 85));   // time in hours
    has_update(telegram->read_value(hc->party, 86));   // time in hours
}

// process_RCTime - type 0x06 - date and time from a thermostat - 14 bytes long
void Thermostat::process_RCTime(std::shared_ptr<const Telegram> telegram) {
    if (telegram->offset > 0 || telegram->message_length < 5) {
        return;
    }

    if (flags() == EMS_DEVICE_FLAG_EASY) {
        return; // not supported
    }

    if (telegram->message_length < 7) {
        return;
    }

    if (telegram->message_data[7] & 0x0C) { // date and time not valid
        set_datetime("ntp", -1);            // set from NTP
        return;
    }

    auto timeold = dateTime_;
    // render time to HH:MM:SS DD/MM/YYYY
    // had to create separate buffers because of how printf works
    // IVT reports Year with high bit set.?
    char buf1[6];
    char buf2[6];
    char buf3[6];
    char buf4[6];
    char buf5[6];
    char buf6[6];
    snprintf_P(dateTime_,
               sizeof(dateTime_),
               "%s:%s:%s %s/%s/%s",
               Helpers::smallitoa(buf1, telegram->message_data[2]),           // hour
               Helpers::smallitoa(buf2, telegram->message_data[4]),           // minute
               Helpers::smallitoa(buf3, telegram->message_data[5]),           // second
               Helpers::smallitoa(buf4, telegram->message_data[3]),           // day
               Helpers::smallitoa(buf5, telegram->message_data[1]),           // month
               Helpers::itoa(buf6, (telegram->message_data[0] & 0x7F) + 2000) // year
    );

    has_update((strcmp(timeold, dateTime_) != 0));
}

// process_RCError - type 0xA2 - error message - 14 bytes long
// 10 00 A2 00 41 32 32 03 30 00 02 00 00 00 00 00 00 02 CRC
//              A  2  2  816
void Thermostat::process_RCError(std::shared_ptr<const Telegram> telegram) {
    if (telegram->offset > 0 || telegram->message_length < 5) {
        return;
    }

    char buf[4];
    buf[0] = telegram->message_data[0];
    buf[1] = telegram->message_data[1];
    buf[2] = telegram->message_data[2];
    buf[3] = 0;
    has_update(telegram->read_value(errorNumber_, 3));
    snprintf_P(errorCode_, sizeof(errorCode_), "%s(%d)", buf, errorNumber_);
}

// 0x12 error log
void Thermostat::process_RCErrorMessage(std::shared_ptr<const Telegram> telegram) {
    if (telegram->offset > 0 || telegram->message_length < 12) {
        return;
    }

    // data: displaycode(2), errornumber(2), year, month, hour, day, minute, duration(2), src-addr
    if (telegram->message_data[4] & 0x80) { // valid date
        char     code[3];
        uint16_t codeNo = EMS_VALUE_USHORT_NOTSET;
        code[0]         = telegram->message_data[0];
        code[1]         = telegram->message_data[1];
        code[2]         = 0;
        telegram->read_value(codeNo, 2);
        uint16_t year  = (telegram->message_data[4] & 0x7F) + 2000;
        uint8_t  month = telegram->message_data[5];
        uint8_t  day   = telegram->message_data[7];
        uint8_t  hour  = telegram->message_data[6];
        uint8_t  min   = telegram->message_data[8];
        snprintf_P(lastCode_, sizeof(lastCode_), "%s(%d) %02d.%02d.%d %02d:%02d", code, codeNo, day, month, year, hour, min);
    }
}

// 0xA5 - Set minimum external temperature
bool Thermostat::set_minexttemp(const char * value, const int8_t id) {
    int mt = 0;
    if (!Helpers::value2number(value, mt)) {
        LOG_WARNING(F("Set min external temperature: Invalid value"));
        return false;
    }

    LOG_INFO(F("Setting min external temperature to %d C"), mt);
    if ((model() == EMS_DEVICE_FLAG_RC300) || (model() == EMS_DEVICE_FLAG_RC100)) {
        write_command(0x240, 10, mt, 0x240);
    } else {
        write_command(EMS_TYPE_IBASettings, 5, mt, EMS_TYPE_IBASettings);
    }

    return true;
}

// 0xA5 - Clock offset
bool Thermostat::set_clockoffset(const char * value, const int8_t id) {
    int co = 0;
    if (!Helpers::value2number(value, co)) {
        LOG_WARNING(F("Set clock offset: Invalid value"));
        return false;
    }

    LOG_INFO(F("Setting clock offset to %d"), co);
    write_command(EMS_TYPE_IBASettings, 12, co, EMS_TYPE_IBASettings);

    return true;
}

// 0xA5 - Calibrate internal temperature
bool Thermostat::set_calinttemp(const char * value, const int8_t id) {
    int ct = 0;
    if (!Helpers::value2number(value, ct)) {
        LOG_WARNING(F("Cal internal temperature: Invalid value"));
        return false;
    }

    LOG_INFO(F("Calibrating internal temperature to %d.%d C"), ct / 10, ct < 0 ? -ct % 10 : ct % 10);
    write_command(EMS_TYPE_IBASettings, 2, ct, EMS_TYPE_IBASettings);

    return true;
}

// 0xA5 - Set the display settings
bool Thermostat::set_display(const char * value, const int8_t id) {
    int ds = 0;
    if (!Helpers::value2number(value, ds)) {
        LOG_WARNING(F("Set display: Invalid value"));
        return false;
    }

    LOG_INFO(F("Setting display to %d"), ds);
    write_command(EMS_TYPE_IBASettings, 0, ds, EMS_TYPE_IBASettings);

    return true;
}

bool Thermostat::set_remotetemp(const char * value, const int8_t id) {
    float f = 0;
    if (!Helpers::value2float(value, f)) {
        LOG_WARNING(F("Set remote temperature: Invalid value"));
        return false;
    }

    uint8_t                                     hc_num = (id == -1) ? AUTO_HEATING_CIRCUIT : id;
    std::shared_ptr<Thermostat::HeatingCircuit> hc     = heating_circuit(hc_num);
    if (hc == nullptr) {
        return false;
    }

    if (f > 100 || f < 0) {
        hc->remotetemp = EMS_VALUE_SHORT_NOTSET;
    } else {
        hc->remotetemp = (int16_t)(f * 10);
    }
    Roomctrl::set_remotetemp(hc->hc_num() - 1, hc->remotetemp);

    return true;
}

// 0xA5 - Set the building settings
bool Thermostat::set_building(const char * value, const int8_t id) {
    uint8_t bd = 0;
    if ((model() == EMS_DEVICE_FLAG_RC300) || (model() == EMS_DEVICE_FLAG_RC100)) {
        if (Helpers::value2enum(value, bd, FL_(enum_ibaBuildingType))) {
            LOG_INFO(F("Setting building to %s"), value);
            write_command(0x240, 9, bd + 1, 0x240);
            return true;
        }
    } else {
        if (Helpers::value2enum(value, bd, FL_(enum_ibaBuildingType))) {
            LOG_INFO(F("Setting building to %s"), value);
            write_command(EMS_TYPE_IBASettings, 6, bd, EMS_TYPE_IBASettings);
            return true;
        }
    }
    LOG_WARNING(F("Set building: Invalid value"));
    return false;
}

// 0xA5 - Set the building settings
bool Thermostat::set_damping(const char * value, const int8_t id) {
    bool dmp;
    if (Helpers::value2bool(value, dmp)) {
        LOG_INFO(F("Setting damping %s"), dmp ? "on" : "off");
        write_command(EMS_TYPE_IBASettings, 21, dmp ? 0xFF : 0, EMS_TYPE_IBASettings);
        return true;
    }
    LOG_WARNING(F("Set damping: Invalid value"));
    return false;
}

// 0xA5 Set the language settings
bool Thermostat::set_language(const char * value, const int8_t id) {
    uint8_t lg = 0;
    if (!Helpers::value2enum(value, lg, FL_(enum_ibaLanguage))) {
        LOG_WARNING(F("Set language: Invalid value"));
        return false;
    }

    LOG_INFO(F("Setting language to %d"), lg);
    write_command(EMS_TYPE_IBASettings, 1, lg, EMS_TYPE_IBASettings);

    return true;
}

// Set the control-mode for hc 0-off, 1-RC20, 2-RC3x
bool Thermostat::set_control(const char * value, const int8_t id) {
    uint8_t ctrl = 0;
    if (!Helpers::value2enum(value, ctrl, FL_(enum_control))) {
        LOG_WARNING(F("Set control: Invalid value"));
        return false;
    }

    uint8_t                                     hc_num = (id == -1) ? AUTO_HEATING_CIRCUIT : id;
    std::shared_ptr<Thermostat::HeatingCircuit> hc     = heating_circuit(hc_num);
    if (hc == nullptr) {
        return false;
    }

    LOG_INFO(F("Setting circuit-control for hc%d to %d"), hc_num, ctrl);
    write_command(set_typeids[hc->hc_num() - 1], 26, ctrl);

    return true;
}

// sets the thermostat ww working mode, where mode is a string, ems and ems+
bool Thermostat::set_wwmode(const char * value, const int8_t id) {
    uint8_t set = 0xFF;

    if ((model() == EMS_DEVICE_FLAG_RC300) || (model() == EMS_DEVICE_FLAG_RC100)) {
        if (!Helpers::value2enum(value, set, FL_(enum_wwMode))) {
            LOG_WARNING(F("Set warm water mode: Invalid mode"));
            return false;
        }
        LOG_INFO(F("Setting warm water mode to %s"), value);
        write_command(0x02F5, 2, set, 0x02F5);
    } else {
        if (!Helpers::value2enum(value, set, FL_(enum_wwMode2))) {
            LOG_WARNING(F("Set warm water mode: Invalid mode"));
            return false;
        }
        LOG_INFO(F("Setting warm water mode to %s"), value);
        write_command(EMS_TYPE_wwSettings, 2, set, EMS_TYPE_wwSettings);
    }
    return true;
}

// Set ww temperature, ems+
bool Thermostat::set_wwtemp(const char * value, const int8_t id) {
    int t = 0;
    if (!Helpers::value2number(value, t)) {
        LOG_WARNING(F("Set warm water high temperature: Invalid value"));
        return false;
    }
    LOG_INFO(F("Setting warm water high temperature to %d C"), t);
    write_command(0x031B, 0, t, 0x031B);
    return true;
}

// Set ww low temperature, ems+
bool Thermostat::set_wwtemplow(const char * value, const int8_t id) {
    int t = 0;
    if (!Helpers::value2number(value, t)) {
        LOG_WARNING(F("Set warm water low temperature: Invalid value"));
        return false;
    }
    LOG_INFO(F("Setting warm water low temperature to %d C"), t);
    write_command(0x031B, 1, t, 0x031B);
    return true;
}

// Set ww charge RC300, ems+
bool Thermostat::set_wwcharge(const char * value, const int8_t id) {
    bool b = false;
    if (!Helpers::value2bool(value, b)) {
        LOG_WARNING(F("Set warm water charge: Invalid value"));
        return false;
    }
    LOG_INFO(F("Setting warm water charge to %s"), b ? F_(on) : F_(off));
    write_command(0x02F5, 11, b, 0x02F5);
    return true;
}

// Set ww charge duration in steps of 15 min, ems+
bool Thermostat::set_wwchargeduration(const char * value, const int8_t id) {
    uint8_t t = 0xFF;
    if (!Helpers::value2enum(value, t, FL_(enum_wwChargeDuration))) {
        LOG_WARNING(F("Set warm water charge duration: Invalid value"));
        return false;
    }
    LOG_INFO(F("Setting warm water charge duration to %d min"), t * 15);
    write_command(0x2F5, 10, t, 0x02F5);
    return true;
}

// set ww prio
bool Thermostat::set_wwprio(const char * value, const int8_t id) {
    uint8_t                                     hc_num = (id == -1) ? AUTO_HEATING_CIRCUIT : id;
    std::shared_ptr<Thermostat::HeatingCircuit> hc     = heating_circuit(hc_num);
    if (hc == nullptr) {
        LOG_WARNING(F("Set wwprio: Heating Circuit %d not found or activated for device ID 0x%02X"), hc_num, device_id());
        return false;
    }
    bool b;
    if (!Helpers::value2bool(value, b)) {
        LOG_WARNING(F("Set wwprio: Invalid value"));
        return false;
    }

    LOG_INFO(F("Setting wwprio: %s"), b ? "on" : "off");
    write_command(set_typeids[hc->hc_num() - 1], 21, b ? 0xFF : 0x00, set_typeids[hc->hc_num() - 1]);

    return true;
}

// Set ww onetime RC300, ems+
bool Thermostat::set_wwonetime(const char * value, const int8_t id) {
    bool b = false;
    if (!Helpers::value2bool(value, b)) {
        LOG_WARNING(F("Set warm water onetime: Invalid value"));
        return false;
    }
    LOG_INFO(F("Setting warm water onetime to %s"), b ? F_(on) : F_(off));
    write_command(0x02F5, 11, b ? 0xFF : 0x00, 0x031D);
    return true;
}


// sets the thermostat ww circulation working mode, where mode is a string
bool Thermostat::set_wwcircmode(const char * value, const int8_t id) {
    uint8_t set = 0xFF;

    if ((model() == EMS_DEVICE_FLAG_RC300) || (model() == EMS_DEVICE_FLAG_RC100)) {
        if (!Helpers::value2enum(value, set, FL_(enum_wwCircMode))) {
            LOG_WARNING(F("Set warm water circulation mode: Invalid mode"));
            return false;
        }
        LOG_INFO(F("Setting warm water circulation mode to %s"), value);
        write_command(0x02F5, 3, set, 0x02F5);
        return true;
    }
    if (!Helpers::value2enum(value, set, FL_(enum_wwCircMode2))) {
        LOG_WARNING(F("Set warm water circulation mode: Invalid mode"));
        return false;
    }
    LOG_INFO(F("Setting warm water circulation mode to %s"), value);
    write_command(EMS_TYPE_wwSettings, 3, set, EMS_TYPE_wwSettings);
    return true;
}

bool Thermostat::set_wwDisinfect(const char * value, const int8_t id) {
    bool b = false;
    if (!Helpers::value2bool(value, b)) {
        LOG_WARNING(F("Set warm water disinfect: Invalid value"));
        return false;
    }
    LOG_INFO(F("Setting warm water disinfect to %s"), b ? F_(on) : F_(off));
    write_command(0x37, 4, b ? 0xFF : 0x00, 0x37);
    return true;
}
bool Thermostat::set_wwDisinfectDay(const char * value, const int8_t id) {
    uint8_t set = 0xFF;
    if (!Helpers::value2enum(value, set, FL_(enum_dayOfWeek))) {
        LOG_WARNING(F("Set warm water disinfection day: Invalid day"));
        return false;
    }
    LOG_INFO(F("Setting warm water disinfection day to %s"), value);
    write_command(0x37, 5, set, 0x37);
    return true;
}

bool Thermostat::set_wwDisinfectHour(const char * value, const int8_t id) {
    int set;
    if (!Helpers::value2number(value, set)) {
        LOG_WARNING(F("Set warm water disinfection hour: Invalid"));
        return false;
    }
    if (set < 0 || set > 23) {
        LOG_WARNING(F("Set warm water disinfection hour: Invalid"));
        return false;
    }
    LOG_INFO(F("Setting warm water disinfection hour to %s"), value);
    write_command(0x37, 6, set, 0x37);
    return true;
}

bool Thermostat::set_wwMaxTemp(const char * value, const int8_t id) {
    int t = 0;
    if (!Helpers::value2number(value, t)) {
        LOG_WARNING(F("Set warm water max temperature: Invalid value"));
        return false;
    }
    if (t < 0 || t > 90) {
        LOG_WARNING(F("Set warm water max temperature: Invalid value"));
        return false;
    }
    LOG_INFO(F("Setting warm water max temperature to %d C"), t);
    write_command(0x37, 8, t, 0x37);
    return true;
}

bool Thermostat::set_wwOneTimeKey(const char * value, const int8_t id) {
    bool b = false;
    if (!Helpers::value2bool(value, b)) {
        LOG_WARNING(F("Set warm water loading one time key: Invalid value"));
        return false;
    }
    LOG_INFO(F("Setting warm water loading one time key to %s"), value);
    write_command(0x37, 9, b ? 0xFF : 0x00, 0x37);
    return true;
}

bool Thermostat::set_wwProgMode(const char * value, const int8_t id) {
    uint8_t set = 0xFF;
    if (!Helpers::value2enum(value, set, FL_(enum_wwProgMode))) {
        LOG_WARNING(F("Set warm water program mode: Invalid mode"));
        return false;
    }
    LOG_INFO(F("Setting warm water program mode to %s"), value);
    write_command(0x37, 0, set ? 0xFF : 0x00, 0x37);
    return true;
}

bool Thermostat::set_wwCircProg(const char * value, const int8_t id) {
    uint8_t set = 0xFF;
    if (!Helpers::value2enum(value, set, FL_(enum_wwProgMode))) {
        LOG_WARNING(F("Set warm water circulation program mode: Invalid mode"));
        return false;
    }
    LOG_INFO(F("Setting warm water circulation program mode to %s"), value);
    write_command(0x37, 1, set ? 0xFF : 0x00, 0x37);
    return true;
}

// set the holiday as string dd.mm.yyyy-dd.mm.yyyy
bool Thermostat::set_holiday(const char * value, const int8_t id) {
    uint8_t                                     hc_num = (id == -1) ? AUTO_HEATING_CIRCUIT : id;
    std::shared_ptr<Thermostat::HeatingCircuit> hc     = heating_circuit(hc_num);
    if (hc == nullptr) {
        LOG_WARNING(F("Set holiday: Heating Circuit %d not found or activated for device ID 0x%02X"), hc_num, device_id());
        return false;
    }
    if (value == nullptr || value[0] == '-') {
        read_command(timer_typeids[hc->hc_num() - 1], 87, 6);
        return true;
    } else if (strlen(value) == 1 && value[0] == '+') {
        read_command(timer_typeids[hc->hc_num() - 1], 93, 6);
        return true;
    } else if (strlen(value) != 21) {
        LOG_WARNING(F("Set holiday: Invalid value"));
        return false;
    }

    uint8_t data[6];
    data[0] = (value[0] - '0') * 10 + (value[1] - '0');
    data[1] = (value[3] - '0') * 10 + (value[4] - '0');
    data[2] = (value[7] - '0') * 100 + (value[8] - '0') * 10 + (value[9] - '0');
    data[3] = (value[11] - '0') * 10 + (value[12] - '0');
    data[4] = (value[14] - '0') * 10 + (value[15] - '0');
    data[5] = (value[18] - '0') * 100 + (value[19] - '0') * 10 + (value[20] - '0');

    if (value[10] == '-') {
        LOG_INFO(F("Setting holiday away from home for hc %d"), hc->hc_num());
        write_command(timer_typeids[hc->hc_num() - 1], 87, data, 6, 0);
    } else if (value[10] == '+') {
        LOG_INFO(F("Setting holiday at home for hc %d"), hc->hc_num());
        write_command(timer_typeids[hc->hc_num() - 1], 93, data, 6, 0);
    } else {
        LOG_WARNING(F("Set holiday: Invalid value"));
        return false;
    }

    return true;
}

// set pause in hours
bool Thermostat::set_pause(const char * value, const int8_t id) {
    uint8_t                                     hc_num = (id == -1) ? AUTO_HEATING_CIRCUIT : id;
    std::shared_ptr<Thermostat::HeatingCircuit> hc     = heating_circuit(hc_num);
    if (hc == nullptr) {
        LOG_WARNING(F("Set pause: Heating Circuit %d not found or activated for device ID 0x%02X"), hc_num, device_id());
        return false;
    }
    if (value == nullptr) {
        read_command(timer_typeids[hc->hc_num() - 1], 85, 1);
        return true;
    }
    int hrs = 0;
    if (!Helpers::value2number(value, hrs)) {
        LOG_WARNING(F("Set pause: Invalid value"));
        return false;
    }

    LOG_INFO(F("Setting pause: %d hours, hc: %d"), hrs, hc->hc_num());
    write_command(timer_typeids[hc->hc_num() - 1], 85, hrs);

    return true;
}

// set partymode in hours
bool Thermostat::set_party(const char * value, const int8_t id) {
    uint8_t                                     hc_num = (id == -1) ? AUTO_HEATING_CIRCUIT : id;
    std::shared_ptr<Thermostat::HeatingCircuit> hc     = heating_circuit(hc_num);
    if (hc == nullptr) {
        LOG_WARNING(F("Set party: Heating Circuit %d not found or activated for device ID 0x%02X"), hc_num, device_id());
        return false;
    }
    if (value == nullptr) {
        read_command(timer_typeids[hc->hc_num() - 1], 85, 1);
        return true;
    }
    int hrs = 0;
    if (!Helpers::value2number(value, hrs)) {
        LOG_WARNING(F("Set party: Invalid value"));
        return false;
    }
    LOG_INFO(F("Setting party: %d hours, hc: %d"), hrs, hc->hc_num());
    write_command(timer_typeids[hc->hc_num() - 1], 86, hrs);

    return true;
}

// set date&time as string hh:mm:ss-dd.mm.yyyy-dw-dst or "NTP" for setting to internet-time
// dw - day of week (0..6), dst- summertime (0/1)
// id is ignored
bool Thermostat::set_datetime(const char * value, const int8_t id) {
    std::string dt(30, '\0');
    if (!Helpers::value2string(value, dt)) {
        LOG_WARNING(F("Set date: Invalid value"));
        return false;
    }

    uint8_t data[9];
    if (dt == "ntp") {
        time_t now = time(nullptr);
        tm *   tm_ = localtime(&now);
        if (tm_->tm_year < 110) { // no NTP time
            LOG_WARNING(F("No NTP time. Cannot set RCtime"));
            return false;
        }

        data[0] = tm_->tm_year - 100; // Bosch counts from 2000
        data[1] = tm_->tm_mon + 1;
        data[2] = tm_->tm_hour;
        data[3] = tm_->tm_mday;
        data[4] = tm_->tm_min;
        data[5] = tm_->tm_sec;
        data[6] = (tm_->tm_wday + 6) % 7; // Bosch counts from Mo, time from Su
        data[7] = tm_->tm_isdst + 2;      // set DST and flag for ext. clock
        char time_string[25];
        strftime(time_string, 25, "%FT%T%z", tm_);
        LOG_INFO(F("Date and time: %s"), time_string);
    } else {
        data[0] = (dt[16] - '0') * 100 + (dt[17] - '0') * 10 + (dt[18] - '0'); // year
        data[1] = (dt[12] - '0') * 10 + (dt[13] - '0');                        // month
        data[2] = (dt[0] - '0') * 10 + (dt[1] - '0');                          // hour
        data[3] = (dt[9] - '0') * 10 + (dt[10] - '0');                         // day
        data[4] = (dt[3] - '0') * 10 + (dt[4] - '0');                          // min
        data[5] = (dt[6] - '0') * 10 + (dt[7] - '0');                          // sec
        data[6] = (dt[20] - '0');                                              // day of week
        data[7] = (dt[22] - '0') + 2;                                          // DST and flag
        LOG_INFO(F("Date and time: %02d.%02d.2%03d-%02d:%02d:%02d"), data[3], data[1], data[0], data[2], data[4], data[5]);
    }

    LOG_INFO(F("Setting date and time"));
    write_command(EMS_TYPE_time, 0, data, 8, EMS_TYPE_time);

    return true;
}

// sets the thermostat working mode, where mode is a string
// converts string mode to HeatingCircuit::Mode
bool Thermostat::set_mode(const char * value, const int8_t id) {
    std::string mode(20, '\0');
    if (strlen(value) >= 20) {
        LOG_WARNING(F("Set mode: Invalid mode"));
        return false;
    }

    if (value[0] >= '0' && value[0] <= '9') {
        uint8_t num = value[0] - '0';
        switch (model()) {
        case EMSdevice::EMS_DEVICE_FLAG_RC20:
        case EMSdevice::EMS_DEVICE_FLAG_RC20_N:
            mode = uuid::read_flash_string(FL_(enum_mode2)[num]);
            break;
        case EMSdevice::EMS_DEVICE_FLAG_RC30:
        case EMSdevice::EMS_DEVICE_FLAG_RC35:
        case EMSdevice::EMS_DEVICE_FLAG_RC30_N:
            mode = uuid::read_flash_string(FL_(enum_mode3)[num]);
            break;
        case EMSdevice::EMS_DEVICE_FLAG_RC300:
        case EMSdevice::EMS_DEVICE_FLAG_RC100:
            mode = uuid::read_flash_string(FL_(enum_mode)[num]);
            break;
        case EMSdevice::EMS_DEVICE_FLAG_JUNKERS:
            mode = uuid::read_flash_string(FL_(enum_mode4)[num]);
            break;
        case EMSdevice::EMS_DEVICE_FLAG_CRF:
            mode = uuid::read_flash_string(FL_(enum_mode5)[num]);
            break;
        default:
            LOG_WARNING(F("Set mode: Invalid mode"));
            return false;
        }
    } else if (!Helpers::value2string(value, mode)) {
        LOG_WARNING(F("Set mode: Invalid mode"));
        return false;
    }

    uint8_t hc_num = (id == -1) ? AUTO_HEATING_CIRCUIT : id;
    if (mode_tostring(HeatingCircuit::Mode::OFF) == mode) {
        return set_mode_n(HeatingCircuit::Mode::OFF, hc_num);
    }
    if (mode_tostring(HeatingCircuit::Mode::MANUAL) == mode) {
        return set_mode_n(HeatingCircuit::Mode::MANUAL, hc_num);
    }
    if (mode_tostring(HeatingCircuit::Mode::AUTO) == mode) {
        return set_mode_n(HeatingCircuit::Mode::AUTO, hc_num);
    }
    if (mode_tostring(HeatingCircuit::Mode::DAY) == mode) {
        return set_mode_n(HeatingCircuit::Mode::DAY, hc_num);
    }
    if (mode_tostring(HeatingCircuit::Mode::NIGHT) == mode) {
        return set_mode_n(HeatingCircuit::Mode::NIGHT, hc_num);
    }
    if (mode_tostring(HeatingCircuit::Mode::HEAT) == mode) {
        return set_mode_n(HeatingCircuit::Mode::HEAT, hc_num);
    }
    if (mode_tostring(HeatingCircuit::Mode::NOFROST) == mode) {
        return set_mode_n(HeatingCircuit::Mode::NOFROST, hc_num);
    }
    if (mode_tostring(HeatingCircuit::Mode::ECO) == mode) {
        return set_mode_n(HeatingCircuit::Mode::ECO, hc_num);
    }
    if (mode_tostring(HeatingCircuit::Mode::HOLIDAY) == mode) {
        return set_mode_n(HeatingCircuit::Mode::HOLIDAY, hc_num);
    }
    if (mode_tostring(HeatingCircuit::Mode::COMFORT) == mode) {
        return set_mode_n(HeatingCircuit::Mode::COMFORT, hc_num);
    }

    LOG_WARNING(F("Set mode: Invalid mode %s"), value);
    return false;
}

// Set the thermostat working mode
// mode is HeatingCircuit::Mode
bool Thermostat::set_mode_n(const uint8_t mode, const uint8_t hc_num) {
    // get hc based on number
    std::shared_ptr<Thermostat::HeatingCircuit> hc = heating_circuit(hc_num);
    if (hc == nullptr) {
        LOG_WARNING(F("Set mode: Heating Circuit %d not found or activated"), hc_num);
        return false;
    }

    uint8_t  set_mode_value, offset;
    uint16_t validate_typeid = 0;
    uint8_t  hc_p            = hc->hc_num() - 1;

    // set the value to send via EMS depending on the mode type
    switch (mode) {
    case HeatingCircuit::Mode::NIGHT:
    case HeatingCircuit::Mode::OFF:
        set_mode_value = 0;
        break;

    case HeatingCircuit::Mode::DAY:
    case HeatingCircuit::Mode::HEAT:
    case HeatingCircuit::Mode::MANUAL:
    case HeatingCircuit::Mode::NOFROST:
        set_mode_value = 1;
        break;

    default:
    case HeatingCircuit::Mode::AUTO:
    case HeatingCircuit::Mode::ECO:
        set_mode_value = 2;
        break;
    }

    switch (model()) {
    case EMSdevice::EMS_DEVICE_FLAG_RC20:
        offset          = EMS_OFFSET_RC20Set_mode;
        validate_typeid = set_typeids[hc_p];
        break;
    case EMSdevice::EMS_DEVICE_FLAG_RC20_N: // ES72
        offset          = EMS_OFFSET_RC20_2_Set_mode;
        validate_typeid = set_typeids[hc_p];
        break;
    case EMSdevice::EMS_DEVICE_FLAG_RC30:
        offset          = EMS_OFFSET_RC30Set_mode;
        validate_typeid = set_typeids[hc_p];
        break;
    case EMSdevice::EMS_DEVICE_FLAG_RC35:
    case EMSdevice::EMS_DEVICE_FLAG_RC30_N:
        offset          = EMS_OFFSET_RC35Set_mode;
        validate_typeid = set_typeids[hc_p];
        break;
    case EMSdevice::EMS_DEVICE_FLAG_RC300:
    case EMSdevice::EMS_DEVICE_FLAG_RC100:
        offset          = EMS_OFFSET_RCPLUSSet_mode;
        validate_typeid = monitor_typeids[hc_p];
        if (mode == HeatingCircuit::Mode::AUTO) {
            set_mode_value = 0xFF; // special value for auto
        } else {
            set_mode_value = 0; // everything else, like manual/day etc..
        }
        break;
    case EMSdevice::EMS_DEVICE_FLAG_JUNKERS:
        if (has_flags(EMS_DEVICE_FLAG_JUNKERS_OLD)) {
            offset = EMS_OFFSET_JunkersSetMessage2_set_mode;
        } else {
            offset = EMS_OFFSET_JunkersSetMessage_set_mode;
        }
        validate_typeid = set_typeids[hc_p];
        if (mode == HeatingCircuit::Mode::NOFROST) {
            set_mode_value = 0x01;
        } else if (mode == HeatingCircuit::Mode::ECO || (mode == HeatingCircuit::Mode::NIGHT)) {
            set_mode_value = 0x02;
        } else if ((mode == HeatingCircuit::Mode::DAY) || (mode == HeatingCircuit::Mode::HEAT)) {
            set_mode_value = 0x03; // comfort
        } else if (mode == HeatingCircuit::Mode::AUTO) {
            set_mode_value = 0x04;
        }
        break;
    default:
        offset = 0;
        break;
    }

    LOG_INFO(F("Setting thermostat mode to %s for heating circuit %d"), mode_tostring(mode).c_str(), hc->hc_num());

    // add the write command to the Tx queue
    // post validate is the corresponding monitor or set type IDs as they can differ per model
    write_command(set_typeids[hc->hc_num() - 1], offset, set_mode_value, validate_typeid);

    return true;
}

// sets the thermostat summermode for RC300
bool Thermostat::set_summermode(const char * value, const int8_t id) {
    uint8_t                                     hc_num = (id == -1) ? AUTO_HEATING_CIRCUIT : id;
    std::shared_ptr<Thermostat::HeatingCircuit> hc     = heating_circuit(hc_num);
    if (hc == nullptr) {
        LOG_WARNING(F("Setting summer mode: Heating Circuit %d not found or activated"), hc_num);
        return false;
    }
    uint8_t set = 0xFF;
    if (!Helpers::value2enum(value, set, FL_(enum_summermode))) {
        LOG_WARNING(F("Setting summer mode: Invalid mode"));
        return false;
    }
    LOG_INFO(F("Setting summer mode to %s for heating circuit %d"), value, hc->hc_num());
    if (is_fetch(summer2_typeids[hc->hc_num() - 1])) {
        write_command(summer2_typeids[hc->hc_num() - 1], 0, set, summer2_typeids[hc->hc_num() - 1]);
    } else {
        write_command(summer_typeids[hc->hc_num() - 1], 7, set, summer_typeids[hc->hc_num() - 1]);
    }
    return true;
}

// Set fastheatupfactor, ems+
bool Thermostat::set_fastheatupfactor(const char * value, const int8_t id) {
    uint8_t                                     hc_num = (id == -1) ? AUTO_HEATING_CIRCUIT : id;
    std::shared_ptr<Thermostat::HeatingCircuit> hc     = heating_circuit(hc_num);
    if (hc == nullptr) {
        LOG_WARNING(F("Setfast heatup factor: Heating Circuit %d not found or activated for device ID 0x%02X"), hc_num, device_id());
        return false;
    }
    int set = 0;
    if (!Helpers::value2number(value, set)) {
        LOG_WARNING(F("Set fast heatup factor: Invalid value"));
        return false;
    }
    LOG_INFO(F("Setting fast heatup factor to %d"), set);
    write_command(summer_typeids[hc->hc_num() - 1], 10, set, summer_typeids[hc->hc_num() - 1]);
    return true;
}

// sets the thermostat reducemode for RC35
bool Thermostat::set_reducemode(const char * value, const int8_t id) {
    uint8_t                                     hc_num = (id == -1) ? AUTO_HEATING_CIRCUIT : id;
    std::shared_ptr<Thermostat::HeatingCircuit> hc     = heating_circuit(hc_num);
    if (hc == nullptr) {
        LOG_WARNING(F("Setting reduce mode: Heating Circuit %d not found or activated"), hc_num);
        return false;
    }
    uint8_t set = 0xFF;
    if (!Helpers::value2enum(value, set, FL_(enum_reducemode))) {
        LOG_WARNING(F("Setting reduce mode: Invalid mode"));
        return false;
    }
    LOG_INFO(F("Setting reduce mode to %s for heating circuit %d"), value, hc->hc_num());
    write_command(set_typeids[hc->hc_num() - 1], 25, set, set_typeids[hc->hc_num() - 1]);
    return true;
}

// sets the thermostat heatingtype for RC35, RC300
bool Thermostat::set_heatingtype(const char * value, const int8_t id) {
    uint8_t                                     hc_num = (id == -1) ? AUTO_HEATING_CIRCUIT : id;
    std::shared_ptr<Thermostat::HeatingCircuit> hc     = heating_circuit(hc_num);
    if (hc == nullptr) {
        LOG_WARNING(F("Setting heating type: Heating Circuit %d not found or activated"), hc_num);
        return false;
    }
    uint8_t set = 0xFF;
    if (Helpers::value2enum(value, set, FL_(enum_heatingtype))) {
        LOG_INFO(F("Setting heating type to %d for heating circuit %d"), set, hc->hc_num());
        if (model() == EMS_DEVICE_FLAG_RC35 || model() == EMS_DEVICE_FLAG_RC30_N) {
            write_command(set_typeids[hc->hc_num() - 1], 0, set, set_typeids[hc->hc_num() - 1]);
        } else {
            write_command(curve_typeids[hc->hc_num() - 1], 1, set, curve_typeids[hc->hc_num() - 1]);
            return true;
        }
    }
    LOG_WARNING(F("Setting heating type: Invalid mode"));
    return false;
}

// sets the thermostat controlmode for RC35, RC300
bool Thermostat::set_controlmode(const char * value, const int8_t id) {
    uint8_t                                     hc_num = (id == -1) ? AUTO_HEATING_CIRCUIT : id;
    std::shared_ptr<Thermostat::HeatingCircuit> hc     = heating_circuit(hc_num);
    if (hc == nullptr) {
        LOG_WARNING(F("Setting control mode: Heating Circuit %d not found or activated"), hc_num);
        return false;
    }
    uint8_t set = 0xFF;
    if (model() == EMS_DEVICE_FLAG_RC300 || model() == EMS_DEVICE_FLAG_RC100) {
        if (Helpers::value2enum(value, set, FL_(enum_controlmode))) {
            LOG_INFO(F("Setting control mode to %d for heating circuit %d"), set, hc->hc_num());
            write_command(curve_typeids[hc->hc_num() - 1], 0, set, curve_typeids[hc->hc_num() - 1]);
            return true;
        }
    } else if (model() == EMS_DEVICE_FLAG_RC35 || model() == EMS_DEVICE_FLAG_RC30_N) {
        if (Helpers::value2enum(value, set, FL_(enum_controlmode2))) {
            LOG_INFO(F("Setting control mode to %d for heating circuit %d"), set, hc->hc_num());
            write_command(set_typeids[hc->hc_num() - 1], 33, set, set_typeids[hc->hc_num() - 1]);
            return true;
        }
    }
    LOG_WARNING(F("Setting control mode: Invalid mode"));
    return false;
}

// sets a single switchtime in the thermostat program for RC35
// format "01:0,1,15:30" Number, day, on, time
bool Thermostat::set_switchtime(const char * value, const int8_t id) {
    uint8_t                                     hc_num = (id == -1) ? AUTO_HEATING_CIRCUIT : id;
    std::shared_ptr<Thermostat::HeatingCircuit> hc     = heating_circuit(hc_num);
    if (hc == nullptr) {
        LOG_WARNING(F("Setting switchtime: Heating Circuit %d not found or activated"), hc_num);
        return false;
    }
    if (value == nullptr) {
        return false;
    } else if (strlen(value) == 2) { // query point 01?
        uint8_t no = (value[0] - '0') * 10 + (value[1] - '0');
        if (no < 42) {
            read_command(timer_typeids[hc->hc_num() - 1], 2 * no, 2);
            return true;
        }
        return false;

    } else if (strlen(value) != 12) {
        LOG_WARNING(F("Setting switchtime: Invalid data"));
        return false;
    }
    uint8_t no      = (value[0] - '0') * 10 + (value[1] - '0');
    uint8_t day     = value[3] - '0';
    uint8_t on      = value[5] - '0';
    uint8_t time    = 6 * ((value[7] - '0') * 10 + (value[8] - '0')) + (value[10] - '0');
    uint8_t data[2] = {0xE7, 0x90}; // unset switchtime

    if (day != 7 && on != 7) {
        data[0] = (day << 5) + on;
        data[1] = time;
    }

    if (no > 41 || day > 7 || (on > 1 && on != 7) || time > 0x90) {
        LOG_WARNING(F("Setting switchtime: Invalid data"));
        return false;
    }

    if ((model() == EMS_DEVICE_FLAG_RC35 || model() == EMS_DEVICE_FLAG_RC30_N)) {
        write_command(timer_typeids[hc->hc_num() - 1], no * 2, (uint8_t *)&data, 2, timer_typeids[hc->hc_num() - 1]);
    } else {
        LOG_WARNING(F("Setting switchtime: thermostat not supported"));
        return false;
    }
    if (data[0] == 0xE7) {
        LOG_INFO(F("Setting switchtime no %d for heating circuit %d undefined"), no, hc->hc_num());
    } else {
        LOG_INFO(F("Setting switchtime no %d for heating circuit %d to day %d, %s, %02d:%d0"), no, hc->hc_num(), day, (on == 1) ? "on" : "off", time / 6, time % 6);
    }
    return true;
}

// sets the thermostat program for RC35 and RC20
bool Thermostat::set_program(const char * value, const int8_t id) {
    uint8_t                                     hc_num = (id == -1) ? AUTO_HEATING_CIRCUIT : id;
    std::shared_ptr<Thermostat::HeatingCircuit> hc     = heating_circuit(hc_num);
    if (hc == nullptr) {
        LOG_WARNING(F("Setting program: Heating Circuit %d not found or activated"), hc_num);
        return false;
    }

    int set = 0xFF;
    if (!Helpers::value2number(value, set)) {
        LOG_WARNING(F("Setting program: Invalid number"));
        return false;
    }

    if (model() == EMS_DEVICE_FLAG_RC20_N && set > 0 && set < 10) {
        write_command(set_typeids[hc->hc_num() - 1], 11, set, set_typeids[hc->hc_num() - 1]);
    } else if ((model() == EMS_DEVICE_FLAG_RC35 || model() == EMS_DEVICE_FLAG_RC30_N) && set < 11) {
        write_command(timer_typeids[hc->hc_num() - 1], 84, set, timer_typeids[hc->hc_num() - 1]);
    } else if ((model() == EMS_DEVICE_FLAG_RC300 || model() == EMS_DEVICE_FLAG_RC100) && (set == 1 || set == 2)) {
        write_command(set_typeids[hc->hc_num() - 1], 11, set, set_typeids[hc->hc_num() - 1]);
    } else {
        LOG_WARNING(F("Setting program: Invalid number"));
        return false;
    }
    LOG_INFO(F("Setting program to %d for heating circuit %d"), set, hc->hc_num());
    return true;
}

// Set the temperature of the thermostat
// the id passed into this function is the heating circuit number
bool Thermostat::set_temperature(const float temperature, const uint8_t mode, const uint8_t hc_num) {
    // get hc based on number
    std::shared_ptr<Thermostat::HeatingCircuit> hc = heating_circuit(hc_num);
    if (hc == nullptr) {
        LOG_WARNING(F("Set temperature: Heating Circuit %d not found or activated for device ID 0x%02X"), hc_num, device_id());
        return false;
    }

    uint8_t  model           = this->model();
    int8_t   offset          = -1; // we use -1 to check if there is a value
    uint8_t  factor          = 2;  // some temperatures only use 1
    uint16_t validate_typeid = monitor_typeids[hc->hc_num() - 1];
    uint16_t set_typeid      = set_typeids[hc->hc_num() - 1];

    if (model == EMS_DEVICE_FLAG_RC10) {
        offset = EMS_OFFSET_RC10Set_temp;

    } else if (model == EMS_DEVICE_FLAG_RC20) {
        offset = EMS_OFFSET_RC20Set_temp;

    } else if (model == EMS_DEVICE_FLAG_RC30) {
        offset = EMS_OFFSET_RC30Set_temp;

    } else if ((model == EMS_DEVICE_FLAG_RC300) || (model == EMS_DEVICE_FLAG_RC100)) {
        validate_typeid = set_typeids[hc->hc_num() - 1];
        switch (mode) {
        case HeatingCircuit::Mode::SUMMER:
            if (is_fetch(summer2_typeids[hc->hc_num() - 1])) {
                offset     = 0x01;
                set_typeid = summer2_typeids[hc->hc_num() - 1];
            } else {
                offset     = 0x06;
                set_typeid = summer_typeids[hc->hc_num() - 1];
            }
            validate_typeid = set_typeid;
            factor          = 1;
            break;
        case HeatingCircuit::Mode::MANUAL:
            offset = 0x0A; // manual offset
            break;
        case HeatingCircuit::Mode::TEMPAUTO:
            offset = 0x08; // manual offset
            if (temperature == -1) {
                factor = 0xFF; // use factor as value
            }
            break;
        case HeatingCircuit::Mode::COMFORT:
            offset = 0x02; // comfort offset level 2
            break;
        case HeatingCircuit::Mode::ECO:
            offset = 0x04; // eco offset
            break;
        case HeatingCircuit::Mode::OFFSET:
            offset          = 2;
            set_typeid      = summer_typeids[hc->hc_num() - 1];
            validate_typeid = set_typeid;
            factor          = 1;
            break;
        case HeatingCircuit::Mode::DESIGN:
            set_typeid      = summer_typeids[hc->hc_num() - 1];
            validate_typeid = set_typeid;
            if (hc->heatingtype == 3) {
                offset = 5;
            } else {
                offset = 4;
            }
            factor = 1;
            break;
        case HeatingCircuit::Mode::MINFLOW:
            set_typeid      = summer_typeids[hc->hc_num() - 1];
            validate_typeid = set_typeid;
            offset          = 8;
            factor          = 1;
            break;
        case HeatingCircuit::Mode::MAXFLOW:
            set_typeid      = curve_typeids[hc->hc_num() - 1];
            validate_typeid = set_typeid;
            if (hc->heatingtype == 3) {
                offset = 7;
            } else {
                offset = 8;
            }
            factor = 1;
            break;
        case HeatingCircuit::Mode::NOFROST:
            set_typeid      = curve_typeids[hc->hc_num() - 1];
            validate_typeid = set_typeid;
            offset          = 6;
            factor          = 1;
            break;
        case HeatingCircuit::Mode::ROOMINFLUENCE:
            set_typeid      = summer_typeids[hc->hc_num() - 1];
            validate_typeid = set_typeid;
            offset          = 0;
            factor          = 1;
            break;
        default:
        case HeatingCircuit::Mode::AUTO:
            uint8_t mode_ = hc->get_mode();
            if (mode_ == HeatingCircuit::Mode::MANUAL) {
                offset = 0x0A; // manual offset
            } else {
                offset = 0x08; // auto offset
                // special case to reactivate auto temperature, see #737, #746
                if (temperature == -1) {
                    factor = 0xFF; // use factor as value
                }
            }
            validate_typeid = monitor_typeids[hc->hc_num() - 1]; // get setpoint roomtemp back
            break;
        }

    } else if (model == EMS_DEVICE_FLAG_RC20_N) {
        switch (mode) {
        case HeatingCircuit::Mode::NIGHT: // change the night temp
            offset = EMS_OFFSET_RC20_2_Set_temp_night;
            break;
        case HeatingCircuit::Mode::DAY: // change the day temp
            offset = EMS_OFFSET_RC20_2_Set_temp_day;
            break;
        default:
        case HeatingCircuit::Mode::AUTO: // automatic selection, if no type is defined, we use the standard code
            uint8_t modetype = hc->get_mode_type();
            offset           = (modetype == HeatingCircuit::Mode::NIGHT) ? EMS_OFFSET_RC20_2_Set_temp_night : EMS_OFFSET_RC20_2_Set_temp_day;
            break;
        }

    } else if ((model == EMS_DEVICE_FLAG_RC35) || (model == EMS_DEVICE_FLAG_RC30_N)) {
        validate_typeid = set_typeids[hc->hc_num() - 1];
        switch (mode) {
        case HeatingCircuit::Mode::NIGHT: // change the night temp
            offset = EMS_OFFSET_RC35Set_temp_night;
            break;
        case HeatingCircuit::Mode::DAY: // change the day temp
            offset = EMS_OFFSET_RC35Set_temp_day;
            break;
        case HeatingCircuit::Mode::HOLIDAY: // change the holiday temp
            offset = EMS_OFFSET_RC35Set_temp_holiday;
            break;
        case HeatingCircuit::Mode::OFFSET: // change the offset temp
            offset = EMS_OFFSET_RC35Set_temp_offset;
            break;
        case HeatingCircuit::Mode::FLOWOFFSET: // change the offset of flowtemp
            offset = EMS_OFFSET_RC35Set_temp_flowoffset;
            factor = 1;
            break;
        case HeatingCircuit::Mode::DESIGN:
            if (hc->heatingtype == 3) {
                offset = EMS_OFFSET_RC35Set_temp_design_floor;
            } else {
                offset = EMS_OFFSET_RC35Set_temp_design;
            }
            factor = 1;
            break;
        case HeatingCircuit::Mode::SUMMER:
            offset = EMS_OFFSET_RC35Set_temp_summer;
            factor = 1;
            break;
        case HeatingCircuit::Mode::NOFROST:
            offset = EMS_OFFSET_RC35Set_temp_nofrost;
            factor = 1;
            break;
        case HeatingCircuit::Mode::ROOMINFLUENCE:
            offset = 4;
            factor = 1;
            break;
        case HeatingCircuit::Mode::NOREDUCE:
            offset = EMS_OFFSET_RC35Set_noreducetemp;
            factor = 1;
            break;
        case HeatingCircuit::Mode::TEMPAUTO:
            offset = EMS_OFFSET_RC35Set_seltemp;
            break;
        case HeatingCircuit::Mode::MINFLOW:
            offset = 16;
            factor = 1;
            break;
        case HeatingCircuit::Mode::MAXFLOW:
            if (hc->heatingtype == 3) {
                offset = 35;
            } else {
                offset = 15;
            }
            factor = 1;
            break;
        default:
        case HeatingCircuit::Mode::AUTO:                         // automatic selection, if no type is defined, we use the standard code
            validate_typeid = monitor_typeids[hc->hc_num() - 1]; //get setpoint roomtemp back
            if (model == EMS_DEVICE_FLAG_RC35) {
                uint8_t mode_ = hc->get_mode();
                if (mode_ == HeatingCircuit::Mode::NIGHT) {
                    offset = EMS_OFFSET_RC35Set_temp_night;
                } else if (mode_ == HeatingCircuit::Mode::DAY) {
                    offset = EMS_OFFSET_RC35Set_temp_day;
                } else {
                    offset = EMS_OFFSET_RC35Set_seltemp; // https://github.com/emsesp/EMS-ESP/issues/310
                }
            } else {
                uint8_t modetype = hc->get_mode_type();
                offset           = (modetype == HeatingCircuit::Mode::NIGHT) ? EMS_OFFSET_RC35Set_temp_night : EMS_OFFSET_RC35Set_temp_day;
            }
            break;
        }

    } else if (model == EMS_DEVICE_FLAG_JUNKERS) {
        // figure out if we have older or new thermostats, Heating Circuits on 0x65 or 0x79
        // see https://github.com/emsesp/EMS-ESP/issues/335#issuecomment-593324716)
        bool old_junkers = (has_flags(EMS_DEVICE_FLAG_JUNKERS_OLD));
        if (!old_junkers) {
            switch (mode) {
            case HeatingCircuit::Mode::NOFROST:
                offset = EMS_OFFSET_JunkersSetMessage_no_frost_temp;
                break;
            case HeatingCircuit::Mode::NIGHT:
            case HeatingCircuit::Mode::ECO:
                offset = EMS_OFFSET_JunkersSetMessage_night_temp;
                break;
            case HeatingCircuit::Mode::HEAT:
            case HeatingCircuit::Mode::DAY:
                offset = EMS_OFFSET_JunkersSetMessage_day_temp;
                break;
            default:
            case HeatingCircuit::Mode::AUTO: // automatic selection, if no type is defined, we use the standard code
                uint8_t modetype = hc->get_mode_type();
                if (modetype == HeatingCircuit::Mode::NIGHT || modetype == HeatingCircuit::Mode::ECO) {
                    offset = EMS_OFFSET_JunkersSetMessage_night_temp;
                } else if (modetype == HeatingCircuit::Mode::DAY || modetype == HeatingCircuit::Mode::HEAT) {
                    offset = EMS_OFFSET_JunkersSetMessage_day_temp;
                } else {
                    offset = EMS_OFFSET_JunkersSetMessage_no_frost_temp;
                }
                break;
            }

        } else {
            // older, like the FR100
            switch (mode) {
            case HeatingCircuit::Mode::NOFROST:
                offset = EMS_OFFSET_JunkersSetMessage2_no_frost_temp;
                break;
            case HeatingCircuit::Mode::ECO:
            case HeatingCircuit::Mode::NIGHT:
                offset = EMS_OFFSET_JunkersSetMessage2_eco_temp;
                break;
            case HeatingCircuit::Mode::HEAT:
            case HeatingCircuit::Mode::DAY:
                offset = EMS_OFFSET_JunkersSetMessage2_heat_temp;
                break;
            default:
            case HeatingCircuit::Mode::AUTO: // automatic selection, if no type is defined, we use the standard code
                uint8_t modetype = hc->get_mode_type();
                if (modetype == HeatingCircuit::Mode::NIGHT || modetype == HeatingCircuit::Mode::ECO) {
                    offset = EMS_OFFSET_JunkersSetMessage2_eco_temp;
                } else if (modetype == HeatingCircuit::Mode::DAY || modetype == HeatingCircuit::Mode::HEAT) {
                    offset = EMS_OFFSET_JunkersSetMessage2_heat_temp;
                } else {
                    offset = EMS_OFFSET_JunkersSetMessage2_no_frost_temp;
                }
                break;
            }
        }
    }

    // if we know what to send and to where, go and do it
    if (offset != -1) {
        char s[10];
        LOG_INFO(F("Setting thermostat temperature to %s for heating circuit %d, mode %s"),
                 Helpers::render_value(s, temperature, 2),
                 hc->hc_num(),
                 mode_tostring(mode).c_str());

        // add the write command to the Tx queue. value is *2
        // post validate is the corresponding monitor or set type IDs as they can differ per model
        if (factor == 0xFF) {
            write_command(set_typeid, offset, factor, validate_typeid);
        } else {
            write_command(set_typeid, offset, (uint8_t)((float)temperature * (float)factor), validate_typeid);
        }
        return true;
    }

    LOG_WARNING(F("Set temperature: Invalid value"));
    return false;
}

bool Thermostat::set_temperature_value(const char * value, const int8_t id, const uint8_t mode) {
    float   f      = 0;
    uint8_t hc_num = (id == -1) ? AUTO_HEATING_CIRCUIT : id;
    if (Helpers::value2float(value, f)) {
        return set_temperature(f, mode, hc_num);
    } else {
        LOG_WARNING(F("Set temperature: Invalid value"));
        return false;
    }
}

bool Thermostat::set_temp(const char * value, const int8_t id) {
    return set_temperature_value(value, id, HeatingCircuit::Mode::AUTO);
}

bool Thermostat::set_nighttemp(const char * value, const int8_t id) {
    return set_temperature_value(value, id, HeatingCircuit::Mode::NIGHT);
}

bool Thermostat::set_daytemp(const char * value, const int8_t id) {
    return set_temperature_value(value, id, HeatingCircuit::Mode::DAY);
}

bool Thermostat::set_comforttemp(const char * value, const int8_t id) {
    return set_temperature_value(value, id, HeatingCircuit::Mode::COMFORT);
}

bool Thermostat::set_nofrosttemp(const char * value, const int8_t id) {
    return set_temperature_value(value, id, HeatingCircuit::Mode::NOFROST);
}

bool Thermostat::set_ecotemp(const char * value, const int8_t id) {
    return set_temperature_value(value, id, HeatingCircuit::Mode::ECO);
}

bool Thermostat::set_heattemp(const char * value, const int8_t id) {
    return set_temperature_value(value, id, HeatingCircuit::Mode::HEAT);
}

bool Thermostat::set_summertemp(const char * value, const int8_t id) {
    return set_temperature_value(value, id, HeatingCircuit::Mode::SUMMER);
}

bool Thermostat::set_designtemp(const char * value, const int8_t id) {
    return set_temperature_value(value, id, HeatingCircuit::Mode::DESIGN);
}

bool Thermostat::set_offsettemp(const char * value, const int8_t id) {
    return set_temperature_value(value, id, HeatingCircuit::Mode::OFFSET);
}

bool Thermostat::set_holidaytemp(const char * value, const int8_t id) {
    return set_temperature_value(value, id, HeatingCircuit::Mode::HOLIDAY);
}

bool Thermostat::set_manualtemp(const char * value, const int8_t id) {
    return set_temperature_value(value, id, HeatingCircuit::Mode::MANUAL);
}

bool Thermostat::set_tempautotemp(const char * value, const int8_t id) {
    return set_temperature_value(value, id, HeatingCircuit::Mode::TEMPAUTO);
}

bool Thermostat::set_noreducetemp(const char * value, const int8_t id) {
    return set_temperature_value(value, id, HeatingCircuit::Mode::NOREDUCE);
}

bool Thermostat::set_flowtempoffset(const char * value, const int8_t id) {
    return set_temperature_value(value, id, HeatingCircuit::Mode::FLOWOFFSET);
}

bool Thermostat::set_maxflowtemp(const char * value, const int8_t id) {
    return set_temperature_value(value, id, HeatingCircuit::Mode::MAXFLOW);
}

bool Thermostat::set_minflowtemp(const char * value, const int8_t id) {
    return set_temperature_value(value, id, HeatingCircuit::Mode::MINFLOW);
}

bool Thermostat::set_roominfluence(const char * value, const int8_t id) {
    return set_temperature_value(value, id, HeatingCircuit::Mode::ROOMINFLUENCE);
}

// register main device values, top level for all thermostats (excluding heating circuits)
// as these are done in void Thermostat::register_device_values_hc()
void Thermostat::register_device_values() {
    // Common for all thermostats
    register_device_value(TAG_THERMOSTAT_DATA, &id_, DeviceValueType::UINT, nullptr, FL_(ID), DeviceValueUOM::NONE);
    register_device_value(TAG_THERMOSTAT_DATA, &errorCode_, DeviceValueType::TEXT, nullptr, FL_(errorCode), DeviceValueUOM::NONE);
    register_device_value(TAG_THERMOSTAT_DATA, &lastCode_, DeviceValueType::TEXT, nullptr, FL_(lastCode), DeviceValueUOM::NONE);

    switch (this->model()) {
    case EMS_DEVICE_FLAG_RC100:
    case EMS_DEVICE_FLAG_RC300:
        register_device_value(TAG_THERMOSTAT_DATA, &dateTime_, DeviceValueType::TEXT, nullptr, FL_(dateTime), DeviceValueUOM::NONE, MAKE_CF_CB(set_datetime));
        register_device_value(TAG_THERMOSTAT_DATA, &floordrystatus_, DeviceValueType::ENUM, FL_(enum_floordrystatus), FL_(floordrystatus), DeviceValueUOM::LIST);
        register_device_value(TAG_THERMOSTAT_DATA, &dampedoutdoortemp2_, DeviceValueType::SHORT, FL_(div10), FL_(dampedoutdoortemp), DeviceValueUOM::DEGREES);
        register_device_value(TAG_THERMOSTAT_DATA, &floordrytemp_, DeviceValueType::UINT, nullptr, FL_(floordrytemp), DeviceValueUOM::DEGREES);
        register_device_value(TAG_THERMOSTAT_DATA,
                              &ibaBuildingType_,
                              DeviceValueType::ENUM,
                              FL_(enum_ibaBuildingType),
                              FL_(ibaBuildingType),
                              DeviceValueUOM::LIST,
                              MAKE_CF_CB(set_building));
        register_device_value(TAG_THERMOSTAT_DATA,
                              &ibaMinExtTemperature_,
                              DeviceValueType::INT,
                              nullptr,
                              FL_(ibaMinExtTemperature),
                              DeviceValueUOM::DEGREES,
                              MAKE_CF_CB(set_minexttemp));
        register_device_value(TAG_THERMOSTAT_DATA, &wwSetTemp_, DeviceValueType::UINT, nullptr, FL_(wwSetTemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_wwtemp));
        register_device_value(TAG_THERMOSTAT_DATA, &wwMode_, DeviceValueType::ENUM, FL_(enum_wwMode), FL_(wwMode), DeviceValueUOM::LIST, MAKE_CF_CB(set_wwmode));
        register_device_value(
            TAG_THERMOSTAT_DATA, &wwSetTempLow_, DeviceValueType::UINT, nullptr, FL_(wwSetTempLow), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_wwtemplow));
        register_device_value(
            TAG_THERMOSTAT_DATA, &wwCircMode_, DeviceValueType::ENUM, FL_(enum_wwCircMode), FL_(wwCircMode), DeviceValueUOM::LIST, MAKE_CF_CB(set_wwcircmode));
        register_device_value(TAG_THERMOSTAT_DATA,
                              &wwChargeDuration_,
                              DeviceValueType::ENUM,
                              FL_(enum_wwChargeDuration),
                              FL_(wwChargeDuration),
                              DeviceValueUOM::LIST,
                              MAKE_CF_CB(set_wwchargeduration));
        register_device_value(TAG_THERMOSTAT_DATA, &wwCharge_, DeviceValueType::BOOL, nullptr, FL_(wwCharge), DeviceValueUOM::BOOLEAN, MAKE_CF_CB(set_wwcharge));
        register_device_value(TAG_THERMOSTAT_DATA, &wwExtra1_, DeviceValueType::UINT, nullptr, FL_(wwExtra1), DeviceValueUOM::DEGREES);
        register_device_value(TAG_THERMOSTAT_DATA, &wwExtra2_, DeviceValueType::UINT, nullptr, FL_(wwExtra2), DeviceValueUOM::DEGREES);
        break;
    case EMS_DEVICE_FLAG_RC20_N:
    case EMS_DEVICE_FLAG_RC20:
        register_device_value(TAG_THERMOSTAT_DATA, &dateTime_, DeviceValueType::TEXT, nullptr, FL_(dateTime), DeviceValueUOM::NONE); // can't set datetime
        break;
    case EMS_DEVICE_FLAG_RC30_N:
        register_device_value(TAG_THERMOSTAT_DATA, &dateTime_, DeviceValueType::TEXT, nullptr, FL_(dateTime), DeviceValueUOM::NONE); // can't set datetime
        register_device_value(TAG_THERMOSTAT_DATA, &ibaMainDisplay_, DeviceValueType::ENUM, FL_(enum_ibaMainDisplay), FL_(ibaMainDisplay), DeviceValueUOM::LIST);
        register_device_value(TAG_THERMOSTAT_DATA, &ibaLanguage_, DeviceValueType::ENUM, FL_(enum_ibaLanguage), FL_(ibaLanguage), DeviceValueUOM::LIST);
        register_device_value(TAG_THERMOSTAT_DATA,
                              &ibaClockOffset_,
                              DeviceValueType::UINT,
                              nullptr,
                              FL_(ibaClockOffset),
                              DeviceValueUOM::NONE); // offset (in sec) to clock, 0xff=-1s, 0x02=2s
        register_device_value(TAG_THERMOSTAT_DATA,
                              &ibaCalIntTemperature_,
                              DeviceValueType::INT,
                              FL_(div2),
                              FL_(ibaCalIntTemperature),
                              DeviceValueUOM::DEGREES,
                              MAKE_CF_CB(set_calinttemp));
        register_device_value(TAG_THERMOSTAT_DATA,
                              &ibaMinExtTemperature_,
                              DeviceValueType::INT,
                              nullptr,
                              FL_(ibaMinExtTemperature),
                              DeviceValueUOM::DEGREES,
                              MAKE_CF_CB(set_minexttemp));
        register_device_value(TAG_THERMOSTAT_DATA, &dampedoutdoortemp_, DeviceValueType::INT, nullptr, FL_(dampedoutdoortemp), DeviceValueUOM::DEGREES);
        register_device_value(TAG_THERMOSTAT_DATA,
                              &ibaBuildingType_,
                              DeviceValueType::ENUM,
                              FL_(enum_ibaBuildingType),
                              FL_(ibaBuildingType),
                              DeviceValueUOM::LIST,
                              MAKE_CF_CB(set_building));
        register_device_value(TAG_THERMOSTAT_DATA, &wwMode_, DeviceValueType::ENUM, FL_(enum_wwMode2), FL_(wwMode), DeviceValueUOM::LIST, MAKE_CF_CB(set_wwmode));
        register_device_value(
            TAG_THERMOSTAT_DATA, &wwCircMode_, DeviceValueType::ENUM, FL_(enum_wwCircMode2), FL_(wwCircMode), DeviceValueUOM::LIST, MAKE_CF_CB(set_wwcircmode));
        break;
    case EMS_DEVICE_FLAG_RC35:
        register_device_value(TAG_THERMOSTAT_DATA, &dateTime_, DeviceValueType::TEXT, nullptr, FL_(dateTime), DeviceValueUOM::NONE, MAKE_CF_CB(set_datetime));
        register_device_value(TAG_THERMOSTAT_DATA,
                              &ibaCalIntTemperature_,
                              DeviceValueType::INT,
                              FL_(div2),
                              FL_(ibaCalIntTemperature),
                              DeviceValueUOM::DEGREES,
                              MAKE_CF_CB(set_calinttemp));
        register_device_value(TAG_THERMOSTAT_DATA,
                              &ibaMinExtTemperature_,
                              DeviceValueType::INT,
                              nullptr,
                              FL_(ibaMinExtTemperature),
                              DeviceValueUOM::DEGREES,
                              MAKE_CF_CB(set_minexttemp));
        register_device_value(TAG_THERMOSTAT_DATA, &tempsensor1_, DeviceValueType::USHORT, FL_(div10), FL_(tempsensor1), DeviceValueUOM::DEGREES);
        register_device_value(TAG_THERMOSTAT_DATA, &tempsensor2_, DeviceValueType::USHORT, FL_(div10), FL_(tempsensor2), DeviceValueUOM::DEGREES);
        register_device_value(TAG_THERMOSTAT_DATA, &ibaDamping_, DeviceValueType::BOOL, nullptr, FL_(damping), DeviceValueUOM::BOOLEAN, MAKE_CF_CB(set_damping));
        register_device_value(TAG_THERMOSTAT_DATA, &dampedoutdoortemp_, DeviceValueType::INT, nullptr, FL_(dampedoutdoortemp), DeviceValueUOM::DEGREES);
        register_device_value(TAG_THERMOSTAT_DATA,
                              &ibaBuildingType_,
                              DeviceValueType::ENUM,
                              FL_(enum_ibaBuildingType),
                              FL_(ibaBuildingType),
                              DeviceValueUOM::LIST,
                              MAKE_CF_CB(set_building));
        register_device_value(TAG_THERMOSTAT_DATA, &wwMode_, DeviceValueType::ENUM, FL_(enum_wwMode2), FL_(wwMode), DeviceValueUOM::LIST, MAKE_CF_CB(set_wwmode));
        register_device_value(
            TAG_THERMOSTAT_DATA, &wwCircMode_, DeviceValueType::ENUM, FL_(enum_wwCircMode2), FL_(wwCircMode), DeviceValueUOM::LIST, MAKE_CF_CB(set_wwcircmode));
        register_device_value(
            TAG_THERMOSTAT_DATA, &wwProgMode_, DeviceValueType::ENUM, FL_(enum_wwProgMode), FL_(wwProgMode), DeviceValueUOM::LIST, MAKE_CF_CB(set_wwProgMode));
        register_device_value(
            TAG_THERMOSTAT_DATA, &wwCircProg_, DeviceValueType::ENUM, FL_(enum_wwProgMode), FL_(wwCircProg), DeviceValueUOM::LIST, MAKE_CF_CB(set_wwCircProg));
        register_device_value(
            TAG_THERMOSTAT_DATA, &wwDisinfect_, DeviceValueType::BOOL, nullptr, FL_(wwDisinfect), DeviceValueUOM::BOOLEAN, MAKE_CF_CB(set_wwDisinfect));
        register_device_value(TAG_THERMOSTAT_DATA,
                              &wwDisinfectDay_,
                              DeviceValueType::ENUM,
                              FL_(enum_dayOfWeek),
                              FL_(wwDisinfectDay),
                              DeviceValueUOM::LIST,
                              MAKE_CF_CB(set_wwDisinfectDay));
        register_device_value(TAG_THERMOSTAT_DATA,
                              &wwDisinfectHour_,
                              DeviceValueType::UINT,
                              nullptr,
                              FL_(wwDisinfectHour),
                              DeviceValueUOM::NONE,
                              MAKE_CF_CB(set_wwDisinfectHour),
                              0,
                              23);
        register_device_value(TAG_THERMOSTAT_DATA, &wwMaxTemp_, DeviceValueType::UINT, nullptr, FL_(wwMaxTemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_wwMaxTemp));
        register_device_value(
            TAG_THERMOSTAT_DATA, &wwOneTimeKey_, DeviceValueType::BOOL, nullptr, FL_(wwOneTimeKey), DeviceValueUOM::BOOLEAN, MAKE_CF_CB(set_wwOneTimeKey));
        break;
    case EMS_DEVICE_FLAG_JUNKERS:
        register_device_value(TAG_THERMOSTAT_DATA, &dateTime_, DeviceValueType::TEXT, nullptr, FL_(dateTime), DeviceValueUOM::NONE, MAKE_CF_CB(set_datetime));
        break;
    case EMS_DEVICE_FLAG_EASY:
        // Easy TC100 have no date/time, see issue #100, not sure about CT200, so leave it.
        register_device_value(TAG_THERMOSTAT_DATA, &dateTime_, DeviceValueType::TEXT, nullptr, FL_(dateTime), DeviceValueUOM::NONE); // can't set datetime
        break;
    default:
        register_device_value(TAG_THERMOSTAT_DATA, &dateTime_, DeviceValueType::TEXT, nullptr, FL_(dateTime), DeviceValueUOM::NONE); // can't set datetime
        break;
    }
}

// registers the values for a heating circuit
void Thermostat::register_device_values_hc(std::shared_ptr<Thermostat::HeatingCircuit> hc) {
    uint8_t model = hc->get_model();

    // heating circuit
    uint8_t tag = TAG_HC1 + hc->hc_num() - 1;

    // different logic on how temperature values are stored, depending on model
    const __FlashStringHelper * const * setpoint_temp_divider;
    const __FlashStringHelper * const * curr_temp_divider;

    if (model == EMS_DEVICE_FLAG_EASY) {
        setpoint_temp_divider = FL_(div100);
        curr_temp_divider     = FL_(div100);
    } else if (model == EMS_DEVICE_FLAG_JUNKERS) {
        setpoint_temp_divider = FL_(div10);
        curr_temp_divider     = FL_(div10);
    } else {
        setpoint_temp_divider = FL_(div2);
        curr_temp_divider     = FL_(div10);
    }
    if (has_flags(EMS_DEVICE_FLAG_NO_WRITE) || device_id() != EMSESP::actual_master_thermostat()) {
        register_device_value(tag, &hc->setpoint_roomTemp, DeviceValueType::SHORT, setpoint_temp_divider, FL_(setpoint_roomTemp), DeviceValueUOM::DEGREES);
    } else {
        register_device_value(tag,
                              &hc->setpoint_roomTemp,
                              DeviceValueType::SHORT,
                              setpoint_temp_divider,
                              FL_(setpoint_roomTemp),
                              DeviceValueUOM::DEGREES,
                              MAKE_CF_CB(set_temp),
                              5,
                              29);
        register_device_value(
            tag, &hc->setpoint_roomTemp, DeviceValueType::SHORT, setpoint_temp_divider, FL_(temp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_temp), 5, 29);
    }
    register_device_value(tag, &hc->curr_roomTemp, DeviceValueType::SHORT, curr_temp_divider, FL_(curr_roomTemp), DeviceValueUOM::DEGREES);

    if (device_id() != EMSESP::actual_master_thermostat()) {
        return;
    }

    // special handling for Home Assistant
    // we create special values called hatemp and hamode, which have empty fullnames so not shown in the web or console
    if (Mqtt::ha_enabled()) {
        uint8_t option = Mqtt::ha_climate_format();
        if (option == Mqtt::HA_Climate_Format::CURRENT) {
            register_device_value(tag, &hc->curr_roomTemp, DeviceValueType::SHORT, curr_temp_divider, FL_(hatemp), DeviceValueUOM::NONE);
        } else if (option == Mqtt::HA_Climate_Format::SETPOINT) {
            register_device_value(tag, &hc->setpoint_roomTemp, DeviceValueType::SHORT, setpoint_temp_divider, FL_(hatemp), DeviceValueUOM::NONE);
        } else if (option == Mqtt::HA_Climate_Format::ZERO) {
            register_device_value(tag, &zero_value_, DeviceValueType::UINT, nullptr, FL_(hatemp), DeviceValueUOM::NONE);
        }

        // if we're sending to HA the only valid mode types are heat, auto and off
        // manual & day = heat
        // night & off = off
        // everything else auto
        register_device_value(tag, &hc->hamode, DeviceValueType::ENUM, FL_(enum_hamode), FL_(hamode), DeviceValueUOM::NONE);
    }

    switch (model) {
    case EMS_DEVICE_FLAG_RC100:
    case EMS_DEVICE_FLAG_RC300:
        register_device_value(tag, &hc->mode, DeviceValueType::ENUM, FL_(enum_mode), FL_(mode), DeviceValueUOM::LIST, MAKE_CF_CB(set_mode));
        register_device_value(tag, &hc->modetype, DeviceValueType::ENUM, FL_(enum_modetype), FL_(modetype), DeviceValueUOM::LIST);
        register_device_value(tag, &hc->nighttemp, DeviceValueType::UINT, FL_(div2), FL_(ecotemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_ecotemp));
        register_device_value(tag, &hc->manualtemp, DeviceValueType::UINT, FL_(div2), FL_(manualtemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_manualtemp));
        register_device_value(tag, &hc->daytemp, DeviceValueType::UINT, FL_(div2), FL_(comforttemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_comforttemp));
        register_device_value(tag, &hc->summertemp, DeviceValueType::UINT, nullptr, FL_(summertemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_summertemp));
        register_device_value(tag, &hc->designtemp, DeviceValueType::UINT, nullptr, FL_(designtemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_designtemp));
        register_device_value(tag, &hc->offsettemp, DeviceValueType::INT, nullptr, FL_(offsettemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_offsettemp));
        register_device_value(tag, &hc->minflowtemp, DeviceValueType::UINT, nullptr, FL_(minflowtemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_minflowtemp));
        register_device_value(tag, &hc->maxflowtemp, DeviceValueType::UINT, nullptr, FL_(maxflowtemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_maxflowtemp));
        register_device_value(tag, &hc->roominfluence, DeviceValueType::UINT, nullptr, FL_(roominfluence), DeviceValueUOM::NONE, MAKE_CF_CB(set_roominfluence));
        register_device_value(tag, &hc->nofrosttemp, DeviceValueType::INT, nullptr, FL_(nofrosttemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_nofrosttemp));
        register_device_value(tag, &hc->targetflowtemp, DeviceValueType::UINT, nullptr, FL_(targetflowtemp), DeviceValueUOM::DEGREES);
        register_device_value(tag, &hc->heatingtype, DeviceValueType::ENUM, FL_(enum_heatingtype), FL_(heatingtype), DeviceValueUOM::LIST);
        register_device_value(
            tag, &hc->summer_setmode, DeviceValueType::ENUM, FL_(enum_summermode), FL_(summersetmode), DeviceValueUOM::LIST, MAKE_CF_CB(set_summermode));
        register_device_value(tag, &hc->summermode, DeviceValueType::BOOL, nullptr, FL_(summermode), DeviceValueUOM::BOOLEAN);
        register_device_value(
            tag, &hc->controlmode, DeviceValueType::ENUM, FL_(enum_controlmode), FL_(controlmode), DeviceValueUOM::LIST, MAKE_CF_CB(set_controlmode));
        register_device_value(tag, &hc->program, DeviceValueType::UINT, nullptr, FL_(program), DeviceValueUOM::NONE, MAKE_CF_CB(set_program));
        register_device_value(tag, &hc->tempautotemp, DeviceValueType::UINT, FL_(div2), FL_(tempautotemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_tempautotemp));
        register_device_value(
            tag, &hc->fastHeatupFactor, DeviceValueType::UINT, nullptr, FL_(fastheatupfactor), DeviceValueUOM::NONE, MAKE_CF_CB(set_fastheatupfactor));
        break;
    case EMS_DEVICE_FLAG_CRF:
        register_device_value(tag, &hc->mode, DeviceValueType::ENUM, FL_(enum_mode5), FL_(mode), DeviceValueUOM::LIST);
        register_device_value(tag, &hc->modetype, DeviceValueType::ENUM, FL_(enum_modetype5), FL_(modetype), DeviceValueUOM::LIST);
        register_device_value(tag, &hc->targetflowtemp, DeviceValueType::UINT, nullptr, FL_(targetflowtemp), DeviceValueUOM::DEGREES);
        break;
    case EMS_DEVICE_FLAG_RC20:
        register_device_value(tag, &hc->mode, DeviceValueType::ENUM, FL_(enum_mode2), FL_(mode), DeviceValueUOM::LIST, MAKE_CF_CB(set_mode));
        break;
    case EMS_DEVICE_FLAG_RC20_N:
        register_device_value(tag, &hc->mode, DeviceValueType::ENUM, FL_(enum_mode2), FL_(mode), DeviceValueUOM::LIST, MAKE_CF_CB(set_mode));
        register_device_value(tag, &hc->modetype, DeviceValueType::ENUM, FL_(enum_modetype2), FL_(modetype), DeviceValueUOM::LIST);
        register_device_value(tag, &hc->daytemp, DeviceValueType::UINT, FL_(div2), FL_(daytemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_daytemp));
        register_device_value(tag, &hc->nighttemp, DeviceValueType::UINT, FL_(div2), FL_(nighttemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_nighttemp));
        register_device_value(tag, &hc->program, DeviceValueType::UINT, nullptr, FL_(program), DeviceValueUOM::NONE, MAKE_CF_CB(set_program));
        // RC25 additions, guess, not validated by users, see:https://github.com/emsesp/EMS-ESP32/issues/106
        register_device_value(tag, &hc->minflowtemp, DeviceValueType::UINT, nullptr, FL_(minflowtemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_minflowtemp));
        register_device_value(tag, &hc->maxflowtemp, DeviceValueType::UINT, nullptr, FL_(maxflowtemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_maxflowtemp));
        register_device_value(tag, &hc->tempautotemp, DeviceValueType::UINT, FL_(div2), FL_(tempautotemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_tempautotemp));
        register_device_value(
            tag, &hc->heatingtype, DeviceValueType::ENUM, FL_(enum_heatingtype), FL_(heatingtype), DeviceValueUOM::LIST, MAKE_CF_CB(set_heatingtype));
        register_device_value(tag, &hc->summertemp, DeviceValueType::UINT, nullptr, FL_(summertemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_summertemp));
        register_device_value(tag, &hc->summermode, DeviceValueType::BOOL, nullptr, FL_(summermode), DeviceValueUOM::BOOLEAN);
        break;
    case EMS_DEVICE_FLAG_RC30_N:
    case EMS_DEVICE_FLAG_RC35:
        register_device_value(tag, &hc->mode, DeviceValueType::ENUM, FL_(enum_mode3), FL_(mode), DeviceValueUOM::LIST, MAKE_CF_CB(set_mode));
        register_device_value(tag, &hc->modetype, DeviceValueType::ENUM, FL_(enum_modetype3), FL_(modetype), DeviceValueUOM::LIST);
        register_device_value(tag, &hc->daytemp, DeviceValueType::UINT, FL_(div2), FL_(daytemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_daytemp));
        register_device_value(tag, &hc->nighttemp, DeviceValueType::UINT, FL_(div2), FL_(nighttemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_nighttemp));
        register_device_value(tag, &hc->designtemp, DeviceValueType::UINT, nullptr, FL_(designtemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_designtemp));
        register_device_value(tag, &hc->offsettemp, DeviceValueType::INT, FL_(div2), FL_(offsettemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_offsettemp));
        register_device_value(tag, &hc->holidaytemp, DeviceValueType::UINT, FL_(div2), FL_(holidaytemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_holidaytemp));
        register_device_value(tag, &hc->targetflowtemp, DeviceValueType::UINT, nullptr, FL_(targetflowtemp), DeviceValueUOM::DEGREES);
        register_device_value(tag, &hc->summertemp, DeviceValueType::UINT, nullptr, FL_(summertemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_summertemp));
        register_device_value(tag, &hc->summermode, DeviceValueType::BOOL, nullptr, FL_(summermode), DeviceValueUOM::BOOLEAN);
        register_device_value(tag, &hc->holidaymode, DeviceValueType::BOOL, nullptr, FL_(holidaymode), DeviceValueUOM::BOOLEAN, MAKE_CF_CB(set_holiday));
        register_device_value(tag, &hc->nofrosttemp, DeviceValueType::INT, nullptr, FL_(nofrosttemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_nofrosttemp));
        register_device_value(tag, &hc->roominfluence, DeviceValueType::UINT, nullptr, FL_(roominfluence), DeviceValueUOM::NONE, MAKE_CF_CB(set_roominfluence));
        register_device_value(tag, &hc->minflowtemp, DeviceValueType::UINT, nullptr, FL_(minflowtemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_minflowtemp));
        register_device_value(tag, &hc->maxflowtemp, DeviceValueType::UINT, nullptr, FL_(maxflowtemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_maxflowtemp));
        register_device_value(tag, &hc->flowtempoffset, DeviceValueType::UINT, nullptr, FL_(flowtempoffset), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_flowtempoffset));
        register_device_value(
            tag, &hc->heatingtype, DeviceValueType::ENUM, FL_(enum_heatingtype), FL_(heatingtype), DeviceValueUOM::LIST, MAKE_CF_CB(set_heatingtype));
        register_device_value(tag, &hc->reducemode, DeviceValueType::ENUM, FL_(enum_reducemode), FL_(reducemode), DeviceValueUOM::LIST, MAKE_CF_CB(set_reducemode));
        register_device_value(
            tag, &hc->controlmode, DeviceValueType::ENUM, FL_(enum_controlmode2), FL_(controlmode), DeviceValueUOM::LIST, MAKE_CF_CB(set_controlmode));
        register_device_value(tag, &hc->control, DeviceValueType::ENUM, FL_(enum_control), FL_(control), DeviceValueUOM::LIST, MAKE_CF_CB(set_control));
        register_device_value(tag, &hc->program, DeviceValueType::UINT, nullptr, FL_(program), DeviceValueUOM::NONE, MAKE_CF_CB(set_program));
        register_device_value(tag, &hc->pause, DeviceValueType::UINT, nullptr, FL_(pause), DeviceValueUOM::HOURS, MAKE_CF_CB(set_pause));
        register_device_value(tag, &hc->party, DeviceValueType::UINT, nullptr, FL_(party), DeviceValueUOM::HOURS, MAKE_CF_CB(set_party));
        register_device_value(tag, &hc->tempautotemp, DeviceValueType::UINT, FL_(div2), FL_(tempautotemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_tempautotemp));
        register_device_value(tag, &hc->noreducetemp, DeviceValueType::INT, nullptr, FL_(noreducetemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_noreducetemp));
        register_device_value(tag, &hc->remotetemp, DeviceValueType::SHORT, FL_(div10), FL_(remotetemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_remotetemp));
        register_device_value(tag, &hc->wwprio, DeviceValueType::BOOL, nullptr, FL_(wwprio), DeviceValueUOM::BOOLEAN, MAKE_CF_CB(set_wwprio));
        register_device_value(tag, &dummy_, DeviceValueType::CMD, nullptr, FL_(switchtime), DeviceValueUOM::NONE, MAKE_CF_CB(set_switchtime));
        break;
    case EMS_DEVICE_FLAG_JUNKERS:
        register_device_value(tag, &hc->mode, DeviceValueType::ENUM, FL_(enum_mode4), FL_(mode), DeviceValueUOM::LIST, MAKE_CF_CB(set_mode));
        register_device_value(tag, &hc->modetype, DeviceValueType::ENUM, FL_(enum_modetype4), FL_(modetype), DeviceValueUOM::LIST);
        register_device_value(tag, &hc->daytemp, DeviceValueType::UINT, FL_(div2), FL_(heattemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_heattemp));
        register_device_value(tag, &hc->nighttemp, DeviceValueType::UINT, FL_(div2), FL_(ecotemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_ecotemp));
        register_device_value(tag, &hc->nofrosttemp, DeviceValueType::INT, FL_(div2), FL_(nofrosttemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_nofrosttemp));
        break;
    }
}

} // namespace emsesp
