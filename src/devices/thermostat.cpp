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

#include "thermostat.h"

MAKE_PSTR_WORD(thermostat)
MAKE_PSTR_WORD(master)
MAKE_PSTR_WORD(temp)
MAKE_PSTR_WORD(mode)
MAKE_PSTR_WORD(wwmode)

MAKE_PSTR(hc_optional, "[heating circuit]")
MAKE_PSTR(mode_mandatory, "<mode>")
MAKE_PSTR(mode_optional, "[mode]")
MAKE_PSTR(master_thermostat_fmt, "Master Thermostat device ID = %s")

namespace emsesp {

REGISTER_FACTORY(Thermostat, EMSdevice::DeviceType::THERMOSTAT);
MAKE_PSTR(logger_name, "thermostat")
uuid::log::Logger Thermostat::logger_{F_(logger_name), uuid::log::Facility::CONSOLE};

Thermostat::Thermostat(uint8_t device_type, uint8_t device_id, uint8_t product_id, const std::string & version, const std::string & name, uint8_t flags, uint8_t brand)
    : EMSdevice(device_type, device_id, product_id, version, name, flags, brand) {
    // common telegram handlers
    register_telegram_type(EMS_TYPE_RCOutdoorTemp, F("RCOutdoorTemp"), false, std::bind(&Thermostat::process_RCOutdoorTemp, this, _1));
    register_telegram_type(EMS_TYPE_RCTime, F("RCTime"), true, std::bind(&Thermostat::process_RCTime, this, _1)); // 0x06

    // RC10
    if (flags == EMSdevice::EMS_DEVICE_FLAG_RC10) {
        monitor_typeids = {0xB1};
        set_typeids     = {0xB0};
        for (uint8_t i = 0; i < monitor_typeids.size(); i++) {
            register_telegram_type(monitor_typeids[i], F("RC10Monitor"), false, std::bind(&Thermostat::process_RC10Monitor, this, _1));
            register_telegram_type(set_typeids[i], F("RC10Set"), false, std::bind(&Thermostat::process_RC10Set, this, _1));
        }

        // RC35
    } else if ((flags == EMSdevice::EMS_DEVICE_FLAG_RC35) || (flags == EMSdevice::EMS_DEVICE_FLAG_RC30_1)) {
        monitor_typeids = {0x3E, 0x48, 0x52, 0x5C};
        set_typeids     = {0x3D, 0x47, 0x51, 0x5B};
        timer_typeids   = {0x3F, 0x49, 0x53, 0x5D};
        for (uint8_t i = 0; i < monitor_typeids.size(); i++) {
            register_telegram_type(monitor_typeids[i], F("RC35Monitor"), false, std::bind(&Thermostat::process_RC35Monitor, this, _1));
            register_telegram_type(set_typeids[i], F("RC35Set"), false, std::bind(&Thermostat::process_RC35Set, this, _1));
        }
        register_telegram_type(EMS_TYPE_IBASettings, F("IBASettings"), true, std::bind(&Thermostat::process_IBASettings, this, _1));
        register_telegram_type(EMS_TYPE_wwSettings, F("WWSettings"), true, std::bind(&Thermostat::process_RC35wwSettings, this, _1));

        // RC20
    } else if (flags == EMSdevice::EMS_DEVICE_FLAG_RC20) {
        monitor_typeids = {0x91};
        set_typeids     = {0xA8};
        for (uint8_t i = 0; i < monitor_typeids.size(); i++) {
            register_telegram_type(monitor_typeids[i], F("RC20Monitor"), false, std::bind(&Thermostat::process_RC20Monitor, this, _1));
            register_telegram_type(set_typeids[i], F("RC20Set"), false, std::bind(&Thermostat::process_RC20Set, this, _1));
        }
        register_telegram_type(0xAF, F("RC20Remote"), false, std::bind(&Thermostat::process_RC20Remote, this, _1));

        // RC20 newer
    } else if (flags == EMSdevice::EMS_DEVICE_FLAG_RC20_2) {
        monitor_typeids = {0xAE};
        set_typeids     = {0xAD};
        for (uint8_t i = 0; i < monitor_typeids.size(); i++) {
            register_telegram_type(monitor_typeids[i], F("RC20Monitor"), false, std::bind(&Thermostat::process_RC20Monitor_2, this, _1));
            register_telegram_type(set_typeids[i], F("RC20Set"), false, std::bind(&Thermostat::process_RC20Set_2, this, _1));
        }
        register_telegram_type(0xAF, F("RC20Remote"), false, std::bind(&Thermostat::process_RC20Remote, this, _1));

        // RC30
    } else if (flags == EMSdevice::EMS_DEVICE_FLAG_RC30) {
        monitor_typeids = {0x41};
        set_typeids     = {0xA7};
        for (uint8_t i = 0; i < monitor_typeids.size(); i++) {
            register_telegram_type(monitor_typeids[i], F("RC30Monitor"), false, std::bind(&Thermostat::process_RC30Monitor, this, _1));
            register_telegram_type(set_typeids[i], F("RC30Set"), false, std::bind(&Thermostat::process_RC30Set, this, _1));
        }

        // EASY
    } else if (flags == EMSdevice::EMS_DEVICE_FLAG_EASY) {
        monitor_typeids = {0x0A};
        set_typeids     = {};
        register_telegram_type(monitor_typeids[0], F("EasyMonitor"), false, std::bind(&Thermostat::process_EasyMonitor, this, _1));

        // RC300/RC100
    } else if ((flags == EMSdevice::EMS_DEVICE_FLAG_RC300) || (flags == EMSdevice::EMS_DEVICE_FLAG_RC100)) {
        monitor_typeids = {0x02A5, 0x02A6, 0x02A7, 0x02A8};
        set_typeids     = {0x02B9, 0x02BA, 0x02BB, 0x02BC};
        for (uint8_t i = 0; i < monitor_typeids.size(); i++) {
            register_telegram_type(monitor_typeids[i], F("RC300Monitor"), false, std::bind(&Thermostat::process_RC300Monitor, this, _1));
            register_telegram_type(set_typeids[i], F("RC300Set"), false, std::bind(&Thermostat::process_RC300Set, this, _1));
        }
        register_telegram_type(0x31D, F("RC300WWmode"), false, std::bind(&Thermostat::process_RC300WWmode, this, _1));
        register_telegram_type(0x31E, F("RC300WWmode"), false, std::bind(&Thermostat::process_RC300WWmode, this, _1));

        // JUNKERS/HT3
    } else if (flags == EMSdevice::EMS_DEVICE_FLAG_JUNKERS) {
        monitor_typeids = {0x016F, 0x0170, 0x0171, 0x0172};
        set_typeids     = {0x0165, 0x0166, 0x0167, 0x0168};
        for (uint8_t i = 0; i < monitor_typeids.size(); i++) {
            register_telegram_type(monitor_typeids[i], F("JunkersMonitor"), false, std::bind(&Thermostat::process_JunkersMonitor, this, _1));
            register_telegram_type(set_typeids[i], F("JunkersSet"), false, std::bind(&Thermostat::process_JunkersSet, this, _1));
        }

    } else if (flags == (EMSdevice::EMS_DEVICE_FLAG_JUNKERS | EMSdevice::EMS_DEVICE_FLAG_JUNKERS_2)) {
        monitor_typeids = {0x016F, 0x0170, 0x0171, 0x0172};
        set_typeids     = {0x0179, 0x017A, 0x017B, 0x017C};
        for (uint8_t i = 0; i < monitor_typeids.size(); i++) {
            register_telegram_type(monitor_typeids[i], F("JunkersMonitor"), false, std::bind(&Thermostat::process_JunkersMonitor, this, _1));
            register_telegram_type(set_typeids[i], F("JunkersSet"), false, std::bind(&Thermostat::process_JunkersSet, this, _1));
        }
    }

    uint8_t master_thermostat = 0;
    EMSESP::emsespSettingsService.read([&](EMSESPSettings & settings) {
        master_thermostat = settings.master_thermostat; // what the user has defined
    });

    EMSESP::esp8266React.getMqttSettingsService()->read([&](MqttSettings & settings) {
        mqtt_format_ = settings.mqtt_format; // single, nested or ha
    });

    uint8_t actual_master_thermostat = EMSESP::actual_master_thermostat();                           // what we're actually using
    uint8_t num_devices              = EMSESP::count_devices(EMSdevice::DeviceType::THERMOSTAT) + 1; // including this thermostat

    // if we're on auto mode, register this thermostat if it has a device id of 0x10 or 0x17
    // or if its the master thermostat we defined
    // see https://github.com/proddy/EMS-ESP/issues/362#issuecomment-629628161
    if (((num_devices == 1) && (actual_master_thermostat == EMSESP_DEFAULT_MASTER_THERMOSTAT) && ((device_id == 0x10) || (device_id == 0x17)))
        || (master_thermostat == device_id)) {
        EMSESP::actual_master_thermostat(device_id);
        LOG_DEBUG(F("Registering new thermostat with device ID 0x%02X (as the master)"), device_id);
        init_mqtt();
    } else {
        LOG_DEBUG(F("Registering new thermostat with device ID 0x%02X"), device_id);
    }


    // for the thermostat, go a query all the heating circuits. This is only done once. The automatic fetch will from now on
    // only update the active heating circuits
    for (uint8_t i = 0; i < monitor_typeids.size(); i++) {
        EMSESP::send_read_request(set_typeids[i], device_id);
    }
    for (uint8_t i = 0; i < monitor_typeids.size(); i++) {
        EMSESP::send_read_request(monitor_typeids[i], device_id);
    }
}

// for the master thermostat initialize the MQTT subscribes
void Thermostat::init_mqtt() {
    register_mqtt_topic("thermostat_cmd", std::bind(&Thermostat::thermostat_cmd, this, _1)); // generic commands

    // if the MQTT format type is ha then send the config to HA (via the mqtt discovery service)
    // for each of the heating circuits
    // state is /state
    // config is /config
    if (mqtt_format_ == MQTT_format::HA) {
        for (uint8_t hc = 0; hc < monitor_typeids.size(); hc++) {
            StaticJsonDocument<EMSESP_MAX_JSON_SIZE_MEDIUM> doc;
            uint8_t                                         hc_num = hc + 1;

            std::string hc_text(10, '\0');
            snprintf_P(&hc_text[0], hc_text.capacity() + 1, PSTR("hc%d"), hc_num);
            doc["name"]    = hc_text;
            doc["uniq_id"] = hc_text;

            // topic root is homeassistant/climate/ems-esp/hc<1..n>/
            std::string root(100, '\0');
            snprintf_P(&root[0], root.capacity() + 1, PSTR("homeassistant/climate/ems-esp/hc%d"), hc_num);
            doc["~"] = root;

            doc["mode_cmd_t"]  = "~/cmd_mode";
            doc["mode_stat_t"] = "~/state";
            doc["temp_cmd_t"]  = "~/cmd_temp";
            doc["temp_stat_t"] = "~/state";
            doc["curr_temp_t"] = "~/state";

            std::string mode_str(30, '\0');
            snprintf_P(&mode_str[0], 30, PSTR("{{value_json.hc%d.mode}}"), hc_num);
            doc["mode_stat_tpl"] = mode_str;

            std::string seltemp_str(30, '\0');
            snprintf_P(&seltemp_str[0], 30, PSTR("{{value_json.hc%d.seltemp}}"), hc_num);
            doc["temp_stat_tpl"] = seltemp_str;

            std::string currtemp_str(30, '\0');
            snprintf_P(&currtemp_str[0], 30, PSTR("{{value_json.hc%d.currtemp}}"), hc_num);
            doc["curr_temp_tpl"] = currtemp_str;

            doc["min_temp"]  = "5";
            doc["max_temp"]  = "40";
            doc["temp_step"] = "0.5";

            JsonArray modes = doc.createNestedArray("modes");
            modes.add("off");
            modes.add("heat");
            modes.add("auto");

            std::string topic(100, '\0'); // e.g homeassistant/climate/hc1/thermostat/config
            snprintf_P(&topic[0], topic.capacity() + 1, PSTR("homeassistant/climate/ems-esp/hc%d/config"), hc_num);
            // Mqtt::publish(topic); // empty payload, this remove any previous config sent to HA
            Mqtt::publish(topic, doc, true); // publish the config payload with retain flag

            // subscribe to the temp and mode commands
            snprintf_P(&topic[0], topic.capacity() + 1, PSTR("homeassistant/climate/ems-esp/hc%d/cmd_temp"), hc_num);
            register_mqtt_topic(topic, std::bind(&Thermostat::thermostat_cmd_temp, this, _1));
            snprintf_P(&topic[0], topic.capacity() + 1, PSTR("homeassistant/climate/ems-esp/hc%d/cmd_mode"), hc_num);
            register_mqtt_topic(topic, std::bind(&Thermostat::thermostat_cmd_mode, this, _1));
        }
    } else {
        // these will be prefixed with hostname and base
        register_mqtt_topic("thermostat_cmd_temp", std::bind(&Thermostat::thermostat_cmd_temp, this, _1));
        register_mqtt_topic("thermostat_cmd_mode", std::bind(&Thermostat::thermostat_cmd_mode, this, _1));
    }
}

// only add the menu for the master thermostat
void Thermostat::add_context_menu() {
    if (device_id() != EMSESP::actual_master_thermostat()) {
        return;
    }

    EMSESPShell::commands->add_command(ShellContext::MAIN,
                                       CommandFlags::USER,
                                       flash_string_vector{F_(thermostat)},
                                       [&](Shell & shell, const std::vector<std::string> & arguments __attribute__((unused))) {
                                           Thermostat::console_commands(shell, ShellContext::THERMOSTAT);
                                       });
}

// general MQTT command for controlling thermostat
// e.g. { "hc": 1, "cmd":"daytemp", "data": 20 }
// or { "hc": 1, "daytemp": 20 } or { "hc2": { "daytemp":20 }}
void Thermostat::thermostat_cmd(const char * message) {
    StaticJsonDocument<EMSESP_MAX_JSON_SIZE_SMALL> doc;
    DeserializationError                           error = deserializeJson(doc, message);
    if (error) {
        LOG_DEBUG(F("MQTT error: payload %s, error %s"), message, error.c_str());
        return;
    }

    // check for nested commands like {"hc2":{"temp":21}}
    for (const auto & hc : heating_circuits_) {
        char hc_name[6], s[3]; // hc{1-4}
        strlcpy(hc_name, "hc", 6);
        uint8_t hc_num = hc->hc_num();
        strlcat(hc_name, Helpers::itoa(s, hc_num), 6);
        if (nullptr != doc[hc_name]["mode"]) {
            std::string mode = doc[hc_name]["mode"];
            set_mode(mode, hc_num);
        }
        if (float f = doc[hc_name]["temp"]) {
            set_temperature(f, HeatingCircuit::Mode::AUTO, hc_num);
        }
        if (float f = doc[hc_name]["nighttemp"]) {
            set_temperature(f, HeatingCircuit::Mode::NIGHT, hc_num);
        }
        if (float f = doc[hc_name]["daytemp"]) {
            set_temperature(f, HeatingCircuit::Mode::DAY, hc_num);
        }
        if (float f = doc[hc_name]["nofrosttemp"]) {
            set_temperature(f, HeatingCircuit::Mode::NOFROST, hc_num);
        }
        if (float f = doc[hc_name]["ecotemp"]) {
            set_temperature(f, HeatingCircuit::Mode::ECO, hc_num);
        }
        if (float f = doc[hc_name]["heattemp"]) {
            set_temperature(f, HeatingCircuit::Mode::HEAT, hc_num);
        }
        if (float f = doc[hc_name]["summertemp"]) {
            set_temperature(f, HeatingCircuit::Mode::SUMMER, hc_num);
        }
        if (float f = doc[hc_name]["designtemp"]) {
            set_temperature(f, HeatingCircuit::Mode::DESIGN, hc_num);
        }
        if (float f = doc[hc_name]["offsettemp"]) {
            set_temperature(f, HeatingCircuit::Mode::OFFSET, hc_num);
        }
        if (float f = doc[hc_name]["holidaytemp"]) { //
            set_temperature(f, HeatingCircuit::Mode::HOLIDAY, hc_num);
        }
        if (float f = doc[hc_name]["remotetemp"]) {
            if (f > 100 || f < 0) {
                Roomctrl::set_remotetemp(hc_num - 1, EMS_VALUE_SHORT_NOTSET);
            } else {
                Roomctrl::set_remotetemp(hc_num - 1, (int16_t)(f * 10));
            }
        }
        if (nullptr != doc[hc_name]["control"]) {
            uint8_t ctrl = doc[hc_name]["control"];
            set_control(ctrl, hc_num);
        }
        if (nullptr != doc[hc_name]["pause"]) {
            uint8_t p = doc[hc_name]["pause"];
            set_pause(p, hc_num);
        }
        if (nullptr != doc[hc_name]["party"]) {
            uint8_t p = doc[hc_name]["party"];
            set_party(p, hc_num);
        }
        if (nullptr != doc[hc_name]["holiday"]) {
            std::string holiday = doc[hc_name]["holiday"];
            set_holiday(holiday.c_str(), hc_num);
        }
    }

    if (nullptr != doc["wwmode"]) {
        std::string mode = doc["wwmode"];
        set_ww_mode(mode);
    }
    if (float ct = doc["calinttemp"]) {
        set_settings_calinttemp((int8_t)(ct * 10));
    }
    if (nullptr != doc["minexttemp"]) {
        int8_t mt = doc["minexttemp"];
        set_settings_minexttemp(mt);
    }
    if (nullptr != doc["building"]) {
        std::string bds = doc["building"];
        uint8_t     bd  = doc["building"];
        if (strcmp(bds.c_str(), "light") == 0) {
            bd = 0;
        } else if (strcmp(bds.c_str(), "medium") == 0) {
            bd = 1;
        } else if (strcmp(bds.c_str(), "heavy") == 0) {
            bd = 2;
        }
        set_settings_building(bd);
    }
    if (nullptr != doc["language"]) {
        uint8_t lg = doc["language"];
        set_settings_language(lg);
    }
    if (nullptr != doc["display"]) {
        uint8_t dp = doc["display"];
        set_settings_display(dp);
    }
    if (nullptr != doc["clockoffset"]) {
        int8_t co = doc["clockoffset"];
        set_settings_clockoffset(co);
    }

    // get heating circuit if it exists
    uint8_t hc_num = doc["hc"] | AUTO_HEATING_CIRCUIT;

    // check for unnested commands like {"temp":21} or {"hc":2,"temp":21,"mode":"auto"}
    if (nullptr != doc["mode"]) {
        std::string mode = doc["mode"];
        set_mode(mode, hc_num);
    }
    if (float f = doc["temp"]) {
        set_temperature(f, HeatingCircuit::Mode::AUTO, hc_num);
    }
    if (float f = doc["nighttemp"]) {
        set_temperature(f, HeatingCircuit::Mode::NIGHT, hc_num);
    }
    if (float f = doc["daytemp"]) {
        set_temperature(f, HeatingCircuit::Mode::DAY, hc_num);
    }
    if (float f = doc["nofrosttemp"]) {
        set_temperature(f, HeatingCircuit::Mode::NOFROST, hc_num);
    }
    if (float f = doc["ecotemp"]) {
        set_temperature(f, HeatingCircuit::Mode::ECO, hc_num);
    }
    if (float f = doc["heattemp"]) {
        set_temperature(f, HeatingCircuit::Mode::HEAT, hc_num);
    }
    if (float f = doc["summertemp"]) {
        set_temperature(f, HeatingCircuit::Mode::SUMMER, hc_num);
    }
    if (float f = doc["designtemp"]) {
        set_temperature(f, HeatingCircuit::Mode::DESIGN, hc_num);
    }
    if (float f = doc["offsettemp"]) {
        set_temperature(f, HeatingCircuit::Mode::OFFSET, hc_num);
    }
    if (float f = doc["holidaytemp"]) { //
        set_temperature(f, HeatingCircuit::Mode::HOLIDAY, hc_num);
    }
    if (float f = doc["remotetemp"]) {
        if (f > 100 || f < 0) {
            Roomctrl::set_remotetemp(hc_num - 1, EMS_VALUE_SHORT_NOTSET);
        } else {
            Roomctrl::set_remotetemp(hc_num - 1, (int16_t)(f * 10));
        }
    }
    if (nullptr != doc["control"]) {
        uint8_t ctrl = doc["control"];
        set_control(ctrl, hc_num);
    }
    if (nullptr != doc["pause"]) {
        uint8_t p = doc["pause"];
        set_pause(p, hc_num);
    }
    if (nullptr != doc["party"]) {
        uint8_t p = doc["party"];
        set_party(p, hc_num);
    }
    if (nullptr != doc["holiday"]) {
        std::string holiday = doc["holiday"];
        set_holiday(holiday.c_str(), hc_num);
    }
    if (nullptr != doc["date"]) {
        std::string date = doc["date"];
        set_datetime(date.c_str());
    }

    // check for commands like {"hc":2,"cmd":"temp","data":21}
    const char * command = doc["cmd"];
    if (command == nullptr) {
        return;
    }

    if (strcmp(command, "temp") == 0) {
        float f = doc["data"];
        if (f) {
            set_temperature(f, HeatingCircuit::Mode::AUTO, hc_num);
        }
        return;
    }

    // thermostat mode changes
    if (strcmp(command, "mode") == 0) {
        std::string mode = doc["data"];
        if (mode.empty()) {
            return;
        }
        set_mode(mode, hc_num);
        return;
    }

    if (strcmp(command, "nighttemp") == 0) {
        float f = doc["data"];
        if (f) {
            set_temperature(f, HeatingCircuit::Mode::NIGHT, hc_num);
        }
        return;
    }

    if (strcmp(command, "daytemp") == 0) {
        float f = doc["data"];
        if (f) {
            set_temperature(f, HeatingCircuit::Mode::DAY, hc_num);
        }
        return;
    }

    if (strcmp(command, "holidaytemp") == 0) {
        float f = doc["data"];
        if (f) {
            set_temperature(f, HeatingCircuit::Mode::HOLIDAY, hc_num);
        }
        return;
    }

    if (strcmp(command, "ecotemp") == 0) {
        float f = doc["data"];
        if (f) {
            set_temperature(f, HeatingCircuit::Mode::ECO, hc_num);
        }
        return;
    }

    if (strcmp(command, "heattemp") == 0) {
        float f = doc["data"];
        if (f) {
            set_temperature(f, HeatingCircuit::Mode::HEAT, hc_num);
        }
        return;
    }

    if (strcmp(command, "nofrosttemp") == 0) {
        float f = doc["data"];
        if (f) {
            set_temperature(f, HeatingCircuit::Mode::NOFROST, hc_num);
        }
        return;
    }

    if (strcmp(command, "summertemp") == 0) {
        float f = doc["data"];
        if (f) {
            set_temperature(f, HeatingCircuit::Mode::SUMMER, hc_num);
        }
        return;
    }

    if (strcmp(command, "designtemp") == 0) {
        float f = doc["data"];
        if (f) {
            set_temperature(f, HeatingCircuit::Mode::DESIGN, hc_num);
        }
        return;
    }

    if (strcmp(command, "offsettemp") == 0) {
        float f = doc["data"];
        if (f) {
            set_temperature(f, HeatingCircuit::Mode::OFFSET, hc_num);
        }
        return;
    }
}

void Thermostat::thermostat_cmd_temp(const char * message) {
    float f = strtof((char *)message, 0);
    set_temperature(f, HeatingCircuit::Mode::AUTO, AUTO_HEATING_CIRCUIT);
}

// message payload holds the text name of the mode e.g. "auto"
void Thermostat::thermostat_cmd_mode(const char * message) {
    std::string s(message);
    set_mode(s, AUTO_HEATING_CIRCUIT);
}

// this function is called post the telegram handler function has been executed
// we check if any of the thermostat values have changed and then republish if necessary
bool Thermostat::updated_values() {
    // only publish on the master thermostat
    if (EMSESP::actual_master_thermostat() != device_id()) {
        return false;
    }

    // quick hack to see if it changed. We simply just add up all the raw values
    uint16_t        new_value      = 0;
    static uint16_t current_value_ = 0;
    for (const auto & hc : heating_circuits_) {
        // don't publish if we haven't yet received some data
        if (!Helpers::hasValue(hc->setpoint_roomTemp)) {
            return false;
        }
        new_value += hc->setpoint_roomTemp + hc->curr_roomTemp + hc->mode;
    }

    if (new_value != current_value_) {
        current_value_ = new_value;
        return true;
    }

    return false;
}

// publish values via MQTT
void Thermostat::publish_values() {
    // only publish on the master thermostat
    if (EMSESP::actual_master_thermostat() != device_id()) {
        return;
    }

    LOG_DEBUG(F("Performing a thermostat publish (device ID 0x%02X)"), device_id());

    uint8_t flags    = (this->flags() & 0x0F); // specific thermostat characteristics, stripping the option bits
    bool    has_data = false;

    StaticJsonDocument<EMSESP_MAX_JSON_SIZE_MEDIUM> doc;
    JsonObject                                      rootThermostat = doc.to<JsonObject>();
    JsonObject                                      dataThermostat;

    // add external temp and other stuff specific to the RC30 and RC35
    if ((flags == EMS_DEVICE_FLAG_RC35 || flags == EMS_DEVICE_FLAG_RC30_1) && (mqtt_format_ == MQTT_format::SINGLE || mqtt_format_ == MQTT_format::CUSTOM)) {
        if (datetime_.size()) {
            rootThermostat["time"] = datetime_.c_str();
        }
        if (Helpers::hasValue(dampedoutdoortemp_)) {
            rootThermostat["dampedtemp"] = dampedoutdoortemp_;
        }
        if (Helpers::hasValue(tempsensor1_)) {
            rootThermostat["inttemp1"] = (float)tempsensor1_ / 10;
        }
        if (Helpers::hasValue(tempsensor2_)) {
            rootThermostat["inttemp2"] = (float)tempsensor2_ / 10;
        }
        if (Helpers::hasValue(ibaCalIntTemperature_)) {
            rootThermostat["intoffset"] = (float)ibaCalIntTemperature_ / 2;
        }
        if (Helpers::hasValue(ibaMinExtTemperature_)) {
            rootThermostat["minexttemp"] = (float)ibaMinExtTemperature_; // min ext temp for heating curve, in deg.
        }
        if (Helpers::hasValue(ibaBuildingType_)) {
            if (ibaBuildingType_ == 0) {
                rootThermostat["building"] = "light";
            } else if (ibaBuildingType_ == 1) {
                rootThermostat["building"] = "medium";
            } else if (ibaBuildingType_ == 2) {
                rootThermostat["building"] = "heavy";
            }
        }
        if (Helpers::hasValue(wwMode_)) {
            if (wwMode_ == 0) {
                rootThermostat["wwmode"] = "off";
            } else if (wwMode_ == 1) {
                rootThermostat["wwmode"] = "on";
            } else if (wwMode_ == 2) {
                rootThermostat["wwmode"] = "auto";
            }
        }

        // send this specific data using the thermostat_data topic
        if ((mqtt_format_ == MQTT_format::SINGLE) || (mqtt_format_ == MQTT_format::HA)) {
            Mqtt::publish("thermostat_data", doc);
            rootThermostat = doc.to<JsonObject>(); // clear object
        }
    }

    // go through all the heating circuits
    for (const auto & hc : heating_circuits_) {
        if (!Helpers::hasValue(hc->setpoint_roomTemp)) {
            break; // skip this HC
        }

        has_data = true;
        // if the MQTT format is 'nested' or 'ha' then create the parent object hc<n>
        if (mqtt_format_ != MQTT_format::SINGLE) {
            char hc_name[10]; // hc{1-4}
            strlcpy(hc_name, "hc", 10);
            char s[3];
            strlcat(hc_name, Helpers::itoa(s, hc->hc_num()), 10);
            dataThermostat = rootThermostat.createNestedObject(hc_name);
        } else {
            dataThermostat = rootThermostat;
        }

        // different logic on how temperature values are stored, depending on model
        uint8_t setpoint_temp_divider;
        uint8_t curr_temp_divider;
        if (flags == EMS_DEVICE_FLAG_EASY) {
            setpoint_temp_divider = 100;
            curr_temp_divider     = 100;
        } else if (flags == EMS_DEVICE_FLAG_JUNKERS) {
            setpoint_temp_divider = 10;
            curr_temp_divider     = 10;
        } else {
            setpoint_temp_divider = 2;
            curr_temp_divider     = 10;
        }

        if (Helpers::hasValue(hc->setpoint_roomTemp)) {
            dataThermostat["seltemp"] = Helpers::round2((float)hc->setpoint_roomTemp / setpoint_temp_divider);
        }

        if (Helpers::hasValue(hc->curr_roomTemp)) {
            dataThermostat["currtemp"] = Helpers::round2((float)hc->curr_roomTemp / curr_temp_divider);
        }

        if (Helpers::hasValue(hc->daytemp)) {
            dataThermostat["daytemp"] = (float)hc->daytemp / 2;
        }
        if (Helpers::hasValue(hc->nighttemp)) {
            dataThermostat["nighttemp"] = (float)hc->nighttemp / 2;
        }
        if (Helpers::hasValue(hc->holidaytemp)) {
            dataThermostat["holidaytemp"] = (float)hc->holidaytemp / 2;
        }

        if (Helpers::hasValue(hc->heatingtype)) {
            dataThermostat["heatingtype"] = hc->heatingtype;
        }

        if (Helpers::hasValue(hc->targetflowtemp)) {
            dataThermostat["targetflowtemp"] = hc->targetflowtemp;
        }

        if (Helpers::hasValue(hc->offsettemp)) {
            dataThermostat["offsettemp"] = hc->offsettemp / 2;
        }

        if (Helpers::hasValue(hc->designtemp)) {
            dataThermostat["designtemp"] = hc->designtemp;
        }
        if (Helpers::hasValue(hc->summertemp)) {
            dataThermostat["summertemp"] = hc->summertemp;
        }

        // when using HA always send the mode otherwise it'll may break the component/widget and report an error
        if ((Helpers::hasValue(hc->mode)) || (mqtt_format_ == MQTT_format::HA)) {
            uint8_t hc_mode = hc->get_mode(flags);
            // if we're sending to HA the only valid mode types are heat, auto and off
            if (mqtt_format_ == MQTT_format::HA) {
                if ((hc_mode == HeatingCircuit::Mode::MANUAL) || (hc_mode == HeatingCircuit::Mode::DAY)) {
                    hc_mode = HeatingCircuit::Mode::HEAT;
                } else if ((hc_mode == HeatingCircuit::Mode::NIGHT) || (hc_mode == HeatingCircuit::Mode::OFF)) {
                    hc_mode = HeatingCircuit::Mode::OFF;
                } else {
                    hc_mode = HeatingCircuit::Mode::AUTO;
                }
            }
            dataThermostat["mode"] = mode_tostring(hc_mode);
        }

        // special handling of mode type, for the RC35 replace with summer/holiday if set
        // https://github.com/proddy/EMS-ESP/issues/373#issuecomment-619810209
        if (Helpers::hasValue(hc->summer_mode) && hc->summer_mode) {
            dataThermostat["modetype"] = F("summer");
        } else if (Helpers::hasValue(hc->holiday_mode) && hc->holiday_mode) {
            dataThermostat["modetype"] = F("holiday");
        } else if (Helpers::hasValue(hc->mode_type)) {
            dataThermostat["modetype"] = mode_tostring(hc->get_mode_type(flags));
        }

        // if format is single, send immediately and clear object for next hc
        // the topic will have the hc number appended
        if (mqtt_format_ == MQTT_format::SINGLE) {
            char topic[30];
            char s[3];
            strlcpy(topic, "thermostat_data", 30);
            strlcat(topic, Helpers::itoa(s, hc->hc_num()), 30); // append hc to topic
            Mqtt::publish(topic, doc);
            rootThermostat = doc.to<JsonObject>(); // clear object
        } else if (mqtt_format_ == MQTT_format::HA) {
            std::string topic(100, '\0');
            snprintf_P(&topic[0], topic.capacity() + 1, PSTR("homeassistant/climate/ems-esp/hc%d/state"), hc->hc_num());
            Mqtt::publish(topic, doc);
        }
    }

    if (!has_data) {
        return; // nothing to send, quit
    }

    // if we're using nested json, send all in one go under one topic called thermostat_data
    if (mqtt_format_ == MQTT_format::NESTED) {
        Mqtt::publish("thermostat_data", doc);
    } else if (mqtt_format_ == MQTT_format::CUSTOM) {
        Mqtt::publish("thermostat_data", doc);
    }
}

// returns the heating circuit object based on the hc number
// of nullptr if it doesn't exist yet
std::shared_ptr<Thermostat::HeatingCircuit> Thermostat::heating_circuit(const uint8_t hc_num) {
    if (hc_num == 0) {
        // return first existing hc
        for (const auto & heating_circuit : heating_circuits_) {
            return heating_circuit;
        }
    }
    for (const auto & heating_circuit : heating_circuits_) {
        if (heating_circuit->hc_num() == hc_num) {
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
    // search set message types
    for (uint8_t i = 0; i < monitor_typeids.size(); i++) {
        if (monitor_typeids[i] == telegram->type_id) {
            hc_num  = i + 1;
            toggle_ = true;
            break;
        }
    }

    // not found, search status message types
    if (hc_num == 0) {
        for (uint8_t i = 0; i < set_typeids.size(); i++) {
            if (set_typeids[i] == telegram->type_id) {
                hc_num = i + 1;
                break;
            }
        }
    }

    // not found, search device-id types for remote thermostats
    if (telegram->src >= 0x18 && telegram->src <= 0x1B) {
        hc_num = telegram->src - 0x17;
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

    // create a new heating circuit object
    heating_circuits_.emplace_back(new HeatingCircuit(hc_num, monitor_typeids[hc_num - 1], set_typeids[hc_num - 1]));

    std::sort(heating_circuits_.begin(), heating_circuits_.end()); // sort based on hc number

    // set the flag saying we want its data during the next auto fetch
    toggle_fetch(monitor_typeids[hc_num - 1], toggle_);
    toggle_fetch(set_typeids[hc_num - 1], toggle_);

    return heating_circuits_.back(); // even after sorting, this should still point back to the newly created HC
}

// decodes the thermostat mode for the heating circuit based on the thermostat type
// modes are off, manual, auto, day and night
uint8_t Thermostat::HeatingCircuit::get_mode(uint8_t flags) const {
    if (!Helpers::hasValue(mode)) {
        return HeatingCircuit::Mode::UNKNOWN;
    }

    flags &= 0x0F; // strip top 4 bits

    if (flags == EMSdevice::EMS_DEVICE_FLAG_RC20) {
        if (mode == 0) {
            return HeatingCircuit::Mode::OFF;
        } else if (mode == 1) {
            return HeatingCircuit::Mode::MANUAL;
        } else if (mode == 2) {
            return HeatingCircuit::Mode::AUTO;
        }
    } else if ((flags == EMSdevice::EMS_DEVICE_FLAG_RC300) || (flags == EMSdevice::EMS_DEVICE_FLAG_RC100)) {
        if (mode == 0) {
            return HeatingCircuit::Mode::MANUAL;
        } else if (mode == 1) {
            return HeatingCircuit::Mode::AUTO;
        }
    } else if (flags == EMSdevice::EMS_DEVICE_FLAG_JUNKERS) {
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
uint8_t Thermostat::HeatingCircuit::get_mode_type(uint8_t flags) const {
    flags &= 0x0F; // strip top 4 bits

    if (flags == EMS_DEVICE_FLAG_JUNKERS) {
        if (mode_type == 3) {
            return HeatingCircuit::Mode::HEAT;
        } else if (mode_type == 2) {
            return HeatingCircuit::Mode::ECO;
        } else if (mode_type == 1) {
            return HeatingCircuit::Mode::NOFROST;
        }
    } else if ((flags == EMS_DEVICE_FLAG_RC35) || (flags == EMS_DEVICE_FLAG_RC30_1)) {
        if (mode_type == 0) {
            return HeatingCircuit::Mode::NIGHT;
        } else if (mode_type == 1) {
            return HeatingCircuit::Mode::DAY;
        }
    } else if (flags == EMS_DEVICE_FLAG_RC300) {
        if (mode_type == 0) {
            return HeatingCircuit::Mode::ECO;
        } else if (mode_type == 1) {
            return HeatingCircuit::Mode::COMFORT;
        }
    } else if (flags == EMS_DEVICE_FLAG_RC100) {
        return HeatingCircuit::Mode::DAY; // no modes on these devices
    }

    return HeatingCircuit::Mode::DAY;
}

// decodes the thermostat mode based on the thermostat type
// works with both modes and mode_types
std::string Thermostat::mode_tostring(uint8_t mode) const {
    switch (mode) {
    case HeatingCircuit::Mode::OFF:
        return read_flash_string(F("off"));
        break;
    case HeatingCircuit::Mode::MANUAL:
        return read_flash_string(F("manual"));
        break;
    case HeatingCircuit::Mode::DAY:
        return read_flash_string(F("day"));
        break;
    case HeatingCircuit::Mode::NIGHT:
        return read_flash_string(F("night"));
        break;
    case HeatingCircuit::Mode::ECO:
        return read_flash_string(F("eco"));
        break;
    case HeatingCircuit::Mode::COMFORT:
        return read_flash_string(F("comfort"));
        break;
    case HeatingCircuit::Mode::HEAT:
        return read_flash_string(F("heat"));
        break;
    case HeatingCircuit::Mode::HOLIDAY:
        return read_flash_string(F("holiday"));
        break;
    case HeatingCircuit::Mode::NOFROST:
        return read_flash_string(F("nofrost"));
        break;
    case HeatingCircuit::Mode::AUTO:
        return read_flash_string(F("auto"));
        break;
    case HeatingCircuit::Mode::SUMMER:
        return read_flash_string(F("summer"));
        break;
    case HeatingCircuit::Mode::OFFSET:
        return read_flash_string(F("offset"));
        break;
    case HeatingCircuit::Mode::DESIGN:
        return read_flash_string(F("design"));
        break;
    default:
    case HeatingCircuit::Mode::UNKNOWN:
        return read_flash_string(F("unknown"));
        break;
    }
}

// display all thermostat values into the shell console
void Thermostat::show_values(uuid::console::Shell & shell) {
    EMSdevice::show_values(shell); // always call this to show header

    uint8_t flags = (this->flags() & 0x0F); // specific thermostat characteristics, strip the option bits

    if (datetime_.size()) {
        shell.printfln(F("  Clock: %s"), datetime_.c_str());
        if (Helpers::hasValue(ibaClockOffset_) && flags == EMS_DEVICE_FLAG_RC30_1) {
            print_value(shell, 2, F("Offset clock"), ibaClockOffset_, nullptr); // offset (in sec) to clock, 0xff = -1 s, 0x02 = 2 s
        }
    }
    if (Helpers::hasValue(wwMode_)) {
        if (wwMode_ == 0) {
            shell.printfln(F("  Warm Water mode: off"));
        } else if (wwMode_ == 1) {
            shell.printfln(F("  Warm Water mode: on"));
        } else if (wwMode_ == 2) {
            shell.printfln(F("  Warm Water mode: auto"));
        }
    }
    if (flags == EMS_DEVICE_FLAG_RC35) {
        print_value(shell, 2, F("Damped Outdoor temperature"), dampedoutdoortemp_, F_(degrees));
        print_value(shell, 2, F("Temp sensor 1"), tempsensor1_, F_(degrees), 10);
        print_value(shell, 2, F("Temp sensor 2"), tempsensor2_, F_(degrees), 10);
    }
    if (flags == EMS_DEVICE_FLAG_RC30_1) {
        // settings parameters
        if (Helpers::hasValue(ibaMainDisplay_)) {
            if (ibaMainDisplay_ == 0) {
                shell.printfln(F("  Display: internal temperature"));
            } else if (ibaMainDisplay_ == 1) {
                shell.printfln(F("  Display: internal setpoint"));
            } else if (ibaMainDisplay_ == 2) {
                shell.printfln(F("  Display: external temperature"));
            } else if (ibaMainDisplay_ == 3) {
                shell.printfln(F("  Display: burner temperature"));
            } else if (ibaMainDisplay_ == 4) {
                shell.printfln(F("  Display: WW temperature"));
            } else if (ibaMainDisplay_ == 5) {
                shell.printfln(F("  Display: functioning mode"));
            } else if (ibaMainDisplay_ == 6) {
                shell.printfln(F("  Display: time"));
            } else if (ibaMainDisplay_ == 7) {
                shell.printfln(F("  Display: date"));
            } else if (ibaMainDisplay_ == 9) {
                shell.printfln(F("  Display: smoke temperature"));
            }
        }

        if (Helpers::hasValue(ibaLanguage_)) {
            if (ibaLanguage_ == 0) {
                shell.printfln(F("  Language: German"));
            } else if (ibaLanguage_ == 1) {
                shell.printfln(F("  Language: Dutch"));
            } else if (ibaLanguage_ == 2) {
                shell.printfln(F("  Language: French"));
            } else if (ibaLanguage_ == 3) {
                shell.printfln(F("  Language: Italian"));
            }
        }
    }
    if (flags == EMS_DEVICE_FLAG_RC35 || flags == EMS_DEVICE_FLAG_RC30_1) {
        if (Helpers::hasValue(ibaCalIntTemperature_)) {
            print_value(shell, 2, F("Offset int. temperature"), ibaCalIntTemperature_, F_(degrees), 2);
        }

        if (Helpers::hasValue(ibaMinExtTemperature_)) {
            print_value(shell, 2, F("Min ext. temperature"), ibaMinExtTemperature_, F_(degrees)); // min ext temp for heating curve, in deg.
        }

        if (Helpers::hasValue(ibaBuildingType_)) {
            if (ibaBuildingType_ == 0) {
                shell.printfln(F("  Building: light"));
            } else if (ibaBuildingType_ == 1) {
                shell.printfln(F("  Building: medium"));
            } else if (ibaBuildingType_ == 2) {
                shell.printfln(F("  Building: heavy"));
            }
        }
    }

    // std::sort(heating_circuits_.begin(), heating_circuits_.end()); // sort based on hc number. This has moved to the heating_circuit() function

    for (const auto & hc : heating_circuits_) {
        if (!Helpers::hasValue(hc->setpoint_roomTemp)) {
            break; // skip this HC
        }
        shell.printfln(F("  Heating Circuit %d:"), hc->hc_num());

        // different thermostat types store their temperature values differently
        uint8_t format_setpoint, format_curr;
        switch (flags) {
        case EMS_DEVICE_FLAG_EASY:
            format_setpoint = 100; // *100
            format_curr     = 100; // *100
            break;
        case EMS_DEVICE_FLAG_JUNKERS:
            format_setpoint = 10; // *10
            format_curr     = 10; // *10
            break;
        default:                  // RC30, RC35 etc...
            format_setpoint = 2;  // *2
            format_curr     = 10; // *10
            break;
        }

        print_value(shell, 4, F("Current room temperature"), hc->curr_roomTemp, F_(degrees), format_curr);
        print_value(shell, 4, F("Setpoint room temperature"), hc->setpoint_roomTemp, F_(degrees), format_setpoint);
        if (Helpers::hasValue(hc->mode)) {
            print_value(shell, 4, F("Mode"), mode_tostring(hc->get_mode(flags)).c_str());
        }
        if (Helpers::hasValue(hc->mode_type)) {
            print_value(shell, 4, F("Mode Type"), mode_tostring(hc->get_mode_type(flags)).c_str());
        }

        if (Helpers::hasValue(hc->summer_mode) && hc->summer_mode) {
            shell.printfln(F("    Program is set to Summer mode"));
        } else if (Helpers::hasValue(hc->holiday_mode) && hc->holiday_mode) {
            shell.printfln(F("    Program is set to Holiday mode"));
        }

        if (Helpers::hasValue(hc->daytemp)) {
            print_value(shell, 4, F("Day temperature"), hc->daytemp, F_(degrees), 2);
        }
        if (Helpers::hasValue(hc->nighttemp)) {
            print_value(shell, 4, F("Night temperature"), hc->nighttemp, F_(degrees), 2);
        }
        if (Helpers::hasValue(hc->holidaytemp)) {
            print_value(shell, 4, F("Holiday temperature"), hc->holidaytemp, F_(degrees), 2);
        }
        if (Helpers::hasValue(hc->offsettemp)) {
            print_value(shell, 4, F("Offset temperature"), hc->offsettemp, F_(degrees), 2);
        }
        if (Helpers::hasValue(hc->designtemp)) {
            print_value(shell, 4, F("Design temperature"), hc->designtemp, F_(degrees));
        }
        if (Helpers::hasValue(hc->summertemp)) {
            print_value(shell, 4, F("Summer temperature"), hc->summertemp, F_(degrees));
        }
        if (Helpers::hasValue(hc->targetflowtemp)) {
            print_value(shell, 4, F("Target flow temperature"), hc->targetflowtemp, F_(degrees));
        }
    }
}

// 0xA8 - for reading the mode from the RC20 thermostat (0x17)
void Thermostat::process_RC20Set(std::shared_ptr<const Telegram> telegram) {
    std::shared_ptr<Thermostat::HeatingCircuit> hc = heating_circuit(telegram);

    telegram->read_value(hc->mode, 23);
}

// type 0xAE - data from the RC20 thermostat (0x17)
void Thermostat::process_RC20Monitor_2(std::shared_ptr<const Telegram> telegram) {
    std::shared_ptr<Thermostat::HeatingCircuit> hc = heating_circuit(telegram);

    telegram->read_bitvalue(hc->mode_type, 0, 7);      // day/night MSB 7th bit is day
    telegram->read_value(hc->setpoint_roomTemp, 2, 1); // is * 2, force as single byte
    telegram->read_value(hc->curr_roomTemp, 3);        // is * 10
}

// 0xAD - for reading the mode from the RC20/ES72 thermostat (0x17)
// see https://github.com/proddy/EMS-ESP/issues/334#issuecomment-611698259
void Thermostat::process_RC20Set_2(std::shared_ptr<const Telegram> telegram) {
    std::shared_ptr<Thermostat::HeatingCircuit> hc = heating_circuit(telegram);

    telegram->read_value(hc->mode, 3);
}

// 0xAF - for reading the roomtemperature from the RC20/ES72 thermostat (0x18, 0x19, ..)
void Thermostat::process_RC20Remote(std::shared_ptr<const Telegram> telegram) {
    std::shared_ptr<Thermostat::HeatingCircuit> hc = heating_circuit(telegram);
    telegram->read_value(hc->curr_roomTemp, 0);
}

// type 0xB1 - data from the RC10 thermostat (0x17)
void Thermostat::process_RC10Monitor(std::shared_ptr<const Telegram> telegram) {
    std::shared_ptr<Thermostat::HeatingCircuit> hc = heating_circuit(telegram);

    telegram->read_value(hc->setpoint_roomTemp, 1, 1); // is * 2, force as single byte
    telegram->read_value(hc->curr_roomTemp, 2);        // is * 10
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

// type 0xB0 - for reading the mode from the RC10 thermostat (0x17)
void Thermostat::process_RC10Set(std::shared_ptr<const Telegram> telegram) {
    // mode not implemented yet
}

// not sure about this one yet
// data comes in but i don't think we need to process it as its covered in the Monitor type_id?
void Thermostat::process_JunkersSet(std::shared_ptr<const Telegram> telegram) {
}

#pragma GCC diagnostic pop

// type 0xA3 - for external temp settings from the the RC* thermostats (e.g. RC35)
void Thermostat::process_RCOutdoorTemp(std::shared_ptr<const Telegram> telegram) {
    telegram->read_value(dampedoutdoortemp_, 0);
    telegram->read_value(tempsensor1_, 3); // sensor 1 - is * 10
    telegram->read_value(tempsensor2_, 5); // sensor 2 - is * 10
}

// 0x91 - data from the RC20 thermostat (0x17) - 15 bytes long
void Thermostat::process_RC20Monitor(std::shared_ptr<const Telegram> telegram) {
    std::shared_ptr<Thermostat::HeatingCircuit> hc = heating_circuit(telegram);

    telegram->read_value(hc->setpoint_roomTemp, 1, 1); // is * 2, force as single byte
    telegram->read_value(hc->curr_roomTemp, 2);        // is * 10
}

// type 0x0A - data from the Nefit Easy/TC100 thermostat (0x18) - 31 bytes long
void Thermostat::process_EasyMonitor(std::shared_ptr<const Telegram> telegram) {
    std::shared_ptr<Thermostat::HeatingCircuit> hc = heating_circuit(telegram);

    telegram->read_value(hc->curr_roomTemp, 8);      // is * 100
    telegram->read_value(hc->setpoint_roomTemp, 10); // is * 100
}

// Settings Parameters - 0xA5 - RC30_1
void Thermostat::process_IBASettings(std::shared_ptr<const Telegram> telegram) {
    // 22 - display line on RC35
    telegram->read_value(ibaMainDisplay_,
                         0); // display on Thermostat: 0 int. temp, 1 int. setpoint, 2 ext. temp., 3 burner temp., 4 ww temp, 5 functioning mode, 6 time, 7 data, 9 smoke temp
    telegram->read_value(ibaLanguage_, 1);          // language on Thermostat: 0 german, 1 dutch, 2 french, 3 italian
    telegram->read_value(ibaCalIntTemperature_, 2); // offset int. temperature sensor, by * 0.1 Kelvin
    telegram->read_value(ibaBuildingType_, 6);      // building type: 0 = light, 1 = medium, 2 = heavy
    telegram->read_value(ibaMinExtTemperature_, 5); // min ext temp for heating curve, in deg., 0xF6=-10, 0x0 = 0, 0xFF=-1
    telegram->read_value(ibaClockOffset_, 12);      // offset (in sec) to clock, 0xff = -1 s, 0x02 = 2 s
}

// Settings WW 0x37 - RC35
void Thermostat::process_RC35wwSettings(std::shared_ptr<const Telegram> telegram) {
    telegram->read_value(wwMode_, 2); // 0 off, 1-on, 2-auto
}

// type 0x6F - FR10/FR50/FR100 Junkers
void Thermostat::process_JunkersMonitor(std::shared_ptr<const Telegram> telegram) {
    // ignore single byte telegram messages
    if (telegram->message_length <= 1) {
        return;
    }

    std::shared_ptr<Thermostat::HeatingCircuit> hc = heating_circuit(telegram);

    telegram->read_value(hc->curr_roomTemp, 4);     // value is * 10
    telegram->read_value(hc->setpoint_roomTemp, 2); // value is * 10

    telegram->read_value(hc->mode_type, 0); // 1 = nofrost, 2 = eco, 3 = heat
    telegram->read_value(hc->mode, 1);      // 1 = manual, 2 = auto
}

// type 0x02A5 - data from the Nefit RC1010/3000 thermostat (0x18) and RC300/310s on 0x10
void Thermostat::process_RC300Monitor(std::shared_ptr<const Telegram> telegram) {
    std::shared_ptr<Thermostat::HeatingCircuit> hc = heating_circuit(telegram);

    telegram->read_value(hc->curr_roomTemp, 0); // is * 10

    telegram->read_bitvalue(hc->mode_type, 10, 1);
    telegram->read_bitvalue(hc->mode, 10, 0); // bit 1, mode (auto=1 or manual=0)

    // if manual, take the current setpoint temp at pos 6
    // if auto, take the next setpoint temp at pos 7
    // pos 3 is the current target temp and sometimes can be 0
    // see https://github.com/proddy/EMS-ESP/issues/256#issuecomment-585171426
    // pos 3 actual setpoint (optimized), i.e. changes with temporary change, summer/holiday-modes
    // pos 6 actual setpoint according to programmed changes eco/comfort
    // pos 7 next setpoint in the future, time to next setpoint in pos 8/9
    telegram->read_value(hc->setpoint_roomTemp, 3, 1); // is * 2, force as single byte
    telegram->read_bitvalue(hc->summer_mode, 2, 4);
    telegram->read_value(hc->targetflowtemp, 4);
}

// type 0x02B9 EMS+ for reading from RC300/RC310 thermostat
void Thermostat::process_RC300Set(std::shared_ptr<const Telegram> telegram) {
    std::shared_ptr<Thermostat::HeatingCircuit> hc = heating_circuit(telegram);

    // NOTE when setting the room temp we pick from two values, hopefully one is correct!
    // manual is position 10
    // comfort is position 2
    // I think auto is position 8?
    // actual setpoint taken from RC300Monitor (Michael 12.06.2020)
    // telegram->read_value(hc->setpoint_roomTemp, 8, 1);  // single byte conversion, value is * 2 - auto?
    // telegram->read_value(hc->setpoint_roomTemp, 10, 1); // single byte conversion, value is * 2 - manual

    // check why mode is both in the Monitor and Set for the RC300. It'll be read twice!
    // telegram->read_value(hc->mode, 0); // Auto = xFF, Manual = x00 eg. 10 00 FF 08 01 B9 FF

    telegram->read_value(hc->daytemp, 2);   // is * 2
    telegram->read_value(hc->nighttemp, 4); // is * 2
}

// types 0x31D and 0x31E
void Thermostat::process_RC300WWmode(std::shared_ptr<const Telegram> telegram) {
    // 0x31D for WW system 1, 0x31E for WW system 2
    wwSystem_ = telegram->type_id - 0x31D + 1;
    telegram->read_value(wwExtra_, 0); // 0=no, 1=yes
    // pos 1 = holiday mode
    // pos 2 = current status of DHW setpoint
    // pos 3 = current status of DHW circulation pump
}

// type 0x41 - data from the RC30 thermostat(0x10) - 14 bytes long
void Thermostat::process_RC30Monitor(std::shared_ptr<const Telegram> telegram) {
    std::shared_ptr<Thermostat::HeatingCircuit> hc = heating_circuit(telegram);

    telegram->read_value(hc->setpoint_roomTemp, 1, 1); // is * 2, force as single byte
    telegram->read_value(hc->curr_roomTemp, 2);
}

// type 0xA7 - for reading the mode from the RC30 thermostat (0x10)
void Thermostat::process_RC30Set(std::shared_ptr<const Telegram> telegram) {
    std::shared_ptr<Thermostat::HeatingCircuit> hc = heating_circuit(telegram);

    telegram->read_value(hc->mode, 23);
}

// type 0x3E (HC1), 0x48 (HC2), 0x52 (HC3), 0x5C (HC4) - data from the RC35 thermostat (0x10) - 16 bytes
void Thermostat::process_RC35Monitor(std::shared_ptr<const Telegram> telegram) {
    // exit if the 15th byte (second from last) is 0x00, which I think is calculated flow setpoint temperature
    // with weather controlled RC35s this value is >=5, otherwise can be zero and our setpoint temps will be incorrect
    // see https://github.com/proddy/EMS-ESP/issues/373#issuecomment-627907301
    if (telegram->message_data[14] == 0x00) {
        return;
    }

    std::shared_ptr<Thermostat::HeatingCircuit> hc = heating_circuit(telegram);

    telegram->read_value(hc->setpoint_roomTemp, 2, 1); // is * 2, force to single byte, is 0 in summermode
    telegram->read_value(hc->curr_roomTemp, 3);        // is * 10 - or 0x7D00 if thermostat is mounted on boiler

    telegram->read_bitvalue(hc->mode_type, 1, 1);
    telegram->read_bitvalue(hc->summer_mode, 1, 0);
    telegram->read_bitvalue(hc->holiday_mode, 0, 5);

    telegram->read_value(hc->targetflowtemp, 14);
}

// type 0x3D (HC1), 0x47 (HC2), 0x51 (HC3), 0x5B (HC4) - Working Mode Heating - for reading the mode from the RC35 thermostat (0x10)
void Thermostat::process_RC35Set(std::shared_ptr<const Telegram> telegram) {
    // check to see we have a valid type
    // heating: 1 radiator, 2 convectors, 3 floors, 4 room supply
    if (telegram->message_data[0] == 0x00) {
        return;
    }

    std::shared_ptr<Thermostat::HeatingCircuit> hc = heating_circuit(telegram);

    telegram->read_value(hc->mode, 7);        // night, day, auto
    telegram->read_value(hc->daytemp, 2);     // is * 2
    telegram->read_value(hc->nighttemp, 1);   // is * 2
    telegram->read_value(hc->holidaytemp, 3); // is * 2
    telegram->read_value(hc->heatingtype, 0); // 0- off, 1-radiator, 2-convector, 3-floor

    telegram->read_value(hc->summertemp, 22);  // is * 1
    telegram->read_value(hc->nofrosttemp, 23); // is * 1
    telegram->read_value(hc->designtemp, 17);  // is * 1
    telegram->read_value(hc->offsettemp, 6);   // is * 2
}

// process_RCTime - type 0x06 - date and time from a thermostat - 14 bytes long
void Thermostat::process_RCTime(std::shared_ptr<const Telegram> telegram) {
    if (flags() == EMS_DEVICE_FLAG_EASY) {
        return; // not supported
    }

    if (datetime_.empty()) {
        datetime_.resize(25, '\0');
    }

    // render time to HH:MM:SS DD/MM/YYYY
    // had to create separate buffers because of how printf works
    char buf1[6];
    char buf2[6];
    char buf3[6];
    char buf4[6];
    char buf5[6];
    char buf6[6];
    snprintf_P(&datetime_[0],
               datetime_.capacity() + 1,
               PSTR("%s:%s:%s %s/%s/%s"),
               Helpers::smallitoa(buf1, telegram->message_data[2]),      // hour
               Helpers::smallitoa(buf2, telegram->message_data[4]),      // minute
               Helpers::smallitoa(buf3, telegram->message_data[5]),      // second
               Helpers::smallitoa(buf4, telegram->message_data[3]),      // day
               Helpers::smallitoa(buf5, telegram->message_data[1]),      // month
               Helpers::itoa(buf6, telegram->message_data[0] + 2000, 10) // year
    );
}

// 0xA5 - Set minimum external temperature
void Thermostat::set_settings_minexttemp(const int8_t mt) {
    if (((flags() & 0x0F) == EMS_DEVICE_FLAG_RC30_1) || ((flags() & 0x0F) == EMS_DEVICE_FLAG_RC35)) {
        LOG_INFO(F("Setting min external temperature to %d"), mt);
        write_command(EMS_TYPE_IBASettings, 5, mt);
    }
}

// 0xA5 - Clock offset
void Thermostat::set_settings_clockoffset(const int8_t co) {
    if ((flags() & 0x0F) == EMS_DEVICE_FLAG_RC30_1) {
        LOG_INFO(F("Setting clock offset to %d"), co);
        write_command(EMS_TYPE_IBASettings, 12, co);
    }
}
// 0xA5 - Calibrate internal temperature
void Thermostat::set_settings_calinttemp(const int8_t ct) {
    if (((flags() & 0x0F) == EMS_DEVICE_FLAG_RC30_1) || ((flags() & 0x0F) == EMS_DEVICE_FLAG_RC35)) {
        LOG_INFO(F("Calibrating internal temperature to %d.%d"), ct / 10, ct < 0 ? -ct % 10 : ct % 10);
        write_command(EMS_TYPE_IBASettings, 2, ct);
    }
}

// 0xA5 - Set the display settings
void Thermostat::set_settings_display(const uint8_t ds) {
    if ((flags() & 0x0F) == EMS_DEVICE_FLAG_RC30_1) {
        LOG_INFO(F("Setting display to %d"), ds);
        write_command(EMS_TYPE_IBASettings, 0, ds);
    }
}

// 0xA5 - Set the building settings
void Thermostat::set_settings_building(const uint8_t bg) {
    if (((flags() & 0x0F) == EMS_DEVICE_FLAG_RC30_1) || ((flags() & 0x0F) == EMS_DEVICE_FLAG_RC35)) {
        LOG_INFO(F("Setting building to %d"), bg);
        write_command(EMS_TYPE_IBASettings, 6, bg);
    }
}

// 0xA5 Set the language settings
void Thermostat::set_settings_language(const uint8_t lg) {
    if ((flags() & 0x0F) == EMS_DEVICE_FLAG_RC30_1) {
        LOG_INFO(F("Setting language to %d"), lg);
        write_command(EMS_TYPE_IBASettings, 1, lg);
    }
}

// Set the control-mode for hc 0-off, 1-RC20, 2-RC3x
void Thermostat::set_control(const uint8_t ctrl, const uint8_t hc_num) {
    std::shared_ptr<Thermostat::HeatingCircuit> hc = heating_circuit(hc_num);
    if (hc == nullptr) {
        LOG_WARNING(F("Set control: Heating Circuit %d not found or activated"), hc_num);
        return;
    }
    if (ctrl > 2) {
        LOG_WARNING(F("Set control: Invalid control mode: %d"), ctrl);
        return;
    }
    if ((flags() & 0x0F) == EMS_DEVICE_FLAG_RC35 || (flags() & 0x0F) == EMS_DEVICE_FLAG_RC30_1) {
        LOG_INFO(F("Setting circuit-control for hc%d to %d"), hc_num, ctrl);
        write_command(set_typeids[hc->hc_num() - 1], 26, ctrl);
    } else {
        LOG_INFO(F("Set circuit-control not supported"));
    }
}

// sets the thermostat ww working mode, where mode is a string
void Thermostat::set_ww_mode(const std::string & mode) {
    if (strcasecmp("off", mode.c_str()) == 0) {
        LOG_INFO(F("Setting thermostat warm water mode to %s"), mode.c_str());
        write_command(EMS_TYPE_wwSettings, 2, 0);
    } else if (strcasecmp("on", mode.c_str()) == 0) {
        LOG_INFO(F("Setting thermostat warm water mode to %s"), mode.c_str());
        write_command(EMS_TYPE_wwSettings, 2, 1);
    } else if (strcasecmp("auto", mode.c_str()) == 0) {
        LOG_INFO(F("Setting thermostat warm water mode to %s"), mode.c_str());
        write_command(EMS_TYPE_wwSettings, 2, 2);
    } else {
        LOG_WARNING(F("Set thermostat warm water mode: Invalid mode: %s"), mode.c_str());
    }
}

// set the holiday as string dd.mm.yyyy-dd.mm.yyyy
void Thermostat::set_holiday(const char * hd, const uint8_t hc_num) {
    std::shared_ptr<Thermostat::HeatingCircuit> hc = heating_circuit(hc_num);
    if (hc == nullptr) {
        LOG_WARNING(F("Set holiday: Heating Circuit %d not found or activated for device ID 0x%02X"), hc_num, device_id());
        return;
    }
    uint8_t data[6];
    data[0] = (hd[0] - '0') * 10 + (hd[1] - '0');
    data[1] = (hd[3] - '0') * 10 + (hd[4] - '0');
    data[2] = (hd[7] - '0') * 100 + (hd[8] - '0') * 10 + (hd[9] - '0');
    data[3] = (hd[11] - '0') * 10 + (hd[11] - '0');
    data[4] = (hd[14] - '0') * 10 + (hd[15] - '0');
    data[5] = (hd[18] - '0') * 100 + (hd[19] - '0') * 10 + (hd[20] - '0');
    if ((flags() & 0x0F) == EMS_DEVICE_FLAG_RC35 || (flags() & 0x0F) == EMS_DEVICE_FLAG_RC30_1) {
        LOG_INFO(F("Setting holiday for hc %d"), hc->hc_num());
        write_command(timer_typeids[hc->hc_num() - 1], 87, data, 6, 0);
    } else {
        LOG_INFO(F("Set holiday not supported"));
    }
}

// set pause in hours
void Thermostat::set_pause(const uint8_t hrs, const uint8_t hc_num) {
    std::shared_ptr<Thermostat::HeatingCircuit> hc = heating_circuit(hc_num);
    if (hc == nullptr) {
        LOG_WARNING(F("Set pause: Heating Circuit %d not found or activated for device ID 0x%02X"), hc_num, device_id());
        return;
    }
    if ((flags() & 0x0F) == EMS_DEVICE_FLAG_RC35 || (flags() & 0x0F) == EMS_DEVICE_FLAG_RC30_1) {
        LOG_INFO(F("Setting pause: %d hours, hc: %d"), hrs, hc->hc_num());
        write_command(timer_typeids[hc->hc_num() - 1], 85, hrs);
    } else {
        LOG_INFO(F("Set pause not supported"));
    }
}

// set partymode in hours
void Thermostat::set_party(const uint8_t hrs, const uint8_t hc_num) {
    std::shared_ptr<Thermostat::HeatingCircuit> hc = heating_circuit(hc_num);
    if (hc == nullptr) {
        LOG_WARNING(F("Set party: Heating Circuit %d not found or activated for device ID 0x%02X"), hc_num, device_id());
        return;
    }
    if ((flags() & 0x0F) == EMS_DEVICE_FLAG_RC35 || (flags() & 0x0F) == EMS_DEVICE_FLAG_RC30_1) {
        LOG_INFO(F("Setting party: %d hours, hc: %d"), hrs, hc->hc_num());
        write_command(timer_typeids[hc->hc_num() - 1], 86, hrs);
    } else {
        LOG_INFO(F("Set party not supported"));
    }
}

// set date&time as string hh:mm:ss-dd.mm.yyyy-dw-dst
// dw - day of week (0..6), dst- summertime (0/1)
void Thermostat::set_datetime(const char * dt) {
    uint8_t data[9];
    data[0] = (dt[16] - '0') * 100 + (dt[17] - '0') * 10 + (dt[18] - '0'); // year
    data[1] = (dt[12] - '0') * 10 + (dt[13] - '0');                        // month
    data[2] = (dt[0] - '0') * 10 + (dt[1] - '0');                          // hour
    data[3] = (dt[9] - '0') * 10 + (dt[10] - '0');                         // day
    data[4] = (dt[3] - '0') * 10 + (dt[4] - '0');                          // min
    data[5] = (dt[6] - '0') * 10 + (dt[7] - '0');                          // sec
    data[6] = (dt[20] - '0');                                              // day of week
    data[7] = (dt[22] - '0');                                              // summerime
    if ((flags() & 0x0F) == EMS_DEVICE_FLAG_RC35 || (flags() & 0x0F) == EMS_DEVICE_FLAG_RC30_1) {
        LOG_INFO(F("Setting date and time"));
        write_command(6, 0, data, 8, 0);
    } else {
        LOG_INFO(F("Set date not supported"));
    }
}

// sets the thermostat working mode, where mode is a string
void Thermostat::set_mode(const std::string & mode, const uint8_t hc_num) {
    if (mode_tostring(HeatingCircuit::Mode::OFF) == mode) {
        set_mode(HeatingCircuit::Mode::OFF, hc_num);
    } else if (mode_tostring(HeatingCircuit::Mode::MANUAL) == mode) {
        set_mode(HeatingCircuit::Mode::MANUAL, hc_num);
    } else if (mode_tostring(HeatingCircuit::Mode::AUTO) == mode) {
        set_mode(HeatingCircuit::Mode::AUTO, hc_num);
    } else if (mode_tostring(HeatingCircuit::Mode::DAY) == mode) {
        set_mode(HeatingCircuit::Mode::DAY, hc_num);
    } else if (mode_tostring(HeatingCircuit::Mode::NIGHT) == mode) {
        set_mode(HeatingCircuit::Mode::NIGHT, hc_num);
    } else if (mode_tostring(HeatingCircuit::Mode::HEAT) == mode) {
        set_mode(HeatingCircuit::Mode::HEAT, hc_num);
    } else if (mode_tostring(HeatingCircuit::Mode::NOFROST) == mode) {
        set_mode(HeatingCircuit::Mode::NOFROST, hc_num);
    } else if (mode_tostring(HeatingCircuit::Mode::ECO) == mode) {
        set_mode(HeatingCircuit::Mode::ECO, hc_num);
    } else if (mode_tostring(HeatingCircuit::Mode::HOLIDAY) == mode) {
        set_mode(HeatingCircuit::Mode::HOLIDAY, hc_num);
    } else if (mode_tostring(HeatingCircuit::Mode::COMFORT) == mode) {
        set_mode(HeatingCircuit::Mode::COMFORT, hc_num);
    } else {
        LOG_WARNING(F("Invalid mode %s. Cannot set"), mode.c_str());
    }
}

// Set the thermostat working mode
void Thermostat::set_mode(const uint8_t mode, const uint8_t hc_num) {
    if (can_write()) {
        LOG_WARNING(F("Write not supported for this model Thermostat"));
        return;
    }

    // get hc based on number
    std::shared_ptr<Thermostat::HeatingCircuit> hc = heating_circuit(hc_num);
    if (hc == nullptr) {
        LOG_WARNING(F("set mode: Heating Circuit %d not found or activated"), hc_num);
        return;
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
    // case HeatingCircuit::Mode::HOLIDAY:
    case HeatingCircuit::Mode::ECO:
        set_mode_value = 2;
        break;
    }

    switch (flags() & 0x0F) {
    case EMSdevice::EMS_DEVICE_FLAG_RC20:
        offset          = EMS_OFFSET_RC20Set_mode;
        validate_typeid = set_typeids[hc_p];
        break;
    case EMSdevice::EMS_DEVICE_FLAG_RC20_2: // ES72
        offset          = EMS_OFFSET_RC20_2_Set_mode;
        validate_typeid = set_typeids[hc_p];
        break;
    case EMSdevice::EMS_DEVICE_FLAG_RC30:
        offset          = EMS_OFFSET_RC30Set_mode;
        validate_typeid = set_typeids[hc_p];
        break;
    case EMSdevice::EMS_DEVICE_FLAG_RC35:
    case EMSdevice::EMS_DEVICE_FLAG_RC30_1:
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
        if ((flags() & EMS_DEVICE_FLAG_JUNKERS_2) == EMS_DEVICE_FLAG_JUNKERS_2) {
            offset = EMS_OFFSET_JunkersSetMessage2_set_mode;
        } else {
            offset = EMS_OFFSET_JunkersSetMessage_set_mode;
        }
        validate_typeid = monitor_typeids[hc_p];
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
}

// sets the thermostat temp, where mode is a string
void Thermostat::set_temperature(const float temperature, const std::string & mode, const uint8_t hc_num) {
    if (mode_tostring(HeatingCircuit::Mode::MANUAL) == mode) {
        set_temperature(temperature, HeatingCircuit::Mode::MANUAL, hc_num);
    } else if (mode_tostring(HeatingCircuit::Mode::AUTO) == mode) {
        set_temperature(temperature, HeatingCircuit::Mode::AUTO, hc_num);
    } else if (mode_tostring(HeatingCircuit::Mode::DAY) == mode) {
        set_temperature(temperature, HeatingCircuit::Mode::DAY, hc_num);
    } else if (mode_tostring(HeatingCircuit::Mode::NIGHT) == mode) {
        set_temperature(temperature, HeatingCircuit::Mode::NIGHT, hc_num);
    } else if (mode_tostring(HeatingCircuit::Mode::COMFORT) == mode) {
        set_temperature(temperature, HeatingCircuit::Mode::COMFORT, hc_num);
    } else if (mode_tostring(HeatingCircuit::Mode::HEAT) == mode) {
        set_temperature(temperature, HeatingCircuit::Mode::HEAT, hc_num);
    } else if (mode_tostring(HeatingCircuit::Mode::NOFROST) == mode) {
        set_temperature(temperature, HeatingCircuit::Mode::NOFROST, hc_num);
    } else if (mode_tostring(HeatingCircuit::Mode::SUMMER) == mode) {
        set_temperature(temperature, HeatingCircuit::Mode::SUMMER, hc_num);
    } else if (mode_tostring(HeatingCircuit::Mode::HOLIDAY) == mode) {
        set_temperature(temperature, HeatingCircuit::Mode::HOLIDAY, hc_num);
    } else if (mode_tostring(HeatingCircuit::Mode::OFFSET) == mode) {
        set_temperature(temperature, HeatingCircuit::Mode::OFFSET, hc_num);
    } else if (mode_tostring(HeatingCircuit::Mode::DESIGN) == mode) {
        set_temperature(temperature, HeatingCircuit::Mode::DESIGN, hc_num);
    } else {
        LOG_WARNING(F("Invalid mode %s."), mode.c_str());
    }
}

// Set the temperature of the thermostat
void Thermostat::set_temperature(const float temperature, const uint8_t mode, const uint8_t hc_num) {
    if (can_write()) {
        LOG_WARNING(F("Write not supported for this model Thermostat"));
        return;
    }

    // get hc based on number
    std::shared_ptr<Thermostat::HeatingCircuit> hc = heating_circuit(hc_num);
    if (hc == nullptr) {
        LOG_WARNING(F("Set temperature: Heating Circuit %d not found or activated for device ID 0x%02X"), hc_num, device_id());
        return;
    }

    uint8_t  model           = flags() & 0x0F;
    int8_t   offset          = -1; // we use -1 to check if there is a value
    uint8_t  factor          = 2;  // some temperatures only use 1
    uint16_t validate_typeid = monitor_typeids[hc->hc_num() - 1];

    if (model == EMS_DEVICE_FLAG_RC10) {
        offset = EMS_OFFSET_RC10Set_temp;

    } else if (model == EMS_DEVICE_FLAG_RC20) {
        offset = EMS_OFFSET_RC20Set_temp;

    } else if (model == EMS_DEVICE_FLAG_RC30) {
        offset = EMS_OFFSET_RC30Set_temp;

    } else if ((model == EMS_DEVICE_FLAG_RC300) || (model == EMS_DEVICE_FLAG_RC100)) {
        validate_typeid = set_typeids[hc->hc_num() - 1];
        if (mode == HeatingCircuit::Mode::AUTO) {
            offset = 0x08; // auto offset
        } else if (mode == HeatingCircuit::Mode::MANUAL) {
            offset = 0x0A; // manual offset
        } else if (mode == HeatingCircuit::Mode::COMFORT) {
            offset = 0x02; // comfort offset
        }

    } else if (model == EMS_DEVICE_FLAG_RC20_2) {
        switch (mode) {
        case HeatingCircuit::Mode::NIGHT: // change the night temp
            offset = EMS_OFFSET_RC20_2_Set_temp_night;
            break;
        case HeatingCircuit::Mode::DAY: // change the day temp
            offset = EMS_OFFSET_RC20_2_Set_temp_day;
            break;
        }

    } else if ((model == EMS_DEVICE_FLAG_RC35) || (model == EMS_DEVICE_FLAG_RC30_1)) {
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
        case HeatingCircuit::Mode::DESIGN:
            offset = EMS_OFFSET_RC35Set_temp_design;
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
        default:
        case HeatingCircuit::Mode::AUTO: // automatic selection, if no type is defined, we use the standard code
            if (model == EMS_DEVICE_FLAG_RC35) {
                uint8_t mode_ = hc->get_mode(flags());
                if (mode_ == HeatingCircuit::Mode::NIGHT) {
                    offset = EMS_OFFSET_RC35Set_temp_night;
                } else if (mode_ == HeatingCircuit::Mode::DAY) {
                    offset = EMS_OFFSET_RC35Set_temp_day;
                } else {
                    offset = EMS_OFFSET_RC35Set_seltemp; // https://github.com/proddy/EMS-ESP/issues/310
                }
            } else {
                uint8_t mode_type = hc->get_mode_type(flags());
                offset            = (mode_type == HeatingCircuit::Mode::NIGHT) ? EMS_OFFSET_RC35Set_temp_night : EMS_OFFSET_RC35Set_temp_day;
            }
            break;
        }

    } else if (model == EMS_DEVICE_FLAG_JUNKERS) {
        // figure out if we have older or new thermostats, Heating Circuits on 0x65 or 0x79
        // see https://github.com/proddy/EMS-ESP/issues/335#issuecomment-593324716)
        bool old_junkers = ((flags() & EMS_DEVICE_FLAG_JUNKERS_2) == EMS_DEVICE_FLAG_JUNKERS_2);
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
                uint8_t mode_type = hc->get_mode_type(flags());
                offset = (mode_type == HeatingCircuit::Mode::NIGHT || mode_type == HeatingCircuit::Mode::ECO) ? EMS_OFFSET_JunkersSetMessage_night_temp
                                                                                                              : EMS_OFFSET_JunkersSetMessage_day_temp;
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
            default:
            case HeatingCircuit::Mode::HEAT:
            case HeatingCircuit::Mode::DAY:
                offset = EMS_OFFSET_JunkersSetMessage3_heat;
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

        // add the write command to the Tx queue
        // value is *2
        // post validate is the corresponding monitor or set type IDs as they can differ per model
        write_command(set_typeids[hc->hc_num() - 1], offset, (uint8_t)((float)temperature * (float)factor), validate_typeid);
    }
}

// add console commands
void Thermostat::console_commands(Shell & shell, unsigned int context) {
    EMSESPShell::commands->add_command(ShellContext::THERMOSTAT,
                                       CommandFlags::ADMIN,
                                       flash_string_vector{F_(set), F_(master)},
                                       flash_string_vector{F_(deviceid_optional)},
                                       [](Shell & shell, const std::vector<std::string> & arguments) {
                                           uint8_t value;
                                           if (arguments.empty()) {
                                               value = EMSESP_DEFAULT_MASTER_THERMOSTAT;
                                           } else {
                                               value = Helpers::hextoint(arguments.front().c_str());
                                           }

                                           EMSESP::emsespSettingsService.update(
                                               [&](EMSESPSettings & settings) {
                                                   settings.master_thermostat = value;
                                                   EMSESP::actual_master_thermostat(value); // set the internal value too
                                                   char buffer[5];
                                                   shell.printfln(F_(master_thermostat_fmt),
                                                                  !value ? uuid::read_flash_string(F_(auto)).c_str() : Helpers::hextoa(buffer, value));
                                                   return StateUpdateResult::CHANGED;
                                               },
                                               "local");
                                       });

    EMSESPShell::commands->add_command(ShellContext::THERMOSTAT,
                                       CommandFlags::ADMIN,
                                       flash_string_vector{F_(read)},
                                       flash_string_vector{F_(typeid_mandatory)},
                                       [=](Shell & shell __attribute__((unused)), const std::vector<std::string> & arguments) {
                                           uint16_t type_id = Helpers::hextoint(arguments.front().c_str());
                                           EMSESP::send_read_request(type_id, device_id());
                                       });

    EMSESPShell::commands->add_command(ShellContext::THERMOSTAT,
                                       CommandFlags::ADMIN,
                                       flash_string_vector{F_(temp)},
                                       flash_string_vector{F_(degrees_mandatory), F_(hc_optional), F_(mode_optional)},
                                       [=](Shell & shell __attribute__((unused)), const std::vector<std::string> & arguments) {
                                           uint8_t hc = (arguments.size() >= 2) ? arguments[1].at(0) - '0' : AUTO_HEATING_CIRCUIT;
                                           if ((arguments.size() == 3)) {
                                               set_temperature(atof(arguments.front().c_str()), arguments.back().c_str(), hc);
                                           } else if (arguments[1].at(0) >= 'A') {
                                               set_temperature(atof(arguments.front().c_str()), arguments.back().c_str(), AUTO_HEATING_CIRCUIT);
                                           } else {
                                               set_temperature(atof(arguments.front().c_str()), HeatingCircuit::Mode::AUTO, hc);
                                           }
                                       });

    EMSESPShell::commands->add_command(
        ShellContext::THERMOSTAT,
        CommandFlags::ADMIN,
        flash_string_vector{F_(mode)},
        flash_string_vector{F_(mode_mandatory), F_(hc_optional)},
        [=](Shell & shell __attribute__((unused)), const std::vector<std::string> & arguments) {
            uint8_t hc = (arguments.size() == 2) ? arguments[1].at(0) - '0' : AUTO_HEATING_CIRCUIT;
            set_mode(arguments.front(), hc);
        },
        [](Shell & shell __attribute__((unused)), const std::vector<std::string> & arguments __attribute__((unused))) -> const std::vector<std::string> {
            return std::vector<std::string>{read_flash_string(F("off")),
                                            read_flash_string(F("manual")),
                                            read_flash_string(F("day")),
                                            read_flash_string(F("night")),
                                            read_flash_string(F("eco")),
                                            read_flash_string(F("comfort")),
                                            read_flash_string(F("heat")),
                                            // read_flash_string(F("holiday")),
                                            read_flash_string(F("nofrost")),
                                            read_flash_string(F("auto"))

            };
        });

    EMSESPShell::commands->add_command(
        ShellContext::THERMOSTAT,
        CommandFlags::ADMIN,
        flash_string_vector{F_(wwmode)},
        flash_string_vector{F_(mode_mandatory)},
        [=](Shell & shell __attribute__((unused)), const std::vector<std::string> & arguments) { set_ww_mode(arguments.front()); },
        [](Shell & shell __attribute__((unused)), const std::vector<std::string> & arguments __attribute__((unused))) -> const std::vector<std::string> {
            return std::vector<std::string>{read_flash_string(F("off")), read_flash_string(F("on")), read_flash_string(F("auto"))};
        });

    EMSESPShell::commands->add_command(ShellContext::THERMOSTAT,
                                       CommandFlags::USER,
                                       flash_string_vector{F_(show)},
                                       [&](Shell & shell, const std::vector<std::string> & arguments __attribute__((unused))) { show_values(shell); });

    EMSESPShell::commands->add_command(ShellContext::THERMOSTAT,
                                       CommandFlags::USER,
                                       flash_string_vector{F_(set)},
                                       [](Shell & shell, const std::vector<std::string> & arguments __attribute__((unused))) {
                                           EMSESP::emsespSettingsService.read([&](EMSESPSettings & settings) {
                                               char buffer[4];
                                               shell.printfln(F_(master_thermostat_fmt),
                                                              settings.master_thermostat == 0 ? uuid::read_flash_string(F_(auto)).c_str()
                                                                                              : Helpers::hextoa(buffer, settings.master_thermostat));
                                               shell.println();
                                           });
                                       });

    // enter the context
    Console::enter_custom_context(shell, context);
}

} // namespace emsesp