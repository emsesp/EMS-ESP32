/*
 * EMS-ESP - https://github.com/emsesp/EMS-ESP
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

#pragma once

// General
MAKE_PSTR(on, "on")
MAKE_PSTR(off, "off")
MAKE_PSTR(ON, "ON")
MAKE_PSTR(OFF, "OFF")

// Unit Of Measurement mapping - maps to DeviceValueUOM_s in emsdevice.cpp
// uom - also used with HA see https://github.com/home-assistant/core/blob/d7ac4bd65379e11461c7ce0893d3533d8d8b8cbf/homeassistant/const.py#L384
MAKE_PSTR(minutes, "Minuten")
MAKE_PSTR(hours, "Stunden")
MAKE_PSTR(days, "Tage")
MAKE_PSTR(seconds, "seconds")

// boiler
MAKE_PSTR(time, "Zeit")
MAKE_PSTR(date, "Datum")
MAKE_PSTR(continuous, "kontinuierlich")
MAKE_PSTR(3wayvalve, "3-Wege Ventil")
MAKE_PSTR(chargepump, "Ladepumpe")
MAKE_PSTR(hot, "Heiss")
MAKE_PSTR(high_comfort, "Heiss Komfort")
MAKE_PSTR(eco, "Eco")
MAKE_PSTR(intelligent, "Intelligent")
MAKE_PSTR(flow, "Fluss")
MAKE_PSTR(manual, "Manuell")
MAKE_PSTR(buffer, "Speicher")
MAKE_PSTR(bufferedflow, "Durchlaufspeicher")
MAKE_PSTR(layeredbuffer, "Schichtspeicher")
MAKE_PSTR(maintenance, "Wartung")
MAKE_PSTR(heating, "Heizen")
MAKE_PSTR(cooling, "K�hlen")

// heatpump
MAKE_PSTR(none, "none")
MAKE_PSTR(hot_water, "hot water")
MAKE_PSTR(pool, "pool")

// mixer
MAKE_PSTR_LIST(enum_shunt, F("gestoppt"), F("öffnen"), F("schließen"), F("Offen"), F("Geschlossen"))

// thermostat
MAKE_PSTR(light, "Leicht")
MAKE_PSTR(medium, "Mittel")
MAKE_PSTR(heavy, "Schwer")
MAKE_PSTR(own_prog, "Eigenprog")
MAKE_PSTR(start, "Start")
MAKE_PSTR(heat, "Heizen")
MAKE_PSTR(hold, "Halten")
MAKE_PSTR(cool, "Kühl")
MAKE_PSTR(end, "Ende")
MAKE_PSTR(german, "Deutsch")
MAKE_PSTR(dutch, "Niederländisch")
MAKE_PSTR(french, "Französisch")
MAKE_PSTR(italian, "Italienisch")
MAKE_PSTR(high, "hoch")
MAKE_PSTR(low, "niedrig")
MAKE_PSTR(radiator, "Heizkörper")
MAKE_PSTR(convector, "Konvektor")
MAKE_PSTR(floor, "Fussboden")
MAKE_PSTR(summer, "Sommer")
MAKE_PSTR(winter, "Winter")
MAKE_PSTR(outdoor, "Aussentemperatur")
MAKE_PSTR_WORD(mpc)
MAKE_PSTR(room, "Raum")
MAKE_PSTR(room_outdoor, "Raum+Au�en")
MAKE_PSTR(power, "Leistung")
MAKE_PSTR(constant, "konstant")
MAKE_PSTR(simple, "einfach")
MAKE_PSTR(optimized, "optimiert")
MAKE_PSTR(nofrost, "Frostschutz")
MAKE_PSTR(comfort, "Komfort")
MAKE_PSTR(night, "Nacht")
MAKE_PSTR(day, "Tag")
MAKE_PSTR(holiday, "Urlaub")
MAKE_PSTR(reduce, "reduziert")
MAKE_PSTR(noreduce, "unreduziert")
MAKE_PSTR(offset, "Anhebung")
MAKE_PSTR(design, "Auslegung")
MAKE_PSTR_WORD(tempauto)
MAKE_PSTR(minflow, "minfluss")
MAKE_PSTR(maxflow, "maxfluss")

MAKE_PSTR(fast, "fast")
MAKE_PSTR(slow, "slow")
MAKE_PSTR(weather_compensated, "weather compensated")
MAKE_PSTR(outside_basepoint, "outside basepoint")

MAKE_PSTR(internal_temperature, "interne Temperatur")
MAKE_PSTR(internal_setpoint, "interner Sollwert")
MAKE_PSTR(external_temperature, "externe Temperatur")
MAKE_PSTR(burner_temperature, "Kesseltemperatur")
MAKE_PSTR(ww_temperature, "Wassertemperatur")
MAKE_PSTR(functioning_mode, "functioning mode")
MAKE_PSTR(smoke_temperature, "Abgastemperatur")

// thermostat lists
MAKE_PSTR_LIST(tpl_datetime, F("Format: < NTP | dd.mm.yyyy-hh:mm:ss-dw-dst >"))
MAKE_PSTR_LIST(tpl_switchtime, F("Format: < nn.d.o.hh:mm >"))
MAKE_PSTR_LIST(tpl_switchtime1, F("Format: <nn> [ not_set | day hh:mm Tn ]"))
MAKE_PSTR_LIST(tpl_holidays, F("Format: < dd.mm.yyyy-dd.mm.yyyy >"))
MAKE_PSTR_LIST(enum_wwProgMode, F("std Prog"), F_(own_prog))
MAKE_PSTR_LIST(enum_dayOfWeek, F("Mo"), F("Di"), F("Mi"), F("Do"), F("Fr"), F("Sa"), F("So"), F("Alle"))
MAKE_PSTR_LIST(enum_progMode, F("Prog_1"), F("Prog_2"))
MAKE_PSTR_LIST(enum_progMode2,
               F("Eigen_1"),
               F("Familie"),
               F("Morgends"),
               F("Abends"),
               F("Vormittag"),
               F("Nachmittag"),
               F("Mittag"),
               F("Singles"),
               F("Senioren"),
               F("Neu"),
               F("Eigen_2"))
MAKE_PSTR_LIST(enum_progMode3, F("Familie"), F("Morgends"), F("Abends"), F("Vormittag"), F("Nachmittag"), F("Mittag"), F("Singles"), F("Senioren"))
MAKE_PSTR_LIST(enum_progMode4, F("prog_a"), F("prog_b"), F("prog_c"), F("prog_d"), F("prog_e"), F("prog_f"))
MAKE_PSTR_LIST(enum_climate, F("Solltemperature"), F("Raumtemperatur"))

// Boiler
// extra commands
MAKE_PSTR_LIST(wwtapactivated, F("wwtapactivated"), F("Aktiviere Warmwasser im Wartungsmodus"))
MAKE_PSTR_LIST(reset, F("reset"), F("Reset"))

// mqtt, commands and text
MAKE_PSTR_LIST(heatingActive, F("heatingactive"), F("Heizung aktiv"))
MAKE_PSTR_LIST(tapwaterActive, F("tapwateractive"), F("Warmwasser aktiv"))
MAKE_PSTR_LIST(selFlowTemp, F("selflowtemp"), F("Sollwert Flusstemperatur"))
MAKE_PSTR_LIST(selBurnPow, F("selburnpow"), F("Sollwert Brennerleistung"))
MAKE_PSTR_LIST(heatingPumpMod, F("heatingpumpmod"), F("Heizungspumpe 1 Modulation"))
MAKE_PSTR_LIST(heatingPump2Mod, F("heatingpump2mod"), F("Heizungspumpe 2 Modulation"))
MAKE_PSTR_LIST(outdoorTemp, F("outdoortemp"), F("Aussentemperatur"))
MAKE_PSTR_LIST(curFlowTemp, F("curflowtemp"), F("aktuelle Flusstemperatur"))
MAKE_PSTR_LIST(retTemp, F("rettemp"), F("Rücklauftemperatur"))
MAKE_PSTR_LIST(switchTemp, F("switchtemp"), F("Mischer Schalttemperatur"))
MAKE_PSTR_LIST(sysPress, F("syspress"), F("Systemdruck"))
MAKE_PSTR_LIST(boilTemp, F("boiltemp"), F("Kesseltemperatur"))
MAKE_PSTR_LIST(exhaustTemp, F("exhausttemp"), F("Auslasstemperatur"))
MAKE_PSTR_LIST(burnGas, F("burngas"), F("Gas"))
MAKE_PSTR_LIST(burnGas2, F("burngas2"), F("Gas Stufe 2"))
MAKE_PSTR_LIST(flameCurr, F("flamecurr"), F("Flammstrom"))
MAKE_PSTR_LIST(heatingPump, F("heatingpump"), F("Heizungspumpe"))
MAKE_PSTR_LIST(fanWork, F("fanwork"), F("Gebläse"))
MAKE_PSTR_LIST(ignWork, F("ignwork"), F("Zündung"))
MAKE_PSTR_LIST(oilPreHeat, F("oilpreheat"), F("oil preheating"))
MAKE_PSTR_LIST(heatingActivated, F("heatingactivated"), F("Heizen aktiviert"))
MAKE_PSTR_LIST(heatingTemp, F("heatingtemp"), F("Kesseltemperatur"))
MAKE_PSTR_LIST(pumpModMax, F("pumpmodmax"), F("Kesselpumpen Maximalleistung"))
MAKE_PSTR_LIST(pumpModMin, F("pumpmodmin"), F("Kesselpumpen Minmalleistung"))
MAKE_PSTR_LIST(pumpDelay, F("pumpdelay"), F("Pumpennachlauf"))
MAKE_PSTR_LIST(burnMinPeriod, F("burnminperiod"), F("Antipendelzeit"))
MAKE_PSTR_LIST(burnMinPower, F("burnminpower"), F("minimale Brennerleistung"))
MAKE_PSTR_LIST(burnMaxPower, F("burnmaxpower"), F("maximale Brennerleistung"))
MAKE_PSTR_LIST(boilHystOn, F("boilhyston"), F("Hysterese ein temperatur"))
MAKE_PSTR_LIST(boilHystOff, F("boilhystoff"), F("Hysterese aus temperatur"))
MAKE_PSTR_LIST(setFlowTemp, F("setflowtemp"), F("Sollwert Flusstemperatur"))
MAKE_PSTR_LIST(setBurnPow, F("setburnpow"), F("Sollwert Brennerleistung"))
MAKE_PSTR_LIST(curBurnPow, F("curburnpow"), F("Brennerleistung"))
MAKE_PSTR_LIST(burnStarts, F("burnstarts"), F("Brenner # starts"))
MAKE_PSTR_LIST(burnWorkMin, F("burnworkmin"), F("Brenner Laufzeit"))
MAKE_PSTR_LIST(burn2WorkMin, F("burn2workmin"), F("Brenner Stufe 2 Laufzeit"))
MAKE_PSTR_LIST(heatWorkMin, F("heatworkmin"), F("Heizung Laufzeit"))
MAKE_PSTR_LIST(UBAuptime, F("ubauptime"), F("gesamte Laufzeit"))
MAKE_PSTR_LIST(lastCode, F("lastcode"), F("Fehlerspeicher"))
MAKE_PSTR_LIST(serviceCode, F("servicecode"), F("Statusmeldung"))
MAKE_PSTR_LIST(serviceCodeNumber, F("servicecodenumber"), F("Statusmeldungsnummer"))
MAKE_PSTR_LIST(maintenanceMessage, F("maintenancemessage"), F("Wartungsmeldung"))
MAKE_PSTR_LIST(maintenanceDate, F("maintenancedate"), F("Wartungsdatum"))
MAKE_PSTR_LIST(maintenanceType, F_(maintenance), F("Wartungsplan"))
MAKE_PSTR_LIST(maintenanceTime, F("maintenancetime"), F("Wartung in"))
MAKE_PSTR_LIST(emergencyOps, F("emergencyops"), F("emergency operation"))
MAKE_PSTR_LIST(emergencyTemp, F("emergencytemp"), F("emergency temperature"))

// heatpump/compress specific
MAKE_PSTR_LIST(upTimeControl, F("uptimecontrol"), F("Betriebszeit total heizen"))
MAKE_PSTR_LIST(upTimeCompHeating, F("uptimecompheating"), F("Betriebszeit Kompressor heizen"))
MAKE_PSTR_LIST(upTimeCompCooling, F("uptimecompcooling"), F("Betriebszeit Kompressor kühlen"))
MAKE_PSTR_LIST(upTimeCompWw, F("uptimecompww"), F("Betriebszeit Kompressor"))
MAKE_PSTR_LIST(upTimeCompPool, F("uptimecomppool"), F("Betriebszeit Kompressor Pool"))
MAKE_PSTR_LIST(totalCompStarts, F("totalcompstarts"), F("gesamt Kompressor Starts"))
MAKE_PSTR_LIST(heatingStarts, F("heatingstarts"), F("Heizen Starts"))
MAKE_PSTR_LIST(coolingStarts, F("coolingstarts"), F("Kühlen Starts"))
MAKE_PSTR_LIST(poolStarts, F("poolstarts"), F("Pool Starts"))
MAKE_PSTR_LIST(nrgConsTotal, F("nrgconstotal"), F("totaler Energieverbrauch"))
MAKE_PSTR_LIST(nrgConsCompTotal, F("nrgconscomptotal"), F("Energieverbrauch Kompressor total"))
MAKE_PSTR_LIST(nrgConsCompHeating, F("nrgconscompheating"), F("Energieverbrauch Kompressor heizen"))
MAKE_PSTR_LIST(nrgConsCompWw, F("nrgconscompww"), F("Energieverbrauch Kompressor"))
MAKE_PSTR_LIST(nrgConsCompCooling, F("nrgconscompcooling"), F("Energieverbrauch Kompressor kühlen"))
MAKE_PSTR_LIST(nrgConsCompPool, F("nrgconscomppool"), F("Energieverbrauch Kompressor Pool"))
MAKE_PSTR_LIST(nrgSuppTotal, F("nrgsupptotal"), F("gesamte Energieabgabe"))
MAKE_PSTR_LIST(nrgSuppHeating, F("nrgsuppheating"), F("gesamte Energieabgabe heizen"))
MAKE_PSTR_LIST(nrgSuppWw, F("nrgsuppww"), F("gesamte Energieabgabe"))
MAKE_PSTR_LIST(nrgSuppCooling, F("nrgsuppcooling"), F("gesamte Energieabgabe kühlen"))
MAKE_PSTR_LIST(nrgSuppPool, F("nrgsupppool"), F("gesamte Energieabgabe Pool"))
MAKE_PSTR_LIST(auxElecHeatNrgConsTotal, F("auxelecheatnrgconstotal"), F("Energieverbrauch el. Zusatzheizung"))
MAKE_PSTR_LIST(auxElecHeatNrgConsHeating, F("auxelecheatnrgconsheating"), F("Energieverbrauch el. Zusatzheizung Heizen"))
MAKE_PSTR_LIST(auxElecHeatNrgConsWW, F("auxelecheatnrgconsww"), F("Energieverbrauch el. Zusatzheizung"))
MAKE_PSTR_LIST(auxElecHeatNrgConsPool, F("auxelecheatnrgconspool"), F("Energieverbrauch el. Zusatzheizung Pool"))

MAKE_PSTR_LIST(hpPower, F("hppower"), F("Leistung Wärmepumpe"))
MAKE_PSTR_LIST(hpCompOn, F("hpcompon"), F("HP Compressor"))
MAKE_PSTR_LIST(hpHeatingOn, F("hpheatingon"), F("HP Heating"))
MAKE_PSTR_LIST(hpCoolingOn, F("hpcoolingon"), F("HP Cooling"))
MAKE_PSTR_LIST(hpWwOn, F("hpwwon"), F("HP dhw"))
MAKE_PSTR_LIST(hpPoolOn, F("hppoolon"), F("HP Pool"))
MAKE_PSTR_LIST(hpBrinePumpSpd, F("hpbrinepumpspd"), F("Brine Pump Speed"))
MAKE_PSTR_LIST(hpCompSpd, F("hpcompspd"), F("Compressor Speed"))
MAKE_PSTR_LIST(hpCircSpd, F("hpcircspd"), F("Circulation pump Speed"))
MAKE_PSTR_LIST(hpBrineIn, F("hpbrinein"), F("Brine in/Evaporator"))
MAKE_PSTR_LIST(hpBrineOut, F("hpbrineout"), F("Brine out/Condenser"))
MAKE_PSTR_LIST(hpSuctionGas, F("hpsuctiongas"), F("Suction gas"))
MAKE_PSTR_LIST(hpHotGas, F("hphotgas"), F("Hot gas/Compressed"))
MAKE_PSTR_LIST(hpSwitchValve, F("hpswitchvalve"), F("Switch Valve"))
MAKE_PSTR_LIST(hpActivity, F("hpactivity"), F("Compressor Activity"))
MAKE_PSTR_LIST(hpTc0, F("hptc0"), F("Wärmeträgerflüssigkeit Eingang (TC0)"))
MAKE_PSTR_LIST(hpTc1, F("hptc1"), F("Wärmeträgerflüssigkeit Ausgang (TC1)"))
MAKE_PSTR_LIST(hpTc3, F("hptc3"), F("Verflüssigertemperatur (TC3)"))
MAKE_PSTR_LIST(hpTr3, F("hptr3"), F(" Temperaturfühler Kältemittel (Flüssigkeit)  (TR3)"))
MAKE_PSTR_LIST(hpTr4, F("hptr4"), F("Verdampfer Eintritt (TR4)"))
MAKE_PSTR_LIST(hpTr5, F("hptr5"), F("Temperaturfühler Kompessoransaugleitung (TR5)"))
MAKE_PSTR_LIST(hpTr6, F("hptr6"), F("Temperaturfühler Kompressorausgangsleitung (TR6)"))
MAKE_PSTR_LIST(hpTr7, F("hptr7"), F("Temperaturfühler Kältemittel (Gas) (TR7)"))
MAKE_PSTR_LIST(hpTl2, F("hptl2"), F("Außenlufttemperaturfühler (TL2)"))
MAKE_PSTR_LIST(hpPl1, F("hppl1"), F("Niedrigdruckfühler (PL1)"))
MAKE_PSTR_LIST(hpPh1, F("hpph1"), F("Hochdruckfühler (PH1)"))

// hybrid heatpump
MAKE_PSTR_LIST(enum_hybridStrategy, F("co2-optimized"), F("cost-optimized"), F("outside-temp-switched"), F("co2-cost-mix"))
MAKE_PSTR_LIST(hybridStrategy, F("hybridstrategy"), F("hybrid control strategy"))
MAKE_PSTR_LIST(switchOverTemp, F("switchovertemp"), F("outside switchover temperature"))
MAKE_PSTR_LIST(energyCostRatio, F("energycostratio"), F("energy cost ratio"))
MAKE_PSTR_LIST(fossileFactor, F("fossilefactor"), F("fossile energy factor"))
MAKE_PSTR_LIST(electricFactor, F("electricfactor"), F("electric energy factor"))
MAKE_PSTR_LIST(delayBoiler, F("delayboiler"), F("delay boiler support"))
MAKE_PSTR_LIST(tempDiffBoiler, F("tempdiffboiler"), F("tempediff boiler support"))

// alternative heatsource AM200
MAKE_PSTR_LIST(aCylTopTemp, F("cyltoptemp"), F("cylinder top temperature"))
MAKE_PSTR_LIST(aCylCenterTemp, F("cylcentertemp"), F("cylinder center temperature"))
MAKE_PSTR_LIST(aCylBottomTemp, F("cylbottomtemp"), F("cylinder bottom temperature"))
MAKE_PSTR_LIST(aFlowTemp, F("altflowtemp"), F("alternative hs flow temperature"))
MAKE_PSTR_LIST(aRetTemp, F("altrettemp"), F("alternative hs return temperature"))
MAKE_PSTR_LIST(sysFlowTemp, F("sysflowtemp"), F("system flow temperature"))
MAKE_PSTR_LIST(sysRetTemp, F("sysrettemp"), F("system return temperature"))
MAKE_PSTR_LIST(valveByPass, F("valvebypass"), F("bypass valve"))
MAKE_PSTR_LIST(valveBuffer, F("valvebuffer"), F("buffer valve"))
MAKE_PSTR_LIST(valveReturn, F("valvereturn"), F("return valve"))
MAKE_PSTR_LIST(aPump, F("altpump"), F("alternative hs pump"))
MAKE_PSTR_LIST(heatSource, F("heatsource"), F("alternative heating active"))

// the following are dhw for the boiler and automatically tagged with 'ww'
MAKE_PSTR_LIST(wwSelTemp, F("wwseltemp"), F("gewählte Temperatur"))
MAKE_PSTR_LIST(wwSelTempLow, F("wwseltemplow"), F("selected lower temperature"))
MAKE_PSTR_LIST(wwSelTempOff, F("wwseltempoff"), F("selected temperature for off"))
MAKE_PSTR_LIST(wwSelTempSingle, F("wwseltempsingle"), F("single charge temperature"))
MAKE_PSTR_LIST(wwSetTemp, F("wwsettemp"), F("Solltemperatur"))
MAKE_PSTR_LIST(wwType, F("wwtype"), F("Typ"))
MAKE_PSTR_LIST(wwComfort, F("wwcomfort"), F("Komfort"))
MAKE_PSTR_LIST(wwComfort1, F("wwcomfort1"), F("Komfort mode"))
MAKE_PSTR_LIST(wwFlowTempOffset, F("wwflowtempoffset"), F("Flusstemperaturanhebung"))
MAKE_PSTR_LIST(wwMaxPower, F("wwmaxpower"), F("max Leistung"))
MAKE_PSTR_LIST(wwCircPump, F("wwcircpump"), F("Zirkulationspumpe vorhanden"))
MAKE_PSTR_LIST(wwChargeType, F("wwchargetype"), F("Ladungstyp"))
MAKE_PSTR_LIST(wwDisinfectionTemp, F("wwdisinfectiontemp"), F("Desinfectionstemperatur"))
MAKE_PSTR_LIST(wwCircMode, F("wwcircmode"), F("Zirkulationspumpenfrequenz"))
MAKE_PSTR_LIST(wwCirc, F("wwcirc"), F("Zirkulation aktiv"))
MAKE_PSTR_LIST(wwCurTemp, F("wwcurtemp"), F("aktuelle Warmwasser Temperatur intern"))
MAKE_PSTR_LIST(wwCurTemp2, F("wwcurtemp2"), F("aktuelle Warmwaser Temperatur extern"))
MAKE_PSTR_LIST(wwCurFlow, F("wwcurflow"), F("aktueller Durchfluss"))
MAKE_PSTR_LIST(wwStorageTemp1, F("wwstoragetemp1"), F("interne Speichertemperature"))
MAKE_PSTR_LIST(wwStorageTemp2, F("wwstoragetemp2"), F("externer Speichertemperatur"))
MAKE_PSTR_LIST(wwActivated, F("wwactivated"), F("aktiviert"))
MAKE_PSTR_LIST(wwOneTime, F("wwonetime"), F("Einmalladung"))
MAKE_PSTR_LIST(wwDisinfecting, F("wwdisinfect"), F("Desinfizieren"))
MAKE_PSTR_LIST(wwCharging, F("wwcharging"), F("Laden"))
MAKE_PSTR_LIST(wwChargeOptimization, F("wwchargeoptimization"), F("charge optimization"))
MAKE_PSTR_LIST(wwRecharging, F("wwrecharging"), F("Nachladen"))
MAKE_PSTR_LIST(wwTempOK, F("wwtempok"), F("Temperatur ok"))
MAKE_PSTR_LIST(wwActive, F("wwactive"), F("aktiv"))
MAKE_PSTR_LIST(ww3wayValve, F("ww3wayvalve"), F("3-Wegeventil aktiv"))
MAKE_PSTR_LIST(wwSetPumpPower, F("wwsetpumppower"), F("Soll Pumpenleistung"))
MAKE_PSTR_LIST(wwMixerTemp, F("wwmixertemp"), F("Mischertemperatur"))
MAKE_PSTR_LIST(wwCylMiddleTemp, F("wwcylmiddletemp"), F("cylinder middle temperature (TS3)"))
MAKE_PSTR_LIST(wwStarts, F("wwstarts"), F("Anzahl starts"))
MAKE_PSTR_LIST(wwStarts2, F("wwstarts2"), F("Kreis 2 Anzahl Starts"))
MAKE_PSTR_LIST(wwWorkM, F("wwworkm"), F("aktive Zeit"))
MAKE_PSTR_LIST(wwHystOn, F("wwhyston"), F("Hysterese Einschalttemperatur"))
MAKE_PSTR_LIST(wwHystOff, F("wwhystoff"), F("Hysterese Ausschalttemperatur"))
MAKE_PSTR_LIST(wwProgMode, F("wwprogmode"), F("Programmmodus"))
MAKE_PSTR_LIST(wwCircProg, F("wwcircprog"), F("Zirkulationsprogramm"))
MAKE_PSTR_LIST(wwMaxTemp, F("wwmaxtemp"), F("Maximale Temperatur"))
MAKE_PSTR_LIST(wwOneTimeKey, F("wwonetimekey"), F("Einmalladungstaste"))

// mqtt values / commands
MAKE_PSTR_LIST(switchtime, F("switchtime"), F("Program Schaltzeit"))
MAKE_PSTR_LIST(switchtime1, F("switchtime1"), F("Program 1 Schaltzeit"))
MAKE_PSTR_LIST(switchtime2, F("switchtime2"), F("Programm 2 Schaltzeit"))
MAKE_PSTR_LIST(wwswitchtime, F("wwswitchtime"), F("Programm Schaltzeit"))
MAKE_PSTR_LIST(wwcircswitchtime, F("wwcircswitchtime"), F("Zirculationsprogramm Schaltzeit"))
MAKE_PSTR_LIST(dateTime, F("datetime"), F("Datum/Zeit"))
MAKE_PSTR_LIST(errorCode, F("errorcode"), F("Fehlermeldung"))
MAKE_PSTR_LIST(ibaMainDisplay, F("display"), F("Anzeige"))
MAKE_PSTR_LIST(ibaLanguage, F("language"), F("Sprache"))
MAKE_PSTR_LIST(ibaClockOffset, F("clockoffset"), F("Uhrkorrektur"))
MAKE_PSTR_LIST(ibaBuildingType, F("building"), F("Gebäude"))
MAKE_PSTR_LIST(heatingPID, F("heatingpid"), F("heating PID"))
MAKE_PSTR_LIST(ibaCalIntTemperature, F("intoffset"), F("Korrektur interner Temperatur"))
MAKE_PSTR_LIST(ibaMinExtTemperature, F("minexttemp"), F("min Aussentemperatur"))
MAKE_PSTR_LIST(backlight, F("backlight"), F("key backlight"))
MAKE_PSTR_LIST(damping, F("damping"), F("Dämpfung der Außentemperatur"))
MAKE_PSTR_LIST(tempsensor1, F("inttemp1"), F("Temperatursensor 1"))
MAKE_PSTR_LIST(tempsensor2, F("inttemp2"), F("Temperatursensor 2"))
MAKE_PSTR_LIST(dampedoutdoortemp, F("dampedoutdoortemp"), F("gedämpfte Aussentemperatur"))
MAKE_PSTR_LIST(floordrystatus, F("floordry"), F("Estrichtrocknung"))
MAKE_PSTR_LIST(floordrytemp, F("floordrytemp"), F("Estrichtrocknungs Temperatur"))
MAKE_PSTR_LIST(brightness, F("brightness"), F("bildschirmhelligkeit"))
MAKE_PSTR_LIST(autodst, F("autodst"), F("automatische sommerzeit umstellung"))
MAKE_PSTR_LIST(preheating, F("preheating"), F("vorheizen im uhrenprogramm"))
MAKE_PSTR_LIST(offtemp, F("offtemp"), F("temperatur bei ausgeschaltetem modus"))
MAKE_PSTR_LIST(mixingvalves, F("mixingvalves"), F("mischventile"))
// thermostat ww
MAKE_PSTR_LIST(wwMode, F("wwmode"), F("modus"))
MAKE_PSTR_LIST(wwSetTempLow, F("wwsettemplow"), F("untere Solltemperatur"))
MAKE_PSTR_LIST(wwCharge, F("wwcharge"), F("charge"))
MAKE_PSTR_LIST(wwChargeDuration, F("wwchargeduration"), F("charge duration"))
MAKE_PSTR_LIST(wwDisinfect, F("wwdisinfect"), F("disinfection"))
MAKE_PSTR_LIST(wwDisinfectDay, F("wwdisinfectday"), F("disinfection day"))
MAKE_PSTR_LIST(wwDisinfectHour, F("wwdisinfecthour"), F("disinfection hour"))
MAKE_PSTR_LIST(wwDisinfectTime, F("wwdisinfecttime"), F("disinfection time"))
MAKE_PSTR_LIST(wwExtra1, F("wwextra1"), F("Kreis 1 Extra"))
MAKE_PSTR_LIST(wwExtra2, F("wwextra2"), F("Kreis 2 Extra"))
MAKE_PSTR_LIST(wwDailyHeating, F("wwdailyheating"), F("daily heating"))
MAKE_PSTR_LIST(wwDailyHeatTime, F("wwdailyheattime"), F("daily heating time"))
MAKE_PSTR_LIST(wwWhenModeOff, F("wwwhenmodeoff"), F("wenn Thermostatmodus ist aus"))
// thermostat hc
MAKE_PSTR_LIST(climate, F("climate"))
MAKE_PSTR_LIST(selRoomTemp, F("seltemp"), F("Sollwert Raumtemperatur"))
MAKE_PSTR_LIST(roomTemp, F("currtemp"), F("aktuelle Raumtemperatur"))
MAKE_PSTR_LIST(mode, F("mode"), F("modus"))
MAKE_PSTR_LIST(modetype, F("modetype"), F("modus Typ"))
MAKE_PSTR_LIST(fastheatup, F("fastheatup"), F("fast heatup"))
MAKE_PSTR_LIST(daytemp, F("daytemp"), F("Tagestemperatur"))
MAKE_PSTR_LIST(daylowtemp, F("daytemp2"), F("Tagestemperatur T2"))
MAKE_PSTR_LIST(daymidtemp, F("daytemp3"), F("Tagestemperatur T3"))
MAKE_PSTR_LIST(dayhightemp, F("daytemp4"), F("Tagestemperatur T4"))
MAKE_PSTR_LIST(heattemp, F("heattemp"), F("Heizen Temperatur"))
MAKE_PSTR_LIST(nighttemp, F("nighttemp"), F("Nachttemperatur"))
MAKE_PSTR_LIST(nighttemp2, F("nighttemp"), F("Nachttemperatur T1"))
MAKE_PSTR_LIST(ecotemp, F("ecotemp"), F("eco Temperatur"))
MAKE_PSTR_LIST(manualtemp, F("manualtemp"), F("manuelle Temperatur"))
MAKE_PSTR_LIST(tempautotemp, F("tempautotemp"), F("zwischenzeitliche Solltemperatur"))
MAKE_PSTR_LIST(comforttemp, F("comforttemp"), F("Komforttemperatur"))
MAKE_PSTR_LIST(summertemp, F("summertemp"), F("Sommertemperatur"))
MAKE_PSTR_LIST(designtemp, F("designtemp"), F("design-Temperatur"))
MAKE_PSTR_LIST(offsettemp, F("offsettemp"), F("Temperaturanhebung"))
MAKE_PSTR_LIST(minflowtemp, F("minflowtemp"), F("min Flusstemperatur"))
MAKE_PSTR_LIST(maxflowtemp, F("maxflowtemp"), F("max Flusstemperatur"))
MAKE_PSTR_LIST(roominfluence, F("roominfluence"), F("Raumeinfluss"))
MAKE_PSTR_LIST(roominfl_factor, F("roominflfactor"), F("Raumeinfluss Factor"))
MAKE_PSTR_LIST(curroominfl, F("curroominfl"), F("aktueller Raumeinfluss"))
MAKE_PSTR_LIST(nofrosttemp, F("nofrosttemp"), F("Frostschutztemperatur"))
MAKE_PSTR_LIST(targetflowtemp, F("targetflowtemp"), F("berechnete Flusstemperatur"))
MAKE_PSTR_LIST(heatingtype, F("heatingtype"), F("Heizungstyp"))
MAKE_PSTR_LIST(summersetmode, F("summersetmode"), F("Einstellung Sommerbetrieb"))
MAKE_PSTR_LIST(hpoperatingmode, F("hpoperatingmode"), F("Wärmepumpe Betriebsmodus"))
MAKE_PSTR_LIST(hpoperatingstate, F("hpoperatingstate"), F("heatpump operating state"))
MAKE_PSTR_LIST(controlmode, F("controlmode"), F("Kontrollmodus"))
MAKE_PSTR_LIST(control, F("control"), F("Fernsteuerung"))
MAKE_PSTR_LIST(wwHolidays, F("wwholidays"), F("holiday dates"))
MAKE_PSTR_LIST(wwVacations, F("wwvacations"), F("vacation dates"))
MAKE_PSTR_LIST(holidays, F("holidays"), F("holiday dates"))
MAKE_PSTR_LIST(vacations, F("vacations"), F("vacation dates"))
MAKE_PSTR_LIST(program, F("program"), F("Programm"))
MAKE_PSTR_LIST(pause, F("pause"), F("Pausenzeit"))
MAKE_PSTR_LIST(party, F("party"), F("Partyzeit"))
MAKE_PSTR_LIST(wwprio, F("wwprio"), F("dhw priority"))
MAKE_PSTR_LIST(holidaytemp, F("holidaytemp"), F("Urlaubstemperatur"))
MAKE_PSTR_LIST(summermode, F("summermode"), F("Sommerbetrieb"))
MAKE_PSTR_LIST(holidaymode, F("holidaymode"), F("Urlaubsbetrieb"))
MAKE_PSTR_LIST(flowtempoffset, F("flowtempoffset"), F("Flusstemperaturanhebung"))
MAKE_PSTR_LIST(reducemode, F("reducemode"), F("Absenkmodus"))
MAKE_PSTR_LIST(noreducetemp, F("noreducetemp"), F("Absenkung unterbrechen unter Temperatur"))
MAKE_PSTR_LIST(reducetemp, F("reducetemp"), F("Absenkmodus unter Temperatur"))
MAKE_PSTR_LIST(vacreducetemp, F("vacreducetemp"), F("Urlaub Absenkmodus unter Temperatur"))
MAKE_PSTR_LIST(vacreducemode, F("vacreducemode"), F("Urlaub Absenkmodus"))
MAKE_PSTR_LIST(nofrostmode, F("nofrostmode"), F("Frostschutz Modus"))
MAKE_PSTR_LIST(nofrostmode1, F("nofrostmode1"), F("nofrost mode")) // RC310
MAKE_PSTR_LIST(remotetemp, F("remotetemp"), F("Raumtemperatur der Fernsteuerung"))
MAKE_PSTR_LIST(reducehours, F("reducehours"), F("duration for nighttemp"))
MAKE_PSTR_LIST(reduceminutes, F("reduceminutes"), F("remaining time for nightmode"))
MAKE_PSTR_LIST(switchonoptimization, F("switchonoptimization"), F("switch-on optimization"))

// heatpump
MAKE_PSTR_LIST(airHumidity, F("airhumidity"), F("relative Luftfeuchte"))
MAKE_PSTR_LIST(dewTemperature, F("dewtemperature"), F("Taupunkttemperatur"))

// mixer
MAKE_PSTR_LIST(flowSetTemp, F("flowsettemp"), F("Sollwert Flusstemperatur"))
MAKE_PSTR_LIST(flowTempHc, F("flowtemphc"), F("Flusstemperatur des hk (TC1)"))
MAKE_PSTR_LIST(pumpStatus, F("pumpstatus"), F("Pumpenstatus des hk (PC1)"))
MAKE_PSTR_LIST(mixerStatus, F("valvestatus"), F("Mischerventil Position (VC1)"))
MAKE_PSTR_LIST(flowTempVf, F("flowtempvf"), F("Flusstemperatur am Kessel (T0/Vf)"))
MAKE_PSTR_LIST(mixerSetTime, F("valvesettime"), F("time to set valve"))
// mixer prefixed with wwc
MAKE_PSTR_LIST(wwPumpStatus, F("pumpstatus"), F("Pumpenstatus des wwk (PC1)"))
MAKE_PSTR_LIST(wwTempStatus, F("tempstatus"), F("Temperaturschalter des wwk (MC1)"))
MAKE_PSTR_LIST(wwTemp, F("wwtemp"), F("aktuelle Temperatur"))
// mixer pool
MAKE_PSTR_LIST(poolSetTemp, F("poolsettemp"), F("pool set temperature"))
MAKE_PSTR_LIST(poolTemp, F("pooltemp"), F("pool temperature"))
MAKE_PSTR_LIST(poolShuntStatus, F("poolshuntstatus"), F("pool shunt status opening/closing"))
MAKE_PSTR_LIST(poolShunt, F("poolshunt"), F("pool shunt open/close (0% = pool / 100% = heat)"))
MAKE_PSTR_LIST(hydrTemp, F("hydrTemp"), F("hydraulic header temperature"))

// solar
MAKE_PSTR_LIST(collectorTemp, F("collectortemp"), F("Kollektortemperatur (TS1)"))
MAKE_PSTR_LIST(collector2Temp, F("collector2temp"), F("collector 2 temperature (TS7)"))
MAKE_PSTR_LIST(cylBottomTemp, F("cylbottomtemp"), F("Speicher Bodentemperatur (TS2)"))
MAKE_PSTR_LIST(cyl2BottomTemp, F("cyl2bottomtemp"), F("2. Speicher Bodentemperatur (TS5)"))
MAKE_PSTR_LIST(heatExchangerTemp, F("heatexchangertemp"), F("wärmetauscher Temperatur (TS6)"))
MAKE_PSTR_LIST(cylMiddleTemp, F("cylmiddletemp"), F("cylinder middle temperature (TS3)"))
MAKE_PSTR_LIST(retHeatAssist, F("retheatassist"), F("return temperature heat assistance (TS4)"))
// correct name for M1? value not found, try this:
MAKE_PSTR_LIST(m1Valve, F("heatassistvalve"), F("heat assistance valve (M1)"))
MAKE_PSTR_LIST(m1Power, F("heatassistpower"), F("heat assistance valve power (M1)"))
MAKE_PSTR_LIST(collectorMaxTemp, F("collectormaxtemp"), F("maximale Kollektortemperatur"))
MAKE_PSTR_LIST(collectorMinTemp, F("collectormintemp"), F("minimale Kollektortemperatur"))
MAKE_PSTR_LIST(cylMaxTemp, F("cylmaxtemp"), F("maximale Speichertemperatur"))
// MAKE_PSTR_LIST(cyl2MaxTemp, F("cyl2maxtemp"), F("maximum cylinder 2 temperature"))
MAKE_PSTR_LIST(solarPumpMod, F("solarpumpmod"), F("Pumpenmodulation (PS1)"))
MAKE_PSTR_LIST(cylPumpMod, F("cylpumpmod"), F("Speicherpumpenmodulation (PS5)"))
MAKE_PSTR_LIST(solarPump, F("solarpump"), F("Pumpe (PS1)"))
MAKE_PSTR_LIST(solarPump2, F("solarpump2"), F("pump 2 (PS4)"))
MAKE_PSTR_LIST(solarPump2Mod, F("solarpump2mod"), F("pump 2 modulation (PS4)"))
MAKE_PSTR_LIST(valveStatus, F("valvestatus"), F("ventilstatus"))
MAKE_PSTR_LIST(cylHeated, F("cylheated"), F("Speichertemperatur erreicht"))
MAKE_PSTR_LIST(collectorShutdown, F("collectorshutdown"), F("Kollektorabschaltung"))
MAKE_PSTR_LIST(pumpWorkTime, F("pumpworktime"), F("Pumpenlaufzeit"))
MAKE_PSTR_LIST(pump2WorkTime, F("pump2worktime"), F("Pumpe 2 Laufzeit"))
MAKE_PSTR_LIST(m1WorkTime, F("m1worktime"), F("Differenzregelung Arbeitszeit"))
MAKE_PSTR_LIST(energyLastHour, F("energylasthour"), F("Energie letzte Std"))
MAKE_PSTR_LIST(energyTotal, F("energytotal"), F("Gesamtenergie"))
MAKE_PSTR_LIST(energyToday, F("energytoday"), F("Energie heute"))
MAKE_PSTR_LIST(pumpMinMod, F("pumpminmod"), F("minimum pump modulation"))
MAKE_PSTR_LIST(maxFlow, F("maxflow"), F("maximum solar flow"))
MAKE_PSTR_LIST(solarPower, F("solarpower"), F("actual solar power"))
MAKE_PSTR_LIST(solarPumpTurnonDiff, F("turnondiff"), F("pump turn on difference"))
MAKE_PSTR_LIST(solarPumpTurnoffDiff, F("turnoffdiff"), F("pump turn off difference"))
MAKE_PSTR_LIST(pump2MinMod, F("pump2minmod"), F("minimum pump 2 modulation"))
MAKE_PSTR_LIST(solarPump2TurnonDiff, F("turnondiff2"), F("pump 2 turn on difference"))
MAKE_PSTR_LIST(solarPump2TurnoffDiff, F("turnoffdiff2"), F("pump 2 turn off difference"))
MAKE_PSTR_LIST(enum_solarmode, F_(constant), F("pwm"), F("analog"))
MAKE_PSTR_LIST(enum_collectortype, F("flach"), F("vakuum"))
MAKE_PSTR_LIST(enum_cylprio, F("Zyl_1"), F("Zyl_2"))

// solar ww
MAKE_PSTR_LIST(wwTemp1, F("wwtemp1"), F("Temperatur 1"))
MAKE_PSTR_LIST(wwTemp3, F("wwtemp3"), F("Temperatur 3"))
MAKE_PSTR_LIST(wwTemp4, F("wwtemp4"), F("Temperatur 4"))
MAKE_PSTR_LIST(wwTemp5, F("wwtemp5"), F("Temperatur 5"))
MAKE_PSTR_LIST(wwTemp7, F("wwtemp7"), F("Temperatur 7"))
MAKE_PSTR_LIST(wwPump, F("wwpump"), F("Pumpe"))
// solar ww and mixer wwc
MAKE_PSTR_LIST(wwMinTemp, F("wwmintemp"), F("minimale Temperatur"))
MAKE_PSTR_LIST(wwRedTemp, F("wwredtemp"), F("redizierte Temperatur"))
MAKE_PSTR_LIST(wwDailyTemp, F("wwdailytemp"), F("tägl. Temperatur"))
MAKE_PSTR_LIST(wwKeepWarm, F("wwkeepwarm"), F("Warmhalten"))
MAKE_PSTR_LIST(wwStatus2, F("wwstatus2"), F("Status 2"))
MAKE_PSTR_LIST(enum_wwStatus2, F(""), F(""), F(""), F("no_heat"), F(""), F(""), F("heatrequest"), F(""), F("disinfecting"), F("hold"))
MAKE_PSTR_LIST(wwPumpMod, F("wwpumpmod"), F("Pumpen Modulation"))
MAKE_PSTR_LIST(wwFlow, F("wwflow"), F("Flussrate"))
// extra mixer ww
MAKE_PSTR_LIST(wwRequiredTemp, F("wwrequiredtemp"), F("benötigte Temperatur"))
MAKE_PSTR_LIST(wwDiffTemp, F("wwdifftemp"), F("Start Differential Temperatur"))

// SM100
MAKE_PSTR_LIST(heatTransferSystem, F("heattransfersystem"), F("Wärmetransfer System"))
MAKE_PSTR_LIST(externalCyl, F("externalcyl"), F("Externer Speicher"))
MAKE_PSTR_LIST(thermalDisinfect, F("thermaldisinfect"), F("Thermische Desinfektion"))
MAKE_PSTR_LIST(heatMetering, F("heatmetering"), F("Wärmemessung"))
MAKE_PSTR_LIST(solarIsEnabled, F("solarenabled"), F("Solarmodul aktiviert"))

// telegram 0x035A
MAKE_PSTR_LIST(solarPumpMode, F("solarpumpmode"), F("solar Pumpen Einst."))
MAKE_PSTR_LIST(solarPumpKick, F("pumpkick"), F("pumpkick"))
MAKE_PSTR_LIST(plainWaterMode, F("plainwatermode"), F("plain water mode"))
MAKE_PSTR_LIST(doubleMatchFlow, F("doublematchflow"), F("doublematchflow"))
MAKE_PSTR_LIST(solarPump2Mode, F("pump2mode"), F("pump 2 mode"))
MAKE_PSTR_LIST(solarPump2Kick, F("pump2kick"), F("pumpkick 2"))

// telegram 0x035F
MAKE_PSTR_LIST(cylPriority, F("cylpriority"), F("Speicher Priorität"))

// telegram 0x380
MAKE_PSTR_LIST(climateZone, F("climatezone"), F("climate zone"))
MAKE_PSTR_LIST(collector1Area, F("collector1area"), F("Kollektor 1 Fläche"))
MAKE_PSTR_LIST(collector1Type, F("collector1type"), F("Kollektor 1 Type"))
MAKE_PSTR_LIST(collector2Area, F("collector2area"), F("Kollektor 2 Fläche"))
MAKE_PSTR_LIST(collector2Type, F("collector2type"), F("Kollektor 2 Type"))

// telegram 0x0363 heatCounter
MAKE_PSTR_LIST(heatCntFlowTemp, F("heatcntflowtemp"), F("Wärmezähler Fluss-Temperatur"))
MAKE_PSTR_LIST(heatCntRetTemp, F("heatcntrettemp"), F("Wärmezähler Rückfluss-Temperatur"))
MAKE_PSTR_LIST(heatCnt, F("heatcnt"), F("Wärmezäler Impulse"))
MAKE_PSTR_LIST(swapFlowTemp, F("swapflowtemp"), F("Austausch Fluss-Temperatur (TS14)"))
MAKE_PSTR_LIST(swapRetTemp, F("swaprettemp"), F("Austausch Rückfluss-Temperatur (TS15)"))

// switch
MAKE_PSTR_LIST(activated, F("activated"), F("aktiviert"))
MAKE_PSTR_LIST(status, F("status"), F("Status"))

// unknown fields to track (SM10)
MAKE_PSTR_LIST(data11, F("data11"), F("unknown datafield 11"))
MAKE_PSTR_LIST(data12, F("data12"), F("unknown datafield 12"))
MAKE_PSTR_LIST(data8, F("data8"), F("unknown datafield 8"))
MAKE_PSTR_LIST(data0, F("data0"), F("unknown datafield 0"))
MAKE_PSTR_LIST(data1, F("data1"), F("unknown datafield 1"))
MAKE_PSTR_LIST(setting3, F("setting3"), F("unknown setting 3"))
MAKE_PSTR_LIST(setting4, F("setting4"), F("unknown setting 4"))

// RF sensor, id 0x40, telegram 0x435
MAKE_PSTR_LIST(RFTemp, F("rftemp"), F("RF Raumtemperatur Sensor"));
