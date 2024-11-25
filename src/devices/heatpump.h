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

#ifndef EMSESP_HEATPUMP_H
#define EMSESP_HEATPUMP_H

#include "emsesp.h"

namespace emsesp {

class Heatpump : public EMSdevice {
  public:
    Heatpump(uint8_t device_type, uint8_t device_id, uint8_t product_id, const char * version, const char * name, uint8_t flags, uint8_t brand);

  private:
    uint8_t airHumidity_;
    uint8_t dewTemperature_;

    // HM200
    uint8_t controlStrategy_;
    uint8_t lowNoiseMode_;
    uint8_t lowNoiseStart_;
    uint8_t lowNoiseStop_;
    uint8_t hybridDHW_;
    uint8_t energyPriceGas_;
    uint8_t energyPriceEl_;
    uint8_t energyPricePV_;
    int8_t  switchOverTemp_;
    uint8_t heatingPumpMod_;
    uint8_t hpCompSpd_;

    // Function test
    uint8_t airPurgeMode_;
    uint8_t heatPumpOutput_;
    uint8_t coolingCircuit_;
    uint8_t compStartMod_;
    uint8_t heatDrainPan_;
    uint8_t heatCable_;

    // HM200 temperatures
    int16_t flowTemp_;   // TH1
    int16_t retTemp_;    // TH2
    int16_t sysRetTemp_; // TH3
    int16_t hpTc3_;      // condenser temp.
    int16_t hpTr1_;      // compressor temp.
    int16_t hpTr3_;      // cond. temp. heating
    int16_t hpTr4_;      // cond. temp. clg
    int16_t hpTr5_;      // suction line temp.
    int16_t hpTr6_;      // hot gas temp.
    int16_t hpTl2_;      // inlet air temperature
    int16_t hpTa4_;      // drain pan temp.
    int16_t hpJr0_;      // low pressure sensor
    int16_t hpJr1_;      // high pressure sensor

    uint32_t nrgTotal_;
    uint32_t nrgWw_;
    uint32_t nrgHeat_;
    uint32_t meterTotal_;
    uint32_t meterComp_;
    uint32_t meterEHeat_;
    uint32_t meterHeat_;
    uint32_t meterWw_;

    uint32_t heatStartsHp_;
    uint32_t wwStartsHp_;
    uint32_t fuelHeat_;
    uint32_t fuelDhw_;
    uint32_t elHeat_;
    uint32_t elDhw_;
    uint32_t elGenHeat_;
    uint32_t elGenDhw_;

    void process_HPMonitor1(std::shared_ptr<const Telegram> telegram);
    void process_HPMonitor2(std::shared_ptr<const Telegram> telegram);
    void process_HPSettings(std::shared_ptr<const Telegram> telegram);
    void process_HPFunctionTest(std::shared_ptr<const Telegram> telegram);
    void process_HPTemperature(std::shared_ptr<const Telegram> telegram);
    void process_HPFlowTemp(std::shared_ptr<const Telegram> telegram);
    void process_HPComp(std::shared_ptr<const Telegram> telegram);
    void process_HpEnergy(std::shared_ptr<const Telegram> telegram);
    void process_HpMeters(std::shared_ptr<const Telegram> telegram);
    void process_HpStarts(std::shared_ptr<const Telegram> telegram);
    void process_HpEnergy1(std::shared_ptr<const Telegram> telegram);
    void process_HpEnergy2(std::shared_ptr<const Telegram> telegram);

    bool set_controlStrategy(const char * value, const int8_t id);
    bool set_lowNoiseMode(const char * value, const int8_t id);
    bool set_lowNoiseStart(const char * value, const int8_t id);
    bool set_lowNoiseStop(const char * value, const int8_t id);
    bool set_hybridDHW(const char * value, const int8_t id);
    bool set_energyPriceGas(const char * value, const int8_t id);
    bool set_energyPriceEl(const char * value, const int8_t id);
    bool set_energyPricePV(const char * value, const int8_t id);
    bool set_switchOverTemp(const char * value, const int8_t id);
    bool set_airPurgeMode(const char * value, const int8_t id);
    bool set_heatPumpOutput(const char * value, const int8_t id);
    bool set_coolingCircuit(const char * value, const int8_t id);
    bool set_compStartMod(const char * value, const int8_t id);
    bool set_heatDrainPan(const char * value, const int8_t id);
    bool set_heatCable(const char * value, const int8_t id);
};

} // namespace emsesp

#endif
