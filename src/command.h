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

#ifndef EMSESP_COMMAND_H_
#define EMSESP_COMMAND_H_

#include <Arduino.h>
#include <ArduinoJson.h>

#include <string>
#include <vector>
#include <functional>
#include <unordered_map>

#include "console.h"

#include <uuid/log.h>

using uuid::console::Shell;

namespace emsesp {

// mqtt flags for command subscriptions
enum CommandFlag : uint8_t {
    MQTT_SUB_FLAG_DEFAULT = 0,        // 0 no flags set, always subscribe to MQTT
    MQTT_SUB_FLAG_HC      = (1 << 0), // 1 TAG_HC1 - TAG_HC4
    MQTT_SUB_FLAG_WWC     = (1 << 1), // 2 TAG_WWC1 - TAG_WWC4
    MQTT_SUB_FLAG_NOSUB   = (1 << 2), // 4
    MQTT_SUB_FLAG_WW      = (1 << 3), // 8 TAG_DEVICE_DATA_WW
    HIDDEN                = (1 << 4), // 16 do not show in API or Web
    ADMIN_ONLY            = (1 << 5)  // 32 requires authentication

};

// return status after calling a Command
enum CommandRet : uint8_t {
    ERRORED = 0, // 0 or FALSE
    OK,          // 1 or TRUE
    NOT_FOUND,   // 2
    ERROR,       // 3
    NOT_ALLOWED  // needs authentication

};

using cmd_function_p      = std::function<bool(const char * data, const int8_t id)>;
using cmd_json_function_p = std::function<bool(const char * data, const int8_t id, JsonObject & output)>;

class Command {
  public:
    struct CmdFunction {
        uint8_t                     device_type_; // DeviceType::
        uint8_t                     flags_;       // mqtt flags for command subscriptions
        const __FlashStringHelper * cmd_;
        const cmd_function_p        cmdfunction_;
        const cmd_json_function_p   cmdfunction_json_;
        const __FlashStringHelper * description_;

        CmdFunction(const uint8_t               device_type,
                    const uint8_t               flags,
                    const __FlashStringHelper * cmd,
                    const cmd_function_p        cmdfunction,
                    const cmd_json_function_p   cmdfunction_json,
                    const __FlashStringHelper * description)
            : device_type_(device_type)
            , flags_(flags)
            , cmd_(cmd)
            , cmdfunction_(cmdfunction)
            , cmdfunction_json_(cmdfunction_json)
            , description_(description) {
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
    };

    static std::vector<CmdFunction> commands() {
        return cmdfunctions_;
    }

#define add_

    static uint8_t call(const uint8_t device_type, const char * cmd, const char * value, bool authenticated, const int8_t id, JsonObject & output);
    static uint8_t call(const uint8_t device_type, const char * cmd, const char * value);

    // with normal call back function taking a value and id
    static void add(const uint8_t               device_type,
                    const __FlashStringHelper * cmd,
                    const cmd_function_p        cb,
                    const __FlashStringHelper * description,
                    uint8_t                     flags = CommandFlag::MQTT_SUB_FLAG_DEFAULT);

    // callback function taking value, id and a json object for its output
    static void add(const uint8_t               device_type,
                    const __FlashStringHelper * cmd,
                    const cmd_json_function_p   cb,
                    const __FlashStringHelper * description,
                    uint8_t                     flags = CommandFlag::MQTT_SUB_FLAG_DEFAULT);

    static void                   show_all(uuid::console::Shell & shell);
    static Command::CmdFunction * find_command(const uint8_t device_type, const char * cmd);

    static void show(uuid::console::Shell & shell, uint8_t device_type, bool verbose);
    static void show_devices(uuid::console::Shell & shell);
    static bool device_has_commands(const uint8_t device_type);

    static bool list(const uint8_t device_type, JsonObject & output);

    static uint8_t process(const char * path, const bool authenticated, const JsonObject & input, JsonObject & output);

    static const char * parse_command_string(const char * command, int8_t & id);

  private:
    static uuid::log::Logger logger_;

    static std::vector<CmdFunction> cmdfunctions_; // list of commands
};

typedef std::unordered_map<std::string, std::string> KeyValueMap_t;
typedef std::vector<std::string>                     Folder_t;

class SUrlParser {
  private:
    KeyValueMap_t m_keysvalues;
    Folder_t      m_folders;

  public:
    SUrlParser(){};
    SUrlParser(const char * url);

    bool parse(const char * url);

    Folder_t & paths() {
        return m_folders;
    };

    KeyValueMap_t & params() {
        return m_keysvalues;
    };

    std::string path();
};

} // namespace emsesp

#endif