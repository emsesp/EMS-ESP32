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

#include "controller.h"

namespace emsesp {

REGISTER_FACTORY(Controller, EMSdevice::DeviceType::CONTROLLER);

Controller::Controller(uint8_t device_type, uint8_t device_id, uint8_t product_id, const char * version, const char * name, uint8_t flags, uint8_t brand)
    : EMSdevice(device_type, device_id, product_id, version, name, flags, brand) {
    // IVT broadcasts Thermostat time  from controller (0x09) if display is off.
    if ((flags & 0x0F) == EMSdevice::EMS_DEVICE_FLAG_IVT) {
        register_telegram_type(0x06, "RCTime", false, MAKE_PF_CB(process_dateTime));
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &dateTime_, DeviceValueType::STRING, FL_(dateTime), DeviceValueUOM::NONE);
    }
}

// process_dateTime - type 0x06 - date and time from a thermostat - 14 bytes long, IVT only
void Controller::process_dateTime(std::shared_ptr<const Telegram> telegram) {
    if (telegram->offset > 0 || telegram->message_length < 5) {
        return;
    }
    char newdatetime[sizeof(dateTime_)];
    // publish as dd.mm.yyyy hh:mm
    snprintf(newdatetime,
             sizeof(dateTime_),
             "%02d.%02d.%04d %02d:%02d",
             telegram->message_data[3],
             telegram->message_data[1] - 1,
             (telegram->message_data[0] & 0x7F) + 2000,
             telegram->message_data[2],
             telegram->message_data[4]);
    has_update(dateTime_, newdatetime, sizeof(dateTime_));
}

} // namespace emsesp