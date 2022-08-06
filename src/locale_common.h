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
MAKE_PSTR_WORD(auto)
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
MAKE_PSTR_WORD(error)
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

MAKE_STR(productid_fmt, "%s EMS ProductID")

// more common names that don't need translations
MAKE_PSTR_WORD(1x3min)
MAKE_PSTR_WORD(2x3min)
MAKE_PSTR_WORD(3x3min)
MAKE_PSTR_WORD(4x3min)
MAKE_PSTR_WORD(5x3min)
MAKE_PSTR_WORD(6x3min)
MAKE_PSTR(na, "n/a")
MAKE_PSTR_WORD(fb10)
MAKE_PSTR_WORD(fb100)

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

// syslog
MAKE_PSTR_LIST(enum_syslog_level, F_(off), F("emerg"), F("alert"), F("crit"), F_(error), F("warn"), F("notice"), F_(info), F_(debug), F("trace"), F("all"))

// watch
MAKE_PSTR_LIST(enum_watch, F_(off), F_(on), F_(raw), F_(unknown))

// sensors
MAKE_PSTR_LIST(enum_sensortype, F("none"), F("digital in"), F("counter"), F("adc"), F("timer"), F("rate"), F("digital out"), F("pwm 0"), F("pwm 1"), F("pwm 2"))

// command descriptions
MAKE_PSTR(info_cmd, "lists all values")
MAKE_PSTR(commands_cmd, "lists all commands")
MAKE_PSTR(entities_cmd, "lists all entities")

MAKE_PSTR_WORD(number)
MAKE_PSTR_WORD(enum)
MAKE_PSTR_WORD(text)

MAKE_PSTR_WORD(2)
MAKE_PSTR_WORD(4)
MAKE_PSTR_WORD(10)
MAKE_PSTR_WORD(100)
MAKE_PSTR_WORD(60)

MAKE_PSTR_LIST(div2, F_(2))
MAKE_PSTR_LIST(div4, F_(4))
MAKE_PSTR_LIST(div10, F_(10))
MAKE_PSTR_LIST(div60, F_(60))
MAKE_PSTR_LIST(div100, F_(100))
MAKE_PSTR_LIST(mul5, F("-5"))
MAKE_PSTR_LIST(mul10, F("-10"))
MAKE_PSTR_LIST(mul15, F("-15"))

MAKE_PSTR_WORD(rc3x)
MAKE_PSTR_WORD(rc20)

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

// Home Assistant
MAKE_PSTR_LIST(ID, F_(id)) // id used to store the device ID. empty full name so only gets displayed in the MQTT payload

// MQTT topics and prefixes
MAKE_PSTR_WORD(heating_active)
MAKE_PSTR_WORD(tapwater_active)
MAKE_PSTR_WORD(response)
MAKE_PSTR(tag_boiler_data_ww_mqtt, "ww")
MAKE_PSTR(tag_device_data_ww_mqtt, "")

// the following lists are generated

// boiler lists
MAKE_PSTR_LIST(tpl_date, F("Format: < dd.mm.yyyy >")) // template for text input
MAKE_PSTR_LIST(enum_off_time_date_manual, F_(off), F_(time), F_(date), F_(manual))
MAKE_PSTR_LIST(enum_freq, F_(off), F_(1x3min), F_(2x3min), F_(3x3min), F_(4x3min), F_(5x3min), F_(6x3min), F_(continuous))
MAKE_PSTR_LIST(enum_charge, F_(chargepump), F_(3wayvalve))
MAKE_PSTR_LIST(enum_comfort, F_(hot), F_(eco), F_(intelligent))
MAKE_PSTR_LIST(enum_comfort1, F_(high_comfort), F_(eco))
MAKE_PSTR_LIST(enum_flow, F_(off), F_(flow), F_(bufferedflow), F_(buffer), F_(layeredbuffer))
MAKE_PSTR_LIST(enum_reset, F("-"), F_(maintenance), F_(error))

// thermostat lists
MAKE_PSTR_LIST(enum_ibaMainDisplay,
               F_(internal_temperature),
               F_(internal_setpoint),
               F_(external_temperature),
               F_(burner_temperature),
               F_(ww_temperature),
               F_(functioning_mode),
               F_(time),
               F_(date),
               F_(smoke_temperature))
MAKE_PSTR_LIST(enum_ibaLanguage, F_(german), F_(dutch), F_(french), F_(italian))
MAKE_PSTR_LIST(enum_ibaLanguage_RC30, F_(german), F_(dutch))
MAKE_PSTR_LIST(enum_floordrystatus, F_(off), F_(start), F_(heat), F_(hold), F_(cool), F_(end))
MAKE_PSTR_LIST(enum_ibaBuildingType, F_(light), F_(medium), F_(heavy))
MAKE_PSTR_LIST(enum_PID, F_(fast), F_(medium), F_(slow))
MAKE_PSTR_LIST(enum_wwMode, F_(off), F_(low), F_(high), F_(auto), F_(own_prog))
MAKE_PSTR_LIST(enum_wwCircMode, F_(off), F_(on), F_(auto), F_(own_prog))
MAKE_PSTR_LIST(enum_wwMode2, F_(off), F_(on), F_(auto))
MAKE_PSTR_LIST(enum_wwMode3, F_(on), F_(off), F_(auto))
MAKE_PSTR_LIST(enum_heatingtype, F_(off), F_(radiator), F_(convector), F_(floor))
MAKE_PSTR_LIST(enum_summermode, F_(summer), F_(auto), F_(winter))
MAKE_PSTR_LIST(enum_hpoperatingmode, F_(off), F_(auto), F_(heating), F_(cooling))
MAKE_PSTR_LIST(enum_summer, F_(winter), F_(summer))
MAKE_PSTR_LIST(enum_operatingstate, F_(heating), F_(off), F_(cooling))

MAKE_PSTR_LIST(enum_mode, F_(manual), F_(auto))                      // RC100, RC300, RC310
MAKE_PSTR_LIST(enum_mode2, F_(off), F_(manual), F_(auto))            // RC20
MAKE_PSTR_LIST(enum_mode3, F_(night), F_(day), F_(auto))             // RC35, RC30, RC25
MAKE_PSTR_LIST(enum_mode4, F_(nofrost), F_(eco), F_(heat), F_(auto)) // JUNKERS
MAKE_PSTR_LIST(enum_mode5, F_(auto), F_(off))                        // CRF
MAKE_PSTR_LIST(enum_mode6, F_(nofrost), F_(night), F_(day))          // RC10

MAKE_PSTR_LIST(enum_modetype, F_(eco), F_(comfort))
MAKE_PSTR_LIST(enum_modetype3, F_(night), F_(day))
MAKE_PSTR_LIST(enum_modetype4, F_(nofrost), F_(eco), F_(heat))
MAKE_PSTR_LIST(enum_modetype5, F_(off), F_(on))

MAKE_PSTR_LIST(enum_reducemode, F_(nofrost), F_(reduce), F_(room), F_(outdoor))
MAKE_PSTR_LIST(enum_reducemode1, F_(outdoor), F_(room), F_(reduce)) // RC310 values: 1-3
MAKE_PSTR_LIST(enum_nofrostmode, F_(off), F_(room), F_(outdoor))
MAKE_PSTR_LIST(enum_nofrostmode1, F_(room), F_(outdoor), F_(room_outdoor))

MAKE_PSTR_LIST(enum_controlmode, F_(off), F_(optimized), F_(simple), F_(mpc), F_(room), F_(power), F_(constant))
MAKE_PSTR_LIST(enum_controlmode1, F_(weather_compensated), F_(outside_basepoint), F_(na), F_(room)) // RC310 1-4
MAKE_PSTR_LIST(enum_controlmode2, F_(outdoor), F_(room))
MAKE_PSTR_LIST(enum_control, F_(off), F_(rc20), F_(rc3x))
MAKE_PSTR_LIST(enum_j_control, F_(off), F_(fb10), F_(fb100))

MAKE_PSTR_LIST(enum_switchmode, F_(off), F_(eco), F_(comfort), F_(heat))

// heatpump
MAKE_PSTR_LIST(enum_hpactivity, F_(none), F_(heating), F_(cooling), F_(hot_water), F_(pool))
