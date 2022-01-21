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
#ifndef EMSESP_EMSESP_H
#define EMSESP_EMSESP_H

#include <Arduino.h>

#include <vector>
#include <string>
#include <functional>
#include <deque>
#include <unordered_map>
#include <list>

#include <ArduinoJson.h>

#include <uuid/common.h>
#include <uuid/console.h>
#include <uuid/log.h>

#ifndef EMSESP_STANDALONE
#include <uuid/telnet.h>
#endif

#include <ESP8266React.h>

#include "web/WebStatusService.h"
#include "web/WebDataService.h"
#include "web/WebSettingsService.h"
#include "web/WebCustomizationService.h"
#include "web/WebAPIService.h"
#include "web/WebLogService.h"

#include "emsdevicevalue.h"
#include "emsdevice.h"
#include "emsfactory.h"
#include "telegram.h"
#include "mqtt.h"
#include "system.h"
#include "dallassensor.h"
#include "analogsensor.h"
#include "console.h"
#include "shower.h"
#include "roomcontrol.h"
#include "command.h"
#include "version.h"

#define WATCH_ID_NONE 0 // no watch id set

#ifndef EMSESP_STANDALONE
#define EMSESP_JSON_SIZE_HA_CONFIG 1024 // for HA config payloads, using StaticJsonDocument
#else
#define EMSESP_JSON_SIZE_HA_CONFIG 2024 // for HA config payloads, using StaticJsonDocument
#endif
#define EMSESP_JSON_SIZE_SMALL 256       // for smaller json docs, using StaticJsonDocument
#define EMSESP_JSON_SIZE_MEDIUM 768      // for medium json docs from ems devices, using StaticJsonDocument
#define EMSESP_JSON_SIZE_LARGE 1024      // for large json docs from ems devices, like boiler or thermostat data, using StaticJsonDocument
#define EMSESP_JSON_SIZE_MEDIUM_DYN 1024 // for large json docs, using DynamicJsonDocument
#define EMSESP_JSON_SIZE_LARGE_DYN 2048  // for very large json docs, using DynamicJsonDocument

#ifndef EMSESP_STANDALONE
#define EMSESP_JSON_SIZE_XLARGE_DYN 4096 // for very very large json docs, using DynamicJsonDocument
#else
#define EMSESP_JSON_SIZE_XLARGE_DYN 16384 // for very very large json docs, using DynamicJsonDocument
#endif

#define EMSESP_JSON_SIZE_XXLARGE_DYN 16384  // for extra very very large json docs, using DynamicJsonDocument
#define EMSESP_JSON_SIZE_XXXLARGE_DYN 20480 // web output (maybe for 4 hc)

// helpers for callback functions
#define MAKE_PF_CB(__f) [&](std::shared_ptr<const Telegram> t) { __f(t); }                  // for Process Function callbacks to EMSDevice::process_function_p
#define MAKE_CF_CB(__f) [&](const char * value, const int8_t id) { return __f(value, id); } // for Command Function callbacks Command::cmd_function_p

namespace emsesp {

using DeviceValueUOM   = emsesp::DeviceValue::DeviceValueUOM;
using DeviceValueType  = emsesp::DeviceValue::DeviceValueType;
using DeviceValueState = emsesp::DeviceValue::DeviceValueState;
using DeviceValueTAG   = emsesp::DeviceValue::DeviceValueTAG;

class Shower; // forward declaration for compiler

class EMSESP {
  public:
    static void start();
    static void loop();

    static void publish_device_values(uint8_t device_type);
    static void publish_other_values();
    static void publish_sensor_values(const bool time, const bool force = false);
    static void publish_all(bool force = false);
    static void reset_mqtt_ha();

#ifdef EMSESP_STANDALONE
    static void run_test(uuid::console::Shell & shell, const std::string & command); // only for testing
    static void dummy_mqtt_commands(const char * message);
    static void rx_telegram(const std::vector<uint8_t> & data);
    static void uart_telegram(const std::vector<uint8_t> & rx_data);
#endif

    static bool        process_telegram(std::shared_ptr<const Telegram> telegram);
    static std::string pretty_telegram(std::shared_ptr<const Telegram> telegram);

    static void send_read_request(const uint16_t type_id, const uint8_t dest, const uint8_t offset = 0, const uint8_t length = 0);
    static void send_write_request(const uint16_t type_id,
                                   const uint8_t  dest,
                                   const uint8_t  offset,
                                   uint8_t *      message_data,
                                   const uint8_t  message_length,
                                   const uint16_t validate_typeid);
    static void send_write_request(const uint16_t type_id, const uint8_t dest, const uint8_t offset, const uint8_t value);
    static void send_write_request(const uint16_t type_id, const uint8_t dest, const uint8_t offset, const uint8_t value, const uint16_t validate_typeid);

    static void send_raw_telegram(const char * data);
    static bool device_exists(const uint8_t device_id);

    static uint8_t count_devices(const uint8_t device_type);
    static uint8_t count_devices();
    static uint8_t device_index(const uint8_t device_type, const uint8_t unique_id);

    static uint8_t actual_master_thermostat();
    static void    actual_master_thermostat(const uint8_t device_id);
    static uint8_t check_master_device(const uint8_t device_id, const uint16_t type_id, const bool read);

    static bool get_device_value_info(JsonObject & root, const char * cmd, const int8_t id, const uint8_t devicetype);

    static void show_device_values(uuid::console::Shell & shell);
    static void show_sensor_values(uuid::console::Shell & shell);

    static void show_devices(uuid::console::Shell & shell);
    static void show_ems(uuid::console::Shell & shell);

    static void init_uart();

    static void incoming_telegram(uint8_t * data, const uint8_t length);

    static bool dallas_enabled() {
        return (dallassensor_.dallas_enabled());
    }

    static bool analog_enabled() {
        return (analogsensor_.analog_enabled());
    }

    enum Watch : uint8_t { WATCH_OFF, WATCH_ON, WATCH_RAW, WATCH_UNKNOWN };
    static void     watch_id(uint16_t id);
    static uint16_t watch_id() {
        return watch_id_;
    }

    static void watch(uint8_t watch) {
        watch_ = watch; // 0=off, 1=on, 2=raw
        if (watch == WATCH_OFF) {
            watch_id_ = 0; // reset watch id if watch is disabled
        }
    }

    static uint8_t watch() {
        return watch_;
    }
    static void set_read_id(uint16_t id) {
        read_id_ = id;
    }

    static bool wait_validate() {
        return (wait_validate_ != 0);
    }
    static void wait_validate(uint16_t wait) {
        wait_validate_ = wait;
    }

    enum Bus_status : uint8_t { BUS_STATUS_CONNECTED = 0, BUS_STATUS_TX_ERRORS, BUS_STATUS_OFFLINE };
    static uint8_t bus_status();

    static bool tap_water_active() {
        return tap_water_active_;
    }

    static void tap_water_active(const bool tap_water_active) {
        tap_water_active_ = tap_water_active;
    }

    static bool trace_raw() {
        return trace_raw_;
    }

    static void trace_raw(bool set) {
        trace_raw_ = set;
    }

    static void fetch_device_values(const uint8_t device_id = 0);
    static void fetch_device_values_type(const uint8_t device_type);
    static bool valid_device(const uint8_t device_id);

    static bool add_device(const uint8_t device_id, const uint8_t product_id, const char * version, const uint8_t brand);
    static void scan_devices();
    static void clear_all_devices();

    static std::vector<std::unique_ptr<EMSdevice>> emsdevices;

    // services
    static Mqtt         mqtt_;
    static System       system_;
    static DallasSensor dallassensor_;
    static AnalogSensor analogsensor_;
    static Console      console_;
    static Shower       shower_;
    static RxService    rxservice_;
    static TxService    txservice_;

    // web controllers
    static ESP8266React            esp8266React;
    static WebSettingsService      webSettingsService;
    static WebStatusService        webStatusService;
    static WebDataService          webDataService;
    static WebAPIService           webAPIService;
    static WebLogService           webLogService;
    static WebCustomizationService webCustomizationService;

    static uuid::log::Logger logger();

  private:
    EMSESP() = delete;

    static uuid::log::Logger logger_;

    static std::string device_tostring(const uint8_t device_id);
    static void        process_UBADevices(std::shared_ptr<const Telegram> telegram);
    static void        process_version(std::shared_ptr<const Telegram> telegram);
    static void        publish_response(std::shared_ptr<const Telegram> telegram);
    static void        publish_all_loop();
    static bool        command_info(uint8_t device_type, JsonObject & output, const int8_t id, const uint8_t output_target);
    static bool        command_commands(uint8_t device_type, JsonObject & output, const int8_t id);
    static bool        command_entities(uint8_t device_type, JsonObject & output, const int8_t id);

    static constexpr uint32_t EMS_FETCH_FREQUENCY = 60000; // check every minute
    static constexpr uint8_t  EMS_WAIT_KM_TIMEOUT = 60;    // wait one minute

    struct Device_record {
        uint8_t                     product_id;
        EMSdevice::DeviceType       device_type;
        const __FlashStringHelper * name;
        uint8_t                     flags;
    };
    static std::vector<Device_record> device_library_;

    static uint8_t  actual_master_thermostat_;
    static uint16_t watch_id_;
    static uint8_t  watch_;
    static uint16_t read_id_;
    static bool     read_next_;
    static uint16_t publish_id_;
    static bool     tap_water_active_;
    static uint8_t  publish_all_idx_;
    static uint8_t  unique_id_count_;
    static bool     trace_raw_;
    static uint16_t wait_validate_;
    static bool     wait_km_;
    static uint32_t last_fetch_;
};

} // namespace emsesp

#endif
