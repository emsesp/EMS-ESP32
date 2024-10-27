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

#include "alert.h"

namespace emsesp {

REGISTER_FACTORY(Alert, EMSdevice::DeviceType::ALERT);

Alert::Alert(uint8_t device_type, uint8_t device_id, uint8_t product_id, const char * version, const char * name, uint8_t flags, uint8_t brand)
    : EMSdevice(device_type, device_id, product_id, version, name, flags, brand) {
    // EM10, device-id 0x12, listens to error messages, measures voltage input and send 0x1A to the boiler.
    // values already shown in boiler
    // see https://github.com/emsesp/EMS-ESP32/issues/575
    register_telegram_type(0x1A, "UBASetPoints", false, MAKE_PF_CB(process_UBASetPoints));

    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &setFlowTemp_, DeviceValueType::UINT8, FL_(setFlowTemp), DeviceValueUOM::DEGREES);
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &setBurnPow_, DeviceValueType::UINT8, FL_(setBurnPow), DeviceValueUOM::PERCENT);
}
// UBASetPoint 0x1A
void Alert::process_UBASetPoints(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, setFlowTemp_, 0); // boiler set temp from thermostat
    has_update(telegram, setBurnPow_, 1);  // max burner power in %
}
} // namespace emsesp