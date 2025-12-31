/*
 * EMS-ESP - https://github.com/emsesp/EMS-ESP
 * Copyright 2020-2025  emsesp.org - proddy, MichaelDvP
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
    if (device_id != EMSdevice::EMS_DEVICE_ID_RFBASE) { // Modems have no entities
        return;
    }
    register_telegram_type(0xD1, "RFOutdoorTemp", false, MAKE_PF_CB(process_OutdoorTemp));
    register_telegram_type(0x06, "RCTime", false, MAKE_PF_CB(process_RCTime));

    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &dateTime_, DeviceValueType::STRING, FL_(tpl_datetime), FL_(dateTime), DeviceValueUOM::NONE);
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                          &outdoorTemp_,
                          DeviceValueType::INT16,
                          DeviceValueNumOp::DV_NUMOP_DIV10,
                          FL_(outdoorTemp),
                          DeviceValueUOM::DEGREES);
    // Roomthermostats
    for (uint8_t i = 0; i < 16; i++) {
        register_telegram_type(0x0BDD + i, "Room", false, MAKE_PF_CB(process_roomThermostat));                 // broadcasted
        register_telegram_type(0x0B3D + i, "Roomname", false, MAKE_PF_CB(process_roomThermostatName));         // fetch for active circuits
        register_telegram_type(0x0BB5 + i, "Roomsettings", false, MAKE_PF_CB(process_roomThermostatSettings)); // fetch for active circuits
        register_telegram_type(0x1230 + i, "Roomparams", false, MAKE_PF_CB(process_roomThermostatParam));      // fetch for active circuits
        register_telegram_type(0x1244 + i, "Roomdata", false, MAKE_PF_CB(process_roomThermostatData));         // broadcasted
    }
    register_telegram_type(0xDB65, "Roomschedule", true, MAKE_PF_CB(process_roomSchedule));
    // 0x2040, broadcast 36 bytes:
    // data: 0E 60 00 DF 0D AF 0A 46 0A 46 02 9A 1C 53 1C 53 12 AD 12 AD 00 00 13 C2
    // data: 1F 37 1F 37 00 00 00 00 18 97 11 27 (offset 24)
#if defined(EMSESP_STANDALONE)
    // if we're just dumping out values, create a single dummy SRC
    auto new_room = std::make_shared<Connect::RoomCircuit>(0);
    room_circuits_.push_back(new_room);
    register_device_values_room(new_room);
#endif
}
/*
 * OutdoorTemp - type 0xD1 - external temperature
 */
void Connect::process_OutdoorTemp(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, outdoorTemp_, 0);
}

// sent if thermostat is connected
// https://github.com/emsesp/EMS-ESP32/issues/2277
void Connect::process_RCTime(std::shared_ptr<const Telegram> telegram) {
    if (telegram->offset || telegram->message_length < 10) {
        return;
    }
    char time[sizeof(dateTime_)];
    snprintf(time,
             sizeof(time),
             "%02d.%02d.%04d %02d:%02d",
             telegram->message_data[3],
             telegram->message_data[1],
             telegram->message_data[0] + 2000,
             telegram->message_data[2],
             telegram->message_data[4]);
    has_update(dateTime_, time, sizeof(dateTime_));
}

// other values from 0x50 RF base
// (0x087F), data: 00 00
// (0x0880), data: 01 04
// (0x0889), data: 00 80 80 01
void Connect::register_device_values_room(std::shared_ptr<Connect::RoomCircuit> room) {
    auto tag = DeviceValueTAG::TAG_SRC1 + room->room();
    register_device_value(tag, &room->temp_, DeviceValueType::INT16, DeviceValueNumOp::DV_NUMOP_DIV10, FL_(roomTemp), DeviceValueUOM::DEGREES);
    register_device_value(tag, &room->humidity_, DeviceValueType::INT8, FL_(airHumidity), DeviceValueUOM::PERCENT);
    register_device_value(tag, &room->dewtemp_, DeviceValueType::INT16, DeviceValueNumOp::DV_NUMOP_DIV10, FL_(dewTemperature), DeviceValueUOM::DEGREES);
    register_device_value(
        tag, &room->seltemp_, DeviceValueType::UINT8, DeviceValueNumOp::DV_NUMOP_DIV2, FL_(selRoomTemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_seltemp), 5, 30);
    register_device_value(tag, &room->mode_, DeviceValueType::ENUM, FL_(enum_mode2), FL_(mode), DeviceValueUOM::NONE, MAKE_CF_CB(set_mode));
    register_device_value(tag, &room->name_, DeviceValueType::STRING, FL_(name), DeviceValueUOM::NONE, MAKE_CF_CB(set_name));
    register_device_value(tag, &room->childlock_, DeviceValueType::BOOL, FL_(childlock), DeviceValueUOM::NONE, MAKE_CF_CB(set_childlock));
    register_device_value(tag, &room->icon_, DeviceValueType::ENUM, FL_(enum_icons), FL_(icon), DeviceValueUOM::NONE, MAKE_CF_CB(set_icon));
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
    toggle_fetch(0x0B3D + num, true); // name
    toggle_fetch(0x0BB5 + num, true); // mode
    toggle_fetch(0x1230 + num, true); // unknown

    return new_room; // return back point to new HC object
}

// gateway(0x50) B all(0x00), ?(0x0BDD), data: 00 E6 36 2A
void Connect::process_roomThermostat(std::shared_ptr<const Telegram> telegram) {
    bool create = telegram->offset == 0 && telegram->message_data[0] < 0x80;
    auto rc     = room_circuit(telegram->type_id - 0xBDD, create);
    if (rc == nullptr) {
        return;
    }
    has_update(telegram, rc->temp_, 0);
    has_update(telegram, rc->humidity_, 2); // could show -3 if not set
    has_update(telegram, rc->seltemp_, 3);

    // calculate dew temperature
    const float k2 = 17.62;
    const float k3 = 243.12;
    const float t  = (float)rc->temp_ / 10;
    const float h  = (float)rc->humidity_ / 100;
    int16_t     dt = (10 * k3 * (((k2 * t) / (k3 + t)) + log(h)) / (((k2 * k3) / (k3 + t)) - log(h)));
    has_update(rc->dewtemp_, dt);
}

// gateway(0x48) W gateway(0x50), ?(0x0B42), data: 01 // icon in offset 0
// gateway(0x48) W gateway(0x50), ?(0x0B42), data: 00 4B 00 FC 00 63 00 68 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 (offset 1)
void Connect::process_roomThermostatName(std::shared_ptr<const Telegram> telegram) {
    auto rc = room_circuit(telegram->type_id - 0xB3D);
    if (rc == nullptr) {
        return;
    }
    has_update(telegram, rc->icon_, 0);
    for (uint8_t i = telegram->offset; i < telegram->message_length + telegram->offset && i < 100; i++) {
        if ((i > 1) && (i % 2) == 0) {
            rc->name_[(i - 2) / 2] = telegram->message_data[i - telegram->offset];
        }
    }
    rc->name_[50] = '\0'; // make sure name is terminated
}

// settings 0-mode, 1-tempautotemp, 3 - manualtemp, 6 - ?, 7 - childlock
// 0x0BB5, ff: data: 00 FF 00 24 01 FF 24 00
void Connect::process_roomThermostatSettings(std::shared_ptr<const Telegram> telegram) {
    auto rc = room_circuit(telegram->type_id - 0xBB5);
    if (rc == nullptr) {
        return;
    }
    has_enumupdate(telegram, rc->mode_, 0, {3, 1, 0}); // modes off, manual auto
    // has_update(telegram, rc->mode_, 0); // modes: auto, heat, cool, off
    // has_update(telegram, rc->tempautotemp_, 1); // FF means off
    // has_update(telegram, rc->manualtemp_, 3);
    has_update(telegram, rc->childlock_, 7);
}

// unknown telegrams, needs fetch
void Connect::process_roomThermostatParam(std::shared_ptr<const Telegram> telegram) {
    auto rc = room_circuit(telegram->type_id - 0x1230);
    if (rc == nullptr) {
        return;
    }
}

// unknown broadcasted telegrams
void Connect::process_roomThermostatData(std::shared_ptr<const Telegram> telegram) {
    auto rc = room_circuit(telegram->type_id - 0x1244);
    if (rc == nullptr) {
        return;
    }
}

// schedule for all thermostats
void Connect::process_roomSchedule(std::shared_ptr<const Telegram> telegram) {
    uint8_t length = ((telegram->offset + telegram->message_length) > 126) ? 126 - telegram->offset : telegram->message_length;
    memcpy(&schedule_[telegram->offset], telegram->message_data, length);
    for (uint8_t c : schedule_) {
        if (c == 0xFE) {
            return;
        }
    }
    toggle_fetch(telegram->type_id, false); // fetch only once if all is initialized
}

// Settings:

bool Connect::set_mode(const char * value, const int8_t id) {
    auto rc = room_circuit(id - DeviceValueTAG::TAG_SRC1);
    if (rc == nullptr) {
        return false;
    }
    uint8_t v;
    if (Helpers::value2enum(value, v, FL_(enum_mode2), {3, 1, 0})) {
        // if (Helpers::value2enum(value, v, FL_(enum_mode8))) {
        write_command(0xBB5 + rc->room(), 0, v); // no validate, mode change is broadcasted
        return true;
    }
    return false;
}

bool Connect::set_seltemp(const char * value, const int8_t id) {
    auto rc = room_circuit(id - DeviceValueTAG::TAG_SRC1);
    if (rc == nullptr) {
        return false;
    }
    float v;
    if (Helpers::value2float(value, v)) {
        // write_command(0xBB5 + rc->room(), rc->mode_ == 2 ? 1 : 3, v == -1 ? 0xFF : uint8_t(v * 2));
        write_command(0xBB5 + rc->room(), rc->mode_ == 0 ? 1 : 3, v == -1 ? 0xFF : uint8_t(v * 2));
        return true;
    }
    return false;
}

bool Connect::set_name(const char * value, const int8_t id) {
    auto rc = room_circuit(id - DeviceValueTAG::TAG_SRC1);
    if (rc == nullptr || value == nullptr || strlen(value) > 50) {
        return false;
    }
    Helpers::utf8tolatin1(rc->name_, value, sizeof(rc->name_));
    uint8_t len = strlen(rc->name_) * 2 + 2;
    uint8_t data[len];
    for (uint8_t i = 0; i < strlen(rc->name_) + 1; i++) { // include terminating '\0'
        data[2 * i]     = 0;
        data[2 * i + 1] = rc->name_[i];
    }
    uint8_t ofs = 0;
    while (len > 0) {
        uint8_t part = len > 25 ? 25 : len;
        write_command(0x0B3D + rc->room(), ofs + 1, &data[ofs], part, 0);
        ofs += part;
        len -= part;
    }
    return true;
}

bool Connect::set_childlock(const char * value, const int8_t id) {
    auto rc = room_circuit(id - DeviceValueTAG::TAG_SRC1);
    if (rc == nullptr) {
        return false;
    }
    bool b;
    if (Helpers::value2bool(value, b)) {
        write_command(0xBB5 + rc->room(), 7, b ? 1 : 0, 0xBB5 + rc->room());
        return true;
    }
    return false;
}

bool Connect::set_icon(const char * value, const int8_t id) {
    auto rc = room_circuit(id - DeviceValueTAG::TAG_SRC1);
    if (rc == nullptr) {
        return false;
    }
    uint8_t v;
    if (Helpers::value2enum(value, v, FL_(enum_icons))) {
        write_command(0x0B3D + rc->room(), 0, v, 0x0B3D + rc->room());
        return true;
    }
    return false;
}

} // namespace emsesp
