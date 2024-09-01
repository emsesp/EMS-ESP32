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

#ifndef EMSESP_HEATSOURCE_H
#define EMSESP_HEATSOURCE_H

#include "emsesp.h"

namespace emsesp {

class Heatsource : public EMSdevice {
  public:
    Heatsource(uint8_t device_type, uint8_t device_id, uint8_t product_id, const char * version, const char * name, uint8_t flags, uint8_t brand);

  private:
    // hs1 .. hs16
    uint32_t burnWorkMin_;
    uint8_t  selBurnPow_;
    uint8_t  curBurnPow_;

    // ahs
    uint16_t curFlowTemp_;   // Current flow temperature
    uint16_t retTemp_;       // Return temperature
    int16_t  cylTopTemp_;    // TB1
    int16_t  cylCenterTemp_; // TB2
    int16_t  cylBottomTemp_; // TB3
    int16_t  aFlowTemp_;     // TA1
    int16_t  aRetTemp_;      // TR1
    uint8_t  aPumpMod_;      // PR1 - percent
    uint8_t  valveBuffer_;   // VB1
    uint8_t  valveReturn_;   // VR1
    uint16_t flueGasTemp_;
    // uint8_t valveBypass_;   // VR2 position unknown
    // uint8_t heatSource_;    // OEV

    uint8_t burner_;        // bit 5, offset 1, 54E
    uint8_t aPump_;         // bit 0, offset 1, 54E
    uint8_t heatRequest_;   // offset 2, percent
    uint8_t blockRemain_;   // offset 24, 550 min
    uint8_t blockRemainWw_; // offset 25, 550 min

    // Settings:
    uint8_t  vr2Config_;     // pos 12: off(00)/Keelbypass(01)/(hc1pump(02) only standalone)
    uint8_t  ahsActivated_;  // pos 00: Alternate heat source activation: No(00),Yes(01)
    uint8_t  aPumpConfig_;   // pos 04: Buffer primary pump->Config pump: No(00),Yes(01)
    uint8_t  aPumpSignal_;   // pos 03: Output for PR1 pump: On/Off(00),PWM(01),PWM invers(02)
    uint8_t  aPumpMin_;      // pos 21: Min output pump PR1 (%)
    uint8_t  tempRise_;      // pos 01: AHS return temp rise: No(00),Yes(01) (mixer VR1)
    uint8_t  setReturnTemp_; // pos 06: Set temp return (°C) (VR1)
    uint16_t mixRuntime_;    // pos 10/11?: Mixer run time (s) (VR1)
    uint8_t  setFlowTemp_;   // pos 07: Set flow temp AHS (°C) (Buffer)
    uint8_t  bufBypass_;     // pos 02: Puffer bypass: No(00), Mischer(01), Ventil(02) (Buffer)
    uint16_t bufMixRuntime_; // pos 8/9: Bypass mixer run time: [time] (s) (Buffer)
    uint8_t  bufConfig_;     // pos 20: Konfig WW-Speicher Monovalent(01), Bivalent(02) (buffer)
    uint8_t  blockMode_;     // pos 16: Config htg. blocking mode: No(00),Automatic(01),Always block02) (blocking)
    uint8_t  blockTerm_;     // pos 17: Config of block terminal: NO(00), NC(01)
    int8_t   blockHyst_;     // pos 14?: Hyst. for bolier block (K)
    uint8_t  releaseWait_;   // pos 15: Boiler release wait time (min)

    void process_CascadeMessage(std::shared_ptr<const Telegram> telegram);
    void process_UBAMonitorFastPlus(std::shared_ptr<const Telegram> telegram);

    void process_amTempMessage(std::shared_ptr<const Telegram> telegram);
    void process_amStatusMessage(std::shared_ptr<const Telegram> telegram);
    void process_amSettingMessage(std::shared_ptr<const Telegram> telegram);
    void process_amCommandMessage(std::shared_ptr<const Telegram> telegram);
    void process_amExtraMessage(std::shared_ptr<const Telegram> telegram);


    bool set_vr2Config(const char * value, const int8_t id);     // pos 12: off(00)/Keelbypass(01)/(hc1pump(02) only standalone)
    bool set_ahsActivated(const char * value, const int8_t id);  // pos 00: Alternate heat source activation: No(00),Yes(01)
    bool set_aPumpConfig(const char * value, const int8_t id);   // pos 04: Buffer primary pump->Config pump: No(00),Yes(01)
    bool set_aPumpSignal(const char * value, const int8_t id);   // pos 03: Output for PR1 pump: On/Off(00),PWM(01),PWM invers(02)
    bool set_aPumpMin(const char * value, const int8_t id);      // pos 21: Min output pump PR1 (%)
    bool set_tempRise(const char * value, const int8_t id);      // pos 01: AHS return temp rise: No(00),Yes(01) (mixer VR1)
    bool set_setReturnTemp(const char * value, const int8_t id); // pos 06: Set temp return (°C) (VR1)
    bool set_mixRuntime(const char * value, const int8_t id);    // pos 10/11?: Mixer run time (s) (VR1)
    bool set_setFlowTemp(const char * value, const int8_t id);   // pos 07: Set flow temp AHS (°C) (Buffer)
    bool set_bufBypass(const char * value, const int8_t id);     // pos 02: Puffer bypass: No(00), Mischer(01), Ventil(02) (Buffer)
    bool set_bufMixRuntime(const char * value, const int8_t id); // pos 8/9: Bypass mixer run time: [time] (s) (Buffer)
    bool set_bufConfig(const char * value, const int8_t id);     // pos 20: Konfig WW-Speicher Monovalent(01), Bivalent(02) (buffer)
    bool set_blockMode(const char * value, const int8_t id);     // pos 16: Config htg. blocking mode: No(00),Automatic(01),Always block02) (blocking)
    bool set_blockTerm(const char * value, const int8_t id);     // pos 17: Config of block terminal: NO(00), NC(01)
    bool set_blockHyst(const char * value, const int8_t id);     // pos 14?: Hyst. for bolier block (K)
    bool set_releaseWait(const char * value, const int8_t id);   // pos 15: Boiler release wait time (min)

    bool set_valveBuffer(const char * value, const int8_t id);
    bool set_valveReturn(const char * value, const int8_t id);
};

} // namespace emsesp

#endif
