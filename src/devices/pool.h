/*
 * EMS-ESP - https://github.com/emsesp/EMS-ESP
 * Copyright 2020-2023  Paul Derbyshire
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

#ifndef EMSESP_POOL_H
#define EMSESP_POOL_H

#include "emsesp.h"

namespace emsesp {

class Pool : public EMSdevice {
  public:
    Pool(uint8_t device_type, uint8_t device_id, uint8_t product_id, const char * version, const char * name, uint8_t flags, uint8_t brand);

  private:
    static uuid::log::Logger logger_;

    void process_HpPoolStatus(std::shared_ptr<const Telegram> telegram);

  private:
    // MP100 pool
    int16_t poolTemp_;
    uint8_t poolShuntStatus_;
    uint8_t poolShunt_;

    uint8_t poolShuntStatus__ = EMS_VALUE_UINT8_NOTSET; // temp value
};

} // namespace emsesp

#endif
