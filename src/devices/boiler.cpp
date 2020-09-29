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

namespace emsesp {

REGISTER_FACTORY(Boiler, EMSdevice::DeviceType::BOILER)

uuid::log::Logger Boiler::logger_{F_(boiler), uuid::log::Facility::CONSOLE};

Boiler::Boiler(uint8_t device_type, int8_t device_id, uint8_t product_id, const std::string & version, const std::string & name, uint8_t flags, uint8_t brand)
    : EMSdevice(device_type, device_id, product_id, version, name, flags, brand) {
    this->reserve_mem(20); // reserve some space for the telegram registries, to avoid memory fragmentation

    LOG_DEBUG(F("Adding new Boiler with device ID 0x%02X"), device_id);

    // the telegram handlers...
    register_telegram_type(0x10, F("UBAErrorMessage1"), false, [&](std::shared_ptr<const Telegram> t) { process_UBAErrorMessage(t); });
    register_telegram_type(0x11, F("UBAErrorMessage2"), false, [&](std::shared_ptr<const Telegram> t) { process_UBAErrorMessage(t); });
    register_telegram_type(0x14, F("UBATotalUptime"), true, [&](std::shared_ptr<const Telegram> t) { process_UBATotalUptime(t); });
    register_telegram_type(0x15, F("UBAMaintenanceData"), false, [&](std::shared_ptr<const Telegram> t) { process_UBAMaintenanceData(t); });
    register_telegram_type(0x16, F("UBAParameters"), true, [&](std::shared_ptr<const Telegram> t) { process_UBAParameters(t); });
    register_telegram_type(0x18, F("UBAMonitorFast"), false, [&](std::shared_ptr<const Telegram> t) { process_UBAMonitorFast(t); });
    register_telegram_type(0x19, F("UBAMonitorSlow"), true, [&](std::shared_ptr<const Telegram> t) { process_UBAMonitorSlow(t); });
    register_telegram_type(0x1A, F("UBASetPoints"), false, [&](std::shared_ptr<const Telegram> t) { process_UBASetPoints(t); });
    register_telegram_type(0x1C, F("UBAMaintenanceStatus"), false, [&](std::shared_ptr<const Telegram> t) { process_UBAMaintenanceStatus(t); });
    register_telegram_type(0x2A, F("MC10Status"), false, [&](std::shared_ptr<const Telegram> t) { process_MC10Status(t); });
    register_telegram_type(0x33, F("UBAParameterWW"), true, [&](std::shared_ptr<const Telegram> t) { process_UBAParameterWW(t); });
    register_telegram_type(0x34, F("UBAMonitorWW"), false, [&](std::shared_ptr<const Telegram> t) { process_UBAMonitorWW(t); });
    register_telegram_type(0x35, F("UBAFlags"), false, [&](std::shared_ptr<const Telegram> t) { process_UBAFlags(t); });
    register_telegram_type(0xD1, F("UBAOutdoorTemp"), false, [&](std::shared_ptr<const Telegram> t) { process_UBAOutdoorTemp(t); });
    register_telegram_type(0xE3, F("UBAMonitorSlowPlus"), false, [&](std::shared_ptr<const Telegram> t) { process_UBAMonitorSlowPlus2(t); });
    register_telegram_type(0xE4, F("UBAMonitorFastPlus"), false, [&](std::shared_ptr<const Telegram> t) { process_UBAMonitorFastPlus(t); });
    register_telegram_type(0xE5, F("UBAMonitorSlowPlus"), false, [&](std::shared_ptr<const Telegram> t) { process_UBAMonitorSlowPlus(t); });
    register_telegram_type(0xE6, F("UBAParametersPlus"), true, [&](std::shared_ptr<const Telegram> t) { process_UBAParametersPlus(t); });
    register_telegram_type(0xE9, F("UBADHWStatus"), false, [&](std::shared_ptr<const Telegram> t) { process_UBADHWStatus(t); });
    register_telegram_type(0xEA, F("UBAParameterWWPlus"), true, [&](std::shared_ptr<const Telegram> t) { process_UBAParameterWWPlus(t); });

    // MQTT commands for boiler topic
    register_mqtt_cmd(F("comfort"), [&](const char * value, const int8_t id) { return set_warmwater_mode(value, id); });
    register_mqtt_cmd(F("wwactivated"), [&](const char * value, const int8_t id) { return set_warmwater_activated(value, id); });
    register_mqtt_cmd(F("wwtapactivated"), [&](const char * value, const int8_t id) { return set_tapwarmwater_activated(value, id); });
    register_mqtt_cmd(F("wwonetime"), [&](const char * value, const int8_t id) { return set_warmwater_onetime(value, id); });
    register_mqtt_cmd(F("wwcircpump"), [&](const char * value, const int8_t id) { return set_warmwater_circulation_pump(value, id); });
    register_mqtt_cmd(F("wwcirculation"), [&](const char * value, const int8_t id) { return set_warmwater_circulation(value, id); });
    register_mqtt_cmd(F("wwcircmode"), [&](const char * value, const int8_t id) { return set_warmwater_circulation_mode(value, id); });
    register_mqtt_cmd(F("flowtemp"), [&](const char * value, const int8_t id) { return set_flow_temp(value, id); });
    register_mqtt_cmd(F("wwtemp"), [&](const char * value, const int8_t id) { return set_warmwater_temp(value, id); });
    register_mqtt_cmd(F("heatingactivated"), [&](const char * value, const int8_t id) { return set_heating_activated(value, id); });
    register_mqtt_cmd(F("heatingtemp"), [&](const char * value, const int8_t id) { return set_heating_temp(value, id); });
    register_mqtt_cmd(F("burnmaxpower"), [&](const char * value, const int8_t id) { return set_max_power(value, id); });
    register_mqtt_cmd(F("burnminpower"), [&](const char * value, const int8_t id) { return set_min_power(value, id); });
    register_mqtt_cmd(F("boilhyston"), [&](const char * value, const int8_t id) { return set_hyst_on(value, id); });
    register_mqtt_cmd(F("boilhystoff"), [&](const char * value, const int8_t id) { return set_hyst_off(value, id); });
    register_mqtt_cmd(F("burnperiod"), [&](const char * value, const int8_t id) { return set_burn_period(value, id); });
    register_mqtt_cmd(F("pumpdelay"), [&](const char * value, const int8_t id) { return set_pump_delay(value, id); });

    // API call
    // Command::add_with_json(this->device_type(), F("info"), Boiler::command_info);
    Command::add_with_json(this->device_type(), F("info"), [&](const char * value, const int8_t id, JsonObject & object) {
        return command_info(value, id, object);
    });

    if (Mqtt::mqtt_format() == Mqtt::Format::HA) {
        register_mqtt_ha_config();
    }
}

// create the config topics for Home Assistant MQTT Discovery
// for each of the main elements
void Boiler::register_mqtt_ha_config() {
    Mqtt::register_mqtt_ha_binary_sensor(F("Boiler DHW"), "tapwater_active");
    Mqtt::register_mqtt_ha_binary_sensor(F("Boiler Heating"), "heating_active");

    Mqtt::register_mqtt_ha_sensor(F("Service Code"), this->device_type(), "serviceCode", "", "");
    Mqtt::register_mqtt_ha_sensor(F("Service Code number"), this->device_type(), "serviceCodeNumber", "", "");
    Mqtt::register_mqtt_ha_sensor(F("Boiler WW Selected Temp"), this->device_type(), "wWSelTemp", "°C", "mdi:coolant-temperature");
    Mqtt::register_mqtt_ha_sensor(F("Selected flow temperature"), this->device_type(), "selFlowTemp", "°C", "mdi:coolant-temperature");
    Mqtt::register_mqtt_ha_sensor(F("Current flow temperature"), this->device_type(), "curFlowTemp", "°C", "mdi:coolant-temperature");
    Mqtt::register_mqtt_ha_sensor(F("Warm Water set temperature"), this->device_type(), "wWSetTemp", "°C", "mdi:coolant-temperature");
    Mqtt::register_mqtt_ha_sensor(F("Warm Water current temperature (intern)"), this->device_type(), "wWCurTmp", "°C", "mdi:coolant-temperature");
    Mqtt::register_mqtt_ha_sensor(F("Warm Water current temperature (extern)"), this->device_type(), "wWCurTmp2", "°C", "mdi:coolant-temperature");
    Mqtt::register_mqtt_ha_sensor(F("Pump modulation"), this->device_type(), "pumpMod", "%", "mdi:sine-wave");
    Mqtt::register_mqtt_ha_sensor(F("Heat Pump modulation"), this->device_type(), "pumpMod2", "%", "mdi:sine-wave");
    Mqtt::register_mqtt_ha_sensor(F("System Pressure"), this->device_type(), "sysPress", "bar", "");
}

// send stuff to the Web UI
void Boiler::device_info_web(JsonArray & root) {
    JsonObject dataElement;

    render_value_json(root, "", F("Service code"), serviceCodeChar_, nullptr);
    render_value_json(root, "", F("Service code number"), serviceCode_, nullptr);
    render_value_json(root, "", F("Hot tap water"), tap_water_active_, nullptr, EMS_VALUE_BOOL);
    render_value_json(root, "", F("Central Heating"), heating_active_, nullptr, EMS_VALUE_BOOL);
    render_value_json(root, "", F("Selected flow temperature"), selFlowTemp_, F_(degrees));
    render_value_json(root, "", F("Current flow temperature"), curFlowTemp_, F_(degrees), 10);
    render_value_json(root, "", F("Warm Water selected temperature"), wWSelTemp_, F_(degrees));
    render_value_json(root, "", F("Warm Water set temperature"), wWSetTmp_, F_(degrees));
    render_value_json(root, "", F("Warm Water current temperature (intern)"), wWCurTmp_, F_(degrees), 10);
    render_value_json(root, "", F("Warm Water current temperature (extern)"), wWCurTmp2_, F_(degrees), 10);
    render_value_json(root, "", F("Pump modulation"), pumpMod_, F_(percent));
    render_value_json(root, "", F("Heat Pump modulation"), pumpMod2_, F_(percent));
    render_value_json(root, "", F("System pressure"), sysPress_, F_(bar), 10);
}

bool Boiler::command_info(const char * value, const int8_t id, JsonObject & output) {
    return (export_values(output));
}

// creates JSON doc from values
// returns false if empty
bool Boiler::export_values(JsonObject & output) {
    char s[10]; // for formatting strings

    if (Helpers::hasValue(heating_active_, EMS_VALUE_BOOL)) {
        output["heating_active"] = Helpers::render_value(s, heating_active_, EMS_VALUE_BOOL);
    }

    if (Helpers::hasValue(tap_water_active_, EMS_VALUE_BOOL)) {
        output["tap_water_active"] = Helpers::render_value(s, tap_water_active_, EMS_VALUE_BOOL);
    }

    if (Helpers::hasValue(wWComfort_)) {
        if (wWComfort_ == 0x00) {
            output["wWComfort"] = "Hot";
        } else if (wWComfort_ == 0xD8) {
            output["wWComfort"] = "Eco";
        } else if (wWComfort_ == 0xEC) {
            output["wWComfort"] = "Intelligent";
        }
    }

    if (Helpers::hasValue(wWSelTemp_)) {
        output["wWSelTemp"] = wWSelTemp_;
    }
    if (Helpers::hasValue(wWSetTmp_)) {
        output["wWSetTemp"] = wWSetTmp_;
    }
    if (Helpers::hasValue(wWDisinfectTemp_)) {
        output["wWDisinfectionTemp"] = wWDisinfectTemp_;
    }
    if (Helpers::hasValue(selFlowTemp_)) {
        output["selFlowTemp"] = selFlowTemp_;
    }
    if (Helpers::hasValue(selBurnPow_)) {
        output["selBurnPow"] = selBurnPow_;
    }
    if (Helpers::hasValue(curBurnPow_)) {
        output["curBurnPow"] = curBurnPow_;
    }
    if (Helpers::hasValue(pumpMod_)) {
        output["pumpMod"] = pumpMod_;
    }
    if (Helpers::hasValue(pumpMod2_)) {
        output["pumpMod2"] = pumpMod2_;
    }
    if (wWType_ == 0) { // no output if not set
        output["wWType"] = F("off");
    } else if (wWType_ == 1) {
        output["wWType"] = F("flow");
    } else if (wWType_ == 2) {
        output["wWType"] = F("buffered flow");
    } else if (wWType_ == 3) {
        output["wWType"] = F("buffer");
    } else if (wWType_ == 4) {
        output["wWType"] = F("layered buffer");
    }
    if (Helpers::hasValue(wWChargeType_, EMS_VALUE_BOOL)) {
        output["wWChargeType"] = wWChargeType_ ? "valve" : "pump";
    }
    if (Helpers::hasValue(wWCircPump_, EMS_VALUE_BOOL)) {
        output["wWCircPump"] = Helpers::render_value(s, wWCircPump_, EMS_VALUE_BOOL);
    }
    if (Helpers::hasValue(wWCircPumpMode_)) {
        output["wWCiPuMode"] = wWCircPumpMode_;
    }
    if (Helpers::hasValue(wWCirc_, EMS_VALUE_BOOL)) {
        output["wWCirc"] = Helpers::render_value(s, wWCirc_, EMS_VALUE_BOOL);
    }
    if (Helpers::hasValue(extTemp_)) {
        output["outdoorTemp"] = (float)extTemp_ / 10;
    }
    if (Helpers::hasValue(wWCurTmp_)) {
        output["wWCurTmp"] = (float)wWCurTmp_ / 10;
    }
    if (Helpers::hasValue(wWCurTmp2_)) {
        output["wWCurTmp2"] = (float)wWCurTmp2_ / 10;
    }
    if (Helpers::hasValue(wWCurFlow_)) {
        output["wWCurFlow"] = (float)wWCurFlow_ / 10;
    }
    if (Helpers::hasValue(curFlowTemp_)) {
        output["curFlowTemp"] = (float)curFlowTemp_ / 10;
    }
    if (Helpers::hasValue(retTemp_)) {
        output["retTemp"] = (float)retTemp_ / 10;
    }
    if (Helpers::hasValue(switchTemp_)) {
        output["switchTemp"] = (float)switchTemp_ / 10;
    }
    if (Helpers::hasValue(sysPress_)) {
        output["sysPress"] = (float)sysPress_ / 10;
    }
    if (Helpers::hasValue(boilTemp_)) {
        output["boilTemp"] = (float)boilTemp_ / 10;
    }
    if (Helpers::hasValue(wwStorageTemp1_)) {
        output["wwStorageTemp1"] = (float)wwStorageTemp1_ / 10;
    }
    if (Helpers::hasValue(wwStorageTemp2_)) {
        output["wwStorageTemp2"] = (float)wwStorageTemp2_ / 10;
    }
    if (Helpers::hasValue(exhaustTemp_)) {
        output["exhaustTemp"] = (float)exhaustTemp_ / 10;
    }
    if (Helpers::hasValue(wWActivated_, EMS_VALUE_BOOL)) {
        output["wWActivated"] = Helpers::render_value(s, wWActivated_, EMS_VALUE_BOOL);
    }
    if (Helpers::hasValue(wWOneTime_, EMS_VALUE_BOOL)) {
        output["wWOnetime"] = Helpers::render_value(s, wWOneTime_, EMS_VALUE_BOOL);
    }
    if (Helpers::hasValue(wWDisinfecting_, EMS_VALUE_BOOL)) {
        output["wWDisinfecting"] = Helpers::render_value(s, wWDisinfecting_, EMS_VALUE_BOOL);
    }
    if (Helpers::hasValue(wWCharging_, EMS_VALUE_BOOL)) {
        output["wWCharge"] = Helpers::render_value(s, wWCharging_, EMS_VALUE_BOOL);
    }
    if (Helpers::hasValue(wWRecharging_, EMS_VALUE_BOOL)) {
        output["wWRecharge"] = Helpers::render_value(s, wWRecharging_, EMS_VALUE_BOOL);
    }
    if (Helpers::hasValue(wWTemperatureOK_, EMS_VALUE_BOOL)) {
        output["wWTempOK"] = Helpers::render_value(s, wWTemperatureOK_, EMS_VALUE_BOOL);
    }
    if (Helpers::hasValue(wWActive_, EMS_VALUE_BOOL)) {
        output["wWActive"] = Helpers::render_value(s, wWActive_, EMS_VALUE_BOOL);
    }
    if (Helpers::hasValue(burnGas_, EMS_VALUE_BOOL)) {
        output["burnGas"] = Helpers::render_value(s, burnGas_, EMS_VALUE_BOOL);
    }
    if (Helpers::hasValue(flameCurr_)) {
        output["flameCurr"] = (float)(int16_t)flameCurr_ / 10;
    }
    if (Helpers::hasValue(heatPmp_, EMS_VALUE_BOOL)) {
        output["heatPump"] = Helpers::render_value(s, heatPmp_, EMS_VALUE_BOOL);
    }
    if (Helpers::hasValue(fanWork_, EMS_VALUE_BOOL)) {
        output["fanWork"] = Helpers::render_value(s, fanWork_, EMS_VALUE_BOOL);
    }
    if (Helpers::hasValue(ignWork_, EMS_VALUE_BOOL)) {
        output["ignWork"] = Helpers::render_value(s, ignWork_, EMS_VALUE_BOOL);
    }
    if (Helpers::hasValue(wWHeat_, EMS_VALUE_BOOL)) {
        output["wWHeat"] = Helpers::render_value(s, wWHeat_, EMS_VALUE_BOOL);
    }
    if (Helpers::hasValue(heating_activated_, EMS_VALUE_BOOL)) {
        output["heatingActivated"] = Helpers::render_value(s, heating_activated_, EMS_VALUE_BOOL);
    }
    if (Helpers::hasValue(heating_temp_)) {
        output["heatingTemp"] = heating_temp_;
    }
    if (Helpers::hasValue(pump_mod_max_)) {
        output["pumpModMax"] = pump_mod_max_;
    }
    if (Helpers::hasValue(pump_mod_min_)) {
        output["pumpModMin"] = pump_mod_min_;
    }
    if (Helpers::hasValue(pumpDelay_)) {
        output["pumpDelay"] = pumpDelay_;
    }
    if (Helpers::hasValue(burnPeriod_)) {
        output["burnMinPeriod"] = burnPeriod_;
    }
    if (Helpers::hasValue(burnPowermin_)) {
        output["burnMinPower"] = burnPowermin_;
    }
    if (Helpers::hasValue(burnPowermax_)) {
        output["burnMaxPower"] = burnPowermax_;
    }
    if (Helpers::hasValue(boilTemp_on_)) {
        output["boilHystOn"] = boilTemp_on_;
    }
    if (Helpers::hasValue(boilTemp_off_)) {
        output["boilHystOff"] = boilTemp_off_;
    }
    if (Helpers::hasValue(setFlowTemp_)) {
        output["setFlowTemp"] = setFlowTemp_;
    }
    if (Helpers::hasValue(setWWPumpPow_)) {
        output["wWSetPumpPower"] = setWWPumpPow_;
    }
    if (Helpers::hasValue(wWStarts_)) {
        output["wWStarts"] = wWStarts_;
    }
    if (Helpers::hasValue(wWWorkM_)) {
        output["wWWorkM"] = wWWorkM_;
    }
    if (Helpers::hasValue(UBAuptime_)) {
        output["UBAuptime"] = UBAuptime_;
    }
    if (Helpers::hasValue(burnStarts_)) {
        output["burnStarts"] = burnStarts_;
    }
    if (Helpers::hasValue(burnWorkMin_)) {
        output["burnWorkMin"] = burnWorkMin_;
    }
    if (Helpers::hasValue(heatWorkMin_)) {
        output["heatWorkMin"] = heatWorkMin_;
    }
    if (Helpers::hasValue(serviceCode_)) {
        output["serviceCode"]       = serviceCodeChar_;
        output["serviceCodeNumber"] = serviceCode_;
    }

    return (output.size());
}

// publish values via MQTT
void Boiler::publish_values() {
    StaticJsonDocument<EMSESP_MAX_JSON_SIZE_LARGE> doc;
    JsonObject                                     output = doc.to<JsonObject>();
    if (export_values(output)) {
        Mqtt::publish(F("boiler_data"), doc.as<JsonObject>());
    }

    // send out heating and tapwater status - even if there is no change (force = true)
    check_active();
}

// called after a process command is called, to check values and see if we need to force an MQTT publish
bool Boiler::updated_values() {
    if (changed_) {
        changed_ = false;
        return true;
    }
    return false;
}

// print values to shell console
void Boiler::show_values(uuid::console::Shell & shell) {
    EMSdevice::show_values(shell); // for showing the header

    print_value(shell, 2, F("Hot tap water"), tap_water_active_, nullptr, EMS_VALUE_BOOL);
    print_value(shell, 2, F("Central heating"), heating_active_, nullptr, EMS_VALUE_BOOL);

    print_value(shell, 2, F("Warm Water activated"), wWActivated_, nullptr, EMS_VALUE_BOOL);
    if (wWType_ == 0) {
        print_value(shell, 2, F("Warm Water type"), F("off"));
    } else if (wWType_ == 1) {
        print_value(shell, 2, F("Warm Water type"), F("flow"));
    } else if (wWType_ == 2) {
        print_value(shell, 2, F("Warm Water type"), F("buffered flow"));
    } else if (wWType_ == 3) {
        print_value(shell, 2, F("Warm Water type"), F("buffer"));
    } else if (wWType_ == 4) {
        print_value(shell, 2, F("Warm Water type"), F("layered buffer"));
    }

    if (Helpers::hasValue(wWChargeType_, EMS_VALUE_BOOL)) {
        print_value(shell, 2, F("Warm Water charging type"), wWChargeType_ ? F("3-way valve") : F("charge pump"));
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
    print_value(shell, 2, F("Warm Water one time charging"), wWOneTime_, nullptr, EMS_VALUE_BOOL);
    print_value(shell, 2, F("Warm Water charging"), wWHeat_, nullptr, EMS_VALUE_BOOL);
    print_value(shell, 2, F("Warm Water disinfecting"), wWDisinfecting_, nullptr, EMS_VALUE_BOOL);
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
    print_value(shell, 2, F("Flame current"), flameCurr_, F_(uA), 10);
    print_value(shell, 2, F("System pressure"), sysPress_, F_(bar), 10);
    if (Helpers::hasValue(serviceCode_)) {
        shell.printfln(F("  System service code: %s (%d)"), serviceCodeChar_, serviceCode_);
    } else if (serviceCodeChar_[0] != '\0') {
        print_value(shell, 2, F("System service code"), serviceCodeChar_);
    }

    // UBAParameters
    print_value(shell, 2, F("Heating activated"), heating_activated_, nullptr, EMS_VALUE_BOOL);
    print_value(shell, 2, F("Heating temperature setting on the boiler"), heating_temp_, F_(degrees));
    print_value(shell, 2, F("Boiler circuit pump modulation max power"), pump_mod_max_, F_(percent));
    print_value(shell, 2, F("Boiler circuit pump modulation min power"), pump_mod_min_, F_(percent));
    print_value(shell, 2, F("Boiler circuit pump delay time"), pumpDelay_, F_(min));
    print_value(shell, 2, F("Boiler temp hysteresis on"), boilTemp_on_, F_(degrees));
    print_value(shell, 2, F("Boiler temp hysteresis off"), boilTemp_off_, F_(degrees));
    print_value(shell, 2, F("Boiler burner min period"), burnPeriod_, F_(min));
    print_value(shell, 2, F("Boiler burner min power"), burnPowermin_, F_(percent));
    print_value(shell, 2, F("Boiler burner max power"), burnPowermax_, F_(percent));

    // UBASetPoint - these may differ from the above
    print_value(shell, 2, F("Set Flow temperature"), setFlowTemp_, F_(degrees));
    print_value(shell, 2, F("Boiler burner set power"), setBurnPow_, F_(percent));
    print_value(shell, 2, F("Warm Water pump set power"), setWWPumpPow_, F_(percent));

    // UBAMonitorSlow
    if (Helpers::hasValue(extTemp_)) {
        print_value(shell, 2, F("Outside temperature"), extTemp_, F_(degrees), 10);
    }

    print_value(shell, 2, F("Exhaust temperature"), exhaustTemp_, F_(degrees), 10);
    print_value(shell, 2, F("Pump modulation"), pumpMod_, F_(percent));
    print_value(shell, 2, F("Heat Pump modulation"), pumpMod2_, F_(percent));
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

    shell.println();
}

/*
 * Check if hot tap water or heating is active
 * If a value has changed, post it immediately to MQTT so we get real time data
 */
void Boiler::check_active() {
    if ((boilerState_ & 0x09) != (last_boilerState & 0x09)) {
        char s[7];
        bool b = ((boilerState_ & 0x09) == 0x09);
        Mqtt::publish(F("heating_active"), Helpers::render_boolean(s, b));
        heating_active_ = b ? EMS_VALUE_BOOL_ON : EMS_VALUE_BOOL_OFF;
    }

    if ((boilerState_ & 0x0A) != (last_boilerState & 0x0A)) {
        char s[7];
        bool b = ((boilerState_ & 0x0A) == 0x0A);
        Mqtt::publish(F("tapwater_active"), Helpers::render_boolean(s, b));
        tap_water_active_ = b ? EMS_VALUE_BOOL_ON : EMS_VALUE_BOOL_OFF;
        EMSESP::tap_water_active(b); // let EMS-ESP know, used in the Shower class
    }

    last_boilerState = boilerState_;

    /*
    // hot tap water, using flow to check instead of the burner power
    // send these values back to the main EMSESP, so other classes (e.g. Shower) can use it
    if (Helpers::hasValue(wWCurFlow_) && Helpers::hasValue(burnGas_) && (wWType_ > 0) && (wWType_ < 3)) {
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
    uint8_t latest_boilerState = (tap_water_active_ << 1) + heating_active_;
    if (latest_boilerState != last_boilerState) {
        last_boilerState = latest_boilerState;
        static char s[10];
        Mqtt::publish(F("tapwater_active"), Helpers::render_boolean(s, tap_water_active_));
        static char s2[10];
        Mqtt::publish(F("heating_active"), Helpers::render_boolean(s2, heating_active_));
    }
    */
}

// 0x33
void Boiler::process_UBAParameterWW(std::shared_ptr<const Telegram> telegram) {
    changed_ |= telegram->read_value(wWActivated_, 1);    // 0xFF means on
    changed_ |= telegram->read_value(wWCircPump_, 6);     // 0xFF means on
    changed_ |= telegram->read_value(wWCircPumpMode_, 7); // 1=1x3min... 6=6x3min, 7=continuous
    changed_ |= telegram->read_value(wWChargeType_, 10);  // 0 = charge pump, 0xff = 3-way valve
    changed_ |= telegram->read_value(wWSelTemp_, 2);
    changed_ |= telegram->read_value(wWDisinfectTemp_, 8);
    changed_ |= telegram->read_value(wWComfort_, 9);
}

// 0x18
void Boiler::process_UBAMonitorFast(std::shared_ptr<const Telegram> telegram) {
    changed_ |= telegram->read_value(selFlowTemp_, 0);
    changed_ |= telegram->read_value(curFlowTemp_, 1);
    changed_ |= telegram->read_value(selBurnPow_, 3); // burn power max setting
    changed_ |= telegram->read_value(curBurnPow_, 4);
    changed_ |= telegram->read_value(boilerState_, 5);

    changed_ |= telegram->read_bitvalue(burnGas_, 7, 0);
    changed_ |= telegram->read_bitvalue(fanWork_, 7, 2);
    changed_ |= telegram->read_bitvalue(ignWork_, 7, 3);
    changed_ |= telegram->read_bitvalue(heatPmp_, 7, 5);
    changed_ |= telegram->read_bitvalue(wWHeat_, 7, 6);
    changed_ |= telegram->read_bitvalue(wWCirc_, 7, 7);

    // warm water storage sensors (if present)
    // wwStorageTemp2 is also used by some brands as the boiler temperature - see https://github.com/proddy/EMS-ESP/issues/206
    changed_ |= telegram->read_value(wwStorageTemp1_, 9);  // 0x8300 if not available
    changed_ |= telegram->read_value(wwStorageTemp2_, 11); // 0x8000 if not available - this is boiler temp

    changed_ |= telegram->read_value(retTemp_, 13);
    changed_ |= telegram->read_value(flameCurr_, 15);
    changed_ |= telegram->read_value(serviceCode_, 20);

    // system pressure. FF means missing
    changed_ |= telegram->read_value(sysPress_, 17); // is *10

    // read the service code / installation status as appears on the display
    if ((telegram->message_length > 18) && (telegram->offset == 0)) {
        changed_ |= telegram->read_value(serviceCodeChar_[0], 18);
        changed_ |= telegram->read_value(serviceCodeChar_[1], 19);
        serviceCodeChar_[2] = '\0'; // null terminate string
    }

    // at this point do a quick check to see if the hot water or heating is active
    check_active();
}

/*
 * UBATotalUptime - type 0x14 - total uptime
 * received only after requested (not broadcasted)
 */
void Boiler::process_UBATotalUptime(std::shared_ptr<const Telegram> telegram) {
    changed_ |= telegram->read_value(UBAuptime_, 0, 3); // force to 3 bytes
}

/*
 * UBAParameters - type 0x16
 */
void Boiler::process_UBAParameters(std::shared_ptr<const Telegram> telegram) {
    changed_ |= telegram->read_value(heating_activated_, 0);
    changed_ |= telegram->read_value(heating_temp_, 1);
    changed_ |= telegram->read_value(burnPowermax_, 2);
    changed_ |= telegram->read_value(burnPowermin_, 3);
    changed_ |= telegram->read_value(boilTemp_off_, 4);
    changed_ |= telegram->read_value(boilTemp_on_, 5);
    changed_ |= telegram->read_value(burnPeriod_, 6);
    changed_ |= telegram->read_value(pumpDelay_, 8);
    changed_ |= telegram->read_value(pump_mod_max_, 9);
    changed_ |= telegram->read_value(pump_mod_min_, 10);
}

/*
 * UBAMonitorWW - type 0x34 - warm water monitor. 19 bytes long
 * received every 10 seconds
 */
void Boiler::process_UBAMonitorWW(std::shared_ptr<const Telegram> telegram) {
    changed_ |= telegram->read_value(wWSetTmp_, 0);
    changed_ |= telegram->read_value(wWCurTmp_, 1);
    changed_ |= telegram->read_value(wWCurTmp2_, 3);
    changed_ |= telegram->read_value(wWCurFlow_, 9);
    changed_ |= telegram->read_value(wWType_, 8);

    changed_ |= telegram->read_value(wWWorkM_, 10, 3);  // force to 3 bytes
    changed_ |= telegram->read_value(wWStarts_, 13, 3); // force to 3 bytes

    changed_ |= telegram->read_bitvalue(wWOneTime_, 5, 1);
    changed_ |= telegram->read_bitvalue(wWDisinfecting_, 5, 2);
    changed_ |= telegram->read_bitvalue(wWCharging_, 5, 3);
    changed_ |= telegram->read_bitvalue(wWRecharging_, 5, 4);
    changed_ |= telegram->read_bitvalue(wWTemperatureOK_, 5, 5);
    changed_ |= telegram->read_bitvalue(wWActive_, 5, 6);
}

/*
 * UBAMonitorFastPlus - type 0xE4 - central heating monitor EMS+
 * Still to figure out are: serviceCode, retTemp, sysPress
 */
void Boiler::process_UBAMonitorFastPlus(std::shared_ptr<const Telegram> telegram) {
    changed_ |= telegram->read_value(selFlowTemp_, 6);
    changed_ |= telegram->read_bitvalue(burnGas_, 11, 0);
    changed_ |= telegram->read_bitvalue(wWHeat_, 11, 2);
    changed_ |= telegram->read_value(curBurnPow_, 10);
    changed_ |= telegram->read_value(selBurnPow_, 9);
    changed_ |= telegram->read_value(curFlowTemp_, 7);
    changed_ |= telegram->read_value(flameCurr_, 19);

    // read the service code / installation status as appears on the display
    if ((telegram->message_length > 4) && (telegram->offset == 0)) {
        changed_ |= telegram->read_value(serviceCodeChar_[0], 4);
        changed_ |= telegram->read_value(serviceCodeChar_[1], 5);
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
    changed_ |= telegram->read_value(extTemp_, 0);
    changed_ |= telegram->read_value(boilTemp_, 2);
    changed_ |= telegram->read_value(exhaustTemp_, 4);
    changed_ |= telegram->read_value(switchTemp_, 25); // only if there is a mixing module present
    changed_ |= telegram->read_value(pumpMod_, 9);
    changed_ |= telegram->read_value(burnStarts_, 10, 3);  // force to 3 bytes
    changed_ |= telegram->read_value(burnWorkMin_, 13, 3); // force to 3 bytes
    changed_ |= telegram->read_value(heatWorkMin_, 19, 3); // force to 3 bytes
}

/*
 * UBAMonitorSlowPlus2 - type 0xE3
 */
void Boiler::process_UBAMonitorSlowPlus2(std::shared_ptr<const Telegram> telegram) {
    changed_ |= telegram->read_value(pumpMod2_, 13); // Heat Pump Modulation
}

/*
 * UBAMonitorSlowPlus - type 0xE5 - central heating monitor EMS+
 */
void Boiler::process_UBAMonitorSlowPlus(std::shared_ptr<const Telegram> telegram) {
    changed_ |= telegram->read_bitvalue(fanWork_, 2, 2);
    changed_ |= telegram->read_bitvalue(ignWork_, 2, 3);
    changed_ |= telegram->read_bitvalue(heatPmp_, 2, 5);
    changed_ |= telegram->read_bitvalue(wWCirc_, 2, 7);
    changed_ |= telegram->read_value(burnStarts_, 10, 3);  // force to 3 bytes
    changed_ |= telegram->read_value(burnWorkMin_, 13, 3); // force to 3 bytes
    changed_ |= telegram->read_value(heatWorkMin_, 19, 3); // force to 3 bytes
    changed_ |= telegram->read_value(pumpMod_, 25);
}

/*
 * UBAParametersPlus - type 0xe6
 */
void Boiler::process_UBAParametersPlus(std::shared_ptr<const Telegram> telegram) {
    changed_ |= telegram->read_value(heating_activated_, 0);
    changed_ |= telegram->read_value(heating_temp_, 1);
    changed_ |= telegram->read_value(burnPowermax_, 6);
    changed_ |= telegram->read_value(burnPowermin_, 7);
    changed_ |= telegram->read_value(boilTemp_off_, 8);
    changed_ |= telegram->read_value(boilTemp_on_, 9);
    changed_ |= telegram->read_value(burnPeriod_, 10);
}

// 0xEA
void Boiler::process_UBAParameterWWPlus(std::shared_ptr<const Telegram> telegram) {
    changed_ |= telegram->read_value(wWActivated_, 5);     // 0x01 means on
    changed_ |= telegram->read_value(wWCircPump_, 10);     // 0x01 means yes
    changed_ |= telegram->read_value(wWCircPumpMode_, 11); // 1=1x3min... 6=6x3min, 7=continuous
    // changed_ |= telegram->read_value(wWDisinfectTemp_, 12); // settings, status in E9
    // changed_ |= telegram->read_value(wWSelTemp_, 6);        // settings, status in E9
}

// 0xE9 - DHW Status
// e.g. 08 00 E9 00 37 01 F6 01 ED 00 00 00 00 41 3C 00 00 00 00 00 00 00 00 00 00 00 00 37 00 00 00 (CRC=77) #data=27
void Boiler::process_UBADHWStatus(std::shared_ptr<const Telegram> telegram) {
    changed_ |= telegram->read_value(wWSetTmp_, 0);
    changed_ |= telegram->read_value(wWCurTmp_, 1);
    changed_ |= telegram->read_value(wWCurTmp2_, 3);

    changed_ |= telegram->read_value(wWWorkM_, 17, 3);  // force to 3 bytes
    changed_ |= telegram->read_value(wWStarts_, 14, 3); // force to 3 bytes

    changed_ |= telegram->read_bitvalue(wWOneTime_, 12, 2);
    changed_ |= telegram->read_bitvalue(wWDisinfecting_, 12, 3);
    changed_ |= telegram->read_bitvalue(wWCharging_, 12, 4);
    changed_ |= telegram->read_bitvalue(wWRecharging_, 13, 4);
    changed_ |= telegram->read_bitvalue(wWTemperatureOK_, 13, 5);
    changed_ |= telegram->read_bitvalue(wWCircPump_, 13, 2);

    // changed_ |= telegram->read_value(wWActivated_, 20); // Activated is in 0xEA, this is something other 0/100%
    changed_ |= telegram->read_value(wWSelTemp_, 10);
    changed_ |= telegram->read_value(wWDisinfectTemp_, 9);
}

// 0x2A - MC10Status
// e.g. 88 00 2A 00 00 00 00 00 00 00 00 00 D2 00 00 80 00 00 01 08 80 00 02 47 00
// see https://github.com/proddy/EMS-ESP/issues/397
void Boiler::process_MC10Status(std::shared_ptr<const Telegram> telegram) {
    changed_ |= telegram->read_value(wwMixTemperature_, 14);
    changed_ |= telegram->read_value(wwBufferBoilerTemperature_, 18);
}

/*
 * UBAOutdoorTemp - type 0xD1 - external temperature EMS+
 */
void Boiler::process_UBAOutdoorTemp(std::shared_ptr<const Telegram> telegram) {
    changed_ |= telegram->read_value(extTemp_, 0);
}

// UBASetPoint 0x1A
void Boiler::process_UBASetPoints(std::shared_ptr<const Telegram> telegram) {
    changed_ |= telegram->read_value(setFlowTemp_, 0);  // boiler set temp from thermostat
    changed_ |= telegram->read_value(setBurnPow_, 1);   // max output power in %
    changed_ |= telegram->read_value(setWWPumpPow_, 2); // ww pump speed/power?
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

// 0x35
// not yet implemented
void Boiler::process_UBAFlags(std::shared_ptr<const Telegram> telegram) {
}

// 0x1C
// not yet implemented
void Boiler::process_UBAMaintenanceStatus(std::shared_ptr<const Telegram> telegram) {
    // first byte: Maintenance due (0 = no, 3 = yes, due to operating hours, 8 = yes, due to date)
}

// 0x10, 0x11, 0x12
// not yet implemented
void Boiler::process_UBAErrorMessage(std::shared_ptr<const Telegram> telegram) {
    // data: displaycode(2), errornumber(2), year, month, hour, day, minute, duration(2), src-addr
}

#pragma GCC diagnostic pop

// 0x15
void Boiler::process_UBAMaintenanceData(std::shared_ptr<const Telegram> telegram) {
    // first byte: Maintenance messages (0 = none, 1 = by operating hours, 2 = by date)
    // I see a value of 3 in the 1st byte when the boiler is booted, so probably a flag
    if (telegram->message_data[0] == 3) {
        LOG_WARNING(F("Boiler has booted."));
    }
}

// Set the warm water temperature 0x33
bool Boiler::set_warmwater_temp(const char * value, const int8_t id) {
    int v = 0;
    if (!Helpers::value2number(value, v)) {
        return false;
    }

    LOG_INFO(F("Setting boiler warm water temperature to %d C"), v);
    if (get_toggle_fetch(EMS_TYPE_UBAParametersPlus)) {
        write_command(EMS_TYPE_UBAParameterWWPlus, 6, v, EMS_TYPE_UBAParameterWWPlus);
    } else {
        write_command(EMS_TYPE_UBAParameterWW, 2, v, EMS_TYPE_UBAParameterWW);
        write_command(EMS_TYPE_UBAFlags, 3, v, EMS_TYPE_UBAParameterWW); // for i9000, see #397
    }

    return true;
}

// flow temp
bool Boiler::set_flow_temp(const char * value, const int8_t id) {
    int v = 0;
    if (!Helpers::value2number(value, v)) {
        return false;
    }

    LOG_INFO(F("Setting boiler flow temperature to %d C"), v);
    write_command(EMS_TYPE_UBASetPoints, 0, v, EMS_TYPE_UBASetPoints);

    return true;
}

// set min boiler output
bool Boiler::set_heating_activated(const char * value, const int8_t id) {
    bool v = false;
    if (!Helpers::value2bool(value, v)) {
        return false;
    }

    LOG_INFO(F("Setting boiler heating %s"), v ? "on" : "off");
    if (get_toggle_fetch(EMS_TYPE_UBAParametersPlus)) {
        write_command(EMS_TYPE_UBAParametersPlus, 0, v ? 0x01 : 0, EMS_TYPE_UBAParametersPlus);
    } else {
        write_command(EMS_TYPE_UBAParameters, 0, v ? 0xFF : 0, EMS_TYPE_UBAParameters);
    }

    return true;
}

// set heating maximum temperature
bool Boiler::set_heating_temp(const char * value, const int8_t id) {
    int v = 0;
    if (!Helpers::value2number(value, v)) {
        return false;
    }

    LOG_INFO(F("Setting boiler heating temperature to "), v);
    if (get_toggle_fetch(EMS_TYPE_UBAParametersPlus)) {
        write_command(EMS_TYPE_UBAParametersPlus, 1, v, EMS_TYPE_UBAParametersPlus);
    } else {
        write_command(EMS_TYPE_UBAParameters, 1, v, EMS_TYPE_UBAParameters);
    }

    return true;
}

// set min boiler output
bool Boiler::set_min_power(const char * value, const int8_t id) {
    int v = 0;
    if (!Helpers::value2number(value, v)) {
        return false;
    }

    LOG_INFO(F("Setting boiler min power to "), v);
    if (get_toggle_fetch(EMS_TYPE_UBAParametersPlus)) {
        write_command(EMS_TYPE_UBAParametersPlus, 7, v, EMS_TYPE_UBAParametersPlus);
    } else {
        write_command(EMS_TYPE_UBAParameters, 3, v, EMS_TYPE_UBAParameters);
    }

    return true;
}

// set max temp
bool Boiler::set_max_power(const char * value, const int8_t id) {
    int v = 0;
    if (!Helpers::value2number(value, v)) {
        return false;
    }

    LOG_INFO(F("Setting boiler max power to %d C"), v);
    if (get_toggle_fetch(EMS_TYPE_UBAParametersPlus)) {
        write_command(EMS_TYPE_UBAParametersPlus, 6, v, EMS_TYPE_UBAParametersPlus);
    } else {
        write_command(EMS_TYPE_UBAParameters, 2, v, EMS_TYPE_UBAParameters);
    }

    return true;
}

// set boiler on hysteresis
bool Boiler::set_hyst_on(const char * value, const int8_t id) {
    int v = 0;
    if (!Helpers::value2number(value, v)) {
        return false;
    }

    LOG_INFO(F("Setting boiler hysteresis on to %d C"), v);
    if (get_toggle_fetch(EMS_TYPE_UBAParametersPlus)) {
        write_command(EMS_TYPE_UBAParametersPlus, 9, v, EMS_TYPE_UBAParametersPlus);
    } else {
        write_command(EMS_TYPE_UBAParameters, 5, v, EMS_TYPE_UBAParameters);
    }

    return true;
}

// set boiler off hysteresis
bool Boiler::set_hyst_off(const char * value, const int8_t id) {
    int v = 0;
    if (!Helpers::value2number(value, v)) {
        return false;
    }

    LOG_INFO(F("Setting boiler hysteresis off to %d C"), v);
    if (get_toggle_fetch(EMS_TYPE_UBAParametersPlus)) {
        write_command(EMS_TYPE_UBAParametersPlus, 8, v, EMS_TYPE_UBAParametersPlus);
    } else {
        write_command(EMS_TYPE_UBAParameters, 4, v, EMS_TYPE_UBAParameters);
    }

    return true;
}

// set min burner period
bool Boiler::set_burn_period(const char * value, const int8_t id) {
    int v = 0;
    if (!Helpers::value2number(value, v)) {
        return false;
    }

    LOG_INFO(F("Setting burner min. period to %d min"), v);
    if (get_toggle_fetch(EMS_TYPE_UBAParametersPlus)) {
        write_command(EMS_TYPE_UBAParametersPlus, 10, v, EMS_TYPE_UBAParametersPlus);
    } else {
        write_command(EMS_TYPE_UBAParameters, 6, v, EMS_TYPE_UBAParameters);
    }

    return true;
}

// set pump delay
bool Boiler::set_pump_delay(const char * value, const int8_t id) {
    int v = 0;
    if (!Helpers::value2number(value, v)) {
        return false;
    }

    if (get_toggle_fetch(EMS_TYPE_UBAParameters)) {
        LOG_INFO(F("Setting boiler pump delay to %d min"), v);
        write_command(EMS_TYPE_UBAParameters, 8, v, EMS_TYPE_UBAParameters);
    }

    return true;
}

// note some boilers do not have this setting, than it's done by thermostat
// on a RC35 it's by EMSESP::send_write_request(0x37, 0x10, 2, &set, 1, 0); (set is 1,2,3) 1=hot, 2=eco, 3=intelligent
bool Boiler::set_warmwater_mode(const char * value, const int8_t id) {
    if (value == nullptr) {
        return false;
    }

    if (!get_toggle_fetch(EMS_TYPE_UBAParameterWW)) {
        return false;
    }

    uint8_t set;
    if (strcmp(value, "hot") == 0) {
        LOG_INFO(F("Setting boiler warm water to Hot"));
        set = 0x00;
    } else if (strcmp(value, "eco") == 0) {
        LOG_INFO(F("Setting boiler warm water to Eco"));
        set = 0xD8;
    } else if (strcmp(value, "intelligent") == 0) {
        LOG_INFO(F("Setting boiler warm water to Intelligent"));
        set = 0xEC;
    } else {
        return false; // do nothing
    }

    write_command(EMS_TYPE_UBAParameterWW, 9, set, EMS_TYPE_UBAParameterWW);
    return true;
}

// turn on/off warm water
bool Boiler::set_warmwater_activated(const char * value, const int8_t id) {
    bool v = false;
    if (!Helpers::value2bool(value, v)) {
        return false;
    }

    LOG_INFO(F("Setting boiler warm water %s"), v ? "on" : "off");

    // https://github.com/proddy/EMS-ESP/issues/268
    uint8_t n;
    if (EMSbus::is_ht3()) {
        n = (v ? 0x08 : 0x00); // 0x08 is on, 0x00 is off
    } else {
        n = (v ? 0xFF : 0x00); // 0xFF is on, 0x00 is off
    }

    if (get_toggle_fetch(EMS_TYPE_UBAParameterWWPlus)) {
        write_command(EMS_TYPE_UBAParameterWWPlus, 1, v ? 1 : 0, EMS_TYPE_UBAParameterWWPlus);
    } else {
        write_command(EMS_TYPE_UBAParameterWW, 1, n, 0x34);
    }

    return true;
}

// Activate / De-activate the Warm Tap Water
// Note: Using the type 0x1D to put the boiler into Test mode. This may be shown on the boiler with a flashing 'T'
bool Boiler::set_tapwarmwater_activated(const char * value, const int8_t id) {
    bool v = false;
    if (!Helpers::value2bool(value, v)) {
        return false;
    }

    LOG_INFO(F("Setting tap warm tap water %s"), v ? "on" : "off");
    uint8_t message_data[EMS_MAX_TELEGRAM_MESSAGE_LENGTH];
    for (uint8_t i = 0; i < sizeof(message_data); i++) {
        message_data[i] = 0x00;
    }

    // we use the special test mode 0x1D for this. Setting the first data to 5A puts the system into test mode and
    // a setting of 0x00 puts it back into normal operating mode
    // when in test mode we're able to mess around with the 3-way valve settings
    if (!v) {
        // on
        message_data[0] = 0x5A; // test mode on
        message_data[1] = 0x00; // burner output 0%
        message_data[3] = 0x64; // boiler pump capacity 100%
        message_data[4] = 0xFF; // 3-way valve hot water only
    } else {
        // get out of test mode. Send all zeros.
        // telegram: 0B 08 1D 00 00
    }

    write_command(EMS_TYPE_UBAFunctionTest, 0, message_data, sizeof(message_data), 0);

    return true;
}

// Activate / De-activate One Time warm water 0x35
// true = on, false = off
// See also https://github.com/proddy/EMS-ESP/issues/341#issuecomment-596245458 for Junkers
bool Boiler::set_warmwater_onetime(const char * value, const int8_t id) {
    bool v = false;
    if (!Helpers::value2bool(value, v)) {
        return false;
    }

    LOG_INFO(F("Setting boiler warm water OneTime loading %s"), v ? "on" : "off");
    if (get_toggle_fetch(EMS_TYPE_UBAParameterWWPlus)) {
        write_command(EMS_TYPE_UBAFlags, 0, (v ? 0x22 : 0x02), 0xE9); // not sure if this is in flags
    } else {
        write_command(EMS_TYPE_UBAFlags, 0, (v ? 0x22 : 0x02), 0x34);
    }

    return true;
}

// Activate / De-activate circulation of warm water 0x35
// true = on, false = off
bool Boiler::set_warmwater_circulation(const char * value, const int8_t id) {
    bool v = false;
    if (!Helpers::value2bool(value, v)) {
        return false;
    }

    LOG_INFO(F("Setting boiler warm water circulation %s"), v ? "on" : "off");
    if (get_toggle_fetch(EMS_TYPE_UBAParameterWWPlus)) {
        write_command(EMS_TYPE_UBAFlags, 1, (v ? 0x22 : 0x02), 0xE9); // not sure if this is in flags
    } else {
        write_command(EMS_TYPE_UBAFlags, 1, (v ? 0x22 : 0x02), 0x34);
    }

    return true;
}

// configuration of warm water circulation pump
bool Boiler::set_warmwater_circulation_pump(const char * value, const int8_t id) {
    bool v = false;
    if (!Helpers::value2bool(value, v)) {
        return false;
    }

    LOG_INFO(F("Setting boiler warm water circulation %s"), v ? "on" : "off");

    if (get_toggle_fetch(EMS_TYPE_UBAParameterWWPlus)) {
        write_command(EMS_TYPE_UBAParameterWWPlus, 10, v ? 0x01 : 0x00, EMS_TYPE_UBAParameterWWPlus);
    } else {
        write_command(EMS_TYPE_UBAParameterWW, 6, v ? 0xFF : 0x00, EMS_TYPE_UBAParameterWW);
    }

    return true;
}

// Set the mode of circulation, 1x3min, ... 6x3min, continous
// true = on, false = off
bool Boiler::set_warmwater_circulation_mode(const char * value, const int8_t id) {
    int v = 0;
    if (!Helpers::value2number(value, v)) {
        return false;
    }

    if (get_toggle_fetch(EMS_TYPE_UBAParameterWW)) {
        if (v < 7) {
            LOG_INFO(F("Setting circulation mode %dx3min"), v);
        } else {
            LOG_INFO(F("Setting circulation mode continous"));
        }
        write_command(EMS_TYPE_UBAParameterWW, 6, v, EMS_TYPE_UBAParameterWW);
    }

    return true;
}

} // namespace emsesp
