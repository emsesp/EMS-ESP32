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

#ifndef EMSESP_EMSDEVICE_H_
#define EMSESP_EMSDEVICE_H_

#include <string>
#include <vector>
#include <functional>

#include "emsfactory.h"
#include "telegram.h"
#include "mqtt.h"
#include "helpers.h"

namespace emsesp {

// Home Assistant icons (https://materialdesignicons.com/)
// MAKE_PSTR(icontemperature, "mdi:temperature-celsius")
MAKE_PSTR(icontemperature, "mdi:coolant-temperature")
MAKE_PSTR(iconpercent, "mdi:percent-outline")
MAKE_PSTR(iconfire, "mdi:fire")
MAKE_PSTR(iconfan, "mdi:fan")
MAKE_PSTR(iconflame, "mdi:flash")
MAKE_PSTR(iconvalve, "mdi:valve")
MAKE_PSTR(iconpump, "mdi:pump")
MAKE_PSTR(iconheatpump, "mdi:water-pump")

enum DeviceValueType : uint8_t {
    BOOL,
    INT,
    UINT,
    SHORT,
    USHORT,
    ULONG,
    TIME, // same as ULONG (32 bits)
    ENUM,
    TEXT

};

// Unit Of Measurement mapping - maps to DeviceValueUOM_s in emsdevice.cpp
// sequence is important!
// uom - also used with HA
MAKE_PSTR(percent, "%")
MAKE_PSTR(degrees, "°C")
MAKE_PSTR(kwh, "kWh")
MAKE_PSTR(wh, "Wh")
MAKE_PSTR(bar, "bar")
MAKE_PSTR(minutes, "minutes")
MAKE_PSTR(hours, "hours")
MAKE_PSTR(ua, "uA")
MAKE_PSTR(lmin, "l/min")
enum DeviceValueUOM : uint8_t { NONE = 0, DEGREES, PERCENT, LMIN, KWH, WH, HOURS, MINUTES, UA, BAR, PUMP };

// TAG mapping - maps to DeviceValueTAG_s in emsdevice.cpp
MAKE_PSTR(tag_boiler_data, "boiler")
MAKE_PSTR(tag_boiler_data_ww, "warm water")
MAKE_PSTR(tag_boiler_data_info, "info")
MAKE_PSTR(tag_hc1, "hc1")
MAKE_PSTR(tag_hc2, "hc2")
MAKE_PSTR(tag_hc3, "hc3")
MAKE_PSTR(tag_hc4, "hc4")
MAKE_PSTR(tag_wwc1, "wwc1")
MAKE_PSTR(tag_wwc2, "wwc2")
MAKE_PSTR(tag_wwc3, "wwc3")
MAKE_PSTR(tag_wwc4, "wwc4")
enum DeviceValueTAG : uint8_t {
    TAG_NONE = 0,
    TAG_BOILER_DATA,
    TAG_BOILER_DATA_WW,
    TAG_BOILER_DATA_INFO,
    TAG_HC1,
    TAG_HC2,
    TAG_HC3,
    TAG_HC4,
    TAG_WWC1,
    TAG_WWC2,
    TAG_WWC3,
    TAG_WWC4

};

class EMSdevice {
  public:
    virtual ~EMSdevice() = default; // destructor of base class must always be virtual because it's a polymorphic class

    static constexpr uint8_t EMS_DEVICES_MAX_TELEGRAMS = 20;

    // virtual functions overrules by derived classes
    virtual bool publish_ha_config() = 0;

    // device_type defines which derived class to use, e.g. BOILER, THERMOSTAT etc..
    EMSdevice(uint8_t device_type, uint8_t device_id, uint8_t product_id, const std::string & version, const std::string & name, uint8_t flags, uint8_t brand)
        : device_type_(device_type)
        , device_id_(device_id)
        , product_id_(product_id)
        , version_(version)
        , name_(name)
        , flags_(flags)
        , brand_(brand) {
    }

    inline uint8_t device_id() const {
        return device_id_;
    }

    std::string        device_type_name() const;
    static std::string device_type_2_device_name(const uint8_t device_type);
    static uint8_t     device_name_2_device_type(const char * topic);

    static const std::string uom_to_string(uint8_t uom);
    static const std::string tag_to_string(uint8_t tag);

    inline uint8_t product_id() const {
        return product_id_;
    }

    void product_id(uint8_t product_id) {
        product_id_ = product_id;
    }

    inline bool is_device_id(uint8_t device_id) {
        return ((device_id & 0x7F) == (device_id_ & 0x7F));
    }

    inline void add_flags(uint8_t flags) {
        flags_ |= flags;
    }
    inline bool has_flags(uint8_t flags) const {
        return (flags_ & flags) == flags;
    }
    inline void remove_flags(uint8_t flags) {
        flags_ &= ~flags;
    }
    inline uint8_t flags() const {
        return flags_;
    }

    // see enum DeviceType below
    inline uint8_t device_type() const {
        return device_type_;
    }

    inline void version(std::string & version) {
        version_ = version;
    }

    inline std::string version() const {
        return version_;
    }

    inline void brand(uint8_t brand) {
        brand_ = brand;
    }

    inline uint8_t brand() const {
        return brand_;
    }

    inline void name(const std::string & name) {
        name_ = name;
    }

    inline std::string name() const {
        return name_;
    }

    inline uint8_t unique_id() const {
        return unique_id_;
    }

    inline void unique_id(uint8_t unique_id) {
        unique_id_ = unique_id;
    }

    inline bool has_update() const {
        return has_update_;
    }

    inline void has_update(bool has_update) {
        has_update_ |= has_update;
    }

    std::string    brand_to_string() const;
    static uint8_t decode_brand(uint8_t value);

    std::string to_string() const;
    std::string to_string_short() const;

    void   show_telegram_handlers(uuid::console::Shell & shell);
    void   show_device_values_debug(uuid::console::Shell & shell);
    char * show_telegram_handlers(char * result);
    void   show_mqtt_handlers(uuid::console::Shell & shell);

    using process_function_p = std::function<void(std::shared_ptr<const Telegram>)>;

    void register_telegram_type(const uint16_t telegram_type_id, const __FlashStringHelper * telegram_type_name, bool fetch, process_function_p cb);
    bool handle_telegram(std::shared_ptr<const Telegram> telegram);

    std::string get_value_uom(const char * key);
    bool        generate_values_json(JsonObject & json, const uint8_t tag_filter, const bool nested, const bool console = false);
    bool        generate_values_json_web(JsonObject & json);

    void register_device_value(uint8_t                             tag,
                               void *                              value_p,
                               uint8_t                             type,
                               const __FlashStringHelper * const * options,
                               const __FlashStringHelper *         short_name,
                               const __FlashStringHelper *         full_name,
                               uint8_t                             uom = DeviceValueUOM::NONE);

    void write_command(const uint16_t type_id, const uint8_t offset, uint8_t * message_data, const uint8_t message_length, const uint16_t validate_typeid);
    void write_command(const uint16_t type_id, const uint8_t offset, const uint8_t value, const uint16_t validate_typeid);
    void write_command(const uint16_t type_id, const uint8_t offset, const uint8_t value);
    void read_command(const uint16_t type_id);

    void register_mqtt_topic(const std::string & topic, mqtt_subfunction_p f);
    void register_mqtt_cmd(const __FlashStringHelper * cmd, cmdfunction_p f);

    void publish_mqtt_ha_sensor();

    std::string telegram_type_name(std::shared_ptr<const Telegram> telegram);

    void fetch_values();
    void toggle_fetch(uint16_t telegram_id, bool toggle);
    bool get_toggle_fetch(uint16_t telegram_id);

    bool ha_config_done() const {
        return ha_config_done_;
    }

    void ha_config_done(const bool v) {
        ha_config_done_ = v;
    }

    enum Brand : uint8_t {
        NO_BRAND = 0, // 0
        BOSCH,        // 1
        JUNKERS,      // 2
        BUDERUS,      // 3
        NEFIT,        // 4
        SIEGER,       // 5
        WORCESTER     // 11
    };

    enum DeviceType : uint8_t {
        SYSTEM = 0,   // this is us (EMS-ESP)
        DALLASSENSOR, // for internal dallas sensors
        BOILER,
        THERMOSTAT,
        MIXER,
        SOLAR,
        HEATPUMP,
        GATEWAY,
        SWITCH,
        CONTROLLER,
        CONNECT,
        GENERIC,
        UNKNOWN
    };

    // static device IDs
    static constexpr uint8_t EMS_DEVICE_ID_BOILER = 0x08; // fixed device_id for Master Boiler/UBA

    // generic type IDs
    static constexpr uint16_t EMS_TYPE_VERSION    = 0x02; // type ID for Version information. Generic across all EMS devices.
    static constexpr uint16_t EMS_TYPE_UBADevices = 0x07; // EMS connected devices

    // device flags: The lower 4 bits hold the unique identifier, the upper 4 bits are used for specific flags
    static constexpr uint8_t EMS_DEVICE_FLAG_NONE = 0;

    // Solar Module
    static constexpr uint8_t EMS_DEVICE_FLAG_SM10  = 1;
    static constexpr uint8_t EMS_DEVICE_FLAG_SM100 = 2;
    static constexpr uint8_t EMS_DEVICE_FLAG_ISM   = 3;

    // Mixer Module
    static constexpr uint8_t EMS_DEVICE_FLAG_MMPLUS = 1;
    static constexpr uint8_t EMS_DEVICE_FLAG_MM10   = 2;
    static constexpr uint8_t EMS_DEVICE_FLAG_IPM    = 3;

    // Thermostats
    static constexpr uint8_t EMS_DEVICE_FLAG_NO_WRITE    = (1 << 7); // last bit
    static constexpr uint8_t EMS_DEVICE_FLAG_JUNKERS_OLD = (1 << 6); // 6th bit set if older models, like FR120, FR100
    static constexpr uint8_t EMS_DEVICE_FLAG_EASY        = 1;
    static constexpr uint8_t EMS_DEVICE_FLAG_RC10        = 2;
    static constexpr uint8_t EMS_DEVICE_FLAG_RC20        = 3;
    static constexpr uint8_t EMS_DEVICE_FLAG_RC20_2      = 4; // Variation on RC20, Older, like ES72
    static constexpr uint8_t EMS_DEVICE_FLAG_RC30_1      = 5; // variation on RC30, Newer models
    static constexpr uint8_t EMS_DEVICE_FLAG_RC30        = 6;
    static constexpr uint8_t EMS_DEVICE_FLAG_RC35        = 7;
    static constexpr uint8_t EMS_DEVICE_FLAG_RC300       = 8;
    static constexpr uint8_t EMS_DEVICE_FLAG_RC100       = 9;
    static constexpr uint8_t EMS_DEVICE_FLAG_JUNKERS     = 10;

    void reserve_device_values(uint8_t elements) {
        devicevalues_.reserve(elements);
    }

    void reserve_telgram_functions(uint8_t elements) {
        telegram_functions_.reserve(elements);
    }

  private:
    uint8_t     unique_id_;
    uint8_t     device_type_ = DeviceType::SYSTEM;
    uint8_t     device_id_   = 0;
    uint8_t     product_id_  = 0;
    std::string version_;
    std::string name_; // the long name for the EMS model
    uint8_t     flags_ = 0;
    uint8_t     brand_ = Brand::NO_BRAND;

    bool ha_config_done_ = false;
    bool has_update_     = false;

    struct TelegramFunction {
        uint16_t                    telegram_type_id_;   // it's type_id
        const __FlashStringHelper * telegram_type_name_; // e.g. RC20Message
        bool                        fetch_;              // if this type_id be queried automatically
        process_function_p          process_function_;

        TelegramFunction(uint16_t telegram_type_id, const __FlashStringHelper * telegram_type_name, bool fetch, process_function_p process_function)
            : telegram_type_id_(telegram_type_id)
            , telegram_type_name_(telegram_type_name)
            , fetch_(fetch)
            , process_function_(process_function) {
        }
    };

    struct DeviceValue {
        uint8_t                             device_type;  // EMSdevice::DeviceType
        uint8_t                             tag;          // DeviceValueTAG::*
        void *                              value_p;      // pointer to variable of any type
        uint8_t                             type;         // DeviceValueType::*
        const __FlashStringHelper * const * options;      // options as a flash char array
        uint8_t                             options_size; // # options in the char array, calculated
        const __FlashStringHelper *         short_name;   // used in MQTT
        const __FlashStringHelper *         full_name;    // used in Web and Console
        uint8_t                             uom;          // DeviceValueUOM::*

        DeviceValue(uint8_t                             device_type,
                    uint8_t                             tag,
                    void *                              value_p,
                    uint8_t                             type,
                    const __FlashStringHelper * const * options,
                    uint8_t                             options_size,
                    const __FlashStringHelper *         short_name,
                    const __FlashStringHelper *         full_name,
                    uint8_t                             uom)
            : device_type(device_type)
            , tag(tag)
            , value_p(value_p)
            , type(type)
            , options(options)
            , options_size(options_size)
            , short_name(short_name)
            , full_name(full_name)
            , uom(uom) {
        }
    };
    const std::vector<DeviceValue> devicevalues() const;

    void init_devicevalues(uint8_t size) {
        devicevalues_.reserve(size);
    }

    std::vector<TelegramFunction> telegram_functions_; // each EMS device has its own set of registered telegram types

    std::vector<DeviceValue> devicevalues_;
};

} // namespace emsesp

#endif
