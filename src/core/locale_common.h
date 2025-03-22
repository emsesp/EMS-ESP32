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

#pragma once
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"

// clang-format off

/*
 * THIS FILE CONTAINS STANDARD STRING LITERALS THAT DON'T NEED LANGUAGE TRANSLATIONS
 */

// common words
MAKE_WORD(exit)
MAKE_WORD(help)
MAKE_WORD(log)
MAKE_WORD(set)
MAKE_WORD(show)
MAKE_WORD(su)
MAKE_WORD(scan)
MAKE_WORD(password)
MAKE_WORD(admin)
MAKE_WORD(read)
MAKE_WORD(values)
MAKE_WORD(system)
MAKE_WORD(fetch)
MAKE_WORD(restart)
MAKE_WORD(format)
MAKE_WORD(raw)
MAKE_WORD(watch)
MAKE_WORD(syslog)
MAKE_WORD(send)
MAKE_WORD(telegram)
MAKE_WORD(bus_id)
MAKE_WORD(tx_mode)
MAKE_WORD(showertimer)
MAKE_WORD(showeralert)
MAKE_WORD(ems)
MAKE_WORD(devices)
MAKE_WORD(shower)
MAKE_WORD(mqtt)
MAKE_WORD(modbus)
MAKE_WORD(emsesp)
MAKE_WORD(connected)
MAKE_WORD(disconnected)
MAKE_WORD(passwd)
MAKE_WORD(hostname)
MAKE_WORD(wifi)
MAKE_WORD(reconnect)
MAKE_WORD(ssid)
MAKE_WORD(heartbeat)
MAKE_WORD(users)
MAKE_WORD(publish)
MAKE_WORD(board_profile)
MAKE_WORD(setvalue)
MAKE_WORD(service)
MAKE_WORD(message)

// for commands
MAKE_WORD(call)
MAKE_WORD(cmd)
MAKE_WORD(id)
MAKE_WORD(hc)
MAKE_WORD(dhw)
MAKE_WORD(device)
MAKE_WORD(data)
MAKE_WORD(command)
MAKE_WORD(commands)
MAKE_WORD(info)
MAKE_WORD(settings)
MAKE_WORD(value)
MAKE_WORD(entities)
MAKE_WORD(coldshot)

// device types - lowercase, used in MQTT
MAKE_WORD(boiler)
MAKE_WORD(thermostat)
MAKE_WORD_CUSTOM(switcher, "switch")
MAKE_WORD(solar)
MAKE_WORD(mixer)
MAKE_WORD(gateway)
MAKE_WORD(controller)
MAKE_WORD(connect)
MAKE_WORD(heatpump)
MAKE_WORD(generic)
MAKE_WORD(analogsensor)
MAKE_WORD(temperaturesensor)
MAKE_WORD(alert)
MAKE_WORD(pump)
MAKE_WORD(extension)
MAKE_WORD(heatsource)
MAKE_WORD(scheduler)
MAKE_WORD(custom)
MAKE_WORD(ventilation)
MAKE_WORD(water)
MAKE_WORD(pool)

// brands
MAKE_WORD_CUSTOM(bosch, "Bosch")
MAKE_WORD_CUSTOM(junkers, "Junkers")
MAKE_WORD_CUSTOM(buderus, "Buderus")
MAKE_WORD_CUSTOM(nefit, "Nefit")
MAKE_WORD_CUSTOM(sieger, "Sieger")
MAKE_WORD_CUSTOM(worcester, "Worcester")
MAKE_WORD_CUSTOM(ivt, "IVT")
MAKE_WORD_CUSTOM(no_brand, "")

// types
MAKE_WORD_CUSTOM(number, "number")
MAKE_WORD_CUSTOM(enum, "enum")
MAKE_WORD_CUSTOM(text, "text")

// Console
MAKE_WORD_CUSTOM(EMSESP, "EMS-ESP")
MAKE_WORD_CUSTOM(host_fmt, "Host: %s")
MAKE_WORD_CUSTOM(port_fmt, "Port: %d")
MAKE_WORD_CUSTOM(hostname_fmt, "Hostname: %s")
MAKE_WORD_CUSTOM(mark_interval_fmt, "Mark interval: %lus")
MAKE_WORD_CUSTOM(wifi_ssid_fmt, "WiFi SSID: %s")
MAKE_WORD_CUSTOM(wifi_password_fmt, "WiFi Password: %S")
MAKE_WORD_CUSTOM(tx_mode_fmt, "Tx mode: %d")
MAKE_WORD_CUSTOM(bus_id_fmt, "Bus ID: %02X")
MAKE_WORD_CUSTOM(log_level_fmt, "Log level: %s")
MAKE_WORD_CUSTOM(cmd_optional, "[cmd]")
MAKE_WORD_CUSTOM(ha_optional, "[ha]")
MAKE_WORD_CUSTOM(deep_optional, "[deep]")
MAKE_WORD_CUSTOM(watchid_optional, "[ID]")
MAKE_WORD_CUSTOM(watch_format_optional, "[off | on | raw | unknown]")
MAKE_WORD_CUSTOM(invalid_watch, "Invalid watch type")
MAKE_WORD_CUSTOM(data_mandatory, "\"XX XX ...\"")
MAKE_WORD_CUSTOM(asterisks, "********")
MAKE_WORD_CUSTOM(n_mandatory, "<n>")
MAKE_WORD_CUSTOM(sensorid_optional, "[sensor ID]")
MAKE_WORD_CUSTOM(id_optional, "[id|hc]")
MAKE_WORD_CUSTOM(partitionname_optional, "[partitionname]")
MAKE_WORD_CUSTOM(data_optional, "[data]")
MAKE_WORD_CUSTOM(offset_optional, "[offset]")
MAKE_WORD_CUSTOM(length_optional, "[length]")
MAKE_WORD_CUSTOM(typeid_mandatory, "<type ID>")
MAKE_WORD_CUSTOM(deviceid_mandatory, "<deviceID>")
MAKE_WORD_CUSTOM(device_type_optional, "[device]")
MAKE_WORD_CUSTOM(invalid_log_level, "Invalid log level")
MAKE_WORD_CUSTOM(log_level_optional, "[level]")
MAKE_WORD_CUSTOM(show_commands, "[system | users | devices | log | ems | values | mqtt | commands]")
MAKE_WORD_CUSTOM(name_mandatory, "<name>")
MAKE_WORD_CUSTOM(name_optional, "[name]")
MAKE_WORD_CUSTOM(new_password_prompt1, "Enter new password: ")
MAKE_WORD_CUSTOM(new_password_prompt2, "Retype new password: ")
MAKE_WORD_CUSTOM(password_prompt, "Password: ")
MAKE_WORD_CUSTOM(unset, "<unset>")
MAKE_WORD_CUSTOM(enable_mandatory, "<enable | disable>")
MAKE_WORD_CUSTOM(service_mandatory, "<ap | mqtt | ntp>")

// more common names that don't need translations
MAKE_NOTRANSLATION(1x3min, "1x3min")
MAKE_NOTRANSLATION(2x3min, "2x3min")
MAKE_NOTRANSLATION(3x3min, "3x3min")
MAKE_NOTRANSLATION(4x3min, "4x3min")
MAKE_NOTRANSLATION(5x3min, "5x3min")
MAKE_NOTRANSLATION(6x3min, "6x3min")
MAKE_NOTRANSLATION(auto, "auto")
MAKE_NOTRANSLATION(rc3x, "RC3x")
MAKE_NOTRANSLATION(rc20, "RC20")
MAKE_NOTRANSLATION(fb10, "FB10")
MAKE_NOTRANSLATION(fb100, "FB100")
MAKE_NOTRANSLATION(rc310, "RC310")
MAKE_NOTRANSLATION(rc200, "RC200")
MAKE_NOTRANSLATION(rc100, "RC100")
MAKE_NOTRANSLATION(rc100h, "RC100H")
MAKE_NOTRANSLATION(tc100, "TC100")
MAKE_NOTRANSLATION(rc120rf, "RC120RF")
MAKE_NOTRANSLATION(rc220, "RC220/RT800")
MAKE_NOTRANSLATION(single, "single")
MAKE_NOTRANSLATION(dash, "-")
MAKE_NOTRANSLATION(BLANK, "")
MAKE_NOTRANSLATION(pwm, "pwm")
MAKE_NOTRANSLATION(pwm_invers, "pwm inverse")
MAKE_NOTRANSLATION(mpc, "mpc")
MAKE_NOTRANSLATION(tempauto, "temp auto")
MAKE_NOTRANSLATION(bypass, "bypass")
MAKE_NOTRANSLATION(mixer, "mixer")
MAKE_NOTRANSLATION(monovalent, "monovalent")
MAKE_NOTRANSLATION(bivalent, "bivalent")
MAKE_NOTRANSLATION(n_o, "n_o")
MAKE_NOTRANSLATION(n_c, "n_c")
MAKE_NOTRANSLATION(prog1, "prog 1")
MAKE_NOTRANSLATION(prog2, "prog 2")
MAKE_NOTRANSLATION(proga, "prog a")
MAKE_NOTRANSLATION(progb, "prog b")
MAKE_NOTRANSLATION(progc, "prog c")
MAKE_NOTRANSLATION(progd, "prog d")
MAKE_NOTRANSLATION(proge, "prog e")
MAKE_NOTRANSLATION(progf, "prog f")
MAKE_NOTRANSLATION(rc35, "RC35")
MAKE_NOTRANSLATION(0kW, "0 kW")
MAKE_NOTRANSLATION(2kW, "2 kW")
MAKE_NOTRANSLATION(3kW, "3 kW")
MAKE_NOTRANSLATION(4kW, "4 kW")
MAKE_NOTRANSLATION(6kW, "6 kW")
MAKE_NOTRANSLATION(9kW, "9 kW")
MAKE_NOTRANSLATION(L1, "L1")
MAKE_NOTRANSLATION(L2, "L2")
MAKE_NOTRANSLATION(L3, "L3")
MAKE_NOTRANSLATION(L4, "L4")

// templates - this are not translated and will be saved under options_single
MAKE_NOTRANSLATION(tpl_datetime, "NTP | dd.mm.yyyy-hh:mm:ss-day(0-6)-dst(0/1)")
MAKE_NOTRANSLATION(tpl_switchtime, "<nn> [ not_set | day hh:mm on|off ]")
MAKE_NOTRANSLATION(tpl_switchtime1, "<nn> [ not_set | day hh:mm Tn ]")
MAKE_NOTRANSLATION(tpl_holidays, "dd.mm.yyyy-dd.mm.yyyy")
MAKE_NOTRANSLATION(tpl_date, "dd.mm.yyyy")
MAKE_NOTRANSLATION(tpl_input, "<inv>[<evu1><evu2><evu3><comp><aux><cool><heat><dhw><pv>]")
MAKE_NOTRANSLATION(tpl_input4, "<inv>[<comp><aux><cool><heat><dhw><pv>]")

#if defined(EMSESP_TEST)
MAKE_NOTRANSLATION(test_cmd, "run a test")
#endif

// TAG mapping - maps to DeviceValueTAG_s in emsdevicevalue.cpp
// use empty string if want to suppress showing tags
// mqtt tags must not have spaces
MAKE_NOTRANSLATION(tag_none, "")
MAKE_NOTRANSLATION(tag_heartbeat, "")
MAKE_NOTRANSLATION(tag_device_data, "")

// Unit Of Measurement mapping - maps to DeviceValueUOM_s in emsdevice.cpp
// Translating hours/minute/seconds are done in emsdevice.cpp (uom_to_string())
MAKE_WORD_CUSTOM(uom_blank, " ")
MAKE_WORD_CUSTOM(uom_percent, "%")
MAKE_WORD_CUSTOM(uom_degrees, "°C")
MAKE_WORD_CUSTOM(uom_kwh, "kWh")
MAKE_WORD_CUSTOM(uom_wh, "Wh")
MAKE_WORD_CUSTOM(uom_bar, "bar")
MAKE_WORD_CUSTOM(uom_ua, "µA")
MAKE_WORD_CUSTOM(uom_lmin, "l/min")
MAKE_WORD_CUSTOM(uom_kw, "kW")
MAKE_WORD_CUSTOM(uom_w, "W")
MAKE_WORD_CUSTOM(uom_kb, "KB")
MAKE_WORD_CUSTOM(uom_dbm, "dBm")
MAKE_WORD_CUSTOM(uom_fahrenheit, "°F")
MAKE_WORD_CUSTOM(uom_mv, "mV")
MAKE_WORD_CUSTOM(uom_sqm, "m²")
MAKE_WORD_CUSTOM(uom_m3, "m³")
MAKE_WORD_CUSTOM(uom_l, "l")
MAKE_WORD_CUSTOM(uom_kmin, "K*min")
MAKE_WORD_CUSTOM(uom_k, "K")
MAKE_WORD_CUSTOM(uom_volts, "V")
MAKE_WORD_CUSTOM(uom_mbar, "mbar")
MAKE_WORD_CUSTOM(uom_lh, "l/h")
MAKE_WORD_CUSTOM(uom_ctkwh, "ct/kWh")

// MQTT topics and prefixes
MAKE_WORD_CUSTOM(heating_active, "heating_active")
MAKE_WORD_CUSTOM(tapwater_active, "tapwater_active")
MAKE_WORD_CUSTOM(response, "response")

// syslog
MAKE_ENUM_FIXED(list_syslog_level, "off", "emerg", "alert", "crit", "error", "warn", "notice", "info", "debug", "trace", "all")

// sensors
MAKE_ENUM_FIXED(counter, "counter")
MAKE_ENUM_FIXED(digital_out, "digital_out")
MAKE_ENUM_FIXED(list_sensortype, "disabled", "digital in", "counter", "adc", "timer", "rate", "digital out", "pwm 0", "pwm 1", "pwm 2")

// watch
MAKE_ENUM_FIXED(list_watch, "off", "on", "raw", "unknown")

/*
 * The rest below are Enums and generated from translations lists
 */
MAKE_ENUM(enum_cylprio, FL_(cyl1), FL_(cyl2))
MAKE_ENUM(enum_progMode, FL_(prog1), FL_(prog2))
MAKE_ENUM(enum_progMode4, FL_(proga), FL_(progb), FL_(progc), FL_(progd), FL_(proge), FL_(progf))
MAKE_ENUM(enum_climate, FL_(seltemp), FL_(roomtemp))
MAKE_ENUM(enum_charge, FL_(chargepump), FL_(3wayvalve))
MAKE_ENUM(enum_freq, FL_(off), FL_(1x3min), FL_(2x3min), FL_(3x3min), FL_(4x3min), FL_(5x3min), FL_(6x3min), FL_(continuous))
MAKE_ENUM(enum_off_time_date_manual, FL_(off), FL_(time), FL_(date), FL_(manual))
MAKE_ENUM(enum_comfort, FL_(hot), FL_(eco), FL_(intelligent))
MAKE_ENUM(enum_comfort1, FL_(high_comfort), FL_(eco))
MAKE_ENUM(enum_comfort2, FL_(eco), FL_(high_comfort))
MAKE_ENUM(enum_flow, FL_(off), FL_(flow), FL_(bufferedflow), FL_(buffer), FL_(layeredbuffer))
MAKE_ENUM(enum_reset, FL_(dash), FL_(maintenance), FL_(error), FL_(history), FL_(message))
MAKE_ENUM(enum_maxHeat, FL_(0kW), FL_(2kW), FL_(3kW), FL_(4kW), FL_(6kW), FL_(9kW))
MAKE_ENUM(enum_pumpMode, FL_(proportional), FL_(deltaP1), FL_(deltaP2), FL_(deltaP3), FL_(deltaP4))
MAKE_ENUM(enum_pumpCharacter, FL_(proportional), FL_(pressure1), FL_(pressure2), FL_(pressure3), FL_(pressure4), FL_(pressure5), FL_(pressure6))
MAKE_ENUM(enum_hpPumpMode, FL_(auto), FL_(continuous))

// thermostat lists
MAKE_ENUM(enum_ibaMainDisplay, FL_(internal_temperature), FL_(internal_setpoint), FL_(external_temperature), FL_(burner_temperature), FL_(ww_temperature), FL_(functioning_mode), FL_(time), FL_(date), FL_(smoke_temperature))
MAKE_ENUM(enum_ibaLanguage, FL_(german), FL_(dutch), FL_(french), FL_(italian))
MAKE_ENUM(enum_ibaLanguage_RC30, FL_(german), FL_(dutch))
MAKE_ENUM(enum_floordrystatus, FL_(off), FL_(start), FL_(heat), FL_(hold), FL_(cool), FL_(end))
MAKE_ENUM(enum_ibaBuildingType, FL_(light), FL_(medium), FL_(heavy))
MAKE_ENUM(enum_PID, FL_(fast), FL_(medium), FL_(slow))
MAKE_ENUM(enum_heatup, FL_(slow), FL_(medium), FL_(fast))
MAKE_ENUM(enum_wwMode, FL_(off), FL_(normal), FL_(comfort), FL_(auto), FL_(own_prog))
MAKE_ENUM(enum_wwCircMode, FL_(off), FL_(on), FL_(auto), FL_(own_prog))
MAKE_ENUM(enum_wwMode2, FL_(off), FL_(on), FL_(auto))
MAKE_ENUM(enum_wwMode3, FL_(on), FL_(off), FL_(auto))
MAKE_ENUM(enum_wwMode4, FL_(off), FL_(ecoplus), FL_(eco), FL_(comfort), FL_(auto))
MAKE_ENUM(enum_wwMode5, FL_(normal), FL_(comfort), FL_(ecoplus)) // Rego3000
MAKE_ENUM(enum_wwMode6, FL_(off), FL_(comfort), FL_(auto)) // CR120
MAKE_ENUM(enum_heatingtype, FL_(off), FL_(radiator), FL_(convector), FL_(floor))
MAKE_ENUM(enum_heatingtype1, FL_(off), FL_(curve), FL_(radiator), FL_(convector), FL_(floor))
MAKE_ENUM(enum_heatingtype2, FL_(off), FL_(radiator), FL_(convector), FL_(floor), FL_(roomflow), FL_(roomload))
MAKE_ENUM(enum_summermode, FL_(summer), FL_(auto), FL_(winter))
MAKE_ENUM(enum_hpoperatingmode, FL_(off), FL_(auto), FL_(heating), FL_(cooling))
MAKE_ENUM(enum_summer, FL_(winter), FL_(summer))
MAKE_ENUM(enum_operatingstate, FL_(heating), FL_(off), FL_(cooling))
MAKE_ENUM(enum_hpmode, FL_(heating), FL_(cooling), FL_(heatandcool))

MAKE_ENUM(enum_mode, FL_(manual), FL_(auto))                        // RC100, RC300, RC310
MAKE_ENUM(enum_mode2, FL_(off), FL_(manual), FL_(auto))             // RC20, RC30, BC400
MAKE_ENUM(enum_mode3, FL_(night), FL_(day), FL_(auto))              // RC35, RC30_N, RC25, RC20_N
MAKE_ENUM(enum_mode4, FL_(nofrost), FL_(eco), FL_(heat), FL_(auto)) // JUNKERS
MAKE_ENUM(enum_mode5, FL_(auto), FL_(off))                          // CRF
MAKE_ENUM(enum_mode6, FL_(nofrost), FL_(night), FL_(day))           // RC10
MAKE_ENUM(enum_mode_ha, FL_(off), FL_(heat), FL_(auto))             // HA climate

MAKE_ENUM(enum_modetype, FL_(eco), FL_(comfort))
MAKE_ENUM(enum_modetype3, FL_(night), FL_(day))
MAKE_ENUM(enum_modetype4, FL_(nofrost), FL_(eco), FL_(heat))
MAKE_ENUM(enum_modetype5, FL_(off), FL_(on))

MAKE_ENUM(enum_reducemode, FL_(nofrost), FL_(reduce), FL_(room), FL_(outdoor))
MAKE_ENUM(enum_reducemode1, FL_(outdoor), FL_(room), FL_(reduce)) // RC310 values: 1-3
MAKE_ENUM(enum_nofrostmode, FL_(off), FL_(outdoor), FL_(room))
MAKE_ENUM(enum_nofrostmode1, FL_(room), FL_(outdoor), FL_(room_outdoor))

MAKE_ENUM(enum_controlmode, FL_(optimized), FL_(simple), FL_(na), FL_(room), FL_(power))
MAKE_ENUM(enum_controlmode1, FL_(weather_compensated), FL_(outside_basepoint), FL_(na), FL_(room), FL_(power), FL_(constant)) // RC310 1-4
MAKE_ENUM(enum_controlmode2, FL_(outdoor), FL_(room))
MAKE_ENUM(enum_controlmode3, FL_(off), FL_(unmixed), FL_(unmixedIPM), FL_(mixed))
MAKE_ENUM(enum_control, FL_(off), FL_(rc20), FL_(rc3x))
MAKE_ENUM(enum_j_control, FL_(off), FL_(fb10), FL_(fb100))
MAKE_ENUM(enum_roomsensor, FL_(extern), FL_(intern), FL_(auto))
MAKE_ENUM(enum_roominfluence, FL_(off), FL_(intern), FL_(extern), FL_(auto))
MAKE_ENUM(enum_control1, FL_(rc310), FL_(rc200), FL_(rc100), FL_(rc100h), FL_(tc100))
MAKE_ENUM(enum_control2, FL_(off), FL_(dash), FL_(rc100), FL_(rc100h), FL_(dash), FL_(rc120rf), FL_(rc220), FL_(single)) // BC400

MAKE_ENUM(enum_switchmode, FL_(off), FL_(eco), FL_(comfort), FL_(heat))
MAKE_ENUM(enum_switchProgMode, FL_(level), FL_(absolute))

MAKE_ENUM(enum_dayOfWeek, FL_(day_mo), FL_(day_tu), FL_(day_we), FL_(day_th), FL_(day_fr), FL_(day_sa), FL_(day_su), FL_(all))
MAKE_ENUM(enum_progMode2, FL_(own_1), FL_(family), FL_(morning), FL_(evening), FL_(am), FL_(pm), FL_(midday), FL_(singles), FL_(seniors), FL_(new), FL_(own_2))
MAKE_ENUM(enum_progMode3, FL_(family), FL_(morning), FL_(evening), FL_(am), FL_(pm), FL_(midday), FL_(singles), FL_(seniors))
MAKE_ENUM(enum_hybridStrategy, FL_(co2_optimized), FL_(cost_optimized), FL_(outside_temp_switched), FL_(co2_cost_mix))
MAKE_ENUM(enum_hybridStrategy1, FL_(cost_optimized), FL_(co2_optimized), FL_(outside_temp_alt), FL_(outside_temp_par), FL_(hp_preferred), FL_(boiler_only))
MAKE_ENUM(enum_lowNoiseMode, FL_(off), FL_(reduced_output), FL_(switchoff), FL_(perm))

// heat pump
MAKE_ENUM(enum_hpactivity, FL_(off), FL_(heating), FL_(cooling), FL_(hot_water), FL_(pool), FL_(pool_heating), FL_(defrost), FL_(compressor_alarm)) // BV name COMPRESSOR_E21_STATUS
MAKE_ENUM(enum_silentMode, FL_(off), FL_(auto), FL_(on))
MAKE_ENUM(enum_4way, FL_(cool_defrost), FL_(heat_ww))

// solar
MAKE_ENUM(enum_solarmode, FL_(constant), FL_(pwm), FL_(analog))
MAKE_ENUM(enum_collectortype, FL_(flat), FL_(vacuum))
MAKE_ENUM(enum_wwStatus2, FL_(BLANK), FL_(BLANK), FL_(BLANK), FL_(no_heat), FL_(BLANK), FL_(BLANK), FL_(heatrequest), FL_(BLANK), FL_(disinfecting), FL_(hold))

// mixer
MAKE_ENUM(enum_shunt, FL_(stopped), FL_(opening), FL_(closing), FL_(open), FL_(close))
MAKE_ENUM(enum_wwProgMode, FL_(std_prog), FL_(own_prog))

// AM200 lists
MAKE_ENUM(enum_vr2Config, FL_(off), FL_(bypass))
MAKE_ENUM(enum_aPumpSignal, FL_(off), FL_(pwm), FL_(pwm_invers))
MAKE_ENUM(enum_bufBypass, FL_(no), FL_(mixer), FL_(valve))
MAKE_ENUM(enum_blockMode, FL_(off), FL_(auto), FL_(blocking))
MAKE_ENUM(enum_bufConfig, FL_(off), FL_(monovalent), FL_(bivalent))
MAKE_ENUM(enum_blockTerm, FL_(n_o), FL_(n_c))

// Ventilation
MAKE_ENUM(enum_ventMode, FL_(auto), FL_(off), FL_(L1), FL_(L2), FL_(L3), FL_(L4), FL_(demand), FL_(sleep), FL_(intense), FL_(bypass), FL_(partymode), FL_(fireplace))

// water
MAKE_ENUM(enum_errorDisp, FL_(off), FL_(normal), FL_(inverted))

#pragma GCC diagnostic pop

// clang-format on