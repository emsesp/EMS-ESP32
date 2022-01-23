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

#include "generic.h"

namespace emsesp {

REGISTER_FACTORY(Generic, EMSdevice::DeviceType::GENERIC);

uuid::log::Logger Generic::logger_{F_(generic), uuid::log::Facility::CONSOLE};

Generic::Generic(uint8_t device_type, uint8_t device_id, uint8_t product_id, const char * version, const std::string & name, uint8_t flags, uint8_t brand)
    : EMSdevice(device_type, device_id, product_id, version, name, flags, brand) {
    // RF-Sensor 0x40 sending temperature in telegram 0x435, see https://github.com/emsesp/EMS-ESP32/issues/103
    if (device_id == 0x40) {
        register_telegram_type(0x435, F("RFSensorMessage"), false, MAKE_PF_CB(process_RFSensorMessage));
        register_device_value(DeviceValueTAG::TAG_NONE, &rfTemp_, DeviceValueType::SHORT, FL_(div10), FL_(RFTemp), DeviceValueUOM::DEGREES);
    }
}

// type 0x435 rf remote sensor
void Generic::process_RFSensorMessage(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, rfTemp_, 0); // is * 10
}

} // namespace emsesp
