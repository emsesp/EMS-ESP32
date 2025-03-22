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

// clang-format off

/*
 * These is the EMS devices that we currently recognize
 * The types and flags are stored in emsdevice.h 
 */

// Boilers - 0x08
{  8, DeviceType::BOILER, "CS*800i, Logatherm WLW*", DeviceFlags::EMS_DEVICE_FLAG_HEATPUMP},
{ 12, DeviceType::BOILER, "C1200W", DeviceFlags::EMS_DEVICE_FLAG_NONE},
{ 64, DeviceType::BOILER, "BK13/BK15, Smartline, GB1*2", DeviceFlags::EMS_DEVICE_FLAG_NONE},
{ 72, DeviceType::BOILER, "Logano GB1*5, Logamatic MC10", DeviceFlags::EMS_DEVICE_FLAG_EMS},
{ 81, DeviceType::BOILER, "Cascade CM10", DeviceFlags::EMS_DEVICE_FLAG_NONE},
{ 84, DeviceType::BOILER, "Logamax Plus GB022", DeviceFlags::EMS_DEVICE_FLAG_NONE},
{ 95, DeviceType::BOILER, "Condens, Logamax/Logomatic, Cerapur Top, Greenstar, Generic HT3", DeviceFlags::EMS_DEVICE_FLAG_HT3},
{115, DeviceType::BOILER, "Topline, GB162", DeviceFlags::EMS_DEVICE_FLAG_NONE},
{121, DeviceType::BOILER, "Cascade MCM10", DeviceFlags::EMS_DEVICE_FLAG_NONE},
{122, DeviceType::BOILER, "Proline", DeviceFlags::EMS_DEVICE_FLAG_NONE},
{123, DeviceType::BOILER, "GB*72, Trendline, Cerapur, Greenstar Si", DeviceFlags::EMS_DEVICE_FLAG_NONE},
{131, DeviceType::BOILER, "GB212", DeviceFlags::EMS_DEVICE_FLAG_NONE},
{132, DeviceType::BOILER, "GC7000F", DeviceFlags::EMS_DEVICE_FLAG_NONE},
{133, DeviceType::BOILER, "Logano GB125/KB195i, Logamatic MC110", DeviceFlags::EMS_DEVICE_FLAG_NONE},
{154, DeviceType::BOILER, "Greenstar 30Ri Compact", DeviceFlags::EMS_DEVICE_FLAG_NONE},
{155, DeviceType::BOILER, "Suprapur-o", DeviceFlags::EMS_DEVICE_FLAG_NONE},
{167, DeviceType::BOILER, "Cerapur Aero", DeviceFlags::EMS_DEVICE_FLAG_NONE},
{168, DeviceType::BOILER, "Hybrid Heatpump", DeviceFlags::EMS_DEVICE_FLAG_HYBRID},
{170, DeviceType::BOILER, "Logano GB212", DeviceFlags::EMS_DEVICE_FLAG_NONE},
{172, DeviceType::BOILER, "Enviline, Compress 6000AW, Hybrid 3000-7000iAW, SupraEco/Geo 5xx, WLW196i/WSW196i", DeviceFlags::EMS_DEVICE_FLAG_HEATPUMP},
{173, DeviceType::BOILER, "Geo 5xx", DeviceFlags::EMS_DEVICE_FLAG_HEATPUMP},
{195, DeviceType::BOILER, "Condens 5000i, Greenstar 8000/GC9800IW, GB192i*2", DeviceFlags::EMS_DEVICE_FLAG_NONE},
{203, DeviceType::BOILER, "Logamax U122, Cerapur", DeviceFlags::EMS_DEVICE_FLAG_NONE},
{206, DeviceType::BOILER, "Ecomline Excellent", DeviceFlags::EMS_DEVICE_FLAG_NONE},
{208, DeviceType::BOILER, "Logamax Plus, GB192, Condens GC9000, Greenstar ErP", DeviceFlags::EMS_DEVICE_FLAG_NONE},
{210, DeviceType::BOILER, "Cascade MC400", DeviceFlags::EMS_DEVICE_FLAG_NONE},
{211, DeviceType::BOILER, "EasyControl Adapter", DeviceFlags::EMS_DEVICE_FLAG_NONE},
{219, DeviceType::BOILER, "Greenstar HIU, Logamax kompakt WS170", DeviceFlags::EMS_DEVICE_FLAG_HIU},
{234, DeviceType::BOILER, "Logamax Plus GB122, Condense 2300, Junkers Cerapur GC2200W", DeviceFlags::EMS_DEVICE_FLAG_NONE},

// Controllers - 0x09 / 0x10 / 0x50
{ 68, DeviceType::CONTROLLER, "BC10, RFM20", DeviceFlags::EMS_DEVICE_FLAG_NONE}, // 0x09
{ 81, DeviceType::CONTROLLER, "CM10", DeviceFlags::EMS_DEVICE_FLAG_NONE},
{ 84, DeviceType::CONTROLLER, "GB022", DeviceFlags::EMS_DEVICE_FLAG_NONE},
{ 89, DeviceType::CONTROLLER, "BC10, GB142", DeviceFlags::EMS_DEVICE_FLAG_NONE}, // 0x09
{ 95, DeviceType::CONTROLLER, "HT3", DeviceFlags::EMS_DEVICE_FLAG_NONE}, // 0x09
{114, DeviceType::CONTROLLER, "BC10", DeviceFlags::EMS_DEVICE_FLAG_NONE}, // 0x09
{121, DeviceType::CONTROLLER, "MCM10", DeviceFlags::EMS_DEVICE_FLAG_NONE},
{125, DeviceType::CONTROLLER, "BC25", DeviceFlags::EMS_DEVICE_FLAG_NONE}, // 0x09
{152, DeviceType::CONTROLLER, "Controller", DeviceFlags::EMS_DEVICE_FLAG_NONE}, // 0x09
{166, DeviceType::CONTROLLER, "Suprapur-o", DeviceFlags::EMS_DEVICE_FLAG_NONE}, // 0x09
{168, DeviceType::CONTROLLER, "Hybrid Heatpump", DeviceFlags::EMS_DEVICE_FLAG_NONE}, // 0x09
{169, DeviceType::CONTROLLER, "BC40", DeviceFlags::EMS_DEVICE_FLAG_NONE}, // 0x09
{190, DeviceType::CONTROLLER, "BC10", DeviceFlags::EMS_DEVICE_FLAG_NONE}, // 0x09
{194, DeviceType::CONTROLLER, "BC10", DeviceFlags::EMS_DEVICE_FLAG_NONE}, // 0x09
{206, DeviceType::CONTROLLER, "Ecomline", DeviceFlags::EMS_DEVICE_FLAG_NONE}, // 0x09
{207, DeviceType::CONTROLLER, "Sense II/CS200", DeviceFlags::EMS_DEVICE_FLAG_NONE}, // 0x10
{209, DeviceType::CONTROLLER, "ErP", DeviceFlags::EMS_DEVICE_FLAG_NONE}, // 0x09
{224, DeviceType::CONTROLLER, "9000i", DeviceFlags::EMS_DEVICE_FLAG_NONE}, // 0x09
{229, DeviceType::CONTROLLER, "8700i", DeviceFlags::EMS_DEVICE_FLAG_NONE}, // 0x09
{230, DeviceType::CONTROLLER, "BC Base", DeviceFlags::EMS_DEVICE_FLAG_NONE}, // 0x09
{240, DeviceType::CONTROLLER, "Rego 3000", DeviceFlags::EMS_DEVICE_FLAG_IVT}, // 0x09
{241, DeviceType::CONTROLLER, "Condens 5000i", DeviceFlags::EMS_DEVICE_FLAG_NONE}, // 0x09

// Thermostat - not currently supporting write operations, like the Easy/100 types - 0x18
{202, DeviceType::THERMOSTAT, "Logamatic TC100, Moduline Easy", DeviceFlags::EMS_DEVICE_FLAG_EASY | DeviceFlags::EMS_DEVICE_FLAG_NO_WRITE}, // 0x18, cannot write
{203, DeviceType::THERMOSTAT, "EasyControl, CT200", DeviceFlags::EMS_DEVICE_FLAG_EASY | DeviceFlags::EMS_DEVICE_FLAG_NO_WRITE}, // 0x18, cannot write

// Thermostat - Buderus/Nefit/Bosch specific - 0x17 / 0x10 / 0x18 / 0x19-0x1B for hc2-4 / 0x38
{  4, DeviceType::THERMOSTAT, "UI800, BC400", DeviceFlags::EMS_DEVICE_FLAG_BC400}, // 0x10
{ 10, DeviceType::THERMOSTAT, "CR11", DeviceFlags::EMS_DEVICE_FLAG_CR11}, // 0x18
{ 65, DeviceType::THERMOSTAT, "RC10", DeviceFlags::EMS_DEVICE_FLAG_RC20_N},// 0x17
{ 67, DeviceType::THERMOSTAT, "RC30", DeviceFlags::EMS_DEVICE_FLAG_RC30_N},// 0x10 - based on RC35
{ 77, DeviceType::THERMOSTAT, "RC20, Moduline 300", DeviceFlags::EMS_DEVICE_FLAG_RC20},// 0x17
{ 78, DeviceType::THERMOSTAT, "Moduline 400", DeviceFlags::EMS_DEVICE_FLAG_RC30}, // 0x10
{ 79, DeviceType::THERMOSTAT, "RC10, Moduline 100", DeviceFlags::EMS_DEVICE_FLAG_RC10},// 0x17
{ 80, DeviceType::THERMOSTAT, "Moduline 200", DeviceFlags::EMS_DEVICE_FLAG_RC10}, // 0x17
{ 86, DeviceType::THERMOSTAT, "RC35", DeviceFlags::EMS_DEVICE_FLAG_RC35}, // 0x10
{ 90, DeviceType::THERMOSTAT, "RC10, Moduline 100", DeviceFlags::EMS_DEVICE_FLAG_RC20_N}, // 0x17
{ 93, DeviceType::THERMOSTAT, "RC20RF", DeviceFlags::EMS_DEVICE_FLAG_RC20}, // 0x19
{ 94, DeviceType::THERMOSTAT, "RFM20 Remote", DeviceFlags::EMS_DEVICE_FLAG_NONE}, // 0x18
{151, DeviceType::THERMOSTAT, "RC25", DeviceFlags::EMS_DEVICE_FLAG_RC25}, // 0x17
{157, DeviceType::THERMOSTAT, "RC200, CW100, CR120, CR50", DeviceFlags::EMS_DEVICE_FLAG_RC100}, // 0x18, CR120 v22 is FLAG_BC400
{158, DeviceType::THERMOSTAT, "RC3*0, Moduline 3000/1010H, CW400, Sense II, HPC410", DeviceFlags::EMS_DEVICE_FLAG_RC300}, // 0x10
{165, DeviceType::THERMOSTAT, "RC100, CR10, Moduline 1000/1010", DeviceFlags::EMS_DEVICE_FLAG_CR11}, // 0x18, 0x38
{172, DeviceType::THERMOSTAT, "Rego 2000/3000", DeviceFlags::EMS_DEVICE_FLAG_R3000}, // 0x10
{215, DeviceType::THERMOSTAT, "Comfort RF", DeviceFlags::EMS_DEVICE_FLAG_CRF | DeviceFlags::EMS_DEVICE_FLAG_NO_WRITE}, // 0x18
{216, DeviceType::THERMOSTAT, "CRF200S", DeviceFlags::EMS_DEVICE_FLAG_CRF | DeviceFlags::EMS_DEVICE_FLAG_NO_WRITE}, // 0x18
{246, DeviceType::THERMOSTAT, "Comfort+2RF", DeviceFlags::EMS_DEVICE_FLAG_CRF | DeviceFlags::EMS_DEVICE_FLAG_NO_WRITE}, // 0x18
{253, DeviceType::THERMOSTAT, "Rego 3000, UI800, Logamatic BC400", DeviceFlags::EMS_DEVICE_FLAG_BC400}, // 0x10

// Thermostat - Sieger - 0x10 / 0x17
{ 66, DeviceType::THERMOSTAT, "ES72, RC20", DeviceFlags::EMS_DEVICE_FLAG_RC20_N}, // 0x17 or remote
{ 76, DeviceType::THERMOSTAT, "ES73", DeviceFlags::EMS_DEVICE_FLAG_RC30_N}, // 0x10
{113, DeviceType::THERMOSTAT, "ES72, RC20", DeviceFlags::EMS_DEVICE_FLAG_RC20_N}, // 0x17
{156, DeviceType::THERMOSTAT, "ES79", DeviceFlags::EMS_DEVICE_FLAG_RC35}, // 0x10

// Thermostat - Junkers - 0x10
{105, DeviceType::THERMOSTAT, "FW100", DeviceFlags::EMS_DEVICE_FLAG_JUNKERS},
{106, DeviceType::THERMOSTAT, "FW200", DeviceFlags::EMS_DEVICE_FLAG_JUNKERS},
{107, DeviceType::THERMOSTAT, "FR100", DeviceFlags::EMS_DEVICE_FLAG_JUNKERS | DeviceFlags::EMS_DEVICE_FLAG_JUNKERS_OLD}, // older model
{108, DeviceType::THERMOSTAT, "FR110", DeviceFlags::EMS_DEVICE_FLAG_JUNKERS | DeviceFlags::EMS_DEVICE_FLAG_JUNKERS_OLD}, // older model
{109, DeviceType::THERMOSTAT, "FB10", DeviceFlags::EMS_DEVICE_FLAG_JUNKERS},
{110, DeviceType::THERMOSTAT, "FB100", DeviceFlags::EMS_DEVICE_FLAG_JUNKERS},
{111, DeviceType::THERMOSTAT, "FR10", DeviceFlags::EMS_DEVICE_FLAG_JUNKERS | DeviceFlags::EMS_DEVICE_FLAG_JUNKERS_OLD}, // older model
{116, DeviceType::THERMOSTAT, "FW500", DeviceFlags::EMS_DEVICE_FLAG_JUNKERS},
{147, DeviceType::THERMOSTAT, "FR50", DeviceFlags::EMS_DEVICE_FLAG_JUNKERS | DeviceFlags::EMS_DEVICE_FLAG_JUNKERS_OLD},
{191, DeviceType::THERMOSTAT, "FR120", DeviceFlags::EMS_DEVICE_FLAG_JUNKERS | DeviceFlags::EMS_DEVICE_FLAG_JUNKERS_OLD}, // older model
{192, DeviceType::THERMOSTAT, "FW120", DeviceFlags::EMS_DEVICE_FLAG_JUNKERS},

// Thermostat remote - 0x38
{  3, DeviceType::THERMOSTAT, "RT800, RC220", DeviceFlags::EMS_DEVICE_FLAG_RC100H},
{200, DeviceType::THERMOSTAT, "RC100H, CR10H", DeviceFlags::EMS_DEVICE_FLAG_RC100H},
{249, DeviceType::THERMOSTAT, "TR120RF, CR20RF", DeviceFlags::EMS_DEVICE_FLAG_RC100H},

// Solar Modules - 0x30 (for solar), 0x2A, 0x41 (for ww)
{ 73, DeviceType::SOLAR, "SM10", DeviceFlags::EMS_DEVICE_FLAG_SM10},
{101, DeviceType::SOLAR, "ISM1", DeviceFlags::EMS_DEVICE_FLAG_ISM},
{103, DeviceType::SOLAR, "ISM2", DeviceFlags::EMS_DEVICE_FLAG_ISM},
{162, DeviceType::SOLAR, "SM50", DeviceFlags::EMS_DEVICE_FLAG_SM100},
{163, DeviceType::SOLAR, "SM100, MS100", DeviceFlags::EMS_DEVICE_FLAG_SM100},
{164, DeviceType::SOLAR, "SM200, MS200", DeviceFlags::EMS_DEVICE_FLAG_SM100},

// Mixer Modules - 0x20-0x27 for HC, 0x28-0x29 for WWC and 0x11 for the MP100
{ 69, DeviceType::MIXER, "MM10", DeviceFlags::EMS_DEVICE_FLAG_MM10},
{100, DeviceType::MIXER, "IPM", DeviceFlags::EMS_DEVICE_FLAG_IPM},
{102, DeviceType::MIXER, "IPM2", DeviceFlags::EMS_DEVICE_FLAG_IPM},
{159, DeviceType::MIXER, "MM50", DeviceFlags::EMS_DEVICE_FLAG_MMPLUS},
{160, DeviceType::MIXER, "MM100", DeviceFlags::EMS_DEVICE_FLAG_MMPLUS},
{161, DeviceType::MIXER, "MM200", DeviceFlags::EMS_DEVICE_FLAG_MMPLUS},
{193, DeviceType::MIXER, "MZ100", DeviceFlags::EMS_DEVICE_FLAG_MMPLUS},
{204, DeviceType::POOL, "MP100", DeviceFlags::EMS_DEVICE_FLAG_MP}, // pool

// Heat Pumps - 0x38? This is a thermostat like RC100H
// also prod-id of wifi module and wireless base
{252, DeviceType::HEATPUMP, "HP Module", DeviceFlags::EMS_DEVICE_FLAG_NONE},

// Heat Pumps - 0x53
{248, DeviceType::HEATPUMP, "Hybrid Manager HM200", DeviceFlags::EMS_DEVICE_FLAG_NONE},

// Ventilation - 0x51
{231, DeviceType::VENTILATION, "Logavent HRV176", DeviceFlags::EMS_DEVICE_FLAG_NONE},

// Heatsource - 0x60
{228, DeviceType::HEATSOURCE, "AM200", DeviceFlags::EMS_DEVICE_FLAG_NONE}, // alternative heatsource

// Connect devices - 0x02
{171, DeviceType::CONNECT, "OpenTherm Converter", DeviceFlags::EMS_DEVICE_FLAG_NONE},
{205, DeviceType::CONNECT, "Moduline Easy Connect", DeviceFlags::EMS_DEVICE_FLAG_NONE},
{206, DeviceType::CONNECT, "Easy Connect", DeviceFlags::EMS_DEVICE_FLAG_NONE},

// Wireless sensor base - 0x50
{218, DeviceType::CONNECT, "M200, RFM200", DeviceFlags::EMS_DEVICE_FLAG_NONE},
{236, DeviceType::CONNECT, "Wireless sensor base", DeviceFlags::EMS_DEVICE_FLAG_NONE},
{238, DeviceType::CONNECT, "Wireless sensor base", DeviceFlags::EMS_DEVICE_FLAG_NONE},

// Switches - 0x11
{ 71, DeviceType::SWITCH, "WM10", DeviceFlags::EMS_DEVICE_FLAG_NONE},

// EM10/100 extension module, pump module - 0x15
{ 243, DeviceType::EXTENSION, "EM10, EM100", DeviceFlags::EMS_DEVICE_FLAG_NONE},

// Wireless outdoor sensor T1RF - 0x16
{ 220, DeviceType::EXTENSION, "T1RF", DeviceFlags::EMS_DEVICE_FLAG_NONE}, // 0x16

// EM10 error contact and analog flowtemp control- 0x12
{ 74, DeviceType::ALERT, "EM10", DeviceFlags::EMS_DEVICE_FLAG_NONE},

// Gateways - 0x48
{17, DeviceType::GATEWAY, "MX400", DeviceFlags::EMS_DEVICE_FLAG_NONE}, // 0x48 and 0x4B
{189, DeviceType::GATEWAY, "KM200, MB LAN 2", DeviceFlags::EMS_DEVICE_FLAG_NONE},
{252, DeviceType::GATEWAY, "K30RF, MX300", DeviceFlags::EMS_DEVICE_FLAG_NONE},

// Generic - 0x40 or other with no product-id and no version
{0, DeviceType::GENERIC, "unknown", DeviceFlags::EMS_DEVICE_FLAG_NONE}

#if defined(EMSESP_STANDALONE)
,
{100, DeviceType::WATER, "IPM", DeviceFlags::EMS_DEVICE_FLAG_IPM},
{102, DeviceType::WATER, "IPM2", DeviceFlags::EMS_DEVICE_FLAG_IPM},
{160, DeviceType::WATER, "MM100", DeviceFlags::EMS_DEVICE_FLAG_MMPLUS},
{161, DeviceType::WATER, "MM200", DeviceFlags::EMS_DEVICE_FLAG_MMPLUS},
{163, DeviceType::WATER, "SM100, MS100", DeviceFlags::EMS_DEVICE_FLAG_SM100},
{164, DeviceType::WATER, "SM200, MS200", DeviceFlags::EMS_DEVICE_FLAG_SM100},
{248, DeviceType::MIXER, "HM210", DeviceFlags::EMS_DEVICE_FLAG_MMPLUS}
// {157, DeviceType::THERMOSTAT, "RC120", DeviceFlags::EMS_DEVICE_FLAG_CR120}
#endif

// clang-format on
