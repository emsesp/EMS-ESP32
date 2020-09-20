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

#include "command.h"
#include "emsdevice.h"
#include "emsesp.h"

namespace emsesp {

uuid::log::Logger Command::logger_{F_(command), uuid::log::Facility::DAEMON};

std::vector<Command::CmdFunction> Command::cmdfunctions_;

// calls a command, context is the device_type
// id may be used to represent a heating circuit for example
// returns false if error or not found
bool Command::call(const uint8_t device_type, const char * cmd, const char * value, const int8_t id, JsonObject & output) {
#ifdef EMSESP_DEBUG
    std::string dname = EMSdevice::device_type_2_device_name(device_type);
    if (value == nullptr) {
        LOG_DEBUG(F("[DEBUG] Calling command %s in %s"), cmd, dname.c_str());
    } else if (id == -1) {
        LOG_DEBUG(F("[DEBUG] Calling command %s, value %s, id is default in %s"), cmd, value, dname.c_str());
    } else {
        LOG_DEBUG(F("[DEBUG] Calling command %s, value %s, id is %d in %s"), cmd, value, id, dname.c_str());
    }
#endif

    if (!cmdfunctions_.empty()) {
        for (const auto & cf : cmdfunctions_) {
            if (cf.device_type_ == device_type) {
                const char * cf_cmd = uuid::read_flash_string(cf.cmd_).c_str();
                // find a matching command and call it
                if (strcmp(cf_cmd, cmd) == 0) {
                    if (cf.cmdfunction_json_) {
                        // check if json object is empty, if so quit
                        if (output.isNull()) {
                            LOG_WARNING(F("Ignore call for command %s in %s because no json"), cmd, EMSdevice::device_type_2_device_name(device_type).c_str());
                        }
                        return ((cf.cmdfunction_json_)(value, id, output));
                    } else {
                        return ((cf.cmdfunction_)(value, id));
                    }
                }
            }
        }
    }
    return false; // command not found
}

// add a command to the list, which does not return json
void Command::add(const uint8_t device_type, const uint8_t device_id, const __FlashStringHelper * cmd, cmdfunction_p cb) {
    cmdfunctions_.emplace_back(device_type, cmd, cb, nullptr);

    // see if we need to subscribe
    Mqtt::register_command(device_type, device_id, cmd, cb);
}

// add a command to the list, which does return json object as output
void Command::add_with_json(const uint8_t device_type, const __FlashStringHelper * cmd, cmdfunction_json_p cb) {
    cmdfunctions_.emplace_back(device_type, cmd, nullptr, cb);
}

// see if a command exists for that device type
bool Command::find(const uint8_t device_type, const char * cmd) {
    for (const auto & cf : cmdfunctions_) {
        if (strcmp(cmd, uuid::read_flash_string(cf.cmd_).c_str()) == 0) {
            return true;
        }
    }
    return false; // not found
}

// output list of all commands to console for a specific DeviceType
void Command::show(uuid::console::Shell & shell, uint8_t device_type) {
    for (const auto & cf : Command::commands()) {
        if (cf.device_type_ == device_type) {
            shell.printf("%s ", uuid::read_flash_string(cf.cmd_).c_str());
        }
    }
    shell.println();
}

// determines the device_type from the shell context we're in
uint8_t Command::context_2_device_type(unsigned int context) {
    // if (context == ShellContext::MAIN) {
    //     return EMSdevice::DeviceType::SERVICEKEY;
    // }
    if (context == ShellContext::BOILER) {
        return EMSdevice::DeviceType::BOILER;
    }
    if (context == ShellContext::MIXING) {
        return EMSdevice::DeviceType::MIXING;
    }
    if (context == ShellContext::SOLAR) {
        return EMSdevice::DeviceType::SOLAR;
    }
    if (context == ShellContext::SYSTEM) {
        return EMSdevice::DeviceType::SERVICEKEY;
    }
    if (context == ShellContext::THERMOSTAT) {
        return EMSdevice::DeviceType::THERMOSTAT;
    }
    if (context == ShellContext::SENSOR) {
        return EMSdevice::DeviceType::SENSOR;
    }

    return EMSdevice::DeviceType::UNKNOWN; // unknown type
}

// show command per current context
void Command::show(uuid::console::Shell & shell) {
    show(shell, context_2_device_type(shell.context()));
}

// output list of all commands to console
void Command::show_all(uuid::console::Shell & shell) {
    // show system first
    shell.printf("%s: ", EMSdevice::device_type_2_device_name(EMSdevice::DeviceType::SERVICEKEY).c_str());
    show(shell, EMSdevice::DeviceType::SERVICEKEY);

    // do this in the order of factory classes to keep a consistent order when displaying
    for (const auto & device_class : EMSFactory::device_handlers()) {
        for (const auto & emsdevice : EMSESP::emsdevices) {
            if ((emsdevice) && (emsdevice->device_type() == device_class.first)) {
                shell.printf("%s: ", EMSdevice::device_type_2_device_name(device_class.first).c_str());
                show(shell, device_class.first);
            }
        }
    }
}

// Add the console 'call' command to the given context
void Command::add_context_commands(unsigned int context) {
    // if we're adding commands for a thermostat or mixing, then include an additional optional paramter called heating circuit
    flash_string_vector params;
    if ((context == ShellContext::THERMOSTAT) || (context == ShellContext::MIXING)) {
        params = flash_string_vector{F_(cmd_optional), F_(data_optional), F_(hc_optional)};
    } else if ((context == ShellContext::MAIN) || (context == ShellContext::SYSTEM)) {
        params = flash_string_vector{F_(cmd_optional), F_(data_optional), F_(n_optional)};
    } else {
        params = flash_string_vector{F_(cmd_optional), F_(data_optional)};
    }

    EMSESPShell::commands->add_command(
        context,
        CommandFlags::ADMIN,
        flash_string_vector{F_(call)},
        params,
        [&](Shell & shell, const std::vector<std::string> & arguments) {
            if (arguments.empty()) {
                // list options
                shell.print("Available commands: ");
                show(shell);
                shell.println();
                return;
            }

            // determine the device_type from the shell context
            uint8_t device_type = context_2_device_type(shell.context());

            // validate the command
            const char * cmd = arguments[0].c_str();
            if (!find(device_type, cmd)) {
                shell.print(F("Unknown command. Available commands are: "));
                show(shell);
                shell.println();
                return;
            }

            DynamicJsonDocument doc(EMSESP_MAX_JSON_SIZE_LARGE);
            JsonObject          output = doc.to<JsonObject>();

            bool ok = false;
            if (arguments.size() == 1) {
                // no value specified, just the cmd
                ok = Command::call(device_type, cmd, nullptr, -1, output);
            } else if (arguments.size() == 2) {
                // has a value but no id
                ok = Command::call(device_type, cmd, arguments.back().c_str(), -1, output);
            } else {
                // use value, which could be an id or hc
                ok = Command::call(device_type, cmd, arguments[1].c_str(), atoi(arguments[2].c_str()), output);
            }

            if (ok) {
                shell.print(F("output: "));
                serializeJson(doc, shell);
                shell.println();
                shell.println();
            }
        },
        [&](Shell & shell __attribute__((unused)), const std::vector<std::string> & arguments) -> std::vector<std::string> {
            if (arguments.size() > 0) {
                return {};
            }
            std::vector<std::string> commands;
            uint8_t                  device_type = context_2_device_type(shell.context());
            for (const auto & cf : Command::commands()) {
                if (cf.device_type_ == device_type) {
                    commands.emplace_back(uuid::read_flash_string(cf.cmd_));
                }
            }
            return commands;
        });
}

} // namespace emsesp