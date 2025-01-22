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

#if defined(EMSESP_STANDALONE) || defined(EMSESP_TEST)

#ifndef EMSESP_TEST_H
#define EMSESP_TEST_H

#include "emsesp.h"

#include <ESPAsyncWebServer.h>

namespace emsesp {

// #define EMSESP_DEBUG_DEFAULT "thermostat"
// #define EMSESP_DEBUG_DEFAULT "solar"
// #define EMSESP_DEBUG_DEFAULT "web"
// #define EMSESP_DEBUG_DEFAULT "mqtt"
// #define EMSESP_DEBUG_DEFAULT "boiler"
// #define EMSESP_DEBUG_DEFAULT "mqtt2"
// #define EMSESP_DEBUG_DEFAULT "mqtt_nested"
// #define EMSESP_DEBUG_DEFAULT "ha"
// #define EMSESP_DEBUG_DEFAULT "masked"
// #define EMSESP_DEBUG_DEFAULT "board_profile"
// #define EMSESP_DEBUG_DEFAULT "shower_alert"
// #define EMSESP_DEBUG_DEFAULT "310"
// #define EMSESP_DEBUG_DEFAULT "render"
// #define EMSESP_DEBUG_DEFAULT "api"
// #define EMSESP_DEBUG_DEFAULT "api3"
// #define EMSESP_DEBUG_DEFAULT "api4"
// #define EMSESP_DEBUG_DEFAULT "crash"
// #define EMSESP_DEBUG_DEFAULT "dv"
// #define EMSESP_DEBUG_DEFAULT "lastcode"
// #define EMSESP_DEBUG_DEFAULT "2thermostats"
// #define EMSESP_DEBUG_DEFAULT "temperature"
// #define EMSESP_DEBUG_DEFAULT "analog"
// #define EMSESP_DEBUG_DEFAULT "mqtt_post"
// #define EMSESP_DEBUG_DEFAULT "api_wwmode"
// #define EMSESP_DEBUG_DEFAULT "customization"
// #define EMSESP_DEBUG_DEFAULT "entity_dump"
// #define EMSESP_DEBUG_DEFAULT "telegram_dump"
// #define EMSESP_DEBUG_DEFAULT "memory"
// #define EMSESP_DEBUG_DEFAULT "coldshot"
// #define EMSESP_DEBUG_DEFAULT "custom"
// #define EMSESP_DEBUG_DEFAULT "scheduler"
// #define EMSESP_DEBUG_DEFAULT "heat_exchange"
// #define EMSESP_DEBUG_DEFAULT "ls"
// #define EMSESP_DEBUG_DEFAULT "upload"

#ifndef EMSESP_DEBUG_DEFAULT
#define EMSESP_DEBUG_DEFAULT "general"
#endif

class Test {
  public:
    static void run_test(uuid::console::Shell & shell, const std::string & command, const std::string & id1 = "", const std::string & id2 = "");
    static bool test(const std::string & command, int8_t id1 = -1, int8_t id2 = -1);
    static void dummy_mqtt_commands(const char * message);
    static void rx_telegram(const std::vector<uint8_t> & data);
    static void uart_telegram(const std::vector<uint8_t> & rx_data);
    static void uart_telegram(const char * rx_data);
    static void uart_telegram_withCRC(const char * rx_data);
    static void add_device(uint8_t device_id, uint8_t product_id);
    static void refresh();
    static void listDir(fs::FS & fs, const char * dirname, uint8_t levels);
};

} // namespace emsesp

#endif

#endif
