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

#include "command.h"
#include "emsdevice.h"
#include "emsesp.h"

namespace emsesp {

uuid::log::Logger Command::logger_{F_(command), uuid::log::Facility::DAEMON};

std::vector<Command::CmdFunction> Command::cmdfunctions_;

// calls a command
// id may be used to represent a heating circuit for example, it's optional
// returns 0 if the command errored, 1 (TRUE) if ok, 2 if not found, 3 if error or 4 if not allowed
uint8_t Command::call(const uint8_t device_type, const char * cmd, const char * value, bool authenticated, const int8_t id) {
    int8_t id_new      = id;
    char   cmd_new[30] = {'\0'};
    strlcpy(cmd_new, cmd, sizeof(cmd_new));

    // find the command
    auto cf = find_command(device_type, cmd_new, id_new);
    if ((cf == nullptr) || (cf->cmdfunction_json_)) {
        LOG_WARNING(F("Command %s on %s not found"), cmd, EMSdevice::device_type_2_device_name(device_type).c_str());
        return CommandRet::NOT_FOUND;
    }

    // check if we're allowed to call it
    if (cf->has_flags(CommandFlag::ADMIN_ONLY) && !authenticated) {
        LOG_WARNING(F("Command %s on %s requires valid authorization"), cmd, EMSdevice::device_type_2_device_name(device_type).c_str());
        return CommandRet::NOT_ALLOWED;
    }

    std::string dname = EMSdevice::device_type_2_device_name(device_type);
    if (value == nullptr) {
        LOG_INFO(F("Calling %s command '%s'"), dname.c_str(), cmd);
    } else if (id == -1) {
        LOG_INFO(F("Calling %s command '%s', value %s, id is default"), dname.c_str(), cmd, value);
    } else {
        LOG_INFO(F("Calling %s command '%s', value %s, id is %d"), dname.c_str(), cmd, value, id);
    }

    return ((cf->cmdfunction_)(value, id_new)) ? CommandRet::OK : CommandRet::ERROR;
}

// calls a command. Takes a json object for output.
// id may be used to represent a heating circuit for example
// returns 0 if the command errored, 1 (TRUE) if ok, 2 if not found, 3 if error or 4 if not allowed
uint8_t Command::call(const uint8_t device_type, const char * cmd, const char * value, bool authenticated, const int8_t id, JsonObject & json) {
    int8_t id_new      = id;
    char   cmd_new[30] = {'\0'};
    strlcpy(cmd_new, cmd, sizeof(cmd_new));

    auto cf = find_command(device_type, cmd_new, id_new);

    // check if we're allowed to call it
    if (cf != nullptr) {
        if (cf->has_flags(CommandFlag::ADMIN_ONLY) && !authenticated) {
            LOG_WARNING(F("Command %s on %s requires valid authorization"), cmd, EMSdevice::device_type_2_device_name(device_type).c_str());
            return CommandRet::NOT_ALLOWED; // command not allowed
        }
    }

    std::string dname = EMSdevice::device_type_2_device_name(device_type);
    if (value == nullptr) {
        LOG_INFO(F("Calling %s command '%s'"), dname.c_str(), cmd);
    } else if (id == -1) {
        LOG_INFO(F("Calling %s command '%s', value %s, id is default"), dname.c_str(), cmd, value);
    } else {
        LOG_INFO(F("Calling %s command '%s', value %s, id is %d"), dname.c_str(), cmd, value, id_new);
    }

    // check if json object is empty, if so quit
    if (json.isNull()) {
        LOG_WARNING(F("Ignore call for command %s in %s because it has no json body"), cmd, EMSdevice::device_type_2_device_name(device_type).c_str());
        return CommandRet::ERROR;
    }

    // this is for endpoints that don't have commands, i.e not writable (e.g. boiler/syspress)
    if (cf == nullptr) {
        return EMSESP::get_device_value_info(json, cmd_new, id_new, device_type) ? CommandRet::OK : CommandRet::ERROR;
    }

    if (cf->cmdfunction_json_) {
        return ((cf->cmdfunction_json_)(value, id_new, json)) ? CommandRet::OK : CommandRet::ERROR;
    } else {
        if ((device_type != EMSdevice::DeviceType::SYSTEM) && (value == nullptr || strlen(value) == 0 || strcmp(value, "?") == 0 || strcmp(value, "*") == 0)) {
            return EMSESP::get_device_value_info(json, cmd_new, id_new, device_type) ? CommandRet::OK : CommandRet::ERROR;
        }
        return ((cf->cmdfunction_)(value, id_new)) ? CommandRet::OK : CommandRet::ERROR;
    }
}

// strip prefixes, check, and find command
Command::CmdFunction * Command::find_command(const uint8_t device_type, char * cmd, int8_t & id) {
    // special cases for id=0 and id=-1 will be removed in V3 API
    // no command for id0
    if (id == 0) {
        return nullptr;
    }

    // empty command is info with id0
    if (cmd[0] == '\0') {
        strcpy(cmd, "info");
        id = 0;
    }

    // convert cmd to lowercase
    for (char * p = cmd; *p; p++) {
        *p = tolower(*p);
    }

    // hack for commands that could have hc or wwc prefixed. will be removed in new API V3 eventually
    // scan for prefix hc.
    for (uint8_t i = DeviceValueTAG::TAG_HC1; i <= DeviceValueTAG::TAG_HC4; i++) {
        const char * tag = EMSdevice::tag_to_string(i).c_str();
        uint8_t      len = strlen(tag);
        if (strncmp(cmd, tag, len) == 0) {
            if (cmd[len] != '\0') {
                strcpy(cmd, &cmd[len + 1]);
            } else {
                strcpy(cmd, &cmd[len]);
            }
            id = 1 + i - DeviceValueTAG::TAG_HC1;
            break;
        }
    }

    // scan for prefix wwc.
    for (uint8_t i = DeviceValueTAG::TAG_WWC1; i <= DeviceValueTAG::TAG_WWC4; i++) {
        const char * tag = EMSdevice::tag_to_string(i).c_str();
        uint8_t      len = strlen(tag);
        if (strncmp(cmd, tag, len) == 0) {
            if (cmd[len] != '\0') {
                strcpy(cmd, &cmd[len + 1]);
            } else {
                strcpy(cmd, &cmd[len]);
            }
            id = 8 + i - DeviceValueTAG::TAG_WWC1;
            break;
        }
    }

    // empty command after processing prefix is info
    if (cmd[0] == '\0') {
        strcpy(cmd, "info");
    }

    return find_command(device_type, cmd);
}

// add a command to the list, which does not return json
// these commands are not callable directly via MQTT subscriptions either
void Command::add(const uint8_t device_type, const __FlashStringHelper * cmd, const cmd_function_p cb, const __FlashStringHelper * description, uint8_t flags) {
    // if the command already exists for that device type don't add it
    if (find_command(device_type, uuid::read_flash_string(cmd).c_str()) != nullptr) {
        return;
    }

    // if the description is empty, it's hidden which means it will not show up in Web API or Console as an available command
    if (description == nullptr) {
        flags |= CommandFlag::HIDDEN;
    }

    cmdfunctions_.emplace_back(device_type, flags, cmd, cb, nullptr, description); // callback for json is nullptr

    // see if we need to subscribe
    if (Mqtt::enabled()) {
        Mqtt::register_command(device_type, cmd, cb, flags);
    }
}

// add a command to the list, which does return a json object as output
// flag is fixed to MqttSubFlag::FLAG_NOSUB
void Command::add_json(const uint8_t               device_type,
                       const __FlashStringHelper * cmd,
                       const cmd_json_function_p   cb,
                       const __FlashStringHelper * description,
                       uint8_t                     flags) {
    // if the command already exists for that device type don't add it
    if (find_command(device_type, uuid::read_flash_string(cmd).c_str()) != nullptr) {
        return;
    }

    cmdfunctions_.emplace_back(device_type, CommandFlag::MQTT_SUB_FLAG_NOSUB | flags, cmd, nullptr, cb, description); // callback for json is included
}

// see if a command exists for that device type
// is not case sensitive
Command::CmdFunction * Command::find_command(const uint8_t device_type, const char * cmd) {
    if ((cmd == nullptr) || (strlen(cmd) == 0) || (cmdfunctions_.empty())) {
        return nullptr;
    }

    // convert cmd to lowercase and compare
    char lowerCmd[30];
    strlcpy(lowerCmd, cmd, sizeof(lowerCmd));
    for (char * p = lowerCmd; *p; p++) {
        *p = tolower(*p);
    }

    for (auto & cf : cmdfunctions_) {
        if (!strcmp(lowerCmd, Helpers::toLower(uuid::read_flash_string(cf.cmd_)).c_str()) && (cf.device_type_ == device_type)) {
            return &cf;
        }
    }

    return nullptr; // command not found
}

// list all commands for a specific device, output as json
bool Command::list(const uint8_t device_type, JsonObject & json) {
    if (cmdfunctions_.empty()) {
        json["message"] = "no commands available";
        return false;
    }

    // create a list of commands, sort them
    std::list<std::string> sorted_cmds;
    for (const auto & cf : cmdfunctions_) {
        if ((cf.device_type_ == device_type) && !cf.has_flags(CommandFlag::HIDDEN)) {
            sorted_cmds.push_back(uuid::read_flash_string(cf.cmd_));
        }
    }
    sorted_cmds.sort();

    for (auto & cl : sorted_cmds) {
        for (const auto & cf : cmdfunctions_) {
            if ((cf.device_type_ == device_type) && !cf.has_flags(CommandFlag::HIDDEN) && cf.description_ && (cl == uuid::read_flash_string(cf.cmd_))) {
                json[cl] = cf.description_;
            }
        }
    }

    return true;
}

// output list of all commands to console for a specific DeviceType
void Command::show(uuid::console::Shell & shell, uint8_t device_type, bool verbose) {
    if (cmdfunctions_.empty()) {
        shell.println(F("No commands available"));
        return;
    }

    // create a list of commands, sort them
    std::list<std::string> sorted_cmds;
    for (const auto & cf : cmdfunctions_) {
        if ((cf.device_type_ == device_type) && !cf.has_flags(CommandFlag::HIDDEN)) {
            sorted_cmds.push_back(uuid::read_flash_string(cf.cmd_));
        }
    }
    sorted_cmds.sort();

    // if not in verbose mode, just print them on a single line
    if (!verbose) {
        for (auto & cl : sorted_cmds) {
            shell.print(cl);
            shell.print(" ");
        }
        shell.println();
        return;
    }

    // verbose mode
    shell.println();
    for (auto & cl : sorted_cmds) {
        // find and print the description
        for (const auto & cf : cmdfunctions_) {
            if ((cf.device_type_ == device_type) && !cf.has_flags(CommandFlag::HIDDEN) && cf.description_ && (cl == uuid::read_flash_string(cf.cmd_))) {
                uint8_t i = cl.length();
                shell.print("  ");
                if (cf.has_flags(MQTT_SUB_FLAG_HC)) {
                    shell.print("[hc] ");
                    i += 5;
                } else if (cf.has_flags(MQTT_SUB_FLAG_WWC)) {
                    shell.print("[wwc] ");
                    i += 6;
                }
                shell.print(cl);
                // pad with spaces
                while (i++ < 22) {
                    shell.print(' ');
                }
                shell.print(COLOR_BRIGHT_CYAN);
                if (cf.has_flags(MQTT_SUB_FLAG_WW)) {
                    shell.print(EMSdevice::tag_to_string(TAG_DEVICE_DATA_WW));
                    shell.print(' ');
                }
                shell.print(uuid::read_flash_string(cf.description_));
                if (cf.has_flags(CommandFlag::ADMIN_ONLY)) {
                    shell.print(' ');
                    shell.print(COLOR_BRIGHT_RED);
                    shell.print('!');
                }
                shell.print(COLOR_RESET);
            }
        }
        shell.println();
    }

    shell.println();
}

// see if a device_type is active and has associated commands
// returns false if the device has no commands
bool Command::device_has_commands(const uint8_t device_type) {
    if (device_type == EMSdevice::DeviceType::UNKNOWN) {
        return false;
    }

    if (device_type == EMSdevice::DeviceType::SYSTEM) {
        return true; // we always have System
    }

    if (device_type == EMSdevice::DeviceType::DALLASSENSOR) {
        return true; // we always have Sensor, but should check if there are actual sensors attached!
    }

    for (const auto & emsdevice : EMSESP::emsdevices) {
        if ((emsdevice) && (emsdevice->device_type() == device_type)) {
            // device found, now see if it has any commands
            for (const auto & cf : cmdfunctions_) {
                if (cf.device_type_ == device_type) {
                    return true;
                }
            }
        }
    }

    return false;
}

void Command::show_devices(uuid::console::Shell & shell) {
    shell.printf("%s ", EMSdevice::device_type_2_device_name(EMSdevice::DeviceType::SYSTEM).c_str());

    if (EMSESP::have_sensors()) {
        shell.printf("%s ", EMSdevice::device_type_2_device_name(EMSdevice::DeviceType::DALLASSENSOR).c_str());
    }

    for (const auto & device_class : EMSFactory::device_handlers()) {
        for (const auto & emsdevice : EMSESP::emsdevices) {
            if ((emsdevice) && (emsdevice->device_type() == device_class.first) && (device_has_commands(device_class.first))) {
                shell.printf("%s ", EMSdevice::device_type_2_device_name(device_class.first).c_str());
                break; // we only want to show one (not multiple of the same device types)
            }
        }
    }
    shell.println();
}

// output list of all commands to console
// calls show with verbose mode set
void Command::show_all(uuid::console::Shell & shell) {
    shell.println(F("Available commands (!=requires authorization): "));

    // show system first
    shell.print(COLOR_BOLD_ON);
    shell.printf(" %s: ", EMSdevice::device_type_2_device_name(EMSdevice::DeviceType::SYSTEM).c_str());
    shell.print(COLOR_RESET);
    show(shell, EMSdevice::DeviceType::SYSTEM, true);

    // show sensor
    if (EMSESP::have_sensors()) {
        shell.print(COLOR_BOLD_ON);
        shell.printf(" %s: ", EMSdevice::device_type_2_device_name(EMSdevice::DeviceType::DALLASSENSOR).c_str());
        shell.print(COLOR_RESET);
        show(shell, EMSdevice::DeviceType::DALLASSENSOR, true);
    }

    // do this in the order of factory classes to keep a consistent order when displaying
    for (const auto & device_class : EMSFactory::device_handlers()) {
        if (Command::device_has_commands(device_class.first)) {
            shell.print(COLOR_BOLD_ON);
            shell.printf(" %s: ", EMSdevice::device_type_2_device_name(device_class.first).c_str());
            shell.print(COLOR_RESET);
            show(shell, device_class.first, true);
        }
    }
}

} // namespace emsesp
