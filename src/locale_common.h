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

// common words
MAKE_PSTR_WORD(debug)
MAKE_PSTR_WORD(exit)
MAKE_PSTR_WORD(help)
MAKE_PSTR_WORD(log)
MAKE_PSTR_WORD(logout)
MAKE_PSTR_WORD(enabled)
MAKE_PSTR_WORD(disabled)
MAKE_PSTR_WORD(set)
MAKE_PSTR_WORD(show)
MAKE_PSTR_WORD(su)
MAKE_PSTR_WORD(name)
MAKE_PSTR_WORD(scan)
MAKE_PSTR_WORD(password)
MAKE_PSTR_WORD(read)
MAKE_PSTR_WORD(version)
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
MAKE_PSTR_WORD(timeout)
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
MAKE_PSTR_WORD(customizations)
MAKE_PSTR_WORD(value)
MAKE_PSTR_WORD(entities)

// devices
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
MAKE_PSTR_WORD(unknown)
MAKE_PSTR_WORD(dallassensor)

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
MAKE_PSTR_LIST(1x3min, F("1x3min"))
MAKE_PSTR_LIST(2x3min, F("2x3min"))
MAKE_PSTR_LIST(3x3min, F("3x3min"))
MAKE_PSTR_LIST(4x3min, F("4x3min"))
MAKE_PSTR_LIST(5x3min, F("5x3min"))
MAKE_PSTR_LIST(6x3min, F("6x3min"))
MAKE_PSTR_LIST(auto, F("auto"))
MAKE_PSTR_LIST(na, F("n/a"))
MAKE_PSTR_LIST(rc3x, F("rc3x"))
MAKE_PSTR_LIST(rc20, F("rc20"))
MAKE_PSTR_LIST(fb10, F("fb10"))
MAKE_PSTR_LIST(fb100, F("fb100"))
MAKE_PSTR_LIST(dash, F("-"))
MAKE_PSTR_LIST(error, F("error"))
MAKE_PSTR_LIST(BLANK, F(""))
MAKE_PSTR_LIST(pwm, F("pwm"))

// Unit Of Measurement mapping - maps to DeviceValueUOM_s in emsdevice.cpp
// These don't need translating
MAKE_PSTR(blank, " ")
MAKE_PSTR(percent, "%")
MAKE_PSTR(degrees, "°C")
MAKE_PSTR(kwh, "kWh")
MAKE_PSTR(wh, "Wh")
MAKE_PSTR(bar, "bar")
MAKE_PSTR(ua, "uA")
MAKE_PSTR(lmin, "l/min")
MAKE_PSTR(kw, "kW")
MAKE_PSTR(w, "W")
MAKE_PSTR(kb, "KB")
MAKE_PSTR(dbm, "dBm")
MAKE_PSTR(fahrenheit, "°F")
MAKE_PSTR(mv, "mV")
MAKE_PSTR(sqm, "sqm")
MAKE_PSTR(m3, "m3")
MAKE_PSTR(l, "l")

// commands
MAKE_PSTR(info_cmd, "lists all values")
MAKE_PSTR(commands_cmd, "lists all commands")
MAKE_PSTR(entities_cmd, "lists all entities")
MAKE_PSTR(number, "number")
MAKE_PSTR(enum, "enum")
MAKE_PSTR(text, "text")

// TAG mapping - maps to DeviceValueTAG_s in emsdevice.cpp
// use empty string if want to suppress showing tags
// mqtt tags must not have spaces
MAKE_PSTR(tag_none, "")
MAKE_PSTR(tag_heartbeat, "")
MAKE_PSTR(tag_boiler_data_ww, "dhw")
MAKE_PSTR(tag_device_data, "")
MAKE_PSTR(tag_device_data_ww, "dhw")
MAKE_PSTR(tag_hc1, "hc1")
MAKE_PSTR(tag_hc2, "hc2")
MAKE_PSTR(tag_hc3, "hc3")
MAKE_PSTR(tag_hc4, "hc4")
MAKE_PSTR(tag_hc5, "hc5")
MAKE_PSTR(tag_hc6, "hc6")
MAKE_PSTR(tag_hc7, "hc7")
MAKE_PSTR(tag_hc8, "hc8")
MAKE_PSTR(tag_wwc1, "wwc1")
MAKE_PSTR(tag_wwc2, "wwc2")
MAKE_PSTR(tag_wwc3, "wwc3")
MAKE_PSTR(tag_wwc4, "wwc4")
MAKE_PSTR(tag_wwc5, "wwc5")
MAKE_PSTR(tag_wwc6, "wwc6")
MAKE_PSTR(tag_wwc7, "wwc7")
MAKE_PSTR(tag_wwc8, "wwc8")
MAKE_PSTR(tag_wwc9, "wwc9")
MAKE_PSTR(tag_wwc10, "wwc10")
MAKE_PSTR(tag_ahs, "ahs")
MAKE_PSTR(tag_hs1, "hs1")
MAKE_PSTR(tag_hs2, "hs2")
MAKE_PSTR(tag_hs3, "hs3")
MAKE_PSTR(tag_hs4, "hs4")
MAKE_PSTR(tag_hs5, "hs5")
MAKE_PSTR(tag_hs6, "hs6")
MAKE_PSTR(tag_hs7, "hs7")
MAKE_PSTR(tag_hs8, "hs8")
MAKE_PSTR(tag_hs9, "hs9")
MAKE_PSTR(tag_hs10, "hs10")
MAKE_PSTR(tag_hs11, "hs11")
MAKE_PSTR(tag_hs12, "hs12")
MAKE_PSTR(tag_hs13, "hs13")
MAKE_PSTR(tag_hs14, "hs14")
MAKE_PSTR(tag_hs15, "hs15")
MAKE_PSTR(tag_hs16, "hs16")

// MQTT topics and prefixes
MAKE_PSTR(heating_active, "heating_active")
MAKE_PSTR(tapwater_active, "tapwater_active")
MAKE_PSTR(response, "response")
MAKE_PSTR(tag_boiler_data_ww_mqtt, "ww")
MAKE_PSTR(tag_device_data_ww_mqtt, "")

// command parameters
MAKE_PSTR_LIST(div2, F("2"))
MAKE_PSTR_LIST(div4, F("4"))
MAKE_PSTR_LIST(div10, F("10"))
MAKE_PSTR_LIST(div60, F("60"))
MAKE_PSTR_LIST(div100, F("100"))
MAKE_PSTR_LIST(mul5, F("-5"))
MAKE_PSTR_LIST(mul10, F("-10"))
MAKE_PSTR_LIST(mul15, F("-15"))

MAKE_PSTR_LIST(climate, F("HA climate config creation")) // no full-name, hidden, only for creation

// syslog
MAKE_PSTR_LIST(list_syslog_level, F("off"), F("emerg"), F("alert"), F("crit"), F("error"), F("warn"), F("notice"), F("info"), F("debug"), F("trace"), F("all"))

// sensors
MAKE_PSTR_LIST(list_sensortype, F("none"), F("digital in"), F("counter"), F("adc"), F("timer"), F("rate"), F("digital out"), F("pwm 0"), F("pwm 1"), F("pwm 2"))

// watch
MAKE_PSTR_LIST(list_watch, F("off"), F("on"), F("raw"), F("unknown"))

// solar
MAKE_PSTR_LIST(list_cylprio, F("cyl_1"), F("cyl_2"))

//
// These next set are translated
//

// boiler lists

// TODO replace with enums - translate these too! or see if they work
MAKE_PSTR_LIST(enum_progMode, F("prog_1"), F("prog_2"))
MAKE_PSTR_LIST(enum_progMode4, F("prog_a"), F("prog_b"), F("prog_c"), F("prog_d"), F("prog_e"), F("prog_f"))
MAKE_PSTR_LIST(enum_climate, F("selTemp"), F("roomTemp"))

MAKE_PSTR_ENUM(enum_charge, FL_(chargepump), FL_(3wayvalve))
MAKE_PSTR_ENUM(enum_freq, FL_(off), FL_(1x3min), FL_(2x3min), FL_(3x3min), FL_(4x3min), FL_(5x3min), FL_(6x3min), FL_(continuous))
MAKE_PSTR_ENUM(enum_off_time_date_manual, FL_(off), FL_(time), FL_(date), FL_(manual))
MAKE_PSTR_ENUM(enum_comfort, FL_(hot), FL_(eco), FL_(intelligent))
MAKE_PSTR_ENUM(enum_comfort1, FL_(high_comfort), FL_(eco))
MAKE_PSTR_ENUM(enum_flow, FL_(off), FL_(flow), FL_(bufferedflow), FL_(buffer), FL_(layeredbuffer))
MAKE_PSTR_ENUM(enum_reset, FL_(dash), FL_(maintenance), FL_(error))

// thermostat lists

MAKE_PSTR_ENUM(enum_ibaMainDisplay,
               FL_(internal_temperature),
               FL_(internal_setpoint),
               FL_(external_temperature),
               FL_(burner_temperature),
               FL_(ww_temperature),
               FL_(functioning_mode),
               FL_(time),
               FL_(date),
               FL_(smoke_temperature))
MAKE_PSTR_ENUM(enum_ibaLanguage, FL_(german), FL_(dutch), FL_(french), FL_(italian))
MAKE_PSTR_ENUM(enum_ibaLanguage_RC30, FL_(german), FL_(dutch))
MAKE_PSTR_ENUM(enum_floordrystatus, FL_(off), FL_(start), FL_(heat), FL_(hold), FL_(cool), FL_(end))
MAKE_PSTR_ENUM(enum_ibaBuildingType, FL_(light), FL_(medium), FL_(heavy))
MAKE_PSTR_ENUM(enum_PID, FL_(fast), FL_(medium), FL_(slow))
MAKE_PSTR_ENUM(enum_wwMode, FL_(off), FL_(low), FL_(high), FL_(auto), FL_(own_prog))
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

MAKE_PSTR_ENUM(enum_modetype, FL_(eco), FL_(comfort))
MAKE_PSTR_ENUM(enum_modetype3, FL_(night), FL_(day))
MAKE_PSTR_ENUM(enum_modetype4, FL_(nofrost), FL_(eco), FL_(heat))
MAKE_PSTR_ENUM(enum_modetype5, FL_(off), FL_(on))

MAKE_PSTR_ENUM(enum_reducemode, FL_(nofrost), FL_(reduce), FL_(room), FL_(outdoor))
MAKE_PSTR_ENUM(enum_reducemode1, FL_(outdoor), FL_(room), FL_(reduce)) // RC310 values: 1-3
MAKE_PSTR_ENUM(enum_nofrostmode, FL_(off), FL_(room), FL_(outdoor))
MAKE_PSTR_ENUM(enum_nofrostmode1, FL_(room), FL_(outdoor), FL_(room_outdoor))

MAKE_PSTR_ENUM(enum_controlmode, FL_(off), FL_(optimized), FL_(simple), FL_(mpc), FL_(room), FL_(power), FL_(constant))
MAKE_PSTR_ENUM(enum_controlmode1, FL_(weather_compensated), FL_(outside_basepoint), FL_(na), FL_(room)) // RC310 1-4
MAKE_PSTR_ENUM(enum_controlmode2, FL_(outdoor), FL_(room))
MAKE_PSTR_ENUM(enum_control, FL_(off), FL_(rc20), FL_(rc3x))
MAKE_PSTR_ENUM(enum_j_control, FL_(off), FL_(fb10), FL_(fb100))

MAKE_PSTR_ENUM(enum_switchmode, FL_(off), FL_(eco), FL_(comfort), FL_(heat))

MAKE_PSTR_ENUM(enum_dayOfWeek, FL_(day_mo), FL_(day_tu), FL_(day_we), FL_(day_th), FL_(day_fr), FL_(day_sa), FL_(day_su), FL_(all))
MAKE_PSTR_ENUM(enum_progMode2, FL_(own_1), FL_(family), FL_(morning), FL_(evening), FL_(am), FL_(pm), FL_(midday), FL_(singles), FL_(seniors), FL_(new), FL_(own_2))
MAKE_PSTR_ENUM(enum_progMode3, FL_(family), FL_(morning), FL_(evening), FL_(am), FL_(pm), FL_(midday), FL_(singles), FL_(seniors))
MAKE_PSTR_ENUM(enum_hybridStrategy, FL_(co2_optimized), FL_(cost_optimized), FL_(outside_temp_switched), FL_(co2_cost_mix))

// heat pump
MAKE_PSTR_ENUM(enum_hpactivity, FL_(none), FL_(heating), FL_(cooling), FL_(hot_water), FL_(pool))

// solar
MAKE_PSTR_ENUM(enum_solarmode, FL_(constant), FL_(pwm), FL_(analog))
MAKE_PSTR_ENUM(enum_collectortype, FL_(flat), FL_(vacuum))
MAKE_PSTR_ENUM(enum_wwStatus2, FL_(BLANK), FL_(BLANK), FL_(BLANK), FL_(no_heat), FL_(BLANK), FL_(BLANK), FL_(heatrequest), FL_(BLANK), FL_(disinfecting), FL_(hold))

// mixer
MAKE_PSTR_ENUM(enum_shunt, FL_(stopped), FL_(opening), FL_(closing), FL_(open), FL_(close))
MAKE_PSTR_ENUM(enum_wwProgMode, FL_(std_prog), FL_(own_prog))

#pragma GCC diagnostic pop
