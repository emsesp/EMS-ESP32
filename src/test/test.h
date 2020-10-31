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

#if defined(EMSESP_DEBUG)

#ifndef EMSESP_TEST_H
#define EMSESP_TEST_H

#include <Arduino.h>

#include <vector>
#include <string>

#include <uuid/common.h>
#include <uuid/console.h>
#include <uuid/log.h>

#include "emsdevice.h"
#include "emsfactory.h"
#include "telegram.h"
#include "mqtt.h"
#include "emsesp.h"

namespace emsesp {

class Test {
  public:
    static void run_test(uuid::console::Shell & shell, const std::string & command); // only for testing
    static void dummy_mqtt_commands(const char * message);
    static void rx_telegram(const std::vector<uint8_t> & data);
    static void uart_telegram(const std::vector<uint8_t> & rx_data);
    static void uart_telegram(const char * rx_data);
    static void uart_telegram_withCRC(const char * rx_data);
};

} // namespace emsesp

#endif

#endif
