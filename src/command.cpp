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
// returns false if error or not found
bool Command::call(const uint8_t device_type, const char * cmd, const char * value, const int8_t id) {
    std::string dname  = EMSdevice::device_type_2_device_name(device_type);
    int8_t      id_new = id;
    char        cmd_new[20];

    check_command(cmd_new, cmd, id_new);
    auto cf = find_command(device_type, cmd_new);
    if ((cf == nullptr) || (cf->cmdfunction_json_)) {
        LOG_WARNING(F("Command %s on %s not found"), cmd, dname.c_str());
        return false; // command not found, or requires a json
    }

#ifdef EMSESP_DEBUG
    if (value == nullptr) {
        LOG_DEBUG(F("[DEBUG] Calling %s command %s"), dname.c_str(), cmd);
    } else if (id == -1) {
        LOG_DEBUG(F("[DEBUG] Calling %s command %s, value %s, id is default"), dname.c_str(), cmd, value);
    } else {
        LOG_DEBUG(F("[DEBUG] Calling %s command %s, value %s, id is %d"), dname.c_str(), cmd, value, id);
    }
#endif

    return ((cf->cmdfunction_)(value, id_new));
}

// calls a command. Takes a json object for output.
// id may be used to represent a heating circuit for example
// returns false if error or not found
bool Command::call(const uint8_t device_type, const char * cmd, const char * value, const int8_t id, JsonObject & json) {
    int8_t id_new = id;
    char   cmd_new[20];

    check_command(cmd_new, cmd, id_new);
    auto cf = find_command(device_type, cmd_new);

#ifdef EMSESP_DEBUG
    std::string dname = EMSdevice::device_type_2_device_name(device_type);
    if (value == nullptr) {
        LOG_DEBUG(F("[DEBUG] Calling %s command %s"), dname.c_str(), cmd);
    } else if (id == -1) {
        LOG_DEBUG(F("[DEBUG] Calling %s command %s, value %s, id is default"), dname.c_str(), cmd, value);
    } else {
        LOG_DEBUG(F("[DEBUG] Calling %s command %s, value %s, id is %d"), dname.c_str(), cmd, value, id);
    }
#endif

    // check if json object is empty, if so quit
    if (json.isNull()) {
        LOG_WARNING(F("Ignore call for command %s in %s because no json"), cmd, EMSdevice::device_type_2_device_name(device_type).c_str());
        return false;
    }

    if (cf == nullptr) {
        return EMSESP::get_device_value_info(json, cmd_new, id_new, device_type);
    }

    if (cf->cmdfunction_json_) {
        return ((cf->cmdfunction_json_)(value, id_new, json));
    } else {
        if (value == nullptr || strlen(value) == 0 || strcmp(value, "?") == 0 || strcmp(value, "*") == 0) {
            return EMSESP::get_device_value_info(json, cmd_new, id_new, device_type);
        }
        return ((cf->cmdfunction_)(value, id_new));
    }
}

// set the id if there are prefixes
char * Command::check_command(char * out, const char * cmd, int8_t & id) {
    // convert cmd to lowercase
    strlcpy(out, cmd, 20);
    for (char * p = out; *p; p++) {
        *p = tolower(*p);
    }

    // scan for prefix hc.
    for (uint8_t i = DeviceValueTAG::TAG_HC1; i <= DeviceValueTAG::TAG_HC4; i++) {
        if ((strncmp(out, EMSdevice::tag_to_string(i).c_str(), 3) == 0) && (strlen(out) > 4)) {
            strcpy(out, &out[4]);
            id = 1 + i - DeviceValueTAG::TAG_HC1;
            break;
        }
    }

    // scan for prefix wwc.
    for (uint8_t i = DeviceValueTAG::TAG_WWC1; i <= DeviceValueTAG::TAG_WWC4; i++) {
        if ((strncmp(out, EMSdevice::tag_to_string(i).c_str(), 4) == 0) && (strlen(out) > 5)) {
            strcpy(out, &out[5]);
            id = 8 + i - DeviceValueTAG::TAG_WWC1;
            break;
        }
    }

    return out;
}

// add a command to the list, which does not return json
void Command::add(const uint8_t device_type, const __FlashStringHelper * cmd, cmdfunction_p cb, uint8_t flag) {
    // if the command already exists for that device type don't add it
    if (find_command(device_type, uuid::read_flash_string(cmd).c_str()) != nullptr) {
        return;
    }

    cmdfunctions_.emplace_back(device_type, flag, cmd, cb, nullptr);

    // see if we need to subscribe
    if (Mqtt::enabled()) {
        Mqtt::register_command(device_type, cmd, cb, flag);
    }
}

// add a command to the list, which does return json object as output
// optional parameter hidden for commands that will not show up on the Console
void Command::add_with_json(const uint8_t device_type, const __FlashStringHelper * cmd, cmdfunction_json_p cb, bool hidden) {
    // if the command already exists for that device type don't add it
    if (find_command(device_type, uuid::read_flash_string(cmd).c_str()) != nullptr) {
        return;
    }

    cmdfunctions_.emplace_back(device_type, MqttSubFlag::FLAG_NOSUB, cmd, nullptr, cb, hidden); // add command
}

// see if a command exists for that device type
// is not case sensitive
Command::CmdFunction * Command::find_command(const uint8_t device_type, const char * cmd) {
    if ((cmd == nullptr) || (strlen(cmd) == 0) || (cmdfunctions_.empty())) {
        return nullptr;
    }

    // convert cmd to lowercase and compare
    char lowerCmd[20];
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

// output list of all commands to console for a specific DeviceType
void Command::show(uuid::console::Shell & shell, uint8_t device_type) {
    if (cmdfunctions_.empty()) {
        shell.println(F("No commands available"));
    }

    // create a list of commands, sort them, print them
    std::list<std::string> sorted_cmds;
    for (const auto & cf : cmdfunctions_) {
        if ((cf.device_type_ == device_type) && !cf.hidden_) {
            sorted_cmds.push_back(uuid::read_flash_string(cf.cmd_));
        }
    }

    sorted_cmds.sort();
    for (auto & cl : sorted_cmds) {
        shell.print(cl);
        shell.print(" ");
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
void Command::show_all(uuid::console::Shell & shell) {
    shell.println(F("Available commands per device: "));

    // show system first
    shell.printf(" %s: ", EMSdevice::device_type_2_device_name(EMSdevice::DeviceType::SYSTEM).c_str());
    show(shell, EMSdevice::DeviceType::SYSTEM);

    // show sensor
    if (EMSESP::have_sensors()) {
        shell.printf(" %s: ", EMSdevice::device_type_2_device_name(EMSdevice::DeviceType::DALLASSENSOR).c_str());
        show(shell, EMSdevice::DeviceType::DALLASSENSOR);
    }

    // do this in the order of factory classes to keep a consistent order when displaying
    for (const auto & device_class : EMSFactory::device_handlers()) {
        if (Command::device_has_commands(device_class.first)) {
            shell.printf(" %s: ", EMSdevice::device_type_2_device_name(device_class.first).c_str());
            show(shell, device_class.first);
        }
    }
}

} // namespace emsesp
