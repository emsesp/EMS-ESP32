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

#ifndef EMSESP_EMSDEVICE_H_
#define EMSESP_EMSDEVICE_H_

#include "emsfactory.h"
#include "telegram.h"
#include "mqtt.h"
#include "helpers.h"
#include "emsdevicevalue.h"

namespace emsesp {

class EMSdevice {
  public:
    virtual ~EMSdevice() = default; // destructor of base class must always be virtual because it's a polymorphic class

    using process_function_p = std::function<void(std::shared_ptr<const Telegram>)>;

    // device_type defines which derived class to use, e.g. BOILER, THERMOSTAT etc..
    EMSdevice(uint8_t device_type, uint8_t device_id, uint8_t product_id, const char * version, const char * default_name, uint8_t flags, uint8_t brand)
        : device_type_(device_type)
        , device_id_(device_id)
        , product_id_(product_id)
        , default_name_(default_name)
        , flags_(flags)
        , brand_(brand) {
        strlcpy(version_, version, sizeof(version_));
    }

    // static functions, used outside the class like in console.cpp, command.cpp, emsesp.cpp, mqtt.cpp
    static const char * device_type_2_device_name(const uint8_t device_type);
    static uint8_t      device_name_2_device_type(const char * topic);
    static const char * tag_to_string(int8_t tag, const bool translate = true);
    static const char * uom_to_string(uint8_t uom);
    static const char * tag_to_mqtt(int8_t tag);
    static uint8_t      decode_brand(uint8_t value);
    static bool         export_values(uint8_t device_type, JsonObject output, const int8_t id, const uint8_t output_target);

    // non static functions

    const char * device_type_name();                     // returns short non-translated device type name
    const char * device_type_2_device_name_translated(); // returns translated device type name
    bool         has_tags(const int8_t tag) const;
    bool         has_cmd(const char * cmd, const int8_t id) const;
    const char * brand_to_char();
    std::string  to_string();
    std::string  to_string_short();
    std::string  name(); // returns either default or custom name of a device (if defined)

    bool is_device_id(uint8_t device_id) const {
        return ((device_id & 0x7F) == (device_id_ & 0x7F));
    }

    // Getters
    uint8_t device_id() const {
        return device_id_;
    }

    uint8_t product_id() const {
        return product_id_;
    }

    uint8_t device_type() const {
        return device_type_; // see enum DeviceType below
    }

    const char * version() const {
        return version_;
    }

    uint8_t brand() const {
        return brand_;
    }

    void active(bool active) {
        active_ = active;
    }

    const char * default_name() const {
        return default_name_;
    }

    // flags
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

    // set custom device name
    void custom_name(std::string const & custom_name) {
        custom_name_ = custom_name;
    }

    std::string custom_name() const {
        return custom_name_;
    }

    // set device model
    void model(std::string const & model) {
        model_ = model;
    }

    std::string model() const {
        return model_;
    }

    inline uint8_t unique_id() const {
        return unique_id_;
    }

    void unique_id(uint8_t unique_id) {
        unique_id_ = unique_id;
    }

    bool has_update() const {
        return has_update_;
    }

    void has_update(bool flag) {
        has_update_ = flag;
    }

    void has_update(void * value) {
        has_update_ = true;
        publish_value(value);
    }

    void has_update(char * value, const char * newvalue, size_t len) {
        if (value && newvalue && strcmp(value, newvalue) != 0) {
            strlcpy(value, newvalue, len);
            has_update_ = true;
            publish_value(value);
        }
    }

    void has_update(uint8_t & value, uint8_t newvalue) {
        if (value != newvalue) {
            value       = newvalue;
            has_update_ = true;
            publish_value((void *)&value);
        }
    }

    void has_update(uint16_t & value, uint16_t newvalue) {
        if (value != newvalue) {
            value       = newvalue;
            has_update_ = true;
            publish_value((void *)&value);
        }
    }

    void has_update(int16_t & value, int16_t newvalue) {
        if (value != newvalue) {
            value       = newvalue;
            has_update_ = true;
            publish_value((void *)&value);
        }
    }

    void has_update(uint32_t & value, uint32_t newvalue) {
        if (value != newvalue) {
            value       = newvalue;
            has_update_ = true;
            publish_value((void *)&value);
        }
    }

    void has_enumupdate(std::shared_ptr<const Telegram> telegram, uint8_t & value, const uint8_t index, int8_t s = 0) {
        if (telegram->read_enumvalue(value, index, s)) {
            has_update_ = true;
            publish_value((void *)&value);
        }
    }

    template <typename Value>
    void has_update(std::shared_ptr<const Telegram> telegram, Value & value, const uint8_t index, uint8_t s = 0) {
        if (telegram->read_value(value, index, s)) {
            has_update_ = true;
            publish_value((void *)&value);
        }
    }

    template <typename BitValue>
    void has_bitupdate(std::shared_ptr<const Telegram> telegram, BitValue & value, const uint8_t index, uint8_t b) {
        if (telegram->read_bitvalue(value, index, b)) {
            has_update_ = true;
            publish_value((void *)&value);
        }
    }

    // modbus
    int get_modbus_value(uint8_t tag, const std::string & shortname, std::vector<uint16_t> & result);
    int modbus_value_to_json(uint8_t tag, const std::string & shortname, const std::vector<uint8_t> & modbus_data, JsonObject jsonValue);

    enum Handlers : uint8_t { ALL, RECEIVED, FETCHED, PENDING, IGNORED };
    void   show_telegram_handlers(uuid::console::Shell & shell) const;
    char * show_telegram_handlers(char * result, const size_t len, const uint8_t handlers);
    void   show_mqtt_handlers(uuid::console::Shell & shell) const;
    void   add_handlers_ignored(const uint16_t handler);

    void set_climate_minmax(int8_t tag, int16_t min, uint32_t max);
    void setValueEnum(const void * value_p, const char * const ** options);
    void setCustomizationEntity(const std::string & entity_id);
    void getCustomizationEntities(std::vector<std::string> & entity_ids);

    void register_telegram_type(const uint16_t telegram_type_id, const char * telegram_type_name, bool fetch, const process_function_p cb);
    bool handle_telegram(std::shared_ptr<const Telegram> telegram);

    std::string get_value_uom(const std::string & shortname) const;
    bool        get_value_info(JsonObject root, const char * cmd, const int8_t id);
    void        get_value_json(JsonObject output, DeviceValue & dv);
    void        get_dv_info(JsonObject json);

    enum OUTPUT_TARGET : uint8_t { API_VERBOSE, API_SHORTNAMES, MQTT, CONSOLE };
    bool generate_values(JsonObject output, const int8_t tag_filter, const bool nested, const uint8_t output_target);
    void generate_values_web(JsonObject output, const bool is_dashboard = false);
    void generate_values_web_customization(JsonArray output);

    void add_device_value(int8_t                tag,
                          void *                value_p,
                          uint8_t               type,
                          const char * const ** options,
                          const char * const *  options_single,
                          int8_t                numeric_operator,
                          const char * const *  name,
                          uint8_t               uom,
                          const cmd_function_p  f,
                          int16_t               min,
                          uint32_t              max);

    void register_device_value(int8_t                tag,
                               void *                value_p,
                               uint8_t               type,
                               const char * const ** options,
                               const char * const *  name,
                               uint8_t               uom,
                               const cmd_function_p  f,
                               int16_t               min,
                               uint32_t              max);

    void
    register_device_value(int8_t tag, void * value_p, uint8_t type, const char * const ** options, const char * const * name, uint8_t uom, const cmd_function_p f);

    void register_device_value(int8_t tag, void * value_p, uint8_t type, const char * const ** options, const char * const * name, uint8_t uom);

    void register_device_value(int8_t               tag,
                               void *               value_p,
                               uint8_t              type,
                               int8_t               numeric_operator,
                               const char * const * name,
                               uint8_t              uom,
                               const cmd_function_p f = nullptr);

    void register_device_value(int8_t               tag,
                               void *               value_p,
                               uint8_t              type,
                               int8_t               numeric_operator,
                               const char * const * name,
                               uint8_t              uom,
                               const cmd_function_p f,
                               int16_t              min,
                               uint32_t             max);

    // single list of options
    void register_device_value(int8_t               tag,
                               void *               value_p,
                               uint8_t              type,
                               const char * const * options_single,
                               const char * const * name,
                               uint8_t              uom,
                               const cmd_function_p f = nullptr);

    // single list of options, with no translations, with min and max
    void register_device_value(int8_t               tag,
                               void *               value_p,
                               uint8_t              type,
                               const char * const * options_single,
                               const char * const * name,
                               uint8_t              uom,
                               const cmd_function_p f,
                               int16_t              min,
                               uint32_t             max);

    // no options, optional function f
    void register_device_value(int8_t tag, void * value_p, uint8_t type, const char * const * name, uint8_t uom, const cmd_function_p f = nullptr);

    // no options, with min/max
    void register_device_value(int8_t tag, void * value_p, uint8_t type, const char * const * name, uint8_t uom, const cmd_function_p f, int16_t min, uint32_t max);

    void write_command(const uint16_t type_id, const uint8_t offset, uint8_t * message_data, const uint8_t message_length, const uint16_t validate_typeid) const;
    void write_command(const uint16_t type_id, const uint8_t offset, const uint8_t value, const uint16_t validate_typeid) const;
    void write_command(const uint16_t type_id, const uint8_t offset, const uint8_t value) const;

    void read_command(const uint16_t type_id, uint8_t offset = 0, uint8_t length = 0) const;

    bool         is_readable(const void * value_p) const;
    bool         is_readonly(const std::string & cmd, const int8_t id) const;
    bool         has_command(const void * value_p) const;
    void         set_minmax(const void * value_p, int16_t min, uint32_t max);
    void         publish_value(void * value_p) const;
    void         publish_all_values();
    void         mqtt_ha_entity_config_create();
    const char * telegram_type_name(std::shared_ptr<const Telegram> telegram);
    void         fetch_values();
    void         toggle_fetch(uint16_t telegram_id, bool toggle);
    bool         is_fetch(uint16_t telegram_id) const;
    bool         is_received(uint16_t telegram_id) const;
    bool         has_telegram_id(uint16_t id) const;
    void         ha_config_clear();

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
        WORCESTER,    // 11
        IVT           // 13
    };

    // Unique Identifiers for each Device type, used in Dashboard table
    // 100 and above is reserved for DeviceType
    enum DeviceTypeUniqueID : uint8_t {
        SCHEDULER_UID         = 96,
        ANALOGSENSOR_UID      = 97,
        TEMPERATURESENSOR_UID = 98,
        CUSTOM_UID            = 99 // always 99
    };

    enum DeviceType : uint8_t {
        SYSTEM = 0,        // this is us (EMS-ESP)
        TEMPERATURESENSOR, // for internal temperature sensors
        ANALOGSENSOR,      // for internal analog sensors
        SCHEDULER,         // for internal schedule
        CUSTOM,            // for user defined entities
        BOILER,            // from here on enum the ems-devices
        THERMOSTAT,
        MIXER,
        SOLAR,
        HEATPUMP,
        GATEWAY,
        SWITCH,
        CONTROLLER,
        CONNECT,
        ALERT,
        EXTENSION,
        GENERIC,
        HEATSOURCE,
        VENTILATION,
        WATER,
        POOL,
        UNKNOWN
    };

    static constexpr uint8_t EMS_DEVICES_MAX_TELEGRAMS = 20;

    // static device IDs
    static constexpr uint8_t EMS_DEVICE_ID_BOILER         = 0x08; // fixed device_id for Master Boiler/UBA
    static constexpr uint8_t EMS_DEVICE_ID_HS1            = 0x70; // fixed device_id for 1st. Cascade Boiler/UBA
    static constexpr uint8_t EMS_DEVICE_ID_HS16           = 0x7F; // fixed device_id for last Cascade Boiler/UBA
    static constexpr uint8_t EMS_DEVICE_ID_AHS1           = 0x60; // fixed device_id for alternative Heating AM200
    static constexpr uint8_t EMS_DEVICE_ID_CONTROLLER     = 0x09;
    static constexpr uint8_t EMS_DEVICE_ID_RS232          = 0x04;
    static constexpr uint8_t EMS_DEVICE_ID_TERMINAL       = 0x0A;
    static constexpr uint8_t EMS_DEVICE_ID_SERVICEKEY     = 0x0B;
    static constexpr uint8_t EMS_DEVICE_ID_CASCADE        = 0x0C;
    static constexpr uint8_t EMS_DEVICE_ID_EASYCOM        = 0x0D;
    static constexpr uint8_t EMS_DEVICE_ID_CONVERTER      = 0x0E;
    static constexpr uint8_t EMS_DEVICE_ID_CLOCK          = 0x0F;
    static constexpr uint8_t EMS_DEVICE_ID_SWITCH         = 0x11; // Switch WM10
    static constexpr uint8_t EMS_DEVICE_ID_ALERT          = 0x12; // Error module EM10
    static constexpr uint8_t EMS_DEVICE_ID_EXTENSION      = 0x15; // Extension module EM1000, Pump module PM10
    static constexpr uint8_t EMS_DEVICE_ID_MODEM          = 0x48;
    static constexpr uint8_t EMS_DEVICE_ID_RFSENSOR       = 0x40; // RF sensor only sending, no reply
    static constexpr uint8_t EMS_DEVICE_ID_RFBASE         = 0x50;
    static constexpr uint8_t EMS_DEVICE_ID_ROOMTHERMOSTAT = 0x17; // TADO using this with no version reply #174
    static constexpr uint8_t EMS_DEVICE_ID_TADO_OLD       = 0x19; // older TADO using this with no version reply, #1031
    static constexpr uint8_t EMS_DEVICE_ID_MIXER1         = 0x20; // e.g MH210 module as mixer
    static constexpr uint8_t EMS_DEVICE_ID_MIXER8         = 0x27;
    static constexpr uint8_t EMS_DEVICE_ID_DHW1           = 0x28; // MM100 module as water station
    static constexpr uint8_t EMS_DEVICE_ID_DHW2           = 0x29; // MM100 module as water station
    static constexpr uint8_t EMS_DEVICE_ID_DHW8           = 0x2F; // last DHW module id?

    // generic type IDs
    static constexpr uint16_t EMS_TYPE_NAME        = 0x01; // device config for ems devices, name ascii on offset 27ff  for ems+
    static constexpr uint16_t EMS_TYPE_VERSION     = 0x02; // type ID for Version information. Generic across all EMS devices.
    static constexpr uint16_t EMS_TYPE_UBADevices  = 0x07; // EMS connected devices
    static constexpr uint16_t EMS_TYPE_DEVICEERROR = 0xBE;
    static constexpr uint16_t EMS_TYPE_SYSTEMERROR = 0xBF;
    static constexpr uint16_t EMS_TYPE_MENUCONFIG  = 0xF7;
    static constexpr uint16_t EMS_TYPE_VALUECONFIG = 0xF9;

    // device flags: The lower 4 bits hold the unique identifier, the upper 4 bits are used for specific flags
    static constexpr uint8_t EMS_DEVICE_FLAG_NONE = 0;

    // Controller
    static constexpr uint8_t EMS_DEVICE_FLAG_IVT = 1;

    // Boiler
    static constexpr uint8_t EMS_DEVICE_FLAG_EMS      = 1;
    static constexpr uint8_t EMS_DEVICE_FLAG_EMSPLUS  = 2;
    static constexpr uint8_t EMS_DEVICE_FLAG_HT3      = 3;
    static constexpr uint8_t EMS_DEVICE_FLAG_HEATPUMP = 4;
    static constexpr uint8_t EMS_DEVICE_FLAG_HYBRID   = 5;
    static constexpr uint8_t EMS_DEVICE_FLAG_HIU      = 6;

    // Solar Module
    static constexpr uint8_t EMS_DEVICE_FLAG_SM10  = 1;
    static constexpr uint8_t EMS_DEVICE_FLAG_SM100 = 2;
    static constexpr uint8_t EMS_DEVICE_FLAG_ISM   = 3;

    // Mixer Module
    static constexpr uint8_t EMS_DEVICE_FLAG_MMPLUS = 1;
    static constexpr uint8_t EMS_DEVICE_FLAG_MM10   = 2;
    static constexpr uint8_t EMS_DEVICE_FLAG_IPM    = 3;
    static constexpr uint8_t EMS_DEVICE_FLAG_MP     = 4;

    // Thermostats
    static constexpr uint8_t EMS_DEVICE_FLAG_NO_WRITE    = (1 << 7); // last bit
    static constexpr uint8_t EMS_DEVICE_FLAG_JUNKERS_OLD = (1 << 6); // 6th bit set if older models, like FR120, FR100
    static constexpr uint8_t EMS_DEVICE_FLAG_EASY        = 1;
    static constexpr uint8_t EMS_DEVICE_FLAG_RC10        = 2;
    static constexpr uint8_t EMS_DEVICE_FLAG_RC20        = 3;
    static constexpr uint8_t EMS_DEVICE_FLAG_RC20_N      = 4; // Variation on RC20, Older, like ES72
    static constexpr uint8_t EMS_DEVICE_FLAG_RC25        = 5;
    static constexpr uint8_t EMS_DEVICE_FLAG_RC30_N      = 6; // variation on RC30, Newer models
    static constexpr uint8_t EMS_DEVICE_FLAG_RC30        = 7;
    static constexpr uint8_t EMS_DEVICE_FLAG_RC35        = 8;
    static constexpr uint8_t EMS_DEVICE_FLAG_RC300       = 9;
    static constexpr uint8_t EMS_DEVICE_FLAG_RC100       = 10;
    static constexpr uint8_t EMS_DEVICE_FLAG_JUNKERS     = 11;
    static constexpr uint8_t EMS_DEVICE_FLAG_CRF         = 12; // CRF200 only monitor
    static constexpr uint8_t EMS_DEVICE_FLAG_RC100H      = 13; // with humidity
    static constexpr uint8_t EMS_DEVICE_FLAG_BC400       = 14; // mostly like RC300, but some changes
    static constexpr uint8_t EMS_DEVICE_FLAG_R3000       = 15; // Rego3000, same as RC300 with different wwmodes
    static constexpr uint8_t EMS_DEVICE_FLAG_CR120       = 16; // mostly like RC300, but some changes
    static constexpr uint8_t EMS_DEVICE_FLAG_CR11        = 17; // CRF200 only monitor

    uint8_t count_entities();
    uint8_t count_entities_fav();
    bool    has_entities() const;

    // void reserve_device_values(uint8_t elements) {
    //     devicevalues_.reserve(elements);
    // }

    // void reserve_telegram_functions(uint8_t elements) {
    //     telegram_functions_.reserve(elements);
    // }

#if defined(EMSESP_STANDALONE)
    struct TelegramFunctionDump {
        uint16_t     type_id_;
        const char * name_;
        bool         fetch_;
        TelegramFunctionDump(uint16_t type_id, const char * name, bool fetch)
            : type_id_(type_id)
            , name_(name)
            , fetch_(fetch) {
        }
    };
    void dump_telegram_info(std::vector<TelegramFunctionDump> & telegram_functions_dump);
    void dump_devicevalue_info();
#endif

  private:
    uint8_t      unique_id_;
    uint8_t      device_type_ = DeviceType::SYSTEM;
    uint8_t      device_id_   = 0;
    uint8_t      product_id_  = 0;
    char         version_[6];
    const char * default_name_;     // the fixed name the EMS model taken from the device library
    std::string  custom_name_ = ""; // custom name
    std::string  model_       = ""; // model, taken from the 0x01 telegram. see process_deviceName()
    uint8_t      flags_       = 0;
    uint8_t      brand_       = Brand::NO_BRAND;
    bool         active_      = true;

    bool ha_config_done_ = false;
    bool has_update_     = false;

    struct TelegramFunction {
        const uint16_t           telegram_type_id_;   // it's type_id
        const char *             telegram_type_name_; // e.g. RC20Message
        bool                     fetch_;              // if this type_id be queried automatically
        bool                     received_;
        const process_function_p process_function_;

        TelegramFunction(uint16_t telegram_type_id, const char * telegram_type_name, bool fetch, bool received, const process_function_p process_function)
            : telegram_type_id_(telegram_type_id)
            , telegram_type_name_(telegram_type_name)
            , fetch_(fetch)
            , received_(received)
            , process_function_(process_function) {
        }
    };

    std::vector<uint16_t> handlers_ignored_;

#if defined(EMSESP_STANDALONE) || defined(EMSESP_TEST)
  public: // so we can call it from WebCustomizationService::test() and EMSESP::dump_all_entities()
#endif
    std::vector<TelegramFunction> telegram_functions_; // each EMS device has its own set of registered telegram types
    std::vector<DeviceValue>      devicevalues_;       // all the device values
};

} // namespace emsesp

#endif
