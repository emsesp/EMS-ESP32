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

#ifndef EMSESP_WATER_H
#define EMSESP_WATER_H

#include "emsesp.h"

namespace emsesp {

class Water : public EMSdevice {
  public:
    Water(uint8_t device_type, uint8_t device_id, uint8_t product_id, const char * version, const char * name, uint8_t flags, uint8_t brand);

  private:
    static uuid::log::Logger logger_;

    uint8_t dhw_;

    // SM100wwTemperature - 0x07D6
    uint16_t wwTemp_;
    uint16_t wwFlow_;
    uint16_t wwTemp2_;
    uint16_t wwColdTemp_;
    uint16_t wwTemp_5_;
    uint16_t wwRetTemp_;

    // SM100ValveStatus - 0x07AD
    uint8_t wwRetValve_; // VS5

    // SM100wwStatus - 0x07AA
    uint8_t wwPump_;

    // SM100wwParam2 - 0x07AC
    uint8_t wwDeltaTRet_;

    // SM100wwParam - 0x07A6
    uint8_t wwMaxTemp_;
    uint8_t wwSelTemp_;
    uint8_t wwRedTemp_;
    uint8_t wwDailyTemp_;
    uint8_t wwDisinfectionTemp_;
    uint8_t wwHotTemp_;
    uint8_t errorDisp_; // error display off/normal/inverted

    // SM100wwKeepWarm - 0x07AE
    uint8_t wwKeepWarm_;

    // SM100wwCirc - 0x07A5
    uint8_t wwCirc_;
    uint8_t wwCircMode_;
    uint8_t wwCircTc_;

    // SM100wwStatus2 - 0x07E0
    uint8_t wwPumpMod_;
    uint8_t wwStatus2_;

    // mixer
    uint8_t wwStatus_;
    int8_t  wwDiffTemp_;
    uint8_t wwRequiredTemp_;

    // IPM
    uint16_t HydrTemp_;
    int8_t   wwHystOn_;         // Hyst on (default -5)
    int8_t   wwHystOff_;        // Hyst off (default -1)
    uint8_t  wwFlowTempOffset_; // default 40


    void process_SM100wwTemperature(std::shared_ptr<const Telegram> telegram);
    void process_SM100wwStatus(std::shared_ptr<const Telegram> telegram);
    void process_SM100wwStatus2(std::shared_ptr<const Telegram> telegram);
    void process_SM100wwCommand(std::shared_ptr<const Telegram> telegram);
    void process_SM100wwCirc(std::shared_ptr<const Telegram> telegram);
    void process_SM100wwParam(std::shared_ptr<const Telegram> telegram);
    void process_SM100wwParam2(std::shared_ptr<const Telegram> telegram);
    void process_SM100wwKeepWarm(std::shared_ptr<const Telegram> telegram);
    void process_SM100ValveStatus(std::shared_ptr<const Telegram> telegram);

    void process_MMPLUSStatusMessage_WWC(std::shared_ptr<const Telegram> telegram);
    void process_MMPLUSSetMessage_WWC(std::shared_ptr<const Telegram> telegram);
    void process_MMPLUSConfigMessage_WWC(std::shared_ptr<const Telegram> telegram);

    void process_IPMMonitorWW(std::shared_ptr<const Telegram> telegram);
    void process_IPMHydrTemp(std::shared_ptr<const Telegram> telegram);
    void process_IPMParameterWW(std::shared_ptr<const Telegram> telegram);


    bool set_wwSelTemp(const char * value, const int8_t id);
    bool set_wwMaxTemp(const char * value, const int8_t id);
    bool set_wwRedTemp(const char * value, const int8_t id);
    bool set_wwHotTemp(const char * value, const int8_t id);
    bool set_wwCirc(const char * value, const int8_t id);
    bool set_wwCircMode(const char * value, const int8_t id);
    bool set_wwCircTc(const char * value, const int8_t id);
    bool set_wwKeepWarm(const char * value, const int8_t id);
    bool set_wwDisinfectionTemp(const char * value, const int8_t id);
    bool set_wwDailyTemp(const char * value, const int8_t id);

    bool set_wwDiffTemp(const char * value, const int8_t id);
    bool set_wwRequiredTemp(const char * value, const int8_t id);

    bool set_wwFlowTempOffset(const char * value, const int8_t id);
    bool set_wwHystOn(const char * value, const int8_t id);
    bool set_wwHystOff(const char * value, const int8_t id);

    bool set_wwDeltaTRet(const char * value, const int8_t id);
    bool set_wwRetValve(const char * value, const int8_t id);
    bool set_errorDisp(const char * value, const int8_t id);
};

} // namespace emsesp

#endif
