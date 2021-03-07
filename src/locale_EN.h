/*
 * EMS-ESP - https://github.com/proddy/EMS-ESP
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
MAKE_PSTR_WORD(on)
MAKE_PSTR_WORD(off)
MAKE_PSTR_WORD(ON)
MAKE_PSTR_WORD(OFF)
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
MAKE_PSTR_WORD(master)
MAKE_PSTR_WORD(pin)
MAKE_PSTR_WORD(publish)
MAKE_PSTR_WORD(timeout)
MAKE_PSTR_WORD(ethernet)

// for commands
MAKE_PSTR_WORD(call)
MAKE_PSTR_WORD(cmd)
MAKE_PSTR_WORD(id)
MAKE_PSTR_WORD(device)
MAKE_PSTR_WORD(data)
MAKE_PSTR_WORD(command)
MAKE_PSTR_WORD(commands)
MAKE_PSTR_WORD(info)
MAKE_PSTR_WORD(settings)

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
MAKE_PSTR_WORD(dallassensor)
MAKE_PSTR_WORD(unknown)

// strings
MAKE_PSTR(EMSESP, "EMS-ESP")
MAKE_PSTR(master_thermostat_fmt, "Master Thermostat Device ID = %s")
MAKE_PSTR(host_fmt, "Host = %s")
MAKE_PSTR(hostname_fmt, "Hostname = %s")
MAKE_PSTR(mark_interval_fmt, "Mark interval = %lus")
MAKE_PSTR(wifi_ssid_fmt, "WiFi SSID = %s")
MAKE_PSTR(wifi_password_fmt, "WiFi Password = %S")
MAKE_PSTR(ethernet_option_fmt, "Ethernet option = %d")
MAKE_PSTR(cmd_optional, "[cmd]")
MAKE_PSTR(ha_optional, "[ha]")
MAKE_PSTR(deep_optional, "[deep]")
MAKE_PSTR(tx_mode_fmt, "Tx mode = %d")
MAKE_PSTR(bus_id_fmt, "Bus ID = %02X")
MAKE_PSTR(watchid_optional, "[ID]")
MAKE_PSTR(watch_format_optional, "[off | on | raw | unknown]")
MAKE_PSTR(invalid_watch, "Invalid watch type")
MAKE_PSTR(data_mandatory, "\"XX XX ...\"")
MAKE_PSTR(asterisks, "********")
MAKE_PSTR(n_mandatory, "<n>")
MAKE_PSTR(id_optional, "[id|hc]")
MAKE_PSTR(data_optional, "[data]")
MAKE_PSTR(offset_optional, "[offset]")
MAKE_PSTR(typeid_mandatory, "<type ID>")
MAKE_PSTR(deviceid_mandatory, "<device ID>")
MAKE_PSTR(device_type_optional, "[device]")
MAKE_PSTR(invalid_log_level, "Invalid log level")
MAKE_PSTR(log_level_fmt, "Log level = %s")
MAKE_PSTR(log_level_optional, "[level]")
MAKE_PSTR(name_mandatory, "<name>")
MAKE_PSTR(name_optional, "[name]")
MAKE_PSTR(new_password_prompt1, "Enter new password: ")
MAKE_PSTR(new_password_prompt2, "Retype new password: ")
MAKE_PSTR(password_prompt, "Password: ")
MAKE_PSTR(unset, "<unset>")

MAKE_PSTR_WORD(2);
MAKE_PSTR_WORD(10);
MAKE_PSTR_WORD(100);
MAKE_PSTR_WORD(60);

MAKE_PSTR_LIST(div2, F_(2))
MAKE_PSTR_LIST(div10, F_(10))
MAKE_PSTR_LIST(div100, F_(100))
MAKE_PSTR_LIST(div60, F_(60))


// boiler
MAKE_PSTR_WORD(time)
MAKE_PSTR_WORD(date)
MAKE_PSTR_WORD(1x3min)
MAKE_PSTR_WORD(2x3min)
MAKE_PSTR_WORD(3x3min)
MAKE_PSTR_WORD(4x3min)
MAKE_PSTR_WORD(5x3min)
MAKE_PSTR_WORD(6x3min)
MAKE_PSTR_WORD(continuos);
MAKE_PSTR(3wayvalve, "3-way valve")
MAKE_PSTR(chargepump, "charge pump")
MAKE_PSTR_WORD(hot)
MAKE_PSTR_WORD(eco)
MAKE_PSTR_WORD(intelligent)
MAKE_PSTR_WORD(flow)
MAKE_PSTR_WORD(buffer)
MAKE_PSTR(bufferedflow, "buffered flow")
MAKE_PSTR(layeredbuffer, "layered buffer")

// boiler lists
MAKE_PSTR_LIST(enum_off_time_date, F_(off), F_(time), F_(date))
MAKE_PSTR_LIST(enum_freq, F_(off), F_(1x3min), F_(2x3min), F_(3x3min), F_(4x3min), F_(5x3min), F_(6x3min), F_(continuos))
MAKE_PSTR_LIST(enum_charge, F_(3wayvalve), F_(chargepump))
MAKE_PSTR_LIST(enum_comfort, F_(hot), F_(eco), F_(intelligent))
MAKE_PSTR_LIST(enum_flow, F_(off), F_(flow), F_(bufferedflow), F_(buffer), F_(layeredbuffer))

// thermostat
MAKE_PSTR_WORD(light)
MAKE_PSTR_WORD(medium)
MAKE_PSTR_WORD(heavy)
MAKE_PSTR_WORD(own_prog)
MAKE_PSTR(blank, "")
MAKE_PSTR_WORD(start)
MAKE_PSTR_WORD(heat)
MAKE_PSTR_WORD(hold)
MAKE_PSTR_WORD(cool)
MAKE_PSTR_WORD(end)
MAKE_PSTR_WORD(German)
MAKE_PSTR_WORD(Dutch)
MAKE_PSTR_WORD(French)
MAKE_PSTR_WORD(Italian)
MAKE_PSTR_WORD(high)
MAKE_PSTR_WORD(low)
MAKE_PSTR(internal_temperature, "internal temperature")
MAKE_PSTR(internal_setpoint, "internal setpoint")
MAKE_PSTR(external_temperature, "external temperature")
MAKE_PSTR(burner_temperature, "burner temperature")
MAKE_PSTR(WW_temperature, "WW temperature")
MAKE_PSTR(functioning_mode, "functioning mode")
MAKE_PSTR(smoke_temperature, "smoke temperature")
MAKE_PSTR_WORD(radiator)
MAKE_PSTR_WORD(convector)
MAKE_PSTR_WORD(floor)
MAKE_PSTR_WORD(summer)
MAKE_PSTR_WORD(winter)
MAKE_PSTR_WORD(outdoor)
MAKE_PSTR_WORD(MPC)
MAKE_PSTR_WORD(room)
MAKE_PSTR_WORD(power)
MAKE_PSTR_WORD(constant)
MAKE_PSTR_WORD(simple)
MAKE_PSTR_WORD(nofrost)
MAKE_PSTR_WORD(comfort)
MAKE_PSTR_WORD(manual)
MAKE_PSTR_WORD(night)
MAKE_PSTR_WORD(day)
MAKE_PSTR_WORD(holiday)
MAKE_PSTR_WORD(reduce)

// thermostat lists
MAKE_PSTR_LIST(enum_ibaMainDisplay, F_(internal_temperature), F_(internal_setpoint), F_(external_temperature), F_(burner_temperature), F_(WW_temperature), F_(functioning_mode), F_(time), F_(date), F_(smoke_temperature))
MAKE_PSTR_LIST(enum_ibaLanguage, F_(German), F_(Dutch), F_(French), F_(Italian))
MAKE_PSTR_LIST(enum_floordrystatus, F_(off), F_(start), F_(heat), F_(hold), F_(cool), F_(end))
MAKE_PSTR_LIST(enum_ibaBuildingType, F_(blank), F_(light), F_(medium), F_(heavy))
MAKE_PSTR_LIST(enum_wwMode, F_(off), F_(low), F_(high), F_(auto), F_(own_prog))
MAKE_PSTR_LIST(enum_wwCircMode, F_(off), F_(on), F_(auto), F_(own_prog))
MAKE_PSTR_LIST(enum_ibaBuildingType2, F_(light), F_(medium), F_(heavy))
MAKE_PSTR_LIST(enum_wwMode2, F_(off), F_(on), F_(auto))
MAKE_PSTR_LIST(enum_wwCircMode2, F_(off), F_(on), F_(auto))
MAKE_PSTR_LIST(enum_heatingtype, F_(off), F_(radiator), F_(convector), F_(floor))
MAKE_PSTR_LIST(enum_summermode, F_(summer), F_(auto), F_(winter))

MAKE_PSTR_LIST(enum_mode, F_(manual), F_(auto))
MAKE_PSTR_LIST(enum_mode2, F_(off), F_(manual), F_(auto))
MAKE_PSTR_LIST(enum_mode3, F_(night), F_(day), F_(auto))
MAKE_PSTR_LIST(enum_mode4, F_(manual), F_(auto), F_(holiday))

MAKE_PSTR_LIST(enum_modetype, F_(eco), F_(comfort))
MAKE_PSTR_LIST(enum_modetype2, F_(day))
MAKE_PSTR_LIST(enum_modetype3, F_(night), F_(day))
MAKE_PSTR_LIST(enum_modetype4, F_(heat), F_(eco), F_(nofrost))

MAKE_PSTR_LIST(enum_reducemode, F_(nofrost), F_(reduce), F_(room), F_(outdoor))

MAKE_PSTR_LIST(enum_controlmode, F_(off), F_(outdoor), F_(simple), F_(MPC), F_(room), F_(power), F_(constant))
MAKE_PSTR_LIST(enum_controlmode2, F_(outdoor), F_(room))

MAKE_PSTR_LIST(enum_hamode, F_(off), F_(heat), F_(auto), F_(heat), F_(off), F_(heat), F_(auto), F_(auto), F_(auto), F_(auto))
