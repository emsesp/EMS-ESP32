/*
 * EMS-ESP - https://github.com/proddy/EMS-ESP
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

// clang-format off

/*
 * These is the EMS devices that we currently recognize
 * The types and flags are stored in emsdevice.h 
 */

// Boilers - 0x08
{ 64, DeviceType::BOILER, F("BK13/BK15/Smartline/GB1x2"), DeviceFlags::EMS_DEVICE_FLAG_NONE},
{ 72, DeviceType::BOILER, F("GB125/MC10"), DeviceFlags::EMS_DEVICE_FLAG_NONE},
{ 84, DeviceType::BOILER, F("Logamax Plus GB022"), DeviceFlags::EMS_DEVICE_FLAG_NONE},
{ 95, DeviceType::BOILER, F("Condens 2500/Logamax/Logomatic/Cerapur Top/Greenstar/Generic HT3"), DeviceFlags::EMS_DEVICE_FLAG_NONE},
{115, DeviceType::BOILER, F("Topline/GB162"), DeviceFlags::EMS_DEVICE_FLAG_NONE},
{122, DeviceType::BOILER, F("Proline"), DeviceFlags::EMS_DEVICE_FLAG_NONE},
{123, DeviceType::BOILER, F("GBx72/Trendline/Cerapur/Greenstar Si/27i"), DeviceFlags::EMS_DEVICE_FLAG_NONE},
{131, DeviceType::BOILER, F("GB212"), DeviceFlags::EMS_DEVICE_FLAG_NONE},
{133, DeviceType::BOILER, F("GB125/Logamatic MC110"), DeviceFlags::EMS_DEVICE_FLAG_NONE},
{167, DeviceType::BOILER, F("Cerapur Aero"), DeviceFlags::EMS_DEVICE_FLAG_NONE},
{170, DeviceType::BOILER, F("Logano GB212"), DeviceFlags::EMS_DEVICE_FLAG_NONE},
{172, DeviceType::BOILER, F("Enviline/Compress 6000AW/Hybrid 7000iAW"), DeviceFlags::EMS_DEVICE_FLAG_NONE},
{195, DeviceType::BOILER, F("Condens 5000i/Greenstar 8000"), DeviceFlags::EMS_DEVICE_FLAG_NONE},
{203, DeviceType::BOILER, F("Logamax U122/Cerapur"), DeviceFlags::EMS_DEVICE_FLAG_NONE},
{208, DeviceType::BOILER, F("Logamax Plus/GB192/Condens GC9000"), DeviceFlags::EMS_DEVICE_FLAG_NONE},
{210, DeviceType::BOILER, F("Cascade MC400"), DeviceFlags::EMS_DEVICE_FLAG_NONE},
{211, DeviceType::BOILER, F("EasyControl Adapter"), DeviceFlags::EMS_DEVICE_FLAG_NONE},
{234, DeviceType::BOILER, F("Logamax Plus GB122"), DeviceFlags::EMS_DEVICE_FLAG_NONE},
{206, DeviceType::BOILER, F("Ecomline Excellent"), DeviceFlags::EMS_DEVICE_FLAG_NONE},

// Controllers - 0x09 / 0x10 / 0x50
{ 68, DeviceType::CONTROLLER, F("BC10/RFM20"), DeviceFlags::EMS_DEVICE_FLAG_NONE}, // 0x09
{ 84, DeviceType::CONTROLLER, F("GB022"), DeviceFlags::EMS_DEVICE_FLAG_NONE},
{ 89, DeviceType::CONTROLLER, F("BC10 GB142"), DeviceFlags::EMS_DEVICE_FLAG_NONE}, // 0x09
{ 95, DeviceType::CONTROLLER, F("HT3"), DeviceFlags::EMS_DEVICE_FLAG_NONE}, // 0x09
{114, DeviceType::CONTROLLER, F("BC10"), DeviceFlags::EMS_DEVICE_FLAG_NONE}, // 0x09
{125, DeviceType::CONTROLLER, F("BC25"), DeviceFlags::EMS_DEVICE_FLAG_NONE}, // 0x09
{152, DeviceType::CONTROLLER, F("Controller"), DeviceFlags::EMS_DEVICE_FLAG_NONE}, // 0x09
{169, DeviceType::CONTROLLER, F("BC40"), DeviceFlags::EMS_DEVICE_FLAG_NONE}, // 0x09
{190, DeviceType::CONTROLLER, F("BC10"), DeviceFlags::EMS_DEVICE_FLAG_NONE}, // 0x09
{194, DeviceType::CONTROLLER, F("BC10"), DeviceFlags::EMS_DEVICE_FLAG_NONE}, // 0x09
{206, DeviceType::CONTROLLER, F("Ecomline"), DeviceFlags::EMS_DEVICE_FLAG_NONE}, // 0x09
{207, DeviceType::CONTROLLER, F("Sense II/CS200"), DeviceFlags::EMS_DEVICE_FLAG_NONE}, // 0x10
{209, DeviceType::CONTROLLER, F("ErP"), DeviceFlags::EMS_DEVICE_FLAG_NONE}, // 0x09
{218, DeviceType::CONTROLLER, F("M200/RFM200"), DeviceFlags::EMS_DEVICE_FLAG_NONE}, // 0x50
{224, DeviceType::CONTROLLER, F("9000i"), DeviceFlags::EMS_DEVICE_FLAG_NONE}, // 0x09
{230, DeviceType::CONTROLLER, F("BC Base"), DeviceFlags::EMS_DEVICE_FLAG_NONE}, // 0x09
{241, DeviceType::CONTROLLER, F("Condens 5000i"), DeviceFlags::EMS_DEVICE_FLAG_NONE}, // 0x09

// Thermostat - not currently supporting write operations, like the Easy/100 types - 0x18
{202, DeviceType::THERMOSTAT, F("Logamatic TC100/Moduline Easy"), DeviceFlags::EMS_DEVICE_FLAG_EASY | DeviceFlags::EMS_DEVICE_FLAG_NO_WRITE}, // 0x18, cannot write
{203, DeviceType::THERMOSTAT, F("EasyControl CT200"), DeviceFlags::EMS_DEVICE_FLAG_EASY | DeviceFlags::EMS_DEVICE_FLAG_NO_WRITE}, // 0x18, cannot write

// Thermostat - Buderus/Nefit/Bosch specific - 0x17 / 0x10 / 0x18 / 0x19 / 0x38
{ 67, DeviceType::THERMOSTAT, F("RC30"), DeviceFlags::EMS_DEVICE_FLAG_RC30_1},// 0x10 - based on RC35
{ 77, DeviceType::THERMOSTAT, F("RC20/Moduline 300"), DeviceFlags::EMS_DEVICE_FLAG_RC20},// 0x17
{ 78, DeviceType::THERMOSTAT, F("Moduline 400"), DeviceFlags::EMS_DEVICE_FLAG_RC30}, // 0x10
{ 79, DeviceType::THERMOSTAT, F("RC10/Moduline 100"), DeviceFlags::EMS_DEVICE_FLAG_RC10},// 0x17
{ 80, DeviceType::THERMOSTAT, F("Moduline 200"), DeviceFlags::EMS_DEVICE_FLAG_RC10}, // 0x17
{ 86, DeviceType::THERMOSTAT, F("RC35"), DeviceFlags::EMS_DEVICE_FLAG_RC35}, // 0x10
{ 90, DeviceType::THERMOSTAT, F("RC10/Moduline 100"), DeviceFlags::EMS_DEVICE_FLAG_RC20_2}, // 0x17
{ 93, DeviceType::THERMOSTAT, F("RC20RF"), DeviceFlags::EMS_DEVICE_FLAG_RC20}, // 0x19
{ 94, DeviceType::THERMOSTAT, F("RFM20 Remote"), DeviceFlags::EMS_DEVICE_FLAG_NONE}, // 0x18
{157, DeviceType::THERMOSTAT, F("RC200/CW100"), DeviceFlags::EMS_DEVICE_FLAG_RC100}, // 0x18
{158, DeviceType::THERMOSTAT, F("RC300/RC310/Moduline 3000/1010H/CW400/Sense II"), DeviceFlags::EMS_DEVICE_FLAG_RC300}, // 0x10
{165, DeviceType::THERMOSTAT, F("RC100/Moduline 1000/1010"), DeviceFlags::EMS_DEVICE_FLAG_RC100}, // 0x18, 0x38

// Thermostat - Sieger - 0x10 / 0x17
{ 66, DeviceType::THERMOSTAT, F("ES72/RC20"), DeviceFlags::EMS_DEVICE_FLAG_RC20_2}, // 0x17 or remote
{ 76, DeviceType::THERMOSTAT, F("ES73"), DeviceFlags::EMS_DEVICE_FLAG_RC35}, // 0x10
{113, DeviceType::THERMOSTAT, F("ES72/RC20"), DeviceFlags::EMS_DEVICE_FLAG_RC20_2}, // 0x17

// Thermostat - Junkers - 0x10
{105, DeviceType::THERMOSTAT, F("FW100"), DeviceFlags::EMS_DEVICE_FLAG_JUNKERS},
{106, DeviceType::THERMOSTAT, F("FW200"), DeviceFlags::EMS_DEVICE_FLAG_JUNKERS},
{107, DeviceType::THERMOSTAT, F("FR100"), DeviceFlags::EMS_DEVICE_FLAG_JUNKERS | DeviceFlags::EMS_DEVICE_FLAG_JUNKERS_OLD}, // older model
{108, DeviceType::THERMOSTAT, F("FR110"), DeviceFlags::EMS_DEVICE_FLAG_JUNKERS | DeviceFlags::EMS_DEVICE_FLAG_JUNKERS_OLD}, // older model
{111, DeviceType::THERMOSTAT, F("FR10"), DeviceFlags::EMS_DEVICE_FLAG_JUNKERS},  
{147, DeviceType::THERMOSTAT, F("FR50"), DeviceFlags::EMS_DEVICE_FLAG_JUNKERS | DeviceFlags::EMS_DEVICE_FLAG_JUNKERS_OLD},
{191, DeviceType::THERMOSTAT, F("FR120"), DeviceFlags::EMS_DEVICE_FLAG_JUNKERS | DeviceFlags::EMS_DEVICE_FLAG_JUNKERS_OLD}, // older model
{192, DeviceType::THERMOSTAT, F("FW120"), DeviceFlags::EMS_DEVICE_FLAG_JUNKERS},

// Solar Modules - 0x30, 0x2A (for ww)
{ 73, DeviceType::SOLAR, F("SM10"), DeviceFlags::EMS_DEVICE_FLAG_SM10},
{101, DeviceType::SOLAR, F("ISM1"), DeviceFlags::EMS_DEVICE_FLAG_ISM},
{162, DeviceType::SOLAR, F("SM50"), DeviceFlags::EMS_DEVICE_FLAG_SM100},
{163, DeviceType::SOLAR, F("SM100/MS100"), DeviceFlags::EMS_DEVICE_FLAG_SM100},
{164, DeviceType::SOLAR, F("SM200/MS200"), DeviceFlags::EMS_DEVICE_FLAG_SM100},

// Mixer Modules - 0x20-0x27 for HC, 0x28-0x29 for WWC
{ 69, DeviceType::MIXER, F("MM10"), DeviceFlags::EMS_DEVICE_FLAG_MM10},
{102, DeviceType::MIXER, F("IPM"), DeviceFlags::EMS_DEVICE_FLAG_IPM},
{159, DeviceType::MIXER, F("MM50"), DeviceFlags::EMS_DEVICE_FLAG_MMPLUS},
{160, DeviceType::MIXER, F("MM100"), DeviceFlags::EMS_DEVICE_FLAG_MMPLUS},
{161, DeviceType::MIXER, F("MM200"), DeviceFlags::EMS_DEVICE_FLAG_MMPLUS},

// Heat Pumps - 0x38
{200, DeviceType::HEATPUMP, F("HP Module"), DeviceFlags::EMS_DEVICE_FLAG_NONE},
{252, DeviceType::HEATPUMP, F("HP Module"), DeviceFlags::EMS_DEVICE_FLAG_NONE},

// Connect devices - 0x02
{171, DeviceType::CONNECT, F("OpenTherm Converter"), DeviceFlags::EMS_DEVICE_FLAG_NONE},
{205, DeviceType::CONNECT, F("Moduline Easy Connect"), DeviceFlags::EMS_DEVICE_FLAG_NONE},
{206, DeviceType::CONNECT, F("Easy Connect"), DeviceFlags::EMS_DEVICE_FLAG_NONE},

// Switches - 0x11
{ 71, DeviceType::SWITCH, F("WM10"), DeviceFlags::EMS_DEVICE_FLAG_NONE},

// Gateways - 0x48
{189, DeviceType::GATEWAY, F("KM200/MB LAN 2"), DeviceFlags::EMS_DEVICE_FLAG_NONE}

// clang-format on
