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

#include "command.h"
#include "emsdevice.h"
#include "emsesp.h"

namespace emsesp {

uuid::log::Logger Command::logger_{F_(command), uuid::log::Facility::DAEMON};

std::vector<Command::CmdFunction> Command::cmdfunctions_;

// calls a command, context is the device_type
// id may be used to represent a heating circuit for example
// returns false if error or not found
bool Command::call(const uint8_t device_type, const char * cmd, const char * value, const int8_t id, JsonObject & json) {
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
    bool ok = false;
    if (!cmdfunctions_.empty()) {
        for (const auto & cf : cmdfunctions_) {
            if (cf.device_type_ == device_type) {
                // find a matching command and call it
                if (uuid::read_flash_string(cf.cmd_) == cmd) {
                    if (cf.cmdfunction_json_) {
                        // check if json object is empty, if so quit
                        if (json.isNull()) {
                            LOG_WARNING(F("Ignore call for command %s in %s because no json"), cmd, EMSdevice::device_type_2_device_name(device_type).c_str());
                            return false;
                        }
                        ok |= ((cf.cmdfunction_json_)(value, id, json));
                    } else {
                        ok |= ((cf.cmdfunction_)(value, id));
                    }
                }
            }
        }
    }
    return ok;
}

// add a command to the list, which does not return json
void Command::add(const uint8_t device_type, const uint8_t device_id, const __FlashStringHelper * cmd, cmdfunction_p cb) {
    cmdfunctions_.emplace_back(device_type, cmd, cb, nullptr);

    // see if we need to subscribe
    Mqtt::register_command(device_type, device_id, cmd, cb);
}

// add a command to the list, which does return json object as output
void Command::add_with_json(const uint8_t device_type, const __FlashStringHelper * cmd, cmdfunction_json_p cb) {
    // if the command already exists for that device type don't add it
    if (!find_command(device_type, uuid::read_flash_string(cmd).c_str())) {
        cmdfunctions_.emplace_back(device_type, cmd, nullptr, cb);
    }
}

// see if a command exists for that device type
bool Command::find_command(const uint8_t device_type, const char * cmd) {
    if ((cmd == nullptr) || (strlen(cmd) == 0)) {
        return false;
    }

    for (const auto & cf : cmdfunctions_) {
        if (!strcmp_P(cmd, reinterpret_cast<PGM_P>(cf.cmd_)) && (cf.device_type_ == device_type)) {
            return true;
        }
    }

    return false; // not found
}

// output list of all commands to console for a specific DeviceType
void Command::show(uuid::console::Shell & shell, uint8_t device_type) {
    if (commands().empty()) {
        shell.println(F("No commands"));
    }

    for (const auto & cf : Command::commands()) {
        if (cf.device_type_ == device_type) {
            shell.printf("%s ", uuid::read_flash_string(cf.cmd_).c_str());
        }
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
            for (const auto & cf : Command::commands()) {
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