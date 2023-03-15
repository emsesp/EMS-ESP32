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
#pragma once

// clang-format off

// Define language codes here and add to languages[] in system.cpp
#define EMSESP_LOCALE_EN "en"
#define EMSESP_LOCALE_DE "de"
#define EMSESP_LOCALE_NL "nl"
#define EMSESP_LOCALE_SV "sv"
#define EMSESP_LOCALE_PL "pl"
#define EMSESP_LOCALE_NO "no"
#define EMSESP_LOCALE_FR "fr"
#define EMSESP_LOCALE_TR "tr"

// IMPORTANT!
//  translations are in the order: en, de, nl, sv, pl, no, fr, tr
//
// if there is no translation, it will default to en
//

// device types, as display in Web and Console
MAKE_WORD_TRANSLATION(boiler_device, "Boiler", "Kessel", "Boiler", "Värmepanna", "Kocioł", "", "", "Kazan")
MAKE_WORD_TRANSLATION(thermostat_device, "Thermostat", "Thermostat", "Thermostaat",  "Termostat", "Termostat", "", "", "Termostat")
MAKE_WORD_TRANSLATION(heatpump_device, "Heat Pump", "Wärmepumpe", "Warmtepomp",  "Värmepump", "Pompa ciepła", "", "", "Isı Pompası")
MAKE_WORD_TRANSLATION(solar_device, "Solar Module", "Solarmodul", "Solar Module",  "Solmodul", "Moduł solarny", "", "", "Güneş Enerjisi Cihazı")
MAKE_WORD_TRANSLATION(connect_device, "Connect Module", "Verbindungsmodul", "Connect Module",  "Uppkopplingsmodul", "Moduł przyłączeń", "", "", "Güneş Enerjisi Cihazı")
MAKE_WORD_TRANSLATION(mixer_device, "Mixer Module", "Mischermodul", "Mixer Module",  "Blandningsmodul", "Moduł mieszacza", "", "", "Karışım Cihazı")
MAKE_WORD_TRANSLATION(controller_device, "Controller Module", "Kontrollmodul", "Controller Module",  "Styrmodul", "Moduł sterujący", "", "", "Kontrol Ünitesi")
MAKE_WORD_TRANSLATION(switch_device, "Switch Module", "Schaltmodul", "Switch Module",  "Relämodul", "Moduł przełączający", "", "", "Anahtar")
MAKE_WORD_TRANSLATION(gateway_device, "Gateway Module", "Gateway Modul", "Gateway Module",  "Gateway", "Moduł IP", "", "", "Ağ Geçidi")
MAKE_WORD_TRANSLATION(alert_device, "Alert Module", "Alarmmodul", "Alert Module",  "Larmmodul", "Moduł alarmowy", "", "", "Alarm Cihazı")
MAKE_WORD_TRANSLATION(pump_device, "Pump Module", "Pumpenmodul", "Pump Module",  "Pumpmodul", "Moduł pompy", "", "", "Pompa")
MAKE_WORD_TRANSLATION(heatsource_device, "Heatsource", "Heizquelle", "Heatsource",  "Värmekälla", "Źródło ciepła", "", "", "Isı Kaynağı")
MAKE_WORD_TRANSLATION(sensors_device, "Sensors", "Sensoren", "Sensoren", "Sensorer", "Czujniki", "Sensorer", "Capteurs", "Sensör Cihazı")
MAKE_WORD_TRANSLATION(unknown_device, "Unknown", "Unbekannt", "Onbekend", "Okänt", "Nieznane urządzenie", "Ukjent", "Inconnu", "")

// commands
// TODO translate
MAKE_WORD_TRANSLATION(info_cmd, "lists all values", "Liste aller Werte", "", "", "wyświetl wszystkie wartości", "", "", "Tüm değerleri listele")
MAKE_WORD_TRANSLATION(commands_cmd, "lists all commands", "Liste aller Kommandos", "", "", "wyświetl wszystkie komendy", "", "", "Tüm komutları listele")
MAKE_WORD_TRANSLATION(entities_cmd, "lists all entities", "Liste aller Entitäten", "", "", "wyświetl wszsytkie encje", "", "", "Tüm varlıkları listele")
MAKE_WORD_TRANSLATION(send_cmd, "send a telegram", "Sende EMS-Telegramm", "", "", "wyślij telegram", "", "", "Bir telegram gönder")
MAKE_WORD_TRANSLATION(setiovalue_cmd, "set io value", "Setze Wertevorgabe", "", "", "ustaw wartość", "", "", "Giriş/Çıkış değerlerini ayarla")
MAKE_WORD_TRANSLATION(changeloglevel_cmd, "change log level", "Ändere Sysloglevel", "", "", "zmień poziom log-u", "", "", "Kayıt seviyesini değiştir")
MAKE_WORD_TRANSLATION(fetch_cmd, "refresh all EMS values", "Lese alle EMS-Werte neu", "", "", "odśwież wszystkie wartości EMS", "", "", "Bütün EMS değerlerini yenile")
MAKE_WORD_TRANSLATION(restart_cmd, "restart EMS-ESP", "Neustart", "", "", "uruchom ponownie EMS-ESP", "", "", "EMS-ESPyi yeniden başlat")
MAKE_WORD_TRANSLATION(watch_cmd, "watch incoming telegrams", "Watch auf eingehende Telegramme", "", "", "obserwuj przyczodzące telegramy", "", "", "Gelen telegramları ")
MAKE_WORD_TRANSLATION(publish_cmd, "publish all to MQTT", "Publiziere MQTT", "", "", "opublikuj wszystko na MQTT", "", "", "Hepsini MQTTye gönder")
MAKE_WORD_TRANSLATION(system_info_cmd, "show system status", "Zeige System-Status", "", "", "pokaż status systemu", "", "", "Sistem Durumunu Göster")
MAKE_WORD_TRANSLATION(schedule_cmd, "enable schedule item", "Aktiviere Zeitplan", "", "", "aktywuj wybrany harmonogram", "", "", "")

// tags
MAKE_WORD_TRANSLATION(tag_boiler_data_ww, "dhw", "WW", "dhw", "VV", "CWU", "dhw", "ecs", "SKS")
MAKE_WORD_TRANSLATION(tag_device_data_ww, "dhw", "WW", "dhw", "VV", "CWU", "dhw", "ecs", "SKS")
MAKE_WORD_TRANSLATION(tag_hc1, "hc1", "HK1", "hc1", "VK1", "OG1", "hc1", "hc1", "ID1")
MAKE_WORD_TRANSLATION(tag_hc2, "hc2", "HK2", "hc2", "VK2", "OG2", "hc2", "hc2", "ID2")
MAKE_WORD_TRANSLATION(tag_hc3, "hc3", "HK3", "hc3", "VK3", "OG3", "hc3", "hc3", "ID3")
MAKE_WORD_TRANSLATION(tag_hc4, "hc4", "HK4", "hc4", "VK4", "OG4", "hc4", "hc4", "ID4")
MAKE_WORD_TRANSLATION(tag_hc5, "hc5", "HK5", "hc5", "VK5", "OG5", "hc5", "hc5", "ID5")
MAKE_WORD_TRANSLATION(tag_hc6, "hc6", "HK6", "hc6", "vk6", "OG6", "hc6", "hc6", "ID6")
MAKE_WORD_TRANSLATION(tag_hc7, "hc7", "HK7", "hc7", "VK7", "OG7", "hc7", "hc7", "ID7")
MAKE_WORD_TRANSLATION(tag_hc8, "hc8", "HK8", "hc8", "VK8", "OG8", "hc8", "hc8", "ID8")
MAKE_WORD_TRANSLATION(tag_wwc1, "wwc1", "WWK1", "wwc1", "VVK1", "CWU1", "wwc1", "wwc1", "SKS1")
MAKE_WORD_TRANSLATION(tag_wwc2, "wwc2", "WWK2", "wwc2", "VVK2", "CWU2", "wwc2", "wwc2", "SKS2")
MAKE_WORD_TRANSLATION(tag_wwc3, "wwc3", "WWK3", "wwc3", "VVK3", "CWU3", "wwc3", "wwc3", "SKS3")
MAKE_WORD_TRANSLATION(tag_wwc4, "wwc4", "WWK4", "wwc4", "VVK4", "CWU4", "wwc4", "wwc4", "SKS4")
MAKE_WORD_TRANSLATION(tag_wwc5, "wwc5", "WWK5", "wwc5", "VVK5", "CWU5", "wwc5", "wwc5", "SKS5")
MAKE_WORD_TRANSLATION(tag_wwc6, "wwc6", "WWK6", "wwc6", "VVK6", "CWU6", "wwc6", "wwc6", "SKS6")
MAKE_WORD_TRANSLATION(tag_wwc7, "wwc7", "WWK7", "wwc7", "VVK7", "CWU7", "wwc7", "wwc7", "SKS7")
MAKE_WORD_TRANSLATION(tag_wwc8, "wwc8", "WWK8", "wwc8", "VVK8", "CWU8", "wwc8", "wwc8", "SKS8")
MAKE_WORD_TRANSLATION(tag_wwc9, "wwc9", "WWK9", "wwc9", "VVK9", "CWU9", "wwc9", "wwc9", "SKS9")
MAKE_WORD_TRANSLATION(tag_wwc10, "wwc10", "WWK10", "wwc10", "VVK10", "CWU10", "wwc10", "wwc10", "SKS10")
MAKE_WORD_TRANSLATION(tag_ahs1, "ahs1", "AHQ1", "ahs1", "AVK1", "AŹC1", "ahs1", "ahs1", "ahs1")
MAKE_WORD_TRANSLATION(tag_hs1, "hs1", "hs1", "hs1", "VK1", "ŹC1", "hs1", "hs1", "hs1")
MAKE_WORD_TRANSLATION(tag_hs2, "hs2", "hs2", "hs2", "VK2", "ŹC2", "hs2", "hs2", "hs2")
MAKE_WORD_TRANSLATION(tag_hs3, "hs3", "hs3", "hs3", "VK3", "ŹC3", "hs3", "hs3", "hs3")
MAKE_WORD_TRANSLATION(tag_hs4, "hs4", "hs4", "hs4", "VK4", "ŹC4", "hs4", "hs4", "hs4")
MAKE_WORD_TRANSLATION(tag_hs5, "hs5", "hs5", "hs5", "VK5", "ŹC5", "hs5", "hs5", "hs5")
MAKE_WORD_TRANSLATION(tag_hs6, "hs6", "hs6", "hs6", "VK6", "ŹC6", "hs6", "hs6", "hs6")
MAKE_WORD_TRANSLATION(tag_hs7, "hs7", "hs7", "hs7", "VK7", "ŹC7", "hs7", "hs7", "hs7")
MAKE_WORD_TRANSLATION(tag_hs8, "hs8", "hs8", "hs8", "VK8", "ŹC8", "hs8", "hs8", "hs8")
MAKE_WORD_TRANSLATION(tag_hs9, "hs9", "hs9", "hs9", "VK9", "ŹC9", "hs9", "hs9", "hs9")
MAKE_WORD_TRANSLATION(tag_hs10, "hs10", "hs10", "hs10", "VK10", "ŹC10", "hs10", "hs10", "hs10")
MAKE_WORD_TRANSLATION(tag_hs11, "hs11", "hs11", "hs11", "VK11", "ŹC11", "hs11", "hs11", "hs11")
MAKE_WORD_TRANSLATION(tag_hs12, "hs12", "hs12", "hs12", "VK12", "ŹC12", "hs12", "hs12", "hs12")
MAKE_WORD_TRANSLATION(tag_hs13, "hs13", "hs13", "hs13", "VK13", "ŹC13", "hs13", "hs13", "hs13")
MAKE_WORD_TRANSLATION(tag_hs14, "hs14", "hs14", "hs14", "VK14", "ŹC14", "hs14", "hs14", "hs14")
MAKE_WORD_TRANSLATION(tag_hs15, "hs15", "hs15", "hs15", "VK15", "ŹC15", "hs15", "hs15", "hs15")
MAKE_WORD_TRANSLATION(tag_hs16, "hs16", "hs16", "hs16", "VK16", "ŹC16", "hs16", "hs16", "hs16")


// General
MAKE_WORD_TRANSLATION(on, "on", "an", "aan", "på", "włączono", "på", "on", "açık")
MAKE_WORD_TRANSLATION(off, "off", "aus", "uit", "av", "wyłączono", "av", "off", "kapalı")
MAKE_WORD_TRANSLATION(ON, "ON", "AN", "AAN", "PÅ", "wł.", "PÅ", "ON", "AÇIK")
MAKE_WORD_TRANSLATION(OFF, "OFF", "AUS", "UIT", "AV", "wył.", "AV", "OFF", "KAPALI")

// Unit Of Measurement mapping - maps to DeviceValueUOM_s in emsdevice.cpp
// uom - also used with HA see https://github.com/home-assistant/core/blob/d7ac4bd65379e11461c7ce0893d3533d8d8b8cbf/homeassistant/const.py#L384
MAKE_WORD_TRANSLATION(minutes, "minutes", "Minuten", "Minuten", "Minuter", "minut", "Minutter", "minutes", "dakika")
MAKE_WORD_TRANSLATION(hours, "hours", "Stunden", "Uren", "Timmar", "godzin", "Timer", "heures", "saat")
MAKE_WORD_TRANSLATION(days, "days", "Tage", "Dagen", "Dagar", "dni", "Dager", "jours", "gün")
MAKE_WORD_TRANSLATION(seconds, "seconds", "Sekunden", "Seconden", "Sekunder", "sekund", "Sekunder", "secondes", "saniye")

// Enum translations
// general
MAKE_WORD_TRANSLATION(day_mo, "mo", "Mo", "Mo", "Må", "poniedziałek", "Ma", "lun", "pzt")
MAKE_WORD_TRANSLATION(day_tu, "tu", "Di", "Di", "Ti", "wtorek", "Ti", "mar", "sal")
MAKE_WORD_TRANSLATION(day_we, "we", "Mi", "Wo", "On", "środa", "On", "mer", "çar")
MAKE_WORD_TRANSLATION(day_th, "th", "Do", "Do", "To", "czwartek", "To", "jeu", "per")
MAKE_WORD_TRANSLATION(day_fr, "fr", "Fr", "Vr", "Fr", "piątek", "Fr", "ven", "cum")
MAKE_WORD_TRANSLATION(day_sa, "sa", "Sa", "Za", "Lö", "sobota", "Lø", "sam", "cts")
MAKE_WORD_TRANSLATION(day_su, "su", "So", "Zo", "Sö", "niedziela", "Sø", "dim", "paz")
MAKE_WORD_TRANSLATION(all, "all", "Alle", "Alle", "Alla", "codziennie", "alle", "tous", "tüm")
MAKE_WORD_TRANSLATION(own_1, "own 1", "Eigen 1", "Eigen 1", "Egen 1", "własny 1", "Egen 1", "propre 1", "kendi 1")
MAKE_WORD_TRANSLATION(family, "family", "Familie", "Familie", "Familj", "rodzina", "familie", "famille", "aile")
MAKE_WORD_TRANSLATION(morning, "morning", "Morgends", "'s ochtends", "Morgon", "zmiana 1", "morgen", "matin", "sabah")
MAKE_WORD_TRANSLATION(evening, "evening", "Abends", "'s avonds", "Kväll", "zmiana 2", "kveld", "soir", "akşam")
MAKE_WORD_TRANSLATION(seniors, "seniors", "Senioren", "Senioren", "Seniorer", "senior", "seniorer", "séniors", "yaşlılar")
MAKE_WORD_TRANSLATION(no, "no", "nein", "nee", "nej", "nie", "nei", "non", "hayır")
MAKE_WORD_TRANSLATION(new, "new", "Neu", "Nieuw", "Ny", "nowy", "ny", "nouveau", "yeni")
MAKE_WORD_TRANSLATION(own_2, "own 2", "Eigen 2", "Eigen 2", "Egen 2", "własny 2", "egen 2", "propre 2", "kendi 2")
MAKE_WORD_TRANSLATION(singles, "singles", "Singles", "Singles", "Singlar", "osoba samotna", "single", "seuls", "")
MAKE_WORD_TRANSLATION(am, "am", "Vormittag", "Ochtend", "Förmiddag", "do południa", "formiddag", "matin", "sabah")
MAKE_WORD_TRANSLATION(pm, "pm", "Nachmittag", "Namiddag", "Eftermiddag", "po południu", "ettermiddag", "après-midi", "akşam")
MAKE_WORD_TRANSLATION(midday, "midday", "Mittag", "Middag", "Middag", "południe", "middag", "midi", "öğlen")
MAKE_WORD_TRANSLATION(unknown, "unknown", "Unbekannt", "Onbekend", "Okänt", "nieznany", "ukjent", "inconnu", "bilinmeyen")
MAKE_WORD_TRANSLATION(flat, "flat", "flach", "vlak", "Platt", "płaski", "flat", "plat", "düz")
MAKE_WORD_TRANSLATION(vacuum, "vacuum", "Vakuum", "vacuum", "Vakuum", "próżnia", "vakum", "vide", "vakum")
MAKE_WORD_TRANSLATION(co2_optimized, "co2 optimized", "CO2 optimiert", "CO2 geoptimaliseerd", "CO2-optimerad", "optymalizacja CO2", "co2 optimalisert", "optimisé en CO2", "CO2 verimli")
MAKE_WORD_TRANSLATION(cost_optimized, "cost optimized", "kostenoptimiert", "kosten geoptimaliseerd", "kostnadsoptimerad", "optymalizacja kosztów", "kostnadsoptimalisert", "optimisé en coût", "maliyet odaklı")
MAKE_WORD_TRANSLATION(outside_temp_switched, "outside temp switched", "Außentemp. gesteuert", "Buitentemp. gestuurd", "Utomhustemp korrigerad", "temperatura zewn. przeł.", "utetemp optimalisert", "contrôle par temp. ext.", "dış hava sıcaklığına bağlı")
MAKE_WORD_TRANSLATION(co2_cost_mix, "co2 cost mix", "Kostenmix", "Kostenmix", "Kostnadsmix", "mieszany koszt CO2", "", "coût mixte CO2", "karışık maliyet") // TODO translate
MAKE_WORD_TRANSLATION(analog, "analog", "analog", "analoog", "analog", "analogowy", "analog", "analogique", "analog")
MAKE_WORD_TRANSLATION(normal, "normal", "normal", "normaal", "normal", "normalny", "normal", "normal", "normal")
MAKE_WORD_TRANSLATION(blocking, "blocking", "Blockierung", "Blokkering", "Blockering", "blokowanie", "blokkering", "bloquant", "engelleme")
MAKE_WORD_TRANSLATION(extern, "extern", "extern", "extern", "extern", "zewnętrzny", "ekstern", "externe", "dış")
MAKE_WORD_TRANSLATION(intern, "intern", "intern", "intern", "intern", "wewnętrzny", "intern", "interne", "iç")
MAKE_WORD_TRANSLATION(lower, "lower", "niedirger", "lager", "lägre", "mniejszy", "nedre", "inférieur", "daha düşük")
MAKE_WORD_TRANSLATION(error, "error", "Fehler", "error", "Fel", "błąd", "", "erreur", "Hata") // TODO translate
MAKE_WORD_TRANSLATION(na, "n/a", "n/a", "n/a", "n/a", "nd.", "", "n/c", "mevcut değil") // TODO translate

// boiler
MAKE_WORD_TRANSLATION(time, "time", "Zeit", "Tijd", "Tid", "godzina", "tid", "heure", "zaman")
MAKE_WORD_TRANSLATION(date, "date", "Datum", "Datum", "Datum", "data", "dato", "date", "tarih")
MAKE_WORD_TRANSLATION(continuous, "continuous", "kontinuierlich", "continue", "kontinuerlig", "ciągły", "kontinuerlig", "continu", "devam eden")
MAKE_WORD_TRANSLATION(3wayvalve, "3-way valve", "3-Wege Ventil", "3-weg klep", "trevägsventil", "zawór 3-drogowy", "treveisventil", "vanne 3 voies" ,"3 yollu vana")
MAKE_WORD_TRANSLATION(chargepump, "chargepump", "Ladepumpe", "laadpomp", "laddpump", "pompa ładująca", "ladepumpe", "pompe de charge", "besleme pompası")
MAKE_WORD_TRANSLATION(hot, "hot", "Heiß", "Heet", "Het", "gorący", "het", "chaud", "sıcak")
MAKE_WORD_TRANSLATION(high_comfort, "high comfort", "gehobener Komfort", "Verhoogd comfort", "Förhöjd komfort", "wysoki komfort", "høy komfort", "comfort", "komfor")
MAKE_WORD_TRANSLATION(eco, "eco", "Eco", "Eco", "Eko", "eko", "øko", "éco", "eko")
MAKE_WORD_TRANSLATION(intelligent, "intelligent", "Intelligent", "Intelligent", "Intelligent", "inteligentny", "intelligent", "intelligent", "akıllı")
MAKE_WORD_TRANSLATION(flow, "flow", "Durchfluss", "Volumestroom", "Flöde", "przepływ", "strømme", "débit", "akım")
MAKE_WORD_TRANSLATION(manual, "manual", "Manuell", "Hamdmatig", "Manuell", "ręczny", "manuell", "manuel", "manuel")
MAKE_WORD_TRANSLATION(buffer, "buffer", "Speicher", "Buffer", "Buffert", "bufor", "buffer", "buffer", "tampon")
MAKE_WORD_TRANSLATION(bufferedflow, "buffered flow", "Durchlaufspeicher", "Doorstroombuffer", "Buffertflöde", "przepływ buforowany", "bufret strømning", "", "tampon akım") // TODO translate
MAKE_WORD_TRANSLATION(layeredbuffer, "layered buffer", "Schichtspeicher", "Gelaagde buffer", "Lagrad buffert", "bufor warstwowy", "lagdelt buffer", "", "katmanlı akım") // TODO translate
MAKE_WORD_TRANSLATION(maintenance, "maintenance", "Wartung", "Onderhoud", "Underhåll", "przegląd", "vedlikehold", "maintenance", "bakım")
MAKE_WORD_TRANSLATION(heating, "heating", "Heizen", "Verwarmen", "Uppvärmning", "ogrzewanie", "oppvarming", "chauffage", "ısıtma")
MAKE_WORD_TRANSLATION(cooling, "cooling", "Kühlen", "Koelen", "Kyler", "chłodzenie", "kjøling", "refroidissement", "soğuma")
MAKE_WORD_TRANSLATION(disinfecting, "disinfecting", "Desinfizieren", "Desinfecteren", "Desinficerar", "dezynfekcja termiczna", "desinfisering", "désinfection", "dezenfeksiyon")
MAKE_WORD_TRANSLATION(no_heat, "no heat", "keine Wärme", "Geen warmte", "Ingen värme", "brak ciepła", "ingen varme", "pas de chauffage", "ısınma yok")
MAKE_WORD_TRANSLATION(heatrequest, "heat request", "Wärmeanforderung", "Verwarmignsverzoek", "Värmeförfrågan", "zapotrzebowanie na ciepło", "varmeforespørsel", "demande de chauffage", "ısınma ihtiyacı")
MAKE_WORD_TRANSLATION(valve, "valve", "Ventil", "Klep", "Ventil", "zawór", "ventil", "valve", "vana")
MAKE_WORD_TRANSLATION(proportional, "proportional", "", "", "", "", "", "", "") // TODO translate
MAKE_WORD_TRANSLATION(deltaP1, "deltaP-1", "", "", "", "", "", "", "") // TODO translate
MAKE_WORD_TRANSLATION(deltaP2, "deltaP-2", "", "", "", "", "", "", "") // TODO translate
MAKE_WORD_TRANSLATION(deltaP3, "deltaP-3", "", "", "", "", "", "", "") // TODO translate
MAKE_WORD_TRANSLATION(deltaP4, "deltaP-4", "", "", "", "", "", "", "") // TODO translate

// heatpump
MAKE_WORD_TRANSLATION(none, "none", "keine", "geen", "ingen", "brak", "ingen", "aucun", "hiçbiri")
MAKE_WORD_TRANSLATION(hot_water, "hot water", "Warmwasser", "warm water", "varmvatten", "c.w.u.", "varmtvann", "eau chaude", "sıcak su")
MAKE_WORD_TRANSLATION(pool, "pool", "Pool", "zwembad", "pool", "basen", "basseng", "piscine", "havuz")
MAKE_WORD_TRANSLATION(outside_temp_alt, "outside temperature alt.", "Außentemp. alternativ", "", "Alternativ utomhustemp.", "temp. zewn. alternat.", "", "température extérieure alternative", "alternatif dış sıcaklık") // TODO translate
MAKE_WORD_TRANSLATION(outside_temp_par, "outside temperature parallel", "Außentemp. parallel", "", "Parallell utomhustemp.", "temp. zewn. równoległa", "", "température extérieure parallèle", "paralel dış sıcaklık") // TODO translate
MAKE_WORD_TRANSLATION(hp_prefered, "heatpump prefered", "Wärmepumpe bevorzugt", "", "Värmepump föredraget", "preferowana pompa ciepła", "", "pompe à chaleur préférée", "tercih edilen pompa") // TODO translate
MAKE_WORD_TRANSLATION(boiler_only, "boiler only", "nur Kessel", "", "Värmepanna enbart", "tylko kocioł", "", "chaudière uniquement", "sadece kazan") // TODO translate
MAKE_WORD_TRANSLATION(reduced_output, "reduced output", "Reduzierte Leistung", "", "Reducerad produktion", "zmniejszona wydajność", "", "sortie réduite", "düşürülmüş çıkış") // TODO translate
MAKE_WORD_TRANSLATION(switchoff, "switch off hp", "WP ausschalten", "", "Värmepump avstängd", "wyłącz pompę ciepła", "", "éteindre la PAC", "ısı pompasını kapat") // TODO translate
MAKE_WORD_TRANSLATION(perm, "perm. reduced", "perm. reduziert", "", "Permanent reducerad", "stale zmniejszona wydajność", "", "réduction permanente", "sürekli azaltılmış") // TODO translate

// thermostat
MAKE_WORD_TRANSLATION(seltemp, "selTemp", "Solltemperatur", "Doeltemperatuur", "Börtemperatur", "temperatura zadana", "innstilt temperatur", "consigne température", "ayarlanmış sıcaklık")
MAKE_WORD_TRANSLATION(roomtemp, "roomTemp", "Raumtemperatur", "Kamertemperatuur", "Rumstemperatur", "temperatura w pomieszczeniu", "romstemperatur", "température de la pièce", "oda sıcaklığı")
MAKE_WORD_TRANSLATION(own_prog, "own prog", "Eigenprog.", "Eigen prog.", "Egen prog.", "program własny", "eget prog.", "programme propre", "isteğe göre ayarlanmış program")
MAKE_WORD_TRANSLATION(std_prog, "std prog", "Standardprog.", "Standaard prog.", "Standardprog.", "program standardowy", "standardprog.", "programme standard", "sandart pogram")
MAKE_WORD_TRANSLATION(light, "light", "Leicht", "Licht", "Lätt", "lekki", "lett", "léger", "düşük")
MAKE_WORD_TRANSLATION(medium, "medium", "Mittel", "Middel", "Medel", "średni", "medium", "medium", "orta")
MAKE_WORD_TRANSLATION(heavy, "heavy", "Schwer", "Zwaar", "Tung", "ciężki", "tung", "lourd", "yüksek")
MAKE_WORD_TRANSLATION(start, "start", "Start", "Start", "Start", "start", "start", "début", "başlat")
MAKE_WORD_TRANSLATION(heat, "heat", "Heizen", "Verwarmen", "Värme", "ciepło", "varmer", "chaleur", "ısıtma")
MAKE_WORD_TRANSLATION(hold, "hold", "Halten", "Pauzeren", "Paus", "pauza", "pause", "pause", "durdur")
MAKE_WORD_TRANSLATION(cool, "cool", "Kühlen", "Koelen", "Kyla", "zimno", "kjøler", "froid", "soğutma")
MAKE_WORD_TRANSLATION(end, "end", "Ende", "Einde", "Slut", "koniec", "slutt", "fin", "bitti")
MAKE_WORD_TRANSLATION(german, "german", "Deutsch", "Duits", "Tyska", "niemiecki", "tysk", "allemand", "Almanca")
MAKE_WORD_TRANSLATION(dutch, "dutch", "Niederländisch", "Nederlands", "Nederländska", "niderlandzki", "nederlandsk", "néerlandais", "Flemenkçe")
MAKE_WORD_TRANSLATION(french, "french", "Französisch", "Frans", "Franska", "francuski", "fransk", "français", "Fransızca")
MAKE_WORD_TRANSLATION(italian, "italian", "Italienisch", "Italiaans", "Italienska", "włoski", "italiensk", "italien", "İtalyanca")

MAKE_WORD_TRANSLATION(high, "high", "hoch", "hoog", "Hög", "wysoki", "", "haut") // TODO translate
MAKE_WORD_TRANSLATION(low, "low", "niedrig", "laag", "Låg", "niski", "lav", "bas", "düşük")
MAKE_WORD_TRANSLATION(radiator, "radiator", "Heizkörper", "Radiator", "Radiator", "grzejniki", "radiator", "radiateur", "radyatör")
MAKE_WORD_TRANSLATION(convector, "convector", "Konvektor", "Convector", "Konvektor", "konwektory", "konvektor", "convecteur", "convector")
MAKE_WORD_TRANSLATION(floor, "floor", "Fussboden", "Vloer", "Golv", "podłoga", "gulv", "sol", "yer")
MAKE_WORD_TRANSLATION(summer, "summer", "Sommer", "Zomer", "Sommar", "lato", "sommer", "été", "yaz")
MAKE_WORD_TRANSLATION(winter, "winter", "Winter", "Winter", "Vinter", "zima", "vinter", "hiver", "kış")
MAKE_WORD_TRANSLATION(outdoor, "outdoor", "Außen", "Buiten", "Utomhus", "temp. zewnętrzna", "utendørs", "extérieur", "dış")
MAKE_WORD_TRANSLATION(room, "room", "Raum", "Kamer", "Rum", "temp. w pomieszczeniu", "", "pièce", "oda") // TODO translate
MAKE_WORD_TRANSLATION(room_outdoor, "room outdoor", "Raum+Außen", "Kamer+Buiten", "Rum+Ute", "temp. w pom. i zewn.", "rom utendørs", "pièce extérieure", "oda ve dış")
MAKE_WORD_TRANSLATION(power, "power", "Leistung", "Vermogen", "Effekt", "moc", "effekt", "puissance", "güç")
MAKE_WORD_TRANSLATION(constant, "constant", "konstant", "constant", "Konstant", "stały", "konstant", "constant", "sabit")
MAKE_WORD_TRANSLATION(simple, "simple", "einfach", "simpel", "enkel", "prosty", "enkel", "simple", "basit")
MAKE_WORD_TRANSLATION(optimized, "optimized", "optimiert", "geoptimaliseerd", "optimerad", "zoptymalizowany", "optimalisert", "optimisé", "optimize")
MAKE_WORD_TRANSLATION(nofrost, "nofrost", "Frostschutz", "Vorstbescherming", "Frostskydd", "ochrona przed zamarzaniem", "frostsikring", "protection gel", "Donma koruması")
MAKE_WORD_TRANSLATION(defrost, "defrost", "Abtauen", "ontdooien", "avfrostning", "rozmrażać", "tine", "dégivrage", "buz çözücü")
MAKE_WORD_TRANSLATION(comfort, "comfort", "Komfort", "Comfort", "Komfort", "komfort", "komfort", "comfort", "konfor")
MAKE_WORD_TRANSLATION(night, "night", "Nacht", "Nacht", "Natt", "noc", "natt", "nuit", "gece")
MAKE_WORD_TRANSLATION(day, "day", "Tag", "Dag", "Dag", "dzień", "dag", "jour", "gün")
MAKE_WORD_TRANSLATION(holiday, "holiday", "Urlaub", "Vakantie", "Helgdag", "urlop?", "ferie", "vacances", "tatil")
MAKE_WORD_TRANSLATION(reduce, "reduce", "reduziert", "gereduceerd", "Reducera", "zredukowany", "redusere", "réduit", "düşür")
MAKE_WORD_TRANSLATION(noreduce, "no reduce", "unreduziert", "niet gerduceerd", "oreducerad", "bez redukcji", "ingen reduksjon", "pas de réduction", "düşürme")
MAKE_WORD_TRANSLATION(offset, "offset", "Anhebung", "offset", "Förskutning", "przesunięcie", "kompensasjon", "offset", "kompansasyon")
MAKE_WORD_TRANSLATION(design, "design", "Auslegung", "Ontwero", "Design", "projekt", "design", "design", "tasarım")
MAKE_WORD_TRANSLATION(minflow, "min flow", "min. Durchfluss", "Min. Doorstroom", "Min flöde", "minimalny przepływ", "min strømming", "flux min", "minimum akış")
MAKE_WORD_TRANSLATION(maxflow, "max flow", "max. Durchfluss", "Max. Doorstroom", "Max flöde", "maksymalny przepływ", "maks strømming", "flux max", "maksimum akış")
MAKE_WORD_TRANSLATION(fast, "fast", "schnell", "snel", "snabb", "szybkie", "hurtig", "rapide", "hızlı")
MAKE_WORD_TRANSLATION(slow, "slow", "langsam", "langzaam", "långsam", "powolne", "langsom", "lent", "yavaş")
MAKE_WORD_TRANSLATION(internal_temperature, "internal temperature", "Interne Temperatur", "Interne Temperatuur", "Interntemperatur", "temperatura wewnętrzna", "interntemperatur", "température interne", "oda sıcaklığı")
MAKE_WORD_TRANSLATION(internal_setpoint, "internal setpoint", "Interner Sollwert", "Interne Streeftemperatuur", "Internt börvärde", "nastawa wewnętrzna", "internt settpunkt", "consigne interne", "istenen oda sıcaklığı")
MAKE_WORD_TRANSLATION(external_temperature, "external temperature", "Externe Temperatur", "Externe Temperatuur", "Extern temperatur", "temperatura zewnętrzna", "ekstern temperatur", "température externe")
MAKE_WORD_TRANSLATION(burner_temperature, "burner temperature", "Brennertemperatur", "Brander Temperuur", "Brännartemperatur", "temperatura palnika", "brennertemperatur", "température du brûleur", "kazan sıcaklığı")
MAKE_WORD_TRANSLATION(ww_temperature, "ww temperature", "Wassertemperatur", "Watertemperatuur", "Vattentemperatur", "temperatura c.w.u.", "vanntemperatur", "température de l'eau", "Kullanım suyu sıcaklığı")
MAKE_WORD_TRANSLATION(smoke_temperature, "smoke temperature", "Abgastemperatur", "Buitentemperatuur", "Rökgastemperatur", "temperatura dymu", "røykgasstemperatur", "température des gaz d'échappement", "baca gazı sıcaklığı")
MAKE_WORD_TRANSLATION(weather_compensated, "weather compensated", "Wetter kompensiert", "Weer gecompenseerd", "Väderkompenserad", "skompensow. pogodą", "værkompensert", "compensation par l'extérieur", "hava durumuna göre dengelenmiş")
MAKE_WORD_TRANSLATION(outside_basepoint, "outside basepoint", "Basispunkt Außentemp.", "Buiten basispunt", "Utomhus baspunkt", "temp. zewn. z pkt. pocz.", "utendørs basispunkt", "point de base temp. ext.", "dış hava sıcaklığı taban noktası")
MAKE_WORD_TRANSLATION(functioning_mode, "functioning mode", "Funktionsweise", "Functiemodus", "Driftläge", "tryb pracy", "driftsmodus", "mode de fonctionnement", "işletme konumu")

// MQTT Discovery - this is special device entity for 'climate'
MAKE_WORD_TRANSLATION(haclimate, "haclimate", "Discovery current room temperature", "Discovery Temperatur") // TODO translate

// mixer
MAKE_WORD_TRANSLATION(stopped, "stopped", "gestoppt", "gestopt", "stoppad", "zatrzymany", "stoppet", "arrêté", "durdu")
MAKE_WORD_TRANSLATION(opening, "opening", "öffnen", "openen", "öppnar", "otwieranie", "åpner", "ouverture", "açılıyor")
MAKE_WORD_TRANSLATION(closing, "closing", "schließen", "sluiten", "stänger", "zamykanie", "stenger", "fermeture", "kapanıyor")
MAKE_WORD_TRANSLATION(open, "open", "offen", "Open", "Öppen", "otwórz", "åpen", "ouvert", "açık")
MAKE_WORD_TRANSLATION(close, "close", "geschlossen", "Gesloten", "Stängd", "zamknij", "stengt", "fermé", "kapalı")

// solar ww
MAKE_WORD_TRANSLATION(cyl1, "cyl 1", "Zyl_1", "Cil 1", "Cyl 1", "cyl 1", "cyl 1", "cyl 1", "cly 1")
MAKE_WORD_TRANSLATION(cyl2, "cyl 2", "Zyl_2", "Cil 2", "Cyl 2", "cyl 2", "cyl 2", "cyl 2", "cly 1")

// Entity translations
// Boiler
MAKE_TRANSLATION(wwtapactivated, "wwtapactivated", "turn on/off", "Durchlauferhitzer aktiv", "zet aan/uit", "på/av", "system przygotowywania", "slå på/av", "ecs activée")
MAKE_TRANSLATION(reset, "reset", "Reset", "Reset", "Reset", "Nollställ", "kasowanie komunikatu", "nullstill", "reset")
MAKE_TRANSLATION(oilPreHeat, "oilpreheat", "oil preheating", "Ölvorwärmung", "Olie voorverwarming", "Förvärmning olja", "podgrzewanie oleju", "oljeforvarming", "préchauffage de l'huile")
MAKE_TRANSLATION(heatingActive, "heatingactive", "heating active", "Heizen aktiv", "Verwarming actief", "Uppvärmning aktiv", "c.o. aktywne", "oppvarming aktiv", "chauffage actif")
MAKE_TRANSLATION(tapwaterActive, "tapwateractive", "tapwater active", "Warmwasser aktiv", "Warm water actief", "Varmvatten aktiv", "c.w.u. aktywne", "varmtvann aktiv", "eau chaude active")
MAKE_TRANSLATION(selFlowTemp, "selflowtemp", "selected flow temperature", "Sollwert Vorlauftemperatur", "Ingestelde aanvoertemperatuur", "Börvärde Flödestemperatur", "wybrana temperatura zasilania", "valgt turtemperatur", "température de flux selectionnée")
MAKE_TRANSLATION(selBurnPow, "selburnpow", "burner selected max power", "Sollwert Brennerleistung", "Ingestelde maximale brandervermogen", "Brännare vald maxeffekt", "wybrana moc źródła ciepła", "settpunkt brennerkapasitet", "puissance max du brûleur selectionnée")
MAKE_TRANSLATION(absBurnPow, "absburnpow", "burner current power (absolute)", "Brennerleistung (absolut)", "Brandervermogen (abs)", "Värmepanna aktuell effekt (abs)", "aktualna moc źródła ciepła (absolutna)", "brennereffekt", "puissance du brûleur actuelle (abs)")
MAKE_TRANSLATION(heatingPumpMod, "heatingpumpmod", "heating pump modulation", "Heizungspumpe 1 Modulation", "Modulatie verwarmingspomp", "Modulering Värmepump", "wysterowanie pompy c.o.", "varmepumpemodulering", "modulation de la pompe à chaleur")
MAKE_TRANSLATION(outdoorTemp, "outdoortemp", "outside temperature", "Aussentemperatur", "Buitentemperatuur", "Utomhustemperatur", "temperatura zewnętrzna", "utetemperatur", "température extérieure")
MAKE_TRANSLATION(curFlowTemp, "curflowtemp", "current flow temperature", "aktuelle Vorlauftemperatur", "Huidige aanvoertemperatuur", "Flödestemperatur", "temperatura zasilania", "aktuell strømmetemperatur", "température actuelle du flux")
MAKE_TRANSLATION(retTemp, "rettemp", "return temperature", "Rücklauftemperatur", "Retourtemperatuur", "Returtemperatur", "temperatura powrotu", "returtemperatur", "température de retour")
MAKE_TRANSLATION(switchTemp, "switchtemp", "mixing switch temperature", "Mischer Schalttemperatur", "Mixer temperatuur", "Blandartemperatur", "temperatura przełączania mieszacza", "Blandertemperatur", "température de bascule du mélangeur")
MAKE_TRANSLATION(sysPress, "syspress", "system pressure", "Systemdruck", "Systeemdruk", "Systemtryck", "ciśnienie w systemie", "systemtrykk", "pression du système")
MAKE_TRANSLATION(boilTemp, "boiltemp", "actual boiler temperature", "Kesseltemperatur", "Keteltemperatuur", "Temperatur Värmepanna", "temperatura zasobnika", "Temperatur Värmepanna", "température de la chaudière")
MAKE_TRANSLATION(exhaustTemp, "exhausttemp", "exhaust temperature", "Abgastemperatur", "Uitlaattemperatuur", "Avgastemperatur", "temperatura spalin", "røykgasstemp", "température des gaz d'échappement")
MAKE_TRANSLATION(burnGas, "burngas", "gas", "Gas", "Gas", "Gas", "gaz", "gass", "gaz")
MAKE_TRANSLATION(burnGas2, "burngas2", "gas stage 2", "Gas Stufe 2", "gas fase 2", "Gas Fas 2", "gaz 2 stopień", "gass 2", "gaz état 2")
MAKE_TRANSLATION(flameCurr, "flamecurr", "flame current", "Flammenstrom", "Vlammenstroom", "Lågström", "prąd palnika", "flammestrøm", "courrant de flamme")
MAKE_TRANSLATION(heatingPump, "heatingpump", "heating pump", "Heizungspumpe", "Verwarmingspomp", "Värmepump", "pompa ciepła", "varmepumpe", "pompe à chaleur")
MAKE_TRANSLATION(fanWork, "fanwork", "fan", "Gebläse", "Ventilator", "Fläkt", "wentylator", "vifte", "ventilateur")
MAKE_TRANSLATION(ignWork, "ignwork", "ignition", "Zündung", "Ontsteking", "Tändning", "zapłon", "tenning", "ignition")
MAKE_TRANSLATION(heatingActivated, "heatingactivated", "heating activated", "Heizen aktiviert", "Verwarmen geactiveerd", "Uppvärmning aktiv", "system c.o.", "oppvarming aktivert", "chauffage activé")
MAKE_TRANSLATION(heatingTemp, "heatingtemp", "heating temperature", "Heizungstemperatur", "Verwarmingstemperatuur", "Uppvärmningstemperatur", "temperatura grzania", "oppvarmingstemperatur", "température de chauffage")
MAKE_TRANSLATION(pumpModMax, "pumpmodmax", "boiler pump max power", "Kesselpumpen Maximalleistung", "Ketelpomp max vermogen", "Värmepannepump max effekt", "maksymalna moc pompy zasobnika", "varmepumpe maks effekt", "puissance max pompe à chaleur")
MAKE_TRANSLATION(pumpModMin, "pumpmodmin", "boiler pump min power", "Kesselpumpen Minmalleistung", "Ketelpomp min vermogen", "Värmepannepump min effekt", "minimalna moc pompy zasobnika", "varmepumpe min effekt", "puissance min pompe à chaleur")
MAKE_TRANSLATION(pumpDelay, "pumpdelay", "pump delay", "Pumpennachlaufzeit", "Pomp nalooptijd", "Pumpfördröjning", "opóźnienie pompy", "pumpeforsinkelse", "délai d'attente pompe")
MAKE_TRANSLATION(burnMinPeriod, "burnminperiod", "burner min period", "Antipendelzeit", "Antipendeltijd", "Värmepanna Min Period", "minimalny czas pracy palnika", "varmekjele min periode", "délai d'attente du brûleur")
MAKE_TRANSLATION(burnMinPower, "burnminpower", "burner min power", "minimale Brennerleistung", "Minimaal brandervermogen", "Värmepanna Min Effekt", "minimalna moc źródła ciepła", "varmekjele min effekt", "puissance min brûleur")
MAKE_TRANSLATION(burnMaxPower, "burnmaxpower", "burner max power", "maximale Brennerleistung", "Maximaal brandervermogen", "Värmepanna Max Effekt", "maksymalna moc źródła ciepła", "varmekjele maks effekt", "puissance max brûleur")
MAKE_TRANSLATION(boilHystOn, "boilhyston", "hysteresis on temperature", "Einschaltdifferenz", "ketel aan hysterese verschil", "Hysteres aktiveringstemperatur", "histereza załączania", "hysterese på temperatur", "hysteresis température d'allumage")
MAKE_TRANSLATION(boilHystOff, "boilhystoff", "hysteresis off temperature", "Ausschaltdifferenz", "ketel uit hysterese verschil", "Hysteres inaktiveringstemperatur", "histereza wyłączania", "hysterese av temperatur", "hysteresis température d'extinction")
MAKE_TRANSLATION(boil2HystOn, "boil2hyston", "hysteresis stage 2 on temperature", "Einschaltdifferenz Stufe 2", "ketel aan hysterese verschil 2", "Hysteres aktiveringstemperatur 2", "histereza załączania stopnia 2", "", "hysteresis état 2 température d'allumage") // TODO translate
MAKE_TRANSLATION(boil2HystOff, "boil2hystoff", "hysteresis stage 2 off temperature", "Ausschaltdifferenz Stufe 2", "ketel uit hysterese verschil 2", "Hysteres inaktiveringstemperatur 2", "histereza wyłączania stopnia 2", "", "hysteresis état 2 température d'extinction") // TODO translate
MAKE_TRANSLATION(setFlowTemp, "setflowtemp", "set flow temperature", "Sollwert Vorlauftemperatur", "Ingestelde aanvoertemperatuur", "Börvärde Flödestemperatur", "zadana temperatura zasilania", "innstilt strømmetemperatur", "température du flux définie")
MAKE_TRANSLATION(setBurnPow, "setburnpow", "burner set power", "Sollwert Brennerleistung", "Ingesteld brandervermogen", "Värmepanna vald Effekt", "zadana moc palnika", "varmekjele valgt effekt", "puissance du brûleur définie")
MAKE_TRANSLATION(curBurnPow, "curburnpow", "burner current power", "Brennerleistung", "Brandervermogen", "Värmepanna aktuell effekt", "aktualna moc źródła ciepła", "brennereffekt", "puissance du brûleur actuelle")
MAKE_TRANSLATION(burnStarts, "burnstarts", "burner starts", "Brenner Starts", "Aantal brander starts", "Värmepanna antal starter", "liczba uruchomień palnika", "antall brenner starter", "démarrages du brûleur")
MAKE_TRANSLATION(burnWorkMin, "burnworkmin", "total burner operating time", "Brenner Laufzeit", "Totale branderlooptijd", "Värmepanna aktiva timmar", "łączny czas pracy palnika", "brennersteg tid i min", "durée de fonctionnement totale du brûleur")
MAKE_TRANSLATION(burn2WorkMin, "burn2workmin", "burner stage 2 operating time", "Brenner Stufe 2 Laufzeit", "Totale looptijd brander fase 2", "Värmepanna steg 2 aktiva timmar", "łączny czas pracy palnika 2 stopnia", "brennersteg2 tid i min", "durée de fonctionnement totale du brûleur état 2")
MAKE_TRANSLATION(heatWorkMin, "heatworkmin", "total heat operating time", "Heizung Laufzeit", "Totale looptijd verwarming", "Uppvärmning aktiva timmar", "łączny czas grzania", "varmetid i min", "durée de fonctionnement du chauffage")
MAKE_TRANSLATION(heatStarts, "heatstarts", "burner starts heating", "Brenner Starts Heizung", "Aantal brander starts verwarming", "Uppvärmning antal starter", "liczba uruchomień palnika na ogrzewanie", "", "démarrages du chauffage") // TODO translate
MAKE_TRANSLATION(UBAuptime, "ubauptime", "total UBA operating time", "Anlagen-Gesamtlaufzeit", "totale looptijd branderautomaat (UBA)", "Total Tid", "łączny czas pracy układu sterowania", "totaltid", "durée de fonctionnement totale de l'appareil (UBA)")
MAKE_TRANSLATION(lastCode, "lastcode", "last error code", "Letzter Fehler", "Laatste foutcode", "Senaste Felkod", "ostatni błąd", "siste feilkode", "dernier code d'erreur")
MAKE_TRANSLATION(serviceCode, "servicecode", "service code", "Statusmeldung", "Statuscode", "Servicekod", "kod serwisowy", "servicekode", "code de service")
MAKE_TRANSLATION(serviceCodeNumber, "servicecodenumber", "service code number", "Statusmeldungsnummer", "Status codenummer", "Servicekod", "numer kodu serwisowego", "servicekodenummer", "numéro du code de service")
MAKE_TRANSLATION(maintenanceMessage, "maintenancemessage", "maintenance message", "Wartungsmeldung", "Onderhoudsmelding", "Servicemeddelande", "komunikat przeglądu", "vedlikeholdsmelding", "message de maintenance")
MAKE_TRANSLATION(maintenanceDate, "maintenancedate", "next maintenance date", "Wartungsdatum", "Onderhoudsdatum", "Datum nästa Service", "termin następnego przeglądu", "vedlikeholdsdato", "prochaine date de maintenance")
MAKE_TRANSLATION(maintenanceType, "maintenance", "maintenance scheduled", "Wartungsplan", "Onderhoud gepland", "Underhall schemlagt", "rodzaj przeglądu", "vedlikeholdstype", "maintenance prévue")
MAKE_TRANSLATION(maintenanceTime, "maintenancetime", "time to next maintenance", "Wartung in", "Onderhoud in", "Tid till nästa underhall", "czas do kolejnego przeglądu", "vedlikeholdstid", "durée avant la prochaine maintenance")
MAKE_TRANSLATION(emergencyOps, "emergencyops", "emergency operation", "Notoperation", "Noodoperatie", "Nöddrift", "praca w trybie awaryjnym", "nøddrift", "opération d'urgence")
MAKE_TRANSLATION(emergencyTemp, "emergencytemp", "emergency temperature", "Nottemperatur", "Noodtemperatuur", "Nöddrift temperatur", "temperatura w trybie awaryjnym", "nødtemperatur", "température d'urgence")
MAKE_TRANSLATION(pumpMode, "pumpmode", "boiler pump mode", "Kesselpumpen Modus", "", "", "", "", "") // TODO translate

// heatpump/compress specific
MAKE_TRANSLATION(upTimeControl, "uptimecontrol", "total operating time heat", "Betriebszeit Heizen gesamt", "Totale bedrijfstijd", "Total tid uppvärmning", "łączny czas generowania ciepła", "", "durée totale de fonctionnement chauffage") // TODO translate
MAKE_TRANSLATION(upTimeCompHeating, "uptimecompheating", "operating time compressor heating", "Betriebszeit Kompressor heizen", "Bedrijfstijd compressor verwarmingsbedrijf", "Total tid kompressor uppvärmning", "łączny czas ogrzewania (sprężarka)", "", "durée de fonctionnement compresseur chauffage") // TODO translate
MAKE_TRANSLATION(upTimeCompCooling, "uptimecompcooling", "operating time compressor cooling", "Betriebszeit Kompressor kühlen", "Bedrijfstijd compressor koelbedrijf", "Total tid kompressor kyla", "łączny czas chłodzenia (sprężarka)", "Total tid kompressor kjøling", "durée de fonctionnement compresseur refroidissement")
MAKE_TRANSLATION(upTimeCompWw, "uptimecompww", "operating time compressor dhw", "Betriebszeit Kompressor", "Bedrijfstijd compressor warmwaterbedrijf", "Total tid kompressor varmvatten", "łączny czas grzania c.w.u. (sprężarka)", "Total tid kompressor varmtvann", "durée de fonctionnement compresseur ecs")
MAKE_TRANSLATION(upTimeCompPool, "uptimecomppool", "operating time compressor pool", "Betriebszeit Kompressor Pool", "Bedrijfstijd compressor voor zwembadbedrijf", "Total tid kompressor pool", "łączny czas podgrzewania basenu (sprężarka)", "Total tid kompressor basseng", "durée de fonctionnement compresseur piscine")
MAKE_TRANSLATION(totalCompStarts, "totalcompstarts", "total compressor control starts", "Kompressor Starts gesamt", "Totaal compressorstarts", "Kompressorstarter Totalt", "liczba załączeń sprężarki", "kompressorstarter Totalt", "nombre démarrages total contrôle compresseur")
MAKE_TRANSLATION(heatingStarts, "heatingstarts", "heating control starts", "Heizen Starts", "Starts verwarmingsbedrijf", "Kompressorstarter Uppvärmning", "liczba załączeń ogrzewania", "kompressorstarter oppvarming", "démarrages contrôle chauffage")
MAKE_TRANSLATION(coolingStarts, "coolingstarts", "cooling control starts", "Kühlen Starts", "Starts koelbedrijf", "Kompressorstarter Kyla", "liczba załączeń chłodzenia", "kompressorstarter kjøling", "démarrages contrôle refroidissement")
MAKE_TRANSLATION(poolStarts, "poolstarts", "pool control starts", "Pool Starts", "Starts zwembadbedrijf", "Kompressorstarter Pool", "liczba załączeń podgrzewania basenu", "kompressorstarter basseng", "démarrages contrôle piscine")
MAKE_TRANSLATION(nrgConsTotal, "nrgconstotal", "total energy consumption", "Energieverbrauch gesamt", "Energieverbrauch gesamt", "Energiförbrukning totalt", "energia pobrana (sumarycznie)", "energiforbruk totalt", "consommation totale énergie")
MAKE_TRANSLATION(nrgConsCompTotal, "nrgconscomptotal", "total energy consumption compressor", "Energieverbrauch Kompressor gesamt", "Energieverbruik compressor totaal", "Energiförbrukning kompressor", "energia pobrana przez sprężarkę", "energiforbruk kompressor", "consommation totale énergie compresseur")
MAKE_TRANSLATION(nrgConsCompHeating, "nrgconscompheating", "energy consumption compressor heating", "Energieverbrauch Kompressor heizen", "Energieverbruik compressor verwarmingsbedrijf", "Energiförbrukning uppvärmning", "energia pobrana przez sprężarkę na ogrzewanie", "energiforbruk oppvarming", "consommation  énergie compresseur chauffage")
MAKE_TRANSLATION(nrgConsCompWw, "nrgconscompww", "energy consumption compressor dhw", "Energieverbrauch Kompressor", "Energieverbruik compressor warmwaterbedrijf", "Energiförbrukning varmvatten", "energia pobrana przez sprężarkę na c.w.u.", "energiforbruk varmvann", "consommation  énergie compresseur ecs")
MAKE_TRANSLATION(nrgConsCompCooling, "nrgconscompcooling", "energy consumption compressor cooling", "Energieverbrauch Kompressor kühlen", "Energieverbruik compressor koelbedrijf", "Energiförbrukning kyla", "energia pobrana przez sprężarkę na chłodzenie", "energiforbruk kjøling", "consommation  énergie compresseur refroidissement")
MAKE_TRANSLATION(nrgConsCompPool, "nrgconscomppool", "energy consumption compressor pool", "Energieverbrauch Kompressor Pool", "Energiebedrijf compressor zwembadbedrijf", "Energiförbrukning pool", "energia pobrana przez sprężarkę na podgrzewanie basenu", "energiforbruk basseng", "consommation  énergie compresseur piscine")
MAKE_TRANSLATION(nrgSuppTotal, "nrgsupptotal", "total energy supplied", "gesamte Energieabgabe", "Totaal opgewekte energie", "Genererad energi", "energia oddana (sumarycznie)", "tilført energi", "énergie totale fournie")
MAKE_TRANSLATION(nrgSuppHeating, "nrgsuppheating", "total energy supplied heating", "gesamte Energieabgabe heizen", "Opgewekte energie verwarmingsbedrijf", "Genererad energi Uppvärmning", "energia oddana na ogrzewanie", "tilført energi oppvarming", "énergie totale fournie chauffage")
MAKE_TRANSLATION(nrgSuppWw, "nrgsuppww", "total energy warm supplied dhw", "gesamte Energieabgabe", "Opgewekte energie warmwaterbedrijf", "Genererad energi Varmvatten", "energia oddana na c.w.u.", "tilført energi varmvann", "énergie chaude totale fournie ecs")
MAKE_TRANSLATION(nrgSuppCooling, "nrgsuppcooling", "total energy supplied cooling", "gesamte Energieabgabe kühlen", "Opgewekte energie koelbedrijf", "Genererad energi Kyla", "energia oddana na chłodzenie", "Tillført energi kjøling", "énergie totale fournie refroidissement")
MAKE_TRANSLATION(nrgSuppPool, "nrgsupppool", "total energy supplied pool", "gesamte Energieabgabe Pool", "Opgewekte energie zwembadbedrijf", "Genererad energi Pool", "energia oddana na podgrzewanie basenu", "tilført energi basseng", "énergie totale fournie piscine")
MAKE_TRANSLATION(auxElecHeatNrgConsTotal, "auxelecheatnrgconstotal", "total aux elec. heater energy consumption", "Energieverbrauch el. Zusatzheizung", "Totaal energieverbruik electrisch verwarmingselement", "Energiförbrukning Eltillkott", "energia pobrana przez grzałki", "energiforbruk varmekolbe", "consommation totale énergie electrique auxiliaire chauffage")
MAKE_TRANSLATION(auxElecHeatNrgConsHeating, "auxelecheatnrgconsheating", "aux elec. heater energy consumption heating", "Energieverbrauch el. Zusatzheizung Heizen", "Energieverbruik electrisch verwarmingselement voor verwarmingsbedrijf", "Energiförbrukning Eltillskott Uppvärmning", "energia pobrana przez grzałki na ogrzewanie", "energiforbruk varmekolbe oppvarming", "consommation énergie electrique auxiliaire chauffage")
MAKE_TRANSLATION(auxElecHeatNrgConsWW, "auxelecheatnrgconsww", "aux elec. heater energy consumption dhw", "Energieverbrauch el. Zusatzheizung", "Energieverbruik electrisch verwarmingselement voor warmwaterbedrijf", "Energiförbrukning Eltillskott Varmvatten", "energia pobrana przez grzałki na c.w.u.", "energiförbruk varmekolbe varmvann", "consommation énergie electrique auxiliaire chauffage ecs")
MAKE_TRANSLATION(auxElecHeatNrgConsPool, "auxelecheatnrgconspool", "aux elec. heater energy consumption pool", "Energieverbrauch el. Zusatzheizung Pool", "Energieverbruik electrisch verwarmingselement voor zwembadbedrijf", "Energiförbrukning Eltillskott Pool", "energia pobrana przez grzałki na podgrzewanie basenu", "", "consommation énergie electrique auxiliaire chauffage piscine") // TODO translate

MAKE_TRANSLATION(hpCompOn, "hpcompon", "hp compressor", "WP Kompressor", "WP compressor", "VP Kompressor", "sprężarka pompy ciepła", "vp kompressor", "compresseur pompe à chaleur")
MAKE_TRANSLATION(hpHeatingOn, "hpheatingon", "hp heating", "WP Heizen", "WP verwarmingsbedrijf", "VP Uppvärmning", "pompa ciepła, ogrzewanie", "vp oppvarmning", "chauffe pompe à chaleur")
MAKE_TRANSLATION(hpCoolingOn, "hpcoolingon", "hp cooling", "WP Kühlen", "WP koelbedrijf", "VP Kyla", "pompa ciepła, chłodzenie", "vp kjøling", "refroidissement pompe à chaleur")
MAKE_TRANSLATION(hpWwOn, "hpwwon", "hp dhw", "WP Warmwasser", "WP warmwaterbedrijf", "VP Varmvatten", "pompa ciepła, c.w.u.", "vp varmvatten", "eau chaude pompe à chaleur")
MAKE_TRANSLATION(hpPoolOn, "hppoolon", "hp pool", "WP Pool", "WP zwembadbedrijf", "VP Pool", "pompa ciepła, podgrzewanie basenu", "vp basseng", "pompe à chaleur piscine")
MAKE_TRANSLATION(hpBrinePumpSpd, "hpbrinepumpspd", "brine pump speed", "Solepumpen-Geschw.", "Snelheid pekelpomp", "Hastighet Brine-pump", "wysterowanie pompy glikolu", "hastighet brine-pumpe", "vitesse pompe à saumure")
MAKE_TRANSLATION(hpCompSpd, "hpcompspd", "compressor speed", "Kompressor-Geschw.", "Snelheid compressor", "Kompressorhastighet", "wysterowanie sprężarki", "kompressorhastighet", "vitesse du compresseur")
MAKE_TRANSLATION(hpCircSpd, "hpcircspd", "circulation pump speed", "Zirkulationspumpen-Geschw.", "Snelheid circulatiepomp", "Hastighet Cirkulationspump", "wysterowanie pompy obiegu grzewczego", "hastighet sirkulationspumpe", "vitesse pompe à circulation")
MAKE_TRANSLATION(hpBrineIn, "hpbrinein", "brine in/evaporator", "Sole in/Verdampfer", "pekel in/verdamper", "Brine in (förangare)", "temperatura glikolu na wejściu kolektora (TB0)", "brine in/fordamper", "entrée saumure/évaporateur")
MAKE_TRANSLATION(hpBrineOut, "hpbrineout", "brine out/condenser", "Sole aus/Kondensator", "pekel uit/condensor", "Brine ut (kondensor)", "temperatura glikolu na wyjściu kolektora (TB1)", "Brine ut/kondensor", "sortie saumure/condenseur")
MAKE_TRANSLATION(hpSwitchValve, "hpswitchvalve", "switch valve", "Schaltventil", "schakelklep", "Växelventil", "zawór przełączający", "skifteventil", "valve de commutation")
MAKE_TRANSLATION(hpActivity, "hpactivity", "compressor activity", "Kompressoraktivität", "Compressoractiviteit", "Kompressoraktivitet", "pompa ciepła, aktywność sprężarki", "", "activité du compresseur") // TODO translate

MAKE_TRANSLATION(hpPower, "hppower", "compressor power output", "Kompressorleistung", "Compressorvermogen", "Kompressoreffekt", "moc wyjściowa sprężarki", "kompressoreffekt", "puissance de sortie compresseur")
MAKE_TRANSLATION(hpTc0, "hptc0", "heat carrier return (TC0)", "Kältemittel Rücklauf (TC0)", "Koudemiddel retour (TC0)", "Värmebärare Retur (TC0)", "temperatura nośnika ciepła na powrocie (TC0)", "kjølemiddel retur (TC0)", "retour caloporteur (TC0)")
MAKE_TRANSLATION(hpTc1, "hptc1", "heat carrier forward (TC1)", "Kältemittel Vorlauf (TC1)", "Koudemiddel aanvoer (TC1)", "Värmebärare Framledning (TC1)", "temperatura nośnika ciepła pierwotna (TC1)", "kjølemiddel tur (TC1)", "avance caloporteur (TC1)")
MAKE_TRANSLATION(hpTc3, "hptc3", "condenser temperature (TC3)", "Verflüssigertemperatur (TC3)", "Condensortemperatuur (TC3)", "Kondensortemperatur (TC3)", "temperatura skraplacza/na wyjściu sprężarki (TC3)", "kondensortemperatur (TC3)", "température condensateur (TC3)")
MAKE_TRANSLATION(hpTr1, "hptr1", "compressor temperature (TR1)", "Kompessortemperatur (TR1)", "Compressor temperatuur (TR1)", "Kompressor temp (TR1)", "temperatura sprężarki (TR1)", "kompressor temperatur (TR1)", "température compresseur (TR1)")
MAKE_TRANSLATION(hpTr3, "hptr3", "refrigerant temperature liquid side (condenser output) (TR3)", "Kältemittel (flüssig) (TR3)", "Temperatuur koudemiddel vloeibare zijde (TR3)", "Köldmedium temperatur (kondensorutlopp) (TR3)", "temperatura skraplacza ogrzew. (TR3)", "kjølemiddeltemperatur på væskesiden (TR3)", "température réfrigérant côté liquide (sortie condensateur) (TR3)")
MAKE_TRANSLATION(hpTr4, "hptr4", "evaporator inlet temperature (TR4)", "Verdampfer Eingang (TR4)", "Verdamper ingangstemperatuur (TR4)", "Förångare inloppstemp (TR4)", "temperatura na wejściu parownika (TR4)", "innløpstemperatur for fordamperen (TR4)", "température entrée évaporateur (TR4)")
MAKE_TRANSLATION(hpTr5, "hptr5", "compressor inlet temperature (TR5)", "Kompessoreingang (TR5)", "Compressor ingangstemperatuur (TR5)", "Kompressor inloppstemp (TR5)", "temperatura na wejściu sprężarki (TR5)", "kompressor innløpstemp (TR5)", "température entrée compresseur (TR5)")
MAKE_TRANSLATION(hpTr6, "hptr6", "compressor outlet temperature (TR6)", "Kompressorausgang (TR6)", "Compressor uitgangstemperatuur (TR6)", "Kompressor utloppstemp (TR6)", "temperatura na wyjściu sprężarki (TR6)", "kompressor utløpstemp (TR6)", "température sortie compresseur (TR6)")
MAKE_TRANSLATION(hpTr7, "hptr7", "refrigerant temperature gas side (condenser input) (TR7)", "Kältemittel (gasförmig) (TR7)", "Temperatuur koudemiddel gasvormig (TR7)", "Köldmedium temperatur gassida (kondensorinlopp) (TR7)", "temperatura czynnika chłodniczego po stronie gazu (wejście skraplacza) (TR7)", "kjølemedium temperatur gassida (kondensatorinløp) (TR7)", "température réfrigérant côté gaz (sortie condensateur) (TR7)")
MAKE_TRANSLATION(hpTl2, "hptl2", "air inlet temperature (TL2)", "Außenluft-Einlasstemperatur (TL2)", "Temperatuur luchtinlaat (TL2)", "Luftintagstemperatur (TL2)", "temperatura wlotu powietrza (TL2)", "luftinntakstemperatur (TL2)", "température entrée air (TL2)")
MAKE_TRANSLATION(hpPl1, "hppl1", "low pressure side temperature (PL1)", "Niederdruckfühler (PL1)", "Temperatuur lage drukzijde (PL1)", "Temperatur Lågtryckssidan (PL1)", "temperatura po stronie niskiego ciśnienia (PL1)", "temperatur lavtrykksiden (PL1)", "température côté basse pression (PL1)")
MAKE_TRANSLATION(hpPh1, "hpph1", "high pressure side temperature (PH1)", "Hochdruckfühler (PH1)", "Temperatuur hoge drukzijde (PH1)", "Temperatur Högtryckssidan (PH1)", "temperatura po stronie wysokiego ciśnienia (PH1)", "Temperatur Høytrykksiden (PH1)", "température côté bhauteasse pression (PH1)")
MAKE_TRANSLATION(hpTa4, "hpta4", "drain pan temp (TA4)", "Kondensatorwanne (TA4)", "", "", "temperatura ociekacza (TA4)", "", "") // TODO translate

MAKE_TRANSLATION(hpInput1, "hpin1", "input 1 state", "Eingang 1 Status", "Status input 1", "Status Ingång 1", "stan wejścia 1", "status Inggng 1", "état entrée 1")
MAKE_TRANSLATION(hpInput2, "hpin2", "input 2 state", "Eingang 2 Status", "Status input 2", "Status Ingång 2", "stan wejścia 2", "status Inggng 2", "état entrée 2")
MAKE_TRANSLATION(hpInput3, "hpin3", "input 3 state", "Eingang 3 Status", "Status input 3", "Status Ingång 3", "stan wejścia 3", "status Inggng 3", "état entrée 3")
MAKE_TRANSLATION(hpInput4, "hpin4", "input 4 state", "Eingang 4 Status", "Status input 4", "Status Ingång 4", "stan wejścia 4", "status Inggng 4", "état entrée 4")
MAKE_TRANSLATION(hpIn1Opt, "hpin1opt", "input 1 options", "Eingang 1 Einstellung", "Instelling input 1", "Inställningar Ingång 1", "opcje wejścia 1", "innstillinger inngang 1", "options entrée 1")
MAKE_TRANSLATION(hpIn2Opt, "hpin2opt", "input 2 options", "Eingang 2 Einstellung", "Instelling input 2", "Inställningar Ingång 2", "opcje wejścia 2", "innstillinger inngang 2", "options entrée 2")
MAKE_TRANSLATION(hpIn3Opt, "hpin3opt", "input 3 options", "Eingang 3 Einstellung", "Instelling input 3", "Inställningar Ingång 3", "opcje wejścia 3", "innstillinger inngang 3", "options entrée 3")
MAKE_TRANSLATION(hpIn4Opt, "hpin4opt", "input 4 options", "Eingang 4 Einstellung", "Instelling input 4", "Inställningar Ingång 4", "opcje wejścia 4", "innstillinger inngang 4", "options entrée 4")
MAKE_TRANSLATION(maxHeatComp, "maxheatcomp", "heat limit compressor", "Heizgrenze Kompressor", "heat limit compressor", "heat limit compressor", "ograniczenie mocy sprężarki", "max varmegrense kompressor", "limite chaleur compresseur")
MAKE_TRANSLATION(maxHeatHeat, "maxheatheat", "heat limit heating", "Heizgrenze Heizen", "heat limit heating", "heat limit heating", "ograniczenie mocy w trybie ogrzewania", "maks varmegrense oppvarming", "limite chaleur chauffage")
MAKE_TRANSLATION(maxHeatDhw, "maxheatdhw", "heat limit dhw", "Heizgrenze Warmwasser", "heat limit dhw", "heat limit dhw", "ograniczenie mocy w trybie c.w.u.", "varmegrense varmtvann", "limite chaleur ecs")

MAKE_TRANSLATION(auxHeaterOff, "auxheateroff", "disable aux heater", "Verbiete Zusatzheizer", "Bijverwarming uitsc", "Blockera eltillskott", "wyłącz dogrzewacz", "", "Désactiver chauff. d'app") // TODO translate
MAKE_TRANSLATION(auxHeaterStatus, "auxheaterstatus", "aux heater status", "Status Zusatzheizer", "Bijverwarming", "Eltillskott Status", "status dogrzewacza", "", "Chauffage auxiliaire") // TODO translate
MAKE_TRANSLATION(auxHeaterOnly, "auxheateronly", "aux heater only", "nur Zusatzheizer","Alleen bijverwarming", "Eltillskott Enbart", "tylko dogrzewacz", "", "Que chauffage auxiliaire") // TODO translate
MAKE_TRANSLATION(auxHeaterDelay, "auxheaterdelay", "aux heater on delay", "Zusatzheizer verzögert ein", "Bijverw. vertraagd aan", "Eltillskottfördröjning på", "opóźnienie włączenia dogrzewacza", "Tilleggsvarmer forsinket på", "Chauff app tempo marche")
MAKE_TRANSLATION(silentMode, "silentmode", "silent mode", "Silentmodus", " Stiller gebruik", "Tyst läge", "tryb cichy",  "", "Fct silencieux") // TODO translate
MAKE_TRANSLATION(minTempSilent, "mintempsilent", "min outside temp for silent mode", "Minimale Aussentemperatur Silentmodus", " Stiller gebruik min. buitentemp", "Tyst läge min temp", "minimalna temperatura zewnętrzna dla trybu cichego", "", "Fct silencieux: Temp. extérieure min.") // TODO translate
MAKE_TRANSLATION(tempParMode, "tempparmode", "outside temp parallel mode", "Aussentemperatur Parallelmodus", "Buitentemp. parallelbedr", "Parallelläge Utomhustemp.", "maksymalna temperatura zewnętrzna dla dogrzewacza", "", "Temp. ext. fct parallèle") // TODO translate
MAKE_TRANSLATION(auxHeatMixValve, "auxheatmix", "aux heater mixing valve", "Mischer Zusatzheizer", "Bijverwarming menger", "Eltilskott Blandarventil", "mieszacz dogrzewacza", "", "Chauffage auxiliaire mélangeur") // TODO translate
MAKE_TRANSLATION(hpHystHeat, "hphystheat", "on/off hyst heat", "Schalthysterese Heizen", "Aan/uit-hysteresis in verw. bedrijf", "Hstereses Uppvärm.", "histereza wł./wył. ogrzewania", "På/av-hysterese Oppvar.", "Hystérésis Marche en mode chauffage")
MAKE_TRANSLATION(hpHystCool, "hphystcool", "on/off hyst cool", "Schalthysterese Kühlen", "Aan/uit-hysteresis in koelbedrijf ", "Hystereses Kyla", "histereza wł./wył. chłodzenia", "", "Hystérésis Marche en mode refroidissement") // TODO translate
MAKE_TRANSLATION(hpHystPool, "hphystpool", "on/off hyst pool", "Schalthysterese Pool", "an/uit-hysteresis in zwembadbedri", "Hystereses Pool", "histereza wł./wył. podgrzewania basenu", "", "Hystérésis Marche en mode piscine") // TODO translate
MAKE_TRANSLATION(tempDiffHeat, "tempdiffheat", "temp diff TC3/TC0 heat", "Temp.diff. TC3/TC0 Heizen", "Temp.vers. TC3/TC0 verw", "Delta(T) TC3/TC0 Uppvärm.", "różnica temperatur TC3/TC0 w trakcie ogrzewania", "", "Delta T TC3/TC0 Chauff") // TODO translate
MAKE_TRANSLATION(tempDiffCool, "tempdiffcool", "temp diff TC3/TC0 cool", "Temp.diff. TC3/TC0 Kühlen", "Temp.vers. TC3/TC0 koel.", "Delta(T) TC3/TC0 Kyla", "różnica temperatur TC3/TC0 w trakcie chłodzenia", "", "Delta T TC3/TC0 Refroid.") // TODO translate
MAKE_TRANSLATION(silentFrom, "silentfrom", "silent mode from", "Silentmodus Start", "", "", "początek trybu cichego", "", "") // TODO translate
MAKE_TRANSLATION(silentTo, "silentto", "silent mode to", "Silentmodus Ende", "", "", "koniec trybu cichego", "", "") // TODO translate

MAKE_TRANSLATION(wwComfOffTemp, "wwcomfoff", "comfort switch off", "Komfort Ausschalttemp", "Comfort Uitschakeltemp.", "Komfortläge avstängingstemp.", "temperatura wyłączania w trybie komfort", "", "Confort Temp. d'arrêt") // TODO translate
MAKE_TRANSLATION(wwEcoOffTemp, "wwecooff", "eco switch off", "ECO Ausschalttemp", "Eco Uitschakeltemp.", "Ekoläge avstängningstemp.", "temperatura wyłączania w trybie eko", "", "Eco Temp. d'arrêt") // TODO translate
MAKE_TRANSLATION(wwEcoPlusOffTemp, "wwecoplusoff", "eco+ switch off", "ECO+ Ausschalttemp", "Eco+ Uitschakeltemp.", "Eko+ avstängningstemp.", "temperatura wyłączania w trybie eko+", "", "Eco+ Temp. d'arrêt") // TODO translate

MAKE_TRANSLATION(auxHeatMode, "auxheatrmode", "aux heater mode", "Modus Zusatzheizer", "", "", "tryb pracy dogrzewacza po blokadzie z Zakładu Energetycznego", "", "") // TODO translate
MAKE_TRANSLATION(auxMaxLimit, "auxmaxlimit", "aux heater max limit", "Zusatzheizer max. Grenze", "", "", "dogrzewacz, maksymalny limit", "", "") // TODO translate
MAKE_TRANSLATION(auxLimitStart, "auxlimitstart", "aux heater limit start", "Zusatzheizer Grenze Start", "", "", "dogrzewacz, początek ograniczenia", "", "") // TODO translate
MAKE_TRANSLATION(manDefrost, "mandefrost", "manual defrost", "Manuelle Enteisung", "", "", "ręczne odladzanie", "", "") // TODO translate
MAKE_TRANSLATION(pvCooling, "pvcooling", "Cooling only with PV", "Kühlen nur mit PV", "", "", "chłodzenie tylko z PV", "", "") // TODO translate
MAKE_TRANSLATION(hpCircPumpWw, "hpcircpumpww", "circulation pump available during dhw", "", "", "", "pompa cyrkulacji dostępna w trakcie c.w.u.", "", "") // TODO translate
MAKE_TRANSLATION(vp_cooling, "vpcooling", "valve/pump cooling", "", "", "", "zawór/pompa chłodzenia", "", "") // TODO translate
MAKE_TRANSLATION(VC0valve, "vc0valve", "VC0 valve", "", "", "", "zawór VC0", "", "") // TODO translate
MAKE_TRANSLATION(primePump, "primepump", "primary heatpump", "", "", "", "główna pompa ciepła", "", "") // TODO translate
MAKE_TRANSLATION(primePumpMod, "primepumpmod", "primary heatpump modulation", "", "", "", "wysterowanie głównej pompy ciepła", "", "") // TODO translate
MAKE_TRANSLATION(hp3wayValve, "hp3way", "3-way valve", "", "", "", "zawór 3-drogowy pompy ciepła", "", "") // TODO translate
MAKE_TRANSLATION(elHeatStep1, "elheatstep1", "el. heater step 1", "", "", "", "dogrzewacz poziom 1", "", "") // TODO translate
MAKE_TRANSLATION(elHeatStep2, "elheatstep2", "el. heater step 2", "", "", "", "dogrzewacz poziom 2", "", "") // TODO translate
MAKE_TRANSLATION(elHeatStep3, "elheatstep3", "el. heater step 3", "", "", "", "dogrzewacz poziom 3", "", "") // TODO translate
MAKE_TRANSLATION(wwAlternatingOper, "wwalternatingop", "alternating operation", "", "", "", "praca naprzemienna", "", "") // TODO translate
MAKE_TRANSLATION(wwAltOpPrioHeat, "wwaltopprioheat", "prioritise heating during dhw", "", "", "", "czas na ogrzewanie w trakcie c.w.u", "", "") // TODO translate
MAKE_TRANSLATION(wwAltOpPrioWw, "wwaltopprioww", "prioritise dhw during heating", "", "", "", "czas na c.w.u w trakcie ogrzewania", "", "") // TODO translate

// hybrid heatpump
MAKE_TRANSLATION(hybridStrategy, "hybridstrategy", "hybrid control strategy", "Hybrid Strategie", "Hybride strategie", "Hybrid kontrollstrategi", "strategia sterowania hybrydowego", "hybrid kontrollstrategi", "stratégie contrôle hybride")
MAKE_TRANSLATION(switchOverTemp, "switchovertemp", "outside switchover temperature", "Außentemperatur für Umschaltung", "Schakeltemperatuur buitentemperatuur", "Utomhus Omställningstemperatur", "zewnętrzna temperatura przełączania", "utendørstemp styring", "basculement par température extérieure")
MAKE_TRANSLATION(energyCostRatio, "energycostratio", "energy cost ratio", "Energie/Kosten-Verhältnis", "Energiekostenratio", "Energi/Kostnads-förhållande", "współczynnik energia/koszt", "energi/kostnads forhold", "ratio coût énergie")
MAKE_TRANSLATION(fossileFactor, "fossilefactor", "fossile energy factor", "Energiefaktor Fossil", "Energiefactor fossiele brandstof", "Energifaktor fossilenergi", "udział energii z paliw kopalnych", "energifaktor fossilenergi", "facteur énergie fossile")
MAKE_TRANSLATION(electricFactor, "electricfactor", "electric energy factor", "Energiefaktor elektrisch", "Energiefactor electrisch", "Elektrisk energifaktor", "udział energii elektrycznej", "elektrisk energifaktor", "facteur énergie électrique")
MAKE_TRANSLATION(delayBoiler, "delayboiler", "delay boiler support", "Verzögerungs-Option", "Vertragingsoptie", "Fördröjningsoption", "opcja opóźnienia", "Fördörjningsoption", "option retardement chaudière")
MAKE_TRANSLATION(tempDiffBoiler, "tempdiffboiler", "temp diff boiler support", "Temperaturdifferenz-Option", "Verschiltemperatuuroptie", "Temperaturskillnadsoption", "opcja różnicy temperatur", "temperatursforskjell kjele", "option différence température")
MAKE_TRANSLATION(lowNoiseMode, "lownoisemode", "low noise mode", "Geräuscharmer Betrieb", "", "Tyst läge", "tryb cichy", "", "mode faible bruit") // TODO translate
MAKE_TRANSLATION(lowNoiseStart, "lownoisestart", "low noise starttime", "Start geräuscharmer Betrieb", "", "Tyst läge starttid", "początek trybu cichego", "", "heure démarrage faible bruit") // TODO translate
MAKE_TRANSLATION(lowNoiseStop, "lownoisestop", "low noise stoptime", "Stopp geräuscharmer Betrieb", "", "Tyst läge stopptid", "koniec trybu cichego", "", "heure arrêt faible bruit") // TODO translate
MAKE_TRANSLATION(energyPriceGas, "energypricegas", "energy price gas", "Energiepreis Gas", "", "Gaspris", "cena energii z gazu", "", "prix énergie gaz") // TODO translate
MAKE_TRANSLATION(energyPriceEl, "energypriceel", "energy price electric", "Energiepreis Eletrizität", "", "Elpris", "cena energii elektrycznej", "", "prix énergie électrique") // TODO translate
MAKE_TRANSLATION(energyPricePV, "energyfeedpv", "feed in PV", "PV Einspeisevergütung", "", "PV Energi", "zasilanie energią PV", "", "alimentation PV") // TODO translate
MAKE_TRANSLATION(hybridDHW, "hybriddhw", "hybrid DHW", "Hybrid Warmwasser", "", "Hybridläge varmvatten", "hybrydowa c.w.u.", "", "ecs hybride") // TODO translate
MAKE_TRANSLATION(airPurgeMode, "airpurgemode", "air purge mode", "Luftspülung", "", "Luftreningsläge", "tryb oczyszczania powietrza", "", "mode purge air") // TODO translate
MAKE_TRANSLATION(heatPumpOutput, "heatpumpoutput", "heatpump output", "WP Leistung", "", "Värmepumpseffekt", "moc wyjściowa pompy ciepła", "", "sortie pompe à chaleur") // TODO translate
MAKE_TRANSLATION(coolingCircuit, "coolingcircuit", "cooling circuit", "Kühlkreislauf", "", "Kylkrets", "obwód chłodzący", "", "circuit refroidissement") // TODO translate
MAKE_TRANSLATION(compStartMod, "compstartmod", "compressor start modulation", "Kompressor Startleistung", "", "Kompressor startmodulering", "początkowa modulacja sprężarki", "", "modulation démarrage compresseur") // TODO translate
MAKE_TRANSLATION(heatDrainPan, "heatdrainpan", "heat drain pan", "Wärmeausgleichsgefäß", "", "Uppvärm. dränering", "zbiornik wyrównawczy ciepła", "", "bac récupération chaleur") // TODO translate
MAKE_TRANSLATION(heatCable, "heatcable", "heating cable", "Heizband", "heating cable", "värmekabel", "przewód grzejny", "", "câble chauffant") // TODO translate

// alternative heatsource AM200
MAKE_TRANSLATION(aCylTopTemp, "cyltoptemp", "cylinder top temperature", "Speichertemperatur Oben", "Buffer temperatuur boven", "Cylindertemperatur Toppen", "temperatura na górze cylindra", "beredertemperatur topp", "température haut cylindre")
MAKE_TRANSLATION(aCylCenterTemp, "cylcentertemp", "cylinder center temperature", "Speichertemperatur Mitte", "Buffer temperatuur midden", "Cylindertemperatur Mitten", "temperatura na środku cylindra", "beredertemperatur midten", "température centre cylindre")
MAKE_TRANSLATION(aCylBottomTemp, "cylbottomtemp", "cylinder bottom temperature", "Speichertemperatur Unten", "Buffer temperatuur onder", "Cylindertemperatur Botten", "temperatura na dole cylindra", "beredertemperatur nederst", "température fond cylindre")
MAKE_TRANSLATION(aFlowTemp, "altflowtemp", "alternative hs flow temperature", "Alternativer WE Vorlauftemperatur", "Alternatieve warmtebron aanvoertemperatuur", "Alternativ flödestemp värmekälla", "temperatura zasilania z alternatywnego źródła", "", "température flux hs alternative") // TODO translate
MAKE_TRANSLATION(aRetTemp, "altrettemp", "alternative hs return temperature", "Alternativer WE Rücklauftemperatur", "Alternatieve warmtebron retourtemperatuur", "Alternativ returtemp värmekälla", "temperatura powrotu z alternatywnego źródła", "", "température retour hs alternative") // TODO translate
MAKE_TRANSLATION(sysFlowTemp, "sysflowtemp", "system flow temperature", "System Vorlauftemperatur", "Systeem aanvoertemperatuur", "Systemflödestemperatur", "temperatura zasilania systemu", "", "température flux système") // TODO translate
MAKE_TRANSLATION(sysRetTemp, "sysrettemp", "system return temperature", "System Rücklauftemperatur", "Systeem retourtemperatuur", "Systemreturtemperatur", "temperatura powrotu z systemu", "", "température retour système") // TODO translate
MAKE_TRANSLATION(valveByPass, "valvebypass", "bypass valve", "Bypass-Ventil", "Bypass klep", "Bypassventil", "zawór obejścia", "", "vanne dérivation") // TODO translate
MAKE_TRANSLATION(valveBuffer, "valvebuffer", "buffer valve", "Puffer-Ventil", "Bufferklep", "Buffertventil", "zawór bufora", "", "vanne tampon") // TODO translate
MAKE_TRANSLATION(valveReturn, "valvereturn", "return valve", "Rückfluss-Ventil", "Retourklep", "Returventil", "zawór powrotu", "", "vanne retour") // TODO translate
MAKE_TRANSLATION(aPumpMod, "apumpmod", "alternative hs pump modulation", "Alternativer WE Pumpenmodulation", "Alternatieve warmtebron pomp modulatie", "Alternativ Pumpmodulering Värmekälla", "modulacja pompy alternatywnego źródła ciepła", "alternativ pumpemodulering varmekilde", "modulation alternative pompe hs")
MAKE_TRANSLATION(heatSource, "heatsource", "alternative heating active", "Alternativer Wärmeerzeuger aktiv", "Alternatieve warmtebron aktief", "Alternativ Värmekälla aktiv", "aktywne alternatywne źródło ciepła", "alternativ varmekilde aktiv", "chauffage alternatif actif")
MAKE_TRANSLATION(aPump, "apump", "alternative hs pump", "Alternativer WE Pumpe", "Alternatieve warmtebron pomp", "Alternativ Pump Värmekälla", "pompy alternatywnego źródła ciepła", "alternativ pumpe varmekilde", "alternative pompe hs")
MAKE_TRANSLATION(burner, "burner", "burner", "Brenner", "", "", "palnik", "", "") // TODO translate
MAKE_TRANSLATION(heatRequest, "heatrequest", "heat request", "Wärmeanforderung", "", "", "zapotrzebowanie na ciepło", "", "") // TODO translate
MAKE_TRANSLATION(blockRemain, "blockremain", "remaining blocktime", "verbleibende Blockzeit", "", "", "czas do końca blokady", "", "") // TODO translate
MAKE_TRANSLATION(blockRemainWw, "blockremainww", "remaining blocktime dhw", "verbleibende Blockzeit WW", "", "", "czas do końca blokady c.w.u.", "", "") // TODO translate
MAKE_TRANSLATION(flueGasTemp, "fluegastemp", "flue gas temperature", "Abgastemperatur", "", "", "temperatura spalin", "", "") // TODO translate

MAKE_TRANSLATION(vr2Config, "vr2config", "vr2 configuration", "VR2 Konfiguration", "VR2 configuratie", "VR2 Konfiguration", "konfiguracja VR2", "vr2 konfigurasjon", "configuration vr2")
MAKE_TRANSLATION(ahsActivated, "ahsactivated", "alternate heat source activation", "Alt. Wärmeerzeuger aktiviert", "Altenatieve warmtebron geactiveerd", "Alternativ värmekälla aktivering", "aktywacja alternatywnego źródła ciepła", "alternativ varmekilde aktivering", "activation source chaleur alternative")
MAKE_TRANSLATION(aPumpConfig, "apumpconfig", "primary pump config", "Konfig. Hauptpumpe", "Primaire pomp configuratie", "Konfiguration Primärpump", "konfiguracja pompy głównej", "konfiguration Primærpumpe", "configuration pompe primaire")
MAKE_TRANSLATION(aPumpSignal, "apumpsignal", "output for pr1 pump", "Ausgang Pumpe PR1", "Output voor pomp PR1", "Utgång från pump PR1", "wyjście pompy PR1", "utgang fra pumpe PR1", "sortie pompe pr1")
MAKE_TRANSLATION(aPumpMin, "apumpmin", "min output pump pr1", "Minimale Pumpenansteuerung", "Minimale output pomp PR1", "Min Output Pump PR1", "minimalne wysterowanie pompy PR1", "minimal output pumpe PR1", "sortie min pompe pr1")
MAKE_TRANSLATION(tempRise, "temprise", "ahs return temp rise", "Rücklauf Temperaturerhöhung", "Verhoging retourtemperatuur", "Förhöjd returtemperatur", "wzrost temperatury powrotu", "forhøyd returtemperatur", "augmentation température retour ahs")
MAKE_TRANSLATION(setReturnTemp, "setreturntemp", "set temp return", "Soll-Rücklauftemperatur", "Streeftemperatuur retour", "Vald returtemperatur", "zadana temperatura powrotu", "valgt returtemperatur", "régler température retour")
MAKE_TRANSLATION(mixRuntime, "mixruntime", "mixer run time", "Mischer-Laufzeit", "Mixer looptijd", "Blandningsventil drifttid", "czas pracy miksera", "blandingsventil drifttid", "durée fonctionnement mélangeur")
MAKE_TRANSLATION(bufBypass, "bufbypass", "buffer bypass config", "Puffer-Bypass Konfig.", "Buffer bypass configuratie", "Konfiguration Buffer bypass", "konfiguracja z obejściem bufora", "konfigurasjon buffer bypass", "configuration contournement buffer")
MAKE_TRANSLATION(bufMixRuntime, "bufmixruntime", "bypass mixer run time", "Speicher-Mischer-Laufzeit", "Buffer mixer looptijd", "Blandningsventil Bypass drifttid", "czas pracy mieszacza obejścia", "blandningsventil bypass drifttid", "durée fonctionnement contournement mélangeur")
MAKE_TRANSLATION(bufConfig, "bufconfig", "dhw buffer config", "Konfig. Warmwasserspeicher", "Warmwater boiler configuratie", "Konfiguration Varmvattentank", "konfiguracja bufora c.w.u.", "konfigurasjon varmvannstank", "configuration buffer ecs")
MAKE_TRANSLATION(blockMode, "blockmode", "config htg. blocking mode", "Konfig. Sperr-Modus", "Configuratie blokeermodus", "Konfiguration Blockeringsläge", "konfiguracja trybu blokady", "konfigurasjon blokkeringsmodus", "config mode blocage htg.")
MAKE_TRANSLATION(blockTerm, "blockterm", "config of block terminal", "Konfig. Sperrterminal", "Configuratie blookerterminal", "Konfiguration Blockeringsterminal", "konfiguracja terminala blokującego", "konfiguration blokkeringsterminal", "config. du bloque terminal")
MAKE_TRANSLATION(blockHyst, "blockhyst", "hyst. for boiler block", "Hysterese Sperrmodus", "Hysterese blokeerterminal", "Hysteres Blockeringsmodul", "tryb blokowania histerezy", "hystrese blokkeringsmodus", "hyst. Blocage chaudière")
MAKE_TRANSLATION(releaseWait, "releasewait", "boiler release wait time", "Wartezeit Kessel-Freigabe", "Wachttijd ketel vrijgave", "Väntetid Frisläppning", "czas oczekiwania na zwolnienie kotła", "kjele ventetid frigjøring", "temps attente libération chaudière")

// the following are dhw for the boiler and automatically tagged with 'dhw'
MAKE_TRANSLATION(wwSelTemp, "wwseltemp", "selected temperature", "gewählte Temperatur", "Geselecteerd temperatuur", "Vald Temperatur", "temperatura wyższa/komfort", "valgt temperatur", "température sélectionnée")
MAKE_TRANSLATION(wwSelTempLow, "wwseltemplow", "selected lower temperature", "untere Solltemperatur", "Onderste streeftemperatuur", "Vald lägstatemperatur", "temperatura niższa/eko", "valgt nedre temperatur", "température basse sélectionnée")
MAKE_TRANSLATION(wwSelTempOff, "wwseltempoff", "selected temperature for off", "Solltemperatur bei AUS", "Streeftemperatuur bij UIT", "Vald tempereatur för AV", "temperatura gdy grzanie wyłączone", "valgt tempereatur for av", "température sélectionnée pour arrêt")
MAKE_TRANSLATION(wwSelTempSingle, "wwseltempsingle", "single charge temperature", "Solltemperatur Einmalladung", "Streeftemperatuur enkele lading", "Temperatur Engångsladdning", "temperatura dodatkowej ciepłej wody", "temp engangsoppvarming", "température charge unique")
MAKE_TRANSLATION(wwCylMiddleTemp, "wwcylmiddletemp", "cylinder middle temperature (TS3)", "Speichertemperatur Mitte", "Buffer temperatuur midden", "Cylinder Temperatur Mitten (TS3)", "temperatura środka cylindra (TS3)", "vanntank midten temperatur (TS3)", "température moyenne ballon (TS3)")
MAKE_TRANSLATION(wwSetTemp, "wwsettemp", "set temperature", "Solltemperatur", "Streeftemperatuut", "Börtempertur", "temperatura zadana", "innstilt temperatur", "régler température")
MAKE_TRANSLATION(wwType, "wwtype", "type", "Typ", "type", "Typ", "typ", "type", "type")
MAKE_TRANSLATION(wwComfort, "wwcomfort", "comfort", "Komfort", "Comfort", "Komfort", "komfort", "komfort", "confort")
MAKE_TRANSLATION(wwComfort1, "wwcomfort1", "comfort mode", "Komfort-Modus", "Comfort modus", "Komfortläge", "tryb komfortu", "komfort modus", "mode confort")
MAKE_TRANSLATION(wwFlowTempOffset, "wwflowtempoffset", "flow temperature offset", "Vorlauftemperaturanhebung", "Aanvoertemperatuur offset", "Flödestemperatur förskjutning", "korekta temperatury wypływu", "flyttemperaturforskyvning", "offset température flux")
MAKE_TRANSLATION(wwMaxPower, "wwmaxpower", "max power", "max Leistung", "Maximaal vermogen", "Max Effekt", "moc maksymalna", "maks effekt", "puissance max")
MAKE_TRANSLATION(wwCircPump, "wwcircpump", "circulation pump available", "Zirkulationspumpe vorhanden", "Circulatiepomp aanwezig", "Cirkulationspump tillgänglig", "pompa cyrkulacji zainstalowana", "sirkulasjonspumpe tilgjengelig", "pompe circulation disponible")
MAKE_TRANSLATION(wwChargeType, "wwchargetype", "charging type", "Speicher-Ladungstyp", "Buffer laadtype", "Laddningstyp", "sposób grzania zasobnika", "varmetype", "type chargement")
MAKE_TRANSLATION(wwDisinfectionTemp, "wwdisinfectiontemp", "disinfection temperature", "Desinfektionstemperatur", "Desinfectietemperatuur", "Desinfektionstemperatur", "temperatura dezynfekcji termicznej", "", "température désinfection") // TODO translate
MAKE_TRANSLATION(wwCircMode, "wwcircmode", "circulation pump mode", "Zirkulationspumpen-Modus", "Modus circulatiepomp", "Läge Cirkulationspump", "tryb pracy cyrkulacji", "modus sikulasjonspumpe", "mode pompe circulation")
MAKE_TRANSLATION(wwCirc, "wwcirc", "circulation active", "Zirkulation aktiv", "Circulatiepomp actief", "Cirkulation aktiv", "pompa cyrkulacji", "sirkulasjon aktiv", "circulation active")
MAKE_TRANSLATION(wwCurTemp, "wwcurtemp", "current intern temperature", "aktuelle interne Temperatur", "Huidige interne temperatuur", "Intern Temperatur", "temperatura zasobnika", "gjeldende intern temperatur", "température interne actuelle")
MAKE_TRANSLATION(wwCurTemp2, "wwcurtemp2", "current extern temperature", "aktuelle externe Temperatur", "Huidige externe temperatuur", "Extern Temperatur", "temperatura wypływu", "gjeldende ekstern temperaur", "température externe actuelle")
MAKE_TRANSLATION(wwCurFlow, "wwcurflow", "current tap water flow", "aktueller Durchfluss", "Hudige warmwater doorstroming", "Aktuellt tappvattenflöde", "aktualny przepływ", "gjeldende strømningshastighet", "débit actuel eau robinet")
MAKE_TRANSLATION(wwStorageTemp1, "wwstoragetemp1", "storage intern temperature", "interne Speichertemperatur", "Interne buffertemperatuur", "Beredare Intern Temperatur", "temperatura wewnątrz zasobnika", "intern temperatur bereder", "température interne stockage")
MAKE_TRANSLATION(wwStorageTemp2, "wwstoragetemp2", "storage extern temperature", "externer Speichertemperatur", "Externe buffertemperatuur", "Beredare Extern Tempereatur", "temperatura na wyjściu zasobnika", "ekstern temperatur bereder", "température externe stockage")
MAKE_TRANSLATION(wwActivated, "wwactivated", "activated", "aktiviert", "geactiveerd", "Aktiverad", "system przygotowywania c.w.u.", "aktivert", "activé")
MAKE_TRANSLATION(wwOneTime, "wwonetime", "one time charging", "Einmalladung", "Buffer eenmalig laden", "Engångsladdning", "jednorazowa dodatkowa ciepła woda", "", "charge unique") // TODO translate
MAKE_TRANSLATION(wwDisinfecting, "wwdisinfecting", "disinfecting", "Desinfizieren", "Desinfectie", "Desinficerar", "dezynfekcja termiczna", "desinfiserer", "désinfection")
MAKE_TRANSLATION(wwCharging, "wwcharging", "charging", "Laden", "Laden", "Värmer", "grzanie", "varmer", "chargement")
MAKE_TRANSLATION(wwChargeOptimization, "wwchargeoptimization", "charge optimization", "Ladungsoptimierung", "laadoptimalisatie", "Laddningsoptimering", "optymalizacja grzania", "oppvarmingsoptimalisering", "optimisation charge")
MAKE_TRANSLATION(wwRecharging, "wwrecharging", "recharging", "Nachladen", "herladen", "Laddar om", "ponowne grzanie", "varm på nytt", "en recharge")
MAKE_TRANSLATION(wwTempOK, "wwtempok", "temperature ok", "Temperatur ok", "Temperatuur OK", "Temperatur OK", "temperatura OK", "temperatur ok!", "température ok")
MAKE_TRANSLATION(wwActive, "wwactive", "active", "aktiv", "Actief", "Aktiv", "aktywna", "aktiv", "actif")
MAKE_TRANSLATION(ww3wayValve, "ww3wayvalve", "3-way valve active", "3-Wegeventil aktiv", "3-wegklep actief", "Trevägsventil aktiv", "zawór 3-drogowy aktywny", "aktiv trevisventil", "vanne 3 voies active")
MAKE_TRANSLATION(wwSetPumpPower, "wwsetpumppower", "set pump power", "Soll Pumpenleistung", "Streefwaarde pompvermogen", "Vald pumpeffekt", "ustawione wysterowanie pompy", "valgt pumpeeffekt", "régler puissance pompe")
MAKE_TRANSLATION(wwMixerTemp, "wwmixertemp", "mixer temperature", "Mischertemperatur", "Mixertemperatuur", "Blandningsventil-tempertur", "temperatura mieszacza", "temperatur blandeventil", "température mélangeur")
MAKE_TRANSLATION(wwStarts, "wwstarts", "starts", "Anzahl Starts", "Aantal starts", "Antal starter", "liczba załączeń", "antall starter", "démarrages")
MAKE_TRANSLATION(wwStarts2, "wwstarts2", "control starts2", "Kreis 2 Anzahl Starts", "Aantal starts circuit 2", "Antal starter Krets 2", "liczba załączeń 2", "antall starter krets 2", "démarrages contrôle 2")
MAKE_TRANSLATION(wwWorkM, "wwworkm", "active time", "aktive Zeit", "Actieve tijd", "Aktiv Tid", "czas aktywności", "driftstid", "temps actif")
MAKE_TRANSLATION(wwHystOn, "wwhyston", "hysteresis on temperature", "Einschalttemperaturdifferenz", "Inschakeltemperatuurverschil", "Hysteres PÅ-temperatur", "histereza załączania", "innkoblingstemperaturforskjell", "hystérésis température allumage")
MAKE_TRANSLATION(wwHystOff, "wwhystoff", "hysteresis off temperature", "Ausschalttemperaturdifferenz", "Uitschakeltemperatuurverschil", "Hysteres AV-temperatur", "histereza wyłączania", "utkoblingstemperaturforskjell", "hystérésis température extinction")
MAKE_TRANSLATION(wwProgMode, "wwprogmode", "program", "Programmmodus", "Programma", "Program", "program", "program", "programme")
MAKE_TRANSLATION(wwCircProg, "wwcircprog", "circulation program", "Zirkulationsprogramm", "Circulatieprogramma", "Cirkulationsprogram", "program cyrkulacji c.w.u.", "sirkulationsprogram", "programme circulation")
MAKE_TRANSLATION(wwMaxTemp, "wwmaxtemp", "maximum temperature", "Maximale Temperatur", "Maximale temperatuur", "Maximal Temperatur", "temperatura maksymalna", "", "température max") // TODO translate
MAKE_TRANSLATION(wwOneTimeKey, "wwonetimekey", "one time key function", "Einmalladungstaste", "Knop voor eenmalig laden buffer", "Engångsfunktion", "przycisk jednorazowego ogrzania", "engangsknapp varme", "fonction touche unique")
MAKE_TRANSLATION(wwSolarTemp, "wwsolartemp", "solar boiler temperature", "Solarboiler Temperatur", "Zonneboiler temperatuur", "Solpanel Temp", "temperatura zasobnika solarnego", "", "température chaudière solaire") // TODO translate

// mqtt values / commands
MAKE_TRANSLATION(switchtime, "switchtime", "program switchtime", "Programm Schaltzeit", "Programma schakeltijd", "Program Bytestid", "program czasowy", "programbyttetid", "heure commutation programme")
MAKE_TRANSLATION(switchtime1, "switchtime1", "own1 program switchtime", "Programm 1 Schaltzeit", "Schakeltijd programma 1", "Program 1 Bytestid", "program przełączania 1", "byttetidprogram 1", "heure de commutation programme 1")
MAKE_TRANSLATION(switchtime2, "switchtime2", "own2 program switchtime", "Programm 2 Schaltzeit", "Schakeltijd programma 2", "Program 2 Bytestid", "program przełączania 2", "byttetid program 2", "heure de changement programme 2")
MAKE_TRANSLATION(wwswitchtime, "wwswitchtime", "program switchtime", "Programm Schaltzeit", "Warm water programma schakeltijd", "Varmvattenprogram Bytestid", "program czasowy", "byttetid varmtvannsprogram", "heure commutation programme")
MAKE_TRANSLATION(wwcircswitchtime, "wwcircswitchtime", "circulation program switchtime", "Zirculationsprogramm Schaltzeit", "Schakeltijd circulatieprogramma", "Cirkulationsprogram Bytestid", "program cyrkulacji", "byttetid sirkulasjonsprogram", "heure commutation programme circulation")
MAKE_TRANSLATION(dateTime, "datetime", "date/time", "Datum/Zeit", "Datum/Tijd", "Datum/Tid", "data i godzina", "dato/tid", "date/heure")
MAKE_TRANSLATION(errorCode, "errorcode", "error code", "Fehlernummer", "Foutmeldingscode", "Felkod", "kod błędu", "feikode", "code erreur")
MAKE_TRANSLATION(ibaMainDisplay, "display", "display", "Anzeige", "Display", "Display", "wyświetlacz", "skjerm", "affichage")
MAKE_TRANSLATION(ibaLanguage, "language", "language", "Sprache", "Taal", "Sprak", "język", "språk", "langue")
MAKE_TRANSLATION(ibaClockOffset, "clockoffset", "clock offset", "Uhrkorrektur", "Klokcorrectie", "Tidskorrigering", "korekta zegara", "tidskorrigering", "offset horloge")
MAKE_TRANSLATION(ibaBuildingType, "building", "building type", "Gebäudetyp", "", "Byggnadstyp", "typ budynku", "bygningstype", "type bâtiment") // TODO translate
MAKE_TRANSLATION(heatingPID, "heatingpid", "heating PID", "Heizungs-PID", "", "Uppvärmning PID", "PID ogrzewania", "oppvarmings PID", "PID chauffage") // TODO translate
MAKE_TRANSLATION(ibaCalIntTemperature, "intoffset", "internal temperature offset", "Korrektur interner Temperatur", "", "Korrigering interntemperatur", "korekta temperatury w pomieszczeniu", "Korrigering interntemperatur", "offset température interne") // TODO translate
MAKE_TRANSLATION(ibaMinExtTemperature, "minexttemp", "minimal external temperature", "min. Aussentemperatur", "", "Min Extern Temperatur", "minimalna miejscowa temperatura zewnętrzna", "minimal eksterntemperatur", "température extérieure minimale") // TODO translate
MAKE_TRANSLATION(backlight, "backlight", "key backlight", "Gegenlicht", "Toetsverlichting", "Bakgrundsbelysning", "podświetlenie klawiatury", "bakgrunnsbelysning", "rétroéclairage touches")
MAKE_TRANSLATION(damping, "damping", "damping outdoor temperature", "Dämpfung der Außentemperatur", "Demping buitentemperatuur", "Utomhustemperatur dämpning", "tłumienie temperatury zewnętrznej", "demping av utetemperatur", "température extérieure minimale")
MAKE_TRANSLATION(tempsensor1, "inttemp1", "temperature sensor 1", "Temperatursensor 1", "Temperatuursensor 1", "Temperatursensor 1", "czujnik temperatury 1", "temperatursensor 1", "sonde température 1")
MAKE_TRANSLATION(tempsensor2, "inttemp2", "temperature sensor 2", "Temperatursensor 2", "Temperatuursensor 2", "Temperatursensor 2", "czujnik temperatury 2", "temperatursensor 2", "capteur température 2")
MAKE_TRANSLATION(dampedoutdoortemp, "dampedoutdoortemp", "damped outdoor temperature", "gedämpfte Außentemperatur", "Gedempte buitentemperatuur", "Utomhustemperatur dämpad", "tłumiona temperatura zewnętrzna", "dempet utetemperatur", "température extérieure amortie")
MAKE_TRANSLATION(floordrystatus, "floordry", "floor drying", "Estrichtrocknung", "Vloerdroogprogramma", "Golvtorkning", "suszenie jastrychu", "gulvtørkeprogram", "séchage sol")
MAKE_TRANSLATION(floordrytemp, "floordrytemp", "floor drying temperature", "Estrichtrocknungs Temperatur", "Temperatuur vloerdroogprogramma", "Golvtorkning Temperatur", "temperatura suszenia jastrychu", "gulvtørketemperatur", "température séchage sol")
MAKE_TRANSLATION(brightness, "brightness", "screen brightness", "Bildschirmhelligkeit", "Schermhelderheid", "Ljusstyrka", "jasność", "lysstyrke", "luminosité écran")
MAKE_TRANSLATION(autodst, "autodst", "automatic change daylight saving time", "automatische Sommerzeit Umstellung", "Automatische omschakeling zomer-wintertijd", "Automatisk växling sommar/vinter-tid", "automatycznie przełączaj na czas letni/zimowy", "automatisk skifte av sommer/vinter-tid", "changement automatique heure d'été")
MAKE_TRANSLATION(preheating, "preheating", "preheating in the clock program", "Vorheizen im Zeitprogramm", "Voorverwarming in het klokprogramma", "Förvärmning i tidsprogram", "podgrzewanie w programie czasowym", "forvarming i tidsprogram", "préchauffage dans programme horloge")
MAKE_TRANSLATION(offtemp, "offtemp", "temperature when mode is off", "Temperatur bei AUS", "Temperatuur bij UIT", "Temperatur Avslagen", "temperatura w trybie \"wył.\"", "temperatur avslått", "température lorsque mode désactivé")
MAKE_TRANSLATION(mixingvalves, "mixingvalves", "mixing valves", "Mischventile", "Mengkleppen", "Blandningsventiler", "zawory mieszające", "blandeventiler", "vannes mélange")
MAKE_TRANSLATION(pvEnableWw, "pvenableww", "enable raise dhw", "aktiviere Anhebung WW", "", "", "podwyższenie c.w.u. z PV", "", "") // TODO translate
MAKE_TRANSLATION(pvRaiseHeat, "pvraiseheat", "raise heating with PV", "Anhebung Heizen mit PV", "", "", "podwyższenie grzania z PV", "", "") // TODO translate
MAKE_TRANSLATION(pvLowerCool, "pvlowercool", "lower cooling with PV", "Kühlabsenkung mit PV", "", "", "obniżenie chłodzenia z PV", "", "") // TODO translate

// thermostat ww
MAKE_TRANSLATION(wwMode, "wwmode", "mode", "Modus", "Modus", "Läge", "tryb pracy", "modus", "mode")
MAKE_TRANSLATION(wwSetTempLow, "wwsettemplow", "set low temperature", "untere Solltemperatur", "Onderste streeftemperatuur", "Nedre Börvärde", "zadana temperatura obniżona", "nedre settverdi", "réglage température basse")
MAKE_TRANSLATION(wwWhenModeOff, "wwwhenmodeoff", "when thermostat mode off", "bei Thermostatmodus AUS", "Als Thermostaat op UIT", "när Termostatläge är AV", "gdy wyłączono na termostacie", "når modus er av", "lorsque mode thermostat off")
MAKE_TRANSLATION(wwExtra1, "wwextra1", "circuit 1 extra", "Kreis 1 Extra", "Circuit 1 extra", "Krets 1 Extra", "obieg dodatkowy 1", "ekstra krets 1", "circuit 1 extra")
MAKE_TRANSLATION(wwExtra2, "wwextra2", "circuit 2 extra", "Kreis 2 Extra", "Circuit 2 extra", "Kets 2 Extra", "obieg dodatkowy 2", "ekstra krets 2", "circuit 2 extra")
MAKE_TRANSLATION(wwCharge, "wwcharge", "charge", "Laden", "Laden", "Ladda", "grzanie", "lade", "charge")
MAKE_TRANSLATION(wwChargeDuration, "wwchargeduration", "charge duration", "Ladedauer", "Laadtijd", "Laddtid", "czas grzania dodatkowej ciepłej wody", "ladetid", "durée charge")
MAKE_TRANSLATION(wwDisinfect, "wwdisinfect", "disinfection", "Desinfektion", "Desinfectie", "Desinfektion", "dezynfekcja termiczna", "desinfeksjon", "désinfection")
MAKE_TRANSLATION(wwDisinfectDay, "wwdisinfectday", "disinfection day", "Desinfektionstag", "Desinfectiedag", "Desinfektionsdag", "dzień dezynfekcji termicznej", "desinfeksjonsdag", "jour désinfection")
MAKE_TRANSLATION(wwDisinfectHour, "wwdisinfecthour", "disinfection hour", "Desinfektionsstunde", "Desinfectieuur", "Desinfektionstimme", "godzina dezynfekcji termicznej", "desinfeksjonstime", "heure désinfection")
MAKE_TRANSLATION(wwDisinfectTime, "wwdisinfecttime", "disinfection time", "Desinfektionsdauer", "Desinfectietijd", "Desinfektionstid", "maksymalny czas trwania dezynfekcji termicznej", "desinfeksjonstid", "durée désinfection")
MAKE_TRANSLATION(wwDailyHeating, "wwdailyheating", "daily heating", "täglich Heizen", "Dagelijks opwarmen", "Daglig Uppvärmning", "codzienne nagrzewanie", "", "chauffage quotidien") // TODO translate
MAKE_TRANSLATION(wwDailyHeatTime, "wwdailyheattime", "daily heating time", "tägliche Heizzeit", "Tijd dagelijkse opwarming", "Daglig Uppvärmningstid", "czas trwania codziennego nagrzewania", "daglig oppvarmingstid", "heure chauffage quotidien")

// thermostat hc
MAKE_TRANSLATION(selRoomTemp, "seltemp", "selected room temperature", "Sollwert Raumtemperatur", "Streeftemperatuur kamer", "Vald Rumstemperatur", "zadana temperatura w pomieszczeniu", "valgt rumstemperatur", "température ambiante sélectionnée")
MAKE_TRANSLATION(roomTemp, "currtemp", "current room temperature", "aktuelle Raumtemperatur", "Huidige kamertemperatuur", "Aktuell Rumstemperatur", "temperatura w pomieszczeniu", "gjeldende romstemperatur", "température ambiante actuelle")
MAKE_TRANSLATION(mode, "mode", "mode", "Modus", "Modus", "Läge", "sposób sterowania", "modus", "mode")
MAKE_TRANSLATION(modetype, "modetype", "mode type", "Modus Typ", "Type modus", "Typ av läge", "aktualny tryb pracy", "modusrype", "type mode")
MAKE_TRANSLATION(fastheatup, "fastheatup", "fast heatup", "schnelles Aufheizen", "Snel opwarmen", "Snabb Uppvärmning", "szybkie nagrzewanie", "rask oppvarming", "chauffage rapide")
MAKE_TRANSLATION(daytemp, "daytemp", "day temperature", "Tagestemperatur", "temperatuur dag", "Dagstemperatur", "temperatura w dzień", "dagtemperatur", "température jour")
MAKE_TRANSLATION(daylowtemp, "daytemp2", "day temperature T2", "Tagestemperatur T2", "Temperatuur dag T2", "Dagstemperatur T2", "temperatura w dzień T2", "dagtemperatur T2", "température jour T2")
MAKE_TRANSLATION(daymidtemp, "daytemp3", "day temperature T3", "Tagestemperatur T3", "Temperatuur dag T3", "Dagstemperatur T3", "temperatura w dzień T3", "dagtemperatur T3", "température jour T3")
MAKE_TRANSLATION(dayhightemp, "daytemp4", "day temperature T4", "Tagestemperatur T4", "Temperatuur dag T4", "Dagstemperatur T4", "temperatura w dzień T4", "dagtemperatur T4", "température jour T4")
MAKE_TRANSLATION(heattemp, "heattemp", "heat temperature", "Heizen Temperatur", "Temperatuur verwarming", "Temperatur Uppvärmning", "temperatura ogrzewania", "oppvarmingstemperatur", "température chauffage")
MAKE_TRANSLATION(nighttemp, "nighttemp", "night temperature", "Nachttemperatur", "Nachttemperatuur", "Nattemperatur", "temperatura w nocy", "nattemperatur", "température de nuit")
MAKE_TRANSLATION(nighttemp2, "nighttemp", "night temperature T1", "Nachttemperatur T1", "Nachttemperatuur T1", "Nattemperatur T1", "temperatura w nocy T1", "nattemperatur T1", "température nuit T1")
MAKE_TRANSLATION(ecotemp, "ecotemp", "eco temperature", "eco Temperatur", "Temperatuur eco", "Eko-temperatur", "temperatura w trybie eko", "øko temperatur", "température éco")
MAKE_TRANSLATION(manualtemp, "manualtemp", "manual temperature", "manuelle Temperatur", "Temperatuur handmatig", "Temperatur Manuell", "temperatura ustawiona ręcznie", "manuell temperatur", "température manuelle")
MAKE_TRANSLATION(tempautotemp, "tempautotemp", "temporary set temperature automode", "temporäre Solltemperatur", "Streeftemperatuur automodus tijdelijk", "Temporär Aktivering av Auto-läge", "zadana temperatura w pomieszczenia w trybie \"auto\" (tymczasowa)", "temporær valgt temp i automodus", "température temporaire mode automatique")
MAKE_TRANSLATION(remoteseltemp, "remoteseltemp", "temporary set temperature from remote", "temporäre Solltemperatur Remote", "Temperatuur van afstandsbedieding", "Temperatur från fjärruppkoppling", "zadana zdalnie temperatura a pomieszczeniu (tymczasowa)", "temporær valgt temp fra fjernbetjening", "température temporaire depuis télécommande")
MAKE_TRANSLATION(comforttemp, "comforttemp", "comfort temperature", "Komforttemperatur", "Comforttemperatuur", "Komforttemperatur", "temperatura w trybie komfort", "komforttemperatur", "température confort")
MAKE_TRANSLATION(summertemp, "summertemp", "summer temperature", "Sommertemperatur", "Zomertemperatuur", "Sommartemperatur", "temperatura przełączania lato/zima", "Sommertemperatur", "température été")
MAKE_TRANSLATION(designtemp, "designtemp", "design temperature", "Auslegungstemperatur", "Ontwerptemperatuur", "Design-temperatur", "temperatura projektowa", "designtemperatur", "température conception")
MAKE_TRANSLATION(offsettemp, "offsettemp", "offset temperature", "Temperaturanhebung", "Temperatuur offset", "Temperaturkorrigering", "korekta temperatury", "temperaturkorrigering", "température offset")
MAKE_TRANSLATION(minflowtemp, "minflowtemp", "min flow temperature", "min Vorlauftemperatur", "", "Min Flödestemperatur", "minimalna temperatura zasilania", "min turtemperatur", "température min flux") // TODO translate
MAKE_TRANSLATION(maxflowtemp, "maxflowtemp", "max flow temperature", "max Vorlauftemperatur", "", "Max Flödestemperatur", "maksymalna temperatura zasilania", "maks turtemperatur", "température max flux") // TODO translate
MAKE_TRANSLATION(roominfluence, "roominfluence", "room influence", "Raumeinfluss", "Ruimteinvloed", "Rumspåverkan", "wpływ pomieszczenia", "rominnflytelse", "influence pièce")
MAKE_TRANSLATION(roominfl_factor, "roominflfactor", "room influence factor", "Raumeinflussfaktor", "Factor ruimteinvloed", "Rumspåverkansfaktor", "współczynnik wpływu pomieszczenia", "rominnflytelsesfaktor", "facteur d'influence pièce")
MAKE_TRANSLATION(curroominfl, "curroominfl", "current room influence", "aktueller Raumeinfluss", "Huidige ruimteinvloed", "Aktuell Rumspåverkan", "aktualny wpływ pomieszczenia", "gjeldende rominnflytelse", "influence actuelle pièce")
MAKE_TRANSLATION(nofrosttemp, "nofrosttemp", "nofrost temperature", "Frostschutztemperatur", "Temperatuur vorstbeveiliging", "Temperatur Frostskydd", "temperatura ochrony przed zamarzaniem", "frostbeskyttelsestemperatur", "température protection gel")
MAKE_TRANSLATION(targetflowtemp, "targetflowtemp", "target flow temperature", "berechnete Vorlauftemperatur", "Berekende aanvoertemperatuur", "Målvärde Flödestemperatur", "zadana temperatura zasilania", "målverdi turtemperatur", "température cible flux")
MAKE_TRANSLATION(heatingtype, "heatingtype", "heating type", "Heizungstyp", "Verwarmingstype", "Värmesystem", "system grzewczy", "varmesystem", "type chauffage")
MAKE_TRANSLATION(summersetmode, "summersetmode", "set summer mode", "Einstellung Sommerbetrieb", "Instelling zomerbedrijf", "Aktivera Sommarläge", "tryb lato/zima", "aktiver sommermodus", "activer mode été")
MAKE_TRANSLATION(hpoperatingmode, "hpoperatingmode", "heatpump operating mode", "Wärmepumpe Betriebsmodus", "Bedrijfsmodus warmtepomp", "Värmepump Driftläge", "tryb pracy pompy ciepła", "driftsmodus varmepumpe", "mode fonctionnement pompe à chaleur")
MAKE_TRANSLATION(hpoperatingstate, "hpoperatingstate", "heatpump operating state", "WP Arbeitsweise", "Huidige modus warmtepomp", "Värmepump driftläge", "aktualny tryb pracy pompy ciepła", "driftstatus varmepumpe", "état fonctionnement pompe à chaleur")
MAKE_TRANSLATION(controlmode, "controlmode", "control mode", "Kontrollmodus", "Comtrolemodus", "Kontrolläge", "tryb sterowania", "kontrollmodus", "mode régulation")
MAKE_TRANSLATION(control, "control", "control device", "Fernsteuerung", "Afstandsbedieding", "Kontrollenhet", "sterownik", "kontrollenhet", "dispositif régulation")
MAKE_TRANSLATION(roomsensor, "roomsensor", "room sensor", "Raumsensor", "Ruimtesensor", "Rumssensor", "czujnik temperatury pomieszczenia", "romsensor", "capteur pièce")
MAKE_TRANSLATION(program, "program", "program", "Programm", "Programma", "Program", "program", "program", "programme")
MAKE_TRANSLATION(pause, "pause", "pause time", "Pausenzeit", "Pausetijd", "Paustid", "czas przerwy", "pausetid", "temps de pause")
MAKE_TRANSLATION(party, "party", "party time", "Partyzeit", "Partytijd", "Partytid", "czas przyjęcia", "partytid", "temps de fête")
MAKE_TRANSLATION(holidaytemp, "holidaytemp", "holiday temperature", "Urlaubstemperatur", "Vakantietemperatuur", "Helgtemperatur", "temperatura w trybie urlopowym", "ferietemperatur", "température vacances")
MAKE_TRANSLATION(summermode, "summermode", "summer mode", "Sommerbetrieb", "Zomerbedrijf", "Sommarläge", "aktualny tryb lato/zima", "sommermodus", "mode été")
MAKE_TRANSLATION(holidaymode, "holidaymode", "holiday mode", "Urlaubsbetrieb", "Vakantiebedrijf", "Helgläge", "tryb urlopowy", "feriemodus", "mode vacances")
MAKE_TRANSLATION(flowtempoffset, "flowtempoffset", "flow temperature offset for mixer", "Vorlauftemperaturanhebung", "Mixer aanvoertemperatuur offset", "Temperaturkorrigering Flödestemp. Blandningsventil", "korekta temperatury przepływu dla miksera", "temperaturkorrigering av blandingsventil", "décalage température de bascule pour mélangeur")
MAKE_TRANSLATION(reducemode, "reducemode", "reduce mode", "Absenkmodus", "Gereduceerde modus", "Reducerat Läge", "tryb zredukowany/obniżony", "", "mode réduction") // TODO translate
MAKE_TRANSLATION(noreducetemp, "noreducetemp", "no reduce below temperature", "Durchheizen unter", "Reduceermodus onderbreken onder", "Inaktivera reducering under", "bez redukcji poniżej temperatury", "inaktiver redusert nedre temp", "pas de réduction en dessous température")
MAKE_TRANSLATION(reducetemp, "reducetemp", "off/reduce switch temperature", "Absenkmodus unter", "Onderste afschakeltemperatuur", "Avslag/Reducera under", "tryb zredukowany poniżej temperatury", "nedre avstengningstemperatur", "arrêt/réduction température bascule")
MAKE_TRANSLATION(vacreducetemp, "vacreducetemp", "vacations off/reduce switch temperature", "Urlaub Absenkmodus unter", "Vakantiemodus onderste afschakeltemperatuur", "Helg Avslag/Reducering under", "tryb urlopowy poniżej temperatury", "feriemodus nedre utkoblingstemperatur", "vacances – arrêt/réduction température bascule")
MAKE_TRANSLATION(vacreducemode, "vacreducemode", "vacations reduce mode", "Urlaub Absenkmodus", "Vakantie afschakelmodus", "Helg reduceringsläge", "redukcja w trakcie urlopu", "ferieavstengningsmodus", "mode réduction vacances")
MAKE_TRANSLATION(nofrostmode, "nofrostmode", "nofrost mode", "Frostschutz Modus", "Vorstbeveiligingsmodus", "Frostskyddsläge", "temperatura wiodąca dla ochrony przed zamarzaniem", "frostbeskyttelsesmodus", "mode protection gel")
MAKE_TRANSLATION(remotetemp, "remotetemp", "room temperature from remote", "Raumtemperatur Remote", "Ruimtetemperatuur van afstandsbediening", "Rumstemperatur från fjärr", "temperatura w pomieszczeniu (z termostatu)", "romstemperatur fra fjernbetjening", "température pièce depuis télécommande")
MAKE_TRANSLATION(wwHolidays, "wwholidays", "holiday dates", "Feiertage", "Feestdagen", "Helgdagar", "dni świąteczne", "feriedager varmtvann", "dates vacances")
MAKE_TRANSLATION(wwVacations, "wwvacations", "vacation dates", "Urlaubstage", "Vakantiedagen", "Semesterdatum Varmvatten", "dni urlopowe", "ferie dato varmtvann", "dates vacances")
MAKE_TRANSLATION(holidays, "holidays", "holiday dates", "Feiertage", "Feestdagen", "Helgdatum", "święta", "helligdager", "dates vacances")
MAKE_TRANSLATION(vacations, "vacations", "vacation dates", "Urlaubstage", "Vakantiedagen", "Semesterdatum", "urlop", "feriedager", "dates vacances")
MAKE_TRANSLATION(wwprio, "wwprio", "dhw priority", "WW-Vorrang", "Prioriteit warm water", "Prioritera Varmvatten", "priorytet dla c.w.u.", "prioroter varmtvann", "priorité ecs")
MAKE_TRANSLATION(nofrostmode1, "nofrostmode1", "nofrost mode", "Frostschutz", "Vorstbeveiligingsmodus", "Frostskyddsläge", "ochrona przed zamarzaniem", "frostbeskyttelse", "mode protection gel")
MAKE_TRANSLATION(reducehours, "reducehours", "duration for nighttemp", "Dauer Nachttemp.", "Duur nachtverlaging", "Timmar Nattsänkning", "czas trwania trybu nocnego", "timer nattsenkning", "durée température nuit")
MAKE_TRANSLATION(reduceminutes, "reduceminutes", "remaining time for nightmode", "Restzeit Nachttemp.", "Resterende tijd nachtverlaging", "Återstående Tid Nattläge", "czas do końca trybu nocnego", "gjenværende tid i nattstilling", "temps restant mode nuit")
MAKE_TRANSLATION(switchonoptimization, "switchonoptimization", "switch-on optimization", "Einschaltoptimierung", "Inschakeloptimalisering", "Växlingsoptimering", "optymalizacja załączania", "slå på optimalisering", "optimisation mise en marche")

// heatpump
MAKE_TRANSLATION(airHumidity, "airhumidity", "relative air humidity", "relative Luftfeuchte", "Relatieve luchtvochtigheid", "Relativ Luftfuktighet", "wilgotność względna w pomieszczeniu", "luftfuktighet", "humidité relative air")
MAKE_TRANSLATION(dewTemperature, "dewtemperature", "dew point temperature", "Taupunkttemperatur", "Dauwpunttemperatuur", "Daggpunkt", "punkt rosy w pomieszczeniu", "duggtemperatur", "température point rosée")

// mixer
MAKE_TRANSLATION(flowSetTemp, "flowsettemp", "setpoint flow temperature", "Sollwert Vorlauftemperatur", "Streefwaarde aanvoertemperatuur", "Vald flödestemperatur", "zadana temperatura zasilania", "valgt turtemperatur", "consigne température flux")
MAKE_TRANSLATION(flowTempHc, "flowtemphc", "flow temperature (TC1)", "Vorlauftemperatur HK (TC1)", "Aanvoertemperatuut circuit (TC1)", "Flödestemperatur (TC1)", "temperatura zasilania (TC1)", "turtemperatur (TC1)", "température flux (TC1)")
MAKE_TRANSLATION(pumpStatus, "pumpstatus", "pump status (PC1)", "Pumpenstatus HK (PC1)", "pompstatus circuit (PC1)", "Pumpstatus (PC1)", "status pompy (PC1)", "pumpestatus (PC1)", "état pompe (PC1)")
MAKE_TRANSLATION(mixerStatus, "valvestatus", "mixing valve actuator (VC1)", "Mischerventil Position (VC1)", "positie mixerklep (VC1)", "Shuntventil Status (VC1)", "siłownik zaworu mieszającego (VC1)", "shuntventil status (VC1)", "actionnement vanne mélangeur (VC1)")
MAKE_TRANSLATION(flowTempVf, "flowtempvf", "flow temperature in header (T0/Vf)", "Vorlauftemperatur am Verteiler (T0/Vf)", "aanvoertemperatuur verdeler (T0/Vf)", "Flödestemperatur Fördelare (T0/Vf)", "temperatura zasilania na rozdzielaczu (T0/Vf)", "turtemperatur ved fordeleren (T0/Vf)", "température départ collecteur (T0/Vf)")
MAKE_TRANSLATION(mixerSetTime, "valvesettime", "time to set valve", "Zeit zum Einstellen des Ventils", "Inschakeltijd mengklep", "Inställningstid Ventil", "czas na ustawienie zaworu", "instillningstid ventil", "délai activation vanne")
// mixer prefixed with wwc
MAKE_TRANSLATION(wwPumpStatus, "pumpstatus", "pump status in assigned wwc (PC1)", "Pumpenstatus des wwk (PC1)", "Pompstatus in WW circuit (PC1)", "Pumpstatus i VV-krets (PC1)", "stan pompy w obwodzie c.w.u. (PC1)", "Pumpestatus i VV-krets (PC1)", "état pompe wwc (PC1)", "Kullanım suyu devresindeki(PC1) pompa durumu")
MAKE_TRANSLATION(wwTempStatus, "wwtempstatus", "temperature switch in assigned wwc (MC1)", "Temperaturschalter des wwk (MC1)", "Temperatuurschakeling in WW circuit (MC1)", "Temperaturventil i VV-krets (MC1)", "temperatura w obwodzie c.w.u. (MC1)", "temperaturventil i VV-krets (MC1)", "température bascule wwc (MC1).")
MAKE_TRANSLATION(wwTemp, "wwtemp", "current temperature", "aktuelle Temperatur", "huidige temperatuur", "Aktuell Temperatur", "temperatura c.w.u.", "aktuell temperatur", "température actuelle")
// mixer pool
MAKE_TRANSLATION(poolSetTemp, "poolsettemp", "pool set temperature", "Pool Solltemperatur", "Streeftemperatuur zwembad", "Pool Temperatur Börvärde", "zadana temperatura basenu", "valgt temp basseng", "température consigne piscine")
MAKE_TRANSLATION(poolTemp, "pooltemp", "pool temperature", "Pool Temperatur", "Zwembadtemperatuur", "Pooltemperatur", "temperatura basenu", "bassengtemperatur", "température piscine")
MAKE_TRANSLATION(poolShuntStatus, "poolshuntstatus", "pool shunt status opening/closing", "Pool Ventil öffnen/schließen", "Zwembadklep status openen/sluiten", "Pool Shunt-status öppnen/stängd", "status bocznika basenu", "bassengshunt-status åpen/stengt", "état shunt piscine ouvert/fermé")
MAKE_TRANSLATION(poolShunt, "poolshunt", "pool shunt open/close (0% = pool / 100% = heat)", "Pool Ventil Öffnung", "Mengklep zwembad stand", "Pool Shunt Öppen/Stängd", "bocznik basenu (0% = basen / 100% = grzanie)", "bassengshunt åpen/stengt (0% = basseng / 100% = varme)", "ouverture/fermeture shunt piscine (0% = piscine / 100% = chaleur).")
MAKE_TRANSLATION(hydrTemp, "hydrTemp", "hydraulic header temperature", "Verteilertemperatur", "Temperatuur open verdeler", "Fördelartemperatur", "temperatura kolektora hydraulicznego", "Fordelertemperatur", "température collecteur hydraulique")

// solar
MAKE_TRANSLATION(cylMiddleTemp, "cylmiddletemp", "cylinder middle temperature (TS3)", "Speichertemperatur Mitte (TS3)", "Zonneboilertemperatuur midden (TS3)", "Cylindertemperatur Mitten (TS3)", "temperatura w środku zasobnika (TS3)", "beredertemperatur i midten (TS3)", "température moyenne cylindre (TS3)")
MAKE_TRANSLATION(retHeatAssist, "retheatassist", "return temperature heat assistance (TS4)", "Rücklaufanhebungs-Temp. (TS4)", "Retourtemperatuur verwarmingsassistentie (TS4)", "Returtemperatur värmestöd (TS4)", "temperatura powrotu wspomagania grzania (TS4)", "returtemperatur varmestøtte (TS4)", "température retour de assistance thermique (TS4)")
MAKE_TRANSLATION(m1Valve, "heatassistvalve", "heat assistance valve (M1)", "Ventil Heizungsunterstützung (M1)", "Klep verwarmingsassistentie (M1)", "Uppvärmningsstöd Ventil (M1)", "zawór wspomagania grzania (M1)", "varmehjelpsventil (M1)", "vanne assistance thermique (M1)")
MAKE_TRANSLATION(m1Power, "heatassistpower", "heat assistance valve power (M1)", "Ventilleistung Heizungsunterstützung (M1)", "Vermogen klep verwarmingsassistentie (M1)", "Uppvärmningsstöd Ventil Effekt (M1)", "moc zaworu wspomagania grzania (M1)", "varmehjelpsventileffekt (M1)", "puissance vanne assistance thermique (M1)")
MAKE_TRANSLATION(pumpMinMod, "pumpminmod", "minimum pump modulation", "minimale Pumpenmodulation", "Minimale pompmodulatie", "Min Pumpmodulering", "minimalna modulacja pompy", "minimum pumpmodulering", "modulation minimale pompe")
MAKE_TRANSLATION(maxFlow, "maxflow", "maximum solar flow", "maximaler Durchfluss", "Maximale doorstroom solar", "Max Flöde Solpanel", "maksymalny przepływ solarów", "maks strømming solpanel ", "débit solaire maximum")
MAKE_TRANSLATION(solarPower, "solarpower", "actual solar power", "aktuelle Solarleistung", "Huidig solar vermogen", "Aktuellt Sol-effekt", "aktualna moc solarów", "aktuell soleffekt", "puissance solaire réelle")
MAKE_TRANSLATION(solarPumpTurnonDiff, "turnondiff", "pump turn on difference", "Einschalthysterese Pumpe", "Inschakelhysterese pomp", "Aktiveringshysteres Pump", "histereza załączenia pompy", "slå på hysteresepumpe", "différence activation pompe")
MAKE_TRANSLATION(solarPumpTurnoffDiff, "turnoffdiff", "pump turn off difference", "Ausschalthysterese Pumpe", "Uitschakelhysterese pomp", "Avslagshysteres Pump", "histereza włączenia pompy", "slå av hysteresepumpe", "différence arrêt pompe")
MAKE_TRANSLATION(pump2MinMod, "pump2minmod", "minimum pump 2 modulation", "minimale Modulation Pumpe 2", "Minimale modulatie pomp 2", "Min Modulering Pump 2", "minimalna modulacja pompy 2", "minimum pumpmodulering 2", "modulation minimale pompe 2")
MAKE_TRANSLATION(solarPump2TurnonDiff, "turnondiff2", "pump 2 turn on difference", "Einschalthysterese Pumpe 2", "Inschakelhysterese pomp 2", "Aktiveringshysteres Pump 2", "histereza załączenia pompy 2", "slå på hysteresepumpe 2", "différence activation pompe 2")
MAKE_TRANSLATION(solarPump2TurnoffDiff, "turnoffdiff2", "pump 2 turn off difference", "Ausschalthysterese Pumpe 2", "Uitschakelhysterese pomp 2", "Avslagshysteres Pump 2", "histereza wyłączenia pompy 2", "slå av hysteresepumpe 2", "différence arrêt pompe 2")
MAKE_TRANSLATION(collectorTemp, "collectortemp", "collector temperature (TS1)", "Kollektortemperatur (TS1)", "Collectortemperatuur (TS1)", "Kollektor Temperatur (TS1)", "temperatura kolektora (TS1)", "kollektor temperatur (TS1)", "température collecteur (TS1)")
MAKE_TRANSLATION(collector2Temp, "collector2temp", "collector 2 temperature (TS7)", "Kollector 2 Temperatur (TS7)", "Collector 2 temperatuur (TS7)", "Kollektor 2 Temperatur (TS7)", "temperatura kolektora 2 (TS7)", "kollektor 2 temperatur (TS7)", "température collecteur 2 (TS7)")
MAKE_TRANSLATION(cylBottomTemp, "cylbottomtemp", "cylinder bottom temperature (TS2)", "Speicher Bodentemperatur (TS2)", "Bodemtemperatuur zonneboiler (TS2)", "Cylindertemperatur Botten (TS2)", "temperatura na spodzie zasobnika (TS2)", "beredertemp i bunn (TS2)", "température fond de cylindre (TS2)")
MAKE_TRANSLATION(cyl2BottomTemp, "cyl2bottomtemp", "second cylinder bottom temperature (TS5)", "2. Speicher Bodentemperatur (TS5)", "Bodemtemperatuur 2e boiler", "Sekundär Cylindertemperatur Botten (TS5)", "temperatura na spodzie drugiego zasobnika (TS5)", "skundær beredertemp i bunn (TS5)", "température fond de cylindre (TS5)")
MAKE_TRANSLATION(heatExchangerTemp, "heatexchangertemp", "heat exchanger temperature (TS6)", "wärmetauscher Temperatur (TS6)", "Temperatuur warmtewisselaar (TS6)", "Värmeväxlare Temperatur (TS6)", "temperatura wymiennika ciepła (TS6)", "", "température échangeur de chaleur (TS6)") // TODO translate
MAKE_TRANSLATION(collectorMaxTemp, "collectormaxtemp", "maximum collector temperature", "maximale Kollektortemperatur", "Maximale collectortemperatuur", "Max Kollektortemperatur", "maksymalna temperatura kolektora", "maks kollektortemperatur", "température max. collecteur")
MAKE_TRANSLATION(collectorMinTemp, "collectormintemp", "minimum collector temperature", "minimale Kollektortemperatur", "Minimale collectortemperatuur", "Min Kollektortemperatur", "minimalna temperatura kolektora", "min kollektortemperatur", "température min. collecteur")
MAKE_TRANSLATION(cylMaxTemp, "cylmaxtemp", "maximum cylinder temperature", "maximale Speichertemperatur", "maximale temperatuur zonneboiler", "Max Cylindertemperatur", "maksymalna temperatura zasobnika", "maks beredertemperatur", "température max. cylindre")
MAKE_TRANSLATION(solarPumpMod, "solarpumpmod", "pump modulation (PS1)", "Pumpenmodulation (PS1)", "Pompmodulatie (PS1)", "Pumpmodulering (PS1)", "modulacja pompy solarnej (PS1)", "solpumpmodulering (PS1)", "modulation pompe (PS1)")
MAKE_TRANSLATION(cylPumpMod, "cylpumpmod", "cylinder pump modulation (PS5)", "Speicherpumpenmodulation (PS5)", "Modulatie zonneboilerpomp (PS5)", "Cylinderpumpmodulering (PS5)", "modulacja pompy zasobnika (PS5)", "", "modulation pompe cylindre (PS5)") // TODO translate
MAKE_TRANSLATION(solarPump, "solarpump", "pump (PS1)", "Pumpe (PS1)", "Pomp (PS1)", "Pump (PS1)", "pompa solarna (PS1)", "solpumpe (PS1)", "pompe solaire (PS1)")
MAKE_TRANSLATION(solarPump2, "solarpump2", "pump 2 (PS4)", "Pumpe 2 (PS4)", "Pomp 2 (PS4)", "Pump 2 (PS4)", "pompa solarna 2 (PS4)", "solpumpe 2 (PS4)", "pompe 2 (PS4)")
MAKE_TRANSLATION(solarPump2Mod, "solarpump2mod", "pump 2 modulation (PS4)", "Pumpe 2 Modulation (PS4)", "Modulatie pomp 2 (PS4)", "Pump 2 Modulering (PS4)", "modulacja pompy solarnej 2 (PS4)", "solpumpe2modulering (PS4)", "modulation pompe solaire 2 (PS4)")
MAKE_TRANSLATION(valveStatus, "valvestatus", "valve status", "Ventilstatus", "Klepstatus", "Ventilstatus", "stan zaworu", "ventilstatus", "statut valve")
MAKE_TRANSLATION(vs1Status, "vs1status", "valve status VS1", "Ventilstatus VS1", "Klepstatus VS1", "Ventilstatus VS1", "stan zaworu VS1", "ventilstatus VS1", "statut valve VS1")
MAKE_TRANSLATION(cylHeated, "cylheated", "cyl heated", "Speichertemperatur erreicht", "Boilertemperatuur behaald", "Värmepanna Uppvärmd", "zasobnik został nagrzany", "bereder oppvarmt", "cylindre chauffé")
MAKE_TRANSLATION(collectorShutdown, "collectorshutdown", "collector shutdown", "Kollektorabschaltung", "Collector afschakeling", "Kollektor Avstängning", "wyłączenie kolektora", "kollektor stengt", "arrêt collecteur")
MAKE_TRANSLATION(pumpWorkTime, "pumpworktime", "pump working time", "Pumpenlaufzeit", "Pomplooptijd", "Pump Drifttid", "czas pracy pompy", "driftstid pumpe", "durée fonctionnement pompe")
MAKE_TRANSLATION(pump2WorkTime, "pump2worktime", "pump 2 working time", "Pumpe 2 Laufzeit", "Looptijd pomp 2", "Pump 2 Drifttid", "czas pracy pompy 2", "driftstid pumpe2", "durée fonctionnement pompe 2")
MAKE_TRANSLATION(m1WorkTime, "m1worktime", "differential control working time", "Differenzregelung Arbeitszeit", "Verschilregeling arbeidstijd", "Differentialreglering Drifttid", "czas pracy regulacji różnicowej", "differentialreguleringssrifttid", "durée fonctionnement contrôle différentiel")
MAKE_TRANSLATION(energyLastHour, "energylasthour", "energy last hour", "Energie letzte Std", "Energie laatste uur", "Energi Senaste Timmen", "energia w ciągu ostatniej godziny", "energi siste time", "énergie dernière heure")
MAKE_TRANSLATION(energyTotal, "energytotal", "total energy", "Gesamtenergie", "Totale energie", "Total Energi", "energia całkowita", "total energi", "énergie totale") 
MAKE_TRANSLATION(energyToday, "energytoday", "total energy today", "Energie heute", "Energie vandaag", "Total Energi Idag", "energia całkowita dzisiaj", "total energi i dag", "énergie totale aujourd'hui")

// solar ww
MAKE_TRANSLATION(wwTemp1, "wwtemp1", "temperature 1", "Temperatur 1", "Temperatuur 1", "Temperatur 1", "temperatura 1", "temperatur 1", "température 1")
MAKE_TRANSLATION(wwTemp3, "wwtemp3", "temperature 3", "Temperatur 3", "Temperatuur 2", "Temperatur 2", "temperatura 2", "Temperatur 3", "température 3")
MAKE_TRANSLATION(wwTemp4, "wwtemp4", "temperature 4", "Temperatur 4", "Temperatuur 3", "Temperatur 3", "temperatura 3", "Temperatur 4", "température 4")
MAKE_TRANSLATION(wwTemp5, "wwtemp5", "temperature 5", "Temperatur 5", "Temperatuur 5", "Temperatur 4", "temperatura 4", "Temperatur 5", "température 5")
MAKE_TRANSLATION(wwTemp7, "wwtemp7", "temperature 7", "Temperatur 7", "Temperatuur 7", "Temperatur 5", "temperatura 5", "Temperatur 7", "température 7")
MAKE_TRANSLATION(wwPump, "wwpump", "pump", "Pumpe", "Pomp", "Pump", "pompa", "", "pompe") // TODO translate
// solar ww and mixer wwc
MAKE_TRANSLATION(wwMinTemp, "wwmintemp", "minimum temperature", "minimale Temperatur", "Minimale temperatuur", "Min Temperatur", "temperatura minimalna", "min temperatur", "température min")
MAKE_TRANSLATION(wwRedTemp, "wwredtemp", "reduced temperature", "reduzierte Temperatur", "Gereduceerde temperatuur", "Reducerad Temperatur", "temperatura zredukowana", "reducert temperatur", "température réduite")
MAKE_TRANSLATION(wwDailyTemp, "wwdailytemp", "daily temperature", "tägl. Temperatur", "Dagelijkse temperatuur", "Daglig temperatur", "temperatura dzienna", "dagtemperatur", "température en journée")
MAKE_TRANSLATION(wwKeepWarm, "wwkeepwarm", "keep warm", "Warmhalten", "Warm houde", "Varmhållning", "utrzymywanie ciepła", "holde varmen", "maintenir chaleur")
MAKE_TRANSLATION(wwStatus2, "wwstatus2", "status 2", "Status 2", "Status 2", "Status 2", "status 2", "status 2", "statut 2")
MAKE_TRANSLATION(wwPumpMod, "wwpumpmod", "pump modulation", "Pumpen Modulation", "Pompmodulatie", "Pumpmodulering", "modulacja pompy", "pumpemodulering", "modulation de pompe")
MAKE_TRANSLATION(wwFlow, "wwflow", "flow rate", "Volumenstrom", "Doorstroomsnelheid", "Flöde", "przepływ", "strømningshastighet", "débit")
// extra mixer ww
MAKE_TRANSLATION(wwRequiredTemp, "wwrequiredtemp", "required temperature", "benötigte Temperatur", "Benodigde temperatuur", "Nödvändig Temperatur", "temperatura wymagana", "nødvendig temperatur", "température requise")
MAKE_TRANSLATION(wwDiffTemp, "wwdifftemp", "start differential temperature", "Start Differential Temperatur", "Start differentiele temperatuur", "Start Differentialtemperatur", "start temperatury różnicowej", "start differensialtemperatur", "température différentielle de départ")

// SM100
MAKE_TRANSLATION(heatTransferSystem, "heattransfersystem", "heattransfer system", "Wärmeübertragungs-System", "Warmteoverdrachtssysteem", "Värmeöverföringssystem", "system wymiany ciepła", "varmeoverføringssystem", "système de transfert de chaleur")
MAKE_TRANSLATION(externalCyl, "externalcyl", "external cylinder", "Externer Speicher", "Externe boiler", "Extern Cylinder", "zbiornik zewnętrzny", "ekstern bereder", "cylindre externe")
MAKE_TRANSLATION(thermalDisinfect, "thermaldisinfect", "thermal disinfection", "Thermische Desinfektion", "Thermische desinfectie", "Termisk Desinfektion", "dezynfekcja termiczna", "termisk desinfeksjon", "désinfection thermique")
MAKE_TRANSLATION(heatMetering, "heatmetering", "heatmetering", "Wärmemessung", "warmtemeting", "Värmemätning", "pomiar ciepła", "varmemåling", "mesure de chaleur")
MAKE_TRANSLATION(solarIsEnabled, "solarenabled", "solarmodule enabled", "Solarmodul aktiviert", "Solarmodule geactiveerd", "Solmodul Aktiverad", "system solarny", "solmodul aktivert", "module solaire activé")

// telegram 0x035A
MAKE_TRANSLATION(solarPumpMode, "solarpumpmode", "pump mode", "Solar Pumpen Einst.", "Modus zonneboilerpomp", "Sol Pumpläge", "tryb pracy pompy", "solpumpemodus", "mode pompe solaire")
MAKE_TRANSLATION(solarPumpKick, "pumpkick", "pump kick", "Röhrenkollektorfunktion", "Modus buizencollector", "Sol Kollektorfunktion", "wspomaganie startu pompy", "solkllektorfunksjon", "démarrage boost pompe solaire")
MAKE_TRANSLATION(plainWaterMode, "plainwatermode", "plain water mode", "Südeuropafunktion", "Modus Zuid-Europa", "Sydeuropa-funktion", "tylko woda (dla Europy Południowej)", "vanlig vannmodus", "mode eau ordinaire")
MAKE_TRANSLATION(doubleMatchFlow, "doublematchflow", "doublematchflow", "Double Match Flow", "Double Match Flow", "Dubbelmatchning Flöde", "przepływ podwójnie dopasowany", "dobbelmatch flow", "double match flow")
MAKE_TRANSLATION(solarPump2Mode, "pump2mode", "pump 2 mode", "Pumpe 2 Modus", "Modus pomp 2", "Pump 2 Läge", "tryb pracy pompy 2", "pump 2 modus", "mode pompe 2")
MAKE_TRANSLATION(solarPump2Kick, "pump2kick", "pump kick 2", "Pumpe 2 Startboost", "Startboost pomp 2", "Pump 2 Kollektorfunktion", "wspomaganie startu pompy 2", "startboost pumpe 2", "démarrage boost pompe 2")

// telegram 0x035F
MAKE_TRANSLATION(cylPriority, "cylpriority", "cylinder priority", "Speicher Priorität", "Prioriteit boiler", "Cylinderprioritering", "priorytet cylindra", "berederprioritering", "priorité de cylindre")

// telegram 0x380
MAKE_TRANSLATION(climateZone, "climatezone", "climate zone", "Klimazone", "klimaatzone", "Klimatzon", "strefa klimatyczna", "klimasone", "zone de climat")
MAKE_TRANSLATION(collector1Area, "collector1area", "collector 1 area", "Kollektor 1 Fläche", "oppervlakte collector 1", "Kollektor 1 Area", "powierzchnia kolektora 1", "kollektor 1 område", "zone collecteur 1")
MAKE_TRANSLATION(collector1Type, "collector1type", "collector 1 type", "Kollektor 1 Typ", "Type collector 1", "Kollektor 1 Typ", "typ kolektora 1", "kollektor 1 type", "type collecteur 1")
MAKE_TRANSLATION(collector2Area, "collector2area", "collector 2 area", "Kollektor 2 Fläche", "Oppervlakte collector 2", "Kollektor 2 Area", "powierzchnia kolektora 2", "kollektor 2 område", "zone collecteur 2")
MAKE_TRANSLATION(collector2Type, "collector2type", "collector 2 type", "Kollektor 2 Typ", "Type collector 2", "Kollektor 2 Typ", "typ kolektora 2", "kollektor 2 type", "type collecteur 2")

// telegram 0x0363 heatCounter
MAKE_TRANSLATION(heatCntFlowTemp, "heatcntflowtemp", "heat counter flow temperature", "Wärmezähler Vorlauf-Temperatur", "Aanvoertemperatuur warmteenergiemeter", "Värmeräknare Flödestemperatur", "temperatura zasilania ciepłomierza", "varmeenergimåler turtemperatur", "température flux compteur chaleur")
MAKE_TRANSLATION(heatCntRetTemp, "heatcntrettemp", "heat counter return temperature", "Wärmezähler Rücklauf-Temperatur", "Retourtemperatuur warmteenergiemeter", "Värmeräknare Returtemperatur", "temperatura powrotu ciepłomierza", "varmeenergimåler returtemperatur", "température retour compteur chaleur")
MAKE_TRANSLATION(heatCnt, "heatcnt", "heat counter impulses", "Wärmezähler Impulse", "Warmteenergiemeter pulsen", "Värmeräknare Impuls", "liczba impulsów ciepłomierza", "varmemåler impuls", "impulsions compteur chaleur")
MAKE_TRANSLATION(swapFlowTemp, "swapflowtemp", "swap flow temperature (TS14)", "Austausch Vorlauf-Temperatur (TS14)", "Aanvoertemperatuur verwisselaar (TS14)", "Växlingstemperatur Flöde (TS14)", "temperatura zasilania wymiennika", "veksler turledningstemperatur (TS14)", "température flux échangeur (TS14)")
MAKE_TRANSLATION(swapRetTemp, "swaprettemp", "swap return temperature (TS15)", "Austausch Rücklauf-Temperatur (TS15)", "Retourtemperatuur verwisselaar (TS15)", "Växlingstemperatur Returflöde (TS15)", "temperatura powrotu wymiennika", "veksler returledningstemperatur (TS15)", "température retour échangeur (TS15)")

// switch
MAKE_TRANSLATION(activated, "activated", "activated", "Aktiviert", "Geactiveerd", "Aktiverad", "aktywowany", "aktivert", "activé")
MAKE_TRANSLATION(status, "status", "status", "Status", "Status", "Status", "status", "status", "statut")

// RF sensor, id 0x40, telegram 0x435
MAKE_TRANSLATION(RFTemp, "rftemp", "RF room temperature sensor", "RF Raumtemperatur Sensor", "RF ruimtetemperatuur sensor", "RF Rumsgivare Temp", "bezprzewodowy czujnik temperatury pomieszczenia", "RF romsgiver temp", "capteur de température de pièce RF")

/*
// unknown fields to track (SM10), only for testing
// **** NO TRANSLATION NEEDED ****
MAKE_TRANSLATION(data11, "data11", "unknown datafield 11")
MAKE_TRANSLATION(data12, "data12", "unknown datafield 12")
MAKE_TRANSLATION(data8, "data8", "unknown datafield 8")
MAKE_TRANSLATION(data0, "data0", "unknown datafield 0")
MAKE_TRANSLATION(data1, "data1", "unknown datafield 1")
MAKE_TRANSLATION(setting3, "setting3", "unknown setting 3")
MAKE_TRANSLATION(setting4, "setting4", "unknown setting 4")
*/

// clang-format on