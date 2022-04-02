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
    if (device_id == 0x41) { // DHW module
        register_telegram_type(0x34, F("MonitorWW"), false, MAKE_PF_CB(process_MonitorWW));
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA_WW, &wwSetTemp_, DeviceValueType::UINT, nullptr, FL_(wwSetTemp), DeviceValueUOM::DEGREES);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA_WW, &wwCurTemp_, DeviceValueType::USHORT, FL_(div10), FL_(wwCurTemp), DeviceValueUOM::DEGREES);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA_WW, &wwCurTemp2_, DeviceValueType::USHORT, FL_(div10), FL_(wwCurTemp2), DeviceValueUOM::DEGREES);
    }
}


// type 0x435 rf remote sensor
void Generic::process_RFSensorMessage(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, rfTemp_, 0); // is * 10
}
/*
 * MonitorWW - type 0x34 - dhw monitor. 10 bytes long
 * received every 10 seconds
 * Unknown(0x41) -> All(0x00), UBAMonitorWW(0x34), data: 37 02 25 02 25 00 00 00 00
*/
void Generic::process_MonitorWW(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, wwSetTemp_, 0);
    has_update(telegram, wwCurTemp_, 1);
    has_update(telegram, wwCurTemp2_, 3);
}

} // namespace emsesp
