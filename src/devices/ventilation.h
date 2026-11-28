/*
 * EMS-ESP - https://github.com/emsesp/EMS-ESP
 * Copyright 2020-2026  emsesp.org
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

#ifndef EMSESP_VENTILATION_H
#define EMSESP_VENTILATION_H

#include "emsesp.h"

namespace emsesp {

class Ventilation : public EMSdevice {
  public:
    Ventilation(uint8_t device_type, uint8_t device_id, uint8_t product_id, const char * version, const char * name, uint8_t flags, uint8_t brand);

  private:
    // index of autoL0 in enum_ventMode, the auto level entries (autoL0..autodemand) are appended at the end
    static constexpr uint8_t VENTMODE_AUTOL0 = 12;

    uint8_t  mode_      = EMS_VALUE_UINT8_NOTSET; // combined mode (0x56B + 0x589) shown in the ventmode entity
    uint8_t  rawMode_   = EMS_VALUE_UINT8_NOTSET; // raw mode from 0x56B (enum index, 0=auto)
    uint8_t  autoLevel_ = EMS_VALUE_UINT8_NOTSET; // raw auto level from 0x589 (0=L0..4=L4, 5=demand)
    int16_t  outFresh_;
    int16_t  inFresh_;
    int16_t  inEx_;
    int16_t  outEx_;
    uint16_t voc_;
    uint8_t  humidity_;
    uint8_t  bypass_;
    uint8_t  ventInSpeed_;
    uint8_t  ventOutSpeed_;
    uint16_t ventOutRpm_;
    uint16_t ventInRpm_;
    uint8_t  ventOutVoltage_;
    uint8_t  ventInVoltage_;
    uint8_t  ventBalance_;
    uint8_t  level1Pct_;
    uint8_t  level2Pct_;
    uint8_t  level4Pct_;
    uint8_t  bypassMinOutdoorTemp_ = EMS_VALUE_UINT8_NOTSET;
    uint8_t  bypassMaxSupplyTemp_  = EMS_VALUE_UINT8_NOTSET;
    uint16_t airflowNominal_      = EMS_VALUE_UINT16_NOTSET; // nominal airflow in m³/h (0x557)

    // handlers: 0x0556 0x055C 0x0561 0x056B 0x0575 0x0583 0x0585 0x0586 0x0587 0x0588 0x058D 0x058E 0x058F 0x0590 0x05CF 0x05D9 0x05E3
    void process_SetMessage(const std::shared_ptr<const Telegram> & telegram);     // 0x55C
    void process_MonitorMessage(const std::shared_ptr<const Telegram> & telegram); // 0x583
    void process_ModeMessage(const std::shared_ptr<const Telegram> & telegram);    // 0x56B
    void process_BlowerMessage(const std::shared_ptr<const Telegram> & telegram);  // 0x585
    void process_VOCMessage(const std::shared_ptr<const Telegram> & telegram);     // 0x5D9
    void process_BypassMessage(const std::shared_ptr<const Telegram> & telegram);  // 0x587
    void process_BalanceMessage(const std::shared_ptr<const Telegram> & telegram); // 0x556
    void process_LevelMessage(const std::shared_ptr<const Telegram> & telegram);   // 0x561
    void process_AutoLevelMessage(const std::shared_ptr<const Telegram> & telegram); // 0x589
    void process_AirflowMessage(const std::shared_ptr<const Telegram> & telegram);   // 0x557

    bool set_ventMode(const char * value, const int8_t id);
    bool set_bypass(const char * value, const int8_t id);
    bool set_bypassMinOutdoorTemp(const char * value, const int8_t id);
    bool set_bypassMaxSupplyTemp(const char * value, const int8_t id);
    bool set_filter(const char * value, const int8_t id);
    bool set_ventBalance(const char * value, const int8_t id);
    bool set_level1Pct(const char * value, const int8_t id);
    bool set_level2Pct(const char * value, const int8_t id);
    bool set_level4Pct(const char * value, const int8_t id);
    bool set_airflowNominal(const char * value, const int8_t id);

    void update_mode(); // combines 0x56B and 0x589 into mode_
};

} // namespace emsesp

#endif
