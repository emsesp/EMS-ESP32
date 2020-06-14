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

#ifndef EMSESP_EMSESP_H
#define EMSESP_EMSESP_H

#include <Arduino.h>

#include <vector>
#include <queue>
#include <string>

#include <uuid/common.h>
#include <uuid/console.h>
#include <uuid/log.h>

#ifndef EMSESP_STANDALONE
#include <uuid/telnet.h>
#endif

#include "emsdevice.h"
#include "emsfactory.h"
#include "telegram.h"
#include "mqtt.h"
#include "settings.h"
#include "system.h"
#include "sensors.h"
#include "console.h"
#include "shower.h"
#include "roomcontrol.h"

#include "devices/boiler.h"

#define WATCH_NONE 0 // no watch id set

namespace emsesp {

class Shower; // forward declaration for compiler

class EMSESP {
  public:
    static void start();
    static void loop();

    static void publish_all_values();

#ifdef EMSESP_DEBUG
    static void run_test(uuid::console::Shell & shell, const std::string & command); // only for testing
    static void dummy_mqtt_commands(const char * message);
    static void rx_telegram(const std::vector<uint8_t> & data);
    static void uart_telegram(const std::vector<uint8_t> & rx_data);
#endif

    static bool        process_telegram(std::shared_ptr<const Telegram> telegram);
    static std::string pretty_telegram(std::shared_ptr<const Telegram> telegram);

    static void send_read_request(const uint16_t type_id, const uint8_t dest);
    static void send_write_request(const uint16_t type_id,
                                   const uint8_t  dest,
                                   const uint8_t  offset,
                                   uint8_t *      message_data,
                                   const uint8_t  message_length,
                                   const uint16_t validate_typeid);
    static void send_raw_telegram(const char * data);
    static bool device_exists(const uint8_t device_id);

    static uint8_t count_devices(const uint8_t device_type);

    static uint8_t actual_master_thermostat();
    static void    actual_master_thermostat(const uint8_t device_id);

    static void show_values(uuid::console::Shell & shell);
    static void show_device_values(uuid::console::Shell & shell);
    static void show_sensor_values(uuid::console::Shell & shell);

    static void show_devices(uuid::console::Shell & shell);
    static void show_emsbus(uuid::console::Shell & shell);

    static void add_context_menus();

    static void incoming_telegram(uint8_t * data, const uint8_t length);

    static const std::vector<Sensors::Device> sensor_devices() {
        return sensors_.devices();
    }

    static void watch_id(uint16_t id);

    static uint16_t watch_id() {
        return watch_id_;
    }

    static void watch(uint8_t watch) {
        watch_ = watch; // 0=off, 1=on, 2=raw
    }

    static uint8_t watch() {
        return watch_; // 0=off, 1=on, 2=raw
    }

    static bool tap_water_active() {
        return tap_water_active_;
    }

    static void tap_water_active(const bool tap_water_active) {
        tap_water_active_ = tap_water_active;
    }

    static void set_ems_read_only();

    static bool ems_read_only() {
        return ems_read_only_;
    }

    static void console_commands(Shell & shell, unsigned int context);

    static void fetch_device_values(const uint8_t device_id = 0);

    static bool add_device(const uint8_t device_id, const uint8_t product_id, std::string & version, const uint8_t brand);

    static Mqtt      mqtt_;
    static System    system_;
    static Network   network_;
    static Sensors   sensors_;
    static Console   console_;
    static Shower    shower_;
    static RxService rxservice_;
    static TxService txservice_;

  private:
    EMSESP() = delete;

    static uuid::log::Logger logger_;

    static std::string device_tostring(const uint8_t device_id);

    static void process_UBADevices(std::shared_ptr<const Telegram> telegram);
    static void process_version(std::shared_ptr<const Telegram> telegram);

    static constexpr uint32_t EMS_FETCH_FREQUENCY = 60000; // check every minute
    static uint32_t           last_fetch_;

    struct Device_record {
        uint8_t                     product_id;
        EMSdevice::DeviceType       device_type;
        const __FlashStringHelper * name;
        uint8_t                     flags;
    };

    static std::vector<std::unique_ptr<EMSdevice>> emsdevices;
    static std::vector<Device_record>              device_library_;

    static uint8_t  actual_master_thermostat_;
    static uint16_t watch_id_;
    static uint8_t  watch_;
    static bool     tap_water_active_;
    static bool     ems_read_only_;
};

} // namespace emsesp

#endif
