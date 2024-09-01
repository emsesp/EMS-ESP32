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

#ifndef EMSESP_SWITCH_H
#define EMSESP_SWITCH_H

#include "emsesp.h"

namespace emsesp {

class Switch : public EMSdevice {
  public:
    Switch(uint8_t device_type, uint8_t device_id, uint8_t product_id, const char * version, const char * name, uint8_t flags, uint8_t brand);

  private:
    void process_WM10SetMessage(std::shared_ptr<const Telegram> telegram);
    void process_WM10MonitorMessage(std::shared_ptr<const Telegram> telegram);
    void process_WM10TempMessage(std::shared_ptr<const Telegram> telegram);

    uint16_t flowTempHc_;
    uint8_t  status_;
    uint8_t  activated_;
};

} // namespace emsesp

#endif
