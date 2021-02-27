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

#ifndef EMSESP_HEATPUMP_H
#define EMSESP_HEATPUMP_H

#include "emsesp.h"

namespace emsesp {

class Heatpump : public EMSdevice {
  public:
    Heatpump(uint8_t device_type, uint8_t device_id, uint8_t product_id, const std::string & version, const std::string & name, uint8_t flags, uint8_t brand);

    virtual bool publish_ha_config();

  private:
    static uuid::log::Logger logger_;

    uint8_t airHumidity_;
    uint8_t dewTemperature_;
    uint8_t id_;

    void process_HPMonitor1(std::shared_ptr<const Telegram> telegram);
    void process_HPMonitor2(std::shared_ptr<const Telegram> telegram);
};

} // namespace emsesp

#endif
