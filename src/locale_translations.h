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

// clang-format off

// Define languages here
// Makes sure they are also added in same order to languages[] in system.cpp
#define EMSESP_LOCALE_EN "en"
#define EMSESP_LOCALE_DE "de"
#define EMSESP_LOCALE_NL "nl"
#define EMSESP_LOCALE_SV "sv"
#define EMSESP_LOCALE_PL "pl"
#define EMSESP_LOCALE_NO "no"
#define EMSESP_LOCALE_FR "fr"

// translations are in order en, de, nl, sv, pl, no, fr, ....
// if there is no translation, it will default to en

// device types, as display in Web and Console
// TODO translate
MAKE_PSTR_LIST(boiler_device, "Boiler", "Kessel", "Boiler")
MAKE_PSTR_LIST(thermostat_device, "Thermostat", "Thermostat", "Thermostaat")
MAKE_PSTR_LIST(heatpump_device, "Heat Pump", "Wärmepumpe", "Warmtepomp")
MAKE_PSTR_LIST(solar_device, "Solar Module", "Solarmodul", "Solar Module")
MAKE_PSTR_LIST(connect_device, "Connect Module", "Verbindungsmodul", "Connect Module")
MAKE_PSTR_LIST(mixer_device, "Mixer Module", "Mischermodul", "Mixer Module")
MAKE_PSTR_LIST(controller_device, "Controller Module", "Kontrollmodul", "Controller Module")
MAKE_PSTR_LIST(switch_device, "Switch Module", "Schaltmodul", "Switch Module")
MAKE_PSTR_LIST(gateway_device, "Gateway Module", "Gateway Modul", "Gateway Module")
MAKE_PSTR_LIST(alert_device, "Alert Module", "Alarmmodul", "Alert Module")
MAKE_PSTR_LIST(pump_device, "Pump Module", "Pumpenmodul", "Pump Module")
MAKE_PSTR_LIST(heatsource_device, "Heatsource", "Heizquelle", "Heatsource")
MAKE_PSTR_LIST(sensors_device, "Sensors", "Sensoren", "Sensoren", "Sensorer", "czujniki", "Sensorer", "Capteurs")

// commands
MAKE_PSTR_LIST(info_cmd, "lists all values", "Liste aller Werte")
MAKE_PSTR_LIST(commands_cmd, "lists all commands", "Liste aller Kommandos")
MAKE_PSTR_LIST(entities_cmd, "lists all entities", "Liste aller Entitäten")
MAKE_PSTR_LIST(send_cmd, "send a telegram", "Sende EMS-Telegramm")
MAKE_PSTR_LIST(setiovalue_cmd, "set io value", "Setze Wertevorgabe")
MAKE_PSTR_LIST(changeloglevel_cmd, "change log level", "Ändere Sysloglevel")
MAKE_PSTR_LIST(fetch_cmd, "refresh all EMS values", "Lese alle EMS-Werte neu")
MAKE_PSTR_LIST(restart_cmd, "restart EMS-ESP", "Neustart")
MAKE_PSTR_LIST(watch_cmd, "watch incoming telegrams", "Watch auf eingehende Telegramme")
MAKE_PSTR_LIST(publish_cmd, "publish all to MQTT", "Publiziere MQTT")
MAKE_PSTR_LIST(system_info_cmd, "show system status", "Zeige System-Status")

#if defined(EMSESP_DEBUG)
MAKE_PSTR_LIST(test_cmd, "run a test")
#endif

// TAG mapping - maps to DeviceValueTAG_s in emsdevice.cpp
// use empty string if want to suppress showing tags
// mqtt tags must not have spaces
MAKE_PSTR_LIST(tag_none, "")
MAKE_PSTR_LIST(tag_heartbeat, "")
MAKE_PSTR_LIST(tag_boiler_data_ww, "dhw", "WW", "dhw", "VV", "CWU", "dhw", "ecs")
MAKE_PSTR_LIST(tag_device_data, "")
MAKE_PSTR_LIST(tag_device_data_ww, "dhw", "WW", "dhw", "VV", "CWU", "dhw", "ecs")
MAKE_PSTR_LIST(tag_hc1, "hc1", "HK1", "hc1", "VK1", "OG1", "hc1", "hc1")
MAKE_PSTR_LIST(tag_hc2, "hc2", "HK2", "hc2", "VK2", "OG2", "hc2", "hc2")
MAKE_PSTR_LIST(tag_hc3, "hc3", "HK3", "hc3", "VK3", "OG3", "hc3", "hc3")
MAKE_PSTR_LIST(tag_hc4, "hc4", "HK4", "hc4", "VK4", "OG4", "hc4", "hc4")
MAKE_PSTR_LIST(tag_hc5, "hc5", "HK5", "hc5", "VK5", "OG5", "hc5", "hc5")
MAKE_PSTR_LIST(tag_hc6, "hc6", "HK6", "hc6", "vk6", "OG6", "hc6", "hc6")
MAKE_PSTR_LIST(tag_hc7, "hc7", "HK7", "hc7", "VK7", "OG7", "hc7", "hc7")
MAKE_PSTR_LIST(tag_hc8, "hc8", "HK8", "hc8", "VK8", "OG8", "hc8", "hc8")
MAKE_PSTR_LIST(tag_wwc1, "wwc1", "WWK1", "wwc1", "VVK1", "CWU1", "wwc1", "wwc1")
MAKE_PSTR_LIST(tag_wwc2, "wwc2", "WWK2", "wwc2", "VVK2", "CWU2", "wwc2", "wwc2")
MAKE_PSTR_LIST(tag_wwc3, "wwc3", "WWK3", "wwc3", "VVK3", "CWU3", "wwc3", "wwc3")
MAKE_PSTR_LIST(tag_wwc4, "wwc4", "WWK4", "wwc4", "VVK4", "CWU4", "wwc4", "wwc4")
MAKE_PSTR_LIST(tag_wwc5, "wwc5", "WWK5", "wwc5", "VVK5", "CWU5", "wwc5", "wwc5")
MAKE_PSTR_LIST(tag_wwc6, "wwc6", "WWK6", "wwc6", "VVK6", "CWU6", "wwc6", "wwc6")
MAKE_PSTR_LIST(tag_wwc7, "wwc7", "WWK7", "wwc7", "VVK7", "CWU7", "wwc7", "wwc7")
MAKE_PSTR_LIST(tag_wwc8, "wwc8", "WWK8", "wwc8", "VVK8", "CWU8", "wwc8", "wwc8")
MAKE_PSTR_LIST(tag_wwc9, "wwc9", "WWK9", "wwc9", "VVK9", "CWU9", "wwc9", "wwc9")
MAKE_PSTR_LIST(tag_wwc10, "wwc10", "WWK10", "wwc10", "VVK10", "CWU10", "wwc10", "wwc10")
MAKE_PSTR_LIST(tag_ahs1, "ahs1", "AHQ1", "ahs1", "AVK1", "AŹC1", "ahs1", "ahs1")
MAKE_PSTR_LIST(tag_hs1, "hs1", "hs1", "hs1", "VK1", "ŹC1", "hs1", "hs1")
MAKE_PSTR_LIST(tag_hs2, "hs2", "hs2", "hs2", "VK2", "ŹC2", "hs2", "hs2")
MAKE_PSTR_LIST(tag_hs3, "hs3", "hs3", "hs3", "VK3", "ŹC3", "hs3", "hs3")
MAKE_PSTR_LIST(tag_hs4, "hs4", "hs4", "hs4", "VK4", "ŹC4", "hs4", "hs4")
MAKE_PSTR_LIST(tag_hs5, "hs5", "hs5", "hs5", "VK5", "ŹC5", "hs5", "hs5")
MAKE_PSTR_LIST(tag_hs6, "hs6", "hs6", "hs6", "VK6", "ŹC6", "hs6", "hs6")
MAKE_PSTR_LIST(tag_hs7, "hs7", "hs7", "hs7", "VK7", "ŹC7", "hs7", "hs7")
MAKE_PSTR_LIST(tag_hs8, "hs8", "hs8", "hs8", "VK8", "ŹC8", "hs8", "hs8")
MAKE_PSTR_LIST(tag_hs9, "hs9", "hs9", "hs9", "VK9", "ŹC9", "hs9", "hs9")
MAKE_PSTR_LIST(tag_hs10, "hs10", "hs10", "hs10", "VK10", "ŹC10", "hs10", "hs10")
MAKE_PSTR_LIST(tag_hs11, "hs11", "hs11", "hs11", "VK11", "ŹC11", "hs11", "hs11")
MAKE_PSTR_LIST(tag_hs12, "hs12", "hs12", "hs12", "VK12", "ŹC12", "hs12", "hs12")
MAKE_PSTR_LIST(tag_hs13, "hs13", "hs13", "hs13", "VK13", "ŹC13", "hs13", "hs13")
MAKE_PSTR_LIST(tag_hs14, "hs14", "hs14", "hs14", "VK14", "ŹC14", "hs14", "hs14")
MAKE_PSTR_LIST(tag_hs15, "hs15", "hs15", "hs15", "VK15", "ŹC15", "hs15", "hs15")
MAKE_PSTR_LIST(tag_hs16, "hs16", "hs16", "hs16", "VK16", "ŹC16", "hs16", "hs16")

// General
MAKE_PSTR_LIST(on, "on", "an", "aan", "på", "włączono", "på", "on")
MAKE_PSTR_LIST(off, "off", "aus", "uit", "av", "wyłączono", "av", "off")
MAKE_PSTR_LIST(ON, "ON", "AN", "AAN", "PÅ", "wł.", "PÅ", "ON")
MAKE_PSTR_LIST(OFF, "OFF", "AUS", "UIT", "AV", "wył.", "AV", "OFF")

// Unit Of Measurement mapping - maps to DeviceValueUOM_s in emsdevice.cpp
// uom - also used with HA see https://github.com/home-assistant/core/blob/d7ac4bd65379e11461c7ce0893d3533d8d8b8cbf/homeassistant/const.py#L384
MAKE_PSTR_LIST(minutes, "minutes", "Minuten", "Minuten", "Minuter", "minut", "Minutter", "minutes")
MAKE_PSTR_LIST(hours, "hours", "Stunden", "Uren", "Timmar", "godzin", "Timer", "heures")
MAKE_PSTR_LIST(days, "days", "Tage", "Dagen", "Dagar", "dni", "Dager", "jours")
MAKE_PSTR_LIST(seconds, "seconds", "Sekunden", "Seconden", "Sekunder", "sekund", "Sekunder", "secondes")

// Enum translations
// general
MAKE_PSTR_LIST(day_mo, "mo", "Mo", "Mo", "Må", "poniedziałek", "Ma", "lun")
MAKE_PSTR_LIST(day_tu, "tu", "Di", "Di", "Ti", "wtorek", "Ti", "mar")
MAKE_PSTR_LIST(day_we, "we", "Mi", "Wo", "On", "środa", "On", "mer")
MAKE_PSTR_LIST(day_th, "th", "Do", "Do", "To", "czwartek", "To", "jeu")
MAKE_PSTR_LIST(day_fr, "fr", "Fr", "Vr", "Fr", "piątek", "Fr", "ven")
MAKE_PSTR_LIST(day_sa, "sa", "Sa", "Za", "Lö", "sobota", "Lø", "sam")
MAKE_PSTR_LIST(day_su, "su", "So", "Zo", "Sö", "niedziela", "Sø", "dim")
MAKE_PSTR_LIST(all, "all", "Alle", "Alle", "Alla", "codziennie", "alle", "tous")
MAKE_PSTR_LIST(own_1, "own 1", "Eigen 1", "Eigen 1", "Egen 1", "własny 1", "Egen 1", "propre 1")
MAKE_PSTR_LIST(family, "family", "Familie", "Familie", "Familj", "rodzina", "familie", "famille")
MAKE_PSTR_LIST(morning, "morning", "Morgends", "'s ochtends", "Morgon", "zmiana 1", "morgen", "matin")
MAKE_PSTR_LIST(evening, "evening", "Abends", "'s avonds", "Kväll", "zmiana 2", "kveld", "soir")
MAKE_PSTR_LIST(seniors, "seniors", "Senioren", "Senioren", "Seniorer", "senior", "seniorer", "séniors")
MAKE_PSTR_LIST(no, "no", "nein", "nee", "nej", "nie", "nei", "non")
MAKE_PSTR_LIST(new, "new", "Neu", "Nieuw", "Ny", "nowy", "ny", "nouveau")
MAKE_PSTR_LIST(own_2, "own 2", "Eigen 2", "Eigen 2", "Egen 2", "własny 2", "egen 2", "propre 2")
MAKE_PSTR_LIST(singles, "singles", "Singles", "Singles", "Singlar", "osoba samotna", "single", "seuls")
MAKE_PSTR_LIST(am, "am", "Vormittag", "Ochtend", "Förmiddag", "do południa", "formiddag", "matin")
MAKE_PSTR_LIST(pm, "pm", "Nachmittag", "Namiddag", "Eftermiddag", "po południu", "ettermiddag", "après-midi")
MAKE_PSTR_LIST(midday, "midday", "Mittag", "Middag", "Middag", "południe", "middag", "midi")
MAKE_PSTR_LIST(unknown, "unknown", "Unbekannt", "Onbekend", "Okänt", "nieznany", "ukjent", "inconnu")
MAKE_PSTR_LIST(flat, "flat", "flach", "vlak", "Platt", "płaski", "flat", "plat")
MAKE_PSTR_LIST(vacuum, "vacuum", "Vakuum", "vacuum", "Vakuum", "próżnia", "vakum", "vide")
MAKE_PSTR_LIST(co2_optimized, "co2 optimized", "CO2 optimiert", "CO2 geoptimaliseerd", "CO2-optimerad", "optymalizacja CO2", "co2 optimalisert", "optimisé en CO2")
MAKE_PSTR_LIST(cost_optimized, "cost optimized", "kostenoptimiert", "kosten geoptimaliseerd", "kostnadsoptimerad", "optymalizacja kosztów", "kostnadsoptimalisert", "optimisé en coût")
MAKE_PSTR_LIST(outside_temp_switched, "outside temp switched", "Außentemp. gesteuert", "Buitentemp. gestuurd", "Utomhustemp korrigerad", "temperatura zewn. przeł.", "utetemp optimalisert", "contrôle par temp. ext.")
MAKE_PSTR_LIST(co2_cost_mix, "co2 cost mix", "Kostenmix", "Kostenmix", "Kostnadsmix", "mieszany koszt CO2", "", "coût mixte CO2") // TODO translate
MAKE_PSTR_LIST(analog, "analog", "analog", "analoog", "analog", "analogowy", "analog", "analogique")
MAKE_PSTR_LIST(normal, "normal", "normal", "normaal", "normal", "normalny", "normal", "normal")
MAKE_PSTR_LIST(blocking, "blocking", "Blockierung", "Blokkering", "Blockering", "blokowanie", "blokkering", "bloquant")
MAKE_PSTR_LIST(extern, "extern", "extern", "extern", "extern", "zewnętrzny", "ekstern", "externe")
MAKE_PSTR_LIST(intern, "intern", "intern", "intern", "intern", "wewnętrzny", "intern", "interne")
MAKE_PSTR_LIST(lower, "lower", "niedirger", "lager", "lägre", "mniejszy", "nedre", "inférieur")
MAKE_PSTR_LIST(error, "error", "Fehler", "error", "error", "błąd", "", "erreur") // TODO translate
MAKE_PSTR_LIST(na, "n/a", "n/a", "n/a", "n/a", "nd.", "", "n/c") // TODO translate

// boiler
MAKE_PSTR_LIST(time, "time", "Zeit", "Tijd", "Tid", "godzina", "tid", "heure")
MAKE_PSTR_LIST(date, "date", "Datum", "Datum", "Datum", "data", "dato", "date")
MAKE_PSTR_LIST(continuous, "continuous", "kontinuierlich", "continue", "kontinuerlig", "ciągły", "kontinuerlig", "continu")
MAKE_PSTR_LIST(3wayvalve, "3-way valve", "3-Wege Ventil", "3-weg klep", "trevägsventil", "zawór 3-drogowy", "treveisventil", "vanne 3 voies")
MAKE_PSTR_LIST(chargepump, "chargepump", "Ladepumpe", "laadpomp", "laddpump", "pompa ładująca", "ladepumpe", "pompe de charge")
MAKE_PSTR_LIST(hot, "hot", "Heiß", "Heet", "Het", "gorący", "het", "chaud")
MAKE_PSTR_LIST(high_comfort, "high comfort", "gehobener Komfort", "Verhoogd comfort", "Förhöjd komfort", "wysoki komfort", "høy komfort", "comfort")
MAKE_PSTR_LIST(eco, "eco", "Eco", "Eco", "Eko", "eko", "øko", "éco")
MAKE_PSTR_LIST(intelligent, "intelligent", "Intelligent", "Intelligent", "Intelligent", "inteligentny", "intelligent", "intelligent")
MAKE_PSTR_LIST(flow, "flow", "Durchfluss", "Volumestroom", "Flöde", "przepływ", "strømme", "débit")
MAKE_PSTR_LIST(manual, "manual", "Manuell", "Hamdmatig", "Manuell", "ręczny", "manuell", "manuel")
MAKE_PSTR_LIST(buffer, "buffer", "Speicher", "Buffer", "Buffert", "bufor", "buffer", "buffer")
MAKE_PSTR_LIST(bufferedflow, "buffered flow", "Durchlaufspeicher", "Doorstroombuffer", "Buffertflöde", "przepływ buforowany", "bufret strømning", "") // TODO translate
MAKE_PSTR_LIST(layeredbuffer, "layered buffer", "Schichtspeicher", "Gelaagde buffer", "Lagrad buffert", "bufor warstwowy", "lagdelt buffer", "") // TODO translate
MAKE_PSTR_LIST(maintenance, "maintenance", "Wartung", "Onderhoud", "Underhåll", "przegląd", "vedlikehold", "maintenance")
MAKE_PSTR_LIST(heating, "heating", "Heizen", "Verwarmen", "Uppvärmning", "ogrzewanie", "oppvarming", "chauffage")
MAKE_PSTR_LIST(cooling, "cooling", "Kühlen", "Koelen", "Kyler", "chłodzenie", "kjøling", "refroidissement")
MAKE_PSTR_LIST(disinfecting, "disinfecting", "Desinfizieren", "Desinfecteren", "Desinficerar", "dezynfekcja termiczna", "desinfisering", "désinfection")
MAKE_PSTR_LIST(no_heat, "no heat", "keine Wärme", "Geen warmte", "Ingen värme", "brak ciepła", "ingen varme", "pas de chauffage")
MAKE_PSTR_LIST(heatrequest, "heat request", "Wärmeanforderung", "Verwarmignsverzoek", "Värmeförfrågan", "zapotrzebowanie na ciepło", "varmeforespørsel", "demande de chauffage")
MAKE_PSTR_LIST(valve, "valve", "Ventil", "Klep", "Ventil", "zawór", "ventil", "valve")

// heatpump
MAKE_PSTR_LIST(none, "none", "keine", "geen", "ingen", "brak", "ingen", "aucun")
MAKE_PSTR_LIST(hot_water, "hot water", "Warmwasser", "warm water", "varmvatten", "c.w.u.", "varmtvann", "eau chaude")
MAKE_PSTR_LIST(pool, "pool", "Pool", "zwembad", "pool", "basen", "basseng", "piscine")
MAKE_PSTR_LIST(outside_temp_alt, "outside temperature alt.", "Außentemp. alternativ", "", "", "temp. zewn. alternat.", "", "température extérieure alternative") // TODO translate
MAKE_PSTR_LIST(outside_temp_par, "outside temperature parallel", "Außentemp. parallel", "", "", "temp. zewn. równoległa", "", "température extérieure parallèle") // TODO translate
MAKE_PSTR_LIST(hp_prefered, "heatpump prefered", "Wärmepumpe bevorzugt", "", "", "preferowana pompa ciepła", "", "pompe à chaleur préférée") // TODO translate
MAKE_PSTR_LIST(boiler_only, "boiler only", "nur Kessel", "", "", "tylko kocioł", "", "chaudière uniquement") // TODO translate
MAKE_PSTR_LIST(reduced_output, "reduced output", "Reduzierte Leistung", "", "", "zmniejszona wydajność", "", "sortie réduite") // TODO translate
MAKE_PSTR_LIST(switchoff, "switch off hp", "WP ausschalten", "", "", "wyłącz pompę ciepła", "", "éteindre la PAC") // TODO translate
MAKE_PSTR_LIST(perm, "perm. reduced", "perm. reduziert", "", "", "stale zmniejszona wydajność", "", "réduction permanente") // TODO translate

// thermostat
MAKE_PSTR_LIST(seltemp, "selTemp", "Solltemperatur", "Doeltemperatuur", "Börtemperatur", "temperatura zadana", "innstilt temperatur", "consigne température")
MAKE_PSTR_LIST(roomtemp, "roomTemp", "Raumtemperatur", "Kamertemperatuur", "Rumstemperatur", "temperatura w pomieszczeniu", "romstemperatur", "température de la pièce")
MAKE_PSTR_LIST(own_prog, "own prog", "Eigenprog.", "Eigen prog.", "Egen prog.", "program własny", "eget prog.", "programme propre")
MAKE_PSTR_LIST(std_prog, "std prog", "Standardprog.", "Standaard prog.", "Standardprog.", "program standardowy", "standardprog.", "programme standard")
MAKE_PSTR_LIST(light, "light", "Leicht", "Licht", "Lätt", "lekki", "lett", "léger")
MAKE_PSTR_LIST(medium, "medium", "Mittel", "Middel", "Medel", "średni", "medium", "medium")
MAKE_PSTR_LIST(heavy, "heavy", "Schwer", "Zwaar", "Tung", "ciężki", "tung", "lourd")
MAKE_PSTR_LIST(start, "start", "Start", "Start", "Start", "start", "start", "début")
MAKE_PSTR_LIST(heat, "heat", "Heizen", "Verwarmen", "Värme", "ciepło", "varmer", "chaleur")
MAKE_PSTR_LIST(hold, "hold", "Halten", "Pauzeren", "Paus", "pauza", "pause", "pause")
MAKE_PSTR_LIST(cool, "cool", "Kühlen", "Koelen", "Kyla", "zimno", "kjøler", "froid")
MAKE_PSTR_LIST(end, "end", "Ende", "Einde", "Slut", "koniec", "slutt", "fin")
MAKE_PSTR_LIST(german, "german", "Deutsch", "Duits", "Tyska", "niemiecki", "tysk", "allemand")
MAKE_PSTR_LIST(dutch, "dutch", "Niederländisch", "Nederlands", "Nederländska", "niderlandzki", "nederlandsk", "néerlandais")
MAKE_PSTR_LIST(french, "french", "Französisch", "Frans", "Franska", "francuski", "fransk", "français")
MAKE_PSTR_LIST(italian, "italian", "Italienisch", "Italiaans", "Italienska", "włoski", "italiensk", "italien")
MAKE_PSTR_LIST(high, "high", "hoch", "hoog", "Hög", "wysoki", "", "haut") // TODO translate
MAKE_PSTR_LIST(low, "low", "niedrig", "laag", "Låg", "niski", "lav", "bas")
MAKE_PSTR_LIST(radiator, "radiator", "Heizkörper", "Radiator", "Radiator", "grzejniki", "radiator", "radiateur")
MAKE_PSTR_LIST(convector, "convector", "Konvektor", "Convector", "Konvektor", "konwektory", "konvektor", "convecteur")
MAKE_PSTR_LIST(floor, "floor", "Fussboden", "Vloer", "Golv", "podłoga", "gulv", "sol")
MAKE_PSTR_LIST(summer, "summer", "Sommer", "Zomer", "Sommar", "lato", "sommer", "été")
MAKE_PSTR_LIST(winter, "winter", "Winter", "Winter", "Vinter", "zima", "vinter", "hiver")
MAKE_PSTR_LIST(outdoor, "outdoor", "Außen", "Buiten", "Utomhus", "temp. zewnętrzna", "utendørs", "extérieur")
MAKE_PSTR_LIST(room, "room", "Raum", "Kamer", "Rum", "temp. w pomieszczeniu", "", "pièce") // TODO translate
MAKE_PSTR_LIST(room_outdoor, "room outdoor", "Raum+Außen", "Kamer+Buiten", "Rum+Ute", "temp. w pom. i zewn.", "rom utendørs", "pièce extérieure")
MAKE_PSTR_LIST(power, "power", "Leistung", "Vermogen", "Effekt", "moc", "effekt", "puissance")
MAKE_PSTR_LIST(constant, "constant", "konstant", "constant", "Konstant", "stały", "konstant", "constant")
MAKE_PSTR_LIST(simple, "simple", "einfach", "simpel", "enkel", "prosty", "enkel", "simple")
MAKE_PSTR_LIST(optimized, "optimized", "optimiert", "geoptimaliseerd", "optimerad", "zoptymalizowany", "optimalisert", "optimisé")
MAKE_PSTR_LIST(nofrost, "nofrost", "Frostschutz", "Vorstbescherming", "Frostskydd", "ochrona przed zamarzaniem", "frostsikring", "protection gel")
MAKE_PSTR_LIST(defrost, "defrost", "Abtauen", "ontdooien", "avfrostning", "rozmrażać", "tine", "dégivrage")
MAKE_PSTR_LIST(comfort, "comfort", "Komfort", "Comfort", "Komfort", "komfort", "komfort", "comfort")
MAKE_PSTR_LIST(night, "night", "Nacht", "Nacht", "Natt", "noc", "natt", "nuit")
MAKE_PSTR_LIST(day, "day", "Tag", "Dag", "Dag", "dzień", "dag", "jour")
MAKE_PSTR_LIST(holiday, "holiday", "Urlaub", "Vakantie", "Helgdag", "urlop?", "ferie", "vacances")
MAKE_PSTR_LIST(reduce, "reduce", "reduziert", "gereduceerd", "Reducera", "zredukowany", "redusere", "réduit")
MAKE_PSTR_LIST(noreduce, "no reduce", "unreduziert", "niet gerduceerd", "oreducerad", "bez redukcji", "ingen reduksjon", "pas de réduction")
MAKE_PSTR_LIST(offset, "offset", "Anhebung", "offset", "Förskutning", "przesunięcie", "kompensasjon", "offset")
MAKE_PSTR_LIST(design, "design", "Auslegung", "Ontwero", "Design", "projekt", "design", "design")
MAKE_PSTR_LIST(minflow, "min flow", "min. Durchfluss", "Min. Doorstroom", "Min flöde", "minimalny przepływ", "min strømming", "flux min")
MAKE_PSTR_LIST(maxflow, "max flow", "max. Durchfluss", "Max. Doorstroom", "Max flöde", "maksymalny przepływ", "maks strømming", "flux max")
MAKE_PSTR_LIST(fast, "fast", "schnell", "snel", "snabb", "szybkie", "hurtig", "rapide")
MAKE_PSTR_LIST(slow, "slow", "langsam", "langzaam", "långsam", "powolne", "langsom", "lent")
MAKE_PSTR_LIST(internal_temperature, "internal temperature", "Interne Temperatur", "Interne Temperatuur", "Interntemperatur", "temperatura wewnętrzna", "interntemperatur", "température interne")
MAKE_PSTR_LIST(internal_setpoint, "internal setpoint", "Interner Sollwert", "Interne Streeftemperatuur", "Internt börvärde", "nastawa wewnętrzna", "internt settpunkt", "consigne interne")
MAKE_PSTR_LIST(external_temperature, "external temperature", "Externe Temperatur", "Externe Temperatuur", "Extern temperatur", "temperatura zewnętrzna", "ekstern temperatur", "température externe")
MAKE_PSTR_LIST(burner_temperature, "burner temperature", "Brennertemperatur", "Brander Temperuur", "Brännartemperatur", "temperatura palnika", "brennertemperatur", "température du brûleur")
MAKE_PSTR_LIST(ww_temperature, "ww temperature", "Wassertemperatur", "Watertemperatuur", "Vattentemperatur", "temperatura c.w.u.", "vanntemperatur", "température de l'eau")
MAKE_PSTR_LIST(smoke_temperature, "smoke temperature", "Abgastemperatur", "Buitentemperatuur", "Rökgastemperatur", "temperatura dymu", "røykgasstemperatur", "température des gaz d'échappement")
MAKE_PSTR_LIST(weather_compensated, "weather compensated", "Wetter kompensiert", "Weer gecompenseerd", "Väderkompenserad", "skompensow. pogodą", "værkompensert", "compensation par l'extérieur")
MAKE_PSTR_LIST(outside_basepoint, "outside basepoint", "Basispunkt Außentemp.", "Buiten basispunt", "Utomhus baspunkt", "temp. zewn. z pkt. pocz.", "utendørs basispunkt", "point de base temp. ext.")
MAKE_PSTR_LIST(functioning_mode, "functioning mode", "Funktionsweise", "Functiemodus", "Driftläge", "tryb pracy", "driftsmodus", "mode de fonctionnement")

// mixer
MAKE_PSTR_LIST(stopped, "stopped", "gestoppt", "gestopt", "stoppad", "zatrzymany", "stoppet", "arrêté")
MAKE_PSTR_LIST(opening, "opening", "öffnen", "openen", "öppnar", "otwieranie", "åpner", "ouverture")
MAKE_PSTR_LIST(closing, "closing", "schließen", "sluiten", "stänger", "zamykanie", "stenger", "fermeture")
MAKE_PSTR_LIST(open, "open", "offen", "Open", "Öppen", "otwórz", "åpen", "ouvert")
MAKE_PSTR_LIST(close, "close", "geschlossen", "Gesloten", "Stängd", "zamknij", "stengt", "fermé")

// solar ww
MAKE_PSTR_LIST(cyl1, "cyl 1", "Zyl_1", "Cil 1", "Cyl 1", "cyl 1", "cyl 1", "cyl 1")
MAKE_PSTR_LIST(cyl2, "cyl 2", "Zyl_2", "Cil 2", "Cyl 2", "cyl 2", "cyl 2", "cyl 2")

// Entity translations
// Boiler
MAKE_PSTR_LIST(wwtapactivated, "wwtapactivated", "turn on/off", "Durchlauferhitzer aktiv", "zet aan/uit", "på/av", "system przygotowywania", "slå på/av", "ecs activée")
MAKE_PSTR_LIST(reset, "reset", "Reset", "Reset", "Reset", "Nollställ", "kasowanie komunikatu", "nullstill", "reset")
MAKE_PSTR_LIST(oilPreHeat, "oilpreheat", "oil preheating", "Ölvorwärmung", "Olie voorverwarming", "Förvärmning olja", "podgrzewanie oleju", "oljeforvarming", "préchauffage de l'huile")
MAKE_PSTR_LIST(heatingActive, "heatingactive", "heating active", "Heizen aktiv", "Verwarming actief", "Uppvärmning aktiv", "c.o. aktywne", "oppvarming aktiv", "chauffage actif")
MAKE_PSTR_LIST(tapwaterActive, "tapwateractive", "tapwater active", "Warmwasser aktiv", "Warm water actief", "Varmvatten aktiv", "c.w.u. aktywne", "varmtvann aktiv", "eau chaude active")
MAKE_PSTR_LIST(selFlowTemp, "selflowtemp", "selected flow temperature", "Sollwert Vorlauftemperatur", "Ingestelde aanvoertemperatuur", "Börvärde Flödestemperatur", "wybrana temperatura zasilania", "valgt turtemperatur", "température de flux selectionnée")
MAKE_PSTR_LIST(selBurnPow, "selburnpow", "burner selected max power", "Sollwert Brennerleistung", "Ingestelde maximale brandervermogen", "Brännare vald maxeffekt", "wybrana moc źródła ciepła", "settpunkt brennerkapasitet", "puissance max du brûleur selectionnée")
MAKE_PSTR_LIST(heatingPumpMod, "heatingpumpmod", "heating pump modulation", "Heizungspumpe 1 Modulation", "Modulatie verwarmingspomp", "Modulering Värmepump", "wysterowanie pompy c.o.", "varmepumpemodulering", "modulation de la pompe à chaleur")
MAKE_PSTR_LIST(heatingPump2Mod, "heatingpump2mod", "heating pump 2 modulation", "Heizungspumpe 2 Modulation", "Modulatie verwarmingspomp 2", "Modulering Värmepump 2", "wysterowanie pompy c.o. 2", "varmepumpe 2 modulering", "modulation de la pompe à chaleur 2")
MAKE_PSTR_LIST(outdoorTemp, "outdoortemp", "outside temperature", "Aussentemperatur", "Buitentemperatuur", "Utomhustemperatur", "temperatura zewnętrzna", "utetemperatur", "température extérieure")
MAKE_PSTR_LIST(curFlowTemp, "curflowtemp", "current flow temperature", "aktuelle Vorlauftemperatur", "Huidige aanvoertemperatuur", "Flödestemperatur", "temperatura zasilania", "aktuell strømmetemperatur", "température actuelle du flux")
MAKE_PSTR_LIST(retTemp, "rettemp", "return temperature", "Rücklauftemperatur", "Retourtemperatuur", "Returtemperatur", "temperatura powrotu", "returtemperatur", "température de retour")
MAKE_PSTR_LIST(switchTemp, "switchtemp", "mixing switch temperature", "Mischer Schalttemperatur", "Mixer temperatuur", "Blandartemperatur", "temperatura przełączania mieszacza", "Blandertemperatur", "température de bascule du mélangeur")
MAKE_PSTR_LIST(sysPress, "syspress", "system pressure", "Systemdruck", "Systeemdruk", "Systemtryck", "ciśnienie w systemie", "systemtrykk", "pression du système")
MAKE_PSTR_LIST(boilTemp, "boiltemp", "actual boiler temperature", "Kesseltemperatur", "Keteltemperatuur", "Temperatur Värmepanna", "temperatura zasobnika", "Temperatur Värmepanna", "température de la chaudière")
MAKE_PSTR_LIST(exhaustTemp, "exhausttemp", "exhaust temperature", "Abgastemperatur", "Uitlaattemperatuur", "Avgastemperatur", "temperatura spalin", "røykgasstemp", "température des gaz d'échappement")
MAKE_PSTR_LIST(burnGas, "burngas", "gas", "Gas", "Gas", "Gas", "gaz", "gass", "gaz")
MAKE_PSTR_LIST(burnGas2, "burngas2", "gas stage 2", "Gas Stufe 2", "gas fase 2", "Gas Fas 2", "gaz 2 stopień", "gass 2", "gaz état 2")
MAKE_PSTR_LIST(flameCurr, "flamecurr", "flame current", "Flammenstrom", "Vlammenstroom", "Lågström", "prąd palnika", "flammestrøm", "courrant de flamme")
MAKE_PSTR_LIST(heatingPump, "heatingpump", "heating pump", "Heizungspumpe", "Verwarmingspomp", "Värmepump", "pompa ciepła", "varmepumpe", "pompe à chaleur")
MAKE_PSTR_LIST(fanWork, "fanwork", "fan", "Gebläse", "Ventilator", "Fläkt", "wentylator", "vifte", "ventilateur")
MAKE_PSTR_LIST(ignWork, "ignwork", "ignition", "Zündung", "Ontsteking", "Tändning", "zapłon", "tenning", "ignition")
MAKE_PSTR_LIST(heatingActivated, "heatingactivated", "heating activated", "Heizen aktiviert", "Verwarmen geactiveerd", "Uppvärmning aktiv", "system c.o.", "oppvarming aktivert", "chauffage activé")
MAKE_PSTR_LIST(heatingTemp, "heatingtemp", "heating temperature", "Heizungstemperatur", "Verwarmingstemperatuur", "Uppvärmningstemperatur", "temperatura grzania", "oppvarmingstemperatur", "température de chauffage")
MAKE_PSTR_LIST(pumpModMax, "pumpmodmax", "boiler pump max power", "Kesselpumpen Maximalleistung", "Ketelpomp max vermogen", "Värmepannepump max effekt", "maksymalna moc pompy zasobnika", "varmepumpe maks effekt", "puissance max pompe à chaleur")
MAKE_PSTR_LIST(pumpModMin, "pumpmodmin", "boiler pump min power", "Kesselpumpen Minmalleistung", "Ketelpomp min vermogen", "Värmepannepump min effekt", "minimalna moc pompy zasobnika", "varmepumpe min effekt", "puissance min pompe à chaleur")
MAKE_PSTR_LIST(pumpDelay, "pumpdelay", "pump delay", "Pumpennachlaufzeit", "Pomp nalooptijd", "Pumpfördröjning", "opóźnienie pompy", "pumpeforsinkelse", "délai d'attente pompe")
MAKE_PSTR_LIST(burnMinPeriod, "burnminperiod", "burner min period", "Antipendelzeit", "Antipendeltijd", "Värmepanna Min Period", "minimalny czas pracy palnika", "varmekjele min periode", "délai d'attente du brûleur")
MAKE_PSTR_LIST(burnMinPower, "burnminpower", "burner min power", "minimale Brennerleistung", "Minimaal brandervermogen", "Värmepanna Min Effekt", "minimalna moc źródła ciepła", "varmekjele min effekt", "puissance min brûleur")
MAKE_PSTR_LIST(burnMaxPower, "burnmaxpower", "burner max power", "maximale Brennerleistung", "Maximaal brandervermogen", "Värmepanna Max Effekt", "maksymalna moc źródła ciepła", "varmekjele maks effekt", "puissance max brûleur")
MAKE_PSTR_LIST(boilHystOn, "boilhyston", "hysteresis on temperature", "Einschaltdifferenz", "ketel aan hysterese verschil", "Hysteres aktiveringstemperatur", "histereza załączania", "hysterese på temperatur", "hysteresis température d'allumage")
MAKE_PSTR_LIST(boilHystOff, "boilhystoff", "hysteresis off temperature", "Ausschaltdifferenz", "ketel uit hysterese verschil", "Hysteres inaktiveringstemperatur", "histereza wyłączania", "hysterese av temperatur", "hysteresis température d'extinction")
MAKE_PSTR_LIST(boil2HystOn, "boil2hyston", "hysteresis stage 2 on temperature", "Einschaltdifferenz Stufe 2", "ketel aan hysterese verschil 2", "Hysteres aktiveringstemperatur 2", "histereza załączania stopnia 2", "", "hysteresis état 2 température d'allumage") // TODO translate
MAKE_PSTR_LIST(boil2HystOff, "boil2hystoff", "hysteresis stage 2 off temperature", "Ausschaltdifferenz Stufe 2", "ketel uit hysterese verschil 2", "Hysteres inaktiveringstemperatur 2", "histereza wyłączania stopnia 2", "", "hysteresis état 2 température d'extinction") // TODO translate
MAKE_PSTR_LIST(setFlowTemp, "setflowtemp", "set flow temperature", "Sollwert Vorlauftemperatur", "Ingestelde aanvoertemperatuur", "Börvärde Flödestemperatur", "zadana temperatura zasilania", "innstilt strømmetemperatur", "température du flux définie")
MAKE_PSTR_LIST(setBurnPow, "setburnpow", "burner set power", "Sollwert Brennerleistung", "Ingesteld brandervermogen", "Värmepanna vald Effekt", "zadana moc palnika", "varmekjele valgt effekt", "puissance du brûleur définie")
MAKE_PSTR_LIST(curBurnPow, "curburnpow", "burner current power", "Brennerleistung", "Brandervermogen", "Värmepanna aktuell effekt", "aktualna moc źródła ciepła", "brennereffekt", "puissance du brûleur actuelle")
MAKE_PSTR_LIST(burnStarts, "burnstarts", "burner starts", "Brenner Starts", "Aantal brander starts", "Värmepanna antal starter", "liczba uruchomień palnika", "antall brenner starter", "démarrages du brûleur")
MAKE_PSTR_LIST(burnWorkMin, "burnworkmin", "total burner operating time", "Brenner Laufzeit", "Totale branderlooptijd", "Värmepanna aktiva timmar", "łączny czas pracy palnika", "brennersteg tid i min", "durée de fonctionnement totale du brûleur")
MAKE_PSTR_LIST(burn2WorkMin, "burn2workmin", "burner stage 2 operating time", "Brenner Stufe 2 Laufzeit", "Totale looptijd brander fase 2", "Värmepanna steg 2 aktiva timmar", "łączny czas pracy palnika 2 stopnia", "brennersteg2 tid i min", "durée de fonctionnement totale du brûleur état 2")
MAKE_PSTR_LIST(heatWorkMin, "heatworkmin", "total heat operating time", "Heizung Laufzeit", "Totale looptijd verwarming", "Uppvärmning aktiva timmar", "łączny czas grzania", "varmetid i min", "durée de fonctionnement du chauffage")
MAKE_PSTR_LIST(heatStarts, "heatstarts", "burner starts heating", "Brenner Starts Heizung", "Aantal brander starts verwarming", "Uppvärmning antal starter", "liczba uruchomień palnika na ogrzewanie", "", "démarrages du chauffage") // TODO translate
MAKE_PSTR_LIST(UBAuptime, "ubauptime", "total UBA operating time", "Anlagen-Gesamtlaufzeit", "totale looptijd branderautomaat (UBA)", "Total Tid", "łączny czas pracy układu sterowania", "totaltid", "durée de fonctionnement totale de l'appareil (UBA)")
MAKE_PSTR_LIST(lastCode, "lastcode", "last error code", "Letzter Fehler", "Laatste foutcode", "Senaste Felkod", "ostatni błąd", "siste feilkode", "dernier code d'erreur")
MAKE_PSTR_LIST(serviceCode, "servicecode", "service code", "Statusmeldung", "Statuscode", "Servicekod", "kod serwisowy", "servicekode", "code de service")
MAKE_PSTR_LIST(serviceCodeNumber, "servicecodenumber", "service code number", "Statusmeldungsnummer", "Status codenummer", "Servicekod", "numer kodu serwisowego", "servicekodenummer", "numéro du code de service")
MAKE_PSTR_LIST(maintenanceMessage, "maintenancemessage", "maintenance message", "Wartungsmeldung", "Onderhoudsmelding", "Servicemeddelande", "komunikat przeglądu", "vedlikeholdsmelding", "message de maintenance")
MAKE_PSTR_LIST(maintenanceDate, "maintenancedate", "next maintenance date", "Wartungsdatum", "Onderhoudsdatum", "Datum nästa Service", "termin następnego przeglądu", "vedlikeholdsdato", "prochaine date de maintenance")
MAKE_PSTR_LIST(maintenanceType, "maintenance", "maintenance scheduled", "Wartungsplan", "Onderhoud gepland", "Underhall schemlagt", "rodzaj przeglądu", "vedlikeholdstype", "maintenance prévue")
MAKE_PSTR_LIST(maintenanceTime, "maintenancetime", "time to next maintenance", "Wartung in", "Onderhoud in", "Tid till nästa underhall", "czas do kolejnego przeglądu", "vedlikeholdstid", "durée avant la prochaine maintenance")
MAKE_PSTR_LIST(emergencyOps, "emergencyops", "emergency operation", "Notoperation", "Noodoperatie", "Nöddrift", "praca w trybie awaryjnym", "nøddrift", "opération d'urgence")
MAKE_PSTR_LIST(emergencyTemp, "emergencytemp", "emergency temperature", "Nottemperatur", "Noodtemperatuur", "Nöddrift temperatur", "temperatura w trybie awaryjnym", "nødtemperatur", "température d'urgence")

// heatpump/compress specific
MAKE_PSTR_LIST(upTimeControl, "uptimecontrol", "total operating time heat", "Betriebszeit Heizen gesamt", "Totale bedrijfstijd", "Total tid uppvärmning", "łączny czas generowania ciepła", "", "durée totale de fonctionnement chauffage") // TODO translate
MAKE_PSTR_LIST(upTimeCompHeating, "uptimecompheating", "operating time compressor heating", "Betriebszeit Kompressor heizen", "Bedrijfstijd compressor verwarmingsbedrijf", "Total tid kompressor uppvärmning", "łączny czas ogrzewania (sprężarka)", "", "durée de fonctionnement compresseur chauffage") // TODO translate
MAKE_PSTR_LIST(upTimeCompCooling, "uptimecompcooling", "operating time compressor cooling", "Betriebszeit Kompressor kühlen", "Bedrijfstijd compressor koelbedrijf", "Total tid kompressor kyla", "łączny czas chłodzenia (sprężarka)", "Total tid kompressor kjøling", "durée de fonctionnement compresseur refroidissement")
MAKE_PSTR_LIST(upTimeCompWw, "uptimecompww", "operating time compressor dhw", "Betriebszeit Kompressor", "Bedrijfstijd compressor warmwaterbedrijf", "Total tid kompressor varmvatten", "łączny czas grzania c.w.u. (sprężarka)", "Total tid kompressor varmtvann", "durée de fonctionnement compresseur ecs")
MAKE_PSTR_LIST(upTimeCompPool, "uptimecomppool", "operating time compressor pool", "Betriebszeit Kompressor Pool", "Bedrijfstijd compressor voor zwembadbedrijf", "Total tid kompressor pool", "łączny czas podgrzewania basenu (sprężarka)", "Total tid kompressor basseng", "durée de fonctionnement compresseur piscine")
MAKE_PSTR_LIST(totalCompStarts, "totalcompstarts", "total compressor control starts", "Kompressor Starts gesamt", "Totaal compressorstarts", "Kompressorstarter Totalt", "liczba załączeń sprężarki", "kompressorstarter Totalt", "nombre démarrages total contrôle compresseur")
MAKE_PSTR_LIST(heatingStarts, "heatingstarts", "heating control starts", "Heizen Starts", "Starts verwarmingsbedrijf", "Kompressorstarter Uppvärmning", "liczba załączeń ogrzewania", "kompressorstarter oppvarming", "démarrages contrôle chauffage")
MAKE_PSTR_LIST(coolingStarts, "coolingstarts", "cooling control starts", "Kühlen Starts", "Starts koelbedrijf", "Kompressorstarter Kyla", "liczba załączeń chłodzenia", "kompressorstarter kjøling", "démarrages contrôle refroidissement")
MAKE_PSTR_LIST(poolStarts, "poolstarts", "pool control starts", "Pool Starts", "Starts zwembadbedrijf", "Kompressorstarter Pool", "liczba załączeń podgrzewania basenu", "kompressorstarter basseng", "démarrages contrôle piscine")
MAKE_PSTR_LIST(nrgConsTotal, "nrgconstotal", "total energy consumption", "Energieverbrauch gesamt", "Energieverbrauch gesamt", "Energiförbrukning totalt", "energia pobrana (sumarycznie)", "energiforbruk totalt", "consommation totale énergie")
MAKE_PSTR_LIST(nrgConsCompTotal, "nrgconscomptotal", "total energy consumption compressor", "Energieverbrauch Kompressor gesamt", "Energieverbruik compressor totaal", "Energiförbrukning kompressor", "energia pobrana przez sprężarkę", "energiforbruk kompressor", "consommation totale énergie compresseur")
MAKE_PSTR_LIST(nrgConsCompHeating, "nrgconscompheating", "energy consumption compressor heating", "Energieverbrauch Kompressor heizen", "Energieverbruik compressor verwarmingsbedrijf", "Energiförbrukning uppvärmning", "energia pobrana przez sprężarkę na ogrzewanie", "energiforbruk oppvarming", "consommation  énergie compresseur chauffage")
MAKE_PSTR_LIST(nrgConsCompWw, "nrgconscompww", "energy consumption compressor dhw", "Energieverbrauch Kompressor", "Energieverbruik compressor warmwaterbedrijf", "Energiförbrukning varmvatten", "energia pobrana przez sprężarkę na c.w.u.", "energiforbruk varmvann", "consommation  énergie compresseur ecs")
MAKE_PSTR_LIST(nrgConsCompCooling, "nrgconscompcooling", "energy consumption compressor cooling", "Energieverbrauch Kompressor kühlen", "Energieverbruik compressor koelbedrijf", "Energiförbrukning kyla", "energia pobrana przez sprężarkę na chłodzenie", "energiforbruk kjøling", "consommation  énergie compresseur refroidissement")
MAKE_PSTR_LIST(nrgConsCompPool, "nrgconscomppool", "energy consumption compressor pool", "Energieverbrauch Kompressor Pool", "Energiebedrijf compressor zwembadbedrijf", "Energiförbrukning pool", "energia pobrana przez sprężarkę na podgrzewanie basenu", "energiforbruk basseng", "consommation  énergie compresseur piscine")
MAKE_PSTR_LIST(nrgSuppTotal, "nrgsupptotal", "total energy supplied", "gesamte Energieabgabe", "Totaal opgewekte energie", "Genererad energi", "energia oddana (sumarycznie)", "tilført energi", "énergie totale fournie")
MAKE_PSTR_LIST(nrgSuppHeating, "nrgsuppheating", "total energy supplied heating", "gesamte Energieabgabe heizen", "Opgewekte energie verwarmingsbedrijf", "Genererad energi Uppvärmning", "energia oddana na ogrzewanie", "tilført energi oppvarming", "énergie totale fournie chauffage")
MAKE_PSTR_LIST(nrgSuppWw, "nrgsuppww", "total energy warm supplied dhw", "gesamte Energieabgabe", "Opgewekte energie warmwaterbedrijf", "Genererad energi Varmvatten", "energia oddana na c.w.u.", "tilført energi varmvann", "énergie chaude totale fournie ecs")
MAKE_PSTR_LIST(nrgSuppCooling, "nrgsuppcooling", "total energy supplied cooling", "gesamte Energieabgabe kühlen", "Opgewekte energie koelbedrijf", "Genererad energi Kyla", "energia oddana na chłodzenie", "Tillført energi kjøling", "énergie totale fournie refroidissement")
MAKE_PSTR_LIST(nrgSuppPool, "nrgsupppool", "total energy supplied pool", "gesamte Energieabgabe Pool", "Opgewekte energie zwembadbedrijf", "Genererad energi Pool", "energia oddana na podgrzewanie basenu", "tilført energi basseng", "énergie totale fournie piscine")
MAKE_PSTR_LIST(auxElecHeatNrgConsTotal, "auxelecheatnrgconstotal", "total auxiliary electrical heater energy consumption", "Energieverbrauch el. Zusatzheizung", "Totaal energieverbruik electrisch verwarmingselement", "Energiförbrukning Eltillkott", "energia pobrana przez grzałki", "energiforbruk varmekolbe", "consommation totale énergie electrique auxiliaire chauffage")
MAKE_PSTR_LIST(auxElecHeatNrgConsHeating, "auxelecheatnrgconsheating", "auxiliary electrical heater energy consumption heating", "Energieverbrauch el. Zusatzheizung Heizen", "Energieverbruik electrisch verwarmingselement voor verwarmingsbedrijf", "Energiförbrukning Eltillskott Uppvärmning", "energia pobrana przez grzałki na ogrzewanie", "energiforbruk varmekolbe oppvarming", "consommation énergie electrique auxiliaire chauffage")
MAKE_PSTR_LIST(auxElecHeatNrgConsWW, "auxelecheatnrgconsww", "auxiliary electrical heater energy consumption dhw", "Energieverbrauch el. Zusatzheizung", "Energieverbruik electrisch verwarmingselement voor warmwaterbedrijf", "Energiförbrukning Eltillskott Varmvatten", "energia pobrana przez grzałki na c.w.u.", "energiförbruk varmekolbe varmvann", "consommation énergie electrique auxiliaire chauffage ecs")
MAKE_PSTR_LIST(auxElecHeatNrgConsPool, "auxelecheatnrgconspool", "auxiliary electrical heater energy consumption pool", "Energieverbrauch el. Zusatzheizung Pool", "Energieverbruik electrisch verwarmingselement voor zwembadbedrijf", "Energiförbrukning Eltillskott Pool", "energia pobrana przez grzałki na podgrzewanie basenu", "", "consommation énergie electrique auxiliaire chauffage piscine") // TODO translate

MAKE_PSTR_LIST(hpCompOn, "hpcompon", "hp compressor", "WP Kompressor", "WP compressor", "VP Kompressor", "sprężarka pompy ciepła", "vp kompressor", "compresseur pompe à chaleur")
MAKE_PSTR_LIST(hpHeatingOn, "hpheatingon", "hp heating", "WP Heizen", "WP verwarmingsbedrijf", "VP Uppvärmning", "pompa ciepła, ogrzewanie", "vp oppvarmning", "chauffe pompe à chaleur")
MAKE_PSTR_LIST(hpCoolingOn, "hpcoolingon", "hp cooling", "WP Kühlen", "WP koelbedrijf", "VP Kyla", "pompa ciepła, chłodzenie", "vp kjøling", "refroidissement pompe à chaleur")
MAKE_PSTR_LIST(hpWwOn, "hpwwon", "hp dhw", "WP Warmwasser", "WP warmwaterbedrijf", "VP Varmvatten", "pompa ciepła, c.w.u.", "vp varmvatten", "eau chaude pompe à chaleur")
MAKE_PSTR_LIST(hpPoolOn, "hppoolon", "hp pool", "WP Pool", "WP zwembadbedrijf", "VP Pool", "pompa ciepła, podgrzewanie basenu", "vp basseng", "pompe à chaleur piscine")
MAKE_PSTR_LIST(hpBrinePumpSpd, "hpbrinepumpspd", "brine pump speed", "Solepumpen-Geschw.", "Snelheid pekelpomp", "Hastighet Brine-pump", "wysterowanie pompy glikolu", "hastighet brine-pumpe", "vitesse pompe à saumure")
MAKE_PSTR_LIST(hpCompSpd, "hpcompspd", "compressor speed", "Kompressor-Geschw.", "Snelheid compressor", "Kompressorhastighet", "wysterowanie sprężarki", "kompressorhastighet", "vitesse du compresseur")
MAKE_PSTR_LIST(hpCircSpd, "hpcircspd", "circulation pump speed", "Zirkulationspumpen-Geschw.", "Snelheid circulatiepomp", "Hastighet Cirkulationspump", "wysterowanie pompy obiegu grzewczego", "hastighet sirkulationspumpe", "vitesse pompe à circulation")
MAKE_PSTR_LIST(hpBrineIn, "hpbrinein", "brine in/evaporator", "Sole in/Verdampfer", "pekel in/verdamper", "Brine in (förangare)", "temperatura glikolu na wejściu kolektora (TB0)", "brine in/fordamper", "entrée saumure/évaporateur")
MAKE_PSTR_LIST(hpBrineOut, "hpbrineout", "brine out/condenser", "Sole aus/Kondensator", "pekel uit/condensor", "Brine ut (kondensor)", "temperatura glikolu na wyjściu kolektora (TB1)", "Brine ut/kondensor", "sortie saumure/condenseur")
MAKE_PSTR_LIST(hpSwitchValve, "hpswitchvalve", "switch valve", "Schaltventil", "schakelklep", "Växelventil", "zawór przełączający", "skifteventil", "valve de commutation")
MAKE_PSTR_LIST(hpActivity, "hpactivity", "compressor activity", "Kompressoraktivität", "Compressoractiviteit", "Kompressoraktivitet", "pompa ciepła, aktywność sprężarki", "", "activité du compresseur") // TODO translate

MAKE_PSTR_LIST(hpPower, "hppower", "compressor power output", "Kompressorleistung", "Compressorvermogen", "Kompressoreffekt", "moc wyjściowa sprężarki", "kompressoreffekt", "puissance de sortie compresseur")
MAKE_PSTR_LIST(hpTc0, "hptc0", "heat carrier return (TC0)", "Kältemittel Rücklauf (TC0)", "Koudemiddel retour (TC0)", "Värmebärare Retur (TC0)", "temperatura nośnika ciepła na powrocie (TC0)", "kjølemiddel retur (TC0)", "retour caloporteur (TC0)")
MAKE_PSTR_LIST(hpTc1, "hptc1", "heat carrier forward (TC1)", "Kältemittel Vorlauf (TC1)", "Koudemiddel aanvoer (TC1)", "Värmebärare Framledning (TC1)", "temperatura nośnika ciepła pierwotna (TC1)", "kjølemiddel tur (TC1)", "avance caloporteur (TC1)")
MAKE_PSTR_LIST(hpTc3, "hptc3", "condenser temperature (TC3)", "Verflüssigertemperatur (TC3)", "Condensortemperatuur (TC3)", "Kondensortemperatur (TC3)", "temperatura skraplacza/na wyjściu sprężarki (TC3)", "kondensortemperatur (TC3)", "température condensateur (TC3)")
MAKE_PSTR_LIST(hpTr1, "hptr1", "compressor temperature (TR1)", "Kompessortemperatur (TR1)", "Compressor temperatuur (TR1)", "Kompressor temp (TR1)", "temperatura sprężarki (TR1)", "kompressor temperatur (TR1)", "température compresseur (TR1)")
MAKE_PSTR_LIST(hpTr3, "hptr3", "refrigerant temperature liquid side (condenser output) (TR3)", "Kältemittel (flüssig) (TR3)", "Temperatuur koudemiddel vloeibare zijde (TR3)", "Köldmedium temperatur (kondensorutlopp) (TR3)", "temperatura skraplacza ogrzew. (TR3)", "kjølemiddeltemperatur på væskesiden (TR3)", "température réfrigérant côté liquide (sortie condensateur) (TR3)")
MAKE_PSTR_LIST(hpTr4, "hptr4", "evaporator inlet temperature (TR4)", "Verdampfer Eingang (TR4)", "Verdamper ingangstemperatuur (TR4)", "Förångare inloppstemp (TR4)", "temperatura na wejściu parownika (TR4)", "innløpstemperatur for fordamperen (TR4)", "température entrée évaporateur (TR4)")
MAKE_PSTR_LIST(hpTr5, "hptr5", "compressor inlet temperature (TR5)", "Kompessoreingang (TR5)", "Compressor ingangstemperatuur (TR5)", "Kompressor inloppstemp (TR5)", "temperatura na wejściu sprężarki (TR5)", "kompressor innløpstemp (TR5)", "température entrée compresseur (TR5)")
MAKE_PSTR_LIST(hpTr6, "hptr6", "compressor outlet temperature (TR6)", "Kompressorausgang (TR6)", "Compressor uitgangstemperatuur (TR6)", "Kompressor utloppstemp (TR6)", "temperatura na wyjściu sprężarki (TR6)", "kompressor utløpstemp (TR6)", "température sortie compresseur (TR6)")
MAKE_PSTR_LIST(hpTr7, "hptr7", "refrigerant temperature gas side (condenser input) (TR7)", "Kältemittel (gasförmig) (TR7)", "Temperatuur koudemiddel gasvormig (TR7)", "Köldmedium temperatur gassida (kondensorinlopp) (TR7)", "temperatura czynnika chłodniczego po stronie gazu (wejście skraplacza) (TR7)", "kjølemedium temperatur gassida (kondensatorinløp) (TR7)", "température réfrigérant côté gaz (sortie condensateur) (TR7)")
MAKE_PSTR_LIST(hpTl2, "hptl2", "air inlet temperature (TL2)", "Außenluft-Einlasstemperatur (TL2)", "Temperatuur luchtinlaat (TL2)", "Luftintagstemperatur (TL2)", "temperatura wlotu powietrza (TL2)", "luftinntakstemperatur (TL2)", "température entrée air (TL2)")
MAKE_PSTR_LIST(hpPl1, "hppl1", "low pressure side temperature (PL1)", "Niederdruckfühler (PL1)", "Temperatuur lage drukzijde (PL1)", "Temperatur Lågtryckssidan (PL1)", "temperatura po stronie niskiego ciśnienia (PL1)", "temperatur lavtrykksiden (PL1)", "température côté basse pression (PL1)")
MAKE_PSTR_LIST(hpPh1, "hpph1", "high pressure side temperature (PH1)", "Hochdruckfühler (PH1)", "Temperatuur hoge drukzijde (PH1)", "Temperatur Högtryckssidan (PH1)", "temperatura po stronie wysokiego ciśnienia (PH1)", "Temperatur Høytrykksiden (PH1)", "température côté bhauteasse pression (PH1)")
MAKE_PSTR_LIST(hpTa4, "hpta4", "drain pan temp (TA4)", "Kondensatorwanne (TA4)")

MAKE_PSTR_LIST(hpInput1, "hpin1", "input 1 state", "Eingang 1 Status", "Status input 1", "Status Ingång 1", "stan wejścia 1", "status Inggng 1", "état entrée 1")
MAKE_PSTR_LIST(hpInput2, "hpin2", "input 2 state", "Eingang 2 Status", "Status input 2", "Status Ingång 2", "stan wejścia 2", "status Inggng 2", "état entrée 2")
MAKE_PSTR_LIST(hpInput3, "hpin3", "input 3 state", "Eingang 3 Status", "Status input 3", "Status Ingång 3", "stan wejścia 3", "status Inggng 3", "état entrée 3")
MAKE_PSTR_LIST(hpInput4, "hpin4", "input 4 state", "Eingang 4 Status", "Status input 4", "Status Ingång 4", "stan wejścia 4", "status Inggng 4", "état entrée 4")
MAKE_PSTR_LIST(hpIn1Opt, "hpin1opt", "input 1 options", "Eingang 1 Einstellung", "Instelling input 1", "Inställningar Ingång 1", "opcje wejścia 1", "innstillinger inngang 1", "options entrée 1")
MAKE_PSTR_LIST(hpIn2Opt, "hpin2opt", "input 2 options", "Eingang 2 Einstellung", "Instelling input 2", "Inställningar Ingång 2", "opcje wejścia 2", "innstillinger inngang 2", "options entrée 2")
MAKE_PSTR_LIST(hpIn3Opt, "hpin3opt", "input 3 options", "Eingang 3 Einstellung", "Instelling input 3", "Inställningar Ingång 3", "opcje wejścia 3", "innstillinger inngang 3", "options entrée 3")
MAKE_PSTR_LIST(hpIn4Opt, "hpin4opt", "input 4 options", "Eingang 4 Einstellung", "Instelling input 4", "Inställningar Ingång 4", "opcje wejścia 4", "innstillinger inngang 4", "options entrée 4")
MAKE_PSTR_LIST(maxHeatComp, "maxheatcomp", "heat limit compressor", "Heizgrenze Kompressor", "heat limit compressor", "heat limit compressor", "ograniczenie mocy sprężarki", "max varmegrense kompressor", "limite chaleur compresseur")
MAKE_PSTR_LIST(maxHeatHeat, "maxheatheat", "heat limit heating", "Heizgrenze Heizen", "heat limit heating", "heat limit heating", "ograniczenie mocy w trybie ogrzewania", "maks varmegrense oppvarming", "limite chaleur chauffage")
MAKE_PSTR_LIST(maxHeatDhw, "maxheatdhw", "heat limit dhw", "Heizgrenze Warmwasser", "heat limit dhw", "heat limit dhw", "ograniczenie mocy w trybie c.w.u.", "varmegrense varmtvann", "limite chaleur ecs")

MAKE_PSTR_LIST(auxHeaterOff, "auxheateroff", "disable auxilliary heater", "Verbiete Zusatzheizer", "Bijverwarming uitsc", "Blockera eltillskott", "wyłącz dogrzewacz", "", "Désactiver chauff. d'app") // TODO translate
MAKE_PSTR_LIST(auxHeaterStatus, "auxheaterstatus", "auxilliary heater status", "Status Zusatzheizer", "Bijverwarming", "Eltillskott Status", "status dogrzewacza", "", "Chauffage auxiliaire") // TODO translate
MAKE_PSTR_LIST(auxHeaterOnly, "auxheateronly", "auxilliary heater only", "nur Zusatzheizer","Alleen bijverwarming", "Eltillskott Enbart", "tylko dogrzewacz", "", "Que chauffage auxiliaire") // TODO translate
MAKE_PSTR_LIST(auxHeaterDelay, "auxheaterdelay", "auxilliary heater on delay", "Zusatzheizer verzögert ein", "Bijverw. vertraagd aan", "Eltillskottfördröjning på", "opóźnienie włączania dogrzewacza", "Tilleggsvarmer forsinket på", "Chauff app tempo marche")
MAKE_PSTR_LIST(silentMode, "silentmode", "silent mode", "Silentmodus", " Stiller gebruik", "Tyst läge", "trybu cichego",  "", "Fct silencieux") // TODO translate
MAKE_PSTR_LIST(minTempSilent, "mintempsilent", "min. outside temp. for silent mode", "Minimale Aussentemperatur Silentmodus", " Stiller gebruik min. buitentemp", "Tyst läge min temp", "minimalna temperatura zewnętrzna dla trybu cichego", "", "Fct silencieux: Temp. extérieure min.") // TODO translate
MAKE_PSTR_LIST(tempParMode, "tempparmode", "outside temp. parallel mode", "Aussentemperatur Parallelmodus", "Buitentemp. parallelbedr", "Parallelläge Utomhustemp.", "maksymalna temperatura zewnętrzna dla dogrzewacza", "", "Temp. ext. fct parallèle") // TODO translate
MAKE_PSTR_LIST(auxHeatMixValve, "auxheatmix", "aux. heater mixing valve", "Mischer Zusatzheizer", "Bijverwarming menger", "Eltilskott Blandarventil", "mieszacz dogrzewacza", "", "Chauffage auxiliaire mélangeur") // TODO translate
MAKE_PSTR_LIST(hpHystHeat, "hphystheat", "on/off hyst heat", "Schalthysterese Heizen", "Aan/uit-hysteresis in verw. bedrijf", "Hstereses Uppvärm.", "histereza wł./wył. ogrzewania", "På/av-hysterese Oppvar.", "Hystérésis Marche en mode chauffage")
MAKE_PSTR_LIST(hpHystCool, "hphystcool", "on/off hyst cool", "Schalthysterese Kühlen", "Aan/uit-hysteresis in koelbedrijf ", "Hystereses Kyla", "histereza wł./wył. chłodzenia", "", "Hystérésis Marche en mode refroidissement") // TODO translate
MAKE_PSTR_LIST(hpHystPool, "hphystpool", "on/off hyst pool", "Schalthysterese Pool", "an/uit-hysteresis in zwembadbedri", "Hystereses Pool", "histereza wł./wył. podgrzewania basenu", "", "Hystérésis Marche en mode piscine") // TODO translate
MAKE_PSTR_LIST(tempDiffHeat, "tempdiffheat", "temp. diff. TC3/TC0 heat", "Temp.diff. TC3/TC0 Heizen", "Temp.vers. TC3/TC0 verw", "Delta(T) TC3/TC0 Uppvärm.", "różnica temperatur TC3/TC0 w trakcie ogrzewania", "", "Delta T TC3/TC0 Chauff") // TODO translate
MAKE_PSTR_LIST(tempDiffCool, "tempdiffcool", "temp. diff. TC3/TC0 cool", "Temp.diff. TC3/TC0 Kühlen", "Temp.vers. TC3/TC0 koel.", "Delta(T) TC3/TC0 Kyla", "różnica temperatur TC3/TC0 w trakcie chłodzenia", "", "Delta T TC3/TC0 Refroid.") // TODO translate

MAKE_PSTR_LIST(wwComfOffTemp, "wwcomfoff", "comfort switch off", "Komfort Ausschalttemp", "Comfort Uitschakeltemp.", "Komfortläge avstängingstemp.", "temperatura wyłączania w trybie komfort", "", "Confort Temp. d'arrêt") // TODO translate
MAKE_PSTR_LIST(wwEcoOffTemp, "wwecooff", "eco switch off", "ECO Ausschalttemp", "Eco Uitschakeltemp.", "Ekoläge avstängningstemp.", "temperatura wyłączania w trybie eko", "", "Eco Temp. d'arrêt") // TODO translate
MAKE_PSTR_LIST(wwEcoPlusOffTemp, "wwecoplusoff", "eco+ switch off", "ECO+ Ausschalttemp", "Eco+ Uitschakeltemp.", "Eko+ avstängningstemp.", "temperatura wyłączania w trybie eko+", "", "Eco+ Temp. d'arrêt") // TODO translate

MAKE_PSTR_LIST(auxHeatMode, "auxheatrmode", "aux heater mode", "Modus Zusatzheizer", "", "", "", "", "") // TODO translate
MAKE_PSTR_LIST(auxMaxTemp, "auxmaxtemp", "aux heater max temperature", "Zusatzheizer Maximaltemp.", "", "", "", "", "") // TODO translate
MAKE_PSTR_LIST(manDefrost, "mandefrost", "manual defrost", "Manuelle Enteisung", "", "", "", "", "") // TODO translate
MAKE_PSTR_LIST(pvCooling, "pvcooling", "cooling only with PV", "Kühlen nur mit PV", "", "", "", "", "") // TODO translate
MAKE_PSTR_LIST(hpCircPumpWw, "hpcircpumpww", "circulation pump available during dhw", "", "", "", "", "", "") // TODO translate
MAKE_PSTR_LIST(wwAlternatingOper, "wwalternatingop", "alternating operation", "", "", "", "praca naprzemienna", "", "") // TODO translate
MAKE_PSTR_LIST(wwAltOpPrioHeat, "wwaltopprioheat", "prioritise heating during dhw", "", "", "", "czas na ogrzewanie w trakcie c.w.u", "", "") // TODO translate
MAKE_PSTR_LIST(wwAltOpPrioWw, "wwaltopprioww", "prioritise dhw during heating", "", "", "", "czas na c.w.u w trakcie ogrzewania", "", "") // TODO translate

// hybrid heatpump
MAKE_PSTR_LIST(hybridStrategy, "hybridstrategy", "hybrid control strategy", "Hybrid Strategie", "Hybride strategie", "Hybrid kontrollstrategi", "strategia sterowania hybrydowego", "hybrid kontrollstrategi", "stratégie contrôle hybride")
MAKE_PSTR_LIST(switchOverTemp, "switchovertemp", "outside switchover temperature", "Außentemperatur für Umschaltung", "Schakeltemperatuur buitentemperatuur", "Utomhus Omställningstemperatur", "zewnętrzna temperatura przełączania", "utendørstemp styring", "basculement par température extérieure")
MAKE_PSTR_LIST(energyCostRatio, "energycostratio", "energy cost ratio", "Energie/Kosten-Verhältnis", "Energiekostenratio", "Energi/Kostnads-förhållande", "współczynnik energia/koszt", "energi/kostnads forhold", "ratio coût énergie")
MAKE_PSTR_LIST(fossileFactor, "fossilefactor", "fossile energy factor", "Energiefaktor Fossil", "Energiefactor fossiele brandstof", "Energifaktor fossilenergi", "udział energii z paliw kopalnych", "energifaktor fossilenergi", "facteur énergie fossile")
MAKE_PSTR_LIST(electricFactor, "electricfactor", "electric energy factor", "Energiefaktor elektrisch", "Energiefactor electrisch", "Elektrisk energifaktor", "udział energii elektrycznej", "elektrisk energifaktor", "facteur énergie électrique")
MAKE_PSTR_LIST(delayBoiler, "delayboiler", "delay boiler support", "Verzögerungs-Option", "Vertragingsoptie", "Fördröjningsoption", "opcja opóźnienia", "Fördörjningsoption", "option retardement chaudière")
MAKE_PSTR_LIST(tempDiffBoiler, "tempdiffboiler", "temp diff boiler support", "Temperaturdifferenz-Option", "Verschiltemperatuuroptie", "Temperaturskillnadsoption", "opcja różnicy temperatur", "temperatursforskjell kjele", "option différence température")
MAKE_PSTR_LIST(lowNoiseMode, "lownoisemode", "low noise mode", "Geräuscharmer Betrieb", "", "Tyst läge", "tryb cichy", "", "mode faible bruit") // TODO translate
MAKE_PSTR_LIST(lowNoiseStart, "lownoisestart", "low noise starttime", "Start geräuscharmer Betrieb", "", "Tyst läge starttid", "początek trybu cichego", "", "heure démarrage faible bruit") // TODO translate
MAKE_PSTR_LIST(lowNoiseStop, "lownoisestop", "low noise stoptime", "Stopp geräuscharmer Betrieb", "", "Tyst läge stopptid", "koniec trybu cichego", "", "heure arrêt faible bruit") // TODO translate
MAKE_PSTR_LIST(energyPriceGas, "energypricegas", "energy price gas", "Energiepreis Gas", "", "Gaspris", "cena energii z gazu", "", "prix énergie gaz") // TODO translate
MAKE_PSTR_LIST(energyPriceEl, "energypriceel", "energy price electric", "Energiepreis Eletrizität", "", "Elpris", "cena energii elektrycznej", "", "prix énergie électrique") // TODO translate
MAKE_PSTR_LIST(energyPricePV, "energyfeedpv", "feed in PV", "PV Einspeisevergütung", "", "PV Energi", "zasilanie energią fotowoltaiczną", "", "alimentation PV") // TODO translate
MAKE_PSTR_LIST(hybridDHW, "hybriddhw", "hybrid DHW", "Hybrid Warmwasser", "", "Hybridläge varmvatten", "hybrydowa c.w.u.", "", "ecs hybride") // TODO translate
MAKE_PSTR_LIST(airPurgeMode, "airpurgemode", "air purge mode", "Luftspülung", "", "Luftreningsläge", "tryb oczyszczania powietrza", "", "mode purge air") // TODO translate
MAKE_PSTR_LIST(heatPumpOutput, "heatpumpoutput", "heatpump output", "WP Leistung", "", "Värmepumpseffekt", "moc wyjściowa pompy ciepła", "", "sortie pompe à chaleur") // TODO translate
MAKE_PSTR_LIST(coolingCircuit, "coolingcircuit", "cooling circuit", "Kühlkreislauf", "", "Kylkrets", "obwód chłodzący", "", "circuit refroidissement") // TODO translate
MAKE_PSTR_LIST(compStartMod, "compstartmod", "compressor start modulation", "Kompressor Startleistung", "", "Kompressor startmodulering", "początkowa modulacja sprężarki", "", "modulation démarrage compresseur") // TODO translate
MAKE_PSTR_LIST(heatDrainPan, "heatdrainpan", "heat drain pan", "Wärmeausgleichsgefäß", "", "Uppvärm. dränering", "zbiornik wyrównawczy ciepła", "", "bac récupération chaleur") // TODO translate
MAKE_PSTR_LIST(heatCable, "heatcable", "heating cable", "Heizband", "heating cable", "värmekabel", "przewód grzejny", "", "câble chauffant") // TODO translate

// alternative heatsource AM200
MAKE_PSTR_LIST(aCylTopTemp, "cyltoptemp", "cylinder top temperature", "Speichertemperatur Oben", "Buffer temperatuur boven", "Cylindertemperatur Toppen", "temperatura na górze cylindra", "beredertemperatur topp", "température haut cylindre")
MAKE_PSTR_LIST(aCylCenterTemp, "cylcentertemp", "cylinder center temperature", "Speichertemperatur Mitte", "Buffer temperatuur midden", "Cylindertemperatur Mitten", "temperatura na środku cylindra", "beredertemperatur midten", "température centre cylindre")
MAKE_PSTR_LIST(aCylBottomTemp, "cylbottomtemp", "cylinder bottom temperature", "Speichertemperatur Unten", "Buffer temperatuur onder", "Cylindertemperatur Botten", "temperatura na dole cylindra", "beredertemperatur nederst", "température fond cylindre")
MAKE_PSTR_LIST(aFlowTemp, "altflowtemp", "alternative hs flow temperature", "Alternativer WE Vorlauftemperatur", "Alternatieve warmtebron aanvoertemperatuur", "Alternativ flödestemp värmekälla", "temperatura zasilania z alternatywnego źródła", "", "température flux hs alternative") // TODO translate
MAKE_PSTR_LIST(aRetTemp, "altrettemp", "alternative hs return temperature", "Alternativer WE Rücklauftemperatur", "Alternatieve warmtebron retourtemperatuur", "Alternativ returtemp värmekälla", "temperatura powrotu z alternatywnego źródła", "", "température retour hs alternative") // TODO translate
MAKE_PSTR_LIST(sysFlowTemp, "sysflowtemp", "system flow temperature", "System Vorlauftemperatur", "Systeem aanvoertemperatuur", "Systemflödestemperatur", "temperatura zasilania systemu", "", "température flux système") // TODO translate
MAKE_PSTR_LIST(sysRetTemp, "sysrettemp", "system return temperature", "System Rücklauftemperatur", "Systeem retourtemperatuur", "Systemreturtemperatur", "temperatura powrotu z systemu", "", "température retour système") // TODO translate
MAKE_PSTR_LIST(valveByPass, "valvebypass", "bypass valve", "Bypass-Ventil", "Bypass klep", "Bypassventil", "zawór obejścia", "", "vanne dérivation") // TODO translate
MAKE_PSTR_LIST(valveBuffer, "valvebuffer", "buffer valve", "Puffer-Ventil", "Bufferklep", "Buffertventil", "zawór bufora", "", "vanne tampon") // TODO translate
MAKE_PSTR_LIST(valveReturn, "valvereturn", "return valve", "Rückfluss-Ventil", "Retourklep", "Returventil", "zawór powrotu", "", "vanne retour") // TODO translate
MAKE_PSTR_LIST(aPumpMod, "altpumpmod", "alternative hs pump modulation", "Alternativer WE Pumpenmodulation", "Alternatieve warmtebron pomp modulatie", "Alternativ Pumpmodulering Värmekälla", "modulacja pompy alternatywnego źródła ciepła", "alternativ pumpemodulering varmekilde", "modulation alternative pompe hs")
MAKE_PSTR_LIST(heatSource, "heatsource", "alternative heating active", "Alternativer Wärmeerzeuger aktiv", "Alternatieve warmtebron aktief", "Alternativ Värmekälla aktiv", "aktywne alternatywne źródło ciepła", "alternativ varmekilde aktiv", "chauffage alternatif actif")

MAKE_PSTR_LIST(vr2Config, "vr2config", "vr2 configuration", "VR2 Konfiguration", "VR2 configuratie", "VR2 Konfiguration", "konfiguracja VR2", "vr2 konfigurasjon", "configuration vr2")
MAKE_PSTR_LIST(ahsActivated, "ahsactivated", "alternate heat source activation", "Alt. Wärmeerzeuger aktiviert", "Altenatieve warmtebron geactiveerd", "Alternativ värmekälla aktivering", "aktywacja alternatywnego źródła ciepła", "alternativ varmekilde aktivering", "activation source chaleur alternative")
MAKE_PSTR_LIST(aPumpConfig, "apumpconfig", "primary pump config", "Konfig. Hauptpumpe", "Primaire pomp configuratie", "Konfiguration Primärpump", "konfiguracja pompy głównej", "konfiguration Primærpumpe", "configuration pompe primaire")
MAKE_PSTR_LIST(aPumpSignal, "apumpsignal", "output for pr1 pump", "Ausgang Pumpe PR1", "Output voor pomp PR1", "Utgång från pump PR1", "wyjście pompy PR1", "utgang fra pumpe PR1", "sortie pompe pr1")
MAKE_PSTR_LIST(aPumpMin, "apumpmin", "min output pump pr1", "Minimale Pumpenansteuerung", "Minimale output pomp PR1", "Min Output Pump PR1", "minimalne wysterowanie pompy PR1", "minimal output pumpe PR1", "sortie min pompe pr1")
MAKE_PSTR_LIST(tempRise, "temprise", "ahs return temp rise", "Rücklauf Temperaturerhöhung", "Verhoging retourtemperatuur", "Förhöjd returtemperatur", "wzrost temperatury powrotu", "forhøyd returtemperatur", "augmentation température retour ahs")
MAKE_PSTR_LIST(setReturnTemp, "setreturntemp", "set temp return", "Soll-Rücklauftemperatur", "Streeftemperatuur retour", "Vald returtemperatur", "zadana temperatura powrotu", "valgt returtemperatur", "régler température retour")
MAKE_PSTR_LIST(mixRuntime, "mixruntime", "mixer run time", "Mischer-Laufzeit", "Mixer looptijd", "Blandningsventil drifttid", "czas pracy miksera", "blandingsventil drifttid", "durée fonctionnement mélangeur")
MAKE_PSTR_LIST(bufBypass, "bufbypass", "buffer bypass config", "Puffer-Bypass Konfig.", "Buffer bypass configuratie", "Konfiguration Buffer bypass", "konfiguracja z obejściem bufora", "konfigurasjon buffer bypass", "configuration contournement buffer")
MAKE_PSTR_LIST(bufMixRuntime, "bufmixruntime", "bypass mixer run time", "Speicher-Mischer-Laufzeit", "Buffer mixer looptijd", "Blandningsventil Bypass drifttid", "czas pracy mieszacza obejścia", "blandningsventil bypass drifttid", "durée fonctionnement contournement mélangeur")
MAKE_PSTR_LIST(bufConfig, "bufconfig", "dhw buffer config", "Konfig. Warmwasserspeicher", "Warmwater boiler configuratie", "Konfiguration Varmvattentank", "konfiguracja bufora c.w.u.", "konfigurasjon varmvannstank", "configuration buffer ecs")
MAKE_PSTR_LIST(blockMode, "blockmode", "config htg. blocking mode", "Konfig. Sperr-Modus", "Configuratie blokeermodus", "Konfiguration Blockeringsläge", "konfiguracja trybu blokady", "konfigurasjon blokkeringsmodus", "config mode blocage htg.")
MAKE_PSTR_LIST(blockTerm, "blockterm", "config of block terminal", "Konfig. Sperrterminal", "Configuratie blookerterminal", "Konfiguration Blockeringsterminal", "konfiguracja terminala blokującego", "konfiguration blokkeringsterminal", "config. du bloque terminal")
MAKE_PSTR_LIST(blockHyst, "blockhyst", "hyst. for boiler block", "Hysterese Sperrmodus", "Hysterese blokeerterminal", "Hysteres Blockeringsmodul", "tryb blokowania histerezy", "hystrese blokkeringsmodus", "hyst. Blocage chaudière")
MAKE_PSTR_LIST(releaseWait, "releasewait", "boiler release wait time", "Wartezeit Kessel-Freigabe", "Wachttijd ketel vrijgave", "Väntetid Frisläppning", "czas oczekiwania na zwolnienie kotła", "kjele ventetid frigjøring", "temps attente libération chaudière")

// the following are dhw for the boiler and automatically tagged with 'dhw'
MAKE_PSTR_LIST(wwSelTemp, "wwseltemp", "selected temperature", "gewählte Temperatur", "Geselecteerd temperatuur", "Vald Temperatur", "temperatura wyższa/komfort", "valgt temperatur", "température sélectionnée")
MAKE_PSTR_LIST(wwSelTempLow, "wwseltemplow", "selected lower temperature", "untere Solltemperatur", "Onderste streeftemperatuur", "Vald lägstatemperatur", "temperatura niższa/eko", "valgt nedre temperatur", "température basse sélectionnée")
MAKE_PSTR_LIST(wwSelTempOff, "wwseltempoff", "selected temperature for off", "Solltemperatur bei AUS", "Streeftemperatuur bij UIT", "Vald tempereatur för AV", "temperatura gdy grzanie wyłączone", "valgt tempereatur for av", "température sélectionnée pour arrêt")
MAKE_PSTR_LIST(wwSelTempSingle, "wwseltempsingle", "single charge temperature", "Solltemperatur Einmalladung", "Streeftemperatuur enkele lading", "Temperatur Engångsladdning", "temperatura dodatkowej ciepłej wody", "temp engangsoppvarming", "température charge unique")
MAKE_PSTR_LIST(wwCylMiddleTemp, "wwcylmiddletemp", "cylinder middle temperature (TS3)", "Speichertemperatur Mitte", "Buffer temperatuur midden", "Cylinder Temperatur Mitten (TS3)", "temperatura środka cylindra (TS3)", "vanntank midten temperatur (TS3)", "température moyenne ballon (TS3)")
MAKE_PSTR_LIST(wwSetTemp, "wwsettemp", "set temperature", "Solltemperatur", "Streeftemperatuut", "Börtempertur", "temperatura zadana", "innstilt temperatur", "régler température")
MAKE_PSTR_LIST(wwType, "wwtype", "type", "Typ", "type", "Typ", "typ", "type", "type")
MAKE_PSTR_LIST(wwComfort, "wwcomfort", "comfort", "Komfort", "Comfort", "Komfort", "komfort", "komfort", "confort")
MAKE_PSTR_LIST(wwComfort1, "wwcomfort1", "comfort mode", "Komfort-Modus", "Comfort modus", "Komfortläge", "tryb komfortu", "komfort modus", "mode confort")
MAKE_PSTR_LIST(wwFlowTempOffset, "wwflowtempoffset", "flow temperature offset", "Vorlauftemperaturanhebung", "Aanvoertemperatuur offset", "Flödestemperatur förskjutning", "korekta temperatury wypływu", "flyttemperaturforskyvning", "offset température flux")
MAKE_PSTR_LIST(wwMaxPower, "wwmaxpower", "max power", "max Leistung", "Maximaal vermogen", "Max Effekt", "moc maksymalna", "maks effekt", "puissance max")
MAKE_PSTR_LIST(wwCircPump, "wwcircpump", "circulation pump available", "Zirkulationspumpe vorhanden", "Circulatiepomp aanwezig", "Cirkulationspump tillgänglig", "pompa cyrkulacyjna zainstalowana", "sirkulasjonspumpe tilgjengelig", "pompe circulation disponible")
MAKE_PSTR_LIST(wwChargeType, "wwchargetype", "charging type", "Speicher-Ladungstyp", "Buffer laadtype", "Laddningstyp", "sposób grzania zasobnika", "varmetype", "type chargement")
MAKE_PSTR_LIST(wwDisinfectionTemp, "wwdisinfectiontemp", "disinfection temperature", "Desinfektionstemperatur", "Desinfectietemperatuur", "Desinfektionstemperatur", "temperatura dezynfekcji termicznej", "", "température désinfection") // TODO translate
MAKE_PSTR_LIST(wwCircMode, "wwcircmode", "circulation pump mode", "Zirkulationspumpen-Modus", "Modus circulatiepomp", "Läge Cirkulationspump", "tryb pracy cyrkulacji", "modus sikulasjonspumpe", "mode pompe circulation")
MAKE_PSTR_LIST(wwCirc, "wwcirc", "circulation active", "Zirkulation aktiv", "Circulatiepomp actief", "Cirkulation aktiv", "pompa cyrkulacyjna", "sirkulasjon aktiv", "circulation active")
MAKE_PSTR_LIST(wwCurTemp, "wwcurtemp", "current intern temperature", "aktuelle interne Temperatur", "Huidige interne temperatuur", "Intern Temperatur", "temperatura zasobnika", "gjeldende intern temperatur", "température interne actuelle")
MAKE_PSTR_LIST(wwCurTemp2, "wwcurtemp2", "current extern temperature", "aktuelle externe Temperatur", "Huidige externe temperatuur", "Extern Temperatur", "temperatura wypływu", "gjeldende ekstern temperaur", "température externe actuelle")
MAKE_PSTR_LIST(wwCurFlow, "wwcurflow", "current tap water flow", "aktueller Durchfluss", "Hudige warmwater doorstroming", "Aktuellt tappvattenflöde", "aktualny przepływ", "gjeldende strømningshastighet", "débit actuel eau robinet")
MAKE_PSTR_LIST(wwStorageTemp1, "wwstoragetemp1", "storage intern temperature", "interne Speichertemperatur", "Interne buffertemperatuur", "Beredare Intern Temperatur", "temperatura wewnątrz zasobnika", "intern temperatur bereder", "température interne stockage")
MAKE_PSTR_LIST(wwStorageTemp2, "wwstoragetemp2", "storage extern temperature", "externer Speichertemperatur", "Externe buffertemperatuur", "Beredare Extern Tempereatur", "temperatura na wyjściu zasobnika", "ekstern temperatur bereder", "température externe stockage")
MAKE_PSTR_LIST(wwActivated, "wwactivated", "activated", "aktiviert", "geactiveerd", "Aktiverad", "system przygotowywania c.w.u.", "aktivert", "activé")
MAKE_PSTR_LIST(wwOneTime, "wwonetime", "one time charging", "Einmalladung", "Buffer eenmalig laden", "Engångsladdning", "jednorazowa dodatkowa ciepła woda", "", "charge unique") // TODO translate
MAKE_PSTR_LIST(wwDisinfecting, "wwdisinfecting", "disinfecting", "Desinfizieren", "Desinfectie", "Desinficerar", "dezynfekcja termiczna", "desinfiserer", "désinfection")
MAKE_PSTR_LIST(wwCharging, "wwcharging", "charging", "Laden", "Laden", "Värmer", "grzanie", "varmer", "chargement")
MAKE_PSTR_LIST(wwChargeOptimization, "wwchargeoptimization", "charge optimization", "Ladungsoptimierung", "laadoptimalisatie", "Laddningsoptimering", "optymalizacja grzania", "oppvarmingsoptimalisering", "optimisation charge")
MAKE_PSTR_LIST(wwRecharging, "wwrecharging", "recharging", "Nachladen", "herladen", "Laddar om", "ponowne grzanie", "varm på nytt", "en recharge")
MAKE_PSTR_LIST(wwTempOK, "wwtempok", "temperature ok", "Temperatur ok", "Temperatuur OK", "Temperatur OK", "temperatura OK", "temperatur ok!", "température ok")
MAKE_PSTR_LIST(wwActive, "wwactive", "active", "aktiv", "Actief", "Aktiv", "aktywna", "aktiv", "actif")
MAKE_PSTR_LIST(ww3wayValve, "ww3wayvalve", "3-way valve active", "3-Wegeventil aktiv", "3-wegklep actief", "Trevägsventil aktiv", "zawór 3-drogowy aktywny", "aktiv trevisventil", "vanne 3 voies active")
MAKE_PSTR_LIST(wwSetPumpPower, "wwsetpumppower", "set pump power", "Soll Pumpenleistung", "Streefwaarde pompvermogen", "Vald pumpeffekt", "ustawione wysterowanie pompy", "valgt pumpeeffekt", "régler puissance pompe")
MAKE_PSTR_LIST(wwMixerTemp, "wwmixertemp", "mixer temperature", "Mischertemperatur", "Mixertemperatuur", "Blandningsventil-tempertur", "temperatura mieszacza", "temperatur blandeventil", "température mélangeur")
MAKE_PSTR_LIST(wwStarts, "wwstarts", "starts", "Anzahl Starts", "Aantal starts", "Antal starter", "liczba załączeń", "antall starter", "démarrages")
MAKE_PSTR_LIST(wwStarts2, "wwstarts2", "control starts2", "Kreis 2 Anzahl Starts", "Aantal starts circuit 2", "Antal starter Krets 2", "liczba załączeń 2", "antall starter krets 2", "démarrages contrôle 2")
MAKE_PSTR_LIST(wwWorkM, "wwworkm", "active time", "aktive Zeit", "Actieve tijd", "Aktiv Tid", "czas aktywności", "driftstid", "temps actif")
MAKE_PSTR_LIST(wwHystOn, "wwhyston", "hysteresis on temperature", "Einschalttemperaturdifferenz", "Inschakeltemperatuurverschil", "Hysteres PÅ-temperatur", "histereza załączania", "innkoblingstemperaturforskjell", "hystérésis température allumage")
MAKE_PSTR_LIST(wwHystOff, "wwhystoff", "hysteresis off temperature", "Ausschalttemperaturdifferenz", "Uitschakeltemperatuurverschil", "Hysteres AV-temperatur", "histereza wyłączania", "utkoblingstemperaturforskjell", "hystérésis température extinction")
MAKE_PSTR_LIST(wwProgMode, "wwprogmode", "program", "Programmmodus", "Programma", "Program", "program", "program", "programme")
MAKE_PSTR_LIST(wwCircProg, "wwcircprog", "circulation program", "Zirkulationsprogramm", "Circulatieprogramma", "Cirkulationsprogram", "program cyrkulacji c.w.u.", "sirkulationsprogram", "programme circulation")
MAKE_PSTR_LIST(wwMaxTemp, "wwmaxtemp", "maximum temperature", "Maximale Temperatur", "Maximale temperatuur", "Maximal Temperatur", "temperatura maksymalna", "", "température max") // TODO translate
MAKE_PSTR_LIST(wwOneTimeKey, "wwonetimekey", "one time key function", "Einmalladungstaste", "Knop voor eenmalig laden buffer", "Engångsfunktion", "przycisk jednorazowego ogrzania", "engangsknapp varme", "fonction touche unique")
MAKE_PSTR_LIST(wwSolarTemp, "wwsolartemp", "solar boiler temperature", "Solarboiler Temperatur", "Zonneboiler temperatuur", "Solpanel Temp", "temperatura zasobnika solarnego", "", "température chaudière solaire") // TODO translate

// mqtt values / commands
MAKE_PSTR_LIST(switchtime, "switchtime", "program switchtime", "Programm Schaltzeit", "Programma schakeltijd", "Program Bytestid", "program czasowy", "programbyttetid", "heure commutation programme")
MAKE_PSTR_LIST(switchtime1, "switchtime1", "own1 program switchtime", "Programm 1 Schaltzeit", "Schakeltijd programma 1", "Program 1 Bytestid", "program przełączania 1", "byttetidprogram 1", "heure de commutation programme 1")
MAKE_PSTR_LIST(switchtime2, "switchtime2", "own2 program switchtime", "Programm 2 Schaltzeit", "Schakeltijd programma 2", "Program 2 Bytestid", "program przełączania 2", "byttetid program 2", "heure de changement programme 2")
MAKE_PSTR_LIST(wwswitchtime, "wwswitchtime", "program switchtime", "Programm Schaltzeit", "Warm water programma schakeltijd", "Varmvattenprogram Bytestid", "program czasowy", "byttetid varmtvannsprogram", "heure commutation programme")
MAKE_PSTR_LIST(wwcircswitchtime, "wwcircswitchtime", "circulation program switchtime", "Zirculationsprogramm Schaltzeit", "Schakeltijd circulatieprogramma", "Cirkulationsprogram Bytestid", "program cyrkulacji", "byttetid sirkulasjonsprogram", "heure commutation programme circulation")
MAKE_PSTR_LIST(dateTime, "datetime", "date/time", "Datum/Zeit", "Datum/Tijd", "Datum/Tid", "data i godzina", "dato/tid", "date/heure")
MAKE_PSTR_LIST(errorCode, "errorcode", "error code", "Fehlernummer", "Foutmeldingscode", "Felkod", "kod błędu", "feikode", "code erreur")
MAKE_PSTR_LIST(ibaMainDisplay, "display", "display", "Anzeige", "Display", "Display", "wyświetlacz", "skjerm", "affichage")
MAKE_PSTR_LIST(ibaLanguage, "language", "language", "Sprache", "Taal", "Sprak", "język", "språk", "langue")
MAKE_PSTR_LIST(ibaClockOffset, "clockoffset", "clock offset", "Uhrkorrektur", "Klokcorrectie", "Tidskorrigering", "korekta zegara", "tidskorrigering", "offset horloge")
MAKE_PSTR_LIST(ibaBuildingType, "building", "building type", "Gebäudetyp", "", "Byggnadstyp", "typ budynku", "bygningstype", "type bâtiment") // TODO translate
MAKE_PSTR_LIST(heatingPID, "heatingpid", "heating PID", "Heizungs-PID", "", "Uppvärmning PID", "PID ogrzewania", "oppvarmings PID", "PID chauffage") // TODO translate
MAKE_PSTR_LIST(ibaCalIntTemperature, "intoffset", "internal temperature offset", "Korrektur interner Temperatur", "", "Korrigering interntemperatur", "korekta temperatury w pomieszczeniu", "Korrigering interntemperatur", "offset température interne") // TODO translate
MAKE_PSTR_LIST(ibaMinExtTemperature, "minexttemp", "minimal external temperature", "min. Aussentemperatur", "", "Min Extern Temperatur", "minimalna miejscowa temperatura zewnętrzna", "minimal eksterntemperatur", "température extérieure minimale") // TODO translate
MAKE_PSTR_LIST(backlight, "backlight", "key backlight", "Gegenlicht", "Toetsverlichting", "Bakgrundsbelysning", "podświetlenie klawiatury", "bakgrunnsbelysning", "rétroéclairage touches")
MAKE_PSTR_LIST(damping, "damping", "damping outdoor temperature", "Dämpfung der Außentemperatur", "Demping buitentemperatuur", "Utomhustemperatur dämpning", "tłumienie temperatury zewnętrznej", "demping av utetemperatur", "température extérieure minimale")
MAKE_PSTR_LIST(tempsensor1, "inttemp1", "temperature sensor 1", "Temperatursensor 1", "Temperatuursensor 1", "Temperatursensor 1", "czujnik temperatury 1", "temperatursensor 1", "sonde température 1")
MAKE_PSTR_LIST(tempsensor2, "inttemp2", "temperature sensor 2", "Temperatursensor 2", "Temperatuursensor 2", "Temperatursensor 2", "czujnik temperatury 2", "temperatursensor 2", "capteur température 2")
MAKE_PSTR_LIST(dampedoutdoortemp, "dampedoutdoortemp", "damped outdoor temperature", "gedämpfte Außentemperatur", "Gedempte buitentemperatuur", "Utomhustemperatur dämpad", "tłumiona temperatura zewnętrzna", "dempet utetemperatur", "température extérieure amortie")
MAKE_PSTR_LIST(floordrystatus, "floordry", "floor drying", "Estrichtrocknung", "Vloerdroogprogramma", "Golvtorkning", "suszenie jastrychu", "gulvtørkeprogram", "séchage sol")
MAKE_PSTR_LIST(floordrytemp, "floordrytemp", "floor drying temperature", "Estrichtrocknungs Temperatur", "Temperatuur vloerdroogprogramma", "Golvtorkning Temperatur", "temperatura suszenia jastrychu", "gulvtørketemperatur", "température séchage sol")
MAKE_PSTR_LIST(brightness, "brightness", "screen brightness", "Bildschirmhelligkeit", "Schermhelderheid", "Ljusstyrka", "jasność", "lysstyrke", "luminosité écran")
MAKE_PSTR_LIST(autodst, "autodst", "automatic change daylight saving time", "automatische Sommerzeit Umstellung", "Automatische omschakeling zomer-wintertijd", "Automatisk växling sommar/vinter-tid", "automatycznie przełączaj na czas letni/zimowy", "automatisk skifte av sommer/vinter-tid", "changement automatique heure d'été")
MAKE_PSTR_LIST(preheating, "preheating", "preheating in the clock program", "Vorheizen im Zeitprogramm", "Voorverwarming in het klokprogramma", "Förvärmning i tidsprogram", "podgrzewanie w programie czasowym", "forvarming i tidsprogram", "préchauffage dans programme horloge")
MAKE_PSTR_LIST(offtemp, "offtemp", "temperature when mode is off", "Temperatur bei AUS", "Temperatuur bij UIT", "Temperatur Avslagen", "temperatura w trybie \"wył.\"", "temperatur avslått", "température lorsque mode désactivé")
MAKE_PSTR_LIST(mixingvalves, "mixingvalves", "mixing valves", "Mischventile", "Mengkleppen", "Blandningsventiler", "zawory mieszające", "blandeventiler", "vannes mélange")
MAKE_PSTR_LIST(pvEnableWw, "pvenableww", "enable raise dhw", "aktiviere Anhebung WW", "", "", "", "", "") // TODO translate
MAKE_PSTR_LIST(pvRaiseHeat, "pvraiseheat", "raise heating with PV", "Anhebung Heizen mit PV", "", "", "", "", "") // TODO translate
MAKE_PSTR_LIST(pvLowerCool, "pvlowercool", "lower cooling with PV", "Kühlabsenkung mit PV", "", "", "", "", "") // TODO translate

// thermostat ww
MAKE_PSTR_LIST(wwMode, "wwmode", "mode", "Modus", "Modus", "Läge", "tryb pracy", "modus", "mode")
MAKE_PSTR_LIST(wwSetTempLow, "wwsettemplow", "set low temperature", "untere Solltemperatur", "Onderste streeftemperatuur", "Nedre Börvärde", "zadana temperatura obniżona", "nedre settverdi", "réglage température basse")
MAKE_PSTR_LIST(wwWhenModeOff, "wwwhenmodeoff", "when thermostat mode off", "bei Thermostatmodus AUS", "Als Thermostaat op UIT", "när Termostatläge är AV", "gdy wyłączono na termostacie", "når modus er av", "lorsque mode thermostat off")
MAKE_PSTR_LIST(wwExtra1, "wwextra1", "circuit 1 extra", "Kreis 1 Extra", "Circuit 1 extra", "Krets 1 Extra", "obieg dodatkowy 1", "ekstra krets 1", "circuit 1 extra")
MAKE_PSTR_LIST(wwExtra2, "wwextra2", "circuit 2 extra", "Kreis 2 Extra", "Circuit 2 extra", "Kets 2 Extra", "obieg dodatkowy 2", "ekstra krets 2", "circuit 2 extra")
MAKE_PSTR_LIST(wwCharge, "wwcharge", "charge", "Laden", "Laden", "Ladda", "grzanie", "lade", "charge")
MAKE_PSTR_LIST(wwChargeDuration, "wwchargeduration", "charge duration", "Ladedauer", "Laadtijd", "Laddtid", "czas grzania dodatkowej ciepłej wody", "ladetid", "durée charge")
MAKE_PSTR_LIST(wwDisinfect, "wwdisinfect", "disinfection", "Desinfektion", "Desinfectie", "Desinfektion", "dezynfekcja termiczna", "desinfeksjon", "désinfection")
MAKE_PSTR_LIST(wwDisinfectDay, "wwdisinfectday", "disinfection day", "Desinfektionstag", "Desinfectiedag", "Desinfektionsdag", "dzień dezynfekcji termicznej", "desinfeksjonsdag", "jour désinfection")
MAKE_PSTR_LIST(wwDisinfectHour, "wwdisinfecthour", "disinfection hour", "Desinfektionsstunde", "Desinfectieuur", "Desinfektionstimme", "godzina dezynfekcji termicznej", "desinfeksjonstime", "heure désinfection")
MAKE_PSTR_LIST(wwDisinfectTime, "wwdisinfecttime", "disinfection time", "Desinfektionsdauer", "Desinfectietijd", "Desinfektionstid", "maksymalny czas trwania dezynfekcji termicznej", "desinfeksjonstid", "durée désinfection")
MAKE_PSTR_LIST(wwDailyHeating, "wwdailyheating", "daily heating", "täglich Heizen", "Dagelijks opwarmen", "Daglig Uppvärmning", "codzienne nagrzewanie", "", "chauffage quotidien") // TODO translate
MAKE_PSTR_LIST(wwDailyHeatTime, "wwdailyheattime", "daily heating time", "tägliche Heizzeit", "Tijd dagelijkse opwarming", "Daglig Uppvärmningstid", "czas trwania codziennego nagrzewania", "daglig oppvarmingstid", "heure chauffage quotidien")

// thermostat hc
MAKE_PSTR_LIST(selRoomTemp, "seltemp", "selected room temperature", "Sollwert Raumtemperatur", "Streeftemperatuur kamer", "Vald Rumstemperatur", "zadana temperatura w pomieszczeniu", "valgt rumstemperatur", "température ambiante sélectionnée")
MAKE_PSTR_LIST(roomTemp, "currtemp", "current room temperature", "aktuelle Raumtemperatur", "Huidige kamertemperatuur", "Aktuell Rumstemperatur", "temperatura w pomieszczeniu", "gjeldende romstemperatur", "température ambiante actuelle")
MAKE_PSTR_LIST(mode, "mode", "mode", "Modus", "Modus", "Läge", "sposób sterowania", "modus", "mode")
MAKE_PSTR_LIST(modetype, "modetype", "mode type", "Modus Typ", "Type modus", "Typ av läge", "aktualny tryb pracy", "modusrype", "type mode")
MAKE_PSTR_LIST(fastheatup, "fastheatup", "fast heatup", "schnelles Aufheizen", "Snel opwarmen", "Snabb Uppvärmning", "szybkie nagrzewanie", "rask oppvarming", "chauffage rapide")
MAKE_PSTR_LIST(daytemp, "daytemp", "day temperature", "Tagestemperatur", "temperatuur dag", "Dagstemperatur", "temperatura w dzień", "dagtemperatur", "température jour")
MAKE_PSTR_LIST(daylowtemp, "daytemp2", "day temperature T2", "Tagestemperatur T2", "Temperatuur dag T2", "Dagstemperatur T2", "temperatura w dzień T2", "dagtemperatur T2", "température jour T2")
MAKE_PSTR_LIST(daymidtemp, "daytemp3", "day temperature T3", "Tagestemperatur T3", "Temperatuur dag T3", "Dagstemperatur T3", "temperatura w dzień T3", "dagtemperatur T3", "température jour T3")
MAKE_PSTR_LIST(dayhightemp, "daytemp4", "day temperature T4", "Tagestemperatur T4", "Temperatuur dag T4", "Dagstemperatur T4", "temperatura w dzień T4", "dagtemperatur T4", "température jour T4")
MAKE_PSTR_LIST(heattemp, "heattemp", "heat temperature", "Heizen Temperatur", "Temperatuur verwarming", "Temperatur Uppvärmning", "temperatura ogrzewania", "oppvarmingstemperatur", "température chauffage")
MAKE_PSTR_LIST(nighttemp, "nighttemp", "night temperature", "Nachttemperatur", "Nachttemperatuur", "Nattemperatur", "temperatura w nocy", "nattemperatur", "température de nuit")
MAKE_PSTR_LIST(nighttemp2, "nighttemp", "night temperature T1", "Nachttemperatur T1", "Nachttemperatuur T1", "Nattemperatur T1", "temperatura w nocy T1", "nattemperatur T1", "température nuit T1")
MAKE_PSTR_LIST(ecotemp, "ecotemp", "eco temperature", "eco Temperatur", "Temperatuur eco", "Eko-temperatur", "temperatura w trybie eko", "øko temperatur", "température éco")
MAKE_PSTR_LIST(manualtemp, "manualtemp", "manual temperature", "manuelle Temperatur", "Temperatuur handmatig", "Temperatur Manuell", "temperatura ustawiona ręcznie", "manuell temperatur", "température manuelle")
MAKE_PSTR_LIST(tempautotemp, "tempautotemp", "temporary set temperature automode", "temporäre Solltemperatur", "Streeftemperatuur automodus tijdelijk", "Temporär Aktivering av Auto-läge", "zadana temperatura w pomieszczenia w trybie \"auto\" (tymczasowa)", "temporær valgt temp i automodus", "température temporaire mode automatique")
MAKE_PSTR_LIST(remoteseltemp, "remoteseltemp", "temporary set temperature from remote", "temporäre Solltemperatur Remote", "Temperatuur van afstandsbedieding", "Temperatur från fjärruppkoppling", "zadana zdalnie temperatura a pomieszczeniu (tymczasowa)", "temporær valgt temp fra fjernbetjening", "température temporaire depuis télécommande")
MAKE_PSTR_LIST(comforttemp, "comforttemp", "comfort temperature", "Komforttemperatur", "Comforttemperatuur", "Komforttemperatur", "temperatura w trybie komfort", "komforttemperatur", "température confort")
MAKE_PSTR_LIST(summertemp, "summertemp", "summer temperature", "Sommertemperatur", "Zomertemperatuur", "Sommartemperatur", "temperatura przełączania lato/zima", "Sommertemperatur", "température été")
MAKE_PSTR_LIST(designtemp, "designtemp", "design temperature", "Auslegungstemperatur", "Ontwerptemperatuur", "Design-temperatur", "temperatura projektowa", "designtemperatur", "température conception")
MAKE_PSTR_LIST(offsettemp, "offsettemp", "offset temperature", "Temperaturanhebung", "Temperatuur offset", "Temperaturkorrigering", "korekta temperatury", "temperaturkorrigering", "température offset")
MAKE_PSTR_LIST(minflowtemp, "minflowtemp", "min flow temperature", "min Vorlauftemperatur", "", "Min Flödestemperatur", "minimalna temperatura zasilania", "min turtemperatur", "température min flux") // TODO translate
MAKE_PSTR_LIST(maxflowtemp, "maxflowtemp", "max flow temperature", "max Vorlauftemperatur", "", "Max Flödestemperatur", "maksymalna temperatura zasilania", "maks turtemperatur", "température max flux") // TODO translate
MAKE_PSTR_LIST(roominfluence, "roominfluence", "room influence", "Raumeinfluss", "Ruimteinvloed", "Rumspåverkan", "wpływ pomieszczenia", "rominnflytelse", "influence pièce")
MAKE_PSTR_LIST(roominfl_factor, "roominflfactor", "room influence factor", "Raumeinflussfaktor", "Factor ruimteinvloed", "Rumspåverkansfaktor", "współczynnik wpływu pomieszczenia", "rominnflytelsesfaktor", "facteur d'influence pièce")
MAKE_PSTR_LIST(curroominfl, "curroominfl", "current room influence", "aktueller Raumeinfluss", "Huidige ruimteinvloed", "Aktuell Rumspåverkan", "aktualny wpływ pomieszczenia", "gjeldende rominnflytelse", "influence actuelle pièce")
MAKE_PSTR_LIST(nofrosttemp, "nofrosttemp", "nofrost temperature", "Frostschutztemperatur", "Temperatuur vorstbeveiliging", "Temperatur Frostskydd", "temperatura ochrony przed zamarzaniem", "frostbeskyttelsestemperatur", "température protection gel")
MAKE_PSTR_LIST(targetflowtemp, "targetflowtemp", "target flow temperature", "berechnete Vorlauftemperatur", "Berekende aanvoertemperatuur", "Målvärde Flödestemperatur", "zadana temperatura zasilania", "målverdi turtemperatur", "température cible flux")
MAKE_PSTR_LIST(heatingtype, "heatingtype", "heating type", "Heizungstyp", "Verwarmingstype", "Värmesystem", "system grzewczy", "varmesystem", "type chauffage")
MAKE_PSTR_LIST(summersetmode, "summersetmode", "set summer mode", "Einstellung Sommerbetrieb", "Instelling zomerbedrijf", "Aktivera Sommarläge", "tryb lato/zima", "aktiver sommermodus", "activer mode été")
MAKE_PSTR_LIST(hpoperatingmode, "hpoperatingmode", "heatpump operating mode", "Wärmepumpe Betriebsmodus", "Bedrijfsmodus warmtepomp", "Värmepump Driftläge", "tryb pracy pompy ciepła", "driftsmodus varmepumpe", "mode fonctionnement pompe à chaleur")
MAKE_PSTR_LIST(hpoperatingstate, "hpoperatingstate", "heatpump operating state", "WP Arbeitsweise", "Huidige modus warmtepomp", "Värmepump driftläge", "aktualny tryb pracy pompy ciepła", "driftstatus varmepumpe", "état fonctionnement pompe à chaleur")
MAKE_PSTR_LIST(controlmode, "controlmode", "control mode", "Kontrollmodus", "Comtrolemodus", "Kontrolläge", "tryb sterowania", "kontrollmodus", "mode régulation")
MAKE_PSTR_LIST(control, "control", "control device", "Fernsteuerung", "Afstandsbedieding", "Kontrollenhet", "sterownik", "kontrollenhet", "dispositif régulation")
MAKE_PSTR_LIST(roomsensor, "roomsensor", "room sensor", "Raumsensor", "Ruimtesensor", "Rumssensor", "czujnik temperatury pomieszczenia", "romsensor", "capteur pièce")
MAKE_PSTR_LIST(program, "program", "program", "Programm", "Programma", "Program", "program", "program", "programme")
MAKE_PSTR_LIST(pause, "pause", "pause time", "Pausenzeit", "Pausetijd", "Paustid", "czas przerwy", "pausetid", "temps de pause")
MAKE_PSTR_LIST(party, "party", "party time", "Partyzeit", "Partytijd", "Partytid", "czas przyjęcia", "partytid", "temps de fête")
MAKE_PSTR_LIST(holidaytemp, "holidaytemp", "holiday temperature", "Urlaubstemperatur", "Vakantietemperatuur", "Helgtemperatur", "temperatura w trybie urlopowym", "ferietemperatur", "température vacances")
MAKE_PSTR_LIST(summermode, "summermode", "summer mode", "Sommerbetrieb", "Zomerbedrijf", "Sommarläge", "aktualny tryb lato/zima", "sommermodus", "mode été")
MAKE_PSTR_LIST(holidaymode, "holidaymode", "holiday mode", "Urlaubsbetrieb", "Vakantiebedrijf", "Helgläge", "tryb urlopowy", "feriemodus", "mode vacances")
MAKE_PSTR_LIST(flowtempoffset, "flowtempoffset", "flow temperature offset for mixer", "Vorlauftemperaturanhebung", "Mixer aanvoertemperatuur offset", "Temperaturkorrigering Flödestemp. Blandningsventil", "korekta temperatury przepływu dla miksera", "temperaturkorrigering av blandingsventil", "décalage température de bascule pour mélangeur")
MAKE_PSTR_LIST(reducemode, "reducemode", "reduce mode", "Absenkmodus", "Gereduceerde modus", "Reducerat Läge", "tryb zredukowany/obniżony", "", "mode réduction") // TODO translate
MAKE_PSTR_LIST(noreducetemp, "noreducetemp", "no reduce below temperature", "Durchheizen unter", "Reduceermodus onderbreken onder", "Inaktivera reducering under", "bez redukcji poniżej temperatury", "inaktiver redusert nedre temp", "pas de réduction en dessous température")
MAKE_PSTR_LIST(reducetemp, "reducetemp", "off/reduce switch temperature", "Absenkmodus unter", "Onderste afschakeltemperatuur", "Avslag/Reducera under", "tryb zredukowany poniżej temperatury", "nedre avstengningstemperatur", "arrêt/réduction température bascule")
MAKE_PSTR_LIST(vacreducetemp, "vacreducetemp", "vacations off/reduce switch temperature", "Urlaub Absenkmodus unter", "Vakantiemodus onderste afschakeltemperatuur", "Helg Avslag/Reducering under", "tryb urlopowy poniżej temperatury", "feriemodus nedre utkoblingstemperatur", "vacances – arrêt/réduction température bascule")
MAKE_PSTR_LIST(vacreducemode, "vacreducemode", "vacations reduce mode", "Urlaub Absenkmodus", "Vakantie afschakelmodus", "Helg reduceringsläge", "redukcja w trakcie urlopu", "ferieavstengningsmodus", "mode réduction vacances")
MAKE_PSTR_LIST(nofrostmode, "nofrostmode", "nofrost mode", "Frostschutz Modus", "Vorstbeveiligingsmodus", "Frostskyddsläge", "temperatura wiodąca dla ochrony przed zamarzaniem", "frostbeskyttelsesmodus", "mode protection gel")
MAKE_PSTR_LIST(remotetemp, "remotetemp", "room temperature from remote", "Raumtemperatur Remote", "Ruimtetemperatuur van afstandsbediening", "Rumstemperatur från fjärr", "temperatura w pomieszczeniu (z termostatu)", "romstemperatur fra fjernbetjening", "température pièce depuis télécommande")
MAKE_PSTR_LIST(wwHolidays, "wwholidays", "holiday dates", "Feiertage", "Feestdagen", "Helgdagar", "dni świąteczne", "feriedager varmtvann", "dates vacances")
MAKE_PSTR_LIST(wwVacations, "wwvacations", "vacation dates", "Urlaubstage", "Vakantiedagen", "Semesterdatum Varmvatten", "dni urlopowe", "ferie dato varmtvann", "dates vacances")
MAKE_PSTR_LIST(holidays, "holidays", "holiday dates", "Feiertage", "Feestdagen", "Helgdatum", "święta", "helligdager", "dates vacances")
MAKE_PSTR_LIST(vacations, "vacations", "vacation dates", "Urlaubstage", "Vakantiedagen", "Semesterdatum", "urlop", "feriedager", "dates vacances")
MAKE_PSTR_LIST(wwprio, "wwprio", "dhw priority", "WW-Vorrang", "Prioriteit warm water", "Prioritera Varmvatten", "priorytet dla c.w.u.", "prioroter varmtvann", "priorité ecs")
MAKE_PSTR_LIST(nofrostmode1, "nofrostmode1", "nofrost mode", "Frostschutz", "Vorstbeveiligingsmodus", "Frostskyddsläge", "ochrona przed zamarzaniem", "frostbeskyttelse", "mode protection gel")
MAKE_PSTR_LIST(reducehours, "reducehours", "duration for nighttemp", "Dauer Nachttemp.", "Duur nachtverlaging", "Timmar Nattsänkning", "czas trwania trybu nocnego", "timer nattsenkning", "durée température nuit")
MAKE_PSTR_LIST(reduceminutes, "reduceminutes", "remaining time for nightmode", "Restzeit Nachttemp.", "Resterende tijd nachtverlaging", "Återstående Tid Nattläge", "czas do końca trybu nocnego", "gjenværende tid i nattstilling", "temps restant mode nuit")
MAKE_PSTR_LIST(switchonoptimization, "switchonoptimization", "switch-on optimization", "Einschaltoptimierung", "Inschakeloptimalisering", "Växlingsoptimering", "optymalizacja załączania", "slå på optimalisering", "optimisation mise en marche")

// heatpump
MAKE_PSTR_LIST(airHumidity, "airhumidity", "relative air humidity", "relative Luftfeuchte", "Relatieve luchtvochtigheid", "Relativ Luftfuktighet", "wilgotność względna w pomieszczeniu", "luftfuktighet", "humidité relative air")
MAKE_PSTR_LIST(dewTemperature, "dewtemperature", "dew point temperature", "Taupunkttemperatur", "Dauwpunttemperatuur", "Daggpunkt", "punkt rosy w pomieszczeniu", "duggtemperatur", "température point rosée")

// mixer
MAKE_PSTR_LIST(flowSetTemp, "flowsettemp", "setpoint flow temperature", "Sollwert Vorlauftemperatur", "Streefwaarde aanvoertemperatuur", "Vald flödestemperatur", "zadana temperatura zasilania", "valgt turtemperatur", "consigne température flux")
MAKE_PSTR_LIST(flowTempHc, "flowtemphc", "flow temperature (TC1)", "Vorlauftemperatur HK (TC1)", "Aanvoertemperatuut circuit (TC1)", "Flödestemperatur (TC1)", "temperatura zasilania (TC1)", "turtemperatur (TC1)", "température flux (TC1)")
MAKE_PSTR_LIST(pumpStatus, "pumpstatus", "pump status (PC1)", "Pumpenstatus HK (PC1)", "pompstatus circuit (PC1)", "Pumpstatus (PC1)", "status pompy (PC1)", "pumpestatus (PC1)", "état pompe (PC1)")
MAKE_PSTR_LIST(mixerStatus, "valvestatus", "mixing valve actuator (VC1)", "Mischerventil Position (VC1)", "positie mixerklep (VC1)", "Shuntventil Status (VC1)", "siłownik zaworu mieszającego (VC1)", "shuntventil status (VC1)", "actionnement vanne mélangeur (VC1)")
MAKE_PSTR_LIST(flowTempVf, "flowtempvf", "flow temperature in header (T0/Vf)", "Vorlauftemperatur am Verteiler (T0/Vf)", "aanvoertemperatuur verdeler (T0/Vf)", "Flödestemperatur Fördelare (T0/Vf)", "temperatura zasilania na rozdzielaczu (T0/Vf)", "turtemperatur ved fordeleren (T0/Vf)", "température départ collecteur (T0/Vf)")
MAKE_PSTR_LIST(mixerSetTime, "valvesettime", "time to set valve", "Zeit zum Einstellen des Ventils", "Inschakeltijd mengklep", "Inställningstid Ventil", "czas na ustawienie zaworu", "instillningstid ventil", "délai activation vanne")
// mixer prefixed with wwc
MAKE_PSTR_LIST(wwPumpStatus, "pumpstatus", "pump status in assigned wwc (PC1)", "Pumpenstatus des wwk (PC1)", "Pompstatus in WW circuit (PC1)", "Pumpstatus i VV-krets (PC1)", "stan pompy w obwodzie c.w.u. (PC1)", "Pumpestatus i VV-krets (PC1)", "état pompe wwc (PC1)")
MAKE_PSTR_LIST(wwTempStatus, "wwtempstatus", "temperature switch in assigned wwc (MC1)", "Temperaturschalter des wwk (MC1)", "Temperatuurschakeling in WW circuit (MC1)", "Temperaturventil i VV-krets (MC1)", "temperatura w obwodzie c.w.u. (MC1)", "temperaturventil i VV-krets (MC1)", "température bascule wwc (MC1).")
MAKE_PSTR_LIST(wwTemp, "wwtemp", "current temperature", "aktuelle Temperatur", "huidige temperatuur", "Aktuell Temperatur", "temperatura c.w.u.", "aktuell temperatur", "température actuelle")
// mixer pool
MAKE_PSTR_LIST(poolSetTemp, "poolsettemp", "pool set temperature", "Pool Solltemperatur", "Streeftemperatuur zwembad", "Pool Temperatur Börvärde", "zadana temperatura basenu", "valgt temp basseng", "température consigne piscine")
MAKE_PSTR_LIST(poolTemp, "pooltemp", "pool temperature", "Pool Temperatur", "Zwembadtemperatuur", "Pooltemperatur", "temperatura basenu", "bassengtemperatur", "température piscine")
MAKE_PSTR_LIST(poolShuntStatus, "poolshuntstatus", "pool shunt status opening/closing", "Pool Ventil öffnen/schließen", "Zwembadklep status openen/sluiten", "Pool Shunt-status öppnen/stängd", "status bocznika basenu", "bassengshunt-status åpen/stengt", "état shunt piscine ouvert/fermé")
MAKE_PSTR_LIST(poolShunt, "poolshunt", "pool shunt open/close (0% = pool / 100% = heat)", "Pool Ventil Öffnung", "Mengklep zwembad stand", "Pool Shunt Öppen/Stängd", "bocznik basenu (0% = basen / 100% = grzanie)", "bassengshunt åpen/stengt (0% = basseng / 100% = varme)", "ouverture/fermeture shunt piscine (0% = piscine / 100% = chaleur).")
MAKE_PSTR_LIST(hydrTemp, "hydrTemp", "hydraulic header temperature", "Verteilertemperatur", "Temperatuur open verdeler", "Fördelartemperatur", "temperatura kolektora hydraulicznego", "Fordelertemperatur", "température collecteur hydraulique")

// solar
MAKE_PSTR_LIST(cylMiddleTemp, "cylmiddletemp", "cylinder middle temperature (TS3)", "Speichertemperatur Mitte (TS3)", "Zonneboilertemperatuur midden (TS3)", "Cylindertemperatur Mitten (TS3)", "temperatura w środku zasobnika (TS3)", "beredertemperatur i midten (TS3)", "température moyenne cylindre (TS3)")
MAKE_PSTR_LIST(retHeatAssist, "retheatassist", "return temperature heat assistance (TS4)", "Rücklaufanhebungs-Temp. (TS4)", "Retourtemperatuur verwarmingsassistentie (TS4)", "Returtemperatur värmestöd (TS4)", "temperatura powrotu wspomagania grzania (TS4)", "returtemperatur varmestøtte (TS4)", "température retour de assistance thermique (TS4)")
MAKE_PSTR_LIST(m1Valve, "heatassistvalve", "heat assistance valve (M1)", "Ventil Heizungsunterstützung (M1)", "Klep verwarmingsassistentie (M1)", "Uppvärmningsstöd Ventil (M1)", "zawór wspomagania grzania (M1)", "varmehjelpsventil (M1)", "vanne assistance thermique (M1)")
MAKE_PSTR_LIST(m1Power, "heatassistpower", "heat assistance valve power (M1)", "Ventilleistung Heizungsunterstützung (M1)", "Vermogen klep verwarmingsassistentie (M1)", "Uppvärmningsstöd Ventil Effekt (M1)", "moc zaworu wspomagania grzania (M1)", "varmehjelpsventileffekt (M1)", "puissance vanne assistance thermique (M1)")
MAKE_PSTR_LIST(pumpMinMod, "pumpminmod", "minimum pump modulation", "minimale Pumpenmodulation", "Minimale pompmodulatie", "Min Pumpmodulering", "minimalna modulacja pompy", "minimum pumpmodulering", "modulation minimale pompe")
MAKE_PSTR_LIST(maxFlow, "maxflow", "maximum solar flow", "maximaler Durchfluss", "Maximale doorstroom solar", "Max Flöde Solpanel", "maksymalny przepływ solarów", "maks strømming solpanel ", "débit solaire maximum")
MAKE_PSTR_LIST(solarPower, "solarpower", "actual solar power", "aktuelle Solarleistung", "Huidig solar vermogen", "Aktuellt Sol-effekt", "aktualna moc solarów", "aktuell soleffekt", "puissance solaire réelle")
MAKE_PSTR_LIST(solarPumpTurnonDiff, "turnondiff", "pump turn on difference", "Einschalthysterese Pumpe", "Inschakelhysterese pomp", "Aktiveringshysteres Pump", "histereza załączenia pompy", "slå på hysteresepumpe", "différence activation pompe")
MAKE_PSTR_LIST(solarPumpTurnoffDiff, "turnoffdiff", "pump turn off difference", "Ausschalthysterese Pumpe", "Uitschakelhysterese pomp", "Avslagshysteres Pump", "histereza włączenia pompy", "slå av hysteresepumpe", "différence arrêt pompe")
MAKE_PSTR_LIST(pump2MinMod, "pump2minmod", "minimum pump 2 modulation", "minimale Modulation Pumpe 2", "Minimale modulatie pomp 2", "Min Modulering Pump 2", "minimalna modulacja pompy 2", "minimum pumpmodulering 2", "modulation minimale pompe 2")
MAKE_PSTR_LIST(solarPump2TurnonDiff, "turnondiff2", "pump 2 turn on difference", "Einschalthysterese Pumpe 2", "Inschakelhysterese pomp 2", "Aktiveringshysteres Pump 2", "histereza załączenia pompy 2", "slå på hysteresepumpe 2", "différence activation pompe 2")
MAKE_PSTR_LIST(solarPump2TurnoffDiff, "turnoffdiff2", "pump 2 turn off difference", "Ausschalthysterese Pumpe 2", "Uitschakelhysterese pomp 2", "Avslagshysteres Pump 2", "histereza wyłączenia pompy 2", "slå av hysteresepumpe 2", "différence arrêt pompe 2")
MAKE_PSTR_LIST(collectorTemp, "collectortemp", "collector temperature (TS1)", "Kollektortemperatur (TS1)", "Collectortemperatuur (TS1)", "Kollektor Temperatur (TS1)", "temperatura kolektora (TS1)", "kollektor temperatur (TS1)", "température collecteur (TS1)")
MAKE_PSTR_LIST(collector2Temp, "collector2temp", "collector 2 temperature (TS7)", "Kollector 2 Temperatur (TS7)", "Collector 2 temperatuur (TS7)", "Kollektor 2 Temperatur (TS7)", "temperatura kolektora 2 (TS7)", "kollektor 2 temperatur (TS7)", "température collecteur 2 (TS7)")
MAKE_PSTR_LIST(cylBottomTemp, "cylbottomtemp", "cylinder bottom temperature (TS2)", "Speicher Bodentemperatur (TS2)", "Bodemtemperatuur zonneboiler (TS2)", "Cylindertemperatur Botten (TS2)", "temperatura na spodzie zasobnika (TS2)", "beredertemp i bunn (TS2)", "température fond de cylindre (TS2)")
MAKE_PSTR_LIST(cyl2BottomTemp, "cyl2bottomtemp", "second cylinder bottom temperature (TS5)", "2. Speicher Bodentemperatur (TS5)", "Bodemtemperatuur 2e boiler", "Sekundär Cylindertemperatur Botten (TS5)", "temperatura na spodzie drugiego zasobnika (TS5)", "skundær beredertemp i bunn (TS5)", "température fond de cylindre (TS5)")
MAKE_PSTR_LIST(heatExchangerTemp, "heatexchangertemp", "heat exchanger temperature (TS6)", "wärmetauscher Temperatur (TS6)", "Temperatuur warmtewisselaar (TS6)", "Värmeväxlare Temperatur (TS6)", "temperatura wymiennika ciepła (TS6)", "", "température échangeur de chaleur (TS6)") // TODO translate
MAKE_PSTR_LIST(collectorMaxTemp, "collectormaxtemp", "maximum collector temperature", "maximale Kollektortemperatur", "Maximale collectortemperatuur", "Max Kollektortemperatur", "maksymalna temperatura kolektora", "maks kollektortemperatur", "température max. collecteur")
MAKE_PSTR_LIST(collectorMinTemp, "collectormintemp", "minimum collector temperature", "minimale Kollektortemperatur", "Minimale collectortemperatuur", "Min Kollektortemperatur", "minimalna temperatura kolektora", "min kollektortemperatur", "température min. collecteur")
MAKE_PSTR_LIST(cylMaxTemp, "cylmaxtemp", "maximum cylinder temperature", "maximale Speichertemperatur", "maximale temperatuur zonneboiler", "Max Cylindertemperatur", "maksymalna temperatura zasobnika", "maks beredertemperatur", "température max. cylindre")
MAKE_PSTR_LIST(solarPumpMod, "solarpumpmod", "pump modulation (PS1)", "Pumpenmodulation (PS1)", "Pompmodulatie (PS1)", "Pumpmodulering (PS1)", "modulacja pompy solarnej (PS1)", "solpumpmodulering (PS1)", "modulation pompe (PS1)")
MAKE_PSTR_LIST(cylPumpMod, "cylpumpmod", "cylinder pump modulation (PS5)", "Speicherpumpenmodulation (PS5)", "Modulatie zonneboilerpomp (PS5)", "Cylinderpumpmodulering (PS5)", "modulacja pompy zasobnika (PS5)", "", "modulation pompe cylindre (PS5)") // TODO translate
MAKE_PSTR_LIST(solarPump, "solarpump", "pump (PS1)", "Pumpe (PS1)", "Pomp (PS1)", "Pump (PS1)", "pompa solarna (PS1)", "solpumpe (PS1)", "pompe solaire (PS1)")
MAKE_PSTR_LIST(solarPump2, "solarpump2", "pump 2 (PS4)", "Pumpe 2 (PS4)", "Pomp 2 (PS4)", "Pump 2 (PS4)", "pompa solarna 2 (PS4)", "solpumpe 2 (PS4)", "pompe 2 (PS4)")
MAKE_PSTR_LIST(solarPump2Mod, "solarpump2mod", "pump 2 modulation (PS4)", "Pumpe 2 Modulation (PS4)", "Modulatie pomp 2 (PS4)", "Pump 2 Modulering (PS4)", "modulacja pompy solarnej 2 (PS4)", "solpumpe2modulering (PS4)", "modulation pompe solaire 2 (PS4)")
MAKE_PSTR_LIST(valveStatus, "valvestatus", "valve status", "Ventilstatus", "Klepstatus", "Ventilstatus", "stan zaworu", "ventilstatus", "statut valve")
MAKE_PSTR_LIST(cylHeated, "cylheated", "cyl heated", "Speichertemperatur erreicht", "Boilertemperatuur behaald", "Värmepanna Uppvärmd", "zasobnik został nagrzany", "bereder oppvarmt", "cylindre chauffé")
MAKE_PSTR_LIST(collectorShutdown, "collectorshutdown", "collector shutdown", "Kollektorabschaltung", "Collector afschakeling", "Kollektor Avstängning", "wyłączenie kolektora", "kollektor stengt", "arrêt collecteur")
MAKE_PSTR_LIST(pumpWorkTime, "pumpworktime", "pump working time", "Pumpenlaufzeit", "Pomplooptijd", "Pump Drifttid", "czas pracy pompy", "driftstid pumpe", "durée fonctionnement pompe")
MAKE_PSTR_LIST(pump2WorkTime, "pump2worktime", "pump 2 working time", "Pumpe 2 Laufzeit", "Looptijd pomp 2", "Pump 2 Drifttid", "czas pracy pompy 2", "driftstid pumpe2", "durée fonctionnement pompe 2")
MAKE_PSTR_LIST(m1WorkTime, "m1worktime", "differential control working time", "Differenzregelung Arbeitszeit", "Verschilregeling arbeidstijd", "Differentialreglering Drifttid", "czas pracy regulacji różnicowej", "differentialreguleringssrifttid", "durée fonctionnement contrôle différentiel")
MAKE_PSTR_LIST(energyLastHour, "energylasthour", "energy last hour", "Energie letzte Std", "Energie laatste uur", "Energi Senaste Timmen", "energia w ciągu ostatniej godziny", "energi siste time", "énergie dernière heure")
MAKE_PSTR_LIST(energyTotal, "energytotal", "total energy", "Gesamtenergie", "Totale energie", "Total Energi", "energia całkowita", "total energi", "énergie totale") 
MAKE_PSTR_LIST(energyToday, "energytoday", "total energy today", "Energie heute", "Energie vandaag", "Total Energi Idag", "energia całkowita dzisiaj", "total energi i dag", "énergie totale aujourd'hui")

// solar ww
MAKE_PSTR_LIST(wwTemp1, "wwtemp1", "temperature 1", "Temperatur 1", "Temperatuur 1", "Temperatur 1", "temperatura 1", "temperatur 1", "température 1")
MAKE_PSTR_LIST(wwTemp3, "wwtemp3", "temperature 3", "Temperatur 3", "Temperatuur 2", "Temperatur 2", "temperatura 2", "Temperatur 3", "température 3")
MAKE_PSTR_LIST(wwTemp4, "wwtemp4", "temperature 4", "Temperatur 4", "Temperatuur 3", "Temperatur 3", "temperatura 3", "Temperatur 4", "température 4")
MAKE_PSTR_LIST(wwTemp5, "wwtemp5", "temperature 5", "Temperatur 5", "Temperatuur 5", "Temperatur 4", "temperatura 4", "Temperatur 5", "température 5")
MAKE_PSTR_LIST(wwTemp7, "wwtemp7", "temperature 7", "Temperatur 7", "Temperatuur 7", "Temperatur 5", "temperatura 5", "Temperatur 7", "température 7")
MAKE_PSTR_LIST(wwPump, "wwpump", "pump", "Pumpe", "Pomp", "Pump", "pompa", "", "pompe") // TODO translate
// solar ww and mixer wwc
MAKE_PSTR_LIST(wwMinTemp, "wwmintemp", "minimum temperature", "minimale Temperatur", "Minimale temperatuur", "Min Temperatur", "temperatura minimalna", "min temperatur", "température min")
MAKE_PSTR_LIST(wwRedTemp, "wwredtemp", "reduced temperature", "reduzierte Temperatur", "Gereduceerde temperatuur", "Reducerad Temperatur", "temperatura zredukowana", "reducert temperatur", "température réduite")
MAKE_PSTR_LIST(wwDailyTemp, "wwdailytemp", "daily temperature", "tägl. Temperatur", "Dagelijkse temperatuur", "Daglig temperatur", "temperatura dzienna", "dagtemperatur", "température en journée")
MAKE_PSTR_LIST(wwKeepWarm, "wwkeepwarm", "keep warm", "Warmhalten", "Warm houde", "Varmhållning", "utrzymywanie ciepła", "holde varmen", "maintenir chaleur")
MAKE_PSTR_LIST(wwStatus2, "wwstatus2", "status 2", "Status 2", "Status 2", "Status 2", "status 2", "status 2", "statut 2")
MAKE_PSTR_LIST(wwPumpMod, "wwpumpmod", "pump modulation", "Pumpen Modulation", "Pompmodulatie", "Pumpmodulering", "modulacja pompy", "pumpemodulering", "modulation de pompe")
MAKE_PSTR_LIST(wwFlow, "wwflow", "flow rate", "Volumenstrom", "Doorstroomsnelheid", "Flöde", "przepływ", "strømningshastighet", "débit")
// extra mixer ww
MAKE_PSTR_LIST(wwRequiredTemp, "wwrequiredtemp", "required temperature", "benötigte Temperatur", "Benodigde temperatuur", "Nödvändig Temperatur", "temperatura wymagana", "nødvendig temperatur", "température requise")
MAKE_PSTR_LIST(wwDiffTemp, "wwdifftemp", "start differential temperature", "Start Differential Temperatur", "Start differentiele temperatuur", "Start Differentialtemperatur", "start temperatury różnicowej", "start differensialtemperatur", "température différentielle de départ")

// SM100
MAKE_PSTR_LIST(heatTransferSystem, "heattransfersystem", "heattransfer system", "Wärmeübertragungs-System", "Warmteoverdrachtssysteem", "Värmeöverföringssystem", "system wymiany ciepła", "varmeoverføringssystem", "système de transfert de chaleur")
MAKE_PSTR_LIST(externalCyl, "externalcyl", "external cylinder", "Externer Speicher", "Externe boiler", "Extern Cylinder", "zbiornik zewnętrzny", "ekstern bereder", "cylindre externe")
MAKE_PSTR_LIST(thermalDisinfect, "thermaldisinfect", "thermal disinfection", "Thermische Desinfektion", "Thermische desinfectie", "Termisk Desinfektion", "dezynfekcja termiczna", "termisk desinfeksjon", "désinfection thermique")
MAKE_PSTR_LIST(heatMetering, "heatmetering", "heatmetering", "Wärmemessung", "warmtemeting", "Värmemätning", "pomiar ciepła", "varmemåling", "mesure de chaleur")
MAKE_PSTR_LIST(solarIsEnabled, "solarenabled", "solarmodule enabled", "Solarmodul aktiviert", "Solarmodule geactiveerd", "Solmodul Aktiverad", "system solarny", "solmodul aktivert", "module solaire activé")

// telegram 0x035A
MAKE_PSTR_LIST(solarPumpMode, "solarpumpmode", "pump mode", "Solar Pumpen Einst.", "Modus zonneboilerpomp", "Sol Pumpläge", "tryb pracy pompy", "solpumpemodus", "mode pompe solaire")
MAKE_PSTR_LIST(solarPumpKick, "pumpkick", "pump kick", "Röhrenkollektorfunktion", "Modus buizencollector", "Sol Kollektorfunktion", "wspomaganie startu pompy", "solkllektorfunksjon", "démarrage boost pompe solaire")
MAKE_PSTR_LIST(plainWaterMode, "plainwatermode", "plain water mode", "Südeuropafunktion", "Modus Zuid-Europa", "Sydeuropa-funktion", "tylko woda (dla Europy Południowej)", "vanlig vannmodus", "mode eau ordinaire")
MAKE_PSTR_LIST(doubleMatchFlow, "doublematchflow", "doublematchflow", "Double Match Flow", "Double Match Flow", "Dubbelmatchning Flöde", "przepływ podwójnie dopasowany", "dobbelmatch flow", "double match flow")
MAKE_PSTR_LIST(solarPump2Mode, "pump2mode", "pump 2 mode", "Pumpe 2 Modus", "Modus pomp 2", "Pump 2 Läge", "tryb pracy pompy 2", "pump 2 modus", "mode pompe 2")
MAKE_PSTR_LIST(solarPump2Kick, "pump2kick", "pump kick 2", "Pumpe 2 Startboost", "Startboost pomp 2", "Pump 2 Kollektorfunktion", "wspomaganie startu pompy 2", "startboost pumpe 2", "démarrage boost pompe 2")

// telegram 0x035F
MAKE_PSTR_LIST(cylPriority, "cylpriority", "cylinder priority", "Speicher Priorität", "Prioriteit boiler", "Cylinderprioritering", "priorytet cylindra", "berederprioritering", "priorité de cylindre")

// telegram 0x380
MAKE_PSTR_LIST(climateZone, "climatezone", "climate zone", "Klimazone", "klimaatzone", "Klimatzon", "strefa klimatyczna", "klimasone", "zone de climat")
MAKE_PSTR_LIST(collector1Area, "collector1area", "collector 1 area", "Kollektor 1 Fläche", "oppervlakte collector 1", "Kollektor 1 Area", "powierzchnia kolektora 1", "kollektor 1 område", "zone collecteur 1")
MAKE_PSTR_LIST(collector1Type, "collector1type", "collector 1 type", "Kollektor 1 Typ", "Type collector 1", "Kollektor 1 Typ", "typ kolektora 1", "kollektor 1 type", "type collecteur 1")
MAKE_PSTR_LIST(collector2Area, "collector2area", "collector 2 area", "Kollektor 2 Fläche", "Oppervlakte collector 2", "Kollektor 2 Area", "powierzchnia kolektora 2", "kollektor 2 område", "zone collecteur 2")
MAKE_PSTR_LIST(collector2Type, "collector2type", "collector 2 type", "Kollektor 2 Typ", "Type collector 2", "Kollektor 2 Typ", "typ kolektora 2", "kollektor 2 type", "type collecteur 2")

// telegram 0x0363 heatCounter
MAKE_PSTR_LIST(heatCntFlowTemp, "heatcntflowtemp", "heat counter flow temperature", "Wärmezähler Vorlauf-Temperatur", "Aanvoertemperatuur warmteenergiemeter", "Värmeräknare Flödestemperatur", "temperatura zasilania ciepłomierza", "varmeenergimåler turtemperatur", "température flux compteur chaleur")
MAKE_PSTR_LIST(heatCntRetTemp, "heatcntrettemp", "heat counter return temperature", "Wärmezähler Rücklauf-Temperatur", "Retourtemperatuur warmteenergiemeter", "Värmeräknare Returtemperatur", "temperatura powrotu ciepłomierza", "varmeenergimåler returtemperatur", "température retour compteur chaleur")
MAKE_PSTR_LIST(heatCnt, "heatcnt", "heat counter impulses", "Wärmezähler Impulse", "Warmteenergiemeter pulsen", "Värmeräknare Impuls", "liczba impulsów ciepłomierza", "varmemåler impuls", "impulsions compteur chaleur")
MAKE_PSTR_LIST(swapFlowTemp, "swapflowtemp", "swap flow temperature (TS14)", "Austausch Vorlauf-Temperatur (TS14)", "Aanvoertemperatuur verwisselaar (TS14)", "Växlingstemperatur Flöde (TS14)", "temperatura zasilania wymiennika", "veksler turledningstemperatur (TS14)", "température flux échangeur (TS14)")
MAKE_PSTR_LIST(swapRetTemp, "swaprettemp", "swap return temperature (TS15)", "Austausch Rücklauf-Temperatur (TS15)", "Retourtemperatuur verwisselaar (TS15)", "Växlingstemperatur Returflöde (TS15)", "temperatura powrotu wymiennika", "veksler returledningstemperatur (TS15)", "température retour échangeur (TS15)")

// switch
MAKE_PSTR_LIST(activated, "activated", "activated", "Aktiviert", "Geactiveerd", "Aktiverad", "aktywowany", "aktivert", "activé")
MAKE_PSTR_LIST(status, "status", "status", "Status", "Status", "Status", "status", "status", "statut")

// RF sensor, id 0x40, telegram 0x435
MAKE_PSTR_LIST(RFTemp, "rftemp", "RF room temperature sensor", "RF Raumtemperatur Sensor", "RF ruimtetemperatuur sensor", "RF Rumsgivare Temp", "bezprzewodowy czujnik temperatury pomieszczenia", "RF romsgiver temp", "capteur de température de pièce RF")

/*
// unknown fields to track (SM10), only for testing
// **** NO TRANSLATION NEEDED ****
MAKE_PSTR_LIST(data11, "data11", "unknown datafield 11")
MAKE_PSTR_LIST(data12, "data12", "unknown datafield 12")
MAKE_PSTR_LIST(data8, "data8", "unknown datafield 8")
MAKE_PSTR_LIST(data0, "data0", "unknown datafield 0")
MAKE_PSTR_LIST(data1, "data1", "unknown datafield 1")
MAKE_PSTR_LIST(setting3, "setting3", "unknown setting 3")
MAKE_PSTR_LIST(setting4, "setting4", "unknown setting 4")
*/

// clang-format on