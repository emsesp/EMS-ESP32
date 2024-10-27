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

#ifndef EMSESP_VENTILATION_H
#define EMSESP_VENTILATION_H

#include "emsesp.h"

namespace emsesp {

class Ventilation : public EMSdevice {
  public:
    Ventilation(uint8_t device_type, uint8_t device_id, uint8_t product_id, const char * version, const char * name, uint8_t flags, uint8_t brand);

  private:
    uint8_t  mode_;
    int16_t  outFresh_;
    int16_t  inFresh_;
    int16_t  inEx_;
    int16_t  outEx_;
    uint16_t voc_;
    uint8_t  humidity_;
    uint8_t  bypass_;
    // uint16_t filterRemain_;
    uint8_t ventInSpeed_;
    uint8_t ventOutSpeed_;

    // handlers: 0x056B 0x0575 0x0583 0x0585 0x0586 0x0587 0x0588 0x058D 0x058E 0x058F 0x0590 0x05CF 0x05D9 0x05E3
    void process_SetMessage(std::shared_ptr<const Telegram> telegram);
    void process_MonitorMessage(std::shared_ptr<const Telegram> telegram);
    void process_ModeMessage(std::shared_ptr<const Telegram> telegram);   // 0x56B
    void process_BlowerMessage(std::shared_ptr<const Telegram> telegram); // 0x56B
    void process_VOCMessage(std::shared_ptr<const Telegram> telegram);    // 0x56B
    void process_BypassMessage(std::shared_ptr<const Telegram> telegram); // 0x56B

    bool set_ventMode(const char * value, const int8_t id);
    bool set_bypass(const char * value, const int8_t id);
    bool set_filter(const char * value, const int8_t id);


    /* Sensors:
    outdoor air temp (außenluft)
    supply air temp (zuluft)
    extract air temp (abluft)
    away air temp (fortluft)
    supply blower (zuluftgebläse)
    supply blower mod (zuluftebläse drehzahl)
    away blower (abluftgebläse)
    away blower mod (abluftgebläse drehzahl)
    Anschlussvariante
    el. vorheizer
    ext. el. vorheizreg.
    nachheiz zulufttemp
    mischer öffnen
    mischer schließen
    mischerposition
    zuluft temp soll
    zuluft temp ist
    leistung nachheizreg.
    erdwärmetauscher klappe
    solekreispumpe
    abluftfeuchte
    abluftqualität
    raumluftfechte
    raumluftqualität
    luftfeuchte fernbed. 1..4
    */
    /* Parameters:
    Gerätetyp,
    Nennvolumentstrom,
    Filterlaufzeit 1-6-12 m
    Filterwechsel confirm CMD
    Lüftungsfrostschutz: _el._preheat_, Disballance | Interval
    Ext. Frostschutz: on/_off_
    Bypass _on_, off
    min. outdoortemp 12 15 19 °C
    max. outdoortemp 21-24-30 C
    Enthalpie Wärmetauscher instaliert nein-ja
    Feuchteschutz AUs/ 1-24 h
    Lüfterstufe 1-4, Drehzahlanpassung
    ext. Luftfeuchtefühler inst.? _nein_, ja
    Abluftfeuchtefühler inst.? _nein_, ja
    Luftfeuchte Fernbed. _nein_, ja
    Luftfeuchte: trocken, _normal_, feucht
    Abluftqualitätsfühler inst. _ja_, nein
    ext. Luftqualfühl? _nein_, ja
    Lufqualität: ausreichend, _normal_, hoch
    el. Nachheizregister inst. _nein_, ja
    Nachheiz-Zuluft temp: 10-22-30 °C
    Erdwärmetauscher inst? _nein_, Luft, Sole
    Taster Funktion: nein, einschlafen, intensiv, bypass, party, kamin
    ext. Störung aktivieren: _nein_, ja, invertiert
    Dauer einschlafen: 15-60-120 min
    Dauer Intensiv: 5-15-60 min
    Dauer Bypass Abluft: 1-8-12 h
    Dauer Bypass: 1-8-12 h
    Dauer PArty 1-8-12 h
    Dauer Kamin: 5-10-15 min
    Volumenstromabgleich 90-100-110 %
    */
};

} // namespace emsesp

#endif
