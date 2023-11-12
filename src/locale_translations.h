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
#define EMSESP_LOCALE_IT "it"

// IMPORTANT! translations are in the order:,en, de, nl, sv, pl, no, fr, tr, it
//
// if there is no translation, it will default to en
//

// device types, as display in Web and Console
MAKE_WORD_TRANSLATION(boiler_device, "Boiler", "Kessel", "CV ketel", "Värmepanna", "Kocioł", "Varmekjele", "", "Kazan", "Caldaia") // TODO translate
MAKE_WORD_TRANSLATION(thermostat_device, "Thermostat", "Thermostat", "Thermostaat",  "Termostat", "Termostat", "Termostat", "", "Termostat", "Termostato") // TODO translate
MAKE_WORD_TRANSLATION(heatpump_device, "Heat Pump", "Wärmepumpe", "Warmtepomp",  "Värmepump", "Pompa ciepła", "Varmepumpe", "", "Isı Pompası", "Pompa di Calore") // TODO translate
MAKE_WORD_TRANSLATION(solar_device, "Solar Module", "Solarmodul", "Solar Module",  "Solmodul", "Moduł solarny", "Solmodul", "", "Güneş Enerjisi Cihazı", "Modulo Solare") // TODO translate
MAKE_WORD_TRANSLATION(connect_device, "Connect Module", "Verbindungsmodul", "Connect Module",  "Uppkopplingsmodul", "Moduł przyłączeń", "Sammenkoblingsmodul", "", "Güneş Enerjisi Cihazı", "Modulo connessione") // TODO translate
MAKE_WORD_TRANSLATION(mixer_device, "Mixer Module", "Mischermodul", "Mixer Module",  "Blandningsmodul", "Moduł mieszacza", "Miksermodul", "", "Karışım Cihazı", "Modulo Miscela") // TODO translate
MAKE_WORD_TRANSLATION(controller_device, "Controller Module", "Kontrollmodul", "Controller Module",  "Styrmodul", "Moduł sterujący", "Styremodul", "", "Kontrol Ünitesi", "Modulo Controllo") // TODO translate
MAKE_WORD_TRANSLATION(switch_device, "Switch Module", "Schaltmodul", "Switch Module",  "Relämodul", "Moduł przełączający", "Switch modul", "", "Anahtar", "Modulo Switch") // TODO translate
MAKE_WORD_TRANSLATION(gateway_device, "Gateway Module", "Gateway Modul", "Gateway Module",  "Gateway", "Moduł IP", "Gateway", "", "Ağ Geçidi", "Modulo Gateway") // TODO translate
MAKE_WORD_TRANSLATION(alert_device, "Alert Module", "Alarmmodul", "Alert Module",  "Larmmodul", "Moduł alarmowy", "Alarmmodul", "", "Alarm Cihazı", "Module Avviso") // TODO translate
MAKE_WORD_TRANSLATION(pump_device, "Pump Module", "Pumpenmodul", "Pump Module",  "Pumpmodul", "Moduł pompy", "Pumpemodul", "", "Pompa", "Module Pompa") // TODO translate
MAKE_WORD_TRANSLATION(heatsource_device, "Heatsource", "Heizquelle", "Heatsource",  "Värmekälla", "Źródło ciepła", "Varmekilde", "", "Isı Kaynağı", "Fonte di calore") // TODO translate
MAKE_WORD_TRANSLATION(sensors_device, "Sensors", "Sensoren", "Sensoren", "Sensorer", "Czujniki", "Sensorer", "Capteurs", "Sensör Cihazı", "Sensori")
MAKE_WORD_TRANSLATION(unknown_device, "Unknown", "Unbekannt", "Onbekend", "Okänt", "Nieznane urządzenie", "Ukjent", "Inconnu", "Bilinmeyen", "Sconosciuto")
MAKE_WORD_TRANSLATION(custom_device, "Custom", "Nutzerdefiniert", "Aangepast", "", "Niestandardowe", "", "", "Özel", "Personalizzato") // TODO translate
MAKE_WORD_TRANSLATION(custom_device_name, "User defined entities", "Nutzer deklarierte Entitäten", "Gebruiker gedefineerd", "", "Encje zdefiniowane przez użytkownika", "", "", "Kullanıcı tarafından tanımlanmış varlıklar", "Entità definita da utente") // TODO translate
MAKE_WORD_TRANSLATION(ventilation_device, "Ventilation", "Lüftung", "Ventilatie", "", "", "", "", "Havalandırma", "Ventilazione") // TODO translate

// commands
MAKE_WORD_TRANSLATION(info_cmd, "lists all values", "Liste aller Werte", "lijst van alle waardes", "", "wyświetl wszystkie wartości", "Viser alle verdier", "", "Tüm değerleri listele", "elenca tutti i valori") // TODO translate
MAKE_WORD_TRANSLATION(commands_cmd, "lists all commands", "Liste aller Kommandos", "lijst van alle commando's", "", "wyświetl wszystkie komendy", "Viser alle kommandoer", "", "Tüm komutları listele", "elencaa tutti i comandi") // TODO translate
MAKE_WORD_TRANSLATION(entities_cmd, "lists all entities", "Liste aller Entitäten", "lijst van alle entiteiten", "", "wyświetl wszsytkie encje", "Viser alle enheter", "", "Tüm varlıkları listele", "elenca tutte le entità") // TODO translate
MAKE_WORD_TRANSLATION(send_cmd, "send a telegram", "Sende EMS-Telegramm", "stuur een telegram", "", "wyślij telegram", "send et telegram", "", "Bir telegram gönder", "invia un telegramma") // TODO translate
MAKE_WORD_TRANSLATION(setiovalue_cmd, "set io value", "Setze Wertevorgabe", "instellen standaardwaarde", "", "ustaw wartość", "sett en io verdi", "", "Giriş/Çıkış değerlerini ayarla", "imposta valore io") // TODO translate
MAKE_WORD_TRANSLATION(changeloglevel_cmd, "change log level", "Ändere Sysloglevel", "aanpassen log niveau", "", "zmień poziom log-u", "endre loggnivå", "", "Kayıt seviyesini değiştir", "cambia livello registrazione") // TODO translate
MAKE_WORD_TRANSLATION(fetch_cmd, "refresh all EMS values", "Lese alle EMS-Werte neu", "Verversen alle EMS waardes", "", "odśwież wszystkie wartości EMS", "oppfrisk alle EMS verdier", "", "Bütün EMS değerlerini yenile", "aggiornare tutti i valori EMS") // TODO translate
MAKE_WORD_TRANSLATION(restart_cmd, "restart EMS-ESP", "Neustart", "opnieuw opstarten", "", "uruchom ponownie EMS-ESP", "restart EMS-ESP", "redémarrer EMS-ESP", "EMS-ESPyi yeniden başlat", "riavvia EMS-ESP") // TODO translate
MAKE_WORD_TRANSLATION(watch_cmd, "watch incoming telegrams", "Watch auf eingehende Telegramme", "inkomende telegrammen bekijken", "", "obserwuj przyczodzące telegramy", "se innkommende telegrammer", "", "Gelen telegramları ", "guardare i telegrammi in arrivo") // TODO translate
MAKE_WORD_TRANSLATION(publish_cmd, "publish all to MQTT", "Publiziere MQTT", "publiceer alles naar MQTT", "", "opublikuj wszystko na MQTT", "Publiser alt til MQTT", "", "Hepsini MQTTye gönder", "pubblica tutto su MQTT") // TODO translate
MAKE_WORD_TRANSLATION(system_info_cmd, "show system status", "Zeige System-Status", "toon systeemstatus", "", "pokaż status systemu", "vis system status", "", "Sistem Durumunu Göster", "visualizza stati di sistema") // TODO translate
MAKE_WORD_TRANSLATION(schedule_cmd, "enable schedule item", "Aktiviere Zeitplan", "activeer tijdschema item", "", "aktywuj wybrany harmonogram", "", "", "program öğesini etkinleştir", "abilitare l'elemento programmato") // TODO translate
MAKE_WORD_TRANSLATION(entity_cmd, "set custom value on EMS", "Sende eigene Entitäten zu EMS", "verstuur custom waarde naar EMS", "", "wyślij własną wartość na EMS", "", "", "emp üzerinde özel değer ayarla", "imposta valori personalizzati su EMS") // TODO translate
MAKE_WORD_TRANSLATION(commands_response, "get response","Hole Antwort","Verzoek om antwoord", "", "", "", "gelen cevap", "") // TODO translate
MAKE_WORD_TRANSLATION(coldshot_cmd, "send a cold shot of water", "", "", "", "", "", "", "soğuk su gönder", "") // TODO translate
MAKE_WORD_TRANSLATION(values_cmd, "show all values", "", "", "", "", "", "", "", "") // TODO translate

// tags
MAKE_WORD_TRANSLATION(tag_boiler_data_ww, "dhw", "WW", "dhw", "VV", "CWU", "dhw", "ecs", "SKS", "dhw")
MAKE_WORD_TRANSLATION(tag_device_data_ww, "dhw", "WW", "dhw", "VV", "CWU", "dhw", "ecs", "SKS", "dhw")
MAKE_WORD_TRANSLATION(tag_hc1, "hc1", "HK1", "hc1", "VK1", "OG1", "hc1", "hc1", "ID1", "hc1")
MAKE_WORD_TRANSLATION(tag_hc2, "hc2", "HK2", "hc2", "VK2", "OG2", "hc2", "hc2", "ID2", "hc2")
MAKE_WORD_TRANSLATION(tag_hc3, "hc3", "HK3", "hc3", "VK3", "OG3", "hc3", "hc3", "ID3", "hc3")
MAKE_WORD_TRANSLATION(tag_hc4, "hc4", "HK4", "hc4", "VK4", "OG4", "hc4", "hc4", "ID4", "hc4")
MAKE_WORD_TRANSLATION(tag_hc5, "hc5", "HK5", "hc5", "VK5", "OG5", "hc5", "hc5", "ID5", "hc5")
MAKE_WORD_TRANSLATION(tag_hc6, "hc6", "HK6", "hc6", "vk6", "OG6", "hc6", "hc6", "ID6", "hc6")
MAKE_WORD_TRANSLATION(tag_hc7, "hc7", "HK7", "hc7", "VK7", "OG7", "hc7", "hc7", "ID7", "hc7")
MAKE_WORD_TRANSLATION(tag_hc8, "hc8", "HK8", "hc8", "VK8", "OG8", "hc8", "hc8", "ID8", "hc8")
MAKE_WORD_TRANSLATION(tag_wwc1, "wwc1", "WWK1", "wwc1", "VVK1", "CWU1", "wwc1", "wwc1", "SKS1", "wwc1")
MAKE_WORD_TRANSLATION(tag_wwc2, "wwc2", "WWK2", "wwc2", "VVK2", "CWU2", "wwc2", "wwc2", "SKS2", "wwc2")
MAKE_WORD_TRANSLATION(tag_wwc3, "wwc3", "WWK3", "wwc3", "VVK3", "CWU3", "wwc3", "wwc3", "SKS3", "wwc3")
MAKE_WORD_TRANSLATION(tag_wwc4, "wwc4", "WWK4", "wwc4", "VVK4", "CWU4", "wwc4", "wwc4", "SKS4", "wwc4")
MAKE_WORD_TRANSLATION(tag_wwc5, "wwc5", "WWK5", "wwc5", "VVK5", "CWU5", "wwc5", "wwc5", "SKS5", "wwc5")
MAKE_WORD_TRANSLATION(tag_wwc6, "wwc6", "WWK6", "wwc6", "VVK6", "CWU6", "wwc6", "wwc6", "SKS6", "wwc6")
MAKE_WORD_TRANSLATION(tag_wwc7, "wwc7", "WWK7", "wwc7", "VVK7", "CWU7", "wwc7", "wwc7", "SKS7", "wwc7")
MAKE_WORD_TRANSLATION(tag_wwc8, "wwc8", "WWK8", "wwc8", "VVK8", "CWU8", "wwc8", "wwc8", "SKS8", "wwc8")
MAKE_WORD_TRANSLATION(tag_wwc9, "wwc9", "WWK9", "wwc9", "VVK9", "CWU9", "wwc9", "wwc9", "SKS9", "wwc9")
MAKE_WORD_TRANSLATION(tag_wwc10, "wwc10", "WWK10", "wwc10", "VVK10", "CWU10", "wwc10", "wwc10", "SKS10", "wwc10")
MAKE_WORD_TRANSLATION(tag_ahs1, "ahs1", "AHQ1", "ahs1", "AVK1", "AŹC1", "ahs1", "ahs1", "ahs1", "ahs1")
MAKE_WORD_TRANSLATION(tag_hs1, "hs1", "hs1", "hs1", "VK1", "ŹC1", "hs1", "hs1", "hs1", "hs1")
MAKE_WORD_TRANSLATION(tag_hs2, "hs2", "hs2", "hs2", "VK2", "ŹC2", "hs2", "hs2", "hs2", "hs2")
MAKE_WORD_TRANSLATION(tag_hs3, "hs3", "hs3", "hs3", "VK3", "ŹC3", "hs3", "hs3", "hs3", "hs3")
MAKE_WORD_TRANSLATION(tag_hs4, "hs4", "hs4", "hs4", "VK4", "ŹC4", "hs4", "hs4", "hs4", "hs4")
MAKE_WORD_TRANSLATION(tag_hs5, "hs5", "hs5", "hs5", "VK5", "ŹC5", "hs5", "hs5", "hs5", "hs5")
MAKE_WORD_TRANSLATION(tag_hs6, "hs6", "hs6", "hs6", "VK6", "ŹC6", "hs6", "hs6", "hs6", "hs6")
MAKE_WORD_TRANSLATION(tag_hs7, "hs7", "hs7", "hs7", "VK7", "ŹC7", "hs7", "hs7", "hs7", "hs7")
MAKE_WORD_TRANSLATION(tag_hs8, "hs8", "hs8", "hs8", "VK8", "ŹC8", "hs8", "hs8", "hs8", "hs8")
MAKE_WORD_TRANSLATION(tag_hs9, "hs9", "hs9", "hs9", "VK9", "ŹC9", "hs9", "hs9", "hs9", "hs9")
MAKE_WORD_TRANSLATION(tag_hs10, "hs10", "hs10", "hs10", "VK10", "ŹC10", "hs10", "hs10", "hs10", "hs10")
MAKE_WORD_TRANSLATION(tag_hs11, "hs11", "hs11", "hs11", "VK11", "ŹC11", "hs11", "hs11", "hs11", "hs11")
MAKE_WORD_TRANSLATION(tag_hs12, "hs12", "hs12", "hs12", "VK12", "ŹC12", "hs12", "hs12", "hs12", "hs12")
MAKE_WORD_TRANSLATION(tag_hs13, "hs13", "hs13", "hs13", "VK13", "ŹC13", "hs13", "hs13", "hs13", "hs13")
MAKE_WORD_TRANSLATION(tag_hs14, "hs14", "hs14", "hs14", "VK14", "ŹC14", "hs14", "hs14", "hs14", "hs14")
MAKE_WORD_TRANSLATION(tag_hs15, "hs15", "hs15", "hs15", "VK15", "ŹC15", "hs15", "hs15", "hs15", "hs15")
MAKE_WORD_TRANSLATION(tag_hs16, "hs16", "hs16", "hs16", "VK16", "ŹC16", "hs16", "hs16", "hs16", "hs16")


// General
MAKE_WORD_TRANSLATION(on, "on", "an", "aan", "på", "włączono", "på", "on", "açık", "on")
MAKE_WORD_TRANSLATION(off, "off", "aus", "uit", "av", "wyłączono", "av", "off", "kapalı", "off")
MAKE_WORD_TRANSLATION(ON, "ON", "AN", "AAN", "PÅ", "wł.", "PÅ", "ON", "AÇIK", "ON")
MAKE_WORD_TRANSLATION(OFF, "OFF", "AUS", "UIT", "AV", "wył.", "AV", "OFF", "KAPALI", "OFF")

// Unit Of Measurement mapping - maps to DeviceValueUOM_s in emsdevice.cpp
// uom - also used with HA see https://github.com/home-assistant/core/blob/d7ac4bd65379e11461c7ce0893d3533d8d8b8cbf/homeassistant/const.py#L384
MAKE_WORD_TRANSLATION(minutes, "minutes", "Minuten", "Minuten", "Minuter", "minut", "Minutter", "minutes", "dakika", "Minuti")
MAKE_WORD_TRANSLATION(hours, "hours", "Stunden", "Uren", "Timmar", "godzin", "Timer", "heures", "saat", "ore")
MAKE_WORD_TRANSLATION(days, "days", "Tage", "Dagen", "Dagar", "dni", "Dager", "jours", "gün", "giorni")
MAKE_WORD_TRANSLATION(seconds, "seconds", "Sekunden", "Seconden", "Sekunder", "sekund", "Sekunder", "secondes", "saniye", "Secondi")


// Enum translations
// general
MAKE_WORD_TRANSLATION(day_mo, "mo", "Mo", "Mo", "Må", "poniedziałek", "ma", "lun", "Pzt", "lu")
MAKE_WORD_TRANSLATION(day_tu, "tu", "Di", "Di", "Ti", "wtorek", "ti", "mar", "Sal", "ma")
MAKE_WORD_TRANSLATION(day_we, "we", "Mi", "Wo", "On", "środa", "on", "mer", "Çar", "me")
MAKE_WORD_TRANSLATION(day_th, "th", "Do", "Do", "To", "czwartek", "to", "jeu", "Per", "gio")
MAKE_WORD_TRANSLATION(day_fr, "fr", "Fr", "Vr", "Fr", "piątek", "fr", "ven", "Cum", "ve")
MAKE_WORD_TRANSLATION(day_sa, "sa", "Sa", "Za", "Lö", "sobota", "lø", "sam", "Cts", "sa")
MAKE_WORD_TRANSLATION(day_su, "su", "So", "Zo", "Sö", "niedziela", "sø", "dim", "Paz", "do")
MAKE_WORD_TRANSLATION(all, "all", "Alle", "Alle", "Alla", "codziennie", "alle", "tous", "tüm", "tutti")
MAKE_WORD_TRANSLATION(own_1, "own 1", "Eigen 1", "Eigen 1", "Egen 1", "własny 1", "egen 1", "propre 1", "kendi 1", "proprio 1")
MAKE_WORD_TRANSLATION(family, "family", "Familie", "Familie", "Familj", "rodzina", "familie", "famille", "aile", "famiglia")
MAKE_WORD_TRANSLATION(morning, "morning", "Morgends", "'s ochtends", "Morgon", "zmiana 1", "morgen", "matin", "sabah", "mattina")
MAKE_WORD_TRANSLATION(evening, "evening", "Abends", "'s avonds", "Kväll", "zmiana 2", "kveld", "soir", "akşam", "sera")
MAKE_WORD_TRANSLATION(seniors, "seniors", "Senioren", "senioren", "Seniorer", "senior", "seniorer", "séniors", "yaşlılar", "vecchi")
MAKE_WORD_TRANSLATION(no, "no", "nein", "nee", "nej", "nie", "nei", "non", "hayır", "no")
MAKE_WORD_TRANSLATION(new, "new", "Neu", "Nieuw", "Ny", "nowy", "ny", "nouveau", "yeni", "nuovo")
MAKE_WORD_TRANSLATION(own_2, "own 2", "Eigen 2", "Eigen 2", "Egen 2", "własny 2", "egen 2", "propre 2", "kendi 2", "proprio 2")
MAKE_WORD_TRANSLATION(singles, "singles", "Singles", "singles", "Singlar", "osoba samotna", "single", "seuls", "tekliler", "singoli")
MAKE_WORD_TRANSLATION(am, "am", "Vormittag", "ochtend", "Förmiddag", "do południa", "formiddag", "matin", "sabah", "mattina")
MAKE_WORD_TRANSLATION(pm, "pm", "Nachmittag", "namiddag", "Eftermiddag", "po południu", "ettermiddag", "après-midi", "akşam", "pomeriggio")
MAKE_WORD_TRANSLATION(midday, "midday", "Mittag", "middag", "Middag", "południe", "middag", "midi", "öğlen", "mezzogiorno")
MAKE_WORD_TRANSLATION(unknown, "unknown", "Unbekannt", "onbekend", "Okänt", "nieznany", "ukjent", "inconnu", "bilinmeyen", "sconosciuto")
MAKE_WORD_TRANSLATION(flat, "flat", "flach", "vlak", "Platt", "płaski", "flat", "plat", "düz", "piatto")
MAKE_WORD_TRANSLATION(vacuum, "vacuum", "Vakuum", "vacuum", "Vakuum", "próżnia", "vakum", "vide", "vakum", "vacuum")
MAKE_WORD_TRANSLATION(co2_optimized, "co2 optimized", "CO2 optimiert", "CO2 geoptimaliseerd", "CO2-optimerad", "optymalizacja CO2", "co2 optimalisert", "optimisé en CO2", "CO2 verimli", "CO2 ottimizzato")
MAKE_WORD_TRANSLATION(cost_optimized, "cost optimized", "kostenoptimiert", "kosten geoptimaliseerd", "kostnadsoptimerad", "optymalizacja kosztów", "kostnadsoptimalisert", "optimisé en coût", "maliyet odaklı", "costo ottimizzato")
MAKE_WORD_TRANSLATION(outside_temp_switched, "outside temp switched", "Außentemp. gesteuert", "buitentemp. gestuurd", "Utomhustemp korrigerad", "temperatura zewn. przeł.", "utetemp optimalisert", "contrôle par temp. ext.", "dış hava sıcaklığına bağlı", "temperatura esterna cambiata")
MAKE_WORD_TRANSLATION(co2_cost_mix, "co2 cost mix", "Kostenmix", "kostenmix", "Kostnadsmix", "mieszany koszt CO2", "", "coût mixte CO2", "karışık maliyet", "co2 cost mix") // TODO translate
MAKE_WORD_TRANSLATION(analog, "analog", "analog", "analoog", "analog", "analogowy", "analog", "analogique", "analog", "analogico")
MAKE_WORD_TRANSLATION(normal, "normal", "normal", "normaal", "normal", "normalny", "normal", "normal", "normal", "normale")
MAKE_WORD_TRANSLATION(blocking, "blocking", "Blockierung", "blokkering", "Blockering", "blokowanie", "blokkering", "bloquant", "engelleme", "bloccaggio")
MAKE_WORD_TRANSLATION(extern, "extern", "extern", "extern", "extern", "zewnętrzny", "ekstern", "externe", "dış", "eesterno")
MAKE_WORD_TRANSLATION(intern, "intern", "intern", "intern", "intern", "wewnętrzny", "intern", "interne", "iç", "interno")
MAKE_WORD_TRANSLATION(lower, "lower", "niedirger", "lager", "lägre", "mniejszy", "nedre", "inférieur", "daha düşük", "basso")
MAKE_WORD_TRANSLATION(error, "error", "Fehler", "error", "Fel", "błąd", "feil", "erreur", "Hata", "errore")
MAKE_WORD_TRANSLATION(na, "n/a", "n/a", "n/a", "n/a", "nd.", "n/a", "n/c", "mevcut değil", "n/a")

// boiler
MAKE_WORD_TRANSLATION(time, "time", "Zeit", "tijd", "Tid", "godzina", "tid", "heure", "zaman", "ora")
MAKE_WORD_TRANSLATION(date, "date", "Datum", "datum", "Datum", "data", "dato", "date", "tarih", "data")
MAKE_WORD_TRANSLATION(continuous, "continuous", "kontinuierlich", "continue", "kontinuerlig", "ciągły", "kontinuerlig", "continu", "devam eden", "continuo")
MAKE_WORD_TRANSLATION(3wayvalve, "3-way valve", "3-Wege Ventil", "3-weg klep", "trevägsventil", "zawór 3-drogowy", "treveisventil", "vanne 3 voies" , "3 yollu vana", "valvola 3 vie")
MAKE_WORD_TRANSLATION(chargepump, "chargepump", "Ladepumpe", "laadpomp", "laddpump", "pompa ładująca", "ladepumpe", "pompe de charge", "besleme pompası", "pompa di carica")
MAKE_WORD_TRANSLATION(hot, "hot", "Heiß", "heet", "Het", "gorący", "het", "chaud", "sıcak", "caldo")
MAKE_WORD_TRANSLATION(high_comfort, "high comfort", "gehobener Komfort", "verhoogd comfort", "Förhöjd komfort", "wysoki komfort", "høy komfort", "comfort", "komfor", "comfort alto")
MAKE_WORD_TRANSLATION(eco, "eco", "Eco", "Eco", "Eko", "eko", "øko", "éco", "eko", "Eco")
MAKE_WORD_TRANSLATION(intelligent, "intelligent", "Intelligent", "intelligent", "Intelligent", "inteligentny", "intelligent", "intelligent", "akıllı", "intelligente")
MAKE_WORD_TRANSLATION(flow, "flow", "Durchfluss", "volumestroom", "Flöde", "przepływ", "strømme", "débit", "akım", "flusso")
MAKE_WORD_TRANSLATION(manual, "manual", "Manuell", "handmatig", "Manuell", "ręczny", "manuell", "manuel", "manuel", "manuale")
MAKE_WORD_TRANSLATION(buffer, "buffer", "Speicher", "buffer", "Buffert", "bufor", "buffer", "buffer", "tampon", "Buffer")
MAKE_WORD_TRANSLATION(bufferedflow, "buffered flow", "Durchlaufspeicher", "doorstroombuffer", "Buffertflöde", "przepływ buforowany", "bufret strømning", "", "tampon akım", "memoria flusso") // TODO translate
MAKE_WORD_TRANSLATION(layeredbuffer, "layered buffer", "Schichtspeicher", "gelaagde buffer", "Lagrad buffert", "bufor warstwowy", "lagdelt buffer", "", "katmanlı akım", "strato memoria") // TODO translate
MAKE_WORD_TRANSLATION(maintenance, "maintenance", "Wartung", "onderhoud", "Underhåll", "przegląd", "vedlikehold", "maintenance", "bakım", "servizio")
MAKE_WORD_TRANSLATION(heating, "heating", "Heizen", "verwarmen", "Uppvärmning", "ogrzewanie", "oppvarming", "chauffage", "ısıtma", "riscaldamento")
MAKE_WORD_TRANSLATION(cooling, "cooling", "Kühlen", "koelen", "Kyler", "chłodzenie", "kjøling", "refroidissement", "soğuma", "raffreddamento")
MAKE_WORD_TRANSLATION(heatandcool, "heating&cooling", "Heizen&Kühlen", "verwarmen&koelen", "Uppvärmning&Kyler", "", "", "", "ısıtma&soğutma", "") // TODO translate
MAKE_WORD_TRANSLATION(disinfecting, "disinfecting", "Desinfizieren", "desinfecteren", "Desinficerar", "dezynfekcja termiczna", "desinfisering", "désinfection", "dezenfeksiyon", "disinfezione")
MAKE_WORD_TRANSLATION(no_heat, "no heat", "keine Wärme", "geen warmte", "Ingen värme", "brak ciepła", "ingen varme", "pas de chauffage", "ısınma yok", "nessun calore")
MAKE_WORD_TRANSLATION(heatrequest, "heat request", "Wärmeanforderung", "verwarmingsverzoek", "Värmeförfrågan", "zapotrzebowanie na ciepło", "varmeforespørsel", "demande de chauffage", "ısınma ihtiyacı", "richiesta calore")
MAKE_WORD_TRANSLATION(valve, "valve", "Ventil", "klep", "Ventil", "zawór", "ventil", "valve", "vana", "valvola")
MAKE_WORD_TRANSLATION(proportional, "proportional", "proportional", "proportioneel", "", "proporcjonalny", "proposjonal", "", "oransal", "proporzionale") // TODO translate
MAKE_WORD_TRANSLATION(deltaP1, "deltaP-1", "deltaP-1", "deltaP-1", "", "", "deltaP-1", "", "deltaP-1", "deltaP-1") // TODO translate
MAKE_WORD_TRANSLATION(deltaP2, "deltaP-2", "deltaP-2", "deltaP-2", "", "", "deltaP-2", "", "deltaP-2", "deltaP-2") // TODO translate
MAKE_WORD_TRANSLATION(deltaP3, "deltaP-3", "deltaP-3", "deltaP-3", "", "", "deltaP-3", "", "deltaP-3", "deltaP-3") // TODO translate
MAKE_WORD_TRANSLATION(deltaP4, "deltaP-4", "deltaP-4", "deltaP-4", "", "", "deltaP-4", "", "deltaP-4", "deltaP-4") // TODO translate

// heatpump
MAKE_WORD_TRANSLATION(none, "none", "keine", "geen", "ingen", "brak", "ingen", "aucun", "hiçbiri", "nessuno")
MAKE_WORD_TRANSLATION(hot_water, "hot water", "Warmwasser", "warm water", "varmvatten", "c.w.u.", "varmtvann", "eau chaude", "sıcak su", "acqua calda")
MAKE_WORD_TRANSLATION(pool, "pool", "Pool", "zwembad", "pool", "basen", "basseng", "piscine", "havuz", "piscina")
MAKE_WORD_TRANSLATION(outside_temp_alt, "outside temperature alt.", "Außentemp. alternativ", "alternatieve buitentemperatuur", "Alternativ utomhustemp.", "temp. zewn. alternat.", "alternativ utendørstemp.", "température extérieure alternative", "alternatif dış sıcaklık", "temperatura esterna alternativa")
MAKE_WORD_TRANSLATION(outside_temp_par, "outside temperature parallel", "Außentemp. parallel", "buitentemperatuur parallel", "Parallell utomhustemp.", "temp. zewn. równoległa", "parallell utendørstemp.", "température extérieure parallèle", "paralel dış sıcaklık", "temperatura esterna parallela")
MAKE_WORD_TRANSLATION(hp_prefered, "heatpump prefered", "Wärmepumpe bevorzugt", "voorkeur warmtepomp", "Värmepump föredraget", "preferowana pompa ciepła", "varmepumpe prioritert", "pompe à chaleur préférée", "tercih edilen pompa", "pompa di calore preferita")
MAKE_WORD_TRANSLATION(boiler_only, "boiler only", "nur Kessel", "uitsluitend cv ketel", "Värmepanna enbart", "tylko kocioł", "kun kjele", "chaudière uniquement", "sadece kazan", "solo caldaia")
MAKE_WORD_TRANSLATION(reduced_output, "reduced output", "Reduzierte Leistung", "gereduceerde output", "Reducerad produktion", "zmniejszona wydajność", "redusert ytelse", "sortie réduite", "düşürülmüş çıkış", "riduzione uscita")
MAKE_WORD_TRANSLATION(switchoff, "switch off hp", "WP ausschalten", "WP uitschakelen", "Värmepump avstängd", "wyłącz pompę ciepła", "slå av varmepumpe", "éteindre la PAC", "ısı pompasını kapat", "spegnimento pompa calore")
MAKE_WORD_TRANSLATION(perm, "perm. reduced", "perm. reduziert", "permanent gereduceerd", "Permanent reducerad", "stale zmniejszona wydajność", "permanent redusert", "réduction permanente", "sürekli azaltılmış", "riduzione permanente")

// thermostat
MAKE_WORD_TRANSLATION(seltemp, "selTemp", "Solltemperatur", "doeltemperatuur", "Börtemperatur", "temperatura zadana", "innstilt temperatur", "consigne température", "ayarlanmış sıcaklık", "temperatura di consegna")
MAKE_WORD_TRANSLATION(roomtemp, "roomTemp", "Raumtemperatur", "kamertemperatuur", "Rumstemperatur", "temperatura w pomieszczeniu", "romstemperatur", "température de la pièce", "oda sıcaklığı", "temperatura camera")
MAKE_WORD_TRANSLATION(own_prog, "own prog", "Eigenprog.", "eigen prog.", "Egen prog.", "program własny", "eget prog.", "programme propre", "isteğe göre ayarlanmış program", "proprio prog.")
MAKE_WORD_TRANSLATION(std_prog, "std prog", "Standardprog.", "standaard prog.", "Standardprog.", "program standardowy", "standardprog.", "programme standard", "sandart pogram", "programma standard")
MAKE_WORD_TRANSLATION(light, "light", "Leicht", "licht", "Lätt", "lekki", "lett", "léger", "düşük", "luce")
MAKE_WORD_TRANSLATION(medium, "medium", "Mittel", "middel", "Medel", "średni", "medium", "medium", "orta", "medio")
MAKE_WORD_TRANSLATION(heavy, "heavy", "Schwer", "zwaar", "Tung", "ciężki", "tung", "lourd", "yüksek", "pesante")
MAKE_WORD_TRANSLATION(start, "start", "Start", "start", "Start", "start", "start", "début", "başlat", "avvia")
MAKE_WORD_TRANSLATION(heat, "heat", "Heizen", "verwarmen", "Värme", "ciepło", "varmer", "chaleur", "ısıtma", "caldo")
MAKE_WORD_TRANSLATION(hold, "hold", "Halten", "pauzeren", "Paus", "pauza", "pause", "pause", "durdur", "pausa")
MAKE_WORD_TRANSLATION(cool, "cool", "Kühlen", "koelen", "Kyla", "zimno", "kjøler", "froid", "soğutma", "freddo")
MAKE_WORD_TRANSLATION(end, "end", "Ende", "einde", "Slut", "koniec", "slutt", "fin", "bitti", "fine")
MAKE_WORD_TRANSLATION(german, "german", "Deutsch", "Duits", "Tyska", "niemiecki", "tysk", "allemand", "Almanca", "Tedesco")
MAKE_WORD_TRANSLATION(dutch, "dutch", "Niederländisch", "Nederlands", "Nederländska", "niderlandzki", "nederlandsk", "néerlandais", "Flemenkçe", "Olandese")
MAKE_WORD_TRANSLATION(french, "french", "Französisch", "Frans", "Franska", "francuski", "fransk", "français", "Fransızca", "Francese")
MAKE_WORD_TRANSLATION(italian, "italian", "Italienisch", "Italiaans", "Italienska", "włoski", "italiensk", "italien", "İtalyanca", "Italiano")
MAKE_WORD_TRANSLATION(high, "high", "hoch", "hoog", "Hög", "wysoki", "høy", "haut", "yüksek", "alto")
MAKE_WORD_TRANSLATION(low, "low", "niedrig", "laag", "Låg", "niski", "lav", "bas", "düşük", "basso")
MAKE_WORD_TRANSLATION(radiator, "radiator", "Heizkörper", "radiator", "Radiator", "grzejniki", "radiator", "radiateur", "radyatör", "radiatore")
MAKE_WORD_TRANSLATION(convector, "convector", "Konvektor", "convector", "Konvektor", "konwektory", "konvektor", "convecteur", "convector", "convettore")
MAKE_WORD_TRANSLATION(floor, "floor", "Fussboden", "vloer", "Golv", "podłoga", "gulv", "sol", "yer", "pavimento")
MAKE_WORD_TRANSLATION(summer, "summer", "Sommer", "zomer", "Sommar", "lato", "sommer", "été", "yaz", "estate")
MAKE_WORD_TRANSLATION(winter, "winter", "Winter", "winter", "Vinter", "zima", "vinter", "hiver", "kış", "inverno")
MAKE_WORD_TRANSLATION(outdoor, "outdoor", "Außen", "buiten", "Utomhus", "temp. zewnętrzna", "utendørs", "extérieur", "dış", "esterno")
MAKE_WORD_TRANSLATION(room, "room", "Raum", "kamer", "Rum", "temp. w pomieszczeniu", "", "pièce", "oda", "camera") // TODO translate
MAKE_WORD_TRANSLATION(room_outdoor, "room outdoor", "Raum+Außen", "kamer+buiten", "Rum+Ute", "temp. w pom. i zewn.", "rom utendørs", "pièce extérieure", "oda ve dış", "camera esterna")
MAKE_WORD_TRANSLATION(power, "power", "Leistung", "vermogen", "Effekt", "moc", "effekt", "puissance", "güç", "potenza")
MAKE_WORD_TRANSLATION(constant, "constant", "konstant", "constant", "Konstant", "stały", "konstant", "constant", "sabit", "costante")
MAKE_WORD_TRANSLATION(simple, "simple", "einfach", "simpel", "enkel", "prosty", "enkel", "simple", "basit", "semplice")
MAKE_WORD_TRANSLATION(optimized, "optimized", "optimiert", "geoptimaliseerd", "optimerad", "zoptymalizowany", "optimalisert", "optimisé", "optimize", "ottimizzato")
MAKE_WORD_TRANSLATION(nofrost, "nofrost", "Frostschutz", "vorstbescherming", "Frostskydd", "ochrona przed zamarzaniem", "frostsikring", "protection gel", "Donma koruması", "protezione gelo")
MAKE_WORD_TRANSLATION(defrost, "defrost", "Abtauen", "ontdooien", "avfrostning", "rozmrażać", "tine", "dégivrage", "buz çözücü", "scongelamento")
MAKE_WORD_TRANSLATION(comfort, "comfort", "Komfort", "comfort", "Komfort", "komfort", "komfort", "comfort", "konfor", "comfort")
MAKE_WORD_TRANSLATION(night, "night", "Nacht", "nacht", "Natt", "noc", "natt", "nuit", "gece", "notte")
MAKE_WORD_TRANSLATION(day, "day", "Tag", "dag", "Dag", "dzień", "dag", "jour", "gün", "giorno")
MAKE_WORD_TRANSLATION(holiday, "holiday", "Urlaub", "vakantie", "Helgdag", "urlop?", "ferie", "vacances", "tatil", "vacanza")
MAKE_WORD_TRANSLATION(reduce, "reduce", "reduziert", "gereduceerd", "Reducera", "zredukowany", "redusere", "réduit", "düşür", "riduzione")
MAKE_WORD_TRANSLATION(noreduce, "no reduce", "unreduziert", "niet gereduceerd", "oreducerad", "bez redukcji", "ingen reduksjon", "pas de réduction", "düşürme", "non ridurre")
MAKE_WORD_TRANSLATION(offset, "offset", "Anhebung", "offset", "Förskutning", "przesunięcie", "kompensasjon", "offset", "kompansasyon", "offset")
MAKE_WORD_TRANSLATION(design, "design", "Auslegung", "ontwerp", "Design", "projekt", "design", "design", "tasarım", "disegno")
MAKE_WORD_TRANSLATION(minflow, "min flow", "min. Durchfluss", "min. doorstroom", "Min flöde", "minimalny przepływ", "min strømming", "flux min", "minimum akış", "flusso minimo")
MAKE_WORD_TRANSLATION(maxflow, "max flow", "max. Durchfluss", "max. doorstroom", "Max flöde", "maksymalny przepływ", "maks strømming", "flux max", "maksimum akış", "flusso massimo")
MAKE_WORD_TRANSLATION(fast, "fast", "schnell", "snel", "snabb", "szybkie", "hurtig", "rapide", "hızlı", "veloce")
MAKE_WORD_TRANSLATION(slow, "slow", "langsam", "langzaam", "långsam", "powolne", "langsom", "lent", "yavaş", "lento")
MAKE_WORD_TRANSLATION(internal_temperature, "internal temperature", "Interne Temperatur", "interne temperatuur", "Interntemperatur", "temperatura wewnętrzna", "interntemperatur", "température interne", "oda sıcaklığı", "temperatura interna")
MAKE_WORD_TRANSLATION(internal_setpoint, "internal setpoint", "Interner Sollwert", "interne streeftemperatuur", "Internt börvärde", "nastawa wewnętrzna", "internt settpunkt", "consigne interne", "istenen oda sıcaklığı", "setpoint interno")
MAKE_WORD_TRANSLATION(external_temperature, "external temperature", "Externe Temperatur", "externe temperatuur", "Extern temperatur", "temperatura zewnętrzna", "ekstern temperatur", "température externe", "dış sıcaklık", "temperatura esterna")
MAKE_WORD_TRANSLATION(burner_temperature, "burner temperature", "Brennertemperatur", "brander temperatuur", "Brännartemperatur", "temperatura palnika", "brennertemperatur", "température du brûleur", "kazan sıcaklığı", "temperatura bruciatore")
MAKE_WORD_TRANSLATION(ww_temperature, "ww temperature", "Wassertemperatur", "watertemperatuur", "Vattentemperatur", "temperatura c.w.u.", "vanntemperatur", "température de l'eau", "Kullanım suyu sıcaklığı", "temperatura acqua")
MAKE_WORD_TRANSLATION(smoke_temperature, "smoke temperature", "Abgastemperatur", "rookgastemperatuur", "Rökgastemperatur", "temperatura dymu", "røykgasstemperatur", "température des gaz d'échappement", "baca gazı sıcaklığı", "temperatura fumo")
MAKE_WORD_TRANSLATION(weather_compensated, "weather compensated", "Wetter kompensiert", "weer gecompenseerd", "Väderkompenserad", "skompensow. pogodą", "værkompensert", "compensation par l'extérieur", "hava durumuna göre dengelenmiş", "acqua compensata")
MAKE_WORD_TRANSLATION(outside_basepoint, "outside basepoint", "Basispunkt Außentemp.", "buiten basispunt", "Utomhus baspunkt", "temp. zewn. z pkt. pocz.", "utendørs basispunkt", "point de base temp. ext.", "dış hava sıcaklığı taban noktası", "basepoint esterno")
MAKE_WORD_TRANSLATION(functioning_mode, "functioning mode", "Funktionsweise", "functiemodus", "Driftläge", "tryb pracy", "driftsmodus", "mode de fonctionnement", "işletme konumu", "modalità di funzionamento")

// mixer
MAKE_WORD_TRANSLATION(stopped, "stopped", "gestoppt", "gestopt", "stoppad", "zatrzymany", "stoppet", "arrêté", "durdu", "fermato")
MAKE_WORD_TRANSLATION(opening, "opening", "öffnen", "openen", "öppnar", "otwieranie", "åpner", "ouverture", "açılıyor", "aperto")
MAKE_WORD_TRANSLATION(closing, "closing", "schließen", "sluiten", "stänger", "zamykanie", "stenger", "fermeture", "kapanıyor", "chiuso")
MAKE_WORD_TRANSLATION(open, "open", "offen", "open", "Öppen", "otwórz", "åpen", "ouvert", "açık", "aprire")
MAKE_WORD_TRANSLATION(close, "close", "geschlossen", "Gesloten", "Stängd", "zamknij", "stengt", "fermé", "kapalı", "chiudere")

// solar ww
MAKE_WORD_TRANSLATION(cyl1, "cyl 1", "Zyl_1", "Cil 1", "Cyl 1", "cyl 1", "cyl 1", "cyl 1", "cly 1", "Cil 1")
MAKE_WORD_TRANSLATION(cyl2, "cyl 2", "Zyl_2", "Cil 2", "Cyl 2", "cyl 2", "cyl 2", "cyl 2", "cly 1", "Cil 2")

// ventilation
MAKE_WORD_TRANSLATION(demand, "demand", "Bedarf", "vereist", "", "", "", "", "talep", "richiesta") // TODO translate
MAKE_WORD_TRANSLATION(intense, "intense", "Intensiv", "intensief", "", "", "", "", "yoğun", "intensivo") // TODO translate
MAKE_WORD_TRANSLATION(sleep, "sleep", "Einschlafen", "slaapmodus", "", "", "", "", "uyku", "notturno") // TODO translate
MAKE_WORD_TRANSLATION(partymode, "party", "Party", "party", "", "", "", "", "parti", "festa") // TODO translate
MAKE_WORD_TRANSLATION(fireplace, "fireplace", "Kamin", "haard", "", "", "", "", "şömine", "camino") // TODO translate

// MQTT Discovery - this is special device entity for 'climate'
MAKE_TRANSLATION(haclimate, "haclimate", "Discovery current room temperature", "Discovery Temperatur", "Discovery huidige kamertemperatuur", "", "termostat w HA", "HA Avlest temp", "", "Güncel osa sıcaklığı", "verifica temperatura ambiente attuale") // TODO translate

// Entity translations: tag, mqtt, en, de, nl, sv, pl, no, fr, tr, it
// Boiler
MAKE_TRANSLATION(forceHeatingOff, "heatingoff", "force heating off", "Heizen abschalten", "", "", "", "", "", "", "") // TODO translate
MAKE_TRANSLATION(wwtapactivated, "wwtapactivated", "turn on/off", "Durchlauferhitzer aktiv", "zet aan/uit", "på/av", "system przygotowywania", "Varmtvann active", "ecs activée", "aç/kapa", "commuta on/off")
MAKE_TRANSLATION(reset, "reset", "reset", "Reset", "Reset", "Nollställ", "kasowanie komunikatu", "nullstill", "reset", "Sıfırla", "Reset")
MAKE_TRANSLATION(oilPreHeat, "oilpreheat", "oil preheating", "Ölvorwärmung", "Olie voorverwarming", "Förvärmning olja", "podgrzewanie oleju", "oljeforvarming", "préchauffage de l'huile", "Yakıt Ön ısıtma devrede", "preriscaldamento olio")
MAKE_TRANSLATION(heatingActive, "heatingactive", "heating active", "Heizen aktiv", "Verwarming actief", "Uppvärmning aktiv", "c.o. aktywne", "oppvarming aktiv", "chauffage actif", "ısıtma devrede", "riscaldamento attivo")
MAKE_TRANSLATION(tapwaterActive, "tapwateractive", "tapwater active", "Warmwasser aktiv", "Warm water actief", "Varmvatten aktiv", "c.w.u. aktywne", "varmtvann aktiv", "eau chaude active", "sıcak kullanım suyu devrede", "acqua calda attiva")
MAKE_TRANSLATION(selFlowTemp, "selflowtemp", "selected flow temperature", "Sollwert Vorlauftemperatur", "Ingestelde aanvoertemperatuur", "Börvärde Flödestemperatur", "wybrana temperatura zasilania", "valgt turtemperatur", "température de flux selectionnée", "seçili akış sıcaklığı", "flusso temperatura selezionato")
MAKE_TRANSLATION(selBurnPow, "selburnpow", "burner selected max power", "Sollwert Brennerleistung", "Ingestelde maximale brandervermogen", "Brännare vald maxeffekt", "wybrana moc źródła ciepła", "settpunkt brennerkapasitet", "puissance max du brûleur selectionnée", "seçili kazan maksimum güç", "Setpoint potenza bruciatore")
MAKE_TRANSLATION(absBurnPow, "absburnpow", "burner current power (absolute)", "Brennerleistung (absolut)", "Brandervermogen (abs)", "Värmepanna aktuell effekt (abs)", "aktualna moc źródła ciepła (absolutna)", "brennereffekt", "puissance du brûleur actuelle (abs)", "kazan anlık akış gücü(mutlak)", "potenza attuale del bruciatore (assoluta)")
MAKE_TRANSLATION(heatingPumpMod, "heatingpumpmod", "heating pump modulation", "Heizungspumpe 1 Modulation", "Modulatie verwarmingspomp", "Modulering Värmepump", "wysterowanie pompy c.o.", "varmepumpemodulering", "modulation de la pompe à chaleur", "ısı pompası modülasyonu", "modulazione pompa di calore")
MAKE_TRANSLATION(outdoorTemp, "outdoortemp", "outside temperature", "Aussentemperatur", "Buitentemperatuur", "Utomhustemperatur", "temperatura zewnętrzna", "utetemperatur", "température extérieure", "dış ortam sıcaklığı", "tempertura esterna")
MAKE_TRANSLATION(curFlowTemp, "curflowtemp", "current flow temperature", "aktuelle Vorlauftemperatur", "Huidige aanvoertemperatuur", "Flödestemperatur", "temperatura zasilania", "aktuell strømmetemperatur", "température actuelle du flux", "akış sıcaklığı", "temperatura di mandata attuale")
MAKE_TRANSLATION(retTemp, "rettemp", "return temperature", "Rücklauftemperatur", "Retourtemperatuur", "Returtemperatur", "temperatura powrotu", "returtemperatur", "température de retour", "dönüş sıcaklığı", "temperatura di ritorno attuale")
MAKE_TRANSLATION(switchTemp, "switchtemp", "mixing switch temperature", "Mischer Schalttemperatur", "Mixer temperatuur", "Blandartemperatur", "temperatura przełączania mieszacza", "Blandertemperatur", "température de bascule du mélangeur", "karışım hücresi sıcaklığı", "Temperatura di commutazione del miscelatore")
MAKE_TRANSLATION(sysPress, "syspress", "system pressure", "Systemdruck", "Systeemdruk", "Systemtryck", "ciśnienie w systemie", "systemtrykk", "pression du système", "sistem basıncı", "pressione sistema")
MAKE_TRANSLATION(boilTemp, "boiltemp", "actual boiler temperature", "Kesseltemperatur", "Keteltemperatuur", "Temperatur Värmepanna", "temperatura zasobnika", "varmepumpetemp.", "température de la chaudière", "gerçek boyler sıcaklığı", "temperatura attuale caldaia")
MAKE_TRANSLATION(exhaustTemp, "exhausttemp", "exhaust temperature", "Abgastemperatur", "Uitlaattemperatuur", "Avgastemperatur", "temperatura spalin", "røykgasstemp", "température des gaz d'échappement", "baca sıcaklığı", "temperatura di scarico")
MAKE_TRANSLATION(burnGas, "burngas", "gas", "Gas", "Gas", "Gas", "gaz", "gass", "gaz", "gaz", "Gas")
MAKE_TRANSLATION(burnGas2, "burngas2", "gas stage 2", "Gas Stufe 2", "gas fase 2", "Gas Fas 2", "gaz 2 stopień", "gass fase 2", "gaz état 2", "gaz 2.seviye", "gas fase 2")
MAKE_TRANSLATION(flameCurr, "flamecurr", "flame current", "Flammenstrom", "Vlammenstroom", "Lågström", "prąd palnika", "flammetemp", "courrant de flamme", "alev akımı", "corrente di fiamma")
MAKE_TRANSLATION(heatingPump, "heatingpump", "heating pump", "Heizungspumpe", "Verwarmingspomp", "Värmepump", "pompa ciepła", "varmepumpe", "pompe à chaleur", "ısı pompası", "pompa di calore")
MAKE_TRANSLATION(fanWork, "fanwork", "fan", "Gebläse", "Ventilator", "Fläkt", "wentylator", "vifte", "ventilateur", "fan", "Ventilatore")
MAKE_TRANSLATION(ignWork, "ignwork", "ignition", "Zündung", "Ontsteking", "Tändning", "zapłon", "tenning", "ignition", "ateşleme", "accensione")
MAKE_TRANSLATION(heatingActivated, "heatingactivated", "heating activated", "Heizen aktiviert", "Verwarmen geactiveerd", "Uppvärmning aktiv", "system c.o.", "oppvarming aktivert", "chauffage activé", "ısıtma başladı", "riscaldamento attivato")
MAKE_TRANSLATION(heatingTemp, "heatingtemp", "heating temperature", "Heizungstemperatur", "Verwarmingstemperatuur", "Uppvärmningstemperatur", "temperatura grzania", "oppvarmingstemperatur", "température de chauffage", "ısıtma sıcaklığı", "temperatura riscaldamento")
MAKE_TRANSLATION(pumpModMax, "pumpmodmax", "boiler pump max power", "Kesselpumpen Maximalleistung", "Ketelpomp max vermogen", "Värmepannepump max effekt", "maksymalna moc pompy zasobnika", "varmepumpe maks effekt", "puissance max pompe à chaleur", "boyler pompası maksimum güç", "max potenza pompa caldaia")
MAKE_TRANSLATION(pumpModMin, "pumpmodmin", "boiler pump min power", "Kesselpumpen Minmalleistung", "Ketelpomp min vermogen", "Värmepannepump min effekt", "minimalna moc pompy zasobnika", "varmepumpe min effekt", "puissance min pompe à chaleur", "boyler pompası minimum güç", "min potenza pompa caldaia")
MAKE_TRANSLATION(pumpDelay, "pumpdelay", "pump delay", "Pumpennachlaufzeit", "Pomp nalooptijd", "Pumpfördröjning", "opóźnienie pompy", "pumpeforsinkelse", "délai d'attente pompe", "pompa gecikmesi", "ritardo pompa")
MAKE_TRANSLATION(burnMinPeriod, "burnminperiod", "burner min period", "Antipendelzeit", "Antipendeltijd", "Värmepanna Min Period", "minimalny czas pracy palnika", "varmekjele min periode", "délai d'attente du brûleur", "kazan minmum periyod", "periodo minimo del bruciatore")
MAKE_TRANSLATION(burnMinPower, "burnminpower", "burner min power", "minimale Brennerleistung", "Minimaal brandervermogen", "Värmepanna Min Effekt", "minimalna moc źródła ciepła", "varmekjele min effekt", "puissance min brûleur", "kazan minimum güç", "potenza minima bruciatore")
MAKE_TRANSLATION(burnMaxPower, "burnmaxpower", "burner max power", "maximale Brennerleistung", "Maximaal brandervermogen", "Värmepanna Max Effekt", "maksymalna moc źródła ciepła", "varmekjele maks effekt", "puissance max brûleur", "kazan maksimum güç", "potenza massima bruciatore")
MAKE_TRANSLATION(boilHystOn, "boilhyston", "hysteresis on temperature", "Einschaltdifferenz", "ketel aan hysterese verschil", "Hysteres aktiveringstemperatur", "histereza załączania", "hysterese på temperatur", "hysteresis température d'allumage", "gecikme sıcaklığı devrede", "isteresi sulla temperatura")
MAKE_TRANSLATION(boilHystOff, "boilhystoff", "hysteresis off temperature", "Ausschaltdifferenz", "ketel uit hysterese verschil", "Hysteres inaktiveringstemperatur", "histereza wyłączania", "hysterese av temperatur", "hysteresis température d'extinction", "gecikme sıcaklığı kapalı", "isteresi fuori temperatura")
MAKE_TRANSLATION(boil2HystOn, "boil2hyston", "hysteresis stage 2 on temperature", "Einschaltdifferenz Stufe 2", "ketel aan hysterese verschil 2", "Hysteres aktiveringstemperatur 2", "histereza załączania stopnia 2", "", "hysteresis état 2 température d'allumage", "2. seviye gecikme sıcaklığı devrede", "stadio di isteresi 2 sulla temperatura") // TODO translate
MAKE_TRANSLATION(boil2HystOff, "boil2hystoff", "hysteresis stage 2 off temperature", "Ausschaltdifferenz Stufe 2", "ketel uit hysterese verschil 2", "Hysteres inaktiveringstemperatur 2", "histereza wyłączania stopnia 2", "hysterese inaktiveringstemperatur 2", "hysteresis état 2 température d'extinction", "2. seviye gecikme sıcaklığı kapalı", "isteresi stadio 2 fuori temperatura")
MAKE_TRANSLATION(setFlowTemp, "setflowtemp", "set flow temperature", "Sollwert Vorlauftemperatur", "Ingestelde aanvoertemperatuur", "Börvärde Flödestemperatur", "zadana temperatura zasilania", "innstilt turtemperatur", "température du flux définie", "akış sıcaklığını ayarla", "impostare temperatura di mandata")
MAKE_TRANSLATION(setBurnPow, "setburnpow", "burner set power", "Sollwert Brennerleistung", "Ingesteld brandervermogen", "Värmepanna vald Effekt", "zadana moc palnika", "varmekjele valgt effekt", "puissance du brûleur définie", "kazan gücünü ayarla", "impostare potenza bruciatore")
MAKE_TRANSLATION(curBurnPow, "curburnpow", "burner current power", "Brennerleistung", "Brandervermogen", "Värmepanna aktuell effekt", "aktualna moc źródła ciepła", "brennereffekt", "puissance du brûleur actuelle", "kazan güncel gücü", "potenza attuale bruciatore")
MAKE_TRANSLATION(burnStarts, "burnstarts", "burner starts", "Brenner Starts", "Aantal brander starts", "Värmepanna antal starter", "liczba uruchomień palnika", "antall varmepumpe starter", "démarrages du brûleur", "kazan başlıyor", "avvii bruciatore")
MAKE_TRANSLATION(burnWorkMin, "burnworkmin", "total burner operating time", "Brenner Laufzeit", "Totale branderlooptijd", "Värmepanna aktiva timmar", "łączny czas pracy palnika", "brennersteg tid i min", "durée de fonctionnement totale du brûleur", "toplam kazan çalışma süresi", "tempo totale di funzionamento del bruciatore")
MAKE_TRANSLATION(burn2WorkMin, "burn2workmin", "burner stage 2 operating time", "Brenner Stufe 2 Laufzeit", "Totale looptijd brander fase 2", "Värmepanna steg 2 aktiva timmar", "łączny czas pracy palnika 2 stopnia", "brennersteg2 tid i min", "durée de fonctionnement totale du brûleur état 2", "2. seviye toplam kazan çalışma süresi", "tempo di funzionamento del bruciatore 2° stadio")
MAKE_TRANSLATION(heatWorkMin, "heatworkmin", "total heat operating time", "Heizung Laufzeit", "Totale looptijd verwarming", "Uppvärmning aktiva timmar", "łączny czas grzania", "varmetid i min", "durée de fonctionnement du chauffage", "toplam ısıtma çalışma süresi", "tempo totale di funzionamento in riscaldamento")
MAKE_TRANSLATION(heatStarts, "heatstarts", "burner starts heating", "Brenner Starts Heizung", "Aantal brander starts verwarming", "Uppvärmning antal starter", "liczba uruchomień palnika na ogrzewanie", "antall oppvarmninger", "démarrages du chauffage", "kazan ısıtmaya başlıyor", "preriscaldamento bruciatore")
MAKE_TRANSLATION(UBAuptime, "ubauptime", "total UBA operating time", "Anlagen-Gesamtlaufzeit", "totale looptijd branderautomaat (UBA)", "Total Tid", "łączny czas pracy układu sterowania", "totaltid", "durée de fonctionnement totale de l'appareil (UBA)", "kazanın toplam işletme süresi", "Tempo di funzionamento totale del sistema")
MAKE_TRANSLATION(lastCode, "lastcode", "last error code", "Letzter Fehler", "Laatste foutcode", "Senaste Felkod", "ostatni błąd", "siste feilkode", "dernier code d'erreur", "son hata kodu", "ultimo codice errore")
MAKE_TRANSLATION(serviceCode, "servicecode", "service code", "Statusmeldung", "Statuscode", "Servicekod", "kod serwisowy", "servicekode", "code de service", "servis kodu", "codice messaggio di stato")
MAKE_TRANSLATION(serviceCodeNumber, "servicecodenumber", "service code number", "Statusmeldungsnummer", "Status codenummer", "Servicekod", "numer kodu serwisowego", "servicekodenummer", "numéro du code de service", "servis kod numarası", "numero del messaggio di stato")
MAKE_TRANSLATION(maintenanceMessage, "maintenancemessage", "maintenance message", "Wartungsmeldung", "Onderhoudsmelding", "Servicemeddelande", "komunikat przeglądu", "vedlikeholdsmelding", "message de maintenance", "bakım mesajı", "messaggio di manutenzione")
MAKE_TRANSLATION(maintenanceDate, "maintenancedate", "next maintenance date", "Wartungsdatum", "Onderhoudsdatum", "Datum nästa Service", "termin następnego przeglądu", "vedlikeholdsdato", "prochaine date de maintenance", "bakım tarihi", "prossima data di manutenzione")
MAKE_TRANSLATION(maintenanceType, "maintenance", "maintenance scheduled", "Wartungsplan", "Onderhoud gepland", "Underhall schemlagt", "rodzaj przeglądu", "vedlikeholdstype", "maintenance prévue", "planlı bakım", "manutenzione programmata")
MAKE_TRANSLATION(maintenanceTime, "maintenancetime", "time to next maintenance", "Wartung in", "Onderhoud in", "Tid till nästa underhall", "czas do kolejnego przeglądu", "vedlikeholdstid", "durée avant la prochaine maintenance", "bakıma kalan süre", "tempo alla prossima manutenzione")
MAKE_TRANSLATION(emergencyOps, "emergencyops", "emergency operation", "Notoperation", "Noodoperatie", "Nöddrift", "praca w trybie awaryjnym", "nøddrift", "opération d'urgence", "acil durum çalışması", "operazione di emergenza")
MAKE_TRANSLATION(emergencyTemp, "emergencytemp", "emergency temperature", "Nottemperatur", "Noodtemperatuur", "Nöddrift temperatur", "temperatura w trybie awaryjnym", "nødtemperatur", "température d'urgence", "acil durum sıcaklığı", "temperatura di emergenza")
MAKE_TRANSLATION(pumpMode, "pumpmode", "boiler pump mode", "Kesselpumpen Modus", "Ketelpomp modus", "", "tryb pracy pompy kotła", "pumpemodus", "", "pompa modu", "modalità pompa caldaia") // TODO translate
MAKE_TRANSLATION(headertemp, "headertemp", "low loss header", "Hydr. Weiche", "open verdeler", "", "", "", " bouteille de déc. hydr.", "isı bloğu gidiş suyu sıc.", "comp. idr.") // TODO translate
MAKE_TRANSLATION(heatblock, "heatblock", "heating block", "Wärmezelle", "Aanvoertemp. warmtecel", "", "", "", "départ corps de chauffe", "Hid.denge kabı sıcaklığı", "mandata scamb. pr.") // TODO translate

// heatpump/compress specific
MAKE_TRANSLATION(upTimeControl, "uptimecontrol", "total operating time heat", "Betriebszeit Heizen gesamt", "Totale bedrijfstijd", "Total tid uppvärmning", "łączny czas generowania ciepła", "total driftstid", "durée totale de fonctionnement chauffage", "ısınma toplam işletme süresi", "Tempo di funzionamento totale riscaldamento")
MAKE_TRANSLATION(upTimeCompHeating, "uptimecompheating", "operating time compressor heating", "Betriebszeit Kompressor heizen", "Bedrijfstijd compressor verwarmingsbedrijf", "Total tid kompressor uppvärmning", "łączny czas ogrzewania (sprężarka)", "totaltid kompressor", "durée de fonctionnement compresseur chauffage", "ısı pompası ısınma işletme süresi", "tempo di funzionamento del compressore riscaldamento")
MAKE_TRANSLATION(upTimeCompCooling, "uptimecompcooling", "operating time compressor cooling", "Betriebszeit Kompressor kühlen", "Bedrijfstijd compressor koelbedrijf", "Total tid kompressor kyla", "łączny czas chłodzenia (sprężarka)", "Total tid kompressor kjøling", "durée de fonctionnement compresseur refroidissement", "ısı pompası soğuma işletme süresi", "tempo di funzionamento del compressore raffreddamento")
MAKE_TRANSLATION(upTimeCompWw, "uptimecompww", "operating time compressor dhw", "Betriebszeit Kompressor", "Bedrijfstijd compressor warmwaterbedrijf", "Total tid kompressor varmvatten", "łączny czas grzania c.w.u. (sprężarka)", "Total tid kompressor varmtvann", "durée de fonctionnement compresseur ecs", "ısı pompası sıcak kullanım suyu işletme süresi", "tempo di funzionamento del compressore")
MAKE_TRANSLATION(upTimeCompPool, "uptimecomppool", "operating time compressor pool", "Betriebszeit Kompressor Pool", "Bedrijfstijd compressor voor zwembadbedrijf", "Total tid kompressor pool", "łączny czas podgrzewania basenu (sprężarka)", "Total tid kompressor basseng", "durée de fonctionnement compresseur piscine", "ısı pompası havuz işletme süresi", "tempo di funzionamento del compressore piscina")
MAKE_TRANSLATION(totalCompStarts, "totalcompstarts", "total compressor control starts", "Kompressor Starts gesamt", "Totaal compressorstarts", "Kompressorstarter Totalt", "liczba załączeń sprężarki", "kompressorstarter totalt", "nombre démarrages total contrôle compresseur", "ısı pompası kontrolü toplam başlatma", "avvii totali del compressore")
MAKE_TRANSLATION(heatingStarts, "heatingstarts", "heating control starts", "Heizen Starts", "Starts verwarmingsbedrijf", "Kompressorstarter Uppvärmning", "liczba załączeń ogrzewania", "kompressorstarter oppvarming", "démarrages contrôle chauffage", "ısıtma kontrolü toplam başlatma", "avvii riscaldamento")
MAKE_TRANSLATION(coolingStarts, "coolingstarts", "cooling control starts", "Kühlen Starts", "Starts koelbedrijf", "Kompressorstarter Kyla", "liczba załączeń chłodzenia", "kompressorstarter kjøling", "démarrages contrôle refroidissement", "soğutma kontrolü toplam başlatma", "avvii raffreddamento")
MAKE_TRANSLATION(poolStarts, "poolstarts", "pool control starts", "Pool Starts", "Starts zwembadbedrijf", "Kompressorstarter Pool", "liczba załączeń podgrzewania basenu", "kompressorstarter basseng", "démarrages contrôle piscine", "havuz kontrolü toplam başlatma", "avvio controllato piscina")
MAKE_TRANSLATION(nrgConsTotal, "nrgconstotal", "total energy consumption", "Energieverbrauch gesamt", "Energieverbrauch gesamt", "Energiförbrukning totalt", "energia pobrana (sumarycznie)", "energiforbruk totalt", "consommation totale énergie", "toplam enerji tüketimi", "totale energia consumata")
MAKE_TRANSLATION(nrgConsCompTotal, "nrgconscomptotal", "total energy consumption compressor", "Energieverbrauch Kompressor gesamt", "Energieverbruik compressor totaal", "Energiförbrukning kompressor", "energia pobrana przez sprężarkę", "energiforbruk kompressor", "consommation totale énergie compresseur", "ısı pompası toplam enerji tüketimi", "totale energia consumata compressore")
MAKE_TRANSLATION(nrgConsCompHeating, "nrgconscompheating", "energy consumption compressor heating", "Energieverbrauch Kompressor heizen", "Energieverbruik compressor verwarmingsbedrijf", "Energiförbrukning uppvärmning", "energia pobrana przez sprężarkę na ogrzewanie", "energiforbruk oppvarming", "consommation  énergie compresseur chauffage", "ısı pompası ısıtma toplam enerji tüketimi", "consumo energia compressore riscaldamento")
MAKE_TRANSLATION(nrgConsCompWw, "nrgconscompww", "energy consumption compressor dhw", "Energieverbrauch Kompressor", "Energieverbruik compressor warmwaterbedrijf", "Energiförbrukning varmvatten", "energia pobrana przez sprężarkę na c.w.u.", "energiforbruk varmvann", "consommation  énergie compresseur ecs", "ısı pompası sıcak kullanım suyu toplam enerji tüketimi", "consumo energia compressore ACS")
MAKE_TRANSLATION(nrgConsCompCooling, "nrgconscompcooling", "energy consumption compressor cooling", "Energieverbrauch Kompressor kühlen", "Energieverbruik compressor koelbedrijf", "Energiförbrukning kyla", "energia pobrana przez sprężarkę na chłodzenie", "energiforbruk kjøling", "consommation  énergie compresseur refroidissement", "ısı pompası soğutma toplam enerji tüketimi", "consumo energia compressore raffreddamento")
MAKE_TRANSLATION(nrgConsCompPool, "nrgconscomppool", "energy consumption compressor pool", "Energieverbrauch Kompressor Pool", "Energiebedrijf compressor zwembadbedrijf", "Energiförbrukning pool", "energia pobrana przez sprężarkę na podgrzewanie basenu", "energiforbruk basseng", "consommation  énergie compresseur piscine", "ısı pompası havuz toplam enerji tüketimi", "consumo energia compressore piscina")
MAKE_TRANSLATION(nrgSuppTotal, "nrgsupptotal", "total energy supplied", "gesamte Energieabgabe", "Totaal opgewekte energie", "Genererad energi", "energia oddana (sumarycznie)", "tilført energi", "énergie totale fournie", "sağlanan toplam enerji", "totale energia fornita")
MAKE_TRANSLATION(nrgSuppHeating, "nrgsuppheating", "total energy supplied heating", "gesamte Energieabgabe heizen", "Opgewekte energie verwarmingsbedrijf", "Genererad energi Uppvärmning", "energia oddana na ogrzewanie", "tilført energi oppvarming", "énergie totale fournie chauffage", "ısıtma sağlanan toplam enerji", "energia totale fornita - riscaldamento")
MAKE_TRANSLATION(nrgSuppWw, "nrgsuppww", "total energy warm supplied dhw", "gesamte Energieabgabe", "Opgewekte energie warmwaterbedrijf", "Genererad energi Varmvatten", "energia oddana na c.w.u.", "tilført energi varmvann", "énergie chaude totale fournie ecs", "sıcak kullanım suyu sağlanan toplam enerji", "totale energia calorica fornita ACS")
MAKE_TRANSLATION(nrgSuppCooling, "nrgsuppcooling", "total energy supplied cooling", "gesamte Energieabgabe kühlen", "Opgewekte energie koelbedrijf", "Genererad energi Kyla", "energia oddana na chłodzenie", "Tillført energi kjøling", "énergie totale fournie refroidissement", "soğutma sağlanan toplam enerji", "energia totale fornita - raffreddamento")
MAKE_TRANSLATION(nrgSuppPool, "nrgsupppool", "total energy supplied pool", "gesamte Energieabgabe Pool", "Opgewekte energie zwembadbedrijf", "Genererad energi Pool", "energia oddana na podgrzewanie basenu", "tilført energi basseng", "énergie totale fournie piscine", "havuz sağlanan toplam enerji", "totale di energia fornita- piscina")
MAKE_TRANSLATION(auxElecHeatNrgConsTotal, "auxelecheatnrgconstotal", "total aux elec. heater energy consumption", "Energieverbrauch el. Zusatzheizung", "Totaal energieverbruik electrisch verwarmingselement", "Energiförbrukning Eltillkott", "energia pobrana przez grzałki", "energiforbruk varmekolbe", "consommation totale énergie electrique auxiliaire chauffage", "ilave elektrikli ısıtıcı toplam enerji tüketimi", "consumo energetico riscaldamento elettrico supplementare")
MAKE_TRANSLATION(auxElecHeatNrgConsHeating, "auxelecheatnrgconsheating", "aux elec. heater energy consumption heating", "Energieverbrauch el. Zusatzheizung Heizen", "Energieverbruik electrisch verwarmingselement voor verwarmingsbedrijf", "Energiförbrukning Eltillskott Uppvärmning", "energia pobrana przez grzałki na ogrzewanie", "energiforbruk varmekolbe oppvarming", "consommation énergie electrique auxiliaire chauffage", "ilave elektrikli ısıtıcı ısınma toplam enerji tüketimi", "consumo di energia riscaldamento elettrico ausiliario")
MAKE_TRANSLATION(auxElecHeatNrgConsWW, "auxelecheatnrgconsww", "aux elec. heater energy consumption dhw", "Energieverbrauch el. Zusatzheizung", "Energieverbruik electrisch verwarmingselement voor warmwaterbedrijf", "Energiförbrukning Eltillskott Varmvatten", "energia pobrana przez grzałki na c.w.u.", "energiförbruk varmekolbe varmvann", "consommation énergie electrique auxiliaire chauffage ecs", "ilave elektrikli ısıtıcı sıcak kullanım suyu toplam enerji tüketimi", "consumo di energia riscaldamento elettrico ausiliario ACS")
MAKE_TRANSLATION(auxElecHeatNrgConsPool, "auxelecheatnrgconspool", "aux elec. heater energy consumption pool", "Energieverbrauch el. Zusatzheizung Pool", "Energieverbruik electrisch verwarmingselement voor zwembadbedrijf", "Energiförbrukning Eltillskott Pool", "energia pobrana przez grzałki na podgrzewanie basenu", "energiforbruk el. tilleggsvarme basseng", "consommation énergie electrique auxiliaire chauffage piscine", "ilave elektrikli ısıtıcı havuz toplam enerji tüketimi", "consumo di energia riscaldamento elettrico ausiliario piscina")

MAKE_TRANSLATION(hpCompOn, "hpcompon", "hp compressor", "WP Kompressor", "WP compressor", "VP Kompressor", "sprężarka pompy ciepła", "vp kompressor", "compresseur pompe à chaleur", "hp ısı pompası", "compressore pompa calore")
MAKE_TRANSLATION(hpHeatingOn, "hpheatingon", "hp heating", "WP Heizen", "WP verwarmingsbedrijf", "VP Uppvärmning", "pompa ciepła, ogrzewanie", "vp oppvarmning", "hp ısınıyor", "riscaldamento pompa calore")
MAKE_TRANSLATION(hpCoolingOn, "hpcoolingon", "hp cooling", "WP Kühlen", "WP koelbedrijf", "VP Kyla", "pompa ciepła, chłodzenie", "vp kjøling", "hp soğuyor", "raffreddamento pompa calore")
MAKE_TRANSLATION(coolingOn, "coolingon", "cooling", "Kühlen", "koelbedrijf", "Kyla", "chłodzenie", "kjøling", "refroidissement", "hp sıcak kullanım suyu", "") // TODO translate
MAKE_TRANSLATION(hpWwOn, "hpwwon", "hp dhw", "WP Warmwasser", "WP warmwaterbedrijf", "VP Varmvatten", "pompa ciepła, c.w.u.", "vp varmvatten", "hp havuz", "acqua calda pompa calore")
MAKE_TRANSLATION(hpPoolOn, "hppoolon", "hp pool", "WP Pool", "WP zwembadbedrijf", "VP Pool", "pompa ciepła, podgrzewanie basenu", "vp basseng", "tuzlu su pompası hızı", "pompa calore piscina")
MAKE_TRANSLATION(hpBrinePumpSpd, "hpbrinepumpspd", "brine pump speed", "Solepumpen-Geschw.", "Snelheid pekelpomp", "Hastighet Brine-pump", "wysterowanie pompy glikolu", "hastighet brine-pumpe", "vitesse pompe à saumure", "ısı pompası hızı", "velocità pompa sbrinamento")
MAKE_TRANSLATION(hpCompSpd, "hpcompspd", "compressor speed", "Kompressor-Geschw.", "Snelheid compressor", "Kompressorhastighet", "wysterowanie sprężarki", "kompressorhastighet", "vitesse du compresseur", "sirkülasyon pompası hızı", "velocità compressore")
MAKE_TRANSLATION(hpCircSpd, "hpcircspd", "circulation pump speed", "Zirkulationspumpen-Geschw.", "Snelheid circulatiepomp", "Hastighet Cirkulationspump", "wysterowanie pompy obiegu grzewczego", "hastighet sirkulationspumpe", "vitesse pompe à circulation", "evaporatör tuzlu su giişi", "velocità pompa circolazione")
MAKE_TRANSLATION(hpBrineIn, "hpbrinein", "brine in/evaporator", "Sole in/Verdampfer", "pekel in/verdamper", "Brine in (förangare)", "temperatura glikolu na wejściu kolektora (TB0)", "brine in/fordamper", "entrée saumure/évaporateur", "kondenser tuzlu su çıkışı", "salamoia nell evaporatore")
MAKE_TRANSLATION(hpBrineOut, "hpbrineout", "brine out/condenser", "Sole aus/Kondensator", "pekel uit/condensor", "Brine ut (kondensor)", "temperatura glikolu na wyjściu kolektora (TB1)", "Brine ut/kondensor", "sortie saumure/condenseur", "anahtar valfi", "salamoia nell uscita evaporatore")
MAKE_TRANSLATION(hpSwitchValve, "hpswitchvalve", "switch valve", "Schaltventil", "schakelklep", "Växelventil", "zawór przełączający", "skifteventil", "valve de commutation", "ısı pompası aktivitesi", "valvola commutazione pompa di calore")
MAKE_TRANSLATION(hpActivity, "hpactivity", "compressor activity", "Kompressoraktivität", "Compressoractiviteit", "Kompressoraktivitet", "pompa ciepła, aktywność sprężarki", "kompressoraktivitet", "hp ısı pompası", "attività compressore")

MAKE_TRANSLATION(hpPower, "hppower", "compressor power output", "Kompressorleistung", "Compressorvermogen", "Kompressoreffekt", "moc wyjściowa sprężarki", "kompressoreffekt", "puissance de sortie compresseur", "ısı pompası güç çıkışı", "potenza uscita compressore")
MAKE_TRANSLATION(hpTc0, "hptc0", "heat carrier return (TC0)", "Kältemittel Rücklauf (TC0)", "Koudemiddel retour (TC0)", "Värmebärare Retur (TC0)", "temperatura nośnika ciepła na powrocie (TC0)", "kjølemiddel retur (TC0)", "retour caloporteur (TC0)", "sıcak su dönüşü (TC0)", "ritorno del refrigerante (TC0)")
MAKE_TRANSLATION(hpTc1, "hptc1", "heat carrier forward (TC1)", "Kältemittel Vorlauf (TC1)", "Koudemiddel aanvoer (TC1)", "Värmebärare Framledning (TC1)", "temperatura nośnika ciepła pierwotna (TC1)", "kjølemiddel tur (TC1)", "avance caloporteur (TC1)", "sıcak su çıkışı (TC1)", "flusso di refrigerante (TC1)")
MAKE_TRANSLATION(hpTc3, "hptc3", "condenser temperature (TC3)", "Verflüssigertemperatur (TC3)", "Condensortemperatuur (TC3)", "Kondensortemperatur (TC3)", "temperatura skraplacza/na wyjściu sprężarki (TC3)", "kondensortemperatur (TC3)", "température condensateur (TC3)", "kondenser sıcaklığı (TC3)", "temperatura condensatore (TC3)")
MAKE_TRANSLATION(hpTr1, "hptr1", "compressor temperature (TR1)", "Kompessortemperatur (TR1)", "Compressor temperatuur (TR1)", "Kompressor temp (TR1)", "temperatura sprężarki (TR1)", "kompressor temperatur (TR1)", "température compresseur (TR1)", "ısı pompası sıcaklığı (TR1)", "temperatura compressore (TR1)")
MAKE_TRANSLATION(hpTr3, "hptr3", "refrigerant temperature liquid side (condenser output) (TR3)", "Kältemittel (flüssig) (TR3)", "Temperatuur koudemiddel vloeibare zijde (TR3)", "Köldmedium temperatur (kondensorutlopp) (TR3)", "temperatura skraplacza ogrzew. (TR3)", "kjølemiddeltemperatur på væskesiden (TR3)", "température réfrigérant côté liquide (sortie condensateur) (TR3)", "ısı pompası çıkışı (TR3)", "temperatura refrigerante lato liquido (uscita condensatore) (TR3)")
MAKE_TRANSLATION(hpTr4, "hptr4", "evaporator inlet temperature (TR4)", "Verdampfer Eingang (TR4)", "Verdamper ingangstemperatuur (TR4)", "Förångare inloppstemp (TR4)", "temperatura na wejściu parownika (TR4)", "innløpstemperatur for fordamperen (TR4)", "température entrée évaporateur (TR4)", "evaporatör giriş sıcaklığı (TR4)", "temperatura di ingresso dell'evaporatore (TR4)")
MAKE_TRANSLATION(hpTr5, "hptr5", "compressor inlet temperature (TR5)", "Kompessoreingang (TR5)", "Compressor ingangstemperatuur (TR5)", "Kompressor inloppstemp (TR5)", "temperatura na wejściu sprężarki (TR5)", "kompressor innløpstemp (TR5)", "température entrée compresseur (TR5)", "ısı pompası giriş sıcaklığı (TR5)", "temperatura di ingresso del compressore (TR5)")
MAKE_TRANSLATION(hpTr6, "hptr6", "compressor outlet temperature (TR6)", "Kompressorausgang (TR6)", "Compressor uitgangstemperatuur (TR6)", "Kompressor utloppstemp (TR6)", "temperatura na wyjściu sprężarki (TR6)", "kompressor utløpstemp (TR6)", "température sortie compresseur (TR6)", "ısı pompası çıkış sıcaklığı (TR6)", "temperatura di uscita del compressore (TR6)")
MAKE_TRANSLATION(hpTr7, "hptr7", "refrigerant temperature gas side (condenser input) (TR7)", "Kältemittel (gasförmig) (TR7)", "Temperatuur koudemiddel gasvormig (TR7)", "Köldmedium temperatur gassida (kondensorinlopp) (TR7)", "temperatura czynnika chłodniczego po stronie gazu (wejście skraplacza) (TR7)", "kjølemedium temperatur gassida (kondensatorinløp) (TR7)", "température réfrigérant côté gaz (sortie condensateur) (TR7)", "kondenser giriş sıcaklığı (TR7)", "temperatura refrigerante lato gas (ingresso condensatore) (TR7)")
MAKE_TRANSLATION(hpTl2, "hptl2", "air inlet temperature (TL2)", "Außenluft-Einlasstemperatur (TL2)", "Temperatuur luchtinlaat (TL2)", "Luftintagstemperatur (TL2)", "temperatura wlotu powietrza (TL2)", "luftinntakstemperatur (TL2)", "température entrée air (TL2)", "hava giriş sıcaklığı (TL2)", "temperatura ingresso aria (TL2)")
MAKE_TRANSLATION(hpPl1, "hppl1", "low pressure side temperature (PL1)", "Niederdruckfühler (PL1)", "Temperatuur lage drukzijde (PL1)", "Temperatur Lågtryckssidan (PL1)", "temperatura po stronie niskiego ciśnienia (PL1)", "temperatur lavtrykksiden (PL1)", "température côté basse pression (PL1)", "düşük basınç tarafı sıcaklığı (PL1)", "temperatura lato bassa pressione (PL1)")
MAKE_TRANSLATION(hpPh1, "hpph1", "high pressure side temperature (PH1)", "Hochdruckfühler (PH1)", "Temperatuur hoge drukzijde (PH1)", "Temperatur Högtryckssidan (PH1)", "temperatura po stronie wysokiego ciśnienia (PH1)", "Temperatur Høytrykksiden (PH1)", "température côté bhauteasse pression (PH1)", "yüksek basınç tarafı sıcaklığı (PH1)", "temperatura lato alta pressione (PH1)")
MAKE_TRANSLATION(hpTa4, "hpta4", "drain pan temp (TA4)", "Kondensatorwanne (TA4)", "Temperatuur condensorafvoerbak", "", "temperatura ociekacza (TA4)", "kondens temperatur", "", "tahliye sıcaklığı (TA4)", "temperatura condensatore (TA4)") // TODO translate

MAKE_TRANSLATION(hpInput1, "hpin1", "input 1 state", "Eingang 1 Status", "Status input 1", "Status Ingång 1", "stan wejścia 1", "status inggang 1", "état entrée 1", "giriş 1 durumu", "stato ingresso 1")
MAKE_TRANSLATION(hpInput2, "hpin2", "input 2 state", "Eingang 2 Status", "Status input 2", "Status Ingång 2", "stan wejścia 2", "status inggang 2", "état entrée 2", "giriş 2 durumu", "stato ingresso 2")
MAKE_TRANSLATION(hpInput3, "hpin3", "input 3 state", "Eingang 3 Status", "Status input 3", "Status Ingång 3", "stan wejścia 3", "status inggang 3", "état entrée 3", "giriş 3 durumu", "stato ingresso 3")
MAKE_TRANSLATION(hpInput4, "hpin4", "input 4 state", "Eingang 4 Status", "Status input 4", "Status Ingång 4", "stan wejścia 4", "status inggang 4", "état entrée 4", "giriş 4 durumu", "stato ingresso 4")
MAKE_TRANSLATION(hpIn1Opt, "hpin1opt", "input 1 options", "Eingang 1 Einstellung", "Instelling input 1", "Inställningar Ingång 1", "opcje wejścia 1", "innstillinger inngang 1", "options entrée 1", "giriş 1 seçenekleri", "impostazioni ingresso 1")
MAKE_TRANSLATION(hpIn2Opt, "hpin2opt", "input 2 options", "Eingang 2 Einstellung", "Instelling input 2", "Inställningar Ingång 2", "opcje wejścia 2", "innstillinger inngang 2", "options entrée 2", "giriş 2 seçenekleri", "impostazioni ingresso 2")
MAKE_TRANSLATION(hpIn3Opt, "hpin3opt", "input 3 options", "Eingang 3 Einstellung", "Instelling input 3", "Inställningar Ingång 3", "opcje wejścia 3", "innstillinger inngang 3", "options entrée 3", "giriş 3 seçenekleri", "impostazioni ingresso 3")
MAKE_TRANSLATION(hpIn4Opt, "hpin4opt", "input 4 options", "Eingang 4 Einstellung", "Instelling input 4", "Inställningar Ingång 4", "opcje wejścia 4", "innstillinger inngang 4", "options entrée 4", "giriş 4 seçenekleri", "impostazioni ingresso 4")
MAKE_TRANSLATION(maxHeatComp, "maxheatcomp", "heat limit compressor", "Heizgrenze Kompressor", "heat limit compressor", "heat limit compressor", "ograniczenie mocy sprężarki", "max varmegrense kompressor", "limite chaleur compresseur", "ısı pompası ısıtma sınırı", "limite riscaldamento compressore")
MAKE_TRANSLATION(maxHeatHeat, "maxheatheat", "heat limit heating", "Heizgrenze Heizen", "heat limit heating", "heat limit heating", "ograniczenie mocy w trybie ogrzewania", "maks varmegrense oppvarming", "limite chaleur chauffage", "ısınma ısıtma sınırı", "limite calore riscaldamento")
MAKE_TRANSLATION(maxHeatDhw, "maxheatdhw", "heat limit dhw", "Heizgrenze Warmwasser", "heat limit dhw", "heat limit dhw", "ograniczenie mocy w trybie c.w.u.", "varmegrense varmtvann", "limite chaleur ecs", "sıcak kullanım suyu ısınma sınırı", "limite calore ACS")

MAKE_TRANSLATION(auxHeaterOff, "auxheateroff", "disable aux heater", "Verbiete Zusatzheizer", "Bijverwarming uitsc", "Blockera eltillskott", "wyłącz dogrzewacz", "deaktiver tilleggsvarme", "Désactiver chauff. d'app", "ilave ısıtıcıyı kapat", "disattivare i riscaldatori addizionali")
MAKE_TRANSLATION(auxHeaterStatus, "auxheaterstatus", "aux heater status", "Status Zusatzheizer", "Bijverwarming", "Eltillskott Status", "status dogrzewacza", "status el. tillegsvarme", "Chauffage auxiliaire", "ilave ısıtıcı durumu", "stato riscaldatori addizionali")
MAKE_TRANSLATION(auxHeaterOnly, "auxheateronly", "aux heater only", "nur Zusatzheizer", "Alleen bijverwarming", "Eltillskott Enbart", "tylko dogrzewacz", "kun el tilleggsvarme", "Que chauffage auxiliaire", "sadece ilave ısıtıvcı", "solo riscaldatori addizionali")
MAKE_TRANSLATION(auxHeaterDelay, "auxheaterdelay", "aux heater on delay", "Zusatzheizer verzögert ein", "Bijverw. vertraagd aan", "Eltillskottfördröjning på", "opóźnienie włączenia dogrzewacza", "Tilleggsvarmer forsinket på", "Chauff app tempo marche", "ilave ısıtıcı beklemede", "ritardo riscaldatori addizionali")
MAKE_TRANSLATION(silentMode, "silentmode", "silent mode", "Silentmodus", "Stiller gebruik", "Tyst läge", "tryb cichy",  "stille modus", "Fct silencieux", "sessiz mod", "modalità silenziosa")
MAKE_TRANSLATION(minTempSilent, "mintempsilent", "min outside temp for silent mode", "Minimale Aussentemperatur Silentmodus", "Stiller gebruik min. buitentemp", "Tyst läge min temp", "minimalna temperatura zewnętrzna dla trybu cichego", "atille modus min temp", "Fct silencieux: Temp. extérieure min.", "sessiz mod için min. dış ortam sıcaklığı", "modalità silenziosa temperatura esterna minima")
MAKE_TRANSLATION(tempParMode, "tempparmode", "outside temp parallel mode", "Aussentemperatur Parallelmodus", "Buitentemp. parallelbedr", "Parallelläge Utomhustemp.", "maksymalna temperatura zewnętrzna dla dogrzewacza", "", "Temp. ext. fct parallèle", "paralel mod dış ortam sıcaklığı", "modalità parallela temperatura esterna") // TODO translate
MAKE_TRANSLATION(auxHeatMixValve, "auxheatmix", "aux heater mixing valve", "Mischer Zusatzheizer", "Bijverwarming menger", "Eltilskott Blandarventil", "mieszacz dogrzewacza", "eltilskudd blandeventil", "Chauffage auxiliaire mélangeur", "ilave ısıtıcı karışım vanası", "miscela riscaldatori addizionali")
MAKE_TRANSLATION(hpHystHeat, "hphystheat", "on/off hyst heat", "Schalthysterese Heizen", "Aan/uit-hysteresis in verw. bedrijf", "Hstereses Uppvärm.", "histereza wł./wył. ogrzewania", "På/av hysterese Oppvar.", "Hystérésis Marche en mode chauffage", "ısıtma gecikmesi", "isteresi di commutazione riscaldamento")
MAKE_TRANSLATION(hpHystCool, "hphystcool", "on/off hyst cool", "Schalthysterese Kühlen", "Aan/uit-hysteresis in koelbedrijf ", "Hystereses Kyla", "histereza wł./wył. chłodzenia", "hystrese kjøling", "Hystérésis Marche en mode refroidissement", "soğutma gecikmesi", "isteresi di commutazione raffreddamento")
MAKE_TRANSLATION(hpHystPool, "hphystpool", "on/off hyst pool", "Schalthysterese Pool", "an/uit-hysteresis in zwembadbedri", "Hystereses Pool", "histereza wł./wył. podgrzewania basenu", "hystrese basseng", "Hystérésis Marche en mode piscine", "havuz gecikmesi", "isteresi di commutazione piscina")
MAKE_TRANSLATION(tempDiffHeat, "tempdiffheat", "temp diff TC3/TC0 heat", "Temp.diff. TC3/TC0 Heizen", "Temp.vers. TC3/TC0 verw", "Delta(T) TC3/TC0 Uppvärm.", "różnica temperatur TC3/TC0 w trakcie ogrzewania", "temp. diff. TC3/TC0 oppvarm", "Delta T TC3/TC0 Chauff", "TC3-TC0 ısıtma sıcaklık farkı", "Delta T riscaldamento TC3/TC0")
MAKE_TRANSLATION(tempDiffCool, "tempdiffcool", "temp diff TC3/TC0 cool", "Temp.diff. TC3/TC0 Kühlen", "Temp.vers. TC3/TC0 koel.", "Delta(T) TC3/TC0 Kyla", "różnica temperatur TC3/TC0 w trakcie chłodzenia", "temp. diff. TC3/TC0 kjøling", "Delta T TC3/TC0 Refroid.", "TC3-TC0 soğutma sıcaklık farkı", "Delta T raffreddamento TC3/TC0")
MAKE_TRANSLATION(silentFrom, "silentfrom", "silent mode from", "Silentmodus Start", "Start stille modus", "", "początek trybu cichego", "stillemodus starter", "", "sessiz mod başlangıcı", "avvio della modalità silenziosa") // TODO translate
MAKE_TRANSLATION(silentTo, "silentto", "silent mode to", "Silentmodus Ende", "Einde stille modus", "", "koniec trybu cichego", "komfortmodus av", "", "sessiz mod bitişi", "spegnere modalità silenziosa") // TODO translate

MAKE_TRANSLATION(wwComfOffTemp, "wwcomfoff", "comfort switch off", "Komfort Ausschalttemp", "Comfort Uitschakeltemp.", "Komfortläge avstängingstemp.", "temperatura wyłączania w trybie komfort", "eco modus utkoblingstem", "Confort Temp. d'arrêt", "konfor kapalı", "spegnimento modalità comfort")
MAKE_TRANSLATION(wwEcoOffTemp, "wwecooff", "eco switch off", "ECO Ausschalttemp", "Eco Uitschakeltemp.", "Ekoläge avstängningstemp.", "temperatura wyłączania w trybie eko", "Øko avstengningstemp.", "Eco Temp. d'arrêt", "eko kapalı", "spegnimento modalità ECO")
MAKE_TRANSLATION(wwEcoPlusOffTemp, "wwecoplusoff", "eco+ switch off", "ECO+ Ausschalttemp", "Eco+ Uitschakeltemp.", "Eko+ avstängningstemp.", "temperatura wyłączania w trybie eko+", "Øko+ avstengningstemp.", "Eco+ Temp. d'arrêt", "eko+ kapalı", "spegnimento modalità ECO+")

MAKE_TRANSLATION(auxHeatMode, "auxheatrmode", "aux heater mode", "Modus Zusatzheizer", "Modus bijverwarmer", "", "tryb pracy dogrzewacza po blokadzie z Zakładu Energetycznego", "tilleggsvarmer modus", "", "ilave ısıtıcı modu", "modalità riscaldatore addizionale") // TODO translate
MAKE_TRANSLATION(auxMaxLimit, "auxmaxlimit", "aux heater max limit", "Zusatzheizer max. Grenze", "Bijverwarmer grensinstelling maximaal", "", "dogrzewacz, maksymalny limit", "tillegsvarme maksgrense", "ilave ısıtıcı maks limit", "limite massimo riscaldatore addizionale") // TODO translate
MAKE_TRANSLATION(auxLimitStart, "auxlimitstart", "aux heater limit start", "Zusatzheizer Grenze Start", "Bijverwarmer grens voor start", "", "dogrzewacz, początek ograniczenia", "tillegsvarme startgrense", "ilave ısıtıcı limir başlangıcı", "avvio limite massimo riscaldatore addizionale") // TODO translate
MAKE_TRANSLATION(manDefrost, "mandefrost", "manual defrost", "Manuelle Enteisung", "Handmatige ontdooicyclus", "", "ręczne odladzanie", "manuell avisning", "", "manuel buz çözme", "sbrinamento manuale") // TODO translate
MAKE_TRANSLATION(pvCooling, "pvcooling", "Cooling only with PV", "Kühlen nur mit PV", "Koelen alleen met solar PV", "", "chłodzenie tylko z PV", "kjøling med solpanel", "", "sadece PV ile soğutma", "solo raffrescamento con solare") // TODO translate
MAKE_TRANSLATION(hpCircPumpWw, "hpcircpumpww", "circulation pump available during dhw", "Zirkulation möglich bei WW-Bereitung", "Circulatiepomp WP beschikbaar tijdens ww", "", "pompa cyrkulacji dostępna w trakcie c.w.u.", "sirkulasjonspumpe tilgjengelig under varmtvann", "", "SKS esnasında sirkülasyon pompasu uygun", "pompa di circolazione disponibile durante ACS") // TODO translate
MAKE_TRANSLATION(vp_cooling, "vpcooling", "valve/pump cooling", "Ventil/Pumpe für Kühlen", "Klep koeling", "", "zawór/pompa chłodzenia", "varmepumpe kjøling", "", "vana/pompa soğuyor", "valvola/pompa raffrescamento") // TODO translate
MAKE_TRANSLATION(VC0valve, "vc0valve", "VC0 valve", "VC0 Ventil", "Klep VC0", "", "zawór VC0", "vc0 ventil", "", "VC0 vana", "valvola VC0") // TODO translate
MAKE_TRANSLATION(primePump, "primepump", "primary heatpump", "Hauptpumpe", "Hoofdpomp", "", "główna pompa ciepła", "primærpumpe", "", "ana ısı pompası", "pompa principale riscaldamento") // TODO translate
MAKE_TRANSLATION(primePumpMod, "primepumpmod", "primary heatpump modulation", "Modulation Hauptpumpe", "Modulatie hoofdpomp", "", "wysterowanie głównej pompy ciepła", "primærpumpelast", "", "ana ısı pompası modülasyon", "pompa principale modulazione riscaldamento") // TODO translate
MAKE_TRANSLATION(hp3wayValve, "hp3way", "3-way valve", "3-Wege-Ventil", "3-weg klep", "", "zawór 3-drogowy pompy ciepła", "3-veisventil", "", "3 yollu vana", "valvola 3-vie") // TODO translate
MAKE_TRANSLATION(elHeatStep1, "elheatstep1", "el. heater step 1", "El. Heizer Stufe 1", "Electrische bijverwarmer niveau 1", "", "dogrzewacz poziom 1", "el-kolbe steg 1", "", "el.ısıtıcı adım 1", "riscaldatore elettrico livello 1") // TODO translate
MAKE_TRANSLATION(elHeatStep2, "elheatstep2", "el. heater step 2", "El. Heizer Stufe 2", "Electrische bijverwarmer niveau 2", "", "dogrzewacz poziom 2", "el-kolbe steg 2", "", "el.ısıtıcı adım 2", "riscaldatore elettrico livello 2") // TODO translate
MAKE_TRANSLATION(elHeatStep3, "elheatstep3", "el. heater step 3", "El. Heizer Stufe 3", "Electrische bijverwarmer niveau 3", "", "dogrzewacz poziom 3", "el-kolbe steg 3", "", "el.ısıtıcı adım 3", "riscaldatore elettrico livello 3") // TODO translate
MAKE_TRANSLATION(wwAlternatingOper, "wwalternatingop", "alternating operation", "Wechselbetrieb", "Wisselbedrijf ww", "", "praca naprzemienna", "alternativ drift", "", "sıcak kullanım suyu alternatif işletim", "funzionamento alternato") // TODO translate
MAKE_TRANSLATION(wwAltOpPrioHeat, "wwaltopprioheat", "prioritise heating during dhw", "Heizen bevorzugt vor WW", "Proriteit verwarming boven ww", "", "czas na ogrzewanie w trakcie c.w.u", "prioritert oppvarmning", "", "sıcak kullanım suyu esnasında ısıtmayı öne al", "dare la priorità al riscaldamento durante l'ACS") // TODO translate
MAKE_TRANSLATION(wwAltOpPrioWw, "wwaltopprioww", "prioritise dhw during heating", "WW bevorzugt vor Heizen", "Prioriteit ww boven verwarming", "", "czas na c.w.u w trakcie ogrzewania", "prioritert varmtvann", "", "ısıtma esnasında sıcak kullanım suyunu öne al", "dare priorità all'acqua calda durante il riscaldamento") // TODO translate

// hybrid heatpump
MAKE_TRANSLATION(hybridStrategy, "hybridstrategy", "hybrid control strategy", "Hybrid Strategie", "Hybride strategie", "Hybrid kontrollstrategi", "strategia sterowania hybrydowego", "hybrid kontrollstrategi", "stratégie contrôle hybride", "hibrit kontrol stratejisi", "strategia comtrollo ibrido")
MAKE_TRANSLATION(switchOverTemp, "switchovertemp", "outside switchover temperature", "Außentemperatur für Umschaltung", "Schakeltemperatuur buitentemperatuur", "Utomhus Omställningstemperatur", "zewnętrzna temperatura przełączania", "utendørstemp styring", "basculement par température extérieure", "geçiş için dış sıcaklık", "temperatura esterna per commutazione")
MAKE_TRANSLATION(energyCostRatio, "energycostratio", "energy cost ratio", "Energie/Kosten-Verhältnis", "Energiekostenratio", "Energi/Kostnads-förhållande", "współczynnik energia/koszt", "energi/kostnads forhold", "ratio coût énergie", "enerji maliyet oranı", "rapporto energia/costo")
MAKE_TRANSLATION(fossileFactor, "fossilefactor", "fossile energy factor", "Energiefaktor Fossil", "Energiefactor fossiele brandstof", "Energifaktor fossilenergi", "udział energii z paliw kopalnych", "energifaktor fossilenergi", "facteur énergie fossile", "fosil yakıt faktörü", "fattore energia fossile")
MAKE_TRANSLATION(electricFactor, "electricfactor", "electric energy factor", "Energiefaktor elektrisch", "Energiefactor electrisch", "Elektrisk energifaktor", "udział energii elektrycznej", "elektrisk energifaktor", "facteur énergie électrique", "elektrik enerjisi faktörü", "fattore energia elettrica")
MAKE_TRANSLATION(delayBoiler, "delayboiler", "delay boiler support", "Verzögerungs-Option", "Vertragingsoptie", "Fördröjningsoption", "opcja opóźnienia", "Fördörjningsoption", "option retardement chaudière", "kazan desteğini ötele", "opzione ritardo caldaia")
MAKE_TRANSLATION(tempDiffBoiler, "tempdiffboiler", "temp diff boiler support", "Temperaturdifferenz-Option", "Verschiltemperatuuroptie", "Temperaturskillnadsoption", "opcja różnicy temperatur", "temperatursforskjell kjele", "option différence température", "sıcaklık farkı kazan desteği", "opzione differenza temperatura")
MAKE_TRANSLATION(lowNoiseMode, "lownoisemode", "low noise mode", "Geräuscharmer Betrieb", "Stil bedrijf", "Tyst läge", "tryb cichy", "stillemodus", "mode faible bruit", "düşük ses modu", "modalità a basso rumore")
MAKE_TRANSLATION(lowNoiseStart, "lownoisestart", "low noise starttime", "Start geräuscharmer Betrieb", "Start stil bedrijf", "Tyst läge starttid", "początek trybu cichego", "stille modu starttid", "heure démarrage faible bruit", "düşük ses başlangıç", "ora di avvio a basso rumore")
MAKE_TRANSLATION(lowNoiseStop, "lownoisestop", "low noise stoptime", "Stopp geräuscharmer Betrieb", "Stop stil bedrijf", "Tyst läge stopptid", "koniec trybu cichego", "stille modus stopptid", "heure arrêt faible bruit", "düşük ses bitiş", "ora di arresto funzionamento silenzioso")
MAKE_TRANSLATION(energyPriceGas, "energypricegas", "energy price gas", "Energiepreis Gas", "Energieprijs gas", "Gaspris", "cena energii z gazu", "energipris gass", "prix énergie gaz", "gaz enerjisi fiyatı", "prezzo energia gas")
MAKE_TRANSLATION(energyPriceEl, "energypriceel", "energy price electric", "Energiepreis Eletrizität", "energieprijs electriciteit", "Elpris", "cena energii elektrycznej", "strømpris", "prix énergie électrique", "elektrik enerjisi fiyatı", "prezzo energia elettrica")
MAKE_TRANSLATION(energyPricePV, "energyfeedpv", "feed in PV", "PV Einspeisevergütung", "PV teruglevertarief", "PV Energi", "zasilanie energią PV", "strømpris PV", "alimentation PV", "giren güneş enerjisi", "energia fotovoltaico")
MAKE_TRANSLATION(hybridDHW, "hybriddhw", "hybrid DHW", "Hybrid Warmwasser", "Hybride ww", "Hybridläge varmvatten", "hybrydowa c.w.u.", "hybridmodus varmtvann", "ecs hybride", "hibrit SKS", "ACS ibrida")
MAKE_TRANSLATION(airPurgeMode, "airpurgemode", "air purge mode", "Luftspülung", "Luchtzuivering", "Luftreningsläge", "tryb oczyszczania powietrza", "luftsrensningsmodus", "mode purge air", "hava temizleme modu", "modalita spurgo aria")
MAKE_TRANSLATION(heatPumpOutput, "heatpumpoutput", "heatpump output", "WP Leistung", "WP output", "Värmepumpseffekt", "moc wyjściowa pompy ciepła", "varmepumpeeffekt", "sortie pompe à chaleur", "ısı pompası çıkışı", "prestazione pompa calore")
MAKE_TRANSLATION(coolingCircuit, "coolingcircuit", "cooling circuit", "Kühlkreislauf", "Koelcircuit", "Kylkrets", "obwód chłodzący", "kjølekrets", "circuit refroidissement", "soğutma devresi", "circuito raffreddante")
MAKE_TRANSLATION(compStartMod, "compstartmod", "compressor start modulation", "Kompressor Startleistung", "Beginvermogen compressor", "Kompressor startmodulering", "początkowa modulacja sprężarki", "kompressor startmodulering", "modulation démarrage compresseur", "kazan başlangıç modülasyonu", "avvio modulazione compressore")
MAKE_TRANSLATION(heatDrainPan, "heatdrainpan", "heat drain pan", "Wärmeausgleichsgefäß", "Vereffeningsvat", "Uppvärm. dränering", "zbiornik wyrównawczy ciepła", "oppvarming drenering", "bac récupération chaleur", "ısı tahliye tablası", "serbatoio scarico condensa")
MAKE_TRANSLATION(heatCable, "heatcable", "heating cable", "Heizband", "heating cable", "värmekabel", "przewód grzejny", "varmekabel", "câble chauffant", "ısıtma kablosu", "cavo riscaldante")

// alternative heatsource AM200
MAKE_TRANSLATION(aCylTopTemp, "cyltoptemp", "cylinder top temperature", "Speichertemperatur Oben", "Buffer temperatuur boven", "Cylindertemperatur Toppen", "temperatura na górze cylindra", "beredertemperatur topp", "température haut cylindre", "silindir üst yüzey sıcaklığı", "temperatura superiore accumulo")
MAKE_TRANSLATION(aCylCenterTemp, "cylcentertemp", "cylinder center temperature", "Speichertemperatur Mitte", "Buffer temperatuur midden", "Cylindertemperatur Mitten", "temperatura na środku cylindra", "beredertemperatur midten", "température centre cylindre", "silindir merkez sıcaklığı", "temperatura centrale accumulo")
MAKE_TRANSLATION(aCylBottomTemp, "cylbottomtemp", "cylinder bottom temperature", "Speichertemperatur Unten", "Buffer temperatuur onder", "Cylindertemperatur Botten", "temperatura na dole cylindra", "beredertemperatur nederst", "température fond cylindre", "silindir taban sıcaklığı", "temperatura inferiore accumulo")
MAKE_TRANSLATION(aFlowTemp, "altflowtemp", "alternative hs flow temperature", "Alternativer WE Vorlauftemperatur", "Alternatieve warmtebron aanvoertemperatuur", "Alternativ flödestemp värmekälla", "temperatura zasilania z alternatywnego źródła", "alternativ varmekilde tilførselstemperatur", "température flux hs alternative", "alternatif ısı kaynağı besleme sıcaklığı", "temperatura alternativa mandata hs")
MAKE_TRANSLATION(aRetTemp, "altrettemp", "alternative hs return temperature", "Alternativer WE Rücklauftemperatur", "Alternatieve warmtebron retourtemperatuur", "Alternativ returtemp värmekälla", "temperatura powrotu z alternatywnego źródła", "alternativ varmekilde returtemperatur", "température retour hs alternative", "alternatif ısı kaynağı dönüş sıcaklığı", "temperatura alternativa ritorno hs")
MAKE_TRANSLATION(sysFlowTemp, "sysflowtemp", "system flow temperature", "System Vorlauftemperatur", "Systeem aanvoertemperatuur", "Systemflödestemperatur", "temperatura zasilania systemu", "systemturtemperatur", "température flux système", "sistem besleme sıcaklığı", "temperatura di mandata impianto")
MAKE_TRANSLATION(sysRetTemp, "sysrettemp", "system return temperature", "System Rücklauftemperatur", "Systeem retourtemperatuur", "Systemreturtemperatur", "temperatura powrotu z systemu", "systemreturtemperatur", "température retour système", "sistem dönüş sıcaklığı", "temperatura di ritorno impianto")
MAKE_TRANSLATION(valveByPass, "valvebypass", "bypass valve", "Bypass-Ventil", "Bypass klep", "Bypassventil", "zawór obejścia", "bypassventil", "vanne dérivation", "baypas vanası", "valvola Bypass")
MAKE_TRANSLATION(valveBuffer, "valvebuffer", "buffer valve", "Puffer-Ventil", "Bufferklep", "Buffertventil", "zawór bufora", "buffertventil", "vanne tampon", "tampon vanası", "valvola tampone")
MAKE_TRANSLATION(valveReturn, "valvereturn", "return valve", "Rückfluss-Ventil", "Retourklep", "Returventil", "zawór powrotu", "returventil", "vanne retour", "dönüş vanası", "valvola ritorno")
MAKE_TRANSLATION(aPumpMod, "apumpmod", "alternative hs pump modulation", "Alternativer WE Pumpenmodulation", "Alternatieve warmtebron pomp modulatie", "Alternativ Pumpmodulering Värmekälla", "modulacja pompy alternatywnego źródła ciepła", "alternativ pumpemodulering varmekilde", "modulation alternative pompe hs", "alternatif ısı kaynağı pompa modülasyonu", "pompa modulazione alternativa hs")
MAKE_TRANSLATION(heatSource, "heatsource", "alternative heating active", "Alternativer Wärmeerzeuger aktiv", "Alternatieve warmtebron aktief", "Alternativ Värmekälla aktiv", "aktywne alternatywne źródło ciepła", "alternativ varmekilde aktiv", "chauffage alternatif actif", "alternatif ısınma devrede", "riscaldamento alternativo attivo")
MAKE_TRANSLATION(aPump, "apump", "alternative hs pump", "Alternativer WE Pumpe", "Alternatieve warmtebron pomp", "Alternativ Pump Värmekälla", "pompy alternatywnego źródła ciepła", "alternativ pumpe varmekilde", "alternative pompe hs", "alternatif ısı kaynağı pompası", "pompa alternativa hs")
MAKE_TRANSLATION(burner, "burner", "burner", "Brenner", "Brander", "", "palnik", "", "", "kazan", "bruciatore") // TODO translate
MAKE_TRANSLATION(heatRequest, "heatrequest", "heat request", "Wärmeanforderung", "Warmtevraag", "", "zapotrzebowanie na ciepło", "varmeforespørsel", "", "ısı talebi", "richiesta calore") // TODO translate
MAKE_TRANSLATION(blockRemain, "blockremain", "remaining blocktime", "verbleibende Blockzeit", "Resterende bloktijd", "", "czas do końca blokady", "gjenstående blokkeringstid", "", "kalan blok süresi", "tempo di blocco rimanente") // TODO translate
MAKE_TRANSLATION(blockRemainWw, "blockremainww", "remaining blocktime dhw", "verbleibende Blockzeit WW", "Resterende bloktijd ww", "", "czas do końca blokady c.w.u.", "gjenværende blokkeringstid bereder", "", "kalan sıcak kullanım suyu blok süresi", "tempo di blocco rimanente ACS") // TODO translate
MAKE_TRANSLATION(flueGasTemp, "fluegastemp", "flue gas temperature", "Abgastemperatur", "Rookafvoertemperatuur", "", "temperatura spalin", "røykgasstemperatur", "", "baca gazı sıcaklığı", "temperatura gas di scarico") // TODO translate

MAKE_TRANSLATION(vr2Config, "vr2config", "vr2 configuration", "VR2 Konfiguration", "VR2 configuratie", "VR2 Konfiguration", "konfiguracja VR2", "vr2 konfigurasjon", "configuration vr2", "vr2 ayarı", "configurazione VR2")
MAKE_TRANSLATION(ahsActivated, "ahsactivated", "alternate heat source activation", "Alt. Wärmeerzeuger aktiviert", "Altenatieve warmtebron geactiveerd", "Alternativ värmekälla aktivering", "aktywacja alternatywnego źródła ciepła", "alternativ varmekilde aktivering", "activation source chaleur alternative", "alternatif ısı kaynağı devrede", "attivazione fonte di calore alternativa")
MAKE_TRANSLATION(aPumpConfig, "apumpconfig", "primary pump config", "Konfig. Hauptpumpe", "Primaire pomp configuratie", "Konfiguration Primärpump", "konfiguracja pompy głównej", "konfiguration primærpumpe", "configuration pompe primaire", "ana pompa ayarı", "configurazione pompa primaria")
MAKE_TRANSLATION(aPumpSignal, "apumpsignal", "output for pr1 pump", "Ausgang Pumpe PR1", "Output voor pomp PR1", "Utgång från pump PR1", "wyjście pompy PR1", "utgang fra pumpe PR1", "sortie pompe pr1", "p1 pompa çıkışı", "uscita per pompa PR1")
MAKE_TRANSLATION(aPumpMin, "apumpmin", "min output pump pr1", "Minimale Pumpenansteuerung", "Minimale output pomp PR1", "Min Output Pump PR1", "minimalne wysterowanie pompy PR1", "minimal output pumpe PR1", "sortie min pompe pr1", "p1 pompa minimum çıkış", "uscita minima pompa PR1")
MAKE_TRANSLATION(tempRise, "temprise", "ahs return temp rise", "Rücklauf Temperaturerhöhung", "Verhoging retourtemperatuur", "Förhöjd returtemperatur", "wzrost temperatury powrotu", "forhøyd returtemperatur", "augmentation température retour ahs", "alternatif ısı kaynağı dönüş sıcaklığı yükseldi", "aumento della temperatura di ritorno")
MAKE_TRANSLATION(setReturnTemp, "setreturntemp", "set temp return", "Soll-Rücklauftemperatur", "Streeftemperatuur retour", "Vald returtemperatur", "zadana temperatura powrotu", "valgt returtemperatur", "régler température retour", "hedef dönüş sıcaklığı", "imposta temperatura di ritorno")
MAKE_TRANSLATION(mixRuntime, "mixruntime", "mixer run time", "Mischer-Laufzeit", "Mixer looptijd", "Blandningsventil drifttid", "czas pracy miksera", "blandingsventil drifttid", "durée fonctionnement mélangeur", "karışım çalışma süresi", "tempo di funzionamento del miscelatore")
MAKE_TRANSLATION(bufBypass, "bufbypass", "buffer bypass config", "Puffer-Bypass Konfig.", "Buffer bypass configuratie", "Konfiguration Buffer bypass", "konfiguracja z obejściem bufora", "konfigurasjon buffer bypass", "configuration contournement buffer", "tampon baypas ayarı", "configurazione bypass del tampone ")
MAKE_TRANSLATION(bufMixRuntime, "bufmixruntime", "bypass mixer run time", "Speicher-Mischer-Laufzeit", "Buffer mixer looptijd", "Blandningsventil Bypass drifttid", "czas pracy mieszacza obejścia", "blandningsventil bypass drifttid", "durée fonctionnement contournement mélangeur", "baypas karıştırıcı çalışma süresi", "tempo funzionamento bypass miscelatore")
MAKE_TRANSLATION(bufConfig, "bufconfig", "dhw buffer config", "Konfig. Warmwasserspeicher", "Warmwater boiler configuratie", "Konfiguration Varmvattentank", "konfiguracja bufora c.w.u.", "konfigurasjon varmvannstank", "configuration buffer ecs", "sıcak su tampon ayarı", "configurazione tampone ACS")
MAKE_TRANSLATION(blockMode, "blockmode", "config htg. blocking mode", "Konfig. Sperr-Modus", "Configuratie blokeermodus", "Konfiguration Blockeringsläge", "konfiguracja trybu blokady", "konfigurasjon blokkeringsmodus", "config mode blocage htg.", "blok modu yapılandırması", "configurazione modalità di blocco")
MAKE_TRANSLATION(blockTerm, "blockterm", "config of block terminal", "Konfig. Sperrterminal", "Configuratie blookerterminal", "Konfiguration Blockeringsterminal", "konfiguracja terminala blokującego", "konfigurasjon blokkeringsterminal", "config. du bloque terminal", "blok terminal yapılandırması", "configurazione terminale di blocco")
MAKE_TRANSLATION(blockHyst, "blockhyst", "hyst. for boiler block", "Hysterese Sperrmodus", "Hysterese blokeerterminal", "Hysteres Blockeringsmodul", "tryb blokowania histerezy", "hystrese blokkeringsmodus", "hyst. Blocage chaudière", "kazan blok geçikmesi", "modalità blocco isteresi")
MAKE_TRANSLATION(releaseWait, "releasewait", "boiler release wait time", "Wartezeit Kessel-Freigabe", "Wachttijd ketel vrijgave", "Väntetid Frisläppning", "czas oczekiwania na zwolnienie kotła", "kjele frigjøringsventetid", "temps attente libération chaudière", "kazan tahliyesi bekleme süresi", "tempo di attesa sblocco caldaia")

// energy
MAKE_TRANSLATION(nrgHeat, "nrgheat", "energy heating", "Energie Heizen", "", "", "", "", "",  "ısıtma enerjisi", "") // TODO translate
MAKE_TRANSLATION(nrgWw, "nrgww", "energy dhw", "Energie Warmwasser", "", "", "", "", "",  "sıcak kullanım suyu enerjisi", "") // TODO translate
MAKE_TRANSLATION(nomPower, "nompower", "nominal Power", "Brennerleistung", "", "", "", "", "",  "nominal güç", "") // TODO translate

// HIU
MAKE_TRANSLATION(netFlowTemp, "netflowtemp", "heat network flow temp", "System Vorlauftemperatur", "Netto aanvoertemperatuur", "", "", "", "", "ısıtma şebekesi akış derecesi", "temperatura di mandata della rete di riscaldamento") // TODO translate
MAKE_TRANSLATION(cwFlowRate, "cwflowrate", "cold water flow rate", "Kaltwasser Durchfluss", "Stroomsnelheid koud water ", "", "", "", "",  "soğuk su akış hızı", "portata acqua fredda") // TODO translate
MAKE_TRANSLATION(keepWarmTemp, "keepwarmtemp", "keep warm temperature","Warmhaltetemperatur", "Warmhoudtemperatuur", "", "", "", "",  "sıcaklığı koruma derecesi", "mantenere la temperatura calda") // TODO translate

// the following are dhw for the boiler and automatically tagged with 'dhw'
MAKE_TRANSLATION(wwSelTemp, "wwseltemp", "selected temperature", "gewählte Temperatur", "Geselecteerd temperatuur", "Vald Temperatur", "temperatura wyższa/komfort", "valgt temperatur", "température sélectionnée", "seçili sıcaklık", "temperatura selezionata")
MAKE_TRANSLATION(wwSelTempLow, "wwseltemplow", "selected lower temperature", "untere Solltemperatur", "Onderste streeftemperatuur", "Vald lägstatemperatur", "temperatura niższa/eko", "valgt nedre temperatur", "température basse sélectionnée", "seçili düşük sıcaklık", "bassa temperatura selezionata")
MAKE_TRANSLATION(wwSelTempOff, "wwseltempoff", "selected temperature for off", "Solltemperatur bei AUS", "Streeftemperatuur bij UIT", "Vald tempereatur för AV", "temperatura gdy grzanie wyłączone", "valgt tempereatur for av", "température sélectionnée pour arrêt", "kapanma için seçili sıcaklık", "temperatura selezionata per spegnimento")
MAKE_TRANSLATION(wwSelTempSingle, "wwseltempsingle", "single charge temperature", "Solltemperatur Einmalladung", "Streeftemperatuur enkele lading", "Temperatur Engångsladdning", "temperatura dodatkowej ciepłej wody", "temp engangsoppvarming", "température charge unique", "tek şarj sıcaklığı", "temperatura singolaa carica")
MAKE_TRANSLATION(wwCylMiddleTemp, "wwcylmiddletemp", "cylinder middle temperature (TS3)", "Speichertemperatur Mitte", "Buffer temperatuur midden", "Cylinder Temperatur Mitten (TS3)", "temperatura środka cylindra (TS3)", "vanntank midten temperatur (TS3)", "température moyenne ballon (TS3)", "Silindir orta sıcaklığı", "temperatura centrale accumulo (TS3)")
MAKE_TRANSLATION(wwSetTemp, "wwsettemp", "set temperature", "Solltemperatur", "Streeftemperatuut", "Börtempertur", "temperatura zadana", "innstilt temperatur", "régler température", "hedef sıcaklık", "imposta temperatura")
MAKE_TRANSLATION(wwType, "wwtype", "type", "Typ", "type", "Typ", "typ", "type", "type", "tip", "tipo")
MAKE_TRANSLATION(wwComfort, "wwcomfort", "comfort", "Komfort", "Comfort", "Komfort", "komfort", "komfort", "confort", "konfor", "Comfort")
MAKE_TRANSLATION(wwComfort1, "wwcomfort1", "comfort mode", "Komfort-Modus", "Comfort modus", "Komfortläge", "tryb komfortu", "komfort modus", "mode confort", "konfor modu", "modalità comfort")
MAKE_TRANSLATION(wwFlowTempOffset, "wwflowtempoffset", "flow temperature offset", "Vorlauftemperaturanhebung", "Aanvoertemperatuur offset", "Flödestemperatur förskjutning", "korekta temperatury wypływu", "turtemperaturforskyvning", "offset température flux", "akış sıcaklığı artışı", "aumento della temperatura di ritorno")
MAKE_TRANSLATION(wwMaxPower, "wwmaxpower", "max power", "max Leistung", "Maximaal vermogen", "Max Effekt", "moc maksymalna", "maks effekt", "puissance max", "maksimum güç", "potenza massima")
MAKE_TRANSLATION(wwCircPump, "wwcircpump", "circulation pump available", "Zirkulationspumpe vorhanden", "Circulatiepomp aanwezig", "Cirkulationspump tillgänglig", "pompa cyrkulacji zainstalowana", "sirkulasjonspumpe tilgjengelig", "pompe circulation disponible", "sikülasyon pompası müsait", "pompa circolazione disponibile")
MAKE_TRANSLATION(wwChargeType, "wwchargetype", "charging type", "Speicher-Ladungstyp", "Buffer laadtype", "Laddningstyp", "sposób grzania zasobnika", "varmetype", "type chargement", "şarj tipi", "tipo caricamento")
MAKE_TRANSLATION(wwDisinfectionTemp, "wwdisinfectiontemp", "disinfection temperature", "Desinfektionstemperatur", "Desinfectietemperatuur", "Desinfektionstemperatur", "temperatura dezynfekcji termicznej", "desinfeksjonstemperatur", "température désinfection", "dezenfeksiyon sıcaklığı", "temperatura disinfezione")
MAKE_TRANSLATION(wwCircMode, "wwcircmode", "circulation pump mode", "Zirkulationspumpen-Modus", "Modus circulatiepomp", "Läge Cirkulationspump", "tryb pracy cyrkulacji", "sikulasjonspumpemodus", "mode pompe circulation", "sirkülasyon pompa modu", "modalità pompa circolazione")
MAKE_TRANSLATION(wwCirc, "wwcirc", "circulation active", "Zirkulation aktiv", "Circulatiepomp actief", "Cirkulation aktiv", "pompa cyrkulacji", "sirkulasjon aktiv", "circulation active", "sirkülasyon devrede", "circolazione attiva")
MAKE_TRANSLATION(wwCurTemp, "wwcurtemp", "current intern temperature", "aktuelle interne Temperatur", "Huidige interne temperatuur", "Intern Temperatur", "temperatura zasobnika", "gjeldende intern temperatur", "température interne actuelle", "güncel iç sıcaklık", "temperatura interna attuale")
MAKE_TRANSLATION(wwCurTemp2, "wwcurtemp2", "current extern temperature", "aktuelle externe Temperatur", "Huidige externe temperatuur", "Extern Temperatur", "temperatura wypływu", "gjeldende ekstern temperaur", "température externe actuelle", "güncel dış sıcaklık", "temperatura esterna attuale")
MAKE_TRANSLATION(wwCurFlow, "wwcurflow", "current tap water flow", "aktueller Durchfluss", "Hudige warmwater doorstroming", "Aktuellt tappvattenflöde", "aktualny przepływ", "gjeldende tappevannshastighet", "débit actuel eau robinet", "güncel musluk suyu akışı", "portata corrente dell'acqua del rubinetto")
MAKE_TRANSLATION(wwStorageTemp1, "wwstoragetemp1", "storage intern temperature", "interne Speichertemperatur", "Interne buffertemperatuur", "Beredare Intern Temperatur", "temperatura wewnątrz zasobnika", "intern temperatur bereder", "température interne stockage", "depo iç sıcaklığı", "temperatura di conservazione interna")
MAKE_TRANSLATION(wwStorageTemp2, "wwstoragetemp2", "storage extern temperature", "externer Speichertemperatur", "Externe buffertemperatuur", "Beredare Extern Tempereatur", "temperatura na wyjściu zasobnika", "ekstern temperatur bereder", "température externe stockage", "depo dış sıcaklığı", "temperatura di conservazione esterna")
MAKE_TRANSLATION(wwActivated, "wwactivated", "activated", "aktiviert", "geactiveerd", "Aktiverad", "system przygotowywania c.w.u.", "aktivert", "activé", "devreye girdi", "attivato")
MAKE_TRANSLATION(wwOneTime, "wwonetime", "one time charging", "Einmalladung", "Buffer eenmalig laden", "Engångsladdning", "jednorazowa dodatkowa ciepła woda", "engangsoppvarming", "charge unique", "tek seferlik doldurma", "carica singola")
MAKE_TRANSLATION(wwDisinfecting, "wwdisinfecting", "disinfecting", "Desinfizieren", "Desinfectie", "Desinficerar", "dezynfekcja termiczna", "desinfiserer", "désinfection", "dezenfekte ediliyor", "disinfezione")
MAKE_TRANSLATION(wwCharging, "wwcharging", "charging", "Laden", "Laden", "Värmer", "grzanie", "varmer", "chargement", "dolduruluyor", "caricamento")
MAKE_TRANSLATION(wwChargeOptimization, "wwchargeoptimization", "charge optimization", "Ladungsoptimierung", "laadoptimalisatie", "Laddningsoptimering", "optymalizacja grzania", "oppvarmingsoptimalisering", "optimisation charge", "dolum optimizasyonu", "ottimizzazione carica")
MAKE_TRANSLATION(wwRecharging, "wwrecharging", "recharging", "Nachladen", "herladen", "Laddar om", "ponowne grzanie", "varm på nytt", "en recharge", "tekrar dolduruluyor", "in ricarica")
MAKE_TRANSLATION(wwTempOK, "wwtempok", "temperature ok", "Temperatur ok", "Temperatuur OK", "Temperatur OK", "temperatura OK", "temperatur ok!", "température ok", "sıcaklık tamam", "Temperatura OK")
MAKE_TRANSLATION(wwActive, "wwactive", "active", "aktiv", "Actief", "Aktiv", "aktywna", "aktiv", "actif", "devrede", "attivo")
MAKE_TRANSLATION(ww3wayValve, "ww3wayvalve", "3-way valve active", "3-Wegeventil aktiv", "3-wegklep actief", "Trevägsventil aktiv", "zawór 3-drogowy aktywny", "aktiv trevisventil", "vanne 3 voies active", "3 yollu vana", "valvola 3-vie")
MAKE_TRANSLATION(wwSetPumpPower, "wwsetpumppower", "set pump power", "Soll Pumpenleistung", "Streefwaarde pompvermogen", "Vald pumpeffekt", "ustawione wysterowanie pompy", "valgt pumpeeffekt", "régler puissance pompe", "ayarlı pompa gücü", "imposta potenza pompa")
MAKE_TRANSLATION(wwMixerTemp, "wwmixertemp", "mixer temperature", "Mischertemperatur", "Mixertemperatuur", "Blandningsventil-tempertur", "temperatura mieszacza", "temperatur blandeventil", "température mélangeur", "karıştırıcı sıcaklığı", "temperatura miscelatore")
MAKE_TRANSLATION(wwStarts, "wwstarts", "starts", "Anzahl Starts", "Aantal starts", "Antal starter", "liczba załączeń", "antall starter", "démarrages", "başlıyor", "avvii")
MAKE_TRANSLATION(wwStarts2, "wwstarts2", "control starts2", "Kreis 2 Anzahl Starts", "Aantal starts circuit 2", "Antal starter Krets 2", "liczba załączeń 2", "antall starter krets 2", "démarrages contrôle 2", "devre 2 başlıyor", "avvii controllati 2")
MAKE_TRANSLATION(wwWorkM, "wwworkm", "active time", "aktive Zeit", "Actieve tijd", "Aktiv Tid", "czas aktywności", "driftstid", "temps actif", "aktif zaman", "tempo attivo")
MAKE_TRANSLATION(wwHystOn, "wwhyston", "hysteresis on temperature", "Einschalttemperaturdifferenz", "Inschakeltemperatuurverschil", "Hysteres PÅ-temperatur", "histereza załączania", "innkoblingstemperaturforskjell", "hystérésis température allumage", "çalışma sıcaklığı farkı", "differenza di temperatura di accensione")
MAKE_TRANSLATION(wwHystOff, "wwhystoff", "hysteresis off temperature", "Ausschalttemperaturdifferenz", "Uitschakeltemperatuurverschil", "Hysteres AV-temperatur", "histereza wyłączania", "utkoblingstemperaturforskjell", "hystérésis température extinction", "kapatma sıcaklığı farkı", "differenza di temperatura di spegnimento")
MAKE_TRANSLATION(wwProgMode, "wwprogmode", "program", "Programmmodus", "Programma", "Program", "program", "program", "programme", "program", "Programma")
MAKE_TRANSLATION(wwCircProg, "wwcircprog", "circulation program", "Zirkulationsprogramm", "Circulatieprogramma", "Cirkulationsprogram", "program cyrkulacji c.w.u.", "sirkulationsprogram", "programme circulation", "sirkülasyon programı", "programma circolazione")
MAKE_TRANSLATION(wwMaxTemp, "wwmaxtemp", "maximum temperature", "Maximale Temperatur", "Maximale temperatuur", "Maximal Temperatur", "temperatura maksymalna", "maksimal temperatur", "température max", "maksimum sıcaklık", "temperatura massima")
MAKE_TRANSLATION(wwOneTimeKey, "wwonetimekey", "one time key function", "Einmalladungstaste", "Knop voor eenmalig laden buffer", "Engångsfunktion", "przycisk jednorazowego ogrzania", "engangsknapp varme", "fonction touche unique", "tek seferlik doldurma fonksiyonu", "pulsante funzione singola")
MAKE_TRANSLATION(wwSolarTemp, "wwsolartemp", "solar boiler temperature", "Solarboiler Temperatur", "Zonneboiler temperatuur", "Solpanel Temp", "temperatura zasobnika solarnego", "solpaneltemp", "température chaudière solaire", "güneş enerjisi kazan sıcaklığı", "temperatura pannello solare")

// mqtt values / commands
MAKE_TRANSLATION(switchtime, "switchtime", "program switchtime", "Programm Schaltzeit", "Programma schakeltijd", "Program Bytestid", "program czasowy", "programbyttetid", "heure commutation programme", "program değiştirme süresi", "ora commutazione programmata")
MAKE_TRANSLATION(switchtime1, "switchtime1", "own1 program switchtime", "Programm 1 Schaltzeit", "Schakeltijd programma 1", "Program 1 Bytestid", "program przełączania 1", "byttetidprogram 1", "heure de commutation programme 1", "program1 değiştirme süresi", "ora commutazione programma 1")
MAKE_TRANSLATION(switchtime2, "switchtime2", "own2 program switchtime", "Programm 2 Schaltzeit", "Schakeltijd programma 2", "Program 2 Bytestid", "program przełączania 2", "byttetid program 2", "heure de changement programme 2", "program1 değiştirme süresi", "ora commutazione programma 2")
MAKE_TRANSLATION(wwswitchtime, "wwswitchtime", "program switchtime", "Programm Schaltzeit", "Warm water programma schakeltijd", "Varmvattenprogram Bytestid", "program czasowy", "byttetid varmtvannsprogram", "heure commutation programme", "sıcak kullanıom suyu program değiştirme süresi", "Tempo di commutazione del programma")
MAKE_TRANSLATION(wwcircswitchtime, "wwcircswitchtime", "circulation program switchtime", "Zirculationsprogramm Schaltzeit", "Schakeltijd circulatieprogramma", "Cirkulationsprogram Bytestid", "program cyrkulacji", "byttetid sirkulasjonsprogram", "heure commutation programme circulation", "sirkülasyon program değiştirme süresi", "ora commutazione programma circolazione")
MAKE_TRANSLATION(dateTime, "datetime", "date/time", "Datum/Zeit", "Datum/Tijd", "Datum/Tid", "data i godzina", "dato/tid", "date/heure", "zaman/saat", "Data/Ora")
MAKE_TRANSLATION(errorCode, "errorcode", "error code", "Fehlernummer", "Foutmeldingscode", "Felkod", "kod błędu", "feikode", "code erreur", "hata kodu", "codice errore")
MAKE_TRANSLATION(ibaMainDisplay, "display", "display", "Anzeige", "Display", "Display", "wyświetlacz", "skjerm", "affichage", "ekran", "Display")
MAKE_TRANSLATION(ibaLanguage, "language", "language", "Sprache", "Taal", "Sprak", "język", "språk", "langue", "dil", "Lingua")
MAKE_TRANSLATION(ibaClockOffset, "clockoffset", "clock offset", "Uhrkorrektur", "Klokcorrectie", "Tidskorrigering", "korekta zegara", "tidskorrigering", "offset horloge", "saat farkı", "correzione orario")
MAKE_TRANSLATION(ibaBuildingType, "building", "building type", "Gebäudetyp", "Type gebouw", "Byggnadstyp", "typ budynku", "bygningstype", "type bâtiment", "bina tipi", "tipo di edificio")
MAKE_TRANSLATION(heatingPID, "heatingpid", "heating PID", "Heizungs-PID", "PID verwarming", "Uppvärmning PID", "PID ogrzewania", "oppvarmings PID", "PID chauffage", "PID ısınıyor", "PID-riscaldamento")
MAKE_TRANSLATION(ibaCalIntTemperature, "intoffset", "internal temperature offset", "Korrektur interner Temperatur", "Offset interne temperatuur", "Korrigering interntemperatur", "korekta temperatury w pomieszczeniu", "Korrigering interntemperatur", "offset température interne", "iç sıcaklık artışı", "scostamento della temperatura interna")
MAKE_TRANSLATION(ibaMinExtTemperature, "minexttemp", "minimal external temperature", "min. Aussentemperatur", "Min. buitentemperatuur", "Min Extern Temperatur", "minimalna miejscowa temperatura zewnętrzna", "minimal eksterntemperatur", "température extérieure minimale", "en düşük sış sıcaklık", "temperatura esterna minima")
MAKE_TRANSLATION(backlight, "backlight", "key backlight", "Gegenlicht", "Toetsverlichting", "Bakgrundsbelysning", "podświetlenie klawiatury", "bakgrunnsbelysning", "rétroéclairage touches", "tuş takımı aydınlatması", "retroilluminazione dei tasti")
MAKE_TRANSLATION(damping, "damping", "damping outdoor temperature", "Dämpfung der Außentemperatur", "Demping buitentemperatuur", "Utomhustemperatur dämpning", "tłumienie temperatury zewnętrznej", "demping av utetemperatur", "température extérieure minimale", "dış sıcaklığın sönümlenmesi", "smorzamento della temperatura esterna")
MAKE_TRANSLATION(tempsensor1, "inttemp1", "temperature sensor 1", "Temperatursensor 1", "Temperatuursensor 1", "Temperatursensor 1", "czujnik temperatury 1", "temperatursensor 1", "sonde température 1", "sıcaklık sensörü 1", "sensore temperatura 1")
MAKE_TRANSLATION(tempsensor2, "inttemp2", "temperature sensor 2", "Temperatursensor 2", "Temperatuursensor 2", "Temperatursensor 2", "czujnik temperatury 2", "temperatursensor 2", "capteur température 2", "sıcaklık sensörü 2", "sensore temperatura 2")
MAKE_TRANSLATION(dampedoutdoortemp, "dampedoutdoortemp", "damped outdoor temperature", "gedämpfte Außentemperatur", "Gedempte buitentemperatuur", "Utomhustemperatur dämpad", "tłumiona temperatura zewnętrzna", "dempet utetemperatur", "température extérieure amortie", "sönümlenmiş dış sıcaklık", "temperatura esterna smorzata")
MAKE_TRANSLATION(floordrystatus, "floordry", "floor drying", "Estrichtrocknung", "Vloerdroogprogramma", "Golvtorkning", "suszenie jastrychu", "gulvtørkeprogram", "séchage sol", "yerden ısıtma", "asciugatura pavimento")
MAKE_TRANSLATION(floordrytemp, "floordrytemp", "floor drying temperature", "Estrichtrocknungs Temperatur", "Temperatuur vloerdroogprogramma", "Golvtorkning Temperatur", "temperatura suszenia jastrychu", "gulvtørketemperatur", "température séchage sol", "yerden ısıtma sıcaklığı", "Temperatura asciugatura pavimento")
MAKE_TRANSLATION(brightness, "brightness", "screen brightness", "Bildschirmhelligkeit", "Schermhelderheid", "Ljusstyrka", "jasność", "lysstyrke", "luminosité écran", "ekran parlaklığı", "luminosita display")
MAKE_TRANSLATION(autodst, "autodst", "automatic change daylight saving time", "automatische Sommerzeit Umstellung", "Automatische omschakeling zomer-wintertijd", "Automatisk växling sommar/vinter-tid", "automatycznie przełączaj na czas letni/zimowy", "automatisk skifte av sommer/vinter-tid", "changement automatique heure d'été", "gün ışığından yararlanma saatini otomatik olarak değiştir", "cambio automatico dell'ora legale")
MAKE_TRANSLATION(preheating, "preheating", "preheating in the clock program", "Vorheizen im Zeitprogramm", "Voorverwarming in het klokprogramma", "Förvärmning i tidsprogram", "podgrzewanie w programie czasowym", "forvarming i tidsprogram", "préchauffage dans programme horloge", "saat programında ön ısıtma", "preriscaldamento nel programma orologio")
MAKE_TRANSLATION(offtemp, "offtemp", "temperature when mode is off", "Temperatur bei AUS", "Temperatuur bij UIT", "Temperatur Avslagen", "temperatura w trybie \"wył.\"", "temperatur avslått", "température lorsque mode désactivé", "mod kapalı iken sıcaklık", "temperatura quando la modalità è disattivata")
MAKE_TRANSLATION(mixingvalves, "mixingvalves", "mixing valves", "Mischventile", "Mengkleppen", "Blandningsventiler", "zawory mieszające", "blandeventiler", "vannes mélange", "karışım vanaları", "valvole miscela")
MAKE_TRANSLATION(pvEnableWw, "pvenableww", "enable raise dhw", "aktiviere Anhebung WW", "Verhoging WW activeren", "", "podwyższenie c.w.u. z PV", "aktivere hevet temperatur bereder", "", "sıcak kullanım suyu yükseltmeyi etkinleştir", "abilitare aumento ACS") // TODO translate
MAKE_TRANSLATION(pvRaiseHeat, "pvraiseheat", "raise heating with PV", "Anhebung Heizen mit PV", "Verwarmen met PV activeren", "", "podwyższenie grzania z PV", "heve varmen med solpanel", "", "ısıtmayı G.E. İle yükselt", "Aumentare il riscaldamento con il solare") // TODO translate
MAKE_TRANSLATION(pvLowerCool, "pvlowercool", "lower cooling with PV", "Kühlabsenkung mit PV", "Verlagen koeling met PV activeren", "", "obniżenie chłodzenia z PV", "nedre kjøling solpanel", "", "soğutmayı G.E. İle düşür", "Riduzione del raffreddamento con il solare") // TODO translate
// thermostat ww
MAKE_TRANSLATION(wwMode, "wwmode", "mode", "Modus", "Modus", "Läge", "tryb pracy", "modus", "mode", "mod", "modalità")
MAKE_TRANSLATION(wwSetTempLow, "wwsettemplow", "set low temperature", "untere Solltemperatur", "Onderste streeftemperatuur", "Nedre Börvärde", "zadana temperatura obniżona", "nedre settverdi", "réglage température basse", "hedef düşük sıcaklık", "imposta bassa temperatura")
MAKE_TRANSLATION(wwWhenModeOff, "wwwhenmodeoff", "when thermostat mode off", "bei Thermostatmodus AUS", "Als Thermostaat op UIT", "när Termostatläge är AV", "gdy wyłączono na termostacie", "når modus er av", "lorsque mode thermostat off", "termostat modu kapalı olduğunda", "quando termostato modalita OFF")
MAKE_TRANSLATION(wwExtra1, "wwextra1", "circuit 1 extra", "Kreis 1 Extra", "Circuit 1 extra", "Krets 1 Extra", "obieg dodatkowy 1", "ekstra krets 1", "circuit 1 extra", "devre 1 ekstra", "Circuito 1 extra")
MAKE_TRANSLATION(wwExtra2, "wwextra2", "circuit 2 extra", "Kreis 2 Extra", "Circuit 2 extra", "Kets 2 Extra", "obieg dodatkowy 2", "ekstra krets 2", "circuit 2 extra", "devre 2 ekstra", "Circuito 2 extra")
MAKE_TRANSLATION(wwCharge, "wwcharge", "charge", "Laden", "Laden", "Ladda", "grzanie", "lade", "charge", "doldurma", "carica")
MAKE_TRANSLATION(wwChargeDuration, "wwchargeduration", "charge duration", "Ladedauer", "Laadtijd", "Laddtid", "czas grzania dodatkowej ciepłej wody", "ladetid", "durée charge", "doldurma süresi", "durata carica")
MAKE_TRANSLATION(wwDisinfect, "wwdisinfect", "disinfection", "Desinfektion", "Desinfectie", "Desinfektion", "dezynfekcja termiczna", "desinfeksjon", "désinfection", "dezenfeksiyon", "disinfezione")
MAKE_TRANSLATION(wwDisinfectDay, "wwdisinfectday", "disinfection day", "Desinfektionstag", "Desinfectiedag", "Desinfektionsdag", "dzień dezynfekcji termicznej", "desinfeksjonsdag", "jour désinfection", "dezenfeksiyon günü", "giorno disinfezione")
MAKE_TRANSLATION(wwDisinfectHour, "wwdisinfecthour", "disinfection hour", "Desinfektionsstunde", "Desinfectieuur", "Desinfektionstimme", "godzina dezynfekcji termicznej", "desinfeksjonstime", "heure désinfection", "dezenfeksiyon saati", "ora disinfezione")
MAKE_TRANSLATION(wwDisinfectTime, "wwdisinfecttime", "disinfection time", "Desinfektionsdauer", "Desinfectietijd", "Desinfektionstid", "maksymalny czas trwania dezynfekcji termicznej", "desinfeksjonstid", "durée désinfection", "dezenfeksiyon zamanı", "orario disinfezione")
MAKE_TRANSLATION(wwDailyHeating, "wwdailyheating", "daily heating", "täglich Heizen", "Dagelijks opwarmen", "Daglig Uppvärmning", "codzienne nagrzewanie", "daglig oppvarming", "chauffage quotidien", "günlük ısıtma", "riscaldamento giornaliero")
MAKE_TRANSLATION(wwDailyHeatTime, "wwdailyheattime", "daily heating time", "tägliche Heizzeit", "Tijd dagelijkse opwarming", "Daglig Uppvärmningstid", "czas trwania codziennego nagrzewania", "daglig oppvarmingstid", "heure chauffage quotidien", "günlük ısıtma süresi", "orario riscaldamento giornaliero")

// thermostat hc
MAKE_TRANSLATION(selRoomTemp, "seltemp", "selected room temperature", "Sollwert Raumtemperatur", "Streeftemperatuur kamer", "Vald Rumstemperatur", "zadana temperatura w pomieszczeniu", "valgt rumstemperatur", "température ambiante sélectionnée", "seçili oda sıcaklığı", "temperatura ambiente selezionata")
MAKE_TRANSLATION(roomTemp, "currtemp", "current room temperature", "aktuelle Raumtemperatur", "Huidige kamertemperatuur", "Aktuell Rumstemperatur", "temperatura w pomieszczeniu", "gjeldende romstemperatur", "température ambiante actuelle", "güncel oda sıcaklığı", "temperatura ambiente attuale")
MAKE_TRANSLATION(mode, "mode", "mode", "Modus", "Modus", "Läge", "sposób sterowania", "modus", "mode", "mod", "modalità")
MAKE_TRANSLATION(modetype, "modetype", "mode type", "Modus Typ", "Type modus", "Typ av läge", "aktualny tryb pracy", "modusrype", "type mode", "mod tipi", "tipo di modalita")
MAKE_TRANSLATION(fastheatup, "fastheatup", "fast heatup", "schnelles Aufheizen", "Snel opwarmen", "Snabb Uppvärmning", "szybkie nagrzewanie", "rask oppvarming", "chauffage rapide", "hızlı ısıtma", "riscaldamento rapido")
MAKE_TRANSLATION(daytemp, "daytemp", "day temperature", "Tagestemperatur", "temperatuur dag", "Dagstemperatur", "temperatura w dzień", "dagtemperatur", "température jour", "gündüz sıcaklığı", "temperatura giornaliera")
MAKE_TRANSLATION(daylowtemp, "daytemp2", "day temperature T2", "Tagestemperatur T2", "Temperatuur dag T2", "Dagstemperatur T2", "temperatura w dzień T2", "dagtemperatur T2", "température jour T2", "gündüz sıcaklığı T2", "temperatura giornaliera T2")
MAKE_TRANSLATION(daymidtemp, "daytemp3", "day temperature T3", "Tagestemperatur T3", "Temperatuur dag T3", "Dagstemperatur T3", "temperatura w dzień T3", "dagtemperatur T3", "température jour T3", "gündüz sıcaklığı T3", "temperatura giornaliera T3")
MAKE_TRANSLATION(dayhightemp, "daytemp4", "day temperature T4", "Tagestemperatur T4", "Temperatuur dag T4", "Dagstemperatur T4", "temperatura w dzień T4", "dagtemperatur T4", "température jour T4", "gündüz sıcaklığı T4", "temperatura giornaliera T4")
MAKE_TRANSLATION(heattemp, "heattemp", "heat temperature", "Heizen Temperatur", "Temperatuur verwarming", "Temperatur Uppvärmning", "temperatura ogrzewania", "oppvarmingstemperatur", "température chauffage", "ısıtma sıcaklığı", "temperatura riscaldamento")
MAKE_TRANSLATION(nighttemp, "nighttemp", "night temperature", "Nachttemperatur", "Nachttemperatuur", "Nattemperatur", "temperatura w nocy", "nattemperatur", "température de nuit", "gece sıcaklığı", "temperatura notturna")
MAKE_TRANSLATION(nighttemp2, "nighttemp", "night temperature T1", "Nachttemperatur T1", "Nachttemperatuur T1", "Nattemperatur T1", "temperatura w nocy T1", "nattemperatur T1", "température nuit T1", "gece sıcaklığı T1", "temperatura notturna T1")
MAKE_TRANSLATION(ecotemp, "ecotemp", "eco temperature", "eco Temperatur", "Temperatuur eco", "Eko-temperatur", "temperatura w trybie eko", "øko temperatur", "température éco", "eko sıcaklık", "Temperatura eco")
MAKE_TRANSLATION(manualtemp, "manualtemp", "manual temperature", "manuelle Temperatur", "Temperatuur handmatig", "Temperatur Manuell", "temperatura ustawiona ręcznie", "manuell temperatur", "température manuelle", "manuel sıcaklık", "temperatura manuale")
MAKE_TRANSLATION(tempautotemp, "tempautotemp", "temporary set temperature automode", "temporäre Solltemperatur", "Streeftemperatuur automodus tijdelijk", "Temporär Aktivering av Auto-läge", "zadana temperatura w pomieszczenia w trybie \"auto\" (tymczasowa)", "temporær valgt temp i automodus", "température temporaire mode automatique", "geçici ayarlı sıcaklık otomatik mod", "impostare temporaneamente temperatura automatica")
MAKE_TRANSLATION(remoteseltemp, "remoteseltemp", "temporary set temperature from remote", "temporäre Solltemperatur Remote", "Temperatuur van afstandsbedieding", "Temperatur från fjärruppkoppling", "zadana zdalnie temperatura a pomieszczeniu (tymczasowa)", "temporær valgt temp fra fjernbetjening", "température temporaire depuis télécommande", "geçici ayarlı sıcaklık uzaktan", "Temperatura temporanea da remoto")
MAKE_TRANSLATION(comforttemp, "comforttemp", "comfort temperature", "Komforttemperatur", "Comforttemperatuur", "Komforttemperatur", "temperatura w trybie komfort", "komforttemperatur", "température confort", "konfor sıcaklığı", "temperatura comfort")
MAKE_TRANSLATION(summertemp, "summertemp", "summer temperature", "Sommertemperatur", "Zomertemperatuur", "Sommartemperatur", "temperatura przełączania lato/zima", "Sommertemperatur", "température été", "yaz sıcaklığı", "temperatura estiva")
MAKE_TRANSLATION(designtemp, "designtemp", "design temperature", "Auslegungstemperatur", "Ontwerptemperatuur", "Design-temperatur", "temperatura projektowa", "designtemperatur", "température conception", "özel sıcaklık", "temperatura predefinita")
MAKE_TRANSLATION(offsettemp, "offsettemp", "offset temperature", "Temperaturanhebung", "Temperatuur offset", "Temperaturkorrigering", "korekta temperatury", "temperaturkorrigering", "température offset", "artış sıcaklığı", "aumento della temperatura")
MAKE_TRANSLATION(minflowtemp, "minflowtemp", "min flow temperature", "min Vorlauftemperatur", "Minimale aanvoertemperatuur", "Min Flödestemperatur", "minimalna temperatura zasilania", "min turtemperatur", "température min flux", "minimun akış sıcaklığı", "temperatura minima di mandata")
MAKE_TRANSLATION(maxflowtemp, "maxflowtemp", "max flow temperature", "max Vorlauftemperatur", "Maximale aanvoertemperatuur", "Max Flödestemperatur", "maksymalna temperatura zasilania", "maks turtemperatur", "température max flux", "maksimum akış sıcaklığı", "temperatura massima di mandata")
MAKE_TRANSLATION(roominfluence, "roominfluence", "room influence", "Raumeinfluss", "Ruimteinvloed", "Rumspåverkan", "wpływ pomieszczenia", "rominnflytelse", "influence pièce", "oda etkisi", "influenza della camera")
MAKE_TRANSLATION(roominfl_factor, "roominflfactor", "room influence factor", "Raumeinflussfaktor", "Factor ruimteinvloed", "Rumspåverkansfaktor", "współczynnik wpływu pomieszczenia", "rominnflytelsesfaktor", "facteur d'influence pièce", "oda etkisi faktörü", "fattore influenza camera")
MAKE_TRANSLATION(curroominfl, "curroominfl", "current room influence", "aktueller Raumeinfluss", "Huidige ruimteinvloed", "Aktuell Rumspåverkan", "aktualny wpływ pomieszczenia", "gjeldende rominnflytelse", "influence actuelle pièce", "güncel oda etkisi", "fattore corrente influenza camera")
MAKE_TRANSLATION(nofrosttemp, "nofrosttemp", "nofrost temperature", "Frostschutztemperatur", "Temperatuur vorstbeveiliging", "Temperatur Frostskydd", "temperatura ochrony przed zamarzaniem", "frostbeskyttelsestemperatur", "température protection gel", "donma koruması sıcaklığı", "temperatura protezione antigelo")
MAKE_TRANSLATION(targetflowtemp, "targetflowtemp", "target flow temperature", "berechnete Vorlauftemperatur", "Berekende aanvoertemperatuur", "Målvärde Flödestemperatur", "zadana temperatura zasilania", "målverdi turtemperatur", "température cible flux", "hedef akış sıcaklığı", "temperatura di mandata calcolata")
MAKE_TRANSLATION(heatingtype, "heatingtype", "heating type", "Heizungstyp", "Verwarmingstype", "Värmesystem", "system grzewczy", "varmesystem", "type chauffage", "ısıtma tipi", "tipo riscaldamento")
MAKE_TRANSLATION(summersetmode, "summersetmode", "set summer mode", "Einstellung Sommerbetrieb", "Instelling zomerbedrijf", "Aktivera Sommarläge", "tryb lato/zima", "aktiver sommermodus", "activer mode été", "yaz modu ayarı", "Impostazione della modalità estiva")
MAKE_TRANSLATION(hpoperatingmode, "hpoperatingmode", "heatpump operating mode", "Wärmepumpe Betriebsmodus", "Bedrijfsmodus warmtepomp", "Värmepump Driftläge", "tryb pracy pompy ciepła", "driftsmodus varmepumpe", "mode fonctionnement pompe à chaleur", "ısı pompası çalışma modu", "Modalità di funzionamento della pompa di calore")
MAKE_TRANSLATION(hpoperatingstate, "hpoperatingstate", "heatpump operating state", "WP Arbeitsweise", "Huidige modus warmtepomp", "Värmepump driftläge", "aktualny tryb pracy pompy ciepła", "driftstatus varmepumpe", "état fonctionnement pompe à chaleur", "ısı pompası çalışma durumu", "stato funzionamento pompa di calore")
MAKE_TRANSLATION(controlmode, "controlmode", "control mode", "Kontrollmodus", "Comtrolemodus", "Kontrolläge", "tryb sterowania", "kontrollmodus", "mode régulation", "kontrol modu", "modalità di controllo")
MAKE_TRANSLATION(control, "control", "control device", "Fernsteuerung", "Afstandsbedieding", "Kontrollenhet", "sterownik", "kontrollenhet", "dispositif régulation", "kontrol cihazı", "dispositivo di comando")
MAKE_TRANSLATION(roomsensor, "roomsensor", "room sensor", "Raumsensor", "Ruimtesensor", "Rumssensor", "czujnik temperatury pomieszczenia", "romsensor", "capteur pièce", "oda sensörü", "sensore ambiente")
MAKE_TRANSLATION(program, "program", "program", "Programm", "Programma", "Program", "program", "program", "programme", "program", "Programma")
MAKE_TRANSLATION(pause, "pause", "pause time", "Pausenzeit", "Pausetijd", "Paustid", "czas przerwy", "pausetid", "temps de pause", "süreyi durdur", "pausa")
MAKE_TRANSLATION(party, "party", "party time", "Partyzeit", "Partytijd", "Partytid", "czas przyjęcia", "partytid", "temps de fête", "parti zamanı", "festivo")
MAKE_TRANSLATION(holidaytemp, "holidaytemp", "holiday temperature", "Urlaubstemperatur", "Vakantietemperatuur", "Helgtemperatur", "temperatura w trybie urlopowym", "ferietemperatur", "température vacances", "tatil sıcaklığı", "temperatura festiva")
MAKE_TRANSLATION(summermode, "summermode", "summer mode", "Sommerbetrieb", "Zomerbedrijf", "Sommarläge", "aktualny tryb lato/zima", "sommermodus", "mode été", "yaz modu", "funzionamento estivo")
MAKE_TRANSLATION(holidaymode, "holidaymode", "holiday mode", "Urlaubsbetrieb", "Vakantiebedrijf", "Helgläge", "tryb urlopowy", "feriemodus", "mode vacances", "tatil modu", "modalita vacanze")
MAKE_TRANSLATION(flowtempoffset, "flowtempoffset", "flow temperature offset for mixer", "Vorlauftemperaturanhebung", "Mixer aanvoertemperatuur offset", "Temperaturkorrigering Flödestemp. Blandningsventil", "korekta temperatury przepływu dla miksera", "temperaturkorrigering av blandingsventil", "décalage température de bascule pour mélangeur", "karıştırıcı için akış sıcaklığı farkı", "aumento della temperatura di ritorno")
MAKE_TRANSLATION(reducemode, "reducemode", "reduce mode", "Absenkmodus", "Gereduceerde modus", "Reducerat Läge", "tryb zredukowany/obniżony", "redusert modus", "mode réduction", "düşürme modu", "modalità assente")
MAKE_TRANSLATION(noreducetemp, "noreducetemp", "no reduce below temperature", "Durchheizen unter", "Reduceermodus onderbreken onder", "Inaktivera reducering under", "bez redukcji poniżej temperatury", "inaktiver redusert nedre temp", "pas de réduction en dessous température", "bu sıcaklığın altına düşürme", "non ridurre temperatura sotto")
MAKE_TRANSLATION(reducetemp, "reducetemp", "off/reduce switch temperature", "Absenkmodus unter", "Onderste afschakeltemperatuur", "Avslag/Reducera under", "tryb zredukowany poniżej temperatury", "nedre avstengningstemperatur", "arrêt/réduction température bascule", "sıcaklık kapama/düşürme modu", "interruttore riduzione temperatura")
MAKE_TRANSLATION(vacreducetemp, "vacreducetemp", "vacations off/reduce switch temperature", "Urlaub Absenkmodus unter", "Vakantiemodus onderste afschakeltemperatuur", "Helg Avslag/Reducering under", "tryb urlopowy poniżej temperatury", "feriemodus nedre utkoblingstemperatur", "vacances – arrêt/réduction température bascule", "tatil sıcaklık kapama/düşürme modu", "interruttore riduzione temperatura vacanze")
MAKE_TRANSLATION(vacreducemode, "vacreducemode", "vacations reduce mode", "Urlaub Absenkmodus", "Vakantie afschakelmodus", "Helg reduceringsläge", "redukcja w trakcie urlopu", "ferieavstengningsmodus", "mode réduction vacances", "tail düşürme modu", "modalita riduzione vacanze")
MAKE_TRANSLATION(nofrostmode, "nofrostmode", "nofrost mode", "Frostschutz Modus", "Vorstbeveiligingsmodus", "Frostskyddsläge", "temperatura wiodąca dla ochrony przed zamarzaniem", "frostbeskyttelsesmodus", "mode protection gel", "donma koruması modu", "Modalità protezione antigelo")
MAKE_TRANSLATION(remotetemp, "remotetemp", "room temperature from remote", "Raumtemperatur Remote", "Ruimtetemperatuur van afstandsbediening", "Rumstemperatur från fjärr", "temperatura w pomieszczeniu (z termostatu)", "romstemperatur fra fjernbetjening", "température pièce depuis télécommande", "uzaktan oda sıcaklığı", "temperatura ambiente da remoto")
MAKE_TRANSLATION(remotehum, "remotehum", "room humidity from remote", "Raumfeuchte Remote", "", "", "", "", "",  "uzaktan kumandadan oda nemi", "") // TODO translate
MAKE_TRANSLATION(wwHolidays, "wwholidays", "holiday dates", "Feiertage", "Feestdagen", "Helgdagar", "dni świąteczne", "feriedager varmtvann", "dates vacances", "tatil günleri", "feste pubbliche")
MAKE_TRANSLATION(wwVacations, "wwvacations", "vacation dates", "Urlaubstage", "Vakantiedagen", "Semesterdatum Varmvatten", "dni urlopowe", "ferie dato varmtvann", "dates vacances", "izin günleri", "date vacanze")
MAKE_TRANSLATION(holidays, "holidays", "holiday dates", "Feiertage", "Feestdagen", "Helgdatum", "święta", "helligdager", "dates vacances", "tatil günleri", "date feste pubbliche")
MAKE_TRANSLATION(vacations, "vacations", "vacation dates", "Urlaubstage", "Vakantiedagen", "Semesterdatum", "urlop", "feriedager", "dates vacances", "izin günleri", "date vacanze")
MAKE_TRANSLATION(wwprio, "wwprio", "dhw priority", "WW-Vorrang", "Prioriteit warm water", "Prioritera Varmvatten", "priorytet dla c.w.u.", "prioroter varmtvann", "priorité ecs", "sıcak kullanım suyu önceliği", "priorita acqua calda ")
MAKE_TRANSLATION(nofrostmode1, "nofrostmode1", "nofrost mode", "Frostschutz", "Vorstbeveiligingsmodus", "Frostskyddsläge", "ochrona przed zamarzaniem", "frostbeskyttelse", "mode protection gel", "donma koruması modu 1", "modalita protezione antigelo")
MAKE_TRANSLATION(reducehours, "reducehours", "duration for nighttemp", "Dauer Nachttemp.", "Duur nachtverlaging", "Timmar Nattsänkning", "czas trwania trybu nocnego", "timer nattsenkning", "durée température nuit", "gece sıcaklığı süresi", "durata temperatura notturna")
MAKE_TRANSLATION(reduceminutes, "reduceminutes", "remaining time for nightmode", "Restzeit Nachttemp.", "Resterende tijd nachtverlaging", "Återstående Tid Nattläge", "czas do końca trybu nocnego", "gjenværende tid i nattstilling", "temps restant mode nuit", "gece modu için kalan süre", "temperatura notturna residua")
MAKE_TRANSLATION(switchonoptimization, "switchonoptimization", "switch-on optimization", "Einschaltoptimierung", "Inschakeloptimalisering", "Växlingsoptimering", "optymalizacja załączania", "slå på optimalisering", "optimisation mise en marche", "optimizasyonu aç", "ottimizzazione all'accensione")

MAKE_TRANSLATION(hpmode, "hpmode", "HP Mode", "WP Modus", "Modus warmtepomp", "", "", "", "", "yüksek güç modu", "Modalità Termopompa") // TODO translate
MAKE_TRANSLATION(dewoffset, "dewoffset", "dew point offset", "Taupunkt Differenz", "Offset dauwpunt", "", "", "", "", "çiğ noktası göreli", "differenza del punto di rugiada") // TODO translate
MAKE_TRANSLATION(roomtempdiff, "roomtempdiff", "room temp difference", "Raumtemperatur Differenz", "Verschiltemperatuur kamertemp", "", "", "", "", "oda sıcaklığı farkı", "differenza temperatura ambiente") // TODO translate
MAKE_TRANSLATION(hpminflowtemp, "hpminflowtemp", "HP min. flow temp.", "WP minimale Vorlauftemperatur", "Minimale aanvoertemperatuur WP", "", "", "", "", "yüksek güç minimum akış sıcaklığı", "temperatura minima di mandata") // TODO translate
MAKE_TRANSLATION(hpcooling, "cooling", "cooling", "Kühlen", "Koelen", "Kyler", "chłodzenie", "kjøling", "refroidissement", "soğuma", "raffreddamento")

// heatpump and RC100H
MAKE_TRANSLATION(airHumidity, "airhumidity", "relative air humidity", "relative Luftfeuchte", "Relatieve luchtvochtigheid", "Relativ Luftfuktighet", "wilgotność względna w pomieszczeniu", "luftfuktighet", "humidité relative air", "havadaki bağıl nem", "umidità relativa aria")
MAKE_TRANSLATION(dewTemperature, "dewtemperature", "dew point temperature", "Taupunkttemperatur", "Dauwpunttemperatuur", "Daggpunkt", "punkt rosy w pomieszczeniu", "duggtemperatur", "température point rosée", "çiğ noktası sıcaklığı", "temperatura del punto di rugiada")
MAKE_TRANSLATION(battery, "battery", "battery", "Batterie", "", "", "", "", "", "", "")
// mixer
MAKE_TRANSLATION(flowSetTemp, "flowsettemp", "setpoint flow temperature", "Sollwert Vorlauftemperatur", "Streefwaarde aanvoertemperatuur", "Vald flödestemperatur", "zadana temperatura zasilania", "valgt turtemperatur", "consigne température flux", "akış sıcaklığı ayarı", "Setpoint temperatura di mandata")
MAKE_TRANSLATION(flowTempHc, "flowtemphc", "flow temperature (TC1)", "Vorlauftemperatur HK (TC1)", "Aanvoertemperatuut circuit (TC1)", "Flödestemperatur (TC1)", "temperatura zasilania (TC1)", "turtemperatur (TC1)", "température flux (TC1)", "akış sıcaklığı (TC1)", "temperatura di mandata (TC1)")
MAKE_TRANSLATION(pumpStatus, "pumpstatus", "pump status (PC1)", "Pumpenstatus HK (PC1)", "pompstatus circuit (PC1)", "Pumpstatus (PC1)", "status pompy (PC1)", "pumpestatus (PC1)", "état pompe (PC1)", "pompa durumu (PC1)", "stato pompa (PC1)")
MAKE_TRANSLATION(mixerStatus, "valvestatus", "mixing valve actuator (VC1)", "Mischerventil Position (VC1)", "positie mixerklep (VC1)", "Shuntventil Status (VC1)", "siłownik zaworu mieszającego (VC1)", "shuntventil status (VC1)", "actionnement vanne mélangeur (VC1)", "karışım vanası aktüatörü (VC1)", "posizione valvola miscela (VC1)")
MAKE_TRANSLATION(flowTempVf, "flowtempvf", "flow temperature in header (T0/Vf)", "Vorlauftemperatur am Verteiler (T0/Vf)", "aanvoertemperatuur verdeler (T0/Vf)", "Flödestemperatur Fördelare (T0/Vf)", "temperatura zasilania na rozdzielaczu (T0/Vf)", "turtemperatur ved fordeleren (T0/Vf)", "température départ collecteur (T0/Vf)", "başlıkta akış sıcaklığı", "Temperatura di mandata al distributore (T0/Vf)")
MAKE_TRANSLATION(mixerSetTime, "valvesettime", "time to set valve", "Zeit zum Einstellen des Ventils", "Inschakeltijd mengklep", "Inställningstid Ventil", "czas na ustawienie zaworu", "instillningstid ventil", "délai activation vanne", "vana ayar zamanı", "ritardo attivazione valvola")
// mixer prefixed with wwc
MAKE_TRANSLATION(wwPumpStatus, "pumpstatus", "pump status in assigned wwc (PC1)", "Pumpenstatus des wwk (PC1)", "Pompstatus in WW circuit (PC1)", "Pumpstatus i VV-krets (PC1)", "stan pompy w obwodzie c.w.u. (PC1)", "Pumpestatus i VV-krets (PC1)", "état pompe wwc (PC1)", "Kullanım suyu devresindeki(PC1) pompa durumu", "stato pompa assegnato nel ciruito WW (PC1)")
MAKE_TRANSLATION(wwTempStatus, "wwtempstatus", "temperature switch in assigned wwc (MC1)", "Temperaturschalter des wwk (MC1)", "Temperatuurschakeling in WW circuit (MC1)", "Temperaturventil i VV-krets (MC1)", "temperatura w obwodzie c.w.u. (MC1)", "temperaturventil i VV-krets (MC1)", "température bascule wwc (MC1).", "atanmış sıcak su devresinde sıcaklık", "interruttore di temperatura del wwk (MC1)")
MAKE_TRANSLATION(wwTemp, "wwtemp", "current temperature", "aktuelle Temperatur", "huidige temperatuur", "Aktuell Temperatur", "temperatura c.w.u.", "aktuell temperatur", "température actuelle", "güncel sıcaklık", "temperatura attuale")
// mixer pool
MAKE_TRANSLATION(poolSetTemp, "poolsettemp", "pool set temperature", "Pool Solltemperatur", "Streeftemperatuur zwembad", "Pool Temperatur Börvärde", "zadana temperatura basenu", "valgt temp basseng", "température consigne piscine", "hedef havuz sıcaklığı", "temperatura nominale piscina")
MAKE_TRANSLATION(poolTemp, "pooltemp", "pool temperature", "Pool Temperatur", "Zwembadtemperatuur", "Pooltemperatur", "temperatura basenu", "bassengtemperatur", "température piscine", "havuz sıcaklığı", "temperatura piscina")
MAKE_TRANSLATION(poolShuntStatus, "poolshuntstatus", "pool shunt status opening/closing", "Pool Ventil öffnen/schließen", "Zwembadklep status openen/sluiten", "Pool Shunt-status öppnen/stängd", "status bocznika basenu", "bassengshunt-status åpen/stengt", "état shunt piscine ouvert/fermé", "havuz şant durumu açılıyor/kapanıyor", "aprire/chiudere valvola regolazione piscina")
MAKE_TRANSLATION(poolShunt, "poolshunt", "pool shunt open/close (0% = pool / 100% = heat)", "Pool Ventil Öffnung", "Mengklep zwembad stand", "Pool Shunt Öppen/Stängd", "bocznik basenu (0% = basen / 100% = grzanie)", "bassengshunt åpen/stengt (0% = basseng / 100% = varme)", "ouverture/fermeture shunt piscine (0% = piscine / 100% = chaleur).", "havuz şant açık/kapalı (0% = havuz / 100% = ısıtma)", "valvola regolazione piscina (0% = piscina / 100% = caldo)")
MAKE_TRANSLATION(hydrTemp, "hydrTemp", "hydraulic header temperature", "Verteilertemperatur", "Temperatuur open verdeler", "Fördelartemperatur", "temperatura kolektora hydraulicznego", "Fordelertemperatur", "température collecteur hydraulique", "hidrolik başlık sıcaklığı ", "temperatura del collettore")

// solar
MAKE_TRANSLATION(cylMiddleTemp, "cylmiddletemp", "cylinder middle temperature (TS3)", "Speichertemperatur Mitte (TS3)", "Zonneboilertemperatuur midden (TS3)", "Cylindertemperatur Mitten (TS3)", "temperatura w środku zasobnika (TS3)", "beredertemperatur i midten (TS3)", "température moyenne cylindre (TS3)", "orta depolama sıcaklığı (TS3)", "temperatura di conservazione media accumulo (TS3)")
MAKE_TRANSLATION(retHeatAssist, "retheatassist", "return temperature heat assistance (TS4)", "Rücklaufanhebungs-Temp. (TS4)", "Retourtemperatuur verwarmingsassistentie (TS4)", "Returtemperatur värmestöd (TS4)", "temperatura powrotu wspomagania grzania (TS4)", "returtemperatur varmestøtte (TS4)", "température retour de assistance thermique (TS4)", "geri dönüş sıcaklığı artışı", "temperatura ritorno scambiatore (TS4)")
MAKE_TRANSLATION(m1Valve, "heatassistvalve", "heat assistance valve (M1)", "Ventil Heizungsunterstützung (M1)", "Klep verwarmingsassistentie (M1)", "Uppvärmningsstöd Ventil (M1)", "zawór wspomagania grzania (M1)", "varmehjelpsventil (M1)", "vanne assistance thermique (M1)", "ısıtma yardım vanası (M1)", "valvola scambiatore (M1)")
MAKE_TRANSLATION(m1Power, "heatassistpower", "heat assistance valve power (M1)", "Ventilleistung Heizungsunterstützung (M1)", "Vermogen klep verwarmingsassistentie (M1)", "Uppvärmningsstöd Ventil Effekt (M1)", "moc zaworu wspomagania grzania (M1)", "varmehjelpsventileffekt (M1)", "puissance vanne assistance thermique (M1)", "ısıtma yardım vanası gücü (M1)", "potenza valvola scambiatore (M1)")
MAKE_TRANSLATION(pumpMinMod, "pumpminmod", "minimum pump modulation", "minimale Pumpenmodulation", "Minimale pompmodulatie", "Min Pumpmodulering", "minimalna modulacja pompy", "minimum pumpmodulering", "modulation minimale pompe", "minimum pompa modülasyonu", "modulazione minima pompa")
MAKE_TRANSLATION(maxFlow, "maxflow", "maximum solar flow", "maximaler Durchfluss", "Maximale doorstroom solar", "Max Flöde Solpanel", "maksymalny przepływ solarów", "maks strømming solpanel ", "débit solaire maximum", "minimum G.E. akışı", "portata massima solare")
MAKE_TRANSLATION(solarPower, "solarpower", "actual solar power", "aktuelle Solarleistung", "Huidig solar vermogen", "Aktuellt Sol-effekt", "aktualna moc solarów", "aktuell soleffekt", "puissance solaire réelle", "gerçek G.E. gücü", "potenza attuale solare")
MAKE_TRANSLATION(solarPumpTurnonDiff, "turnondiff", "pump turn on difference", "Einschalthysterese Pumpe", "Inschakelhysterese pomp", "Aktiveringshysteres Pump", "histereza załączenia pompy", "slå på hysteresepumpe", "différence activation pompe", "pompa devreye alma farkı", "isteresi di accensione pompa")
MAKE_TRANSLATION(solarPumpTurnoffDiff, "turnoffdiff", "pump turn off difference", "Ausschalthysterese Pumpe", "Uitschakelhysterese pomp", "Avslagshysteres Pump", "histereza włączenia pompy", "slå av hysteresepumpe", "différence arrêt pompe", "pompa kapama farkı", "isteresi di spegnimento pompa")
MAKE_TRANSLATION(pump2MinMod, "pump2minmod", "minimum pump 2 modulation", "minimale Modulation Pumpe 2", "Minimale modulatie pomp 2", "Min Modulering Pump 2", "minimalna modulacja pompy 2", "minimum pumpmodulering 2", "modulation minimale pompe 2", "minimum pompa 2 modülasyonu", "modulazione minima pompa 2")
MAKE_TRANSLATION(solarPump2TurnonDiff, "turnondiff2", "pump 2 turn on difference", "Einschalthysterese Pumpe 2", "Inschakelhysterese pomp 2", "Aktiveringshysteres Pump 2", "histereza załączenia pompy 2", "slå på hysteresepumpe 2", "différence activation pompe 2", "pompa 2 devreye alma farkı", "isteresi di accensione pompa 2")
MAKE_TRANSLATION(solarPump2TurnoffDiff, "turnoffdiff2", "pump 2 turn off difference", "Ausschalthysterese Pumpe 2", "Uitschakelhysterese pomp 2", "Avslagshysteres Pump 2", "histereza wyłączenia pompy 2", "slå av hysteresepumpe 2", "différence arrêt pompe 2", "pompa 2 kapama farkı", "isteresi di spegnimento pompa")
MAKE_TRANSLATION(collectorTemp, "collectortemp", "collector temperature (TS1)", "Kollektortemperatur (TS1)", "Collectortemperatuur (TS1)", "Kollektor Temperatur (TS1)", "temperatura kolektora (TS1)", "kollektor temperatur (TS1)", "température collecteur (TS1)", "kollektör sıcaklığı (TS1)", "temperatura collettore (TS1)")
MAKE_TRANSLATION(collector2Temp, "collector2temp", "collector 2 temperature (TS7)", "Kollector 2 Temperatur (TS7)", "Collector 2 temperatuur (TS7)", "Kollektor 2 Temperatur (TS7)", "temperatura kolektora 2 (TS7)", "kollektor 2 temperatur (TS7)", "température collecteur 2 (TS7)", "kollektör 2 sıcaklığı (TS2)", "temperatura collettore 2 (TS7)")
MAKE_TRANSLATION(cylBottomTemp, "cylbottomtemp", "cylinder bottom temperature (TS2)", "Speicher Bodentemperatur (TS2)", "Bodemtemperatuur zonneboiler (TS2)", "Cylindertemperatur Botten (TS2)", "temperatura na spodzie zasobnika (TS2)", "beredertemp i bunn (TS2)", "température fond de cylindre (TS2)", "alt depolama sıcaklığıc(TS2)", "temperatura inferiore accumulo (TS2)")
MAKE_TRANSLATION(cyl2BottomTemp, "cyl2bottomtemp", "second cylinder bottom temperature (TS5)", "2. Speicher Bodentemperatur (TS5)", "Bodemtemperatuur 2e boiler", "Sekundär Cylindertemperatur Botten (TS5)", "temperatura na spodzie drugiego zasobnika (TS5)", "skundær beredertemp i bunn (TS5)", "température fond de cylindre (TS5)", "ikinci alt depolama sıcaklığıc(TS5)", "temperatura inferiore 2° accumulo (TS5)")
MAKE_TRANSLATION(heatExchangerTemp, "heatexchangertemp", "heat exchanger temperature (TS6)", "wärmetauscher Temperatur (TS6)", "Temperatuur warmtewisselaar (TS6)", "Värmeväxlare Temperatur (TS6)", "temperatura wymiennika ciepła (TS6)", "Varmeveksler temperatur (TS6)", "température échangeur de chaleur (TS6)", "eşanjör sıcaklığı (TS6)", "temperatura scambiatore calore (TS6)")
MAKE_TRANSLATION(collectorMaxTemp, "collectormaxtemp", "maximum collector temperature", "maximale Kollektortemperatur", "Maximale collectortemperatuur", "Max Kollektortemperatur", "maksymalna temperatura kolektora", "maks kollektortemperatur", "température max. collecteur", "maksimum kollektör sıcaklığı", " temperatura massima scambiatore calore")
MAKE_TRANSLATION(collectorMinTemp, "collectormintemp", "minimum collector temperature", "minimale Kollektortemperatur", "Minimale collectortemperatuur", "Min Kollektortemperatur", "minimalna temperatura kolektora", "min kollektortemperatur", "température min. collecteur", "minimum kollektör sıcaklığı", "temperatura minima scambiatore calore")
MAKE_TRANSLATION(cylMaxTemp, "cylmaxtemp", "maximum cylinder temperature", "maximale Speichertemperatur", "maximale temperatuur zonneboiler", "Max Cylindertemperatur", "maksymalna temperatura zasobnika", "maks beredertemperatur", "température max. cylindre", "maksimum silindir sıcaklığı", "temperatura massima vaso accumulo")
MAKE_TRANSLATION(solarPumpMod, "solarpumpmod", "pump modulation (PS1)", "Pumpenmodulation (PS1)", "Pompmodulatie (PS1)", "Pumpmodulering (PS1)", "modulacja pompy solarnej (PS1)", "solpumpmodulering (PS1)", "modulation pompe (PS1)", "pompa modülasyonu (PS1)", "modulazione pompa (PS1)")
MAKE_TRANSLATION(cylPumpMod, "cylpumpmod", "cylinder pump modulation (PS5)", "Speicherpumpenmodulation (PS5)", "Modulatie zonneboilerpomp (PS5)", "Cylinderpumpmodulering (PS5)", "modulacja pompy zasobnika (PS5)", "sylinderpumpemodulering (P55)", "modulation pompe cylindre (PS5)", "silindir pompa modülasyonu (PS5)", "pompa modulazione accumulo (PS5)")
MAKE_TRANSLATION(solarPump, "solarpump", "pump (PS1)", "Pumpe (PS1)", "Pomp (PS1)", "Pump (PS1)", "pompa solarna (PS1)", "solpumpe (PS1)", "pompe solaire (PS1)", "pompa (PS1)", "pompa solare (PS1)")
MAKE_TRANSLATION(solarPump2, "solarpump2", "pump 2 (PS4)", "Pumpe 2 (PS4)", "Pomp 2 (PS4)", "Pump 2 (PS4)", "pompa solarna 2 (PS4)", "solpumpe 2 (PS4)", "pompe 2 (PS4)", "pompa 2 (PS4)", "pompa solare 2 (PS4)")
MAKE_TRANSLATION(solarPump2Mod, "solarpump2mod", "pump 2 modulation (PS4)", "Pumpe 2 Modulation (PS4)", "Modulatie pomp 2 (PS4)", "Pump 2 Modulering (PS4)", "modulacja pompy solarnej 2 (PS4)", "solpumpe2modulering (PS4)", "modulation pompe solaire 2 (PS4)", "pompa2 modülasyonu(PS1)", "pompa modulazione 2 (PS4)")
MAKE_TRANSLATION(valveStatus, "valvestatus", "valve status", "Ventilstatus", "Klepstatus", "Ventilstatus", "stan zaworu", "ventilstatus", "statut valve", "vana durumu", "stato valvola")
MAKE_TRANSLATION(vs1Status, "vs1status", "valve status VS1", "Ventilstatus VS1", "Klepstatus VS1", "Ventilstatus VS1", "stan zaworu VS1", "ventilstatus VS1", "statut valve VS1", "vana durumu VS1", "stato valvola VS1")
MAKE_TRANSLATION(cylHeated, "cylheated", "cyl heated", "Speichertemperatur erreicht", "Boilertemperatuur behaald", "Värmepanna Uppvärmd", "zasobnik został nagrzany", "bereder oppvarmt", "cylindre chauffé", "depolama sıcakllığına ulaşıldı", "temperatura richiesta vaso accumulo raggiunta")
MAKE_TRANSLATION(collectorShutdown, "collectorshutdown", "collector shutdown", "Kollektorabschaltung", "Collector afschakeling", "Kollektor Avstängning", "wyłączenie kolektora", "kollektor stengt", "arrêt collecteur", "kollektör kapalı", "spegnimento del collettore")
MAKE_TRANSLATION(pumpWorkTime, "pumpworktime", "pump working time", "Pumpenlaufzeit", "Pomplooptijd", "Pump Drifttid", "czas pracy pompy", "driftstid pumpe", "durée fonctionnement pompe", "pompa çalışma süresi", "tempo funzionamento pompa")
MAKE_TRANSLATION(pump2WorkTime, "pump2worktime", "pump 2 working time", "Pumpe 2 Laufzeit", "Looptijd pomp 2", "Pump 2 Drifttid", "czas pracy pompy 2", "driftstid pumpe2", "durée fonctionnement pompe 2", "pompa 2 çalışma süresi", "tempo funzionamento pompa 2")
MAKE_TRANSLATION(m1WorkTime, "m1worktime", "differential control working time", "Differenzregelung Arbeitszeit", "Verschilregeling arbeidstijd", "Differentialreglering Drifttid", "czas pracy regulacji różnicowej", "differentialreguleringssrifttid", "durée fonctionnement contrôle différentiel", "çalışma saatlerinin farklı düzenlenmesi", "controllo differenziale durata funzionamento")
MAKE_TRANSLATION(energyLastHour, "energylasthour", "energy last hour", "Energie letzte Std", "Energie laatste uur", "Energi Senaste Timmen", "energia w ciągu ostatniej godziny", "energi siste time", "énergie dernière heure", "son saat enerji", "Eenergia ultima ora")
MAKE_TRANSLATION(energyTotal, "energytotal", "total energy", "Gesamtenergie", "Totale energie", "Total Energi", "energia całkowita", "total energi", "énergie totale", "toplam enerji", "energia totale") 
MAKE_TRANSLATION(energyToday, "energytoday", "total energy today", "Energie heute", "Energie vandaag", "Total Energi Idag", "energia całkowita dzisiaj", "total energi i dag", "énergie totale aujourd'hui", "bugün toplam enerji", "totale energia giornaliera")

// solar ww
MAKE_TRANSLATION(wwTemp1, "wwtemp1", "temperature 1", "Temperatur 1", "Temperatuur 1", "Temperatur 1", "temperatura 1", "temperatur 1", "température 1", "sıcaklık 1", "Temperatura 1")
MAKE_TRANSLATION(wwTemp3, "wwtemp3", "temperature 3", "Temperatur 3", "Temperatuur 2", "Temperatur 2", "temperatura 2", "Temperatur 3", "température 3", "sıcaklık 3", "Temperatura 3")
MAKE_TRANSLATION(wwTemp4, "wwtemp4", "temperature 4", "Temperatur 4", "Temperatuur 3", "Temperatur 3", "temperatura 3", "Temperatur 4", "température 4", "sıcaklık 4", "Temperatura 4")
MAKE_TRANSLATION(wwTemp5, "wwtemp5", "temperature 5", "Temperatur 5", "Temperatuur 5", "Temperatur 4", "temperatura 4", "Temperatur 5", "température 5", "sıcaklık 5", "Temperatura 5")
MAKE_TRANSLATION(wwTemp7, "wwtemp7", "temperature 7", "Temperatur 7", "Temperatuur 7", "Temperatur 5", "temperatura 5", "Temperatur 7", "température 7", "sıcaklık 7", "Temperatura 7")
MAKE_TRANSLATION(wwPump, "wwpump", "pump", "Pumpe", "Pomp", "Pump", "pompa", "pumpe", "pompe", "pompa", "Pompa")
// solar ww and mixer wwc
MAKE_TRANSLATION(wwMinTemp, "wwmintemp", "minimum temperature", "minimale Temperatur", "Minimale temperatuur", "Min Temperatur", "temperatura minimalna", "min temperatur", "température min", "minimum sıcaklık", "temperatura minima")
MAKE_TRANSLATION(wwRedTemp, "wwredtemp", "reduced temperature", "reduzierte Temperatur", "Gereduceerde temperatuur", "Reducerad Temperatur", "temperatura zredukowana", "reducert temperatur", "température réduite", "düşürülmüş sıcaklık", "temperatura ridotta")
MAKE_TRANSLATION(wwDailyTemp, "wwdailytemp", "daily temperature", "tägl. Temperatur", "Dagelijkse temperatuur", "Daglig temperatur", "temperatura dzienna", "dagtemperatur", "température en journée", "günlük sıcaklık", "temperatura giornaliera")
MAKE_TRANSLATION(wwKeepWarm, "wwkeepwarm", "keep warm", "Warmhalten", "Warm houde", "Varmhållning", "utrzymywanie ciepła", "holde varmen", "maintenir chaleur", "ılık tut", "mantenimento calore")
MAKE_TRANSLATION(wwStatus2, "wwstatus2", "status 2", "Status 2", "Status 2", "Status 2", "status 2", "status 2", "statut 2", "durum 2", "Status 2")
MAKE_TRANSLATION(wwPumpMod, "wwpumpmod", "pump modulation", "Pumpen Modulation", "Pompmodulatie", "Pumpmodulering", "modulacja pompy", "pumpemodulering", "modulation de pompe", "pompa modülasyonu", "modulazione pompa")
MAKE_TRANSLATION(wwFlow, "wwflow", "flow rate", "Volumenstrom", "Doorstroomsnelheid", "Flöde", "przepływ", "strømningshastighet", "débit", "akış hızı", "portata flusso")
// extra mixer ww
MAKE_TRANSLATION(wwRequiredTemp, "wwrequiredtemp", "required temperature", "benötigte Temperatur", "Benodigde temperatuur", "Nödvändig Temperatur", "temperatura wymagana", "nødvendig temperatur", "température requise", "gerekli sıcaklık", "temperatura richiesta")
MAKE_TRANSLATION(wwDiffTemp, "wwdifftemp", "start differential temperature", "Start Differential Temperatur", "Start differentiele temperatuur", "Start Differentialtemperatur", "start temperatury różnicowej", "start differensialtemperatur", "température différentielle de départ", "diferansiyel sıcaklık", "avvia temperatura differenziale")

// SM100
MAKE_TRANSLATION(heatTransferSystem, "heattransfersystem", "heattransfer system", "Wärmeübertragungs-System", "Warmteoverdrachtssysteem", "Värmeöverföringssystem", "system wymiany ciepła", "varmeoverføringssystem", "système de transfert de chaleur", "ıs transfer sistemi", "sistema di trasferimento del calore")
MAKE_TRANSLATION(externalCyl, "externalcyl", "external cylinder", "Externer Speicher", "Externe boiler", "Extern Cylinder", "zbiornik zewnętrzny", "ekstern bereder", "cylindre externe", "dış silindir", "vaso accumulo esterno")
MAKE_TRANSLATION(thermalDisinfect, "thermaldisinfect", "thermal disinfection", "Thermische Desinfektion", "Thermische desinfectie", "Termisk Desinfektion", "dezynfekcja termiczna", "termisk desinfeksjon", "désinfection thermique", "ısıl temizlik", "disinfezione termica")
MAKE_TRANSLATION(heatMetering, "heatmetering", "heatmetering", "Wärmemessung", "warmtemeting", "Värmemätning", "pomiar ciepła", "varmemåling", "mesure de chaleur", "ısı ölçümü", "misurazione del calore")
MAKE_TRANSLATION(solarIsEnabled, "solarenabled", "solarmodule enabled", "Solarmodul aktiviert", "Solarmodule geactiveerd", "Solmodul Aktiverad", "system solarny", "solmodul aktivert", "module solaire activé", "güneş modu etkinleştirildi", "modulo solare attivato")

// telegram 0x035A
MAKE_TRANSLATION(solarPumpMode, "solarpumpmode", "pump mode", "Solar Pumpen Einst.", "Modus zonneboilerpomp", "Sol Pumpläge", "tryb pracy pompy", "solpumpemodus", "mode pompe solaire", "pompa modu", "modalità pompa solare")
MAKE_TRANSLATION(solarPumpKick, "pumpkick", "pump kick", "Röhrenkollektorfunktion", "Modus buizencollector", "Sol Kollektorfunktion", "wspomaganie startu pompy", "solkllektorfunksjon", "démarrage boost pompe solaire", "pompa zorunlu çalıştırma", "avvio forzato pompa")
MAKE_TRANSLATION(plainWaterMode, "plainwatermode", "plain water mode", "Südeuropafunktion", "Modus Zuid-Europa", "Sydeuropa-funktion", "tylko woda (dla Europy Południowej)", "vanlig vannmodus", "mode eau ordinaire", "sadece su modu", "modalità acqua normale")
MAKE_TRANSLATION(doubleMatchFlow, "doublematchflow", "doublematchflow", "Double Match Flow", "Double Match Flow", "Dubbelmatchning Flöde", "przepływ podwójnie dopasowany", "dobbelmatch flow", "double match flow", "doublematch akışı", "carico ottimizzato dell'accumulatore ad effetto termosifone ")
MAKE_TRANSLATION(solarPump2Mode, "pump2mode", "pump 2 mode", "Pumpe 2 Modus", "Modus pomp 2", "Pump 2 Läge", "tryb pracy pompy 2", "pump 2 modus", "mode pompe 2", "pompa 2 modu", "modalità pompa 2")
MAKE_TRANSLATION(solarPump2Kick, "pump2kick", "pump kick 2", "Pumpe 2 Startboost", "Startboost pomp 2", "Pump 2 Kollektorfunktion", "wspomaganie startu pompy 2", "startboost pumpe 2", "démarrage boost pompe 2", "pompa 2 zorunlu çalıştırma", "avvio forzato pompa 2")

// telegram 0x035F
MAKE_TRANSLATION(cylPriority, "cylpriority", "cylinder priority", "Speicher Priorität", "Prioriteit boiler", "Cylinderprioritering", "priorytet cylindra", "berederprioritering", "priorité de cylindre", "silindir önceliği", "priorità vaso accumulo")

// telegram 0x380
MAKE_TRANSLATION(climateZone, "climatezone", "climate zone", "Klimazone", "klimaatzone", "Klimatzon", "strefa klimatyczna", "klimasone", "zone de climat", "iklim alanı", "zona clima")
MAKE_TRANSLATION(collector1Area, "collector1area", "collector 1 area", "Kollektor 1 Fläche", "oppervlakte collector 1", "Kollektor 1 Area", "powierzchnia kolektora 1", "kollektor 1 område", "zone collecteur 1", "kollektör 1 alan", "area collettore 1")
MAKE_TRANSLATION(collector1Type, "collector1type", "collector 1 type", "Kollektor 1 Typ", "Type collector 1", "Kollektor 1 Typ", "typ kolektora 1", "kollektor 1 type", "type collecteur 1", "kollektör 1 tip", "tipo collettore 1")
MAKE_TRANSLATION(collector2Area, "collector2area", "collector 2 area", "Kollektor 2 Fläche", "Oppervlakte collector 2", "Kollektor 2 Area", "powierzchnia kolektora 2", "kollektor 2 område", "zone collecteur 2", "kollektör 2 alan", "area collettore 2")
MAKE_TRANSLATION(collector2Type, "collector2type", "collector 2 type", "Kollektor 2 Typ", "Type collector 2", "Kollektor 2 Typ", "typ kolektora 2", "kollektor 2 type", "type collecteur 2", "kollektör 2 tip", "tipo collettore 2")

// telegram 0x0363 heatCounter
MAKE_TRANSLATION(heatCntFlowTemp, "heatcntflowtemp", "heat counter flow temperature", "Wärmezähler Vorlauf-Temperatur", "Aanvoertemperatuur warmteenergiemeter", "Värmeräknare Flödestemperatur", "temperatura zasilania ciepłomierza", "varmeenergimåler turtemperatur", "température flux compteur chaleur", "ısı sayacı akış sıcaklığı", "Temperatura di mandata del contatore di calore")
MAKE_TRANSLATION(heatCntRetTemp, "heatcntrettemp", "heat counter return temperature", "Wärmezähler Rücklauf-Temperatur", "Retourtemperatuur warmteenergiemeter", "Värmeräknare Returtemperatur", "temperatura powrotu ciepłomierza", "varmeenergimåler returtemperatur", "température retour compteur chaleur", "ısı sayacı dönüş sıcaklığı", "Temperatura di ritorno del contatore di calore")
MAKE_TRANSLATION(heatCnt, "heatcnt", "heat counter impulses", "Wärmezähler Impulse", "Warmteenergiemeter pulsen", "Värmeräknare Impuls", "liczba impulsów ciepłomierza", "varmemåler impuls", "impulsions compteur chaleur", "ısı sayacı atış adedi", "contacalore a impulsi")
MAKE_TRANSLATION(swapFlowTemp, "swapflowtemp", "swap flow temperature (TS14)", "Austausch Vorlauf-Temperatur (TS14)", "Aanvoertemperatuur verwisselaar (TS14)", "Växlingstemperatur Flöde (TS14)", "temperatura zasilania wymiennika", "veksler turledningstemperatur (TS14)", "température flux échangeur (TS14)", "değişim akış sıcaklığı(TS14)", "Scambiare la temperatura di mandata (TS14)")
MAKE_TRANSLATION(swapRetTemp, "swaprettemp", "swap return temperature (TS15)", "Austausch Rücklauf-Temperatur (TS15)", "Retourtemperatuur verwisselaar (TS15)", "Växlingstemperatur Returflöde (TS15)", "temperatura powrotu wymiennika", "veksler returledningstemperatur (TS15)", "température retour échangeur (TS15)", "değişim dönüş sıcaklığı(TS15)", "Scambiare la temperatura di ritorno (TS15)")

// switch
MAKE_TRANSLATION(activated, "activated", "activated", "Aktiviert", "Geactiveerd", "Aktiverad", "aktywowany", "aktivert", "activé", "başladı", "attivato")
MAKE_TRANSLATION(status, "status", "status", "Status", "Status", "Status", "status", "status", "statut", "durum", "Stato")

// RF sensor, id 0x40, telegram 0x435
MAKE_TRANSLATION(RFTemp, "rftemp", "RF room temperature sensor", "RF Raumtemperatur Sensor", "RF ruimtetemperatuur sensor", "RF Rumsgivare Temp", "bezprzewodowy czujnik temperatury pomieszczenia", "RF romsgiver temp", "capteur de température de pièce RF", "RF oda sıcaklık sensörü", "Sensore di temperatura ambiente RF")

// ventilation
MAKE_TRANSLATION(outFresh, "outfresh", "outdoor fresh air", "Außenlufttemp.", "temperatuur buitenlucht", "", "", "", "", "dış ortam taze hava", "aria fresca esterna") // TODO translate
MAKE_TRANSLATION(inFresh, "infresh", "indoor fresh air", "Zulufttemp.", "temperatuur aanvoer", "", "", "", "", "iç ortam taze hava", "aria fresca interna") // TODO translate
MAKE_TRANSLATION(outEx, "outexhaust", "outdoor exhaust air", "Fortlufttemp.", "uitlaatemperatuur buiten", "", "", "", "", "dış ortam egsoz", "aria di scarico esterna") // TODO translate
MAKE_TRANSLATION(inEx, "inexhaust", "indoor exhaust air", "Ablufttemp.", "uitlaattemperatuur binnen", "", "", "", "", "iç ortam egsoz", "aria di scarico interna") // TODO translate
MAKE_TRANSLATION(ventMode, "ventmode", "ventilation mode", "Belüftungsmodus", "ventilatiemodus", "", "", "", "", "havalandırma modu", "modalità di ventilazione") // TODO translate
MAKE_TRANSLATION(ventInSpeed, "ventinspeed", "in blower speed", "Zuluft-Drehzahl", "toerental aanvoerventilator", "", "", "", "", "iç fan hızı", "velocità aria di alimentazione") // TODO translate
MAKE_TRANSLATION(ventOutSpeed, "ventoutspeed", "out blower speed", "Abluft-Drehzahl", "toerental afvoerventilator", "", "", "", "", "dış fan hızı", "velocità aria di scarico") // TODO translate
MAKE_TRANSLATION(airquality, "airquality", "air quality (voc)", "Luftqualität (VOC)", "luchtkwaliteit (VOC)", "", "", "", "", "hava kalitesi(voc)", "qualità aria (VOC)") // TODO translate

/*
// unknown fields to track (SM10), only for testing
// **** NO TRANSLATION NEEDED ****
MAKE_TRANSLATION(data11, "data11", "unknown datafield 11", "", "", "", "", "", "", "", "")
MAKE_TRANSLATION(data12, "data12", "unknown datafield 12", "", "", "", "", "", "", "", "")
MAKE_TRANSLATION(data8, "data8", "unknown datafield 8", "", "", "", "", "", "", "", "")
MAKE_TRANSLATION(data0, "data0", "unknown datafield 0", "", "", "", "", "", "", "", "")
MAKE_TRANSLATION(data1, "data1", "unknown datafield 1", "", "", "", "", "", "", "", "")
MAKE_TRANSLATION(setting3, "setting3", "unknown setting 3", "", "", "", "", "", "", "", "")
MAKE_TRANSLATION(setting4, "setting4", "unknown setting 4", "", "", "", "", "", "", "", "")
*/

// clang-format on
