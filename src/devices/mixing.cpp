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

#include "mixing.h"

namespace emsesp {

REGISTER_FACTORY(Mixing, EMSdevice::DeviceType::MIXING);

uuid::log::Logger Mixing::logger_{F_(mixing), uuid::log::Facility::CONSOLE};

Mixing::Mixing(uint8_t device_type, uint8_t device_id, uint8_t product_id, const std::string & version, const std::string & name, uint8_t flags, uint8_t brand)
    : EMSdevice(device_type, device_id, product_id, version, name, flags, brand) {
    LOG_DEBUG(F("Adding new Mixing module with device ID 0x%02X"), device_id);

    if (flags == EMSdevice::EMS_DEVICE_FLAG_MMPLUS) {
        if (device_id <= 0x27) {
            // telegram handlers 0x20 - 0x27 for HC
            register_telegram_type(device_id - 0x20 + 0x02D7, F("MMPLUSStatusMessage_HC"), true, [&](std::shared_ptr<const Telegram> t) {
                process_MMPLUSStatusMessage_HC(t);
            });
        } else {
            // telegram handlers for warm water/DHW 0x28, 0x29
            register_telegram_type(device_id - 0x28 + 0x0331, F("MMPLUSStatusMessage_WWC"), true, [&](std::shared_ptr<const Telegram> t) {
                process_MMPLUSStatusMessage_WWC(t);
            });
        }
    }

    // EMS 1.0
    if (flags == EMSdevice::EMS_DEVICE_FLAG_MM10) {
        register_telegram_type(0x00AA, F("MMConfigMessage"), false, [&](std::shared_ptr<const Telegram> t) { process_MMConfigMessage(t); });
        register_telegram_type(0x00AB, F("MMStatusMessage"), true, [&](std::shared_ptr<const Telegram> t) { process_MMStatusMessage(t); });
        register_telegram_type(0x00AC, F("MMSetMessage"), false, [&](std::shared_ptr<const Telegram> t) { process_MMSetMessage(t); });
    }

    // HT3
    if (flags == EMSdevice::EMS_DEVICE_FLAG_IPM) {
        register_telegram_type(0x010C, F("IPMSetMessage"), false, [&](std::shared_ptr<const Telegram> t) { process_IPMStatusMessage(t); });
    }

    // API call
    Command::add_with_json(this->device_type(), F("info"), [&](const char * value, const int8_t id, JsonObject & object) {
        return command_info(value, id, object);
    });
}

// add context submenu
void Mixing::add_context_menu() {
    // TODO support for multiple mixing units from a single menu, similar to set master with thermostat
    /*
    EMSESPShell::commands->add_command(ShellContext::MAIN,
                                       CommandFlags::USER,
                                       flash_string_vector{F_(mixing)},
                                       [&](Shell & shell, const std::vector<std::string> & arguments __attribute__((unused))) {
                                           Mixing::console_commands(shell, ShellContext::MIXING);
                                       });
                                       */
}

// output json to web UI
void Mixing::device_info_web(JsonArray & root) {
    if (type_ == Type::NONE) {
        return; // don't have any values yet
    }

    if (type_ == Type::WWC) {
        render_value_json(root, "", F("Warm Water Circuit"), hc_, nullptr);
        render_value_json(root, "", F("Current warm water temperature"), flowTemp_, F_(degrees), 10);
        render_value_json(root, "", F("Current pump status"), pump_, nullptr);
        render_value_json(root, "", F("Current temperature status"), status_, nullptr);
    } else {
        render_value_json(root, "", F("Heating Circuit"), hc_, nullptr);
        render_value_json(root, "", F("Current flow temperature"), flowTemp_, F_(degrees), 10);
        render_value_json(root, "", F("Setpoint flow temperature"), flowSetTemp_, F_(degrees));
        render_value_json(root, "", F("Current pump status"), pump_, nullptr, EMS_VALUE_BOOL);
        render_value_json(root, "", F("Current valve status"), status_, F_(percent));
    }
}

// check to see if values have been updated
bool Mixing::updated_values() {
    if (changed_) {
        changed_ = false;
        return true;
    }
    return false;
}

// add console commands
void Mixing::console_commands(Shell & shell, unsigned int context) {
    EMSESPShell::commands->add_command(ShellContext::MIXING,
                                       CommandFlags::ADMIN,
                                       flash_string_vector{F_(read)},
                                       flash_string_vector{F_(typeid_mandatory)},
                                       [=](Shell & shell __attribute__((unused)), const std::vector<std::string> & arguments) {
                                           uint16_t type_id = Helpers::hextoint(arguments.front().c_str());
                                           EMSESP::set_read_id(type_id);
                                           EMSESP::send_read_request(type_id, device_id());
                                       });

    EMSESPShell::commands->add_command(ShellContext::MIXING,
                                       CommandFlags::USER,
                                       flash_string_vector{F_(show)},
                                       [&](Shell & shell, const std::vector<std::string> & arguments __attribute__((unused))) { show_values(shell); });

    // enter the context
    Console::enter_custom_context(shell, context);
}

// display all values into the shell console
void Mixing::show_values(uuid::console::Shell & shell) {
    EMSdevice::show_values(shell); // always call this to show header

    if (type_ == Type::NONE) {
        return; // don't have any values yet
    }

    if (type_ == Type::WWC) {
        print_value(shell, 2, F("Warm Water Circuit"), hc_, nullptr);
        print_value(shell, 4, F("Current warm water temperature"), flowTemp_, F_(degrees), 10);
        print_value(shell, 4, F("Current pump status"), pump_, nullptr);
        print_value(shell, 4, F("Current temperature status"), status_, nullptr);
    } else {
        print_value(shell, 2, F("Heating Circuit"), hc_, nullptr);
        print_value(shell, 4, F("Current flow temperature"), flowTemp_, F_(degrees), 10);
        print_value(shell, 4, F("Setpoint flow temperature"), flowSetTemp_, F_(degrees));
        print_value(shell, 4, F("Current pump status"), pump_, nullptr, EMS_VALUE_BOOL);
        print_value(shell, 4, F("Current valve status"), status_, F_(percent));
    }


    shell.println();
}

bool Mixing::command_info(const char * value, const int8_t id, JsonObject & output) {
    return (export_values(output));
}

// publish values via MQTT
// ideally we should group up all the mixing units together into a nested JSON but for now we'll send them individually
void Mixing::publish_values() {
    StaticJsonDocument<EMSESP_MAX_JSON_SIZE_SMALL> doc;
    JsonObject                                     output = doc.to<JsonObject>();
    if (export_values(output)) {
        char topic[30];
        char s[5];
        strlcpy(topic, "mixing_data", 30);
        strlcat(topic, Helpers::itoa(s, device_id() - 0x20 + 1), 30); // append hc to topic
        Mqtt::publish(topic, doc.as<JsonObject>());
    }
}

// creates JSON doc from values
// returns false if empty
bool Mixing::export_values(JsonObject & output) {
    char s[5]; // for formatting strings

    switch (type_) {
    case Type::HC:
        output["type"] = "hc";
        if (Helpers::hasValue(flowTemp_)) {
            output["flowTemp"] = (float)flowTemp_ / 10;
        }
        if (Helpers::hasValue(flowSetTemp_)) {
            output["flowSetTemp"] = flowSetTemp_;
        }
        if (Helpers::hasValue(pump_)) {
            output["pumpStatus"] = Helpers::render_value(s, pump_, EMS_VALUE_BOOL);
        }
        if (Helpers::hasValue(status_)) {
            output["valveStatus"] = status_;
        }
        break;

    case Type::WWC:
        output["type"] = "wwc";
        if (Helpers::hasValue(flowTemp_)) {
            output["wwTemp"] = (float)flowTemp_ / 10;
        }
        if (Helpers::hasValue(pump_)) {
            output["pumpStatus"] = pump_;
        }
        if (Helpers::hasValue(status_)) {
            output["tempStatus"] = status_;
        }
        break;

    case Type::NONE:
    default:
        return false;
        break;
    }

    return output.size();
}

// heating circuits 0x02D7, 0x02D8 etc...
// e.g.  A0 00 FF 00 01 D7 00 00 00 80 00 00 00 00 03 C5
//       A0 0B FF 00 01 D7 00 00 00 80 00 00 00 00 03 80
void Mixing::process_MMPLUSStatusMessage_HC(std::shared_ptr<const Telegram> telegram) {
    type_ = Type::HC;
    hc_   = telegram->type_id - 0x02D7 + 1;         // determine which circuit this is
    changed_ |= telegram->read_value(flowTemp_, 3); // is * 10
    changed_ |= telegram->read_value(flowSetTemp_, 5);
    changed_ |= telegram->read_value(pump_, 0);
    changed_ |= telegram->read_value(status_, 2); // valve status
}

// Mixing module warm water loading/DHW - 0x0331, 0x0332
// e.g. A9 00 FF 00 02 32 02 6C 00 3C 00 3C 3C 46 02 03 03 00 3C // on 0x28
//      A8 00 FF 00 02 31 02 35 00 3C 00 3C 3C 46 02 03 03 00 3C // in 0x29
void Mixing::process_MMPLUSStatusMessage_WWC(std::shared_ptr<const Telegram> telegram) {
    type_ = Type::WWC;
    hc_   = telegram->type_id - 0x0331 + 1;         // determine which circuit this is. There are max 2.
    changed_ |= telegram->read_value(flowTemp_, 0); // is * 10
    changed_ |= telegram->read_value(pump_, 2);
    changed_ |= telegram->read_value(status_, 11); // temp status
}

// Mixing IMP - 0x010C
// e.g.  A0 00 FF 00 00 0C 01 00 00 00 00 00 54
//       A1 00 FF 00 00 0C 02 04 00 01 1D 00 82
void Mixing::process_IPMStatusMessage(std::shared_ptr<const Telegram> telegram) {
    type_           = Type::HC;
    hc_             = device_id() - 0x20 + 1;
    uint8_t ismixed = 0;
    changed_ |= telegram->read_value(ismixed, 0); // check if circuit is active, 0-off, 1-unmixed, 2-mixed
    if (ismixed == 0) {
        return;
    }
    if (ismixed == 2) {                                 // we have a mixed circuit
        changed_ |= telegram->read_value(flowTemp_, 3); // is * 10
        changed_ |= telegram->read_value(flowSetTemp_, 5);
        changed_ |= telegram->read_value(status_, 2); // valve status
    }
    changed_ |= telegram->read_bitvalue(pump_, 1, 0); // pump is also in unmixed circuits
}

// Mixing on a MM10 - 0xAB
// e.g. Mixing Module -> All, type 0xAB, telegram: 21 00 AB 00 2D 01 BE 64 04 01 00 (CRC=15) #data=7
// see also https://github.com/proddy/EMS-ESP/issues/386
void Mixing::process_MMStatusMessage(std::shared_ptr<const Telegram> telegram) {
    type_ = Type::HC;

    // the heating circuit is determine by which device_id it is, 0x20 - 0x23
    // 0x21 is position 2. 0x20 is typically reserved for the WM10 switch module
    // see https://github.com/proddy/EMS-ESP/issues/270 and https://github.com/proddy/EMS-ESP/issues/386#issuecomment-629610918
    hc_ = device_id() - 0x20 + 1;
    changed_ |= telegram->read_value(flowTemp_, 1); // is * 10
    changed_ |= telegram->read_value(pump_, 3);
    changed_ |= telegram->read_value(flowSetTemp_, 0);
    changed_ |= telegram->read_value(status_, 4); // valve status -100 to 100
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

// Mixing on a MM10 - 0xAA
// e.g. Thermostat -> Mixing Module, type 0xAA, telegram: 10 21 AA 00 FF 0C 0A 11 0A 32 xx
void Mixing::process_MMConfigMessage(std::shared_ptr<const Telegram> telegram) {
    hc_ = device_id() - 0x20 + 1;
    // pos 0: active FF = on
    // pos 1: valve runtime 0C = 120 sec in units of 10 sec
}

// Mixing on a MM10 - 0xAC
// e.g. Thermostat -> Mixing Module, type 0xAC, telegram: 10 21 AC 00 1E 64 01 AB
void Mixing::process_MMSetMessage(std::shared_ptr<const Telegram> telegram) {
    hc_ = device_id() - 0x20 + 1;
    // pos 0: flowtemp setpoint 1E = 30°C
    // pos 1: position in %
}

#pragma GCC diagnostic pop

} // namespace emsesp