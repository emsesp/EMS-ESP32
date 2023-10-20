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

#ifndef EMSESP_ALERT_H
#define EMSESP_ALERT_H

#include "emsesp.h"

namespace emsesp {

class Alert : public EMSdevice {
  public:
    Alert(uint8_t device_type, uint8_t device_id, uint8_t product_id, const char * version, const char * name, uint8_t flags, uint8_t brand);

  private:
    void process_EM100SetMessage(std::shared_ptr<const Telegram> telegram);
    void process_EM100MonitorMessage(std::shared_ptr<const Telegram> telegram);
    void process_EM100TempMessage(std::shared_ptr<const Telegram> telegram);

    int16_t headerTemp_; // T0
    int16_t input_;      // IO1
    uint8_t errorState_; // OE1
    uint8_t errorPump_;  // IE0
    uint8_t outPower_;   // IO1
    uint8_t dip_;        // dip switch
};

} // namespace emsesp

#endif
