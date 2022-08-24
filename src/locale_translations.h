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

// Define languages here
// Makes sure they are also added in same order to languages[] in system.cpp
#define EMSESP_LOCALE_EN "en"
#define EMSESP_LOCALE_DE "de"

// translations are in order en, de
// if there is no translation, it will default to en

// General
MAKE_PSTR_LIST(on, F("on"), F("an"))
MAKE_PSTR_LIST(off, F("off"), F("aus"))
MAKE_PSTR_LIST(ON, F("ON"), F("AN"))
MAKE_PSTR_LIST(OFF, F("OFF"), F("AUS"))

// Unit Of Measurement mapping - maps to DeviceValueUOM_s in emsdevice.cpp
// uom - also used with HA see https://github.com/home-assistant/core/blob/d7ac4bd65379e11461c7ce0893d3533d8d8b8cbf/homeassistant/const.py#L384
MAKE_PSTR_LIST(minutes, F("minutes"), F("Minuten"))
MAKE_PSTR_LIST(hours, F("hours"), F("Stunden"))
MAKE_PSTR_LIST(days, F("days"), F("Tage"))
MAKE_PSTR_LIST(seconds, F("seconds"), F("Sekunden"))

// general
MAKE_PSTR_LIST(day_mo, F("mo"), F("Mo"))
MAKE_PSTR_LIST(day_tu, F("tu"), F("Di"))
MAKE_PSTR_LIST(day_we, F("we"), F("Mi"))
MAKE_PSTR_LIST(day_th, F("th"), F("Do"))
MAKE_PSTR_LIST(day_fr, F("fr"), F("Fr"))
MAKE_PSTR_LIST(day_sa, F("sa"), F("Sa"))
MAKE_PSTR_LIST(day_su, F("su"), F("So"))
MAKE_PSTR_LIST(all, F("all"), F("Alle"))
MAKE_PSTR_LIST(own_1, F("own 1"), F("Eigen 1"))
MAKE_PSTR_LIST(family, F("family"), F("Familie"))
MAKE_PSTR_LIST(morning, F("morning"), F("Morgends"))
MAKE_PSTR_LIST(evening, F("evening"), F("Abends"))
MAKE_PSTR_LIST(seniors, F("seniors"), F("Senioren"))
MAKE_PSTR_LIST(no, F("no"), F("nein"))
MAKE_PSTR_LIST(new, F("new"), F("Neu"))
MAKE_PSTR_LIST(own_2, F("own 2"), F("Eigen 2"))
MAKE_PSTR_LIST(singles, F("singles"), F("Singles"))
MAKE_PSTR_LIST(am, F("am"), F("Vormittag"))
MAKE_PSTR_LIST(pm, F("pm"), F("Nachmittag"))
MAKE_PSTR_LIST(midday, F("midday"), F("Mittag"))
MAKE_PSTR_LIST(unknown, F("unknown"), F("Unbekannt"))
MAKE_PSTR_LIST(flat, F("flat"), F("flach"))
MAKE_PSTR_LIST(vacuum, F("vacuum"), F("vakuum"))
MAKE_PSTR_LIST(co2_optimized, F("co2 optimized"), F("CO2 optimiert"))
MAKE_PSTR_LIST(cost_optimized, F("cost optimized"), F("kostenoptimiert"))
MAKE_PSTR_LIST(outside_temp_switched, F("outside temp switched"), F("Außentemp geschaltet"))
MAKE_PSTR_LIST(co2_cost_mix, F("co2 cost mix"), F("Kostenmix"))
MAKE_PSTR_LIST(analog, F("analog"), F("analog"))
MAKE_PSTR_LIST(normal, F("normal"), F("normal"))
MAKE_PSTR_LIST(blocking, F("blocking"), F("Blockierung"))

// boiler
MAKE_PSTR_LIST(time, F("time"), F("Zeit"))
MAKE_PSTR_LIST(date, F("date"), F("Datum"))
MAKE_PSTR_LIST(continuous, F("continuous"), F("kontinuierlich"))
MAKE_PSTR_LIST(3wayvalve, F("3-way valve"), F("3-Wege Ventil"))
MAKE_PSTR_LIST(chargepump, F("chargepump"), F("Ladepumpe"))
MAKE_PSTR_LIST(hot, F("hot"), F("Heiss"))
MAKE_PSTR_LIST(high_comfort, F("high comfort"), F("Heiss Komfort"))
MAKE_PSTR_LIST(eco, F("eco"), F("Eco"))
MAKE_PSTR_LIST(intelligent, F("intelligent"), F("Intellgent"))
MAKE_PSTR_LIST(flow, F("flow"), F("Fluss"))
MAKE_PSTR_LIST(manual, F("manual"), F("Manuell"))
MAKE_PSTR_LIST(buffer, F("buffer"), F("Speicher"))
MAKE_PSTR_LIST(bufferedflow, F("buffered flow"), F("Durchlaufspeicher"))
MAKE_PSTR_LIST(layeredbuffer, F("layered buffer"), F("Schichtspeicher"))
MAKE_PSTR_LIST(maintenance, F("maintenance"), F("Wartung"))
MAKE_PSTR_LIST(heating, F("heating"), F("Heizen"))
MAKE_PSTR_LIST(cooling, F("cooling"), F("K�hlen"))
MAKE_PSTR_LIST(disinfecting, F("disinfecting"), F("desinfizieren"))
MAKE_PSTR_LIST(no_heat, F("no heat"), F("keine Hitze"))
MAKE_PSTR_LIST(heatrequest, F("heat request"), F("Wärmeanforderung"))
MAKE_PSTR_LIST(valve, F("valve"), F("Ventil"))

// heatpump
MAKE_PSTR_LIST(none, F("none"), F("keiner"))
MAKE_PSTR_LIST(hot_water, F("hot water"), F("heißes Wasser"))
MAKE_PSTR_LIST(pool, F("pool"), F("Schwimmbad"))

// thermostat
MAKE_PSTR_LIST(seltemp, F("selTemp"), F("Solltemperature"))
MAKE_PSTR_LIST(roomtemp, F("roomTemp"), F("Raumtemperatur"))
MAKE_PSTR_LIST(own_prog, F("own prog"), F("Eigenprog"))
MAKE_PSTR_LIST(std_prog, F("std prog"), F("Standardprog"))
MAKE_PSTR_LIST(light, F("light"), F("Leicht"))
MAKE_PSTR_LIST(medium, F("medium"), F("Mittel"))
MAKE_PSTR_LIST(heavy, F("heavy"), F("Schwer"))
MAKE_PSTR_LIST(start, F("start"), F("Start"))
MAKE_PSTR_LIST(heat, F("heat"), F("Heizen"))
MAKE_PSTR_LIST(hold, F("hold"), F("Halten"))
MAKE_PSTR_LIST(cool, F("cool"), F("Kühl"))
MAKE_PSTR_LIST(end, F("end"), F("Ende"))
MAKE_PSTR_LIST(german, F("german"), F("Deutsch"))
MAKE_PSTR_LIST(dutch, F("dutch"), F("Niederländisch"))
MAKE_PSTR_LIST(french, F("french"), F("Französisch"))
MAKE_PSTR_LIST(italian, F("italian"), F("Italienisch"))
MAKE_PSTR_LIST(high, F("high"), F("hoch"))
MAKE_PSTR_LIST(low, F("low"), F("niedrig"))
MAKE_PSTR_LIST(radiator, F("radiator"), F("Heizkörper"))
MAKE_PSTR_LIST(convector, F("convector"), F("Konvektor"))
MAKE_PSTR_LIST(floor, F("floor"), F("Fussboden"))
MAKE_PSTR_LIST(summer, F("summer"), F("Sommer"))
MAKE_PSTR_LIST(winter, F("winter"), F("Winter"))
MAKE_PSTR_LIST(outdoor, F("outdoor"), F("Aussentemperatur"))
MAKE_PSTR_LIST(room, F("room"), F("Raum"))
MAKE_PSTR_LIST(room_outdoor, F("room outdoor"), F("Raum+Au�en"))
MAKE_PSTR_LIST(power, F("power"), F("Leistung"))
MAKE_PSTR_LIST(constant, F("constant"), F("konstant"))
MAKE_PSTR_LIST(simple, F("simple"), F("einfach"))
MAKE_PSTR_LIST(optimized, F("optimized"), F("optimiert"))
MAKE_PSTR_LIST(nofrost, F("nofrost"), F("Frostschutz"))
MAKE_PSTR_LIST(comfort, F("comfort"), F("Komfort"))
MAKE_PSTR_LIST(night, F("night"), F("Nacht"))
MAKE_PSTR_LIST(day, F("day"), F("Tag"))
MAKE_PSTR_LIST(holiday, F("holiday"), F("Urlaub"))
MAKE_PSTR_LIST(reduce, F("reduce"), F("reduziert"))
MAKE_PSTR_LIST(noreduce, F("no reduce"), F("unreduziert"))
MAKE_PSTR_LIST(offset, F("offset"), F("Anhebung"))
MAKE_PSTR_LIST(design, F("design"), F("Auslegung"))
MAKE_PSTR_LIST(minflow, F("min flow"), F("minfluss"))
MAKE_PSTR_LIST(maxflow, F("max flow"), F("maxfluss"))
MAKE_PSTR_LIST(fast, F("fast"), F("schnell"))
MAKE_PSTR_LIST(slow, F("slow"), F("langsam"))
MAKE_PSTR_LIST(internal_temperature, F("internal temperature"), F("Interne Temperatur"))
MAKE_PSTR_LIST(internal_setpoint, F("internal setpoint"), F("Interner Sollwert"))
MAKE_PSTR_LIST(external_temperature, F("external temperature"), F("Externe Temperatur"))
MAKE_PSTR_LIST(burner_temperature, F("burner temperature"), F("Kesseltemperatur"))
MAKE_PSTR_LIST(ww_temperature, F("ww temperature"), F("Wassertemperatur"))
MAKE_PSTR_LIST(smoke_temperature, F("smoke temperature"), F("Abgastemperatur"))
MAKE_PSTR_LIST(weather_compensated, F("weather compensated"), F("Wetter kompensiert"))
MAKE_PSTR_LIST(outside_basepoint, F("outside basepoint"), F("außerhalb des Basispunktes"))
MAKE_PSTR_LIST(functioning_mode, F("functioning mode"), F("Funktionsweise"))

// mixer
MAKE_PSTR_LIST(stopped, F("stopped"), F("gestoppt"))
MAKE_PSTR_LIST(opening, F("opening"), F("öffnen"))
MAKE_PSTR_LIST(closing, F("closing"), F("schließen"))
MAKE_PSTR_LIST(open, F("open"), F("Offen"))
MAKE_PSTR_LIST(close, F("close"), F("Geschlossen"))

// Boiler
MAKE_PSTR_LIST(wwtapactivated, F("wwtapactivated"), F("turn on/off"), F("Aktiviere Warmwasser im Wartungsmodus"))
MAKE_PSTR_LIST(reset, F("reset"), F("Reset"))
MAKE_PSTR_LIST(oilPreHeat, F("oilpreheat"), F("oil preheating"), F("Ölvorwärmung"))
MAKE_PSTR_LIST(heatingActive, F("heatingactive"), F("heating active"), F("Heizung aktiv"))
MAKE_PSTR_LIST(tapwaterActive, F("tapwateractive"), F("tapwater active"), F("Warmwasser aktiv"))
MAKE_PSTR_LIST(selFlowTemp, F("selflowtemp"), F("selected flow temperature"), F("Sollwert Flusstemperatur"))
MAKE_PSTR_LIST(selBurnPow, F("selburnpow"), F("burner selected max power"), F("Sollwert Brennerleistung"))
MAKE_PSTR_LIST(heatingPumpMod, F("heatingpumpmod"), F("heating pump modulation"), F("Heizungspumpe 1 Modulation"))
MAKE_PSTR_LIST(heatingPump2Mod, F("heatingpump2mod"), F("heating pump 2 modulation"), F("Heizungspumpe 2 Modulation"))
MAKE_PSTR_LIST(outdoorTemp, F("outdoortemp"), F("outside temperature"), F("Aussentemperatur"))
MAKE_PSTR_LIST(curFlowTemp, F("curflowtemp"), F("current flow temperature"), F("aktuelle Flusstemperatur"))
MAKE_PSTR_LIST(retTemp, F("rettemp"), F("return temperature"), F("Rücklauftemperatur"))
MAKE_PSTR_LIST(switchTemp, F("switchtemp"), F("mixing switch temperature"), F("Mischer Schalttemperatur"))
MAKE_PSTR_LIST(sysPress, F("syspress"), F("system pressure"), F("Systemdruck"))
MAKE_PSTR_LIST(boilTemp, F("boiltemp"), F("actual boiler temperature"), F("Kesseltemperatur"))
MAKE_PSTR_LIST(exhaustTemp, F("exhausttemp"), F("exhaust temperature"), F("Auslasstemperatur"))
MAKE_PSTR_LIST(burnGas, F("burngas"), F("gas"), F("Gas"))
MAKE_PSTR_LIST(burnGas2, F("burngas2"), F("gas stage 2"), F("Gas Stufe 2"))
MAKE_PSTR_LIST(flameCurr, F("flamecurr"), F("flame current"), F("Flammstrom"))
MAKE_PSTR_LIST(heatingPump, F("heatingpump"), F("heating pump"), F("Heizungspumpe"))
MAKE_PSTR_LIST(fanWork, F("fanwork"), F("fan"), F("Gebläse"))
MAKE_PSTR_LIST(ignWork, F("ignwork"), F("ignition"), F("Zündung"))
MAKE_PSTR_LIST(heatingActivated, F("heatingactivated"), F("heating activated"), F("Heizen aktiviert"))
MAKE_PSTR_LIST(heatingTemp, F("heatingtemp"), F("heating temperature"), F("Kesseltemperatur"))
MAKE_PSTR_LIST(pumpModMax, F("pumpmodmax"), F("burner pump max power"), F("Kesselpumpen Maximalleistung"))
MAKE_PSTR_LIST(pumpModMin, F("pumpmodmin"), F("burner pump min power"), F("Kesselpumpen Minmalleistung"))
MAKE_PSTR_LIST(pumpDelay, F("pumpdelay"), F("pump delay"), F("Pumpennachlauf"))
MAKE_PSTR_LIST(burnMinPeriod, F("burnminperiod"), F("burner min period"), F("Antipendelzeit"))
MAKE_PSTR_LIST(burnMinPower, F("burnminpower"), F("burner min power"), F("minimale Brennerleistung"))
MAKE_PSTR_LIST(burnMaxPower, F("burnmaxpower"), F("burner max power"), F("maximale Brennerleistung"))
MAKE_PSTR_LIST(boilHystOn, F("boilhyston"), F("hysteresis on temperature"), F("Hysterese ein temperatur"))
MAKE_PSTR_LIST(boilHystOff, F("boilhystoff"), F("hysteresis off temperature"), F("Hysterese aus temperatur"))
MAKE_PSTR_LIST(setFlowTemp, F("setflowtemp"), F("set flow temperature"), F("Sollwert Flusstemperatur"))
MAKE_PSTR_LIST(setBurnPow, F("setburnpow"), F("burner set power"), F("Sollwert Brennerleistung"))
MAKE_PSTR_LIST(curBurnPow, F("curburnpow"), F("burner current power"), F("Brennerleistung"))
MAKE_PSTR_LIST(burnStarts, F("burnstarts"), F("burner starts"), F("Brenner # starts"))
MAKE_PSTR_LIST(burnWorkMin, F("burnworkmin"), F("total burner operating time"), F("Brenner Laufzeit"))
MAKE_PSTR_LIST(burn2WorkMin, F("burn2workmin"), F("burner stage 2 operating time"), F("Brenner Stufe 2 Laufzeit"))
MAKE_PSTR_LIST(heatWorkMin, F("heatworkmin"), F("total heat operating time"), F("Heizung Laufzeit"))
MAKE_PSTR_LIST(UBAuptime, F("ubauptime"), F("total UBA operating time"), F("gesamte Laufzeit"))
MAKE_PSTR_LIST(lastCode, F("lastcode"), F("last error code"), F("Fehlerspeicher"))
MAKE_PSTR_LIST(serviceCode, F("servicecode"), F("service code"), F("Statusmeldung"))
MAKE_PSTR_LIST(serviceCodeNumber, F("servicecodenumber"), F("service code number"), F("Statusmeldungsnummer"))
MAKE_PSTR_LIST(maintenanceMessage, F("maintenancemessage"), F("maintenance message"), F("Wartungsmeldung"))
MAKE_PSTR_LIST(maintenanceDate, F("maintenancedate"), F("next maintenance date"), F("Wartungsdatum"))
MAKE_PSTR_LIST(maintenanceType, F("maintenance"), F("maintenance scheduled"), F("Wartungsplan"))
MAKE_PSTR_LIST(maintenanceTime, F("maintenancetime"), F("time to next maintenance"), F("Wartung in"))
MAKE_PSTR_LIST(emergencyOps, F("emergencyops"), F("emergency operation"), F("Notoperation"))
MAKE_PSTR_LIST(emergencyTemp, F("emergencytemp"), F("emergency temperature"), F("Nottemperatur"))

// heatpump/compress specific
MAKE_PSTR_LIST(upTimeControl, F("uptimecontrol"), F("total operating time heat"), F("Betriebszeit total heizen"))
MAKE_PSTR_LIST(upTimeCompHeating, F("uptimecompheating"), F("operating time compressor heating"), F("Betriebszeit Kompressor heizen"))
MAKE_PSTR_LIST(upTimeCompCooling, F("uptimecompcooling"), F("operating time compressor cooling"), F("Betriebszeit Kompressor kühlen"))
MAKE_PSTR_LIST(upTimeCompWw, F("uptimecompww"), F("operating time compressor dhw"), F("Betriebszeit Kompressor"))
MAKE_PSTR_LIST(upTimeCompPool, F("uptimecomppool"), F("operating time compressor pool"), F("Betriebszeit Kompressor Pool"))
MAKE_PSTR_LIST(totalCompStarts, F("totalcompstarts"), F("total compressor control starts"), F("gesamt Kompressor Starts"))
MAKE_PSTR_LIST(heatingStarts, F("heatingstarts"), F("heating control starts"), F("Heizen Starts"))
MAKE_PSTR_LIST(coolingStarts, F("coolingstarts"), F("cooling control starts"), F("Kühlen Starts"))
MAKE_PSTR_LIST(poolStarts, F("poolstarts"), F("pool control starts"), F("Pool Starts"))
MAKE_PSTR_LIST(nrgConsTotal, F("nrgconstotal"), F("total energy consumption"), F("totaler Energieverbrauch"), F("totaler Energieverbrauch"))
MAKE_PSTR_LIST(nrgConsCompTotal, F("nrgconscomptotal"), F("total energy consumption compressor"), F("Energieverbrauch Kompressor total"))
MAKE_PSTR_LIST(nrgConsCompHeating, F("nrgconscompheating"), F("energy consumption compressor heating"), F("Energieverbrauch Kompressor heizen"))
MAKE_PSTR_LIST(nrgConsCompWw, F("nrgconscompww"), F("energy consumption compressor dhw"), F("Energieverbrauch Kompressor"))
MAKE_PSTR_LIST(nrgConsCompCooling, F("nrgconscompcooling"), F("energy consumption compressor cooling"), F("Energieverbrauch Kompressor kühlen"))
MAKE_PSTR_LIST(nrgConsCompPool, F("nrgconscomppool"), F("energy consumption compressor pool"), F("Energieverbrauch Kompressor Pool"))
MAKE_PSTR_LIST(nrgSuppTotal, F("nrgsupptotal"), F("total energy supplied"), F("gesamte Energieabgabe"))
MAKE_PSTR_LIST(nrgSuppHeating, F("nrgsuppheating"), F("total energy supplied heating"), F("gesamte Energieabgabe heizen"))
MAKE_PSTR_LIST(nrgSuppWw, F("nrgsuppww"), F("total energy warm supplied dhw"), F("gesamte Energieabgabe"))
MAKE_PSTR_LIST(nrgSuppCooling, F("nrgsuppcooling"), F("total energy supplied cooling"), F("gesamte Energieabgabe kühlen"))
MAKE_PSTR_LIST(nrgSuppPool, F("nrgsupppool"), F("total energy supplied pool"), F("gesamte Energieabgabe Pool"))
MAKE_PSTR_LIST(auxElecHeatNrgConsTotal,
               F("auxelecheatnrgconstotal"),
               F("total auxiliary electrical heater energy consumption"),
               F("Energieverbrauch el. Zusatzheizung"))
MAKE_PSTR_LIST(auxElecHeatNrgConsHeating,
               F("auxelecheatnrgconsheating"),
               F("auxiliary electrical heater energy consumption heating"),
               F("Energieverbrauch el. Zusatzheizung Heizen"))
MAKE_PSTR_LIST(auxElecHeatNrgConsWW, F("auxelecheatnrgconsww"), F("auxiliary electrical heater energy consumption dhw"), F("Energieverbrauch el. Zusatzheizung"))
MAKE_PSTR_LIST(auxElecHeatNrgConsPool,
               F("auxelecheatnrgconspool"),
               F("auxiliary electrical heater energy consumption pool"),
               F("Energieverbrauch el. Zusatzheizung Pool"))

MAKE_PSTR_LIST(hpCompOn, F("hpcompon"), F("hp compressor"), F("HP Compressor"))                     // need DE
MAKE_PSTR_LIST(hpHeatingOn, F("hpheatingon"), F("hp heating"), F("HP Heating"))                     // need DE
MAKE_PSTR_LIST(hpCoolingOn, F("hpcoolingon"), F("hp cooling"), F("HP Cooling"))                     // need DE
MAKE_PSTR_LIST(hpWwOn, F("hpwwon"), F("hp dhw"), F("HP dhw"))                                       // need DE
MAKE_PSTR_LIST(hpPoolOn, F("hppoolon"), F("hp pool"), F("HP Pool"))                                 // need DE
MAKE_PSTR_LIST(hpBrinePumpSpd, F("hpbrinepumpspd"), F("brine pump speed"), F("Brine Pump Speed"))   // need DE
MAKE_PSTR_LIST(hpCompSpd, F("hpcompspd"), F("compressor speed"), F("Compressor Speed"))             // need DE
MAKE_PSTR_LIST(hpCircSpd, F("hpcircspd"), F("circulation pump speed"), F("Circulation pump Speed")) // need DE
MAKE_PSTR_LIST(hpBrineIn, F("hpbrinein"), F("brine in/evaporator"), F("Brine in/Evaporator"))       // need DE
MAKE_PSTR_LIST(hpBrineOut, F("hpbrineout"), F("brine out/condenser"), F("Brine out/Condenser"))     // need DE
MAKE_PSTR_LIST(hpSuctionGas, F("hpsuctiongas"), F("suction gas"), F("Suction gas"))                 // need DE
MAKE_PSTR_LIST(hpHotGas, F("hphotgas"), F("hot gas/compressed"), F("Hot gas/Compressed"))           // need DE
MAKE_PSTR_LIST(hpSwitchValve, F("hpswitchvalve"), F("switch valve"), F("Switch Valve"))             // need DE
MAKE_PSTR_LIST(hpActivity, F("hpactivity"), F("compressor activity"), F("Compressor Activity"))     // need DE

MAKE_PSTR_LIST(hpPower, F("hppower"), F("compressor power output"), F("Leistung Wärmepumpe"))
MAKE_PSTR_LIST(hpTc0, F("hptc0"), F("heat carrier return (TC0)"), F("Wärmeträgerflüssigkeit Eingang (TC0)"))
MAKE_PSTR_LIST(hpTc1, F("hptc1"), F("heat carrier forward (TC1)"), F("Wärmeträgerflüssigkeit Ausgang (TC1)"))
MAKE_PSTR_LIST(hpTc3, F("hptc3"), F("condenser temperature (TC3)"), F("Verflüssigertemperatur (TC3)"))
MAKE_PSTR_LIST(hpTr3, F("hptr3"), F("refrigerant temperature liquid side (condenser output) (TR3)"), F("Temperaturfühler Kältemittel (Flüssigkeit) (TR3)"))
MAKE_PSTR_LIST(hpTr4, F("hptr4"), F("evaporator inlet temperature (TR4)"), F("Verdampfer Eintritt (TR4)"))
MAKE_PSTR_LIST(hpTr5, F("hptr5"), F("compressor Inlet temperature (TR5)"), F("Temperaturfühler Kompessoransaugleitung (TR5)"))
MAKE_PSTR_LIST(hpTr6, F("hptr6"), F("compressor outlet temperature (TR6)"), F("Temperaturfühler Kompressorausgangsleitung (TR6)"))
MAKE_PSTR_LIST(hpTr7, F("hptr7"), F("refrigerant temperature gas side (condenser input) (TR7)"), F("Temperaturfühler Kältemittel (Gas) (TR7)"))
MAKE_PSTR_LIST(hpTl2, F("hptl2"), F("air inlet temperature (TL2)"), F("Außenlufttemperaturfühler (TL2)"))
MAKE_PSTR_LIST(hpPl1, F("hppl1"), F("low pressure side temperature (PL1)"), F("Niedrigdruckfühler (PL1)"))
MAKE_PSTR_LIST(hpPh1, F("hpph1"), F("high pressure side temperature (PH1)"), F("Hochdruckfühler (PH1)"))

// hybrid heatpump
MAKE_PSTR_LIST(hybridStrategy, F("hybridstrategy"), F("hybrid control strategy"))        // need DE
MAKE_PSTR_LIST(switchOverTemp, F("switchovertemp"), F("outside switchover temperature")) // need DE
MAKE_PSTR_LIST(energyCostRatio, F("energycostratio"), F("energy cost ratio"))            // need DE
MAKE_PSTR_LIST(fossileFactor, F("fossilefactor"), F("fossile energy factor"))            // need DE
MAKE_PSTR_LIST(electricFactor, F("electricfactor"), F("electric energy factor"))         // need DE
MAKE_PSTR_LIST(delayBoiler, F("delayboiler"), F("delay boiler support"))                 // need DE
MAKE_PSTR_LIST(tempDiffBoiler, F("tempdiffboiler"), F("temp diff boiler support"))       // need DE

// alternative heatsource AM200
MAKE_PSTR_LIST(aCylTopTemp, F("cyltoptemp"), F("cylinder top temperature"), F("Zylinder oben Temperatur"))
MAKE_PSTR_LIST(aCylCenterTemp, F("cylcentertemp"), F("cylinder center temperature"), F("Zylinder mitte Temperatur"))
MAKE_PSTR_LIST(aCylBottomTemp, F("cylbottomtemp"), F("cylinder bottom temperature"), F("Zylinder unten Temperatur"))
MAKE_PSTR_LIST(aFlowTemp, F("altflowtemp"), F("alternative hs flow temperature"), F("Alternativ hs Flusstemperatur"))
MAKE_PSTR_LIST(aRetTemp, F("altrettemp"), F("alternative hs return temperature"), F("Alternativ hs Rücktemperatur"))
MAKE_PSTR_LIST(sysFlowTemp, F("sysflowtemp"), F("system flow temperature"), F("System Flusstemperature"))
MAKE_PSTR_LIST(sysRetTemp, F("sysrettemp"), F("system return temperature"), F("System Rücktemperature"))
MAKE_PSTR_LIST(valveByPass, F("valvebypass"), F("bypass valve"), F("bypass Ventil"))
MAKE_PSTR_LIST(valveBuffer, F("valvebuffer"), F("buffer valve"), F("Puffer Ventil"))
MAKE_PSTR_LIST(valveReturn, F("valvereturn"), F("return valve"), F("Rückfluss Ventil"))
MAKE_PSTR_LIST(aPumpMod, F("altpumpmod"), F("alternative hs pump modulation"), F("Alternativ hs Pumpenmodulation"))
MAKE_PSTR_LIST(heatSource, F("heatsource"), F("alternative heating active"), F("Alternativ Heizung"))

MAKE_PSTR_LIST(vr2Config, F("vr2config"), F("vr2 configuration"))                      // need DE
MAKE_PSTR_LIST(ahsActivated, F("ahsactivated"), F("alternate heat source activation")) // need DE
MAKE_PSTR_LIST(aPumpConfig, F("apumpconfig"), F("primary pump config"))                // need DE
MAKE_PSTR_LIST(aPumpSignal, F("apumpsignal"), F("output for pr1 pump"))                // need DE
MAKE_PSTR_LIST(aPumpMin, F("apumpmin"), F("min output pump pr1"))                      // need DE
MAKE_PSTR_LIST(tempRise, F("temprise"), F("ahs return temp rise"))                     // need DE
MAKE_PSTR_LIST(setReturnTemp, F("setreturntemp"), F("set temp return"))                // need DE
MAKE_PSTR_LIST(mixRuntime, F("mixruntime"), F("mixer run time"))                       // need DE
MAKE_PSTR_LIST(bufBypass, F("bufbypass"), F("buffer bypass config"))                   // need DE
MAKE_PSTR_LIST(bufMixRuntime, F("bufmixruntime"), F("bypass mixer run time"))          // need DE
MAKE_PSTR_LIST(bufConfig, F("bufconfig"), F("dhw buffer config"))                      // need DE
MAKE_PSTR_LIST(blockMode, F("blockmode"), F("config htg. blocking mode"))              // need DE
MAKE_PSTR_LIST(blockTerm, F("blockterm"), F("config of block terminal"))               // need DE
MAKE_PSTR_LIST(blockHyst, F("blockhyst"), F("hyst. for bolier block"))                 // need DE
MAKE_PSTR_LIST(releaseWait, F("releasewait"), F("boiler release wait time"))           // need DE

// the following are dhw for the boiler and automatically tagged with 'ww'
MAKE_PSTR_LIST(wwSelTempLow, F("wwseltemplow"), F("selected lower temperature"))              // need DE
MAKE_PSTR_LIST(wwSelTempOff, F("wwseltempoff"), F("selected temperature for off"))            // need DE
MAKE_PSTR_LIST(wwSelTempSingle, F("wwseltempsingle"), F("single charge temperature"))         // need DE
MAKE_PSTR_LIST(wwCylMiddleTemp, F("wwcylmiddletemp"), F("cylinder middle temperature (TS3)")) // need DE

MAKE_PSTR_LIST(wwSelTemp, F("wwseltemp"), F("selected temperature"), F("gewählte Temperatur"))
MAKE_PSTR_LIST(wwSetTemp, F("wwsettemp"), F("set temperature"), F("Solltemperatur"))
MAKE_PSTR_LIST(wwType, F("wwtype"), F("type"), F("Typ"))
MAKE_PSTR_LIST(wwComfort, F("wwcomfort"), F("comfort"), F("Komfort"))
MAKE_PSTR_LIST(wwComfort1, F("wwcomfort1"), F("comfort mode"), F("Komfort mode"))
MAKE_PSTR_LIST(wwFlowTempOffset, F("wwflowtempoffset"), F("flow temperature offset"), F("Flusstemperaturanhebung"))
MAKE_PSTR_LIST(wwMaxPower, F("wwmaxpower"), F("max power"), F("max Leistung"))
MAKE_PSTR_LIST(wwCircPump, F("wwcircpump"), F("circulation pump available"), F("Zirkulationspumpe vorhanden"))
MAKE_PSTR_LIST(wwChargeType, F("wwchargetype"), F("charging type"), F("Ladungstyp"))
MAKE_PSTR_LIST(wwDisinfectionTemp, F("wwdisinfectiontemp"), F("disinfection temperature"), F("Desinfectionstemperatur"))
MAKE_PSTR_LIST(wwCircMode, F("wwcircmode"), F("circulation pump mode"), F("Zirkulationspumpenfrequenz"))
MAKE_PSTR_LIST(wwCirc, F("wwcirc"), F("circulation active"), F("Zirkulation aktiv"))
MAKE_PSTR_LIST(wwCurTemp, F("wwcurtemp"), F("current intern temperature"), F("aktuelle Warmwasser Temperatur intern"))
MAKE_PSTR_LIST(wwCurTemp2, F("wwcurtemp2"), F("current extern temperature"), F("aktuelle Warmwaser Temperatur extern"))
MAKE_PSTR_LIST(wwCurFlow, F("wwcurflow"), F("current tap water flow"), F("aktueller Durchfluss"))
MAKE_PSTR_LIST(wwStorageTemp1, F("wwstoragetemp1"), F("storage intern temperature"), F("interne Speichertemperature"))
MAKE_PSTR_LIST(wwStorageTemp2, F("wwstoragetemp2"), F("storage extern temperature"), F("externer Speichertemperatur"))
MAKE_PSTR_LIST(wwActivated, F("wwactivated"), F("activated"), F("aktiviert"))
MAKE_PSTR_LIST(wwOneTime, F("wwonetime"), F("one time charging"), F("Einmalladung"))
MAKE_PSTR_LIST(wwDisinfecting, F("wwdisinfecting"), F("disinfecting"), F("Desinfizieren"))
MAKE_PSTR_LIST(wwCharging, F("wwcharging"), F("charging"), F("Laden"))
MAKE_PSTR_LIST(wwChargeOptimization, F("wwchargeoptimization"), F("charge optimization"), F("charge optimization"))
MAKE_PSTR_LIST(wwRecharging, F("wwrecharging"), F("recharging"), F("Nachladen"))
MAKE_PSTR_LIST(wwTempOK, F("wwtempok"), F("temperature ok"), F("Temperatur ok"))
MAKE_PSTR_LIST(wwActive, F("wwactive"), F("active"), F("aktiv"))
MAKE_PSTR_LIST(ww3wayValve, F("ww3wayvalve"), F("3-way valve active"), F("3-Wegeventil aktiv"))
MAKE_PSTR_LIST(wwSetPumpPower, F("wwsetpumppower"), F("set pump power"), F("Soll Pumpenleistung"))
MAKE_PSTR_LIST(wwMixerTemp, F("wwmixertemp"), F("mixer temperature"), F("Mischertemperatur"))
MAKE_PSTR_LIST(wwStarts, F("wwstarts"), F("starts"), F("Anzahl starts"))
MAKE_PSTR_LIST(wwStarts2, F("wwstarts2"), F("control starts2"), F("Kreis 2 Anzahl Starts"))
MAKE_PSTR_LIST(wwWorkM, F("wwworkm"), F("active time"), F("aktive Zeit"))
MAKE_PSTR_LIST(wwHystOn, F("wwhyston"), F("hysteresis on temperature"), F("Hysterese Einschalttemperatur"))
MAKE_PSTR_LIST(wwHystOff, F("wwhystoff"), F("hysteresis off temperature"), F("Hysterese Ausschalttemperatur"))
MAKE_PSTR_LIST(wwProgMode, F("wwprogmode"), F("program"), F("Programmmodus"))
MAKE_PSTR_LIST(wwCircProg, F("wwcircprog"), F("circulation program"), F("Zirkulationsprogramm"))
MAKE_PSTR_LIST(wwMaxTemp, F("wwmaxtemp"), F("maximum temperature"), F("Maximale Temperatur"))
MAKE_PSTR_LIST(wwOneTimeKey, F("wwonetimekey"), F("one time key function"), F("Einmalladungstaste"))

// mqtt values / commands
MAKE_PSTR_LIST(switchtime, F("switchtime"), F("program switchtime"), F("Program Schaltzeit"))
MAKE_PSTR_LIST(switchtime1, F("switchtime1"), F("own1 program switchtime"), F("Program 1 Schaltzeit"))
MAKE_PSTR_LIST(switchtime2, F("switchtime2"), F("own2 program switchtime"), F("Programm 2 Schaltzeit"))
MAKE_PSTR_LIST(wwswitchtime, F("wwswitchtime"), F("program switchtime"), F("Programm Schaltzeit"))
MAKE_PSTR_LIST(wwcircswitchtime, F("wwcircswitchtime"), F("circulation program switchtime"), F("Zirculationsprogramm Schaltzeit"))
MAKE_PSTR_LIST(dateTime, F("datetime"), F("date/time"), F("Datum/Zeit"))
MAKE_PSTR_LIST(errorCode, F("errorcode"), F("error code"), F("Fehlermeldung"))
MAKE_PSTR_LIST(ibaMainDisplay, F("display"), F("display"), F("Anzeige"))
MAKE_PSTR_LIST(ibaLanguage, F("language"), F("language"), F("Sprache"))
MAKE_PSTR_LIST(ibaClockOffset, F("clockoffset"), F("clock offset"), F("Uhrkorrektur"))
MAKE_PSTR_LIST(ibaBuildingType, F("building"), F("building type"), F("Gebäude"))
MAKE_PSTR_LIST(heatingPID, F("heatingpid"), F("heating PID"), F("heating PID"))
MAKE_PSTR_LIST(ibaCalIntTemperature, F("intoffset"), F("internal temperature offset"), F("Korrektur interner Temperatur"))
MAKE_PSTR_LIST(ibaMinExtTemperature, F("minexttemp"), F("minimal external temperature"), F("min Aussentemperatur"))
MAKE_PSTR_LIST(backlight, F("backlight"), F("key backlight"), F("Gegenlicht"))
MAKE_PSTR_LIST(damping, F("damping"), F("damping outdoor temperature"), F("Dämpfung der Außentemperatur"))
MAKE_PSTR_LIST(tempsensor1, F("inttemp1"), F("temperature sensor 1"), F("Temperatursensor 1"))
MAKE_PSTR_LIST(tempsensor2, F("inttemp2"), F("temperature sensor 2"), F("Temperatursensor 2"))
MAKE_PSTR_LIST(dampedoutdoortemp, F("dampedoutdoortemp"), F("damped outdoor temperature"), F("gedämpfte Aussentemperatur"))
MAKE_PSTR_LIST(floordrystatus, F("floordry"), F("floor drying"), F("Estrichtrocknung"))
MAKE_PSTR_LIST(floordrytemp, F("floordrytemp"), F("floor drying temperature"), F("Estrichtrocknungs Temperatur"))
MAKE_PSTR_LIST(brightness, F("brightness"), F("screen brightness"), F("bildschirmhelligkeit"))
MAKE_PSTR_LIST(autodst, F("autodst"), F("automatic change daylight saving time"), F("automatische sommerzeit umstellung"))
MAKE_PSTR_LIST(preheating, F("preheating"), F("preheating in the clock program"), F("vorheizen im uhrenprogramm"))
MAKE_PSTR_LIST(offtemp, F("offtemp"), F("temperature when mode is off"), F("temperatur bei ausgeschaltetem modus"))
MAKE_PSTR_LIST(mixingvalves, F("mixingvalves"), F("mixing valves"), F("mischventile"))

// thermostat ww
MAKE_PSTR_LIST(wwMode, F("wwmode"), F("mode"), F("modus"))
MAKE_PSTR_LIST(wwSetTempLow, F("wwsettemplow"), F("set low temperature"), F("untere Solltemperatur"))
MAKE_PSTR_LIST(wwWhenModeOff, F("wwwhenmodeoff"), F("when thermostat mode off"), F("wenn Thermostatmodus ist aus"))
MAKE_PSTR_LIST(wwExtra1, F("wwextra1"), F("circuit 1 extra"), F("Kreis 1 Extra"))
MAKE_PSTR_LIST(wwExtra2, F("wwextra2"), F("circuit 2 extra"), F("Kreis 2 Extra"))

MAKE_PSTR_LIST(wwCharge, F("wwcharge"), F("charge"))                           // need DE
MAKE_PSTR_LIST(wwChargeDuration, F("wwchargeduration"), F("charge duration"))  // need DE
MAKE_PSTR_LIST(wwDisinfect, F("wwdisinfect"), F("disinfection"))               // need DE
MAKE_PSTR_LIST(wwDisinfectDay, F("wwdisinfectday"), F("disinfection day"))     // need DE
MAKE_PSTR_LIST(wwDisinfectHour, F("wwdisinfecthour"), F("disinfection hour"))  // need DE
MAKE_PSTR_LIST(wwDisinfectTime, F("wwdisinfecttime"), F("disinfection time"))  // need DE
MAKE_PSTR_LIST(wwDailyHeating, F("wwdailyheating"), F("daily heating"))        // need DE
MAKE_PSTR_LIST(wwDailyHeatTime, F("wwdailyheattime"), F("daily heating time")) // need DE

// thermostat hc
MAKE_PSTR_LIST(selRoomTemp, F("seltemp"), F("selected room temperature"), F("Sollwert Raumtemperatur"))
MAKE_PSTR_LIST(roomTemp, F("currtemp"), F("current room temperature"), F("aktuelle Raumtemperatur"))
MAKE_PSTR_LIST(mode, F("mode"), F("mode"), F("modus"))
MAKE_PSTR_LIST(modetype, F("modetype"), F("mode type"), F("modus Typ"))
MAKE_PSTR_LIST(fastheatup, F("fastheatup"), F("fast heatup"), F("schnelles Aufheizen"))
MAKE_PSTR_LIST(daytemp, F("daytemp"), F("day temperature"), F("Tagestemperatur"))
MAKE_PSTR_LIST(daylowtemp, F("daytemp2"), F("day temperature T2"), F("Tagestemperatur T2"))
MAKE_PSTR_LIST(daymidtemp, F("daytemp3"), F("day temperature T3"), F("Tagestemperatur T3"))
MAKE_PSTR_LIST(dayhightemp, F("daytemp4"), F("day temperature T4"), F("Tagestemperatur T4"))
MAKE_PSTR_LIST(heattemp, F("heattemp"), F("heat temperature"), F("Heizen Temperatur"))
MAKE_PSTR_LIST(nighttemp, F("nighttemp"), F("night temperature"), F("Nachttemperatur"))
MAKE_PSTR_LIST(nighttemp2, F("nighttemp"), F("night temperature T1"), F("Nachttemperatur T1"))
MAKE_PSTR_LIST(ecotemp, F("ecotemp"), F("eco temperature"), F("eco Temperatur"))
MAKE_PSTR_LIST(manualtemp, F("manualtemp"), F("manual temperature"), F("manuelle Temperatur"))
MAKE_PSTR_LIST(tempautotemp, F("tempautotemp"), F("temporary set temperature automode"), F("zwischenzeitliche Solltemperatur"))
MAKE_PSTR_LIST(remoteseltemp, F("remoteseltemp"), F("temporary set temperature from remote")) // need DE
MAKE_PSTR_LIST(comforttemp, F("comforttemp"), F("comfort temperature"), F("Komforttemperatur"))
MAKE_PSTR_LIST(summertemp, F("summertemp"), F("summer temperature"), F("Sommertemperatur"))
MAKE_PSTR_LIST(designtemp, F("designtemp"), F("design temperature"), F("design-Temperatur"))
MAKE_PSTR_LIST(offsettemp, F("offsettemp"), F("offset temperature"), F("Temperaturanhebung"))
MAKE_PSTR_LIST(minflowtemp, F("minflowtemp"), F("min flow temperature"), F("min Flusstemperatur"))
MAKE_PSTR_LIST(maxflowtemp, F("maxflowtemp"), F("max flow temperature"), F("max Flusstemperatur"))
MAKE_PSTR_LIST(roominfluence, F("roominfluence"), F("room influence"), F("Raumeinfluss"))
MAKE_PSTR_LIST(roominfl_factor, F("roominflfactor"), F("room influence factor"), F("Raumeinfluss Factor"))
MAKE_PSTR_LIST(curroominfl, F("curroominfl"), F("current room influence"), F("aktueller Raumeinfluss"))
MAKE_PSTR_LIST(nofrosttemp, F("nofrosttemp"), F("nofrost temperature"), F("Frostschutztemperatur"))
MAKE_PSTR_LIST(targetflowtemp, F("targetflowtemp"), F("target flow temperature"), F("berechnete Flusstemperatur"))
MAKE_PSTR_LIST(heatingtype, F("heatingtype"), F("heating type"), F("Heizungstyp"))
MAKE_PSTR_LIST(summersetmode, F("summersetmode"), F("set summer mode"), F("Einstellung Sommerbetrieb"))
MAKE_PSTR_LIST(hpoperatingmode, F("hpoperatingmode"), F("heatpump operating mode"), F("Wärmepumpe Betriebsmodus"))
MAKE_PSTR_LIST(hpoperatingstate, F("hpoperatingstate"), F("heatpump operating state"), F("heatpump operating state"))
MAKE_PSTR_LIST(controlmode, F("controlmode"), F("control mode"), F("Kontrollmodus"))
MAKE_PSTR_LIST(control, F("control"), F("control device"), F("Fernsteuerung"))
MAKE_PSTR_LIST(program, F("program"), F("program"), F("Programm"))
MAKE_PSTR_LIST(pause, F("pause"), F("pause time"), F("Pausenzeit"))
MAKE_PSTR_LIST(party, F("party"), F("party time"), F("Partyzeit"))
MAKE_PSTR_LIST(holidaytemp, F("holidaytemp"), F("holiday temperature"), F("Urlaubstemperatur"))
MAKE_PSTR_LIST(summermode, F("summermode"), F("summer mode"), F("Sommerbetrieb"))
MAKE_PSTR_LIST(holidaymode, F("holidaymode"), F("holiday mode"), F("Urlaubsbetrieb"))
MAKE_PSTR_LIST(flowtempoffset, F("flowtempoffset"), F("flow temperature offset for mixer"), F("Flusstemperaturanhebung"))
MAKE_PSTR_LIST(reducemode, F("reducemode"), F("reduce mode"), F("Absenkmodus"))
MAKE_PSTR_LIST(noreducetemp, F("noreducetemp"), F("no reduce below temperature"), F("Absenkung unterbrechen unter Temperatur"))
MAKE_PSTR_LIST(reducetemp, F("reducetemp"), F("off/reduce switch temperature"), F("Absenkmodus unter Temperatur"))
MAKE_PSTR_LIST(vacreducetemp, F("vacreducetemp"), F("vacations off/reduce switch temperature"), F("Urlaub Absenkmodus unter Temperatur"))
MAKE_PSTR_LIST(vacreducemode, F("vacreducemode"), F("vacations reduce mode"), F("Urlaub Absenkmodus"))
MAKE_PSTR_LIST(nofrostmode, F("nofrostmode"), F("nofrost mode"), F("Frostschutz Modus"))
MAKE_PSTR_LIST(remotetemp, F("remotetemp"), F("room temperature from remote"), F("Raumtemperatur der Fernsteuerung"))

MAKE_PSTR_LIST(wwHolidays, F("wwholidays"), F("holiday dates"))                              // need DE
MAKE_PSTR_LIST(wwVacations, F("wwvacations"), F("vacation dates"))                           // need DE
MAKE_PSTR_LIST(holidays, F("holidays"), F("holiday dates"))                                  // need DE
MAKE_PSTR_LIST(vacations, F("vacations"), F("vacation dates"))                               // need DE
MAKE_PSTR_LIST(wwprio, F("wwprio"), F("dhw priority"))                                       // need DE
MAKE_PSTR_LIST(nofrostmode1, F("nofrostmode1"), F("nofrost mode"))                           // need DE
MAKE_PSTR_LIST(reducehours, F("reducehours"), F("duration for nighttemp"))                   // need DE
MAKE_PSTR_LIST(reduceminutes, F("reduceminutes"), F("remaining time for nightmode"))         // need DE
MAKE_PSTR_LIST(switchonoptimization, F("switchonoptimization"), F("switch-on optimization")) // need DE

// heatpump
MAKE_PSTR_LIST(airHumidity, F("airhumidity"), F("relative air humidity"), F("relative Luftfeuchte"))
MAKE_PSTR_LIST(dewTemperature, F("dewtemperature"), F("dew point temperature"), F("Taupunkttemperatur"))

// mixer
MAKE_PSTR_LIST(flowSetTemp, F("flowsettemp"), F("setpoint flow temperature"), F("Sollwert Flusstemperatur"))
MAKE_PSTR_LIST(flowTempHc, F("flowtemphc"), F("flow temperature (TC1)"), F("Flusstemperatur des hk (TC1)"))
MAKE_PSTR_LIST(pumpStatus, F("pumpstatus"), F("pump status (PC1)"), F("Pumpenstatus des hk (PC1)"))
MAKE_PSTR_LIST(mixerStatus, F("valvestatus"), F("mixing valve actuator (VC1)"), F("Mischerventil Position (VC1)"))
MAKE_PSTR_LIST(flowTempVf, F("flowtempvf"), F("flow temperature in header (T0/Vf)"), F("Flusstemperatur am Kessel (T0/Vf)"))
MAKE_PSTR_LIST(mixerSetTime, F("valvesettime"), F("time to set valve"), F("Zeit zum Einstellen des Ventils"))
// mixer prefixed with wwc
MAKE_PSTR_LIST(wwPumpStatus, F("pumpstatus"), F("pump status in assigned wwc (PC1)"), F("Pumpenstatus des wwk (PC1)"))
MAKE_PSTR_LIST(wwTempStatus, F("wwtempstatus"), F("temperature switch in assigned wwc (MC1)"), F("Temperaturschalter des wwk (MC1)"))
MAKE_PSTR_LIST(wwTemp, F("wwtemp"), F("current temperature"), F("aktuelle Temperatur"))
// mixer pool
MAKE_PSTR_LIST(poolSetTemp, F("poolsettemp"), F("pool set temperature"))                        // need DE
MAKE_PSTR_LIST(poolTemp, F("pooltemp"), F("pool temperature"))                                  // need DE
MAKE_PSTR_LIST(poolShuntStatus, F("poolshuntstatus"), F("pool shunt status opening/closing"))   // need DE
MAKE_PSTR_LIST(poolShunt, F("poolshunt"), F("pool shunt open/close (0% = pool / 100% = heat)")) // need DE
MAKE_PSTR_LIST(hydrTemp, F("hydrTemp"), F("hydraulic header temperature"))                      // need DE

// solar
MAKE_PSTR_LIST(cylMiddleTemp, F("cylmiddletemp"), F("cylinder middle temperature (TS3)"), F("cylinder middle temperature (TS3)"))               // need DE
MAKE_PSTR_LIST(retHeatAssist, F("retheatassist"), F("return temperature heat assistance (TS4)"), F("return temperature heat assistance (TS4)")) // need DE
MAKE_PSTR_LIST(m1Valve, F("heatassistvalve"), F("heat assistance valve (M1)"), F("heat assistance valve (M1)"))                                 // need DE
MAKE_PSTR_LIST(m1Power, F("heatassistpower"), F("heat assistance valve power (M1)"), F("heat assistance valve power (M1)"))                     // need DE
MAKE_PSTR_LIST(pumpMinMod, F("pumpminmod"), F("minimum pump modulation"))                                                                       // need DE
MAKE_PSTR_LIST(maxFlow, F("maxflow"), F("maximum solar flow"))                                                                                  // need DE
MAKE_PSTR_LIST(solarPower, F("solarpower"), F("actual solar power"))                                                                            // need DE
MAKE_PSTR_LIST(solarPumpTurnonDiff, F("turnondiff"), F("pump turn on difference"))                                                              // need DE
MAKE_PSTR_LIST(solarPumpTurnoffDiff, F("turnoffdiff"), F("pump turn off difference"))                                                           // need DE
MAKE_PSTR_LIST(pump2MinMod, F("pump2minmod"), F("minimum pump 2 modulation"))                                                                   // need DE
MAKE_PSTR_LIST(solarPump2TurnonDiff, F("turnondiff2"), F("pump 2 turn on difference"))                                                          // need DE
MAKE_PSTR_LIST(solarPump2TurnoffDiff, F("turnoffdiff2"), F("pump 2 turn off difference"))                                                       // need DE

MAKE_PSTR_LIST(collectorTemp, F("collectortemp"), F("collector temperature (TS1)"), F("Kollektortemperatur (TS1)"))
MAKE_PSTR_LIST(collector2Temp, F("collector2temp"), F("collector 2 temperature (TS7)"), F("collector 2 temperature (TS7)"))
MAKE_PSTR_LIST(cylBottomTemp, F("cylbottomtemp"), F("cylinder bottom temperature (TS2)"), F("Speicher Bodentemperatur (TS2)"))
MAKE_PSTR_LIST(cyl2BottomTemp, F("cyl2bottomtemp"), F("second cylinder bottom temperature (TS5)"), F("2. Speicher Bodentemperatur (TS5)"))
MAKE_PSTR_LIST(heatExchangerTemp, F("heatexchangertemp"), F("heat exchanger temperature (TS6)"), F("wärmetauscher Temperatur (TS6)"))
MAKE_PSTR_LIST(collectorMaxTemp, F("collectormaxtemp"), F("maximum collector temperature"), F("maximale Kollektortemperatur"))
MAKE_PSTR_LIST(collectorMinTemp, F("collectormintemp"), F("minimum collector temperature"), F("minimale Kollektortemperatur"))
MAKE_PSTR_LIST(cylMaxTemp, F("cylmaxtemp"), F("maximum cylinder temperature"), F("maximale Speichertemperatur"))
MAKE_PSTR_LIST(solarPumpMod, F("solarpumpmod"), F("pump modulation (PS1)"), F("Pumpenmodulation (PS1)"))
MAKE_PSTR_LIST(cylPumpMod, F("cylpumpmod"), F("cylinder pump modulation (PS5)"), F("Speicherpumpenmodulation (PS5)"))
MAKE_PSTR_LIST(solarPump, F("solarpump"), F("pump (PS1)"), F("Pumpe (PS1)"))
MAKE_PSTR_LIST(solarPump2, F("solarpump2"), F("pump 2 (PS4)"), F("pump 2 (PS4)"))
MAKE_PSTR_LIST(solarPump2Mod, F("solarpump2mod"), F("pump 2 modulation (PS4)"), F("pump 2 modulation (PS4)"))
MAKE_PSTR_LIST(valveStatus, F("valvestatus"), F("valve status"), F("ventilstatus"))
MAKE_PSTR_LIST(cylHeated, F("cylheated"), F("cyl heated"), F("Speichertemperatur erreicht"))
MAKE_PSTR_LIST(collectorShutdown, F("collectorshutdown"), F("collector shutdown"), F("Kollektorabschaltung"))
MAKE_PSTR_LIST(pumpWorkTime, F("pumpworktime"), F("pump working time"), F("Pumpenlaufzeit"))
MAKE_PSTR_LIST(pump2WorkTime, F("pump2worktime"), F("pump 2 working time"), F("Pumpe 2 Laufzeit"))
MAKE_PSTR_LIST(m1WorkTime, F("m1worktime"), F("differential control working time"), F("Differenzregelung Arbeitszeit"))
MAKE_PSTR_LIST(energyLastHour, F("energylasthour"), F("energy last hour"), F("Energie letzte Std"))
MAKE_PSTR_LIST(energyTotal, F("energytotal"), F("total energy"), F("Gesamtenergie"))
MAKE_PSTR_LIST(energyToday, F("energytoday"), F("total energy today"), F("Energie heute"))

// solar ww
MAKE_PSTR_LIST(cyl1, F("cyl 1"), F("Zyl_1"))
MAKE_PSTR_LIST(cyl2, F("cyl 2"), F("Zyl_2"))
MAKE_PSTR_LIST(wwTemp1, F("wwtemp1"), F("temperature 1"), F("Temperatur 1"))
MAKE_PSTR_LIST(wwTemp3, F("wwtemp3"), F("temperature 3"), F("Temperatur 3"))
MAKE_PSTR_LIST(wwTemp4, F("wwtemp4"), F("temperature 4"), F("Temperatur 4"))
MAKE_PSTR_LIST(wwTemp5, F("wwtemp5"), F("temperature 5"), F("Temperatur 5"))
MAKE_PSTR_LIST(wwTemp7, F("wwtemp7"), F("temperature 7"), F("Temperatur 7"))
MAKE_PSTR_LIST(wwPump, F("wwpump"), F("pump"), F("Pumpe"))
// solar ww and mixer wwc
MAKE_PSTR_LIST(wwMinTemp, F("wwmintemp"), F("minimum temperature"), F("minimale Temperatur"))
MAKE_PSTR_LIST(wwRedTemp, F("wwredtemp"), F("reduced temperature"), F("redizierte Temperatur"))
MAKE_PSTR_LIST(wwDailyTemp, F("wwdailytemp"), F("daily temperature"), F("tägl. Temperatur"))
MAKE_PSTR_LIST(wwKeepWarm, F("wwkeepwarm"), F("keep warm"), F("Warmhalten"))
MAKE_PSTR_LIST(wwStatus2, F("wwstatus2"), F("status 2"), F("Status 2"))
MAKE_PSTR_LIST(wwPumpMod, F("wwpumpmod"), F("pump modulation"), F("Pumpen Modulation"))
MAKE_PSTR_LIST(wwFlow, F("wwflow"), F("flow rate"), F("Flussrate"))
// extra mixer ww
MAKE_PSTR_LIST(wwRequiredTemp, F("wwrequiredtemp"), F("required temperature"), F("benötigte Temperatur"))
MAKE_PSTR_LIST(wwDiffTemp, F("wwdifftemp"), F("start differential temperature"), F("Start Differential Temperatur"))

// SM100
MAKE_PSTR_LIST(heatTransferSystem, F("heattransfersystem"), F("heattransfer system"), F("Wärmetransfer System"))
MAKE_PSTR_LIST(externalCyl, F("externalcyl"), F("external cylinder"), F("Externer Speicher"))
MAKE_PSTR_LIST(thermalDisinfect, F("thermaldisinfect"), F("thermal disinfection"), F("Thermische Desinfektion"))
MAKE_PSTR_LIST(heatMetering, F("heatmetering"), F("heatmetering"), F("Wärmemessung"))
MAKE_PSTR_LIST(solarIsEnabled, F("solarenabled"), F("solarmodule enabled"), F("Solarmodul aktiviert"))

// telegram 0x035A
MAKE_PSTR_LIST(solarPumpMode, F("solarpumpmode"), F("pump mode"), F("solar Pumpen Einst."))
MAKE_PSTR_LIST(solarPumpKick, F("pumpkick"), F("pump kick"), F("pump kick"))
MAKE_PSTR_LIST(plainWaterMode, F("plainwatermode"), F("plain water mode"), F("plain water mode"))
MAKE_PSTR_LIST(doubleMatchFlow, F("doublematchflow"), F("doublematchflow"), F("doublematchflow"))
MAKE_PSTR_LIST(solarPump2Mode, F("pump2mode"), F("pump 2 mode"), F("pump 2 mode"))
MAKE_PSTR_LIST(solarPump2Kick, F("pump2kick"), F("pump kick 2"), F("pump kick 2"))

// telegram 0x035F
MAKE_PSTR_LIST(cylPriority, F("cylpriority"), F("cylinder priority"), F("Speicher Priorität"))

// telegram 0x380
MAKE_PSTR_LIST(climateZone, F("climatezone"), F("climate zone"), F("Klimazone"))
MAKE_PSTR_LIST(collector1Area, F("collector1area"), F("collector 1 area"), F("Kollektor 1 Fläche"))
MAKE_PSTR_LIST(collector1Type, F("collector1type"), F("collector 1 type"), F("Kollektor 1 Type"))
MAKE_PSTR_LIST(collector2Area, F("collector2area"), F("collector 2 area"), F("Kollektor 2 Fläche"))
MAKE_PSTR_LIST(collector2Type, F("collector2type"), F("collector 2 type"), F("Kollektor 2 Type"))

// telegram 0x0363 heatCounter
MAKE_PSTR_LIST(heatCntFlowTemp, F("heatcntflowtemp"), F("heat counter flow temperature"), F("Wärmezähler Fluss-Temperatur"))
MAKE_PSTR_LIST(heatCntRetTemp, F("heatcntrettemp"), F("heat counter return temperature"), F("Wärmezähler Rückfluss-Temperatur"))
MAKE_PSTR_LIST(heatCnt, F("heatcnt"), F("heat counter impulses"), F("Wärmezäler Impulse"))
MAKE_PSTR_LIST(swapFlowTemp, F("swapflowtemp"), F("swap flow temperature (TS14)"), F("Austausch Fluss-Temperatur (TS14)"))
MAKE_PSTR_LIST(swapRetTemp, F("swaprettemp"), F("swap return temperature (TS15)"), F("Austausch Rückfluss-Temperatur (TS15)"))

// switch
MAKE_PSTR_LIST(activated, F("activated"), F("activated"), F("aktiviert"))
MAKE_PSTR_LIST(status, F("status"), F("status"), F("Status"))

// RF sensor, id 0x40, telegram 0x435
MAKE_PSTR_LIST(RFTemp, F("rftemp"), F("RF room temperature sensor"), F("RF Raumtemperatur Sensor"))

// unknown fields to track (SM10)
MAKE_PSTR_LIST(data11, F("data11"), F("unknown datafield 11"))
MAKE_PSTR_LIST(data12, F("data12"), F("unknown datafield 12"))
MAKE_PSTR_LIST(data8, F("data8"), F("unknown datafield 8"))
MAKE_PSTR_LIST(data0, F("data0"), F("unknown datafield 0"))
MAKE_PSTR_LIST(data1, F("data1"), F("unknown datafield 1"))
MAKE_PSTR_LIST(setting3, F("setting3"), F("unknown setting 3"))
MAKE_PSTR_LIST(setting4, F("setting4"), F("unknown setting 4"))
