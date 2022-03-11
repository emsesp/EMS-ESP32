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

    static constexpr uint8_t EMS_DEVICES_MAX_TELEGRAMS = 20;

    // device_type defines which derived class to use, e.g. BOILER, THERMOSTAT etc..
    EMSdevice(uint8_t device_type, uint8_t device_id, uint8_t product_id, const char * version, const std::string & name, uint8_t flags, uint8_t brand)
        : device_type_(device_type)
        , device_id_(device_id)
        , product_id_(product_id)
        , name_(name)
        , flags_(flags)
        , brand_(brand) {
        strlcpy(version_, version, sizeof(version_));
    }

    std::string device_type_name() const;

    static std::string device_type_2_device_name(const uint8_t device_type);
    static uint8_t     device_name_2_device_type(const char * topic);
    static std::string uom_to_string(uint8_t uom);
    static std::string tag_to_string(uint8_t tag);
    static std::string tag_to_mqtt(uint8_t tag);

    inline uint8_t device_id() const {
        return device_id_;
    }

    inline uint8_t product_id() const {
        return product_id_;
    }

    void product_id(uint8_t product_id) {
        product_id_ = product_id;
    }

    inline bool is_device_id(uint8_t device_id) const {
        return ((device_id & 0x7F) == (device_id_ & 0x7F));
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

    inline uint8_t device_type() const {
        return device_type_; // see enum DeviceType below
    }

    inline void version(const char * version) {
        strlcpy(version_, version, sizeof(version_));
    }

    inline const char * version() const {
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

    // unique id of a device
    inline uint8_t unique_id() const {
        return unique_id_;
    }

    inline void unique_id(uint8_t unique_id) {
        unique_id_ = unique_id;
    }

    inline bool has_update() const {
        return has_update_;
    }

    inline void has_update(bool flag) {
        has_update_ = flag;
    }

    inline void has_update(void * value) {
        has_update_ = true;
        publish_value(value);
    }

    inline void has_update(char * value, const char * newvalue, size_t len) {
        if (strcmp(value, newvalue) != 0) {
            strlcpy(value, newvalue, len);
            has_update_ = true;
            publish_value(value);
        }
    }

    inline void has_update(uint8_t & value, uint8_t newvalue) {
        if (value != newvalue) {
            value       = newvalue;
            has_update_ = true;
            publish_value((void *)&value);
        }
    }

    inline void has_enumupdate(std::shared_ptr<const Telegram> telegram, uint8_t & value, const uint8_t index, uint8_t s = 0) {
        if (telegram->read_enumvalue(value, index, s)) {
            has_update_ = true;
            publish_value((void *)&value);
        }
    }

    template <typename Value>
    inline void has_update(std::shared_ptr<const Telegram> telegram, Value & value, const uint8_t index, uint8_t s = 0) {
        if (telegram->read_value(value, index, s)) {
            has_update_ = true;
            publish_value((void *)&value);
        }
    }

    template <typename BitValue>
    inline void has_bitupdate(std::shared_ptr<const Telegram> telegram, BitValue & value, const uint8_t index, uint8_t b) {
        if (telegram->read_bitvalue(value, index, b)) {
            has_update_ = true;
            publish_value((void *)&value);
        }
    }

    std::string    brand_to_string() const;
    static uint8_t decode_brand(uint8_t value);

    std::string to_string() const;
    std::string to_string_short() const;

    enum Handlers : uint8_t { ALL, RECEIVED, FETCHED, PENDING };

    void   show_telegram_handlers(uuid::console::Shell & shell) const;
    char * show_telegram_handlers(char * result, const size_t len, const uint8_t handlers);
    void   show_mqtt_handlers(uuid::console::Shell & shell) const;
    void   list_device_entries(JsonObject & output) const;
    void   exclude_entity(uint8_t entity_id);
    void   reset_exclude_entities();

    using process_function_p = std::function<void(std::shared_ptr<const Telegram>)>;

    void register_telegram_type(const uint16_t telegram_type_id, const __FlashStringHelper * telegram_type_name, bool fetch, const process_function_p cb);
    bool handle_telegram(std::shared_ptr<const Telegram> telegram);

    std::string get_value_uom(const char * key) const;
    bool        get_value_info(JsonObject & root, const char * cmd, const int8_t id);
    void        get_dv_info(JsonObject & json);

    enum OUTPUT_TARGET : uint8_t { API_VERBOSE, API_SHORTNAMES, MQTT };
    bool generate_values(JsonObject & output, const uint8_t tag_filter, const bool nested, const uint8_t output_target);
    void generate_values_web(JsonObject & output);
    void generate_values_web_all(JsonArray & output);

    void register_device_value(uint8_t                             tag,
                               void *                              value_p,
                               uint8_t                             type,
                               const __FlashStringHelper * const * options,
                               const __FlashStringHelper *         short_name,
                               const __FlashStringHelper *         full_name,
                               uint8_t                             uom,
                               bool                                has_cmd,
                               int16_t                             min,
                               uint16_t                            max);

    void register_device_value(uint8_t                             tag,
                               void *                              value_p,
                               uint8_t                             type,
                               const __FlashStringHelper * const * options,
                               const __FlashStringHelper * const * name,
                               uint8_t                             uom,
                               const cmd_function_p                f,
                               int16_t                             min,
                               uint16_t                            max);

    void register_device_value(uint8_t                             tag,
                               void *                              value_p,
                               uint8_t                             type,
                               const __FlashStringHelper * const * options,
                               const __FlashStringHelper * const * name,
                               uint8_t                             uom,
                               const cmd_function_p                f);

    void register_device_value(uint8_t                             tag,
                               void *                              value_p,
                               uint8_t                             type,
                               const __FlashStringHelper * const * options,
                               const __FlashStringHelper * const * name,
                               uint8_t                             uom);

    void write_command(const uint16_t type_id, const uint8_t offset, uint8_t * message_data, const uint8_t message_length, const uint16_t validate_typeid) const;
    void write_command(const uint16_t type_id, const uint8_t offset, const uint8_t value, const uint16_t validate_typeid) const;
    void write_command(const uint16_t type_id, const uint8_t offset, const uint8_t value) const;

    void read_command(const uint16_t type_id, uint8_t offset = 0, uint8_t length = 0) const;

    bool is_visible(const void * value_p) const;
    void publish_value(void * value_p) const;
    void publish_all_values();

    void mqtt_ha_entity_config_create();
    void mqtt_ha_entity_config_remove();

    std::string telegram_type_name(std::shared_ptr<const Telegram> telegram) const;

    void fetch_values();
    void toggle_fetch(uint16_t telegram_id, bool toggle);
    bool is_fetch(uint16_t telegram_id) const;
    bool has_telegram_id(uint16_t id) const;
    void ha_config_clear();

    bool ha_config_done() const {
        return ha_config_done_;
    }
    void ha_config_done(const bool v) {
        ha_config_done_ = v;
    }

    bool ha_config_firstrun() const {
        return ha_config_firstrun_;
    }
    void ha_config_firstrun(const bool v) {
        ha_config_firstrun_ = v;
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

    enum DeviceType : uint8_t {
        SYSTEM = 0,   // this is us (EMS-ESP)
        DALLASSENSOR, // for internal dallas sensors
        ANALOGSENSOR, // for internal analog sensors
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
    static constexpr uint8_t EMS_DEVICE_ID_BOILER   = 0x08; // fixed device_id for Master Boiler/UBA
    static constexpr uint8_t EMS_DEVICE_ID_BOILER_1 = 0x70; // fixed device_id for 1st. Cascade Boiler/UBA
    static constexpr uint8_t EMS_DEVICE_ID_BOILER_F = 0x7F; // fixed device_id for last Cascade Boiler/UBA

    // generic type IDs
    static constexpr uint16_t EMS_TYPE_VERSION    = 0x02; // type ID for Version information. Generic across all EMS devices.
    static constexpr uint16_t EMS_TYPE_UBADevices = 0x07; // EMS connected devices

    // device flags: The lower 4 bits hold the unique identifier, the upper 4 bits are used for specific flags
    static constexpr uint8_t EMS_DEVICE_FLAG_NONE = 0;

    // Boiler
    static constexpr uint8_t EMS_DEVICE_FLAG_EMS      = 1;
    static constexpr uint8_t EMS_DEVICE_FLAG_EMSPLUS  = 2;
    static constexpr uint8_t EMS_DEVICE_FLAG_HT3      = 3;
    static constexpr uint8_t EMS_DEVICE_FLAG_HEATPUMP = 4;

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

    uint8_t count_entities();
    bool    has_entities() const;

  private:
    uint8_t     unique_id_;
    uint8_t     device_type_ = DeviceType::SYSTEM;
    uint8_t     device_id_   = 0;
    uint8_t     product_id_  = 0;
    char        version_[6];
    std::string name_; // the long name for the EMS model
    uint8_t     flags_ = 0;
    uint8_t     brand_ = Brand::NO_BRAND;

    bool ha_config_done_     = false;
    bool has_update_         = false;
    bool ha_config_firstrun_ = true; // this means a first setup of HA is needed after a restart

    struct TelegramFunction {
        uint16_t                    telegram_type_id_;   // it's type_id
        const __FlashStringHelper * telegram_type_name_; // e.g. RC20Message
        bool                        fetch_;              // if this type_id be queried automatically
        bool                        received_;
        process_function_p          process_function_;

        TelegramFunction(uint16_t telegram_type_id, const __FlashStringHelper * telegram_type_name, bool fetch, bool received, const process_function_p process_function)
            : telegram_type_id_(telegram_type_id)
            , telegram_type_name_(telegram_type_name)
            , fetch_(fetch)
            , received_(received)
            , process_function_(process_function) {
        }
    };

    std::vector<TelegramFunction> telegram_functions_; // each EMS device has its own set of registered telegram types

    // device values
    std::vector<DeviceValue> devicevalues_;

    uint8_t dv_index_ = 0; // unique counter for each added device value
    uint8_t get_next_dv_id() {
        return (dv_index_++);
    }
};

} // namespace emsesp

#endif
