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
#define EMSESP_LOCALE_NL "nl"
#define EMSESP_LOCALE_SE "se"
#define EMSESP_LOCALE_PL "pl"

// translations are in order en, de,nl, se....
// if there is no translation, it will default to en

// General
MAKE_PSTR_LIST(on, F("on"), F("an"), F("naar"), F("på"))
MAKE_PSTR_LIST(off, F("off"), F("aus"), F("van"), F("av"))
MAKE_PSTR_LIST(ON, F("ON"), F("AN"), F("NAAR"), F("PÅ"))
MAKE_PSTR_LIST(OFF, F("OFF"), F("AUS"), F("VAN"), F("AV"))

// Unit Of Measurement mapping - maps to DeviceValueUOM_s in emsdevice.cpp
// uom - also used with HA see https://github.com/home-assistant/core/blob/d7ac4bd65379e11461c7ce0893d3533d8d8b8cbf/homeassistant/const.py#L384
MAKE_PSTR_LIST(minutes, F("minutes"), F("Minuten"), F("Minuten"), F("Minuter"))
MAKE_PSTR_LIST(hours, F("hours"), F("Stunden"), F("Uren"), F("Timmar"))
MAKE_PSTR_LIST(days, F("days"), F("Tage"), F("Dagen"), F("Dagar"))
MAKE_PSTR_LIST(seconds, F("seconds"), F("Sekunden"), F("Seconden"), F("Sekunder"))

// general
MAKE_PSTR_LIST(day_mo, F("mo"), F("Mo"), F("Mo"), F("Må"))
MAKE_PSTR_LIST(day_tu, F("tu"), F("Di"), F("Di"), F("Ti"))
MAKE_PSTR_LIST(day_we, F("we"), F("Mi"), F("Wo"), F("On"))
MAKE_PSTR_LIST(day_th, F("th"), F("Do"), F("Do"), F("To"))
MAKE_PSTR_LIST(day_fr, F("fr"), F("Fr"), F("Vr"), F("Fr"))
MAKE_PSTR_LIST(day_sa, F("sa"), F("Sa"), F("Za"), F("Lö"))
MAKE_PSTR_LIST(day_su, F("su"), F("So"), F("Zo"), F("Sö"))
MAKE_PSTR_LIST(all, F("all"), F("Alle"), F("Alle"), F("Alla"))
MAKE_PSTR_LIST(own_1, F("own 1"), F("Eigen 1"), F("Eigen 1"), F("Egen 1"))
MAKE_PSTR_LIST(family, F("family"), F("Familie"), F("Familie"), F("Familj"))
MAKE_PSTR_LIST(morning, F("morning"), F("Morgends"), F("'s ochtends"), F("Morgon"))
MAKE_PSTR_LIST(evening, F("evening"), F("Abends"), F("'s avonds"), F("Kväll"))
MAKE_PSTR_LIST(seniors, F("seniors"), F("Senioren"), F("Senioren"), F("Seniorer"))
MAKE_PSTR_LIST(no, F("no"), F("nein"), F("nee"), F("nej"))
MAKE_PSTR_LIST(new, F("new"), F("Neu"), F("Nieuw"), F("Ny"))
MAKE_PSTR_LIST(own_2, F("own 2"), F("Eigen 2"), F("Eigen 2"), F("Egen 2"))
MAKE_PSTR_LIST(singles, F("singles"), F("Singles"), F("Singles"), F("Singlar"))
MAKE_PSTR_LIST(am, F("am"), F("Vormittag"), F("Ochtend"), F("Förmiddag"))
MAKE_PSTR_LIST(pm, F("pm"), F("Nachmittag"), F("Namiddag"), F("Eftermiddag"))
MAKE_PSTR_LIST(midday, F("midday"), F("Mittag"), F("Middag"), F("Middag"))
MAKE_PSTR_LIST(unknown, F("unknown"), F("Unbekannt"), F("Onbekend"), F("Okänt"))
MAKE_PSTR_LIST(flat, F("flat"), F("flach"), F("vlak"), F("Platt"))
MAKE_PSTR_LIST(vacuum, F("vacuum"), F("vakuum"), F("vacuum"), F("Vakuum"))
MAKE_PSTR_LIST(co2_optimized, F("co2 optimized"), F("CO2 optimiert"), F("CO2 geoptimaliseerd"), F("CO2-optimerad"))
MAKE_PSTR_LIST(cost_optimized, F("cost optimized"), F("kostenoptimiert"), F("kosten geoptimaliseerd"), F("kostnadsoptimerad"))
MAKE_PSTR_LIST(outside_temp_switched, F("outside temp switched"), F("Außentemp. gesteuert"), F("Buitentemp. gestuurd"), F("Utomhustemp korrigerad"))
MAKE_PSTR_LIST(co2_cost_mix, F("co2 cost mix"), F("Kostenmix"), F("Kostenmix"), F("Kostnadsmix"))
MAKE_PSTR_LIST(analog, F("analog"), F("analog"), F("analoog"), F("analog"))
MAKE_PSTR_LIST(normal, F("normal"), F("normal"), F("normaal"), F("normal"))
MAKE_PSTR_LIST(blocking, F("blocking"), F("Blockierung"), F("Blokkering"), F("Blockering"))

// boiler
MAKE_PSTR_LIST(time, F("time"), F("Zeit"), F("Tijd"), F("Tid"))
MAKE_PSTR_LIST(date, F("date"), F("Datum"), F("Datum"), F("Datum"))
MAKE_PSTR_LIST(continuous, F("continuous"), F("kontinuierlich"), F("continue"), F("kontinuerlig"))
MAKE_PSTR_LIST(3wayvalve, F("3-way valve"), F("3-Wege Ventil"), F("3-weg klep"), F("trevägsventil"))
MAKE_PSTR_LIST(chargepump, F("chargepump"), F("Ladepumpe"), F("laadpomp"), F("laddpump"))
MAKE_PSTR_LIST(hot, F("hot"), F("Heiß"), F("Heet"), F("Het"))
MAKE_PSTR_LIST(high_comfort, F("high comfort"), F("gehobener Komfort"), F("Verhoogd comfort"), F("Förhöjd komfort"))
MAKE_PSTR_LIST(eco, F("eco"), F("Eco"), F("Eco"), F("Eko"))
MAKE_PSTR_LIST(intelligent, F("intelligent"), F("Intelligent"), F("Intelligent"), F("Intelligent"))
MAKE_PSTR_LIST(flow, F("flow"), F("Durchfluss"), F("Volumestroom"), F("Flöde"))
MAKE_PSTR_LIST(manual, F("manual"), F("Manuell"), F("Hamdmatig"), F("Manuell"))
MAKE_PSTR_LIST(buffer, F("buffer"), F("Speicher"), F("Buffer"), F("Buffert"))
MAKE_PSTR_LIST(bufferedflow, F("buffered flow"), F("Durchlaufspeicher"), F("Doorstroombuffer"), F("Buffertflöde"))
MAKE_PSTR_LIST(layeredbuffer, F("layered buffer"), F("Schichtspeicher"), F("Gelaagde buffer"), F("Lagrad buffert"))
MAKE_PSTR_LIST(maintenance, F("maintenance"), F("Wartung"), F("Onderhoud"), F("Underhåll"))
MAKE_PSTR_LIST(heating, F("heating"), F("Heizen"), F("Verwarmen"), F("Uppvärmning"))
MAKE_PSTR_LIST(cooling, F("cooling"), F("Kühlen"), F("Koelen"), F("Kyler"))
MAKE_PSTR_LIST(disinfecting, F("disinfecting"), F("desinfizieren"), F("Desinfecteren"), F("Desinficerar"))
MAKE_PSTR_LIST(no_heat, F("no heat"), F("keine Wärme"), F("Geen warmte"), F("Ingen värme"))
MAKE_PSTR_LIST(heatrequest, F("heat request"), F("Wärmeanforderung"), F("Verwarmignsverzoek"), F("Värmeförfrågan"))
MAKE_PSTR_LIST(valve, F("valve"), F("Ventil"), F("Klep"), F("Ventil"))

// heatpump
MAKE_PSTR_LIST(none, F("none"), F("keiner"), F("geen"), F("ingen"))
MAKE_PSTR_LIST(hot_water, F("hot water"), F("heißes Wasser"), F("warm water"), F("varmvatten"))
MAKE_PSTR_LIST(pool, F("pool"), F("Pool"), F("zwembad"), F("pool"))

// thermostat
MAKE_PSTR_LIST(seltemp, F("selTemp"), F("Solltemperatur"), F("Doeltemperatuur"), F("Börtemperatur"))
MAKE_PSTR_LIST(roomtemp, F("roomTemp"), F("Raumtemperatur"), F("Kamertemperatuur"), F("Rumstemperatur"))
MAKE_PSTR_LIST(own_prog, F("own prog"), F("Eigenprog."), F("Eigen prog."), F("Egen prog."))
MAKE_PSTR_LIST(std_prog, F("std prog"), F("Standardprog."), F("Standaard prog."), F("Standardprog."))
MAKE_PSTR_LIST(light, F("light"), F("Leicht"), F("Licht"), F("Lätt"))
MAKE_PSTR_LIST(medium, F("medium"), F("Mittel"), F("Middel"), F("Medel"))
MAKE_PSTR_LIST(heavy, F("heavy"), F("Schwer"), F("Zwaar"), F("Tung"))
MAKE_PSTR_LIST(start, F("start"), F("Start"), F("Start"), F("Start"))
MAKE_PSTR_LIST(heat, F("heat"), F("Heizen"), F("Verwarmen"), F("Värme"))
MAKE_PSTR_LIST(hold, F("hold"), F("Halten"), F("Pauzeren"), F("Paus"))
MAKE_PSTR_LIST(cool, F("cool"), F("Kühlen"), F("Koelen"), F("Kyla"))
MAKE_PSTR_LIST(end, F("end"), F("Ende"), F("Einde"), F("Slut"))
MAKE_PSTR_LIST(german, F("german"), F("Deutsch"), F("Duits"), F("Tyska"))
MAKE_PSTR_LIST(dutch, F("dutch"), F("Niederländisch"), F("Nederlands"), F("Nederländska"))
MAKE_PSTR_LIST(french, F("french"), F("Französisch"), F("Frans"), F("Franska"))
MAKE_PSTR_LIST(italian, F("italian"), F("Italienisch"), F("Italiaans"), F("Italienska"))
MAKE_PSTR_LIST(high, F("high"), F("hoch"), F("hoog"), F("Hög"))
MAKE_PSTR_LIST(low, F("low"), F("niedrig"), F("laag"), F("Låg"))
MAKE_PSTR_LIST(radiator, F("radiator"), F("Heizkörper"), F("Radiator"), F("Radiator"))
MAKE_PSTR_LIST(convector, F("convector"), F("Konvektor"), F("Convector"), F("Konvektor"))
MAKE_PSTR_LIST(floor, F("floor"), F("Fussboden"), F("Vloer"), F("Golv"))
MAKE_PSTR_LIST(summer, F("summer"), F("Sommer"), F("Zomer"), F("Sommar"))
MAKE_PSTR_LIST(winter, F("winter"), F("Winter"), F("Winter"), F("Vinter"))
MAKE_PSTR_LIST(outdoor, F("outdoor"), F("Außen"), F("Buiten"), F("Utomhus"))
MAKE_PSTR_LIST(room, F("room"), F("Raum"), F("Kamer"), F("Rum"))
MAKE_PSTR_LIST(room_outdoor, F("room outdoor"), F("Raum+Außen"), F("Kamer+Buiten"), F("Rum+Ute"))
MAKE_PSTR_LIST(power, F("power"), F("Leistung"), F("Vermogen"), F("Effekt"))
MAKE_PSTR_LIST(constant, F("constant"), F("konstant"), F("constant"), F("Konstant"))
MAKE_PSTR_LIST(simple, F("simple"), F("einfach"), F("simpel"), F("enkel"))
MAKE_PSTR_LIST(optimized, F("optimized"), F("optimiert"), F("geoptimaliseerd"), F("optimerad"))
MAKE_PSTR_LIST(nofrost, F("nofrost"), F("Frostschutz"), F("Vorstbescherming"), F("Frostskydd"))
MAKE_PSTR_LIST(comfort, F("comfort"), F("Komfort"), F("Comfort"), F("Komfort"))
MAKE_PSTR_LIST(night, F("night"), F("Nacht"), F("Nacht"), F("Natt"))
MAKE_PSTR_LIST(day, F("day"), F("Tag"), F("Dag"), F("Dag"))
MAKE_PSTR_LIST(holiday, F("holiday"), F("Urlaub"), F("Vakantie"), F("Helgdag"))
MAKE_PSTR_LIST(reduce, F("reduce"), F("reduziert"), F("gereduceerd"), F("Reducera"))
MAKE_PSTR_LIST(noreduce, F("no reduce"), F("unreduziert"), F("niet gerduceerd"), F("oreducerad"))
MAKE_PSTR_LIST(offset, F("offset"), F("Anhebung"), F("offset"), F("Förskutning"))
MAKE_PSTR_LIST(design, F("design"), F("Auslegung"), F("Ontwero"), F("Design"))
MAKE_PSTR_LIST(minflow, F("min flow"), F("min. Durchfluss"), F("Min. Doorstroom"), F("Min flöde"))
MAKE_PSTR_LIST(maxflow, F("max flow"), F("max. Durchfluss"), F("Max. Doorstroom"), F("Max flöde"))
MAKE_PSTR_LIST(fast, F("fast"), F("schnell"), F("snel"), F("snabb"))
MAKE_PSTR_LIST(slow, F("slow"), F("langsam"), F("langzaam"), F("långsam"))
MAKE_PSTR_LIST(internal_temperature, F("internal temperature"), F("Interne Temperatur"), F("Interne Temperatuur"), F("Interntemperatur"))
MAKE_PSTR_LIST(internal_setpoint, F("internal setpoint"), F("Interner Sollwert"), F("Interne Streeftemperatuur"), F("Internt börvärde"))
MAKE_PSTR_LIST(external_temperature, F("external temperature"), F("Externe Temperatur"), F("Externe Temperatuur"), F("Extern temperatur"))
MAKE_PSTR_LIST(burner_temperature, F("burner temperature"), F("Brennertemperatur"), F("Brander Temperuur"), F("Brännartemperatur"))
MAKE_PSTR_LIST(ww_temperature, F("ww temperature"), F("Wassertemperatur"), F("Watertemperatuur"), F("Vattentemperatur"))
MAKE_PSTR_LIST(smoke_temperature, F("smoke temperature"), F("Abgastemperatur"), F("Buitentemperatuur"), F("Rökgastemperatur"))
MAKE_PSTR_LIST(weather_compensated, F("weather compensated"), F("Wetter kompensiert"), F("Weer gecompenseerd"), F("Väderkompenserad"))
MAKE_PSTR_LIST(outside_basepoint, F("outside basepoint"), F("außerhalb des Basispunktes"), F("Buiten basispunt"), F("Utomhus baspunkt"))
MAKE_PSTR_LIST(functioning_mode, F("functioning mode"), F("Funktionsweise"), F("Functiemodus"), F("Driftläge"))

// mixer
MAKE_PSTR_LIST(stopped, F("stopped"), F("gestoppt"), F("gestopt"), F("stoppad"))
MAKE_PSTR_LIST(opening, F("opening"), F("öffnen"), F("openen"), F("öppnar"))
MAKE_PSTR_LIST(closing, F("closing"), F("schließen"), F("sluiten"), F("stänger"))
MAKE_PSTR_LIST(open, F("open"), F("Offen"), F("Open"), F("Öppen"))
MAKE_PSTR_LIST(close, F("close"), F("Geschlossen"), F("Gesloten"), F("Stängd"))

// Boiler
MAKE_PSTR_LIST(wwtapactivated, F("wwtapactivated"), F("turn on/off"), F("aktiviert im Wartungsmodus"), F("zet aan/uit"), F("sätt på/av"))
MAKE_PSTR_LIST(reset, F("reset"), F("Reset"), F("Reset"), F("Nollställ"))
MAKE_PSTR_LIST(oilPreHeat, F("oilpreheat"), F("oil preheating"), F("Ölvorwärmung"), F("Olie voorverwarming"), F("Förvärmning olja"))
MAKE_PSTR_LIST(heatingActive, F("heatingactive"), F("heating active"), F("Heizen aktiv"), F("Verwarming actief"), F("Uppvärmning aktiv"))
MAKE_PSTR_LIST(tapwaterActive, F("tapwateractive"), F("tapwater active"), F("Warmwasser aktiv"), F("Warm water actief"), F("Varmvatten aktiv"))
MAKE_PSTR_LIST(selFlowTemp,
               F("selflowtemp"),
               F("selected flow temperature"),
               F("Sollwert Vorlauftemperatur"),
               F("Ingestelde aanvoertemperatuur"),
               F("Börvärde Flödestemperatur"))
MAKE_PSTR_LIST(selBurnPow,
               F("selburnpow"),
               F("burner selected max power"),
               F("Sollwert Brennerleistung"),
               F("Ingestelde maximale brandervermogen"),
               F("Brännare vald maxeffekt"))
MAKE_PSTR_LIST(heatingPumpMod,
               F("heatingpumpmod"),
               F("heating pump modulation"),
               F("Heizungspumpe 1 Modulation"),
               F("Modulatie verwarmingspomp"),
               F("Modulering Värmepump"))
MAKE_PSTR_LIST(heatingPump2Mod,
               F("heatingpump2mod"),
               F("heating pump 2 modulation"),
               F("Heizungspumpe 2 Modulation"),
               F("Modulatie verwarmingspomp 2"),
               F("Modulering Värmepump 2"))
MAKE_PSTR_LIST(outdoorTemp, F("outdoortemp"), F("outside temperature"), F("Aussentemperatur"), F("Buitentemperatuur"), F("Utomhustemperatur"))
MAKE_PSTR_LIST(curFlowTemp,
               F("curflowtemp"),
               F("current flow temperature"),
               F("aktuelle Vorlauftemperatur"),
               F("Huidige aanvoertemperatuur"),
               F("Aktuell flödestemperatur"))
MAKE_PSTR_LIST(retTemp, F("rettemp"), F("return temperature"), F("Rücklauftemperatur"), F("Retourtemperatuur"), F("Returtemperatur"))
MAKE_PSTR_LIST(switchTemp, F("switchtemp"), F("mixing switch temperature"), F("Mischer Schalttemperatur"), F("Mixer temperatuur"), F("Blandartemperatur"))
MAKE_PSTR_LIST(sysPress, F("syspress"), F("system pressure"), F("Systemdruck"), F("Systeemdruk"), F("systemtryck"))
MAKE_PSTR_LIST(boilTemp, F("boiltemp"), F("actual boiler temperature"), F("Kesseltemperatur"), F("Keteltemperatuur"), F("Temperatur Värmepanna"))
MAKE_PSTR_LIST(exhaustTemp, F("exhausttemp"), F("exhaust temperature"), F("Auslasstemperatur"), F("Uitlaattemperatuur"), F("Avgastemperatur"))
MAKE_PSTR_LIST(burnGas, F("burngas"), F("gas"), F("Gas"), F("Gas"), F("Gas"))
MAKE_PSTR_LIST(burnGas2, F("burngas2"), F("gas stage 2"), F("Gas Stufe 2"), F("gas fase 2"), F("Gas Fas 2"))
MAKE_PSTR_LIST(flameCurr, F("flamecurr"), F("flame current"), F("Flammstrom"), F("Vlammenstroom"), F("Lågström"))
MAKE_PSTR_LIST(heatingPump, F("heatingpump"), F("heating pump"), F("Heizungspumpe"), F("Verwarmingspomp"), F("Värmepump"))
MAKE_PSTR_LIST(fanWork, F("fanwork"), F("fan"), F("Gebläse"), F("Ventilator"), F("Fläkt"))
MAKE_PSTR_LIST(ignWork, F("ignwork"), F("ignition"), F("Zündung"), F("Ontsteking"), F("Tändning"))
MAKE_PSTR_LIST(heatingActivated, F("heatingactivated"), F("heating activated"), F("Heizen aktiviert"), F("Verwarmen geactiveerd"), F("Uppvärmning aktiv"))
MAKE_PSTR_LIST(heatingTemp, F("heatingtemp"), F("heating temperature"), F("Heizungstemperatur"), F("Verwarmingstemperatuur"), F("Uppvärmningstemperatur"))
MAKE_PSTR_LIST(pumpModMax, F("pumpmodmax"), F("boiler pump max power"), F("Kesselpumpen Maximalleistung"), F("Ketelpomp max vermogen"), F("Värmepannepump max effekt"))
MAKE_PSTR_LIST(pumpModMin, F("pumpmodmin"), F("boiler pump min power"), F("Kesselpumpen Minmalleistung"), F("Ketelpomp min vermogen"), F("Värmepannepump min effekt"))
MAKE_PSTR_LIST(pumpDelay, F("pumpdelay"), F("pump delay"), F("Pumpennachlauf"), F("Pomp nalooptijd"), F("Pumpfördröjning"))
MAKE_PSTR_LIST(burnMinPeriod, F("burnminperiod"), F("burner min period"), F("Antipendelzeit"), F("Antipendeltijd"), F("Värmepanna Min Period"))
MAKE_PSTR_LIST(burnMinPower, F("burnminpower"), F("burner min power"), F("minimale Brennerleistung"), F("Minimaal brandervermogen"), F("Värmepanna Min Effekt"))
MAKE_PSTR_LIST(burnMaxPower, F("burnmaxpower"), F("burner max power"), F("maximale Brennerleistung"), F("Maximaal brandervermogen"), F("Värmepanna Max Effekt"))
MAKE_PSTR_LIST(boilHystOn,
               F("boilhyston"),
               F("hysteresis on temperature"),
               F("Einschaltdifferenz"),
               F("ketel aan hysterese verschil"),
               F("Hysteres aktiveringstemperatur"))
MAKE_PSTR_LIST(boilHystOff,
               F("boilhystoff"),
               F("hysteresis off temperature"),
               F("Ausschaltdifferenz"),
               F("ketel uit hysterese verschil"),
               F("Hysteres inaktiveringstemperatur"))
MAKE_PSTR_LIST(setFlowTemp,
               F("setflowtemp"),
               F("set flow temperature"),
               F("Sollwert Vorlauftemperatur"),
               F("Ingestelde aanvoertemperatuur"),
               F("Börvärde Flödestemperatur"))
MAKE_PSTR_LIST(setBurnPow, F("setburnpow"), F("burner set power"), F("Sollwert Brennerleistung"), F("Ingesteld brandervermogen"), F("Värmepanna vald Effekt"))
MAKE_PSTR_LIST(curBurnPow, F("curburnpow"), F("burner current power"), F("Brennerleistung"), F("Brandervermogen"), F("Värmepanna aktuell effekt"))
MAKE_PSTR_LIST(burnStarts, F("burnstarts"), F("burner starts"), F("Brenner # starts"), F("Aantal brander starts"), F("Värmepanna antal starter"))
MAKE_PSTR_LIST(burnWorkMin, F("burnworkmin"), F("total burner operating time"), F("Brenner Laufzeit"), F("Totale branderlooptijd"), F("Värmepanna aktiva timmar"))
MAKE_PSTR_LIST(burn2WorkMin,
               F("burn2workmin"),
               F("burner stage 2 operating time"),
               F("Brenner Stufe 2 Laufzeit"),
               F("Totale looptijd brander fase 2"),
               F("Värmepanna steg 2 aktiva timmar"))
MAKE_PSTR_LIST(heatWorkMin, F("heatworkmin"), F("total heat operating time"), F("Heizung Laufzeit"), F("Totale looptijd verwarming"), F("Uppvärmning aktiva timmar"))
MAKE_PSTR_LIST(UBAuptime, F("ubauptime"), F("total UBA operating time"), F("gesamte Laufzeit"), F("totale looptijd branderautomaat (UBA)"), F("Total Tid"))
MAKE_PSTR_LIST(lastCode, F("lastcode"), F("last error code"), F("Fehlerspeicher"), F("Laatste foutcode"), F("Senaste Felkod"))
MAKE_PSTR_LIST(serviceCode, F("servicecode"), F("service code"), F("Statusmeldung"), F("Statuscode"), F("Servicekod"))
MAKE_PSTR_LIST(serviceCodeNumber, F("servicecodenumber"), F("service code number"), F("Statusmeldungsnummer"), F("Status codenummer"), F("Servicekodsnummer"))
MAKE_PSTR_LIST(maintenanceMessage, F("maintenancemessage"), F("maintenance message"), F("Wartungsmeldung"), F("Onderhoudsmelding"), F("Servicemeddelande"))
MAKE_PSTR_LIST(maintenanceDate, F("maintenancedate"), F("next maintenance date"), F("Wartungsdatum"), F("Onderhoudsdatum"), F("Datum nästa Service"))
MAKE_PSTR_LIST(maintenanceType, F("maintenance"), F("maintenance scheduled"), F("Wartungsplan"), F("Onderhoud gepland"), F("Underhåll schemlagt"))
MAKE_PSTR_LIST(maintenanceTime, F("maintenancetime"), F("time to next maintenance"), F("Wartung in"), F("Onderhoud in"), F("Tid till nästa underhåll"))
MAKE_PSTR_LIST(emergencyOps, F("emergencyops"), F("emergency operation"), F("Notoperation"), F("Noodoperatie"), F("Nöddrift"))
MAKE_PSTR_LIST(emergencyTemp, F("emergencytemp"), F("emergency temperature"), F("Nottemperatur"), F("Noodtemperatuur"), F("Nödtemperatur"))

// heatpump/compress specific
MAKE_PSTR_LIST(upTimeControl, F("uptimecontrol"), F("total operating time heat"), F("Betriebszeit Heizen gesamt"), F("Totale bedrijfstijd"), F("Total tid uppvärmning"))
MAKE_PSTR_LIST(upTimeCompHeating,
               F("uptimecompheating"),
               F("operating time compressor heating"),
               F("Betriebszeit Kompressor heizen"),
               F("Bedrijfstijd compressor verwarmingsbedrijf"),
               F("Total tid kompressor uppvärmning"))
MAKE_PSTR_LIST(upTimeCompCooling,
               F("uptimecompcooling"),
               F("operating time compressor cooling"),
               F("Betriebszeit Kompressor kühlen"),
               F("Bedrijfstijd compressor koelbedrijf"),
               F("Total tid kompressor kyla"))
MAKE_PSTR_LIST(upTimeCompWw,
               F("uptimecompww"),
               F("operating time compressor dhw"),
               F("Betriebszeit Kompressor"),
               F("Bedrijfstijd compressor warmwaterbedrijf"),
               F("Total tid kompressor varmvatten"))
MAKE_PSTR_LIST(upTimeCompPool,
               F("uptimecomppool"),
               F("operating time compressor pool"),
               F("Betriebszeit Kompressor Pool"),
               F("Bedrijfstijd compressor voor zwembadbedrijf"),
               F("Total tid kompressor pool"))
MAKE_PSTR_LIST(totalCompStarts,
               F("totalcompstarts"),
               F("total compressor control starts"),
               F("Kompressor Starts gesamt"),
               F("Totaal compressorstarts"),
               F("Kompressorstarter Totalt"))
MAKE_PSTR_LIST(heatingStarts, F("heatingstarts"), F("heating control starts"), F("Heizen Starts"), F("Starts verwarmingsbedrijf"), F("Kompressorstarter Uppvärmning"))
MAKE_PSTR_LIST(coolingStarts, F("coolingstarts"), F("cooling control starts"), F("Kühlen Starts"), F("Starts koelbedrijf"), F("Kompressorstarter Kyla"))
MAKE_PSTR_LIST(poolStarts, F("poolstarts"), F("pool control starts"), F("Pool Starts"), F("Starts zwembadbedrijf"), F("Kompressorstarter Pool"))
MAKE_PSTR_LIST(nrgConsTotal,
               F("nrgconstotal"),
               F("total energy consumption"),
               F("totaler Energieverbrauch"),
               F("Energieverbrauch gesamt"),
               F("Energieverbruik totaal"),
               F("Energiförbrukning"))
MAKE_PSTR_LIST(nrgConsCompTotal,
               F("nrgconscomptotal"),
               F("total energy consumption compressor"),
               F("Energieverbrauch Kompressor gesamt"),
               F("Energieverbruik compressor totaal"),
               F("Energiförbrukning kompressor"))
MAKE_PSTR_LIST(nrgConsCompHeating,
               F("nrgconscompheating"),
               F("energy consumption compressor heating"),
               F("Energieverbrauch Kompressor heizen"),
               F("Energieverbruik compressor verwarmingsbedrijf"),
               F("Energiförbrukning uppvärmning"))
MAKE_PSTR_LIST(nrgConsCompWw,
               F("nrgconscompww"),
               F("energy consumption compressor dhw"),
               F("Energieverbrauch Kompressor"),
               F("Energieverbruik compressor warmwaterbedrijf"),
               F("Energiförbrukning varmvatten"))
MAKE_PSTR_LIST(nrgConsCompCooling,
               F("nrgconscompcooling"),
               F("energy consumption compressor cooling"),
               F("Energieverbrauch Kompressor kühlen"),
               F("Energieverbruik compressor koelbedrijf"),
               F("Energiförbrukning kyla"))
MAKE_PSTR_LIST(nrgConsCompPool,
               F("nrgconscomppool"),
               F("energy consumption compressor pool"),
               F("Energieverbrauch Kompressor Pool"),
               F("Energiebedrijf compressor zwembadbedrijf"),
               F("Energiförbrukning pool"))
MAKE_PSTR_LIST(nrgSuppTotal, F("nrgsupptotal"), F("total energy supplied"), F("gesamte Energieabgabe"), F("Totaal opgewekte energie"), F("Tillförd energi"))
MAKE_PSTR_LIST(nrgSuppHeating,
               F("nrgsuppheating"),
               F("total energy supplied heating"),
               F("gesamte Energieabgabe heizen"),
               F("Opgewekte energie verwarmingsbedrijf"),
               F("Tillförd energi Uppvärmning"))
MAKE_PSTR_LIST(nrgSuppWw,
               F("nrgsuppww"),
               F("total energy warm supplied dhw"),
               F("gesamte Energieabgabe"),
               F("Opgewekte energie warmwaterbedrijf"),
               F("Tillförd energi Varmvatten"))
MAKE_PSTR_LIST(nrgSuppCooling,
               F("nrgsuppcooling"),
               F("total energy supplied cooling"),
               F("gesamte Energieabgabe kühlen"),
               F("Opgewekte energie koelbedrijf"),
               F("Tillförd energi Kyla"))
MAKE_PSTR_LIST(nrgSuppPool,
               F("nrgsupppool"),
               F("total energy supplied pool"),
               F("gesamte Energieabgabe Pool"),
               F("Opgewekte energie zwembadbedrijf"),
               F("TIllförd energi Pool"))
MAKE_PSTR_LIST(auxElecHeatNrgConsTotal,
               F("auxelecheatnrgconstotal"),
               F("total auxiliary electrical heater energy consumption"),
               F("Energieverbrauch el. Zusatzheizung"),
               F("Totaal energieverbruik electrisch verwarmingselement"),
               F("Energiförbrukning Elpatron"))
MAKE_PSTR_LIST(auxElecHeatNrgConsHeating,
               F("auxelecheatnrgconsheating"),
               F("auxiliary electrical heater energy consumption heating"),
               F("Energieverbrauch el. Zusatzheizung Heizen"),
               F("Energieverbruik electrisch verwarmingselement voor verwarmingsbedrijf"),
               F("Energiförbrukning Elpatron Uppvärmning"))
MAKE_PSTR_LIST(auxElecHeatNrgConsWW,
               F("auxelecheatnrgconsww"),
               F("auxiliary electrical heater energy consumption dhw"),
               F("Energieverbrauch el. Zusatzheizung"),
               F("Energieverbruik electrisch verwarmingselement voor warmwaterbedrijf"),
               F("Energiförbrukning Elpatron Varmvatten"))
MAKE_PSTR_LIST(auxElecHeatNrgConsPool,
               F("auxelecheatnrgconspool"),
               F("auxiliary electrical heater energy consumption pool"),
               F("Energieverbrauch el. Zusatzheizung Pool"),
               F("Energieverbruik electrisch verwarmingselement voor zwembadbedrijf"),
               F("Energiförbrukning Elpatron Pool"))

MAKE_PSTR_LIST(hpCompOn, F("hpcompon"), F("hp compressor"), F("WP Kompressor"), F("WP compressor"), F("VP Kompressor"))
MAKE_PSTR_LIST(hpHeatingOn, F("hpheatingon"), F("hp heating"), F("WP Heizen"), F("WP verwarmingsbedrijf"), F("VP Uppvärmning"))
MAKE_PSTR_LIST(hpCoolingOn, F("hpcoolingon"), F("hp cooling"), F("WP Kühlen"), F("WP koelbedrijf"), F("VP Kyla"))
MAKE_PSTR_LIST(hpWwOn, F("hpwwon"), F("hp dhw"), F("WP Warmwasser"), F("WP warmwaterbedrijf"), F("VP Varmvatten"))
MAKE_PSTR_LIST(hpPoolOn, F("hppoolon"), F("hp pool"), F("WP Pool"), F("WP zwembadbedrijf"), F("VP Pool"))
MAKE_PSTR_LIST(hpBrinePumpSpd, F("hpbrinepumpspd"), F("brine pump speed"), F("Solepumpen-Geschw."), F("Snelheid pekelpomp"), F("Hastighet Brine-pump"))
MAKE_PSTR_LIST(hpCompSpd, F("hpcompspd"), F("compressor speed"), F("Kompressor-Geschw."), F("Snelheid compressor"), F("Kompressorhastighet"))
MAKE_PSTR_LIST(hpCircSpd, F("hpcircspd"), F("circulation pump speed"), F("Zirkulationspumpen-Geschw."), F("Snelheid circulatiepomp"), F("Hastighet Cirkulationspump"))
MAKE_PSTR_LIST(hpBrineIn, F("hpbrinein"), F("brine in/evaporator"), F("Sole in/Verdampfer"), F("pekel in/verdamper"), F("Brine in/förångare"))
MAKE_PSTR_LIST(hpBrineOut, F("hpbrineout"), F("brine out/condenser"), F("Sole aus/Kondensator"), F("pekel uit/condensor"), F("Brine ut/kondensor"))
MAKE_PSTR_LIST(hpSuctionGas, F("hpsuctiongas"), F("suction gas"), F("Gasansaugung"), F("Gasaanzuiging"), F("Gasintag"))
MAKE_PSTR_LIST(hpHotGas, F("hphotgas"), F("hot gas/compressed"), F("Heißgas/verdichtet"), F("heet gas/samengeperst"), F("Hetgas/komprimerad"))
MAKE_PSTR_LIST(hpSwitchValve, F("hpswitchvalve"), F("switch valve"), F("Schaltventil"), F("schakelklep"), F("Växelventil"))
MAKE_PSTR_LIST(hpActivity, F("hpactivity"), F("compressor activity"), F("Kompressor aktiv"), F("Compressoractiviteit"), F("Kompressoraktivitet"))

MAKE_PSTR_LIST(hpPower, F("hppower"), F("compressor power output"), F("Kopressorleistung"), F("Compressorvermogen"), F("Kompressoreffekt"))
MAKE_PSTR_LIST(hpTc0, F("hptc0"), F("heat carrier return (TC0)"), F("Kältemittel Rücklauf (TC0)"), F("Koudemiddel retour (TC0O"), F("Värmebärare Retur (TC0)"))
MAKE_PSTR_LIST(hpTc1, F("hptc1"), F("heat carrier forward (TC1)"), F("Kältemittel Vorlauf (TC1)"), F("Koudemiddel aanvoer (TC1)"), F("Värmebärare Framledning (TC1)"))
MAKE_PSTR_LIST(hpTc3, F("hptc3"), F("condenser temperature (TC3)"), F("Verflüssigertemperatur (TC3)"), F("Condensortemperatuur (TC3)"), F("Kondensortemperatur (TC3)"))
MAKE_PSTR_LIST(hpTr3,
               F("hptr3"),
               F("refrigerant temperature liquid side (condenser output) (TR3)"),
               F("Kältemittel (flüssig) (TR3)"),
               F("Temperatuur koudemiddel vloeibare zijde (TR3)"),
               F("Köldmedium temperatur (kondensorutlopp) (TR3)"))
MAKE_PSTR_LIST(hpTr4,
               F("hptr4"),
               F("evaporator inlet temperature (TR4)"),
               F("Verdampfer Eingang (TR4)"),
               F("Verdamper ingangstemperatuur (TR4)"),
               F("Förångare inloppstemp (TR4)"))
MAKE_PSTR_LIST(hpTr5,
               F("hptr5"),
               F("compressor inlet temperature (TR5)"),
               F("Kompessoreingang (TR5)"),
               F("Compressor ingangstemperatuur (TR5)"),
               F("Kompressor inloppstemp (TR5)"))
MAKE_PSTR_LIST(hpTr6,
               F("hptr6"),
               F("compressor outlet temperature (TR6)"),
               F("Kompressorausgang (TR6)"),
               F("Compressor uitgangstemperatuur (TR6)"),
               F("Kompressor utloppstemp (TR6)"))
MAKE_PSTR_LIST(hpTr7,
               F("hptr7"),
               F("refrigerant temperature gas side (condenser input) (TR7)"),
               F("Kältemittel (gasförmig) (TR7)"),
               F("Temperatuur koudemiddel gasvormig (TR7)"),
               F("Köldmedium temperatur gassida (kondensorinlopp) (TR7)"))
MAKE_PSTR_LIST(hpTl2,
               F("hptl2"),
               F("air inlet temperature (TL2)"),
               F("Außenluft-Einlasstemperatur (TL2)"),
               F("Temperatuur luchtinlaat (TL2)"),
               F("Luftintagstemperatur (TL2)"))
MAKE_PSTR_LIST(hpPl1,
               F("hppl1"),
               F("low pressure side temperature (PL1)"),
               F("Niedrigdruckfühler (PL1)"),
               F("Temperatuur lage drukzijde (PL1)"),
               F("Temperatur Lågtryckssidan (PL1)"))
MAKE_PSTR_LIST(hpPh1,
               F("hpph1"),
               F("high pressure side temperature (PH1)"),
               F("Hochdruckfühler (PH1)"),
               F("Temperatuur hoge drukzijde (PH1)"),
               F("Temperatur Högtryckssidan (PH1)"))

MAKE_PSTR_LIST(hpInput1, F("hpin1"), F("input 1 state"), F("Eingang 1 Status"), F("Status input 1"), F("Status Ingång 1"))
MAKE_PSTR_LIST(hpInput2, F("hpin2"), F("input 2 state"), F("Eingang 2 Status"), F("Status input 2"), F("Status Ingång 2"))
MAKE_PSTR_LIST(hpInput3, F("hpin3"), F("input 3 state"), F("Eingang 3 Status"), F("Status input 3"), F("Status Ingång 3"))
MAKE_PSTR_LIST(hpInput4, F("hpin4"), F("input 4 state"), F("Eingang 4 Status"), F("Status input 4"), F("Status Ingång 4"))
MAKE_PSTR_LIST(hpIn1Opt, F("hpin1opt"), F("input 1 options"), F("Eingang 1 Einstellung"), F("Instelling input 1"), F("Inställningar Ingång 1"))
MAKE_PSTR_LIST(hpIn2Opt, F("hpin2opt"), F("input 2 options"), F("Eingang 2 Einstellung"), F("Instelling input 2"), F("Inställningar Ingång 2"))
MAKE_PSTR_LIST(hpIn3Opt, F("hpin3opt"), F("input 3 options"), F("Eingang 3 Einstellung"), F("Instelling input 3"), F("Inställningar Ingång 3"))
MAKE_PSTR_LIST(hpIn4Opt, F("hpin4opt"), F("input 4 options"), F("Eingang 4 Einstellung"), F("Instelling input 4"), F("Inställningar Ingång 4"))

// hybrid heatpump
MAKE_PSTR_LIST(hybridStrategy, F("hybridstrategy"), F("hybrid control strategy"), F("Hybrid Strategie"), F("Hybride strategie"), F("Hybrid kontrollstrategi"))
MAKE_PSTR_LIST(switchOverTemp,
               F("switchovertemp"),
               F("outside switchover temperature"),
               F("Außentemperatur für Umschaltung"),
               F("Schakeltemperatuur buitentemperatuur"),
               F("Utomhus Omställningstemperatur"))
MAKE_PSTR_LIST(energyCostRatio, F("energycostratio"), F("energy cost ratio"), F("Energie-Kosten-Verhältnis"), F("Energiekostenratio"), F("Energi/Kostnads-förhållande"))
MAKE_PSTR_LIST(fossileFactor,
               F("fossilefactor"),
               F("fossile energy factor"),
               F("Energiefaktor Fossil"),
               F("Energiefactor fossiele brandstof"),
               F("Energifaktor fossilenergi"))
MAKE_PSTR_LIST(electricFactor,
               F("electricfactor"),
               F("electric energy factor"),
               F("Energiefaktor elektrisch"),
               F("Energiefactor electrisch"),
               F("Elektrisk energifaktor"))
MAKE_PSTR_LIST(delayBoiler, F("delayboiler"), F("delay boiler support"), F("Verzögerungs-Option"), F("Vertragingsoptie"), F("Fördörjningsoption"))
MAKE_PSTR_LIST(tempDiffBoiler,
               F("tempdiffboiler"),
               F("temp diff boiler support"),
               F("Temperaturdifferenz-Option"),
               F("Verschiltemperatuuroptie"),
               F("Temperaturskillnadsoption"))

// alternative heatsource AM200
MAKE_PSTR_LIST(aCylTopTemp,
               F("cyltoptemp"),
               F("cylinder top temperature"),
               F("Speicher oben Temperatur"),
               F("Buffer temperatuur boven"),
               F("Cylindertemperatur Toppen"))
MAKE_PSTR_LIST(aCylCenterTemp,
               F("cylcentertemp"),
               F("cylinder center temperature"),
               F("Speicher mitte Temperatur"),
               F("Buffer temperatuur midden"),
               F("Cylindertemperatur Mitten"))
MAKE_PSTR_LIST(aCylBottomTemp,
               F("cylbottomtemp"),
               F("cylinder bottom temperature"),
               F("Speicher unten Temperatur"),
               F("Buffer temperatuur onder"),
               F("Cylindertemperatur Botten"))
MAKE_PSTR_LIST(aFlowTemp,
               F("altflowtemp"),
               F("alternative hs flow temperature"),
               F("Alternativer WE Vorlauftemperatur"),
               F("Alternatieve warmtebron aanvoertemperatuur"),
               F("Alternativ flödestemp värmekälla"))
MAKE_PSTR_LIST(aRetTemp,
               F("altrettemp"),
               F("alternative hs return temperature"),
               F("Alternativer WE Rücklauftemperatur"),
               F("Alternatieve warmtebron retourtemperatuur"),
               F("Alternativ returtemp värmekälla"))
MAKE_PSTR_LIST(sysFlowTemp, F("sysflowtemp"), F("system flow temperature"), F("System Vorlauftemperatur"), F("Systeem aanvoertemperatuur"), F("Systemflödestemperatur"))
MAKE_PSTR_LIST(sysRetTemp, F("sysrettemp"), F("system return temperature"), F("System Rücklauftemperatur"), F("Systeem retourtemperatuur"), F("Systemreturtemperatur"))
MAKE_PSTR_LIST(valveByPass, F("valvebypass"), F("bypass valve"), F("Bypass-Ventil"), F("Bypass klep"), F("Bypassventil"))
MAKE_PSTR_LIST(valveBuffer, F("valvebuffer"), F("buffer valve"), F("Puffer-Ventil"), F("Bufferklep"), F("Buffertventil"))
MAKE_PSTR_LIST(valveReturn, F("valvereturn"), F("return valve"), F("Rückfluss-Ventil"), F("Retourklep"), F("Returventil"))
MAKE_PSTR_LIST(aPumpMod,
               F("altpumpmod"),
               F("alternative hs pump modulation"),
               F("Alternativer WE Pumpenmodulation"),
               F("Alternatieve warmtebron pomp modulatie"),
               F("Alternativ Pumpmodulering Värmekälla"))
MAKE_PSTR_LIST(heatSource,
               F("heatsource"),
               F("alternative heating active"),
               F("Alternativer Wärmeerzeuger aktiv"),
               F("Alternatieve warmtebron aktief"),
               F("Alternativ Värmekälla aktiv"))

MAKE_PSTR_LIST(vr2Config, F("vr2config"), F("vr2 configuration"), F("VR2 Konfiguration"), F("VR2 configuratie"), F("VR2 Konfiguration"))
MAKE_PSTR_LIST(ahsActivated,
               F("ahsactivated"),
               F("alternate heat source activation"),
               F("Alt. Wärmeerzeuger aktiviert"),
               F("Altenatieve warmtebron geactiveerd"),
               F("Alternativ värmekälla aktivering"))
MAKE_PSTR_LIST(aPumpConfig, F("apumpconfig"), F("primary pump config"), F("Konfig. Hauptpumpe"), F("Primaire pomp configuratie"), F("Konfiguration Primärpump"))
MAKE_PSTR_LIST(aPumpSignal, F("apumpsignal"), F("output for pr1 pump"), F("Signal Hauptpumpe"), F("Output voor pomp PR1"), F("Utgång från pump PR1"))
MAKE_PSTR_LIST(aPumpMin, F("apumpmin"), F("min output pump pr1"), F("Minimale Pumpenansteuerung"), F("Minimale output pomp PR1"), F("Min Output Pump PR1"))
MAKE_PSTR_LIST(tempRise, F("temprise"), F("ahs return temp rise"), F("Rücklauf Temperaturerhöhung"), F("Verhoging retourtemperatuur"), F("Förhöjd returtemperatur"))
MAKE_PSTR_LIST(setReturnTemp, F("setreturntemp"), F("set temp return"), F("Soll-Rücklauftemperatur"), F("Streeftemperatuur retour"), F("Vald returtemperatur"))
MAKE_PSTR_LIST(mixRuntime, F("mixruntime"), F("mixer run time"), F("Mischer-Laufzeit"), F("Mixer looptijd"), F("Blandningsventil drifttid"))
MAKE_PSTR_LIST(bufBypass, F("bufbypass"), F("buffer bypass config"), F("Konfig. Bypass"), F("Buffer bypass configuratie"), F("Konfiguration Buffer bypass"))
MAKE_PSTR_LIST(bufMixRuntime,
               F("bufmixruntime"),
               F("bypass mixer run time"),
               F("Speicher-Mischer-Laufzeit"),
               F("Buffer mixer looptijd"),
               F("Blandningsventil Bypass drifttid"))
MAKE_PSTR_LIST(bufConfig,
               F("bufconfig"),
               F("dhw buffer config"),
               F("Konfig. Warmwasserspeicher"),
               F("Warmwater boiler configuratie"),
               F("Konfiguration Varmvattentank"))
MAKE_PSTR_LIST(blockMode, F("blockmode"), F("config htg. blocking mode"), F("Konfig. Sperr-Modus"), F("Configuratie blokeermodus"), F("Konfiguration Blockeringsläge"))
MAKE_PSTR_LIST(blockTerm,
               F("blockterm"),
               F("config of block terminal"),
               F("Konfig. Sperrterminal"),
               F("Configuratie blookerterminal"),
               F("Konfiguration Blockeringsterminal"))
MAKE_PSTR_LIST(blockHyst, F("blockhyst"), F("hyst. for boiler block"), F("Hysterese Sperrmodus"), F("Hysterese blokeerterminal"), F("Hysteres Blockeringsmodul"))
MAKE_PSTR_LIST(releaseWait, F("releasewait"), F("boiler release wait time"), F("Wartezeit Freigabe"), F("Wachttijd ketel vrijgave"), F("Väntetid Frisläppning"))

// the following are dhw for the boiler and automatically tagged with 'dhw'
MAKE_PSTR_LIST(wwSelTempLow,
               F("wwseltemplow"),
               F("selected lower temperature"),
               F("untere Solltemperatur"),
               F("Onderste streeftemperatuur"),
               F("Vald lägstatemperatur"))
MAKE_PSTR_LIST(wwSelTempOff,
               F("wwseltempoff"),
               F("selected temperature for off"),
               F("Solltemperatur bei AUS"),
               F("Streeftemperatuur bij UIT"),
               F("Vald tempereatur för AV"))
MAKE_PSTR_LIST(wwSelTempSingle,
               F("wwseltempsingle"),
               F("single charge temperature"),
               F("Solltemperature Einmalladung"),
               F("Streeftemperatuur enkele lading"),
               F("Temperatur Engångsladdning"))
MAKE_PSTR_LIST(wwCylMiddleTemp,
               F("wwcylmiddletemp"),
               F("cylinder middle temperature (TS3)"),
               F("Speichertemperature Mitte"),
               F("Buffer temperatuur midden"),
               F("Cylinder Temperatur Mitten (TS3)"))

MAKE_PSTR_LIST(wwSelTemp, F("wwseltemp"), F("selected temperature"), F("gewählte Temperatur"), F("Geselecteerd temperatuur"), F("Vald Temperatur"))
MAKE_PSTR_LIST(wwSetTemp, F("wwsettemp"), F("set temperature"), F("Solltemperatur"), F("Streeftemperatuut"), F("Börtempertur"))
MAKE_PSTR_LIST(wwType, F("wwtype"), F("type"), F("Typ"), F("type"), F("Typ"))
MAKE_PSTR_LIST(wwComfort, F("wwcomfort"), F("comfort"), F("Komfort"), F("Comfort"), F("Komfort"))
MAKE_PSTR_LIST(wwComfort1, F("wwcomfort1"), F("comfort mode"), F("Komfort-Modus"), F("Comfort modus"), F("Komfortläge"))
MAKE_PSTR_LIST(wwFlowTempOffset,
               F("wwflowtempoffset"),
               F("flow temperature offset"),
               F("Vorlauftemperaturanhebung"),
               F("Aanvoertemperatuur offset"),
               F("Flödestemperatur förskjutning"))
MAKE_PSTR_LIST(wwMaxPower, F("wwmaxpower"), F("max power"), F("max Leistung"), F("Maximaal vermogen"), F("Max Effekt"))
MAKE_PSTR_LIST(wwCircPump,
               F("wwcircpump"),
               F("circulation pump available"),
               F("Zirkulationspumpe vorhanden"),
               F("Circulatiepomp aanwezig"),
               F("Cirkulationspump tillgänglig"))
MAKE_PSTR_LIST(wwChargeType, F("wwchargetype"), F("charging type"), F("Ladungstyp"), F("Buffer laadtype"), F("Laddningstyp"))
MAKE_PSTR_LIST(wwDisinfectionTemp,
               F("wwdisinfectiontemp"),
               F("disinfection temperature"),
               F("Desinfectionstemperatur"),
               F("Desinfectietemperatuur"),
               F("Desinfektionstemperatur"))
MAKE_PSTR_LIST(wwCircMode, F("wwcircmode"), F("circulation pump mode"), F("Zirkulationspumpen-Modus"), F("Modus circulatiepomp"), F("Läge Cirkulationspump"))
MAKE_PSTR_LIST(wwCirc, F("wwcirc"), F("circulation active"), F("Zirkulation aktiv"), F("Circulatiepomp actief"), F("Cirkulation aktiv"))
MAKE_PSTR_LIST(wwCurTemp,
               F("wwcurtemp"),
               F("current intern temperature"),
               F("aktuelle interne Temperatur"),
               F("Huidige interne temperatuur"),
               F("Aktuell Intern Temperatur"))
MAKE_PSTR_LIST(wwCurTemp2,
               F("wwcurtemp2"),
               F("current extern temperature"),
               F("aktuelle externe Temperatur"),
               F("Huidige externe temperatuur"),
               F("Aktuell Extern Temperatur"))
MAKE_PSTR_LIST(wwCurFlow, F("wwcurflow"), F("current tap water flow"), F("aktueller Durchfluss"), F("Hudige warmwater doorstroming"), F("Aktuellt tappvattenflöde"))
MAKE_PSTR_LIST(wwStorageTemp1,
               F("wwstoragetemp1"),
               F("storage intern temperature"),
               F("interne Speichertemperature"),
               F("Interne buffertemperatuur"),
               F("Beredare Intern Temperatur"))
MAKE_PSTR_LIST(wwStorageTemp2,
               F("wwstoragetemp2"),
               F("storage extern temperature"),
               F("externer Speichertemperatur"),
               F("Externe buffertemperatuur"),
               F("Beredare Extern Tempereatur"))
MAKE_PSTR_LIST(wwActivated, F("wwactivated"), F("activated"), F("aktiviert"), F("geactiveerd"), F("Aktiverad"))
MAKE_PSTR_LIST(wwOneTime, F("wwonetime"), F("one time charging"), F("Einmalladung"), F("Buffer eenmalig laden"), F("Engångsladdning"))
MAKE_PSTR_LIST(wwDisinfecting, F("wwdisinfecting"), F("disinfecting"), F("Desinfizieren"), F("Desinfectie"), F("Desinficerar"))
MAKE_PSTR_LIST(wwCharging, F("wwcharging"), F("charging"), F("Laden"), F("Laden"), F("Laddar"))
MAKE_PSTR_LIST(wwChargeOptimization, F("wwchargeoptimization"), F("charge optimization"), F("Ladungsoptimierung"), F("laadoptimalisatie"), F("Laddningsoptimering"))
MAKE_PSTR_LIST(wwRecharging, F("wwrecharging"), F("recharging"), F("Nachladen"), F("herladen"), F("Omladdar"))
MAKE_PSTR_LIST(wwTempOK, F("wwtempok"), F("temperature ok"), F("Temperatur ok"), F("Temperatuur OK"), F("Temperatur OK"))
MAKE_PSTR_LIST(wwActive, F("wwactive"), F("active"), F("aktiv"), F("Actief"), F("Aktiv"))
MAKE_PSTR_LIST(ww3wayValve, F("ww3wayvalve"), F("3-way valve active"), F("3-Wegeventil aktiv"), F("3-wegklep actief"), F("Trevägsventil Aktiv"))
MAKE_PSTR_LIST(wwSetPumpPower, F("wwsetpumppower"), F("set pump power"), F("Soll Pumpenleistung"), F("Streefwaarde pompvermogen"), F("Vald pumpeffekt"))
MAKE_PSTR_LIST(wwMixerTemp, F("wwmixertemp"), F("mixer temperature"), F("Mischertemperatur"), F("Mixertemperatuur"), F("Blandningsventil-tempertur"))
MAKE_PSTR_LIST(wwStarts, F("wwstarts"), F("starts"), F("Anzahl Starts"), F("Aantal starts"), F("Antal Starter"))
MAKE_PSTR_LIST(wwStarts2, F("wwstarts2"), F("control starts2"), F("Kreis 2 Anzahl Starts"), F("Aantal starts circuit 2"), F("Antal Starter Krets 2"))
MAKE_PSTR_LIST(wwWorkM, F("wwworkm"), F("active time"), F("aktive Zeit"), F("Actieve tijd"), F("Aktiv Tid"))
MAKE_PSTR_LIST(wwHystOn,
               F("wwhyston"),
               F("hysteresis on temperature"),
               F("Einschalttemperaturdifferenz"),
               F("Inschakeltemperatuurverschil"),
               F("Hysteres PÅ-temperatur"))
MAKE_PSTR_LIST(wwHystOff,
               F("wwhystoff"),
               F("hysteresis off temperature"),
               F("Ausschalttemperaturdifferenz"),
               F("Uitschakeltemperatuurverschil"),
               F("Hysteres AV-temperatur"))
MAKE_PSTR_LIST(wwProgMode, F("wwprogmode"), F("program"), F("Programmmodus"), F("Programma"), F("Program"))
MAKE_PSTR_LIST(wwCircProg, F("wwcircprog"), F("circulation program"), F("Zirkulationsprogramm"), F("Circulatieprogramma"), F("Cirkulationsprogram"))
MAKE_PSTR_LIST(wwMaxTemp, F("wwmaxtemp"), F("maximum temperature"), F("Maximale Temperatur"), F("Maximale temperatuur"), F("Maximal Temperatur"))
MAKE_PSTR_LIST(wwOneTimeKey, F("wwonetimekey"), F("one time key function"), F("Einmalladungstaste"), F("Knop voor eenmalig laden buffer"), F("Engångsfunktion"))

// mqtt values / commands
MAKE_PSTR_LIST(switchtime, F("switchtime"), F("program switchtime"), F("Programm Schaltzeit"), F("Programma schakeltijd"), F("Program Bytestid"))
MAKE_PSTR_LIST(switchtime1, F("switchtime1"), F("own1 program switchtime"), F("Programm 1 Schaltzeit"), F("Schakeltijd programma 1"), F("Program 1 Bytestid"))
MAKE_PSTR_LIST(switchtime2, F("switchtime2"), F("own2 program switchtime"), F("Programm 2 Schaltzeit"), F("Schakeltijd programma 2"), F("Program 2 Bytestid"))
MAKE_PSTR_LIST(wwswitchtime,
               F("wwswitchtime"),
               F("program switchtime"),
               F("Programm Schaltzeit"),
               F("Warm water programma schakeltijd"),
               F("Varmvattenprogram Bytestid"))
MAKE_PSTR_LIST(wwcircswitchtime,
               F("wwcircswitchtime"),
               F("circulation program switchtime"),
               F("Zirculationsprogramm Schaltzeit"),
               F("Schakeltijd circulatieprogramma"),
               F("Cirkulationsprogram Bytestid"))
MAKE_PSTR_LIST(dateTime, F("datetime"), F("date/time"), F("Datum/Zeit"), F("Datum/Tijd"), F("Datum/Tid"))
MAKE_PSTR_LIST(errorCode, F("errorcode"), F("error code"), F("Fehlermeldung"), F("Foutmeldingscode"), F("Felkod"))
MAKE_PSTR_LIST(ibaMainDisplay, F("display"), F("display"), F("Anzeige"), F("Display"), F("Display"))
MAKE_PSTR_LIST(ibaLanguage, F("language"), F("language"), F("Sprache"), F("Taal"), F("Språk"))
MAKE_PSTR_LIST(ibaClockOffset, F("clockoffset"), F("clock offset"), F("Uhrkorrektur"), F("Klokcorrectie"), F("Tidskorrigering"))
MAKE_PSTR_LIST(ibaBuildingType, F("building"), F("building type"), F("Gebäude"), F("Byggnadstyp"))
MAKE_PSTR_LIST(heatingPID, F("heatingpid"), F("heating PID"), F("Heizungs-PID"), F("Uppvärmning PID"))
MAKE_PSTR_LIST(ibaCalIntTemperature, F("intoffset"), F("internal temperature offset"), F("Korrektur interner Temperatur"), F("Korrigering interntemperatur"))
MAKE_PSTR_LIST(ibaMinExtTemperature, F("minexttemp"), F("minimal external temperature"), F("min. Aussentemperatur"), F("Min Extern Temperatur"))
MAKE_PSTR_LIST(backlight, F("backlight"), F("key backlight"), F("Gegenlicht"), F("Toetsverlichting"), F("Bakgrundsbelysning"))
MAKE_PSTR_LIST(damping,
               F("damping"),
               F("damping outdoor temperature"),
               F("Dämpfung der Außentemperatur"),
               F("Demping buitentemperatuur"),
               F("Dämpning Utomhustemperatur"))
MAKE_PSTR_LIST(tempsensor1, F("inttemp1"), F("temperature sensor 1"), F("Temperatursensor 1"), F("Temperatuursensor 1"), F("Temperatursensor 1"))
MAKE_PSTR_LIST(tempsensor2, F("inttemp2"), F("temperature sensor 2"), F("Temperatursensor 2"), F("Temperatuursensor 2"), F("Temperatursensor 2"))
MAKE_PSTR_LIST(dampedoutdoortemp,
               F("dampedoutdoortemp"),
               F("damped outdoor temperature"),
               F("gedämpfte Außentemperatur"),
               F("Gedempte buitentemperatuur"),
               F("Dämpad Utomhustemperatur"))
MAKE_PSTR_LIST(floordrystatus, F("floordry"), F("floor drying"), F("Estrichtrocknung"), F("Vloerdroogprogramma"), F("Golvtorkning"))
MAKE_PSTR_LIST(floordrytemp,
               F("floordrytemp"),
               F("floor drying temperature"),
               F("Estrichtrocknungs Temperatur"),
               F("Temperatuur vloerdroogprogramma"),
               F("Golvtorkning Temperatur"))
MAKE_PSTR_LIST(brightness, F("brightness"), F("screen brightness"), F("Bildschirmhelligkeit"), F("Schermhelderheid"), F("Ljusstyrka"))
MAKE_PSTR_LIST(autodst,
               F("autodst"),
               F("automatic change daylight saving time"),
               F("automatische Sommerzeit Umstellung"),
               F("Automatische omschakeling zomer-wintertijd"),
               F("Automatisk växling sommar/vinter-tid"))
MAKE_PSTR_LIST(preheating,
               F("preheating"),
               F("preheating in the clock program"),
               F("Vorheizen im Zeitprogramm"),
               F("Voorverwarming in het klokprogramma"),
               F("Förvärmning i tidsprogram"))
MAKE_PSTR_LIST(offtemp, F("offtemp"), F("temperature when mode is off"), F("Temperatur bei AUS"), F("Temperatuur bij UIT"), F("Temperatur Avslagen"))
MAKE_PSTR_LIST(mixingvalves, F("mixingvalves"), F("mixing valves"), F("Mischventile"), F("Mengkleppen"), F("Blandningsventiler"))

// thermostat ww
MAKE_PSTR_LIST(wwMode, F("wwmode"), F("mode"), F("Modus"), F("Modus"), F("Läge"))
MAKE_PSTR_LIST(wwSetTempLow, F("wwsettemplow"), F("set low temperature"), F("untere Solltemperatur"), F("Onderste streeftemperatuur"), F("Nedre Börvärde"))
MAKE_PSTR_LIST(wwWhenModeOff,
               F("wwwhenmodeoff"),
               F("when thermostat mode off"),
               F("bei Thermostatmodus AUS"),
               F("Als Thermostaat op UIT"),
               F("när Termostatläge är AV"))
MAKE_PSTR_LIST(wwExtra1, F("wwextra1"), F("circuit 1 extra"), F("Kreis 1 Extra"), F("Circuit 1 extra"), F("Krets 1 Extra"))
MAKE_PSTR_LIST(wwExtra2, F("wwextra2"), F("circuit 2 extra"), F("Kreis 2 Extra"), F("Circuit 2 extra"), F("Kets 2 Extra"))

MAKE_PSTR_LIST(wwCharge, F("wwcharge"), F("charge"), F("Laden"), F("Laden"), F("Ladda"))
MAKE_PSTR_LIST(wwChargeDuration, F("wwchargeduration"), F("charge duration"), F("Ladedauer"), F("Laadtijd"), F("Laddtid"))
MAKE_PSTR_LIST(wwDisinfect, F("wwdisinfect"), F("disinfection"), F("Desinfektion"), F("Desinfectie"), F("Desinfektion"))
MAKE_PSTR_LIST(wwDisinfectDay, F("wwdisinfectday"), F("disinfection day"), F("Desinfektionstag"), F("Desinfectiedag"), F("Desinfektionsdag"))
MAKE_PSTR_LIST(wwDisinfectHour, F("wwdisinfecthour"), F("disinfection hour"), F("Desinfektionsstunde"), F("Desinfectieuur"), F("Desinfektionstimme"))
MAKE_PSTR_LIST(wwDisinfectTime, F("wwdisinfecttime"), F("disinfection time"), F("Desinfektionszeit"), F("Desinfectietijd"), F("Desinfektionstid"))
MAKE_PSTR_LIST(wwDailyHeating, F("wwdailyheating"), F("daily heating"), F("täglich Heizen"), F("Dagelijks opwarmen"), F("Daglig Uppvärmning"))
MAKE_PSTR_LIST(wwDailyHeatTime, F("wwdailyheattime"), F("daily heating time"), F("tägliche Heizzeit"), F("Tijd dagelijkse opwarming"), F("Daglig Uppvärmningstid"))

// thermostat hc
MAKE_PSTR_LIST(selRoomTemp, F("seltemp"), F("selected room temperature"), F("Sollwert Raumtemperatur"), F("Streeftemperatuur kamer"), F("Vald Rumstemperatur"))
MAKE_PSTR_LIST(roomTemp, F("currtemp"), F("current room temperature"), F("aktuelle Raumtemperatur"), F("Huidige kamertemperatuur"), F("Aktuell Rumstemperatur"))
MAKE_PSTR_LIST(mode, F("mode"), F("mode"), F("Modus"), F("Modus"), F("Läge"))
MAKE_PSTR_LIST(modetype, F("modetype"), F("mode type"), F("Modus Typ"), F("Type modus"), F("Typ av läge"))
MAKE_PSTR_LIST(fastheatup, F("fastheatup"), F("fast heatup"), F("schnelles Aufheizen"), F("Snel opwarmen"), F("Snabb Uppvärmning"))
MAKE_PSTR_LIST(daytemp, F("daytemp"), F("day temperature"), F("Tagestemperatur"), F("temperatuur dag"), F("Dagstemperatur"))
MAKE_PSTR_LIST(daylowtemp, F("daytemp2"), F("day temperature T2"), F("Tagestemperatur T2"), F("Temperatuur dag T2"), F("Dagstemperatur T2"))
MAKE_PSTR_LIST(daymidtemp, F("daytemp3"), F("day temperature T3"), F("Tagestemperatur T3"), F("Temperatuur dag T3"), F("Dagstemperatur T3"))
MAKE_PSTR_LIST(dayhightemp, F("daytemp4"), F("day temperature T4"), F("Tagestemperatur T4"), F("Temperatuur dag T4"), F("Dagstemperatur T4"))
MAKE_PSTR_LIST(heattemp, F("heattemp"), F("heat temperature"), F("Heizen Temperatur"), F("Temperatuur verwarming"), F("Temperatur Uppvärmning"))
MAKE_PSTR_LIST(nighttemp, F("nighttemp"), F("night temperature"), F("Nachttemperatur"), F("Nachttemperatuur"), F("Nattemperatur"))
MAKE_PSTR_LIST(nighttemp2, F("nighttemp"), F("night temperature T1"), F("Nachttemperatur T1"), F("Nachttemperatuur T1"), F("Nattemperatur T1"))
MAKE_PSTR_LIST(ecotemp, F("ecotemp"), F("eco temperature"), F("eco Temperatur"), F("Temperatuur eco"), F("Eko-temperatur"))
MAKE_PSTR_LIST(manualtemp, F("manualtemp"), F("manual temperature"), F("manuelle Temperatur"), F("temperatuur handmatig"), F("Temperatur Manuell"))
MAKE_PSTR_LIST(tempautotemp,
               F("tempautotemp"),
               F("temporary set temperature automode"),
               F("zwischenzeitliche Solltemperatur"),
               F("Streeftemperatuur automodus tijdelijk"),
               F("Temporär Aktivering av Auto-läge"))
MAKE_PSTR_LIST(remoteseltemp,
               F("remoteseltemp"),
               F("temporary set temperature from remote"),
               F("Temperatur von Fernsteuerung"),
               F("Temperatuur van afstandsbedieding"),
               F("Temperatur från fjärruppkoppling"))
MAKE_PSTR_LIST(comforttemp, F("comforttemp"), F("comfort temperature"), F("Komforttemperatur"), F("Comforttemperatuur"), F("Komforttemperatur"))
MAKE_PSTR_LIST(summertemp, F("summertemp"), F("summer temperature"), F("Sommertemperatur"), F("Zomertemperatuur"), F("Sommartemperatur"))
MAKE_PSTR_LIST(designtemp, F("designtemp"), F("design temperature"), F("Auslegungstemperatur"), F("Ontwerptemperatuur"), F("Design-temperatur"))
MAKE_PSTR_LIST(offsettemp, F("offsettemp"), F("offset temperature"), F("Temperaturanhebung"), F("Temperatuur offset"), F("Temperaturkorrigering"))
MAKE_PSTR_LIST(minflowtemp, F("minflowtemp"), F("min flow temperature"), F("min Vorlauftemperatur"), F("Min Flödestemperatur"))
MAKE_PSTR_LIST(maxflowtemp, F("maxflowtemp"), F("max flow temperature"), F("max Vorlauftemperatur"), F("Max Flödestemperatur"))
MAKE_PSTR_LIST(roominfluence, F("roominfluence"), F("room influence"), F("Raumeinfluss"), F("Ruimteinvloed"), F("Rumspåverkan"))
MAKE_PSTR_LIST(roominfl_factor, F("roominflfactor"), F("room influence factor"), F("Raumeinfluss Factor"), F("Factor ruimteinvloed"), F("Rumspåverkansfaktor"))
MAKE_PSTR_LIST(curroominfl, F("curroominfl"), F("current room influence"), F("aktueller Raumeinfluss"), F("Huidige ruimteinvloed"), F("Aktuell Rumspåverkan"))
MAKE_PSTR_LIST(nofrosttemp, F("nofrosttemp"), F("nofrost temperature"), F("Frostschutztemperatur"), F("Temperatuur vorstbeveiliging"), F("Temperatur Frostskydd"))
MAKE_PSTR_LIST(targetflowtemp,
               F("targetflowtemp"),
               F("target flow temperature"),
               F("berechnete Vorlauftemperatur"),
               F("Berekende aanvoertemperatuur"),
               F("Målvärde Flödestemperatur"))
MAKE_PSTR_LIST(heatingtype, F("heatingtype"), F("heating type"), F("Heizungstyp"), F("Verwarmingstype"), F("Uppvärmningstyp"))
MAKE_PSTR_LIST(summersetmode, F("summersetmode"), F("set summer mode"), F("Einstellung Sommerbetrieb"), F("Instelling zomerbedrijf"), F("Aktivera Sommarläge"))
MAKE_PSTR_LIST(hpoperatingmode,
               F("hpoperatingmode"),
               F("heatpump operating mode"),
               F("Wärmepumpe Betriebsmodus"),
               F("Bedrijfsmodus warmtepomp"),
               F("Värmepump Driftläge"))
MAKE_PSTR_LIST(hpoperatingstate, F("hpoperatingstate"), F("heatpump operating state"), F("WP Arbeitsweise"), F("Huidige modus warmtepomp"), F("Värmepump Driftstatus"))
MAKE_PSTR_LIST(controlmode, F("controlmode"), F("control mode"), F("Kontrollmodus"), F("Comtrolemodus"), F("Kontrolläge"))
MAKE_PSTR_LIST(control, F("control"), F("control device"), F("Fernsteuerung"), F("Afstandsbedieding"), F("Kontrollenhet"))
MAKE_PSTR_LIST(program, F("program"), F("program"), F("Programm"), F("Programma"), F("Program"))
MAKE_PSTR_LIST(pause, F("pause"), F("pause time"), F("Pausenzeit"), F("Pausetijd"), F("Paustid"))
MAKE_PSTR_LIST(party, F("party"), F("party time"), F("Partyzeit"), F("Partytijd"), F("Partytid"))
MAKE_PSTR_LIST(holidaytemp, F("holidaytemp"), F("holiday temperature"), F("Urlaubstemperatur"), F("Vakantietemperatuur"), F("Helgtemperatur"))
MAKE_PSTR_LIST(summermode, F("summermode"), F("summer mode"), F("Sommerbetrieb"), F("Zomerbedrijf"), F("Sommarläge"))
MAKE_PSTR_LIST(holidaymode, F("holidaymode"), F("holiday mode"), F("Urlaubsbetrieb"), F("Vakantiebedrijf"), F("Helgläge"))
MAKE_PSTR_LIST(flowtempoffset,
               F("flowtempoffset"),
               F("flow temperature offset for mixer"),
               F("Vorlauftemperaturanhebung"),
               F("Mixer aanvoertemperatuur offset"),
               F("Temperaturkorrigering Flödestemp. Blandningsventil"))
MAKE_PSTR_LIST(reducemode, F("reducemode"), F("reduce mode"), F("Absenkmodus"), F("Gereduceerde modus"), F("Reducerat Läge"))
MAKE_PSTR_LIST(noreducetemp,
               F("noreducetemp"),
               F("no reduce below temperature"),
               F("Absenkung unterbrechen unter"),
               F("Reduceermodus onderbreken onder"),
               F("Inaktivera reducering under"))
MAKE_PSTR_LIST(reducetemp, F("reducetemp"), F("off/reduce switch temperature"), F("Absenkmodus unter"), F("Onderste afschakeltemperatuur"), F("Avslag/Reducera under"))
MAKE_PSTR_LIST(vacreducetemp,
               F("vacreducetemp"),
               F("vacations off/reduce switch temperature"),
               F("Urlaub Absenkmodus unter"),
               F("Vakantiemodus onderste afschakeltemperatuur"),
               F("Helg Avslag/Reducering under"))
MAKE_PSTR_LIST(vacreducemode, F("vacreducemode"), F("vacations reduce mode"), F("Urlaub Absenkmodus"), F("Vakantie afschakelmodus"), F("Helg reduceringsläge"))
MAKE_PSTR_LIST(nofrostmode, F("nofrostmode"), F("nofrost mode"), F("Frostschutz Modus"), F("Vorstbeveiligingsmodus"), F("Frostskyddsläge"))
MAKE_PSTR_LIST(remotetemp,
               F("remotetemp"),
               F("room temperature from remote"),
               F("Raumtemperatur der Fernsteuerung"),
               F("Ruimtetemperatuur van afstandsbediening"),
               F("Rumstemperatur från fjärr"))

MAKE_PSTR_LIST(wwHolidays, F("wwholidays"), F("holiday dates"), F("Feiertage"), F("Feestdagen"), F("Helgdagar"))
MAKE_PSTR_LIST(wwVacations, F("wwvacations"), F("vacation dates"), F("Urlaubstage"), F("Vakantiedagen"), F("Semesterdatum Varmvatten"))
MAKE_PSTR_LIST(holidays, F("holidays"), F("holiday dates"), F("Feiertage"), F("Feestdagen"), F("Helgdatum"))
MAKE_PSTR_LIST(vacations, F("vacations"), F("vacation dates"), F("Urlaubstage"), F("Vakantiedagen"), F("Semesterdatum"))
MAKE_PSTR_LIST(wwprio, F("wwprio"), F("dhw priority"), F("WW-Vorrang"), F("Prioriteit warm water"), F("Prioritera Varmvatten"))
MAKE_PSTR_LIST(nofrostmode1, F("nofrostmode1"), F("nofrost mode"), F("Frostschutz"), F("Vorstbeveiligingsmodus"), F("Frostskyddsläge"))
MAKE_PSTR_LIST(reducehours, F("reducehours"), F("duration for nighttemp"), F("Dauer Nachttemp."), F("Duur nachtverlaging"), F("Timmar Nattsänkning"))
MAKE_PSTR_LIST(reduceminutes,
               F("reduceminutes"),
               F("remaining time for nightmode"),
               F("Restzeit Nachttemp."),
               F("Resterende tijd nachtverlaging"),
               F("Återstående Tid Nattläge"))
MAKE_PSTR_LIST(switchonoptimization,
               F("switchonoptimization"),
               F("switch-on optimization"),
               F("Schaltoptimierung"),
               F("Inschakeloptimalisering"),
               F("Växlingsoptimering"))

// heatpump
MAKE_PSTR_LIST(airHumidity, F("airhumidity"), F("relative air humidity"), F("relative Luftfeuchte"), F("Relatieve luchtvochtigheid"), F("Relativ Luftfuktighet"))
MAKE_PSTR_LIST(dewTemperature, F("dewtemperature"), F("dew point temperature"), F("Taupunkttemperatur"), F("Dauwpunttemperatuur"), F("Daggpunkt"))

// mixer
MAKE_PSTR_LIST(flowSetTemp,
               F("flowsettemp"),
               F("setpoint flow temperature"),
               F("Sollwert Vorlauftemperatur"),
               F("Streefwaarde aanvoertemperatuur"),
               F("Vald flödestemperatur"))
MAKE_PSTR_LIST(flowTempHc,
               F("flowtemphc"),
               F("flow temperature (TC1)"),
               F("Vorlauftemperatur des HK (TC1)"),
               F("Aanvoertemperatuut circuit (TC1)"),
               F("Flödestemperatur (TC1)"))
MAKE_PSTR_LIST(pumpStatus, F("pumpstatus"), F("pump status (PC1)"), F("Pumpenstatus des HK (PC1)"), F("pompstatus circuit (PC1)"), F("Pumpstatus (PC1)"))
MAKE_PSTR_LIST(mixerStatus,
               F("valvestatus"),
               F("mixing valve actuator (VC1)"),
               F("Mischerventil Position (VC1)"),
               F("positie mixerklep (VC1)"),
               F("Shuntventil Status (VC1)"))
MAKE_PSTR_LIST(flowTempVf,
               F("flowtempvf"),
               F("flow temperature in header (T0/Vf)"),
               F("Vorlauftemperatur am Verteiler (T0/Vf)"),
               F("aanvoertemperatuur verdeler (T0/Vf)"),
               F("Flödestemperatur Fördelare (T0/Vf)"))
MAKE_PSTR_LIST(mixerSetTime, F("valvesettime"), F("time to set valve"), F("Zeit zum Einstellen des Ventils"), F("Inschakeltijd mengklep"), F("Inställningstid Ventil"))
// mixer prefixed with wwc
MAKE_PSTR_LIST(wwPumpStatus,
               F("pumpstatus"),
               F("pump status in assigned wwc (PC1)"),
               F("Pumpenstatus des wwk (PC1)"),
               F("Pompstatus in WW circuit (PC1)"),
               F("Pumpstatus i VV-krets (PC1)"))
MAKE_PSTR_LIST(wwTempStatus,
               F("wwtempstatus"),
               F("temperature switch in assigned wwc (MC1)"),
               F("Temperaturschalter des wwk (MC1)"),
               F("Temperatuurschakeling in WW circuit (MC1)"),
               F("Temperaturventil i VV-krets (MC1)"))
MAKE_PSTR_LIST(wwTemp, F("wwtemp"), F("current temperature"), F("aktuelle Temperatur"), F("huidige temperatuur"), F("Aktuell Temperatur"))
// mixer pool
MAKE_PSTR_LIST(poolSetTemp, F("poolsettemp"), F("pool set temperature"), F("Pool Solltemperatur"), F("Streeftemperatuur zwembad"), F("Pool Temperatur Börvärde"))
MAKE_PSTR_LIST(poolTemp, F("pooltemp"), F("pool temperature"), F("Pool Temperatur"), F("Zwembadtemperatuur"), F("Pooltemperatur"))
MAKE_PSTR_LIST(poolShuntStatus,
               F("poolshuntstatus"),
               F("pool shunt status opening/closing"),
               F("Pool Ventil öffnen/schließen"),
               F("Zwembadklep status openen/sluiten"),
               F("Pool Shunt-status öppnen/stängd"))
MAKE_PSTR_LIST(poolShunt,
               F("poolshunt"),
               F("pool shunt open/close (0% = pool / 100% = heat)"),
               F("Pool Ventil Öffnung"),
               F("Mengklep zwembad stand"),
               F("Pool Shunt Öppen/Stängd"))
MAKE_PSTR_LIST(hydrTemp, F("hydrTemp"), F("hydraulic header temperature"), F("Verteilertemperatur"), F("Temperatuur open verdeler"), F("Fördelartemperatur"))

// solar
MAKE_PSTR_LIST(cylMiddleTemp,
               F("cylmiddletemp"),
               F("cylinder middle temperature (TS3)"),
               F("Speichertemperatur Mitte (TS3)"),
               F("Zonneboilertemperatuur midden (TS3)"),
               F("Cylindertemperatur Mitten (TS3)"))
MAKE_PSTR_LIST(retHeatAssist,
               F("retheatassist"),
               F("return temperature heat assistance (TS4)"),
               F("Rücklaufanhebungs-Temp. (TS4)"),
               F("Retourtemperatuur verwarmingsassistentie (TS4)"),
               F("Returtemperatur värmestöd (TS4)"))
MAKE_PSTR_LIST(m1Valve,
               F("heatassistvalve"),
               F("heat assistance valve (M1)"),
               F("Ventil Heizungsunterstützung (M1)"),
               F("Klep verwarmingsassistentie (M1)"),
               F("Uppvärmningsstöd Ventil (M1)"))
MAKE_PSTR_LIST(m1Power,
               F("heatassistpower"),
               F("heat assistance valve power (M1)"),
               F("Ventilleistung Heizungsunterstützung (M1)"),
               F("Vermogen klep verwarmingsassistentie (M1)"),
               F("Uppvärmningsstöd Ventil Effekt (M1)"))
MAKE_PSTR_LIST(pumpMinMod, F("pumpminmod"), F("minimum pump modulation"), F("minimale Pumpenmodulation"), F("Minimale pompmodulatie"), F("Min Pumpmodulering"))
MAKE_PSTR_LIST(maxFlow, F("maxflow"), F("maximum solar flow"), F("maximaler Durchfluss"), F("Maximale doorstroom solar"), F("Max Flöde Solpanel"))
MAKE_PSTR_LIST(solarPower, F("solarpower"), F("actual solar power"), F("aktuelle Solarleistung"), F("Huidig solar vermogen"), F("Aktuellt Sol-effekt"))
MAKE_PSTR_LIST(solarPumpTurnonDiff,
               F("turnondiff"),
               F("pump turn on difference"),
               F("Einschalthysterese Pumpe"),
               F("Inschakelhysterese pomp"),
               F("Aktiveringshysteres Pump"))
MAKE_PSTR_LIST(solarPumpTurnoffDiff,
               F("turnoffdiff"),
               F("pump turn off difference"),
               F("Ausschalthysterese Pumpe"),
               F("Uitschakelhysterese pomp"),
               F("Avslagshysteres Pump"))
MAKE_PSTR_LIST(pump2MinMod,
               F("pump2minmod"),
               F("minimum pump 2 modulation"),
               F("minimale Modulation Pumpe 2"),
               F("Minimale modulatie pomp 2"),
               F("Min Modulering Pump 2"))
MAKE_PSTR_LIST(solarPump2TurnonDiff,
               F("turnondiff2"),
               F("pump 2 turn on difference"),
               F("Einschalthysterese Pumpe 2"),
               F("Inschakelhysterese pomp 2"),
               F("Aktiveringshysteres Pump 2"))
MAKE_PSTR_LIST(solarPump2TurnoffDiff,
               F("turnoffdiff2"),
               F("pump 2 turn off difference"),
               F("Ausschalthysterese Pumpe 2"),
               F("Uitschakelhysterese pomp 2"),
               F("Avslagshysteres Pump 2"))

MAKE_PSTR_LIST(collectorTemp,
               F("collectortemp"),
               F("collector temperature (TS1)"),
               F("Kollektortemperatur (TS1)"),
               F("Collectortemperatuur (TS1)"),
               F("Kollektor Temperatur (TS1)"))
MAKE_PSTR_LIST(collector2Temp,
               F("collector2temp"),
               F("collector 2 temperature (TS7)"),
               F("Kollector 2 Temperatur (TS7)"),
               F("Collector 2 temperatuur (TS7)"),
               F("Kollektor 2 Temperatur (TS7)"))
MAKE_PSTR_LIST(cylBottomTemp,
               F("cylbottomtemp"),
               F("cylinder bottom temperature (TS2)"),
               F("Speicher Bodentemperatur (TS2)"),
               F("Bodemtemperatuur zonneboiler (TS2)"),
               F("Cylindertemperatur Botten (TS2)"))
MAKE_PSTR_LIST(cyl2BottomTemp,
               F("cyl2bottomtemp"),
               F("second cylinder bottom temperature (TS5)"),
               F("2. Speicher Bodentemperatur (TS5)"),
               F("Bodemtemperatuur 2e boiler"),
               F("Sekundär Cylindertemperatur Botten (TS5)"))
MAKE_PSTR_LIST(heatExchangerTemp,
               F("heatexchangertemp"),
               F("heat exchanger temperature (TS6)"),
               F("wärmetauscher Temperatur (TS6)"),
               F("Temperatuur warmtewisselaar (TS6)"),
               F("Värmeväxlare Temperatur (TS6)"))
MAKE_PSTR_LIST(collectorMaxTemp,
               F("collectormaxtemp"),
               F("maximum collector temperature"),
               F("maximale Kollektortemperatur"),
               F("Maximale collectortemperatuur"),
               F("Max Kollektortemperatur"))
MAKE_PSTR_LIST(collectorMinTemp,
               F("collectormintemp"),
               F("minimum collector temperature"),
               F("minimale Kollektortemperatur"),
               F("Minimale collectortemperatuur"),
               F("Min Kollektortemperatur"))
MAKE_PSTR_LIST(cylMaxTemp,
               F("cylmaxtemp"),
               F("maximum cylinder temperature"),
               F("maximale Speichertemperatur"),
               F("maximale temperatuur zonneboiler"),
               F("Max Cylindertemperatur"))
MAKE_PSTR_LIST(solarPumpMod, F("solarpumpmod"), F("pump modulation (PS1)"), F("Pumpenmodulation (PS1)"), F("Pompmodulatie (PS1)"), F("Pumpmodulering (PS1)"))
MAKE_PSTR_LIST(cylPumpMod,
               F("cylpumpmod"),
               F("cylinder pump modulation (PS5)"),
               F("Speicherpumpenmodulation (PS5)"),
               F("Modulatie zonneboilerpomp (PS5)"),
               F("Cylinderpumpmodulering (PS5)"))
MAKE_PSTR_LIST(solarPump, F("solarpump"), F("pump (PS1)"), F("Pumpe (PS1)"), F("Pomp (PS1)"), F("Pump (PS1)"))
MAKE_PSTR_LIST(solarPump2, F("solarpump2"), F("pump 2 (PS4)"), F("Pumpe 2 (PS4)"), F("Pomp 2 (PS4)"), F("Pump 2 (PS4)"))
MAKE_PSTR_LIST(solarPump2Mod, F("solarpump2mod"), F("pump 2 modulation (PS4)"), F("pump 2 modulation (PS4)"), F("Modulatie pomp 2 (PS4)"), F("Pump 2 Modulering (PS4)"))
MAKE_PSTR_LIST(valveStatus, F("valvestatus"), F("valve status"), F("Ventilstatus"), F("Klepstatus"), F("Ventilstatus"))
MAKE_PSTR_LIST(cylHeated, F("cylheated"), F("cyl heated"), F("Speichertemperatur erreicht"), F("Boilertemperatuur behaald"), F("Värmepanna Uppvärmd"))
MAKE_PSTR_LIST(collectorShutdown, F("collectorshutdown"), F("collector shutdown"), F("Kollektorabschaltung"), F("Collector afschakeling"), F("Kollektor Avstängning"))
MAKE_PSTR_LIST(pumpWorkTime, F("pumpworktime"), F("pump working time"), F("Pumpenlaufzeit"), F("Pomplooptijd"), F("Pump Drifttid"))
MAKE_PSTR_LIST(pump2WorkTime, F("pump2worktime"), F("pump 2 working time"), F("Pumpe 2 Laufzeit"), F("Looptijd pomp 2"), F("Pump 2 Drifttid"))
MAKE_PSTR_LIST(m1WorkTime,
               F("m1worktime"),
               F("differential control working time"),
               F("Differenzregelung Arbeitszeit"),
               F("Verschilregeling arbeidstijd"),
               F("Differentialreglering Drifttid"))
MAKE_PSTR_LIST(energyLastHour, F("energylasthour"), F("energy last hour"), F("Energie letzte Std"), F("Energie laatste uur"), F("Energi Senaste Timmen"))
MAKE_PSTR_LIST(energyTotal, F("energytotal"), F("total energy"), F("Gesamtenergie"), F("Totale energie"), F("Total Energi"))
MAKE_PSTR_LIST(energyToday, F("energytoday"), F("total energy today"), F("Energie heute"), F("Energie vandaag"), F("Total Energi Idag"))

// solar ww
MAKE_PSTR_LIST(cyl1, F("cyl 1"), F("Zyl_1"), F("Cil 1"), F("Cyl 1"))
MAKE_PSTR_LIST(cyl2, F("cyl 2"), F("Zyl_2"), F("Cil 2"), F("Cyl 2"))
MAKE_PSTR_LIST(wwTemp1, F("wwtemp1"), F("temperature 1"), F("Temperatur 1"), F("Temperatuur 1"), F("Temperatur 1"))
MAKE_PSTR_LIST(wwTemp3, F("wwtemp3"), F("temperature 3"), F("Temperatur 3"), F("Temperatuur 2"), F("Temperatur 2"))
MAKE_PSTR_LIST(wwTemp4, F("wwtemp4"), F("temperature 4"), F("Temperatur 4"), F("Temperatuur 3"), F("Temperatur 3"))
MAKE_PSTR_LIST(wwTemp5, F("wwtemp5"), F("temperature 5"), F("Temperatur 5"), F("Temperatuur 5"), F("Temperatur 4"))
MAKE_PSTR_LIST(wwTemp7, F("wwtemp7"), F("temperature 7"), F("Temperatur 7"), F("Temperatuur 7"), F("Temperatur 5"))
MAKE_PSTR_LIST(wwPump, F("wwpump"), F("pump"), F("Pumpe"), F("Pomp"), F("Pump"))
// solar ww and mixer wwc
MAKE_PSTR_LIST(wwMinTemp, F("wwmintemp"), F("minimum temperature"), F("minimale Temperatur"), F("Minimale temperatuur"), F("Min Temperatur"))
MAKE_PSTR_LIST(wwRedTemp, F("wwredtemp"), F("reduced temperature"), F("redizierte Temperatur"), F("Gereduceerde temperatuur"), F("Reducerad Temperatur"))
MAKE_PSTR_LIST(wwDailyTemp, F("wwdailytemp"), F("daily temperature"), F("tägl. Temperatur"), F("Dagelijkse temperatuur"), F("Daglig temperatur"))
MAKE_PSTR_LIST(wwKeepWarm, F("wwkeepwarm"), F("keep warm"), F("Warmhalten"), F("Warm houde"), F("Varmhållning"))
MAKE_PSTR_LIST(wwStatus2, F("wwstatus2"), F("status 2"), F("Status 2"), F("Status 2"), F("Status 2"))
MAKE_PSTR_LIST(wwPumpMod, F("wwpumpmod"), F("pump modulation"), F("Pumpen Modulation"), F("Pompmodulatie"), F("Pumpmodulering"))
MAKE_PSTR_LIST(wwFlow, F("wwflow"), F("flow rate"), F("Flussrate"), F("Doorstroomsnelheid"), F("Flöde"))
// extra mixer ww
MAKE_PSTR_LIST(wwRequiredTemp, F("wwrequiredtemp"), F("required temperature"), F("benötigte Temperatur"), F("Benodigde temperatuur"), F("Nödvändig Temperatur"))
MAKE_PSTR_LIST(wwDiffTemp,
               F("wwdifftemp"),
               F("start differential temperature"),
               F("Start Differential Temperatur"),
               F("Start differentiele temperatuur"),
               F("Start Differentialtemperatur"))

// SM100
MAKE_PSTR_LIST(heatTransferSystem,
               F("heattransfersystem"),
               F("heattransfer system"),
               F("Wärmetransfer System"),
               F("Warmteoverdrachtssysteem"),
               F("Värmeöverföringssystem"))
MAKE_PSTR_LIST(externalCyl, F("externalcyl"), F("external cylinder"), F("Externer Speicher"), F("Externe boiler"), F("Extern Cylinder"))
MAKE_PSTR_LIST(thermalDisinfect, F("thermaldisinfect"), F("thermal disinfection"), F("Thermische Desinfektion"), F("Thermische desinfectie"), F("Termisk Desinfektion"))
MAKE_PSTR_LIST(heatMetering, F("heatmetering"), F("heatmetering"), F("Wärmemessung"), F("warmtemeting"), F("Värmemätning"))
MAKE_PSTR_LIST(solarIsEnabled, F("solarenabled"), F("solarmodule enabled"), F("Solarmodul aktiviert"), F("Solarmodule geactiveerd"), F("Solmodul Aktiverad"))

// telegram 0x035A
MAKE_PSTR_LIST(solarPumpMode, F("solarpumpmode"), F("pump mode"), F("solar Pumpen Einst."), F("Modus zonneboilerpomp"), F("Sol Pumpläge"))
MAKE_PSTR_LIST(solarPumpKick, F("pumpkick"), F("pump kick"), F("Röhrenkollektorfunktion"), F("Modus buizencollector"), F("Sol Kollektorfunktion"))
MAKE_PSTR_LIST(plainWaterMode, F("plainwatermode"), F("plain water mode"), F("Südeuropafunktion"), F("Modus Zuid-Europa"), F("Sydeuropa-funktion"))
MAKE_PSTR_LIST(doubleMatchFlow, F("doublematchflow"), F("doublematchflow"), F("Double Match Flow"), F("Double Match Flow"), F("Dubbelmatchning Flöde"))
MAKE_PSTR_LIST(solarPump2Mode, F("pump2mode"), F("pump 2 mode"), F("Pumpe 2 Modus"), F("Modus pomp 2"), F("Pump 2 Läge"))
MAKE_PSTR_LIST(solarPump2Kick, F("pump2kick"), F("pump kick 2"), F("Pumpe 2 Startboost"), F("Startboost pomp 2"), F("Pump 2 Kollektorfunktion"))

// telegram 0x035F
MAKE_PSTR_LIST(cylPriority, F("cylpriority"), F("cylinder priority"), F("Speicher Priorität"), F("Prioriteit boiler"), F("Cylinderprioritering"))

// telegram 0x380
MAKE_PSTR_LIST(climateZone, F("climatezone"), F("climate zone"), F("Klimazone"), F("klimaatzone"), F("Klimatzon"))
MAKE_PSTR_LIST(collector1Area, F("collector1area"), F("collector 1 area"), F("Kollektor 1 Fläche"), F("oppervlakte collector 1"), F("Kollektor 1 Area"))
MAKE_PSTR_LIST(collector1Type, F("collector1type"), F("collector 1 type"), F("Kollektor 1 Typ"), F("Type collector 1"), F("Kollektor 1 Typ"))
MAKE_PSTR_LIST(collector2Area, F("collector2area"), F("collector 2 area"), F("Kollektor 2 Fläche"), F("Oppervlakte collector 2"), F("Kollektor 2 Area"))
MAKE_PSTR_LIST(collector2Type, F("collector2type"), F("collector 2 type"), F("Kollektor 2 Typ"), F("Type collector 2"), F("Kollektor 2 Typ"))

// telegram 0x0363 heatCounter
MAKE_PSTR_LIST(heatCntFlowTemp,
               F("heatcntflowtemp"),
               F("heat counter flow temperature"),
               F("Wärmezähler Vorlauf-Temperatur"),
               F("Aanvoertemperatuur warmteenergiemeter"),
               F("Värmeräknare Flödestemperatur"))
MAKE_PSTR_LIST(heatCntRetTemp,
               F("heatcntrettemp"),
               F("heat counter return temperature"),
               F("Wärmezähler Rücklauf-Temperatur"),
               F("Retourtemperatuur warmteenergiemeter"),
               F("Värmeräknare Returtemperatur"))
MAKE_PSTR_LIST(heatCnt, F("heatcnt"), F("heat counter impulses"), F("Wärmezähler Impulse"), F("Warmteenergiemeter pulsen"), F("Värmeräknare Impuls"))
MAKE_PSTR_LIST(swapFlowTemp,
               F("swapflowtemp"),
               F("swap flow temperature (TS14)"),
               F("Austausch Vorlauf-Temperatur (TS14)"),
               F("Aanvoertemperatuur verwisselaar (TS14)"),
               F("Växlingstemperatur Flöde (TS14)"))
MAKE_PSTR_LIST(swapRetTemp,
               F("swaprettemp"),
               F("swap return temperature (TS15)"),
               F("Austausch Rücklauf-Temperatur (TS15)"),
               F("Retourtemperatuur verwisselaar (TS15)"),
               F("Växlingstemperatur Returflöde (TS15)"))

// switch
MAKE_PSTR_LIST(activated, F("activated"), F("activated"), F("Aktiviert"), F("Geactiveerd"), F("Aktiverad"))
MAKE_PSTR_LIST(status, F("status"), F("status"), F("Status"), F("Status"), F("Status"))

// RF sensor, id 0x40, telegram 0x435
MAKE_PSTR_LIST(RFTemp, F("rftemp"), F("RF room temperature sensor"), F("RF Raumtemperatur Sensor"), F("RF ruimtetemperatuur sensor"), F("RF Rumsgivare Temp"))

/*
// unknown fields to track (SM10), only for testing
// **** NO TRANSLATION NEEDED ****
MAKE_PSTR_LIST(data11, F("data11"), F("unknown datafield 11"))
MAKE_PSTR_LIST(data12, F("data12"), F("unknown datafield 12"))
MAKE_PSTR_LIST(data8, F("data8"), F("unknown datafield 8"))
MAKE_PSTR_LIST(data0, F("data0"), F("unknown datafield 0"))
MAKE_PSTR_LIST(data1, F("data1"), F("unknown datafield 1"))
MAKE_PSTR_LIST(setting3, F("setting3"), F("unknown setting 3"))
MAKE_PSTR_LIST(setting4, F("setting4"), F("unknown setting 4"))
*/
