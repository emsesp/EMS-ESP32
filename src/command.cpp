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

// takes a path and a json body, parses the data and calls the command
// the path is leading so if duplicate keys are in the input JSON it will be ignored
// the entry point will be either via the Web API (api/) or MQTT (<base>/)
// returns a return code and json output
uint8_t Command::process(const char * path, const bool is_admin, const JsonObject & input, JsonObject & output) {
    SUrlParser p; // parse URL for the path names
    p.parse(path);

    if (!p.paths().size()) {
        return message(CommandRet::ERROR, "invalid path", output);
    }

    // check first if it's from API, if so strip the "api/"
    if ((p.paths().front() == "api")) {
        p.paths().erase(p.paths().begin());
    } else {
        // not /api, so must be MQTT path. Check for base and remove it.
        if (!strncmp(path, Mqtt::base().c_str(), Mqtt::base().length())) {
            char new_path[Mqtt::MQTT_TOPIC_MAX_SIZE];
            strncpy(new_path, path, sizeof(new_path));
            p.parse(new_path + Mqtt::base().length() + 1); // re-parse the stripped path
        } else {
            return message(CommandRet::ERROR, "unrecognized path", output); // error
        }
    }

#if defined(EMSESP_USE_SERIAL)
    // Serial.println(p.path().c_str()); // dump paths, for debugging
#endif

    // re-calculate new path
    // if there is only a path (URL) and no body then error!
    size_t num_paths = p.paths().size();
    if (!num_paths && !input.size()) {
        return message(CommandRet::ERROR, "missing command in path", output);
    }

    std::string cmd_s;
    int8_t      id_n = -1; // default hc

    // check for a device as first item in the path
    // if its not a known device (thermostat, boiler etc) look for any special MQTT subscriptions
    const char * device_s = nullptr;
    if (!num_paths) {
        // we must look for the device in the JSON body
        if (input.containsKey("device")) {
            device_s = input["device"];
        }
    } else {
        // extract it from the path
        device_s = p.paths().front().c_str(); // get the device (boiler, thermostat, system etc)
    }

    // validate the device, make sure it exists
    uint8_t device_type = EMSdevice::device_name_2_device_type(device_s);
    if (!device_has_commands(device_type)) {
        LOG_DEBUG(F("Command failed: unknown device '%s'"), device_s);
        return message(CommandRet::ERROR, "unknown device", output);
    }

    // the next value on the path should be the command
    const char * command_p = nullptr;
    if (num_paths == 2) {
        command_p = p.paths()[1].c_str();
    } else if (num_paths >= 3) {
        // concatenate the path into one string as it could be in the format 'hc/XXX'
        char command[50];
        snprintf(command, sizeof(command), "%s/%s", p.paths()[1].c_str(), p.paths()[2].c_str());
        command_p = command;
    } else {
        // take it from the JSON
        if (input.containsKey("entity")) {
            command_p = input["entity"];
        } else if (input.containsKey("cmd")) {
            command_p = input["cmd"];
        }
    }

    // some commands may be prefixed with hc. or wwc. so extract these if they exist
    // parse_command_string returns the extracted command
    command_p = parse_command_string(command_p, id_n);
    if (command_p == nullptr) {
        // handle dead endpoints like api/system or api/boiler
        // default to 'info' for SYSTEM, DALLASENSOR and ANALOGSENSOR, the other devices to 'values' for shortname version
        if (num_paths < 3) {
            if (device_type < EMSdevice::DeviceType::BOILER) {
                command_p = "info";
            } else {
                command_p = "values";
            }
        } else {
            return message(CommandRet::NOT_FOUND, "missing or bad command", output);
        }
    }

    // if we don't have an id/hc/wwc try and get it from the JSON input
    // it's allowed to have no id, and then keep the default to -1
    if (id_n == -1) {
        if (input.containsKey("hc")) {
            id_n = input["hc"];
        } else if (input.containsKey("wwc")) {
            id_n = input["wwc"];
            id_n += 8; // wwc1 has id 9
        } else if (input.containsKey("id")) {
            id_n = input["id"];
        }
    }

    // the value must always come from the input JSON. It's allowed to be empty.
    JsonVariant data;
    if (input.containsKey("data")) {
        data = input["data"];
    } else if (input.containsKey("value")) {
        data = input["value"];
    }

    // call the command based on the type
    uint8_t return_code = CommandRet::ERROR;
    if (data.is<const char *>()) {
        return_code = Command::call(device_type, command_p, data.as<const char *>(), is_admin, id_n, output);
    } else if (data.is<int>()) {
        char data_str[10];
        return_code = Command::call(device_type, command_p, Helpers::itoa((int16_t)data.as<int>(), data_str), is_admin, id_n, output);
    } else if (data.is<float>()) {
        char data_str[10];
        return_code = Command::call(device_type, command_p, Helpers::render_value(data_str, (float)data.as<float>(), 2), is_admin, id_n, output);
    } else if (data.isNull()) {
        return_code = Command::call(device_type, command_p, "", is_admin, id_n, output); // empty, will do a query instead
    } else {
        return message(CommandRet::ERROR, "cannot parse command", output); // can't process
    }
    return return_code;
}

const std::string Command::return_code_string(const uint8_t return_code) {
    switch (return_code) {
    case CommandRet::ERROR:
        return read_flash_string(F("Error"));
        break;
    case CommandRet::OK:
        return read_flash_string(F("OK"));
        break;
    case CommandRet::NOT_FOUND:
        return read_flash_string(F("Not Found"));
        break;
    case CommandRet::NOT_ALLOWED:
        return read_flash_string(F("Not Authorized"));
        break;
    case CommandRet::FAIL:
        return read_flash_string(F("Failed"));
        break;
    }
    char s[4];
    return Helpers::smallitoa(s, return_code);
}

// takes a string like "hc1/seltemp" or "seltemp" or "wwc2.seltemp" and tries to get the id and cmd
// returns start position of the command string
const char * Command::parse_command_string(const char * command, int8_t & id) {
    if (command == nullptr) {
        return nullptr;
    }

    // make a copy of the string command for parsing
    char command_s[100];
    strncpy(command_s, command, sizeof(command_s));

    // look for a delimeter and split the string
    char * p      = command_s;
    char * breakp = strchr(p, '.');
    if (!breakp) {
        p      = command_s; // reset and look for /
        breakp = strchr(p, '/');
        if (!breakp) {
            p      = command_s; // reset and look for _
            breakp = strchr(p, '_');
            if (!breakp) {
                return command;
            }
        }
    }

    // extract the hc or wwc number
    uint8_t start_pos = breakp - p + 1;
    if (!strncmp(command, "hc", 2) && start_pos == 4) {
        id = command[start_pos - 2] - '0';
    } else if (!strncmp(command, "wwc", 3) && start_pos == 5) {
        id = command[start_pos - 2] - '0' + 8; // wwc1 has id 9
    } else {
#if defined(EMSESP_DEBUG)
        LOG_DEBUG(F("[DEBUG] Command parse error, unknown hc/wwc in %s"), command_s);
#endif
        return nullptr;
    }

    return (command + start_pos);
}

// calls a command directly
uint8_t Command::call(const uint8_t device_type, const char * cmd, const char * value) {
    // create a temporary buffer
    StaticJsonDocument<EMSESP_JSON_SIZE_SMALL> output_doc;
    JsonObject                                 output = output_doc.to<JsonObject>();

    // authenticated is always true and ID is the default value
    return call(device_type, cmd, value, true, -1, output);
}

// calls a command. Takes a json object for output.
// id may be used to represent a heating circuit for example
// returns 0 if the command errored, 1 (TRUE) if ok, 2 if not found, 3 if error or 4 if not allowed
uint8_t Command::call(const uint8_t device_type, const char * cmd, const char * value, const bool is_admin, const int8_t id, JsonObject & output) {
    uint8_t return_code = CommandRet::OK;

    std::string dname = EMSdevice::device_type_2_device_name(device_type);

    // see if there is a command registered
    auto cf = find_command(device_type, cmd);

    // check if its a call to and end-point to a device, i.e. has no value
    // except for system commands as this is a special device without any queryable entities (device values)
    // exclude SYSTEM
    if ((device_type > EMSdevice::DeviceType::SYSTEM) && (!value || !strlen(value))) {
        if (!cf || !cf->cmdfunction_json_) {
#if defined(EMSESP_DEBUG)
            LOG_DEBUG(F("[DEBUG] Calling %s command '%s' to retrieve values"), dname.c_str(), cmd);
#endif
            return EMSESP::get_device_value_info(output, cmd, id, device_type) ? CommandRet::OK : CommandRet::ERROR; // entity = cmd
        }
    }

    // check if we have a matching command
    if (cf) {
        // check permissions
        if (cf->has_flags(CommandFlag::ADMIN_ONLY) && !is_admin) {
            output["message"] = "authentication failed";
            return CommandRet::NOT_ALLOWED; // command not allowed
        }

        if (value == nullptr) {
            if (EMSESP::system_.readonly_mode()) {
                LOG_INFO(F("[readonly] Calling command '%s/%s' (%s)"), dname.c_str(), cmd, read_flash_string(cf->description_).c_str());
            } else {
                LOG_DEBUG(F("Calling command '%s/%s' (%s)"), dname.c_str(), cmd, read_flash_string(cf->description_).c_str());
            }
        } else {
            if (EMSESP::system_.readonly_mode()) {
                LOG_INFO(F("[readonly] Calling command '%s/%s' (%s) with value %s"), dname.c_str(), cmd, read_flash_string(cf->description_).c_str(), value);
            } else {
                LOG_DEBUG(F("Calling command '%s/%s' (%s) with value %s"), dname.c_str(), cmd, read_flash_string(cf->description_).c_str(), value);
            }
        }

        // call the function baesed on type
        if (cf->cmdfunction_json_) {
            return_code = ((cf->cmdfunction_json_)(value, id, output)) ? CommandRet::OK : CommandRet::ERROR;
        }

        if (cf->cmdfunction_) {
            return_code = ((cf->cmdfunction_)(value, id)) ? CommandRet::OK : CommandRet::ERROR;
        }

        // report back
        if (return_code != CommandRet::OK) {
            return message(return_code, "callback function failed", output);
        }

        return return_code;
    }

    // we didn't find the command and its not an endpoint, report error
    LOG_DEBUG(F("Command failed: invalid command '%s'"), cmd);
    return message(CommandRet::NOT_FOUND, "invalid command", output);
}

// add a command to the list, which does not return json
void Command::add(const uint8_t device_type, const __FlashStringHelper * cmd, const cmd_function_p cb, const __FlashStringHelper * description, uint8_t flags) {
    // if the command already exists for that device type don't add it
    if (find_command(device_type, read_flash_string(cmd).c_str()) != nullptr) {
        return;
    }

    // if the description is empty, it's hidden which means it will not show up in Web API or Console as an available command
    if (description == nullptr) {
        flags |= CommandFlag::HIDDEN;
    }

    cmdfunctions_.emplace_back(device_type, flags, cmd, cb, nullptr, description); // callback for json is nullptr
}

// add a command to the list, which does return a json object as output
// flag is fixed to MqttSubFlag::MQTT_SUB_FLAG_NOSUB so there will be no topic subscribed to this
void Command::add(const uint8_t device_type, const __FlashStringHelper * cmd, const cmd_json_function_p cb, const __FlashStringHelper * description, uint8_t flags) {
    // if the command already exists for that device type don't add it
    if (find_command(device_type, read_flash_string(cmd).c_str()) != nullptr) {
        return;
    }

    cmdfunctions_.emplace_back(device_type, (CommandFlag::MQTT_SUB_FLAG_NOSUB | flags), cmd, nullptr, cb, description); // callback for json is included
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
        if (!strcmp(lowerCmd, Helpers::toLower(read_flash_string(cf.cmd_)).c_str()) && (cf.device_type_ == device_type)) {
            return &cf;
        }
    }

    return nullptr; // command not found
}

// list all commands for a specific device, output as json
bool Command::list(const uint8_t device_type, JsonObject & output) {
    if (cmdfunctions_.empty()) {
        output["message"] = "no commands available";
        return false;
    }

    // create a list of commands, sort them
    std::list<std::string> sorted_cmds;
    for (const auto & cf : cmdfunctions_) {
        if ((cf.device_type_ == device_type) && !cf.has_flags(CommandFlag::HIDDEN)) {
            sorted_cmds.push_back(read_flash_string(cf.cmd_));
        }
    }
    sorted_cmds.sort();

    for (auto & cl : sorted_cmds) {
        for (const auto & cf : cmdfunctions_) {
            if ((cf.device_type_ == device_type) && !cf.has_flags(CommandFlag::HIDDEN) && cf.description_ && (cl == read_flash_string(cf.cmd_))) {
                output[cl] = cf.description_;
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
            sorted_cmds.push_back(read_flash_string(cf.cmd_));
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
            if ((cf.device_type_ == device_type) && !cf.has_flags(CommandFlag::HIDDEN) && cf.description_ && (cl == read_flash_string(cf.cmd_))) {
                uint8_t i = cl.length();
                shell.print("  ");
                if (cf.has_flags(MQTT_SUB_FLAG_HC)) {
                    shell.print("[hc<n>.]");
                    i += 8;
                } else if (cf.has_flags(MQTT_SUB_FLAG_WWC)) {
                    shell.print("[wwc<n>.]");
                    i += 9;
                }
                shell.print(cl);
                // pad with spaces
                while (i++ < 22) {
                    shell.print(' ');
                }
                shell.print(COLOR_BRIGHT_CYAN);
                if (cf.has_flags(MQTT_SUB_FLAG_WW)) {
                    shell.print(EMSdevice::tag_to_string(DeviceValueTAG::TAG_DEVICE_DATA_WW));
                    shell.print(' ');
                }
                shell.print(read_flash_string(cf.description_));
                if (!cf.has_flags(CommandFlag::ADMIN_ONLY)) {
                    shell.print(' ');
                    shell.print(COLOR_BRIGHT_RED);
                    shell.print('*');
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
        return (EMSESP::dallassensor_.have_sensors());
    }

    if (device_type == EMSdevice::DeviceType::ANALOGSENSOR) {
        return (EMSESP::analogsensor_.have_sensors());
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

// list sensors and EMS devices
void Command::show_devices(uuid::console::Shell & shell) {
    shell.printf("%s ", EMSdevice::device_type_2_device_name(EMSdevice::DeviceType::SYSTEM).c_str());

    if (EMSESP::dallassensor_.have_sensors()) {
        shell.printf("%s ", EMSdevice::device_type_2_device_name(EMSdevice::DeviceType::DALLASSENSOR).c_str());
    }
    if (EMSESP::analogsensor_.have_sensors()) {
        shell.printf("%s ", EMSdevice::device_type_2_device_name(EMSdevice::DeviceType::ANALOGSENSOR).c_str());
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
    shell.println(F("Available commands (*=do not need authorization): "));

    // show system first
    shell.print(COLOR_BOLD_ON);
    shell.print(COLOR_YELLOW);
    shell.printf(" %s: ", EMSdevice::device_type_2_device_name(EMSdevice::DeviceType::SYSTEM).c_str());
    shell.print(COLOR_RESET);
    show(shell, EMSdevice::DeviceType::SYSTEM, true);

    // show sensors
    if (EMSESP::dallassensor_.have_sensors()) {
        shell.print(COLOR_BOLD_ON);
        shell.print(COLOR_YELLOW);
        shell.printf(" %s: ", EMSdevice::device_type_2_device_name(EMSdevice::DeviceType::DALLASSENSOR).c_str());
        shell.print(COLOR_RESET);
        show(shell, EMSdevice::DeviceType::DALLASSENSOR, true);
    }
    if (EMSESP::analogsensor_.have_sensors()) {
        shell.print(COLOR_BOLD_ON);
        shell.print(COLOR_YELLOW);
        shell.printf(" %s: ", EMSdevice::device_type_2_device_name(EMSdevice::DeviceType::ANALOGSENSOR).c_str());
        shell.print(COLOR_RESET);
        show(shell, EMSdevice::DeviceType::ANALOGSENSOR, true);
    }

    // do this in the order of factory classes to keep a consistent order when displaying
    for (const auto & device_class : EMSFactory::device_handlers()) {
        if (Command::device_has_commands(device_class.first)) {
            shell.print(COLOR_BOLD_ON);
            shell.print(COLOR_YELLOW);
            shell.printf(" %s: ", EMSdevice::device_type_2_device_name(device_class.first).c_str());
            shell.print(COLOR_RESET);
            show(shell, device_class.first, true);
        }
    }
}

// Extract only the path component from the passed URI and normalized it
// e.g. //one/two////three/// becomes /one/two/three
std::string SUrlParser::path() {
    std::string s = "/"; // set up the beginning slash
    for (std::string & f : m_folders) {
        s += f;
        s += "/";
    }
    s.pop_back(); // deleting last letter, that is slash '/'
    return std::string(s);
}

SUrlParser::SUrlParser(const char * uri) {
    parse(uri);
}

bool SUrlParser::parse(const char * uri) {
    m_folders.clear();
    m_keysvalues.clear();
    enum Type { begin, folder, param, value };
    std::string s;

    const char * c = uri;
    enum Type    t = Type::begin;
    std::string  last_param;

    if (c != nullptr || *c != '\0') {
        do {
            if (*c == '/') {
                if (s.length() > 0) {
                    m_folders.push_back(s);
                    s.clear();
                }
                t = Type::folder;
            } else if (*c == '?' && (t == Type::folder || t == Type::begin)) {
                if (s.length() > 0) {
                    m_folders.push_back(s);
                    s.clear();
                }
                t = Type::param;
            } else if (*c == '=' && (t == Type::param || t == Type::begin)) {
                m_keysvalues[s] = "";
                last_param      = s;
                s.clear();
                t = Type::value;
            } else if (*c == '&' && (t == Type::value || t == Type::param || t == Type::begin)) {
                if (t == Type::value) {
                    m_keysvalues[last_param] = s;
                } else if ((t == Type::param || t == Type::begin) && (s.length() > 0)) {
                    m_keysvalues[s] = "";
                    last_param      = s;
                }
                t = Type::param;
                s.clear();
            } else if (*c == '\0' && s.length() > 0) {
                if (t == Type::value) {
                    m_keysvalues[last_param] = s;
                } else if (t == Type::folder || t == Type::begin) {
                    m_folders.push_back(s);
                } else if (t == Type::param) {
                    m_keysvalues[s] = "";
                    last_param      = s;
                }
                s.clear();
            } else if (*c == '\0' && s.length() == 0) {
                if (t == Type::param && last_param.length() > 0) {
                    m_keysvalues[last_param] = "";
                }
                s.clear();
            } else {
                s += *c;
            }
        } while (*c++ != '\0');
    }
    return true;
}

} // namespace emsesp
