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

#include "heatpump.h"

namespace emsesp {

REGISTER_FACTORY(Heatpump, EMSdevice::DeviceType::HEATPUMP);

uuid::log::Logger Heatpump::logger_{F_(heatpump), uuid::log::Facility::CONSOLE};

Heatpump::Heatpump(uint8_t device_type, uint8_t device_id, uint8_t product_id, const std::string & version, const std::string & name, uint8_t flags, uint8_t brand)
    : EMSdevice(device_type, device_id, product_id, version, name, flags, brand) {
    LOG_DEBUG(F("Adding new Heat Pump module with device ID 0x%02X"), device_id);

    // telegram handlers
    register_telegram_type(0x047B, F("HP1"), true, [&](std::shared_ptr<const Telegram> t) { process_HPMonitor1(t); });
    register_telegram_type(0x042B, F("HP2"), true, [&](std::shared_ptr<const Telegram> t) { process_HPMonitor2(t); });
}

// context submenu
void Heatpump::add_context_menu() {
}

void Heatpump::device_info_web(JsonArray & root) {
}

// display all values into the shell console
void Heatpump::show_values(uuid::console::Shell & shell) {
    // EMSdevice::show_values(shell); // always call this to show header
}

// publish values via MQTT
void Heatpump::publish_values() {
}

// check to see if values have been updated
bool Heatpump::updated_values() {
    return false;
}

// add console commands
void Heatpump::console_commands() {
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

/*
 * Type 0x42B- HeatPump Monitor 1
 * e.g. "38 10 FF 00 03 2B 00 D1 08 2A 01"
 */
void Heatpump::process_HPMonitor1(std::shared_ptr<const Telegram> telegram) {
    // still to implement
}

/*
 * Type 0x47B - HeatPump Monitor 2
 * e.g. "38 10 FF 00 03 7B 08 24 00 4B"
 */
void Heatpump::process_HPMonitor2(std::shared_ptr<const Telegram> telegram) {
    // still to implement
}

#pragma GCC diagnostic pop

} // namespace emsesp