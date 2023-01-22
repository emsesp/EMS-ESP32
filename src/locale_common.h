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

#pragma once
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"

// clang-format off

/*
 * THIS FILE CONTAINS STANDARD STRING LITERALS THAT DON'T NEED LANGUAGE TRANSLATIONS
 */

// common words
MAKE_PSTR_WORD(exit)
MAKE_PSTR_WORD(help)
MAKE_PSTR_WORD(log)
MAKE_PSTR_WORD(set)
MAKE_PSTR_WORD(show)
MAKE_PSTR_WORD(su)
MAKE_PSTR_WORD(scan)
MAKE_PSTR_WORD(password)
MAKE_PSTR_WORD(read)
MAKE_PSTR_WORD(values)
MAKE_PSTR_WORD(system)
MAKE_PSTR_WORD(fetch)
MAKE_PSTR_WORD(restart)
MAKE_PSTR_WORD(format)
MAKE_PSTR_WORD(raw)
MAKE_PSTR_WORD(watch)
MAKE_PSTR_WORD(syslog)
MAKE_PSTR_WORD(send)
MAKE_PSTR_WORD(telegram)
MAKE_PSTR_WORD(bus_id)
MAKE_PSTR_WORD(tx_mode)
MAKE_PSTR_WORD(ems)
MAKE_PSTR_WORD(devices)
MAKE_PSTR_WORD(shower)
MAKE_PSTR_WORD(mqtt)
MAKE_PSTR_WORD(emsesp)
MAKE_PSTR_WORD(connected)
MAKE_PSTR_WORD(disconnected)
MAKE_PSTR_WORD(passwd)
MAKE_PSTR_WORD(hostname)
MAKE_PSTR_WORD(wifi)
MAKE_PSTR_WORD(reconnect)
MAKE_PSTR_WORD(ssid)
MAKE_PSTR_WORD(heartbeat)
MAKE_PSTR_WORD(users)
MAKE_PSTR_WORD(publish)
MAKE_PSTR_WORD(board_profile)
MAKE_PSTR_WORD(setvalue)

// for commands
MAKE_PSTR_WORD(call)
MAKE_PSTR_WORD(cmd)
MAKE_PSTR_WORD(id)
MAKE_PSTR_WORD(hc)
MAKE_PSTR_WORD(wwc)
MAKE_PSTR_WORD(device)
MAKE_PSTR_WORD(data)
MAKE_PSTR_WORD(command)
MAKE_PSTR_WORD(commands)
MAKE_PSTR_WORD(info)
MAKE_PSTR_WORD(settings)
MAKE_PSTR_WORD(value)
MAKE_PSTR_WORD(entities)

// device types - lowercase, used in MQTT
MAKE_PSTR_WORD(boiler)
MAKE_PSTR_WORD(thermostat)
MAKE_PSTR_WORD(switch)
MAKE_PSTR_WORD(solar)
MAKE_PSTR_WORD(mixer)
MAKE_PSTR_WORD(gateway)
MAKE_PSTR_WORD(controller)
MAKE_PSTR_WORD(connect)
MAKE_PSTR_WORD(heatpump)
MAKE_PSTR_WORD(generic)
MAKE_PSTR_WORD(analogsensor)
MAKE_PSTR_WORD(dallassensor)
MAKE_PSTR_WORD(alert)
MAKE_PSTR_WORD(pump)
MAKE_PSTR_WORD(heatsource)

MAKE_PSTR(number, "number")
MAKE_PSTR(enum, "enum")
MAKE_PSTR(text, "text")

// Console
MAKE_PSTR(EMSESP, "EMS-ESP")
MAKE_PSTR(host_fmt, "Host: %s")
MAKE_PSTR(port_fmt, "Port: %d")
MAKE_PSTR(hostname_fmt, "Hostname: %s")
MAKE_PSTR(board_profile_fmt, "Board Profile: %s")
MAKE_PSTR(mark_interval_fmt, "Mark interval: %lus")
MAKE_PSTR(wifi_ssid_fmt, "WiFi SSID: %s")
MAKE_PSTR(wifi_password_fmt, "WiFi Password: %S")
MAKE_PSTR(tx_mode_fmt, "Tx mode: %d")
MAKE_PSTR(bus_id_fmt, "Bus ID: %02X")
MAKE_PSTR(log_level_fmt, "Log level: %s")
MAKE_PSTR(cmd_optional, "[cmd]")
MAKE_PSTR(ha_optional, "[ha]")
MAKE_PSTR(deep_optional, "[deep]")
MAKE_PSTR(watchid_optional, "[ID]")
MAKE_PSTR(watch_format_optional, "[off | on | raw | unknown]")
MAKE_PSTR(invalid_watch, "Invalid watch type")
MAKE_PSTR(data_mandatory, "\"XX XX ...\"")
MAKE_PSTR(asterisks, "********")
MAKE_PSTR(n_mandatory, "<n>")
MAKE_PSTR(sensorid_optional, "[sensor ID]")
MAKE_PSTR(id_optional, "[id|hc]")
MAKE_PSTR(data_optional, "[data]")
MAKE_PSTR(offset_optional, "[offset]")
MAKE_PSTR(length_optional, "[length]")
MAKE_PSTR(typeid_mandatory, "<type ID>")
MAKE_PSTR(deviceid_mandatory, "<device ID>")
MAKE_PSTR(device_type_optional, "[device]")
MAKE_PSTR(invalid_log_level, "Invalid log level")
MAKE_PSTR(log_level_optional, "[level]")
MAKE_PSTR(name_mandatory, "<name>")
MAKE_PSTR(name_optional, "[name]")
MAKE_PSTR(new_password_prompt1, "Enter new password: ")
MAKE_PSTR(new_password_prompt2, "Retype new password: ")
MAKE_PSTR(password_prompt, "Password: ")
MAKE_PSTR(unset, "<unset>")

// more common names that don't need translations
MAKE_PSTR_LIST(1x3min, "1x3min")
MAKE_PSTR_LIST(2x3min, "2x3min")
MAKE_PSTR_LIST(3x3min, "3x3min")
MAKE_PSTR_LIST(4x3min, "4x3min")
MAKE_PSTR_LIST(5x3min, "5x3min")
MAKE_PSTR_LIST(6x3min, "6x3min")
MAKE_PSTR_LIST(auto, "auto")
MAKE_PSTR_LIST(rc3x, "RC3x")
MAKE_PSTR_LIST(rc20, "RC20")
MAKE_PSTR_LIST(fb10, "FB10")
MAKE_PSTR_LIST(fb100, "FB100")
MAKE_PSTR_LIST(dash, "-")
MAKE_PSTR_LIST(BLANK, "")
MAKE_PSTR_LIST(pwm, "pwm")
MAKE_PSTR_LIST(pwm_invers, "pwm inverse")
MAKE_PSTR_LIST(mpc, "mpc")
MAKE_PSTR_LIST(tempauto, "temp auto")
MAKE_PSTR_LIST(bypass, "bypass")
MAKE_PSTR_LIST(mixer, "mixer")
MAKE_PSTR_LIST(monovalent, "monovalent")
MAKE_PSTR_LIST(bivalent, "bivalent")
MAKE_PSTR_LIST(n_o, "n_o")
MAKE_PSTR_LIST(n_c, "n_c")
MAKE_PSTR_LIST(prog1, "prog 1")
MAKE_PSTR_LIST(prog2, "prog 2")
MAKE_PSTR_LIST(proga, "prog a")
MAKE_PSTR_LIST(progb, "prog b")
MAKE_PSTR_LIST(progc, "prog c")
MAKE_PSTR_LIST(progd, "prog d")
MAKE_PSTR_LIST(proge, "prog e")
MAKE_PSTR_LIST(progf, "prog f")
MAKE_PSTR_LIST(rc35, "RC35")
MAKE_PSTR_LIST(0kW, "0 kW")
MAKE_PSTR_LIST(2kW, "2 kW")
MAKE_PSTR_LIST(3kW, "3 kW")
MAKE_PSTR_LIST(4kW, "4 kW")
MAKE_PSTR_LIST(6kW, "6 kW")
MAKE_PSTR_LIST(9kW, "9 kW")

// templates - this are not translated and will be saved under options_single
MAKE_PSTR_LIST(tpl_datetime, "Format: < NTP | dd.mm.yyyy-hh:mm:ss-day(0-6)-dst(0/1) >")
MAKE_PSTR_LIST(tpl_switchtime, "Format: <nn> [ not_set | day hh:mm on|off ]")
MAKE_PSTR_LIST(tpl_switchtime1, "Format: <nn> [ not_set | day hh:mm Tn ]")
MAKE_PSTR_LIST(tpl_holidays, "Format: < dd.mm.yyyy-dd.mm.yyyy >")
MAKE_PSTR_LIST(tpl_date, "Format: < dd.mm.yyyy >")
MAKE_PSTR_LIST(tpl_input, "Format: <inv>[<evu1><evu2><evu3><comp><aux><cool><heat><dhw><pv>]")
MAKE_PSTR_LIST(tpl_input4, "Format: <inv>[<comp><aux><cool><heat><dhw><pv>]")

// Unit Of Measurement mapping - maps to DeviceValueUOM_s in emsdevice.cpp
// Translating hours/minute/seconds are done in emsdevice.cpp (uom_to_string())
MAKE_PSTR(uom_blank, " ")
MAKE_PSTR(uom_percent, "%")
MAKE_PSTR(uom_degrees, "°C")
MAKE_PSTR(uom_kwh, "kWh")
MAKE_PSTR(uom_wh, "Wh")
MAKE_PSTR(uom_bar, "bar")
MAKE_PSTR(uom_ua, "µA")
MAKE_PSTR(uom_lmin, "l/min")
MAKE_PSTR(uom_kw, "kW")
MAKE_PSTR(uom_w, "W")
MAKE_PSTR(uom_kb, "KB")
MAKE_PSTR(uom_dbm, "dBm")
MAKE_PSTR(uom_fahrenheit, "°F")
MAKE_PSTR(uom_mv, "mV")
MAKE_PSTR(uom_sqm, "m²")
MAKE_PSTR(uom_m3, "m³")
MAKE_PSTR(uom_l, "l")
MAKE_PSTR(uom_kmin, "K*min")
MAKE_PSTR(uom_k, "K")

// MQTT topics and prefixes
MAKE_PSTR(heating_active, "heating_active")
MAKE_PSTR(tapwater_active, "tapwater_active")
MAKE_PSTR(response, "response")
MAKE_PSTR(tag_boiler_data_ww_mqtt, "ww")
MAKE_PSTR(tag_device_data_ww_mqtt, "")

// Discovery/Home Assistant - this is special device entity for 'climate', and has no translations
MAKE_PSTR_LIST(climate, "Discovery current room temperature")

// syslog
MAKE_PSTR_LIST(list_syslog_level, "off", "emerg", "alert", "crit", "error", "warn", "notice", "info", "debug", "trace", "all")

// sensors
MAKE_PSTR_LIST(counter, "counter")
MAKE_PSTR_LIST(digital_out, "digital_out")
MAKE_PSTR_LIST(list_sensortype, "none", "digital in", "counter", "adc", "timer", "rate", "digital out", "pwm 0", "pwm 1", "pwm 2")

// watch
MAKE_PSTR_LIST(list_watch, "off", "on", "raw", "unknown")

/*
 * The rest below are Enums and generated from translations lists
 */

MAKE_PSTR_ENUM(enum_cylprio, FL_(cyl1), FL_(cyl2))
MAKE_PSTR_ENUM(enum_progMode, FL_(prog1), FL_(prog2))
MAKE_PSTR_ENUM(enum_progMode4, FL_(proga), FL_(progb), FL_(progc), FL_(progd), FL_(proge), FL_(progf))
MAKE_PSTR_ENUM(enum_climate, FL_(seltemp), FL_(roomtemp))
MAKE_PSTR_ENUM(enum_charge, FL_(chargepump), FL_(3wayvalve))
MAKE_PSTR_ENUM(enum_freq, FL_(off), FL_(1x3min), FL_(2x3min), FL_(3x3min), FL_(4x3min), FL_(5x3min), FL_(6x3min), FL_(continuous))
MAKE_PSTR_ENUM(enum_off_time_date_manual, FL_(off), FL_(time), FL_(date), FL_(manual))
MAKE_PSTR_ENUM(enum_comfort, FL_(hot), FL_(eco), FL_(intelligent))
MAKE_PSTR_ENUM(enum_comfort1, FL_(high_comfort), FL_(eco))
MAKE_PSTR_ENUM(enum_comfort2, FL_(eco), FL_(high_comfort))
MAKE_PSTR_ENUM(enum_flow, FL_(off), FL_(flow), FL_(bufferedflow), FL_(buffer), FL_(layeredbuffer))
MAKE_PSTR_ENUM(enum_reset, FL_(dash), FL_(maintenance), FL_(error))
MAKE_PSTR_ENUM(enum_maxHeat, FL_(0kW), FL_(2kW), FL_(3kW), FL_(4kW), FL_(6kW), FL_(9kW))

// thermostat lists
MAKE_PSTR_ENUM(enum_ibaMainDisplay, FL_(internal_temperature), FL_(internal_setpoint), FL_(external_temperature), FL_(burner_temperature), FL_(ww_temperature), FL_(functioning_mode), FL_(time), FL_(date), FL_(smoke_temperature))
MAKE_PSTR_ENUM(enum_ibaLanguage, FL_(german), FL_(dutch), FL_(french), FL_(italian))
MAKE_PSTR_ENUM(enum_ibaLanguage_RC30, FL_(german), FL_(dutch))
MAKE_PSTR_ENUM(enum_floordrystatus, FL_(off), FL_(start), FL_(heat), FL_(hold), FL_(cool), FL_(end))
MAKE_PSTR_ENUM(enum_ibaBuildingType, FL_(light), FL_(medium), FL_(heavy))
MAKE_PSTR_ENUM(enum_PID, FL_(fast), FL_(medium), FL_(slow))
MAKE_PSTR_ENUM(enum_wwMode, FL_(off), FL_(normal), FL_(comfort), FL_(auto), FL_(own_prog), FL_(eco))
MAKE_PSTR_ENUM(enum_wwCircMode, FL_(off), FL_(on), FL_(auto), FL_(own_prog))
MAKE_PSTR_ENUM(enum_wwMode2, FL_(off), FL_(on), FL_(auto))
MAKE_PSTR_ENUM(enum_wwMode3, FL_(on), FL_(off), FL_(auto))
MAKE_PSTR_ENUM(enum_heatingtype, FL_(off), FL_(radiator), FL_(convector), FL_(floor))
MAKE_PSTR_ENUM(enum_summermode, FL_(summer), FL_(auto), FL_(winter))
MAKE_PSTR_ENUM(enum_hpoperatingmode, FL_(off), FL_(auto), FL_(heating), FL_(cooling))
MAKE_PSTR_ENUM(enum_summer, FL_(winter), FL_(summer))
MAKE_PSTR_ENUM(enum_operatingstate, FL_(heating), FL_(off), FL_(cooling))

MAKE_PSTR_ENUM(enum_mode, FL_(manual), FL_(auto))                        // RC100, RC300, RC310
MAKE_PSTR_ENUM(enum_mode2, FL_(off), FL_(manual), FL_(auto))             // RC20
MAKE_PSTR_ENUM(enum_mode3, FL_(night), FL_(day), FL_(auto))              // RC35, RC30, RC25
MAKE_PSTR_ENUM(enum_mode4, FL_(nofrost), FL_(eco), FL_(heat), FL_(auto)) // JUNKERS
MAKE_PSTR_ENUM(enum_mode5, FL_(auto), FL_(off))                          // CRF
MAKE_PSTR_ENUM(enum_mode6, FL_(nofrost), FL_(night), FL_(day))           // RC10
MAKE_PSTR_ENUM(enum_mode_ha, FL_(off), FL_(heat), FL_(auto))             // HA climate

MAKE_PSTR_ENUM(enum_modetype, FL_(eco), FL_(comfort))
MAKE_PSTR_ENUM(enum_modetype3, FL_(night), FL_(day))
MAKE_PSTR_ENUM(enum_modetype4, FL_(nofrost), FL_(eco), FL_(heat))
MAKE_PSTR_ENUM(enum_modetype5, FL_(off), FL_(on))

MAKE_PSTR_ENUM(enum_reducemode, FL_(nofrost), FL_(reduce), FL_(room), FL_(outdoor))
MAKE_PSTR_ENUM(enum_reducemode1, FL_(outdoor), FL_(room), FL_(reduce)) // RC310 values: 1-3
MAKE_PSTR_ENUM(enum_nofrostmode, FL_(off), FL_(room), FL_(outdoor))
MAKE_PSTR_ENUM(enum_nofrostmode1, FL_(room), FL_(outdoor), FL_(room_outdoor))

MAKE_PSTR_ENUM(enum_controlmode, FL_(off), FL_(optimized), FL_(simple), FL_(mpc), FL_(room), FL_(power), FL_(constant))
MAKE_PSTR_ENUM(enum_controlmode1, FL_(weather_compensated), FL_(outside_basepoint), FL_(na), FL_(room), FL_(power), FL_(constant)) // RC310 1-4
MAKE_PSTR_ENUM(enum_controlmode2, FL_(outdoor), FL_(room))
MAKE_PSTR_ENUM(enum_control, FL_(off), FL_(rc20), FL_(rc3x))
MAKE_PSTR_ENUM(enum_j_control, FL_(off), FL_(fb10), FL_(fb100))
MAKE_PSTR_ENUM(enum_roomsensor, FL_(extern), FL_(intern), FL_(auto))

MAKE_PSTR_ENUM(enum_switchmode, FL_(off), FL_(eco), FL_(comfort), FL_(heat))

MAKE_PSTR_ENUM(enum_dayOfWeek, FL_(day_mo), FL_(day_tu), FL_(day_we), FL_(day_th), FL_(day_fr), FL_(day_sa), FL_(day_su), FL_(all))
MAKE_PSTR_ENUM(enum_progMode2, FL_(own_1), FL_(family), FL_(morning), FL_(evening), FL_(am), FL_(pm), FL_(midday), FL_(singles), FL_(seniors), FL_(new), FL_(own_2))
MAKE_PSTR_ENUM(enum_progMode3, FL_(family), FL_(morning), FL_(evening), FL_(am), FL_(pm), FL_(midday), FL_(singles), FL_(seniors))
MAKE_PSTR_ENUM(enum_hybridStrategy, FL_(co2_optimized), FL_(cost_optimized), FL_(outside_temp_switched), FL_(co2_cost_mix))
MAKE_PSTR_ENUM(enum_hybridStrategy1, FL_(cost_optimized), FL_(co2_optimized), FL_(outside_temp_alt), FL_(outside_temp_par), FL_(hp_prefered), FL_(boiler_only))
MAKE_PSTR_ENUM(enum_lowNoiseMode, FL_(off), FL_(reduced_output), FL_(switchoff), FL_(perm))

// heat pump
MAKE_PSTR_ENUM(enum_hpactivity, FL_(none), FL_(heating), FL_(cooling), FL_(hot_water), FL_(pool), FL_(unknown), FL_(defrost))
MAKE_PSTR_ENUM(enum_silentMode, FL_(off), FL_(auto), FL_(on))

// solar
MAKE_PSTR_ENUM(enum_solarmode, FL_(constant), FL_(pwm), FL_(analog))
MAKE_PSTR_ENUM(enum_collectortype, FL_(flat), FL_(vacuum))
MAKE_PSTR_ENUM(enum_wwStatus2, FL_(BLANK), FL_(BLANK), FL_(BLANK), FL_(no_heat), FL_(BLANK), FL_(BLANK), FL_(heatrequest), FL_(BLANK), FL_(disinfecting), FL_(hold))

// mixer
MAKE_PSTR_ENUM(enum_shunt, FL_(stopped), FL_(opening), FL_(closing), FL_(open), FL_(close))
MAKE_PSTR_ENUM(enum_wwProgMode, FL_(std_prog), FL_(own_prog))

// AM200 lists
MAKE_PSTR_ENUM(enum_vr2Config, FL_(off), FL_(bypass))
MAKE_PSTR_ENUM(enum_aPumpSignal, FL_(off), FL_(pwm), FL_(pwm_invers))
MAKE_PSTR_ENUM(enum_bufBypass, FL_(no), FL_(mixer), FL_(valve))
MAKE_PSTR_ENUM(enum_blockMode, FL_(off), FL_(auto), FL_(blocking))
MAKE_PSTR_ENUM(enum_bufConfig, FL_(off), FL_(monovalent), FL_(bivalent))
MAKE_PSTR_ENUM(enum_blockTerm, FL_(n_o), FL_(n_c))

#pragma GCC diagnostic pop

// clang-format on
