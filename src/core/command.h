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

#ifndef EMSESP_COMMAND_H_
#define EMSESP_COMMAND_H_

#include <unordered_map>

#include "console.h"

using uuid::console::Shell;

namespace emsesp {

#define COMMAND_MAX_LENGTH 50

// mqtt flags for command subscriptions
enum CommandFlag : uint8_t {
    CMD_FLAG_DEFAULT = 0,        // 0 no flags set, always subscribe to MQTT
    CMD_FLAG_HC      = (1 << 0), // 1 TAG_HC1 - TAG_HC8
    CMD_FLAG_DHW     = (1 << 1), // 2 TAG_DHW1 - TAG_DHW4
    CMD_FLAG_AHS     = (1 << 2), // 4 TAG_AHS1
    CMD_FLAG_HS      = (1 << 3), // 8 TAG_HS1 - TAG_HS16
    HIDDEN           = (1 << 6), // 64 do not show in API or Web
    ADMIN_ONLY       = (1 << 7)  // 128 requires authentication

};

// return status after calling a Command
enum CommandRet : uint8_t {
    FAIL = 0,    // 0 or FALSE
    OK,          // 1 or TRUE
    NOT_FOUND,   // 2
    ERROR,       // 3
    NOT_ALLOWED, // 4 - needs authentication
    INVALID,     // 5 - invalid (tag)
    NO_VALUE     // 6 - no value
};

using cmd_function_p      = std::function<bool(const char * data, const int8_t id)>;
using cmd_json_function_p = std::function<bool(const char * data, const int8_t id, JsonObject output)>;

class Command {
  public:
    struct CmdFunction {
        uint8_t              device_type_; // DeviceType::
        uint8_t              device_id_;
        uint8_t              flags_; // mqtt flags for command subscriptions
        const char *         cmd_;
        cmd_function_p       cmdfunction_;
        cmd_json_function_p  cmdfunction_json_;
        const char * const * description_;

        CmdFunction(const uint8_t             device_type,
                    const uint8_t             device_id,
                    const uint8_t             flags,
                    const char *              cmd,
                    const cmd_function_p      cmdfunction,
                    const cmd_json_function_p cmdfunction_json,
                    const char * const *      description)
            : device_type_(device_type)
            , device_id_(device_id)
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

    static uint8_t call(const uint8_t device_type, const char * cmd, const char * value, const bool is_admin, const int8_t id, JsonObject output);
    static uint8_t call(const uint8_t device_type, const char * cmd, const char * value, const int8_t id = -1);

    // with normal call back function taking a value and id
    static void add(const uint8_t        device_type,
                    const uint8_t        device_id,
                    const char *         cmd,
                    const cmd_function_p cb,
                    const char * const * description,
                    uint8_t              flags = CommandFlag::CMD_FLAG_DEFAULT);

    // same for system/temperature/analog devices
    static void
    add(const uint8_t device_type, const char * cmd, const cmd_function_p cb, const char * const * description, uint8_t flags = CommandFlag::CMD_FLAG_DEFAULT);
    // callback function taking value, id and a json object for its output
    static void add(const uint8_t             device_type,
                    const char *              cmd,
                    const cmd_json_function_p cb,
                    const char * const *      description,
                    uint8_t                   flags = CommandFlag::CMD_FLAG_DEFAULT);

    static void                   show_all(uuid::console::Shell & shell);
    static Command::CmdFunction * find_command(const uint8_t device_type, const uint8_t device_id, const char * cmd, const uint8_t flag);
    static std::string            tagged_cmd(const std::string & cmd, const uint8_t flag);

    static void erase_device_commands(const uint8_t device_type);
    static void erase_command(const uint8_t device_type, const char * cmd, uint8_t flag = CommandFlag::CMD_FLAG_DEFAULT);
    static void show(uuid::console::Shell & shell, uint8_t device_type, bool verbose);
    static void show_devices(uuid::console::Shell & shell);
    static bool device_has_commands(const uint8_t device_type);

    static bool list(const uint8_t device_type, JsonObject output);

    static uint8_t process(const char * path, const bool is_admin, const JsonObject input, JsonObject output);

    static const char * parse_command_string(const char * command, int8_t & id);
    static const char * get_attribute(const char * cmd);
    static bool         set_attribute(JsonObject output, const char * cmd, const char * attribute);

    static const char * return_code_string(const uint8_t return_code);

  private:
    static uuid::log::Logger logger_;

    static std::vector<CmdFunction> cmdfunctions_; // the list of commands

    static uint8_t json_message(uint8_t error_code, const char * message, JsonObject output, const char * object = nullptr);
};

class SUrlParser {
  private:
    std::unordered_map<std::string, std::string> m_keysvalues;
    std::vector<std::string>                     m_folders;

  public:
    SUrlParser() = default;
    SUrlParser(const char * url);

    bool parse(const char * url);

    std::vector<std::string> & paths() {
        return m_folders;
    };

    std::unordered_map<std::string, std::string> & params() {
        return m_keysvalues;
    };

    std::string path();
};

} // namespace emsesp

#endif