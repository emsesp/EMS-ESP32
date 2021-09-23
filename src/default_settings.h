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
#ifndef EMSESP_DEFAULT_SETTINGS_H
#define EMSESP_DEFAULT_SETTINGS_H

// GENERAL SETTINGS

#ifndef EMSESP_DEFAULT_TX_MODE
#define EMSESP_DEFAULT_TX_MODE 1 // EMS1.0
#endif

#ifndef EMSESP_DEFAULT_EMS_BUS_ID
#define EMSESP_DEFAULT_EMS_BUS_ID 0x0B // service key
#endif

#ifndef EMSESP_DEFAULT_SYSLOG_ENABLED
#define EMSESP_DEFAULT_SYSLOG_ENABLED false
#endif

#ifndef EMSESP_DEFAULT_SYSLOG_LEVEL
#define EMSESP_DEFAULT_SYSLOG_LEVEL 3 // ERR
#endif

#ifndef EMSESP_DEFAULT_SYSLOG_MARK_INTERVAL
#define EMSESP_DEFAULT_SYSLOG_MARK_INTERVAL 0
#endif

#ifndef EMSESP_DEFAULT_SYSLOG_HOST
#define EMSESP_DEFAULT_SYSLOG_HOST ""
#endif

#ifndef EMSESP_DEFAULT_SYSLOG_PORT
#define EMSESP_DEFAULT_SYSLOG_PORT 514
#endif

#ifndef EMSESP_DEFAULT_TRACELOG_RAW
#define EMSESP_DEFAULT_TRACELOG_RAW false
#endif

#ifndef EMSESP_DEFAULT_MASTER_THERMOSTAT
#define EMSESP_DEFAULT_MASTER_THERMOSTAT 0 // not set
#endif

#ifndef EMSESP_DEFAULT_SHOWER_TIMER
#define EMSESP_DEFAULT_SHOWER_TIMER false
#endif

#ifndef EMSESP_DEFAULT_SHOWER_ALERT
#define EMSESP_DEFAULT_SHOWER_ALERT false
#endif

#ifndef EMSESP_DEFAULT_HIDE_LED
#define EMSESP_DEFAULT_HIDE_LED false
#endif

#ifndef EMSESP_DEFAULT_DALLAS_PARASITE
#define EMSESP_DEFAULT_DALLAS_PARASITE false
#endif

#ifndef EMSESP_DEFAULT_NOTOKEN_API
#define EMSESP_DEFAULT_NOTOKEN_API false
#endif

#ifndef EMSESP_DEFAULT_BOOL_FORMAT
#define EMSESP_DEFAULT_BOOL_FORMAT 1 // on/off
#endif

#ifndef EMSESP_DEFAULT_ENUM_FORMAT
#define EMSESP_DEFAULT_ENUM_FORMAT 1 // Text
#endif

#ifndef EMSESP_DEFAULT_ANALOG_ENABLED
#define EMSESP_DEFAULT_ANALOG_ENABLED false
#endif

#ifndef EMSESP_DEFAULT_BOARD_PROFILE
#define EMSESP_DEFAULT_BOARD_PROFILE "S32" // Gateway S32
#endif

#ifndef EMSESP_DEFAULT_AUX_FUNCTION
#define EMSESP_DEFAULT_AUX_FUNCTION 0 // None
#endif

#ifndef EMSESP_DEFAULT_AUX_PUMP_DELAY
#define EMSESP_DEFAULT_AUX_PUMP_DELAY 300 // run pump for 5min after normal off conditions
#endif

// Default GPIO PIN definitions - based on Wemos/Nodemcu

#ifndef EMSESP_DEFAULT_RX_GPIO
#define EMSESP_DEFAULT_RX_GPIO 23 // D7
#endif

#ifndef EMSESP_DEFAULT_TX_GPIO
#define EMSESP_DEFAULT_TX_GPIO 5 // D8
#endif

#ifndef EMSESP_DEFAULT_DALLAS_GPIO
#define EMSESP_DEFAULT_DALLAS_GPIO 18
#endif

#ifndef EMSESP_DEFAULT_LED_GPIO
#define EMSESP_DEFAULT_LED_GPIO 2
#endif

#ifndef EMSESP_DEFAULT_PBUTTON_GPIO
#define EMSESP_DEFAULT_PBUTTON_GPIO 0
#endif

#ifndef EMSESP_DEFAULT_AUX_GPIO
#define EMSESP_DEFAULT_AUX_GPIO 22
#endif

// MQTT

#ifndef EMSESP_DEFAULT_BOOL_FORMAT
#define EMSESP_DEFAULT_BOOL_FORMAT 1 // on/off
#endif

#ifndef EMSESP_DEFAULT_DALLAS_FORMAT
#define EMSESP_DEFAULT_DALLAS_FORMAT 1 // sensorid
#endif

#ifndef EMSESP_DEFAULT_HA_CLIMATE_FORMAT
#define EMSESP_DEFAULT_HA_CLIMATE_FORMAT 1 // current temp
#endif

#ifndef EMSESP_DEFAULT_MQTT_QOS
#define EMSESP_DEFAULT_MQTT_QOS 0
#endif

#ifndef EMSESP_DEFAULT_MQTT_RETAIN
#define EMSESP_DEFAULT_MQTT_RETAIN false
#endif

#ifndef EMSESP_DEFAULT_HA_ENABLED
#define EMSESP_DEFAULT_HA_ENABLED false
#endif

#ifndef EMSESP_DEFAULT_PUBLISH_TIME
#define EMSESP_DEFAULT_PUBLISH_TIME 10
#endif

#ifndef EMSESP_DEFAULT_NESTED_FORMAT
#define EMSESP_DEFAULT_NESTED_FORMAT 1
#endif

#ifndef EMSESP_DEFAULT_SUBSCRIBE_FORMAT
#define EMSESP_DEFAULT_SUBSCRIBE_FORMAT 0
#endif

#ifndef EMSESP_DEFAULT_SOLAR_MAXFLOW
#define EMSESP_DEFAULT_SOLAR_MAXFLOW 30
#endif

#ifndef EMSESP_DEFAULT_SENSOR_NAME
#define EMSESP_DEFAULT_SENSOR_NAME ""
#endif

#ifndef EMSESP_DEFAULT_WEBLOG_LEVEL
#define EMSESP_DEFAULT_WEBLOG_LEVEL 6 // INFO
#endif

#ifndef EMSESP_DEFAULT_WEBLOG_BUFFER
#define EMSESP_DEFAULT_WEBLOG_BUFFER 50
#endif

#ifndef EMSESP_DEFAULT_WEBLOG_COMPACT
#define EMSESP_DEFAULT_WEBLOG_COMPACT true
#endif

#endif
