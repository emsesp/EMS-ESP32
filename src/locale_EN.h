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

// common words
MAKE_PSTR_WORD(exit)
MAKE_PSTR_WORD(help)
MAKE_PSTR_WORD(settings)
MAKE_PSTR_WORD(log)
MAKE_PSTR_WORD(logout)
MAKE_PSTR_WORD(enabled)
MAKE_PSTR_WORD(disabled)
MAKE_PSTR_WORD(set)
MAKE_PSTR_WORD(show)
MAKE_PSTR_WORD(on)
MAKE_PSTR_WORD(off)
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
MAKE_PSTR_WORD(test)
MAKE_PSTR_WORD(pin)

// for commands
MAKE_PSTR_WORD(call)

// devices
MAKE_PSTR_WORD(boiler)
MAKE_PSTR_WORD(thermostat)
MAKE_PSTR_WORD(switch)
MAKE_PSTR_WORD(solar)
MAKE_PSTR_WORD(mixing)
MAKE_PSTR_WORD(gateway)
MAKE_PSTR_WORD(controller)
MAKE_PSTR_WORD(connect)
MAKE_PSTR_WORD(heatpump)

// dallas sensors
MAKE_PSTR_WORD(sensors)

MAKE_PSTR(kwh, "kWh")
MAKE_PSTR(wh, "Wh")
MAKE_PSTR(hc_optional, "[heating circuit]")
MAKE_PSTR(master_thermostat_fmt, "Master Thermostat Device ID = %s")
MAKE_PSTR(host_fmt, "Host = %s")
MAKE_PSTR(hostname_fmt, "WiFi Hostname = %s")
MAKE_PSTR(mark_interval_fmt, "Mark interval = %lus")
MAKE_PSTR(wifi_ssid_fmt, "WiFi SSID = %s")
MAKE_PSTR(wifi_password_fmt, "WiFi Password = %S")
MAKE_PSTR(system_heartbeat_fmt, "MQTT Heartbeat is %s")
MAKE_PSTR(cmd_optional, "[cmd]")
MAKE_PSTR(deep_optional, "[deep]")
MAKE_PSTR(tx_mode_fmt, "Tx mode = %d")
MAKE_PSTR(bus_id_fmt, "Bus ID = %02X")
MAKE_PSTR(watchid_optional, "[ID]")
MAKE_PSTR(watch_format_optional, "[off | on | raw]")
MAKE_PSTR(invalid_watch, "Invalid watch type")
MAKE_PSTR(data_mandatory, "<\"XX XX ...\">")
MAKE_PSTR(percent, "%")
MAKE_PSTR(degrees, "°C")
MAKE_PSTR(asterisks, "********")
MAKE_PSTR(n_mandatory, "<n>")
MAKE_PSTR(n_optional, "[n]")
MAKE_PSTR(gpio_mandatory, "<gpio>")
MAKE_PSTR(data_optional, "[data]")
MAKE_PSTR(typeid_mandatory, "<type ID>")
MAKE_PSTR(deviceid_mandatory, "<device ID>")
MAKE_PSTR(invalid_log_level, "Invalid log level")
MAKE_PSTR(log_level_fmt, "Log level = %s")
MAKE_PSTR(log_level_optional, "[level]")
MAKE_PSTR(name_mandatory, "<name>")
MAKE_PSTR(name_optional, "[name]")
MAKE_PSTR(new_password_prompt1, "Enter new password: ")
MAKE_PSTR(new_password_prompt2, "Retype new password: ")
MAKE_PSTR(password_prompt, "Password: ")
MAKE_PSTR(unset, "<unset>")
