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

#include "boiler.h"

MAKE_PSTR_WORD(boiler)
MAKE_PSTR_WORD(wwtemp)
MAKE_PSTR_WORD(flowtemp)
MAKE_PSTR_WORD(wwactive)
MAKE_PSTR_WORD(wwonetime)
MAKE_PSTR_WORD(wwcirculation)
MAKE_PSTR_WORD(comfort)
MAKE_PSTR_WORD(eco)
MAKE_PSTR_WORD(intelligent)
MAKE_PSTR_WORD(hot)

MAKE_PSTR(comfort_mandatory, "<hot | eco | intelligent>")

// shower
MAKE_PSTR_WORD(shower)
MAKE_PSTR_WORD(timer)
MAKE_PSTR_WORD(alert)
MAKE_PSTR(shower_timer_fmt, "Shower Timer is %s")
MAKE_PSTR(shower_alert_fmt, "Shower Alert is %s")

namespace emsesp {

REGISTER_FACTORY(Boiler, EMSdevice::DeviceType::BOILER)
MAKE_PSTR(logger_name, "boiler")
uuid::log::Logger Boiler::logger_{F_(logger_name), uuid::log::Facility::CONSOLE};

Boiler::Boiler(uint8_t device_type, int8_t device_id, uint8_t product_id, const std::string & version, const std::string & name, uint8_t flags, uint8_t brand)
    : EMSdevice(device_type, device_id, product_id, version, name, flags, brand) {
    LOG_DEBUG(F("Registering new Boiler with device ID 0x%02X"), device_id);

    // the telegram handlers...
    register_telegram_type(0x10, F("UBAErrorMessage1"), false, std::bind(&Boiler::process_UBAErrorMessage, this, _1));
    register_telegram_type(0x11, F("UBAErrorMessage2"), false, std::bind(&Boiler::process_UBAErrorMessage, this, _1));
    register_telegram_type(0x18, F("UBAMonitorFast"), false, std::bind(&Boiler::process_UBAMonitorFast, this, _1));
    register_telegram_type(0x19, F("UBAMonitorSlow"), true, std::bind(&Boiler::process_UBAMonitorSlow, this, _1));
    register_telegram_type(0x34, F("UBAMonitorWW"), false, std::bind(&Boiler::process_UBAMonitorWW, this, _1));
    register_telegram_type(0x1C, F("UBAMaintenanceStatus"), false, std::bind(&Boiler::process_UBAMaintenanceStatus, this, _1));
    register_telegram_type(0x2A, F("MC10Status"), false, std::bind(&Boiler::process_MC10Status, this, _1));
    register_telegram_type(0x33, F("UBAParameterWW"), true, std::bind(&Boiler::process_UBAParameterWW, this, _1));
    register_telegram_type(0x14, F("UBATotalUptime"), false, std::bind(&Boiler::process_UBATotalUptime, this, _1));
    register_telegram_type(0x35, F("UBAFlags"), false, std::bind(&Boiler::process_UBAFlags, this, _1));
    register_telegram_type(0x15, F("UBAMaintenanceSettings"), false, std::bind(&Boiler::process_UBAMaintenanceSettings, this, _1));
    register_telegram_type(0x16, F("UBAParameters"), true, std::bind(&Boiler::process_UBAParameters, this, _1));
    register_telegram_type(0x1A, F("UBASetPoints"), false, std::bind(&Boiler::process_UBASetPoints, this, _1));
    register_telegram_type(0xD1, F("UBAOutdoorTemp"), false, std::bind(&Boiler::process_UBAOutdoorTemp, this, _1));
    register_telegram_type(0xE3, F("UBAMonitorSlowPlus"), false, std::bind(&Boiler::process_UBAMonitorSlowPlus2, this, _1));
    register_telegram_type(0xE4, F("UBAMonitorFastPlus"), false, std::bind(&Boiler::process_UBAMonitorFastPlus, this, _1));
    register_telegram_type(0xE5, F("UBAMonitorSlowPlus"), false, std::bind(&Boiler::process_UBAMonitorSlowPlus, this, _1));

    register_telegram_type(0xE9, F("UBADHWStatus"), false, std::bind(&Boiler::process_UBADHWStatus, this, _1));

    // MQTT callbacks
    register_mqtt_topic("boiler_cmd", std::bind(&Boiler::boiler_cmd, this, _1));
    register_mqtt_topic("boiler_cmd_wwactivated", std::bind(&Boiler::boiler_cmd_wwactivated, this, _1));
    register_mqtt_topic("boiler_cmd_wwonetime", std::bind(&Boiler::boiler_cmd_wwonetime, this, _1));
    register_mqtt_topic("boiler_cmd_wwcirculation", std::bind(&Boiler::boiler_cmd_wwcirculation, this, _1));
    register_mqtt_topic("boiler_cmd_wwtemp", std::bind(&Boiler::boiler_cmd_wwtemp, this, _1));
}

// add submenu context
void Boiler::add_context_menu() {
    EMSESPShell::commands->add_command(ShellContext::MAIN,
                                       CommandFlags::USER,
                                       flash_string_vector{F_(boiler)},
                                       [&](Shell & shell, const std::vector<std::string> & arguments __attribute__((unused))) {
                                           Boiler::console_commands(shell, ShellContext::BOILER);
                                       });
}

// boiler_cmd topic
void Boiler::boiler_cmd(const char * message) {
    StaticJsonDocument<EMSESP_MAX_JSON_SIZE_SMALL> doc;
    DeserializationError                           error = deserializeJson(doc, message);
    if (error) {
        LOG_DEBUG(F("MQTT error: payload %s, error %s"), message, error.c_str());
        return;
    }
    if (nullptr != doc["flowtemp"]) {
        uint8_t t = doc["flowtemp"];
        set_flow_temp(t);
    }
    if (nullptr != doc["wwtemp"]) {
        uint8_t t = doc["wwtemp"];
        set_warmwater_temp(t);
    }

    const char * command = doc["cmd"];
    if (command == nullptr) {
        return;
    }

    // boiler ww comfort setting
    if (strcmp(command, "comfort") == 0) {
        const char * data = doc["data"];
        if (data == nullptr) {
            return;
        }
        if (strcmp((char *)data, "hot") == 0) {
            set_warmwater_mode(1);
        } else if (strcmp((char *)data, "eco") == 0) {
            set_warmwater_mode(2);
        } else if (strcmp((char *)data, "intelligent") == 0) {
            set_warmwater_mode(3);
        }
        return;
    }

    // boiler flowtemp setting
    if (strcmp(command, "flowtemp") == 0) {
        uint8_t t = doc["data"];
        if (t) {
            set_flow_temp(t);
        }
        return;
    }
}

void Boiler::boiler_cmd_wwactivated(const char * message) {
    if ((message[0] == '1' || strcmp(message, "on") == 0) || (strcmp(message, "auto") == 0)) {
        set_warmwater_activated(true);
    } else if (message[0] == '0' || strcmp(message, "off") == 0) {
        set_warmwater_activated(false);
    }
}

void Boiler::boiler_cmd_wwonetime(const char * message) {
    if (message[0] == '1' || strcmp(message, "on") == 0) {
        set_warmwater_onetime(true);
    } else if (message[0] == '0' || strcmp(message, "off") == 0) {
        set_warmwater_onetime(false);
    }
}

void Boiler::boiler_cmd_wwcirculation(const char * message) {
    if (message[0] == '1' || strcmp(message, "on") == 0) {
        set_warmwater_circulation(true);
    } else if (message[0] == '0' || strcmp(message, "off") == 0) {
        set_warmwater_circulation(false);
    }
}

void Boiler::boiler_cmd_wwtemp(const char * message) {
    uint8_t t = atoi((char *)message);
    if (t) {
        set_warmwater_temp(t);
    }
}

// publish values via MQTT
void Boiler::publish_values() {
    const size_t        capacity = JSON_OBJECT_SIZE(47); // must recalculate if more objects addded https://arduinojson.org/v6/assistant/
    DynamicJsonDocument doc(capacity);

    char s[10]; // for formatting strings

    if (Helpers::hasValue(wWComfort_)) {
        if (wWComfort_ == 0x00) {
            doc["wWComfort"] = "Hot";
        } else if (wWComfort_ == 0xD8) {
            doc["wWComfort"] = "Eco";
        } else if (wWComfort_ == 0xEC) {
            doc["wWComfort"] = "Intelligent";
        }
    }

    if (Helpers::hasValue(wWSelTemp_)) {
        doc["wWSelTemp"] = wWSelTemp_;
    }
    if (Helpers::hasValue(wWSetTmp_)) {
        doc["wWSetTemp"] = wWSetTmp_;
    }
    if (Helpers::hasValue(wWDisinfectTemp_)) {
        doc["wWDisinfectionTemp"] = wWDisinfectTemp_;
    }
    if (Helpers::hasValue(selFlowTemp_)) {
        doc["selFlowTemp"] = selFlowTemp_;
    }
    if (Helpers::hasValue(selBurnPow_)) {
        doc["selBurnPow"] = selBurnPow_;
    }
    if (Helpers::hasValue(curBurnPow_)) {
        doc["curBurnPow"] = curBurnPow_;
    }
    if (Helpers::hasValue(pumpMod_)) {
        doc["pumpMod"] = pumpMod_;
    }
    if (Helpers::hasValue(pumpMod2_)) {
        doc["pumpMod2"] = pumpMod2_;
    }
    if (Helpers::hasValue(wWCircPump_, true)) {
        doc["wWCircPump"] = Helpers::render_value(s, wWCircPump_, EMS_VALUE_BOOL);
    }
    if (Helpers::hasValue(wWCircPumpType_)) {
        doc["wWCiPuType"] = wWCircPumpType_ ? "valve" : "pump";
    }
    if (Helpers::hasValue(wWCircPumpMode_)) {
        doc["wWCiPuMode"] = wWCircPumpMode_;
    }
    if (Helpers::hasValue(wWCirc_)) {
        doc["wWCirc"] = Helpers::render_value(s, wWCirc_, EMS_VALUE_BOOL);
    }
    if (Helpers::hasValue(extTemp_)) {
        doc["outdoorTemp"] = (float)extTemp_ / 10;
    }
    if (Helpers::hasValue(wWCurTmp_)) {
        doc["wWCurTmp"] = (float)wWCurTmp_ / 10;
    }
    if (Helpers::hasValue(wWCurFlow_)) {
        doc["wWCurFlow"] = (float)wWCurFlow_ / 10;
    }
    if (Helpers::hasValue(curFlowTemp_)) {
        doc["curFlowTemp"] = (float)curFlowTemp_ / 10;
    }
    if (Helpers::hasValue(retTemp_)) {
        doc["retTemp"] = (float)retTemp_ / 10;
    }
    if (Helpers::hasValue(switchTemp_)) {
        doc["switchTemp"] = (float)switchTemp_ / 10;
    }
    if (Helpers::hasValue(sysPress_)) {
        doc["sysPress"] = (float)sysPress_ / 10;
    }
    if (Helpers::hasValue(boilTemp_)) {
        doc["boilTemp"] = (float)boilTemp_ / 10;
    }
    if (Helpers::hasValue(wwStorageTemp1_)) {
        doc["wwStorageTemp1"] = (float)wwStorageTemp1_ / 10;
    }
    if (Helpers::hasValue(wwStorageTemp2_)) {
        doc["wwStorageTemp2"] = (float)wwStorageTemp2_ / 10;
    }
    if (Helpers::hasValue(exhaustTemp_)) {
        doc["exhaustTemp"] = (float)exhaustTemp_ / 10;
    }
    if (Helpers::hasValue(wWActivated_, true)) {
        doc["wWActivated"] = Helpers::render_value(s, wWActivated_, EMS_VALUE_BOOL);
    }
    if (Helpers::hasValue(wWOneTime_, true)) {
        doc["wWOnetime"] = Helpers::render_value(s, wWOneTime_, EMS_VALUE_BOOL);
    }
    if (Helpers::hasValue(wWDesinfecting_, true)) {
        doc["wWDesinfecting"] = Helpers::render_value(s, wWDesinfecting_, EMS_VALUE_BOOL);
    }
    if (Helpers::hasValue(wWReadiness_, true)) {
        doc["wWReady"] = Helpers::render_value(s, wWReadiness_, EMS_VALUE_BOOL);
    }
    if (Helpers::hasValue(wWRecharging_, true)) {
        doc["wWRecharge"] = Helpers::render_value(s, wWRecharging_, EMS_VALUE_BOOL);
    }
    if (Helpers::hasValue(wWTemperatureOK_, true)) {
        doc["wWTempOK"] = Helpers::render_value(s, wWTemperatureOK_, EMS_VALUE_BOOL);
    }
    if (Helpers::hasValue(wWCirc_, true)) {
        doc["wWCirc"] = Helpers::render_value(s, wWCirc_, EMS_VALUE_BOOL);
    }
    if (Helpers::hasValue(burnGas_, true)) {
        doc["burnGas"] = Helpers::render_value(s, burnGas_, EMS_VALUE_BOOL);
    }
    if (Helpers::hasValue(flameCurr_)) {
        doc["flameCurr"] = (float)(int16_t)flameCurr_ / 10;
    }
    if (Helpers::hasValue(heatPmp_, true)) {
        doc["heatPmp"] = Helpers::render_value(s, heatPmp_, EMS_VALUE_BOOL);
    }
    if (Helpers::hasValue(fanWork_, true)) {
        doc["fanWork"] = Helpers::render_value(s, fanWork_, EMS_VALUE_BOOL);
    }
    if (Helpers::hasValue(ignWork_, true)) {
        doc["ignWork"] = Helpers::render_value(s, ignWork_, EMS_VALUE_BOOL);
    }
    if (Helpers::hasValue(wWHeat_, true)) {
        doc["wWHeat"] = Helpers::render_value(s, wWHeat_, EMS_VALUE_BOOL);
    }
    if (Helpers::hasValue(heating_temp_)) {
        doc["heating_temp"] = heating_temp_;
    }
    if (Helpers::hasValue(pump_mod_max_)) {
        doc["pump_mod_max"] = pump_mod_max_;
    }
    if (Helpers::hasValue(pump_mod_min_)) {
        doc["pump_mod_min"] = pump_mod_min_;
    }
    if (Helpers::hasValue(wWStarts_)) {
        doc["wWStarts"] = wWStarts_;
    }
    if (Helpers::hasValue(wWWorkM_)) {
        doc["wWWorkM"] = wWWorkM_;
    }
    if (Helpers::hasValue(UBAuptime_)) {
        doc["UBAuptime"] = UBAuptime_;
    }
    if (Helpers::hasValue(burnStarts_)) {
        doc["burnStarts"] = burnStarts_;
    }
    if (Helpers::hasValue(burnWorkMin_)) {
        doc["burnWorkMin"] = burnWorkMin_;
    }
    if (Helpers::hasValue(heatWorkMin_)) {
        doc["heatWorkMin"] = heatWorkMin_;
    }

    if (Helpers::hasValue(serviceCode_)) {
        doc["serviceCode"]       = serviceCodeChar_;
        doc["serviceCodeNumber"] = serviceCode_;
    }

    // if we have data, publish it
    if (!doc.isNull()) {
        Mqtt::publish("boiler_data", doc);
    }
}

// called after a process command is called, to check values and see if we need to force an MQTT publish
bool Boiler::updated_values() {
    return false;
}

// print values to shell console
void Boiler::show_values(uuid::console::Shell & shell) {
    EMSdevice::show_values(shell); // for showing the header

    if (Helpers::hasValue(tap_water_active_, true)) {
        print_value(shell, 2, F("Hot tap water"), tap_water_active_ ? F("running") : F("off"));
    }

    if (Helpers::hasValue(heating_active_, true)) {
        print_value(shell, 2, F("Central heating"), heating_active_ ? F("active") : F("off"));
    }

    print_value(shell, 2, F("Warm Water activated"), wWActivated_, nullptr, EMS_VALUE_BOOL);
    if (Helpers::hasValue(wWCircPumpType_, true)) {
        print_value(shell, 2, F("Warm Water charging type"), wWCircPumpType_ ? F("3-way valve") : F("charge pump"));
    }
    print_value(shell, 2, F("Warm Water circulation pump available"), wWCircPump_, nullptr, EMS_VALUE_BOOL);

    if (Helpers::hasValue(wWCircPumpMode_)) {
        if (wWCircPumpMode_ == 7) {
            print_value(shell, 2, F("Warm Water circulation pump freq"), F("continuous"));
        } else {
            char s[7];
            char buffer[2];
            buffer[0] = (wWCircPumpMode_ % 10) + '0';
            buffer[1] = '\0';
            strlcpy(s, buffer, 7);
            strlcat(s, "x3min", 7);
            print_value(shell, 2, F("Warm Water circulation pump freq"), s);
        }
    }

    print_value(shell, 2, F("Warm Water circulation active"), wWCirc_, nullptr, EMS_VALUE_BOOL);

    if (wWComfort_ == 0x00) {
        print_value(shell, 2, F("Warm Water comfort setting"), F("Hot"));
    } else if (wWComfort_ == 0xD8) {
        print_value(shell, 2, F("Warm Water comfort setting"), F("Eco"));
    } else if (wWComfort_ == 0xEC) {
        print_value(shell, 2, F("Warm Water comfort setting"), F("Intelligent"));
    }

    print_value(shell, 2, F("Warm water mix temperature"), wwMixTemperature_, F_(degrees), 10);
    print_value(shell, 2, F("Warm water buffer boiler temperature"), wwBufferBoilerTemperature_, F_(degrees), 10);
    print_value(shell, 2, F("Warm Water disinfection temperature"), wWDisinfectTemp_, F_(degrees));
    print_value(shell, 2, F("Warm Water selected temperature"), wWSelTemp_, F_(degrees));
    print_value(shell, 2, F("Warm Water set temperature"), wWSetTmp_, F_(degrees));
    print_value(shell, 2, F("Warm Water current temperature (intern)"), wWCurTmp_, F_(degrees), 10);
    print_value(shell, 2, F("Warm water storage temperature (intern)"), wwStorageTemp1_, F_(degrees), 10);
    print_value(shell, 2, F("Warm Water current temperature (extern)"), wWCurTmp2_, F_(degrees), 10);
    print_value(shell, 2, F("Warm water storage temperature (extern)"), wwStorageTemp2_, F_(degrees), 10);
    print_value(shell, 2, F("Warm Water current tap water flow"), wWCurFlow_, F("l/min"), 10);
    print_value(shell, 2, F("Warm Water # starts"), wWStarts_, nullptr);
    if (Helpers::hasValue(wWWorkM_)) {
        shell.printfln(F("  Warm Water active time: %d days %d hours %d minutes"), wWWorkM_ / 1440, (wWWorkM_ % 1440) / 60, wWWorkM_ % 60);
    }
    print_value(shell, 2, F("Warm Water charging"), wWHeat_, nullptr, EMS_VALUE_BOOL);
    print_value(shell, 2, F("Warm Water disinfecting"), wWDesinfecting_, nullptr, EMS_VALUE_BOOL);
    print_value(shell, 2, F("Selected flow temperature"), selFlowTemp_, F_(degrees));
    print_value(shell, 2, F("Current flow temperature"), curFlowTemp_, F_(degrees), 10);
    print_value(shell, 2, F("Max boiler temperature"), boilTemp_, F_(degrees), 10);
    print_value(shell, 2, F("Return temperature"), retTemp_, F_(degrees), 10);
    print_value(shell, 2, F("Gas"), burnGas_, nullptr, EMS_VALUE_BOOL);
    print_value(shell, 2, F("Boiler pump"), heatPmp_, nullptr, EMS_VALUE_BOOL);
    print_value(shell, 2, F("Fan"), fanWork_, nullptr, EMS_VALUE_BOOL);
    print_value(shell, 2, F("Ignition"), ignWork_, nullptr, EMS_VALUE_BOOL);

    print_value(shell, 2, F("Burner selected max power"), selBurnPow_, F_(percent));
    print_value(shell, 2, F("Burner current power"), curBurnPow_, F_(percent));
    print_value(shell, 2, F("Flame current"), flameCurr_, F("uA"), 10);
    print_value(shell, 2, F("System pressure"), sysPress_, F("bar"), 10);
    if (Helpers::hasValue(serviceCode_)) {
        shell.printfln(F("  System service code: %s (%d)"), serviceCodeChar_, serviceCode_);
    } else if (serviceCodeChar_[0] != '\0') {
        print_value(shell, 2, F("System service code"), serviceCodeChar_);
    }

    // UBAParameters
    print_value(shell, 2, F("Heating temperature setting on the boiler"), heating_temp_, F_(degrees));
    print_value(shell, 2, F("Boiler circuit pump modulation max power"), pump_mod_max_, F_(percent));
    print_value(shell, 2, F("Boiler circuit pump modulation min power"), pump_mod_min_, F_(percent));

    // UBAMonitorSlow
    if (Helpers::hasValue(extTemp_)) {
        print_value(shell, 2, F("Outside temperature"), extTemp_, F_(degrees), 10);
    }

    print_value(shell, 2, F("Exhaust temperature"), exhaustTemp_, F_(degrees), 10);
    print_value(shell, 2, F("Pump modulation"), pumpMod_, F_(percent));
    print_value(shell, 2, F("Pump modulation2"), pumpMod2_, F_(percent));
    print_value(shell, 2, F("Burner # starts"), burnStarts_, nullptr);
    if (Helpers::hasValue(burnWorkMin_)) {
        shell.printfln(F("  Total burner operating time: %d days %d hours %d minutes"), burnWorkMin_ / 1440, (burnWorkMin_ % 1440) / 60, burnWorkMin_ % 60);
    }
    if (Helpers::hasValue(heatWorkMin_)) {
        shell.printfln(F("  Total heat operating time: %d days %d hours %d minutes"), heatWorkMin_ / 1440, (heatWorkMin_ % 1440) / 60, heatWorkMin_ % 60);
    }
    if (Helpers::hasValue(UBAuptime_)) {
        shell.printfln(F("  Total UBA working time: %d days %d hours %d minutes"), UBAuptime_ / 1440, (UBAuptime_ % 1440) / 60, UBAuptime_ % 60);
    }
}

/*
 * Check if hot tap water or heating is active
 * If a value has changed, post it immediately to MQTT so we get real time data
 */
void Boiler::check_active() {
    // hot tap water, using flow to check instead of the burner power
    // send these values back to the main EMSESP, so other classes (e.g. Shower) can use it
    if (Helpers::hasValue(wWCurFlow_) && Helpers::hasValue(burnGas_)) {
        tap_water_active_ = ((wWCurFlow_ != 0) && (burnGas_ != EMS_VALUE_BOOL_OFF));
        EMSESP::tap_water_active(tap_water_active_);
    }

    // heating
    // using a quick hack for checking the heating by looking at the Selected Flow Temp, but doesn't work for all boilers apparently
    if (Helpers::hasValue(selFlowTemp_) && Helpers::hasValue(burnGas_)) {
        heating_active_ = (!tap_water_active_ && ((selFlowTemp_ >= EMS_BOILER_SELFLOWTEMP_HEATING) && (burnGas_ != EMS_VALUE_BOOL_OFF)));
    }

    // see if the heating or hot tap water has changed, if so send
    // last_boilerActive stores heating in bit 1 and tap water in bit 2
    if (Helpers::hasValue(tap_water_active_, true) && Helpers::hasValue(heating_active_, true)) {
        uint8_t latest_boilerState = (tap_water_active_ << 1) + heating_active_;
        if (latest_boilerState != last_boilerState) {
            last_boilerState = latest_boilerState;
            Mqtt::publish("tapwater_active", tap_water_active_);
            Mqtt::publish("heating_active", heating_active_);
        }
    }
}

// 0x33
void Boiler::process_UBAParameterWW(std::shared_ptr<const Telegram> telegram) {
    telegram->read_value(wWActivated_, 1);     // 0xFF means on
    telegram->read_value(wWCircPump_, 6);      // 0xFF means on
    telegram->read_value(wWCircPumpMode_, 7);  // 1=1x3min... 6=6x3min, 7=continuous
    telegram->read_value(wWCircPumpType_, 10); // 0 = charge pump, 0xff = 3-way valve
    telegram->read_value(wWSelTemp_, 2);
    telegram->read_value(wWDisinfectTemp_, 8);
    telegram->read_value(wWComfort_, 9);
}

// 0x18
void Boiler::process_UBAMonitorFast(std::shared_ptr<const Telegram> telegram) {
    telegram->read_value(selFlowTemp_, 0);
    telegram->read_value(curFlowTemp_, 1);
    telegram->read_value(selBurnPow_, 3); // burn power max setting
    telegram->read_value(curBurnPow_, 4);

    telegram->read_bitvalue(burnGas_, 7, 0);
    telegram->read_bitvalue(fanWork_, 7, 2);
    telegram->read_bitvalue(ignWork_, 7, 3);
    telegram->read_bitvalue(heatPmp_, 7, 5);
    telegram->read_bitvalue(wWHeat_, 7, 6);
    telegram->read_bitvalue(wWCirc_, 7, 7);

    // warm water storage sensors (if present)
    // wwStorageTemp2 is also used by some brands as the boiler temperature - see https://github.com/proddy/EMS-ESP/issues/206
    telegram->read_value(wwStorageTemp1_, 9);  // 0x8300 if not available
    telegram->read_value(wwStorageTemp2_, 11); // 0x8000 if not available - this is boiler temp

    telegram->read_value(retTemp_, 13);
    telegram->read_value(flameCurr_, 15);
    telegram->read_value(serviceCode_, 20);

    // system pressure. FF means missing
    telegram->read_value(sysPress_, 17); // is *10

    // read the service code / installation status as appears on the display
    if ((telegram->message_length > 18) && (telegram->offset == 0)) {
        serviceCodeChar_[0] = char(telegram->message_data[18]); // ascii character 1
        serviceCodeChar_[1] = char(telegram->message_data[19]); // ascii character 2
        serviceCodeChar_[2] = '\0';                             // null terminate string
    }

    // at this point do a quick check to see if the hot water or heating is active
    check_active();
}

/*
 * UBATotalUptime - type 0x14 - total uptime
 * received only after requested (not broadcasted)
 */
void Boiler::process_UBATotalUptime(std::shared_ptr<const Telegram> telegram) {
    telegram->read_value(UBAuptime_, 0, 3); // force to 3 bytes
}

/*
 * UBAParameters - type 0x16
 */
void Boiler::process_UBAParameters(std::shared_ptr<const Telegram> telegram) {
    telegram->read_value(heating_temp_, 1);
    telegram->read_value(pump_mod_max_, 9);
    telegram->read_value(pump_mod_min_, 10);
}

/*
 * UBAMonitorWW - type 0x34 - warm water monitor. 19 bytes long
 * received every 10 seconds
 */
void Boiler::process_UBAMonitorWW(std::shared_ptr<const Telegram> telegram) {
    telegram->read_value(wWSetTmp_, 0);
    telegram->read_value(wWCurTmp_, 1);
    telegram->read_value(wWCurTmp2_, 3);
    telegram->read_value(wWCurFlow_, 9);

    telegram->read_value(wWWorkM_, 10, 3);  // force to 3 bytes
    telegram->read_value(wWStarts_, 13, 3); // force to 3 bytes

    telegram->read_bitvalue(wWOneTime_, 5, 1);
    telegram->read_bitvalue(wWDesinfecting_, 5, 2);
    telegram->read_bitvalue(wWReadiness_, 5, 3);
    telegram->read_bitvalue(wWRecharging_, 5, 4);
    telegram->read_bitvalue(wWTemperatureOK_, 5, 5);
}

/*
 * UBAMonitorFastPlus - type 0xE4 - central heating monitor EMS+
 * Still to figure out are: serviceCode, retTemp, sysPress
 */
void Boiler::process_UBAMonitorFastPlus(std::shared_ptr<const Telegram> telegram) {
    telegram->read_value(selFlowTemp_, 6);
    telegram->read_bitvalue(burnGas_, 11, 0);
    telegram->read_bitvalue(wWHeat_, 11, 2);
    telegram->read_value(curBurnPow_, 10);
    telegram->read_value(selBurnPow_, 9);
    telegram->read_value(curFlowTemp_, 7);
    telegram->read_value(flameCurr_, 19);

    // read the service code / installation status as appears on the display
    if ((telegram->message_length > 4) && (telegram->offset == 0)) {
        serviceCodeChar_[0] = char(telegram->message_data[4]); // ascii character 1
        serviceCodeChar_[1] = char(telegram->message_data[5]); // ascii character 2
        serviceCodeChar_[2] = '\0';
    }

    // at this point do a quick check to see if the hot water or heating is active
    check_active();
}

/*
 * UBAMonitorSlow - type 0x19 - central heating monitor part 2 (27 bytes long)
 * received every 60 seconds
 * e.g. 08 00 19 00 80 00 02 41 80 00 00 00 00 00 03 91 7B 05 B8 40 00 00 00 04 92 AD 00 5E EE 80 00
 *      08 0B 19 00 FF EA 02 47 80 00 00 00 00 62 03 CA 24 2C D6 23 00 00 00 27 4A B6 03 6E 43 
 *                  00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 17 19 20 21 22 23 24
 */
void Boiler::process_UBAMonitorSlow(std::shared_ptr<const Telegram> telegram) {
    telegram->read_value(extTemp_, 0);
    telegram->read_value(boilTemp_, 2);
    telegram->read_value(exhaustTemp_, 4);
    telegram->read_value(switchTemp_, 25); // only if there is a mixing module present
    telegram->read_value(pumpMod_, 9);
    telegram->read_value(burnStarts_, 10, 3);  // force to 3 bytes
    telegram->read_value(burnWorkMin_, 13, 3); // force to 3 bytes
    telegram->read_value(heatWorkMin_, 19, 3); // force to 3 bytes
}

/*
 * UBAMonitorSlowPlus2 - type 0xE3
 */
void Boiler::process_UBAMonitorSlowPlus2(std::shared_ptr<const Telegram> telegram) {
    telegram->read_value(pumpMod2_, 13);
}

/*
 * UBAMonitorSlowPlus - type 0xE5 - central heating monitor EMS+
 */
void Boiler::process_UBAMonitorSlowPlus(std::shared_ptr<const Telegram> telegram) {
    telegram->read_bitvalue(fanWork_, 2, 2);
    telegram->read_bitvalue(ignWork_, 2, 3);
    telegram->read_bitvalue(heatPmp_, 2, 5);
    telegram->read_bitvalue(wWCirc_, 2, 7);
    telegram->read_value(burnStarts_, 10, 3);  // force to 3 bytes
    telegram->read_value(burnWorkMin_, 13, 3); // force to 3 bytes
    telegram->read_value(heatWorkMin_, 19, 3); // force to 3 bytes
    telegram->read_value(pumpMod_, 25);
}

// 0xE9 - DHW Status
// e.g. 08 00 E9 00 37 01 F6 01 ED 00 00 00 00 41 3C 00 00 00 00 00 00 00 00 00 00 00 00 37 00 00 00 (CRC=77) #data=27
void Boiler::process_UBADHWStatus(std::shared_ptr<const Telegram> telegram) {
    telegram->read_value(wWSetTmp_, 0);
    telegram->read_value(wWCurTmp_, 1);
    telegram->read_value(wWCurTmp2_, 3);

    telegram->read_value(wWWorkM_, 17, 3);  // force to 3 bytes
    telegram->read_value(wWStarts_, 14, 3); // force to 3 bytes

    telegram->read_bitvalue(wWOneTime_, 12, 2);
    telegram->read_bitvalue(wWDesinfecting_, 12, 3);
    telegram->read_bitvalue(wWReadiness_, 12, 4);
    telegram->read_bitvalue(wWRecharging_, 13, 4);
    telegram->read_bitvalue(wWTemperatureOK_, 13, 5);
    telegram->read_bitvalue(wWCircPump_, 13, 2);

    telegram->read_value(wWActivated_, 20);
    telegram->read_value(wWSelTemp_, 10);
    telegram->read_value(wWDisinfectTemp_, 9);
}

// 0x2A - MC10Status
// e.g. 88 00 2A 00 00 00 00 00 00 00 00 00 D2 00 00 80 00 00 01 08 80 00 02 47 00
// see https://github.com/proddy/EMS-ESP/issues/397
void Boiler::process_MC10Status(std::shared_ptr<const Telegram> telegram) {
    telegram->read_value(wwMixTemperature_, 14);
    telegram->read_value(wwBufferBoilerTemperature_, 18);
}

/*
 * UBAOutdoorTemp - type 0xD1 - external temperature EMS+
 */
void Boiler::process_UBAOutdoorTemp(std::shared_ptr<const Telegram> telegram) {
    telegram->read_value(extTemp_, 0);
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

// UBASetPoint 0x1A
// not yet implemented
void Boiler::process_UBASetPoints(std::shared_ptr<const Telegram> telegram) {
    // uint8_t setpoint = telegram->message_data[0]; // boiler flow temp
    // uint8_t ww_power = telegram->message_data[2]; // power in %
}

// 0x35
// not yet implemented
void Boiler::process_UBAFlags(std::shared_ptr<const Telegram> telegram) {
}

// 0x1C
// not yet implemented
void Boiler::process_UBAMaintenanceStatus(std::shared_ptr<const Telegram> telegram) {
}

// 0x15
// not yet implemented
void Boiler::process_UBAMaintenanceSettings(std::shared_ptr<const Telegram> telegram) {
}

// 0x10, 0x11, 0x12
// not yet implemented
void Boiler::process_UBAErrorMessage(std::shared_ptr<const Telegram> telegram) {
    // data: displaycode(2), errornumner(2), year, month, hour, day, minute, duration(2), src-addr
}

#pragma GCC diagnostic pop

// Set the warm water temperature 0x33
void Boiler::set_warmwater_temp(const uint8_t temperature) {
    LOG_INFO(F("Setting boiler warm water temperature to %d C"), temperature);
    write_command(EMS_TYPE_UBAParameterWW, 2, temperature);
    // for i9000, see #397
    write_command(EMS_TYPE_UBAFlags, 3, temperature);
}

// flow temp
void Boiler::set_flow_temp(const uint8_t temperature) {
    LOG_INFO(F("Setting boiler flow temperature to %d C"), temperature);
    write_command(EMS_TYPE_UBASetPoints, 0, temperature);
}

// 1=hot, 2=eco, 3=intelligent
// note some boilers do not have this setting, than it's done by thermostat
// on a RC35 it's by EMSESP::send_write_request(0x37, 0x10, 2, &set, 1, 0); (set is 1,2,3)
void Boiler::set_warmwater_mode(const uint8_t comfort) {
    uint8_t set;
    if (comfort == 1) {
        LOG_INFO(F("Setting boiler warm water to Hot"));
        set = 0x00;
    } else if (comfort == 2) {
        LOG_INFO(F("Setting boiler warm water to Eco"));
        set = 0xD8;
    } else if (comfort == 3) {
        LOG_INFO(F("Setting boiler warm water to Intelligent"));
        set = 0xEC;
    } else {
        return; // do nothing
    }
    write_command(EMS_TYPE_UBAParameterWW, 9, set);
}

// turn on/off warm water
void Boiler::set_warmwater_activated(const bool activated) {
    LOG_INFO(F("Setting boiler warm water %s"), activated ? "on" : "off");
    uint8_t value;

    // https://github.com/proddy/EMS-ESP/issues/268
    if (EMSbus::is_ht3()) {
        value = (activated ? 0x08 : 0x00); // 0x08 is on, 0x00 is off
    } else {
        value = (activated ? 0xFF : 0x00); // 0xFF is on, 0x00 is off
    }
    write_command(EMS_TYPE_UBAParameterWW, 1, value);
}

// Activate / De-activate the Warm Tap Water
// true = on, false = off
// Note: Using the type 0x1D to put the boiler into Test mode. This may be shown on the boiler with a flashing 'T'
void Boiler::set_tapwarmwater_activated(const bool activated) {
    LOG_INFO(F("Setting boiler warm tap water %s"), activated ? "on" : "off");
    uint8_t message_data[EMS_MAX_TELEGRAM_MESSAGE_LENGTH];
    for (uint8_t i = 0; i < sizeof(message_data); i++) {
        message_data[i] = 0x00;
    }

    // we use the special test mode 0x1D for this. Setting the first data to 5A puts the system into test mode and
    // a setting of 0x00 puts it back into normal operating mode
    // when in test mode we're able to mess around with the 3-way valve settings
    if (!activated) {
        // on
        message_data[0] = 0x5A; // test mode on
        message_data[1] = 0x00; // burner output 0%
        message_data[3] = 0x64; // boiler pump capacity 100%
        message_data[4] = 0xFF; // 3-way valve hot water only
    } else {
        // get out of test mode. Send all zeros.
        // telegram: 0B 08 1D 00 00
        message_data[4] = 0x00; // test mode off
    }

    write_command(EMS_TYPE_UBAFunctionTest, 0, message_data, sizeof(message_data), 0);
}

// Activate / De-activate One Time warm water 0x35
// true = on, false = off
// See also https://github.com/proddy/EMS-ESP/issues/341#issuecomment-596245458 for Junkers
void Boiler::set_warmwater_onetime(const bool activated) {
    LOG_INFO(F("Setting boiler warm water OneTime loading %s"), activated ? "on" : "off");
    write_command(EMS_TYPE_UBAFlags, 0, (activated ? 0x22 : 0x02));
}

// Activate / De-activate circulation of warm water 0x35
// true = on, false = off
void Boiler::set_warmwater_circulation(const bool activated) {
    LOG_INFO(F("Setting boiler warm water circulation %s"), activated ? "on" : "off");
    write_command(EMS_TYPE_UBAFlags, 1, (activated ? 0x22 : 0x02));
}

// add console commands
void Boiler::console_commands(Shell & shell, unsigned int context) {
    EMSESPShell::commands->add_command(ShellContext::BOILER,
                                       CommandFlags::ADMIN,
                                       flash_string_vector{F_(read)},
                                       flash_string_vector{F_(typeid_mandatory)},
                                       [=](Shell & shell __attribute__((unused)), const std::vector<std::string> & arguments) {
                                           uint16_t type_id = Helpers::hextoint(arguments.front().c_str());
                                           EMSESP::send_read_request(type_id, device_id());
                                       });

    EMSESPShell::commands->add_command(ShellContext::BOILER,
                                       CommandFlags::ADMIN,
                                       flash_string_vector{F_(wwtemp)},
                                       flash_string_vector{F_(degrees_mandatory)},
                                       [=](Shell & shell __attribute__((unused)), const std::vector<std::string> & arguments) {
                                           set_warmwater_temp(Helpers::atoint(arguments.front().c_str()));
                                       });

    EMSESPShell::commands->add_command(ShellContext::BOILER,
                                       CommandFlags::ADMIN,
                                       flash_string_vector{F_(flowtemp)},
                                       flash_string_vector{F_(degrees_mandatory)},
                                       [=](Shell & shell __attribute__((unused)), const std::vector<std::string> & arguments) {
                                           set_flow_temp(Helpers::atoint(arguments.front().c_str()));
                                       });

    EMSESPShell::commands->add_command(
        ShellContext::BOILER,
        CommandFlags::ADMIN,
        flash_string_vector{F_(wwactive)},
        flash_string_vector{F_(bool_mandatory)},
        [=](Shell & shell, const std::vector<std::string> & arguments) {
            if (arguments[0] == read_flash_string(F_(on))) {
                set_warmwater_activated(true);
            } else if (arguments[0] == read_flash_string(F_(off))) {
                set_warmwater_activated(false);
            } else {
                shell.println(F("Must be on or off"));
                return;
            }
        },
        [](Shell & shell __attribute__((unused)), const std::vector<std::string> & arguments __attribute__((unused))) -> const std::vector<std::string> {
            return std::vector<std::string>{read_flash_string(F_(on)), read_flash_string(F_(off))};
        });

    EMSESPShell::commands->add_command(
        ShellContext::BOILER,
        CommandFlags::ADMIN,
        flash_string_vector{F_(wwonetime)},
        flash_string_vector{F_(bool_mandatory)},
        [=](Shell & shell, const std::vector<std::string> & arguments) {
            if (arguments[0] == read_flash_string(F_(on))) {
                set_warmwater_onetime(true);
            } else if (arguments[0] == read_flash_string(F_(off))) {
                set_warmwater_onetime(false);
            } else {
                shell.println(F("Must be on or off"));
                return;
            }
        },
        [](Shell & shell __attribute__((unused)), const std::vector<std::string> & arguments __attribute__((unused))) -> const std::vector<std::string> {
            return std::vector<std::string>{read_flash_string(F_(on)), read_flash_string(F_(off))};
        });

    EMSESPShell::commands->add_command(
        ShellContext::BOILER,
        CommandFlags::ADMIN,
        flash_string_vector{F_(wwcirculation)},
        flash_string_vector{F_(bool_mandatory)},
        [=](Shell & shell, const std::vector<std::string> & arguments) {
            if (arguments[0] == read_flash_string(F_(on))) {
                set_warmwater_circulation(true);
            } else if (arguments[0] == read_flash_string(F_(off))) {
                set_warmwater_circulation(false);
            } else {
                shell.println(F("Must be on or off"));
                return;
            }
        },
        [](Shell & shell __attribute__((unused)), const std::vector<std::string> & arguments __attribute__((unused))) -> const std::vector<std::string> {
            return std::vector<std::string>{read_flash_string(F_(on)), read_flash_string(F_(off))};
        });

    EMSESPShell::commands->add_command(
        ShellContext::BOILER,
        CommandFlags::ADMIN,
        flash_string_vector{F_(comfort)},
        flash_string_vector{F_(comfort_mandatory)},
        [=](Shell & shell, const std::vector<std::string> & arguments) {
            if (arguments[0] == read_flash_string(F_(hot))) {
                set_warmwater_mode(1);
            } else if (arguments[0] == read_flash_string(F_(eco))) {
                set_warmwater_mode(2);
            } else if (arguments[0] == read_flash_string(F_(intelligent))) {
                set_warmwater_mode(3);
            } else {
                shell.println(F("Invalid value. Must be hot, eco or intelligent"));
            }
        },
        [](Shell & shell __attribute__((unused)), const std::vector<std::string> & arguments __attribute__((unused))) -> const std::vector<std::string> {
            return std::vector<std::string>{read_flash_string(F_(hot)), read_flash_string(F_(eco)), read_flash_string(F_(intelligent))};
        });

    EMSESPShell::commands->add_command(ShellContext::BOILER,
                                       CommandFlags::USER,
                                       flash_string_vector{F_(show)},
                                       [&](Shell & shell, const std::vector<std::string> & arguments __attribute__((unused))) { show_values(shell); });

    // enter the context
    Console::enter_custom_context(shell, context);
}

} // namespace emsesp
