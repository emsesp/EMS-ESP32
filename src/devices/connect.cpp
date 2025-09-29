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

#include "connect.h"

namespace emsesp {

REGISTER_FACTORY(Connect, EMSdevice::DeviceType::CONNECT);

Connect::Connect(uint8_t device_type, uint8_t device_id, uint8_t product_id, const char * version, const char * name, uint8_t flags, uint8_t brand)
    : EMSdevice(device_type, device_id, product_id, version, name, flags, brand) {
    if (device_id == 0x50) { // RF Base
        register_telegram_type(0xD1, "RFOutdoorTemp", false, MAKE_PF_CB(process_OutdoorTemp));
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &outdoorTemp_,
                              DeviceValueType::INT16,
                              DeviceValueNumOp::DV_NUMOP_DIV10,
                              FL_(outdoorTemp),
                              DeviceValueUOM::DEGREES);
        // Roomthermostats
        for (uint8_t i = 0; i < 16; i++) {
            register_telegram_type(0x0BDD + i, "Room", false, MAKE_PF_CB(process_roomThermostat));
            register_telegram_type(0x0B3D + i, "Roomname", true, MAKE_PF_CB(process_roomThermostatName));
            register_telegram_type(0x0BB5 + i, "Roomsettings", true, MAKE_PF_CB(process_roomThermostatMode));

            // 1230..123F read,  unknown, 16 bytes long?
            // register_telegram_type(0x1230 + i, "unknown", true, MAKE_PF_CB(process_unknown));
            // 1244..1253 broadcast, unknown, 14 bytes long
        }
        // register_telegram_type(0xDB65, "Roomschedule", true, MAKE_PF_CB(process_roomSchedule));
        // 0x2040, broadcast 36 bytes:
        // data: 0E 60 00 DF 0D AF 0A 46 0A 46 02 9A 1C 53 1C 53 12 AD 12 AD 00 00 13 C2
        // data: 1F 37 1F 37 00 00 00 00 18 97 11 27 (offset 24)
    }
}
/*
 * OutdoorTemp - type 0xD1 - external temperature
 */
void Connect::process_OutdoorTemp(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, outdoorTemp_, 0);
}

/* other values from 0x50 RF base
(0x087F), data: 00 00
(0x0880), data: 01 04
(0x0889), data: 00 80 80 01
*/
void Connect::register_device_values_room(std::shared_ptr<Connect::RoomCircuit> room) {
    auto tag = DeviceValueTAG::TAG_HS1 + room->room();
    register_device_value(tag, &room->temp_, DeviceValueType::INT16, DeviceValueNumOp::DV_NUMOP_DIV10, FL_(roomTemp), DeviceValueUOM::DEGREES);
    register_device_value(tag, &room->humidity_, DeviceValueType::UINT8, FL_(airHumidity), DeviceValueUOM::PERCENT);
    register_device_value(tag, &room->seltemp_, DeviceValueType::UINT8, DeviceValueNumOp::DV_NUMOP_DIV2, FL_(seltemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_seltemp));
    register_device_value(tag, &room->mode_, DeviceValueType::ENUM, FL_(enum_mode2), FL_(mode), DeviceValueUOM::NONE, MAKE_CF_CB(set_mode));
    register_device_value(tag, &room->name_, DeviceValueType::STRING, FL_(name), DeviceValueUOM::NONE, MAKE_CF_CB(set_name));
}

std::shared_ptr<Connect::RoomCircuit> Connect::room_circuit(const uint8_t num, const bool create) {
    // check for existing circuit
    for (const auto & room_circuit : room_circuits_) {
        if (room_circuit->room() == num) {
            return room_circuit;
        }
    }
    if (!create) {
        return nullptr;
    }
    // create a new circuit object and add to the list
    auto new_room = std::make_shared<Connect::RoomCircuit>(num);
    room_circuits_.push_back(new_room);
    // register the device values
    register_device_values_room(new_room);

    return new_room; // return back point to new HC object
}

// gateway(0x50) B all(0x00), ?(0x0BDD), data: 00 E6 36 2A
void Connect::process_roomThermostat(std::shared_ptr<const Telegram> telegram) {
    auto    rc       = room_circuit(telegram->type_id - 0xBDD, true);
    uint8_t humidity = EMS_VALUE_UINT8_NOTSET;
    uint8_t seltemp  = EMS_VALUE_UINT8_NOTSET;
    has_update(telegram, rc->temp_, 0);
    if (Helpers::hasValue(rc->temp_)) {
        telegram->read_value(humidity, 2);
        telegram->read_value(seltemp, 3);
    }
    has_update(rc->humidity_, humidity);
    has_update(rc->seltemp_, seltemp);
}

// gateway(0x48) W gateway(0x50), ?(0x0B42), data: 01
// gateway(0x48) W gateway(0x50), ?(0x0B42), data: 00 4B 00 FC 00 63 00 68 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 (offset 1)
void Connect::process_roomThermostatName(std::shared_ptr<const Telegram> telegram) {
    auto rc = room_circuit(telegram->type_id - 0xB3D);
    if (rc == nullptr) {
        return;
    }
    if (telegram->offset > 1 || telegram->message_length < 3) {
        return;
    }
    std::string s;
    char        c;
    uint8_t     i = 3 - telegram->offset;
    while ((i < telegram->message_length) && ((c = telegram->message_data[i]) != 0)) {
        s += c;
        i += 2;
    }
    if (s.length()) {
        has_update(rc->name_, s.c_str(), s.length());
    }
}

// settings 0-mode, 1-tempautotemp, 3 - manualtemp, 6,7 - ?
void Connect::process_roomThermostatMode(std::shared_ptr<const Telegram> telegram) {
    auto rc = room_circuit(telegram->type_id - 0xBB5);
    if (rc == nullptr) {
        return;
    }
    has_enumupdate(telegram, rc->mode_, 0, {3, 1, 0});
}

// Settings:

bool Connect::set_mode(const char * value, const int8_t id) {
    auto rc = room_circuit(id - DeviceValueTAG::TAG_HS1);
    if (rc == nullptr) {
        return false;
    }
    uint8_t v;
    if (Helpers::value2enum(value, v, FL_(enum_mode2), {3, 1, 0})) {
        write_command(0xBB5 + rc->room(), 0, v); // no validate, mode change is broadcasted
        return true;
    }
    return false;
}

bool Connect::set_seltemp(const char * value, const int8_t id) {
    auto rc = room_circuit(id - DeviceValueTAG::TAG_HS1);
    if (rc == nullptr) {
        return false;
    }
    float v;
    if (Helpers::value2float(value, v)) {
        write_command(0xBB5 + rc->room(), rc->mode_ == 2 ? 1 : 3, v * 2);
        return true;
    }
    return false;
}

bool Connect::set_name(const char * value, const int8_t id) {
    auto rc = room_circuit(id - DeviceValueTAG::TAG_HS1);
    if (rc == nullptr || value == nullptr || strlen(value) > 12) {
        return false;
    }
    uint8_t      data[strlen(value) * 2];    
    uint8_t *    d = data;
    const char * c = value;
    while (*c != 0) {
        *d++ = 0;
        *d++ = *c++;
    }
    write_command(0x0B3D + rc->room(), 1, data, sizeof(data), 0x0B3D + rc->room());
    return true;
}


} // namespace emsesp
