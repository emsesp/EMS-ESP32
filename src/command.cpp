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
bool Command::call_command(const uint8_t device_type, const char * cmd, const char * value, const int8_t id) {
#ifdef EMSESP_DEBUG
    if (id == -1) {
        LOG_DEBUG(F("[DEBUG] Calling command %s, value %s, id is default"), cmd, value);
    } else {
        LOG_DEBUG(F("[DEBUG] Calling command %s, value %s, id is %d"), cmd, value, id);
    }
#endif

    if (!cmdfunctions_.empty()) {
        for (const auto & cf : cmdfunctions_) {
            if (cf.device_type_ == device_type) {
                const char * cf_cmd = uuid::read_flash_string(cf.cmd_).c_str();
                if (strcmp(cf_cmd, cmd) == 0) {
                    return ((cf.cmdfunction_)(value, id)); // call function, data needs to be a string and can be null
                }
            }
        }
    }
    return false;
}

// add a command to the list
void Command::add_command(const uint8_t device_type, const uint8_t device_id, const __FlashStringHelper * cmd, cmdfunction_p cb) {
    cmdfunctions_.emplace_back(device_type, device_id, cmd, cb);

    // see if we need to subscribe
    Mqtt::register_command(device_type, device_id, cmd, cb);
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
void Command::show_commands(uuid::console::Shell & shell, uint8_t device_type) {
    for (const auto & cf : Command::commands()) {
        if (cf.device_type_ == device_type) {
            shell.printf("%s ", uuid::read_flash_string(cf.cmd_).c_str());
        }
    }
    shell.println();
}

// determines the device_type from the shell context we're in
uint8_t Command::context_2_device_type(unsigned int context) {
    if (context == ShellContext::MAIN) {
        return EMSdevice::DeviceType::SERVICEKEY;
    }
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

    return EMSdevice::DeviceType::UNKNOWN; // unknown type
}

// show command per current context
void Command::show_commands(uuid::console::Shell & shell) {
    show_commands(shell, context_2_device_type(shell.context()));
}


// output list of all commands to console
void Command::show_all_commands(uuid::console::Shell & shell) {
    // show system first
    shell.printf("%s: ", EMSdevice::device_type_2_device_name(EMSdevice::DeviceType::SERVICEKEY).c_str());
    show_commands(shell, EMSdevice::DeviceType::SERVICEKEY);

    // do this in the order of factory classes to keep a consistent order when displaying
    for (const auto & device_class : EMSFactory::device_handlers()) {
        for (const auto & emsdevice : EMSESP::emsdevices) {
            if ((emsdevice) && (emsdevice->device_type() == device_class.first)) {
                shell.printf("%s: ", EMSdevice::device_type_2_device_name(device_class.first).c_str());
                show_commands(shell, device_class.first);
            }
        }
    }
}

// given a context, automatically add the commands to the console
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
                show_commands(shell);
                shell.println();
                return;
            }

            // determine the device_type from the shell context
            uint8_t device_type = context_2_device_type(shell.context());

            const char * cmd = arguments[0].c_str();
            if (arguments.size() == 1) {
                // no value specified
                (void)Command::call_command(device_type, cmd, nullptr, -1);
            } else if (arguments.size() == 2) {
                // has a value but no id
                (void)Command::call_command(device_type, cmd, arguments.back().c_str(), -1);
            } else {
                // use value, which could be an id or hc
                (void)Command::call_command(device_type, cmd, arguments[1].c_str(), atoi(arguments[2].c_str()));
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