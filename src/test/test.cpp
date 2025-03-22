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

#if defined(EMSESP_STANDALONE) || defined(EMSESP_TEST)

#include "test.h"

namespace emsesp {

// no shell, called via the API or 'call system test' command
// or http://ems-esp/api?device=system&cmd=test&data=boiler
bool Test::test(const std::string & cmd, int8_t id1, int8_t id2) {
    if (cmd.empty()) {
        return false;
    }

    if (cmd == "add") {
        Mqtt::entity_format(Mqtt::entityFormat::SINGLE_LONG); // SINGLE_LONG, SINGLE_SHORT, MULTI_SHORT
        System::test_set_all_active(true);                    // include all entities and give them fake values
        add_device(id1, id2);
        return true;
    }

    if (cmd == "memory") {
        EMSESP::logger().notice("Testing memory by adding lots of devices and entities...");

        System::test_set_all_active(true); // include all entities and give them fake values

        // simulate HansRemmerswaal's setup - see https://github.com/emsesp/EMS-ESP32/issues/859
        add_device(0x08, 172); // 176 entities - boiler: Enviline/Compress 6000AW/Hybrid 3000-7000iAW/SupraEco/Geo 5xx/WLW196i
        add_device(0x10, 158); // 62 entities - thermostat: RC300/RC310/Moduline 3000/1010H/CW400/Sense II/HPC410
        add_device(0x38, 200); // 4 entities - thermostat: RC100H

        return true;
    }

    if (cmd == "general") {
        EMSESP::logger().notice("Testing general. Adding a Boiler and Thermostat");

        // System::test_set_all_active(true); // uncomment if we want to show all entities and give them fake values

        add_device(0x08, 123); // Nefit Trendline
        add_device(0x18, 157); // RC200/CW100

        // add_device(0x10, 158); // RC300 - there's no data here

        // add some data
        // Boiler -> Me, UBAMonitorFast(0x18), telegram: 08 00 18 00 00 02 5A 73 3D 0A 10 65 40 02 1A 80 00 01 E1 01 76 0E 3D 48 00 C9 44 02 00 (#data=25)
        uart_telegram({0x08, 0x00, 0x18, 0x00, 0x00, 0x02, 0x5A, 0x73, 0x3D, 0x0A, 0x10, 0x65, 0x40, 0x02, 0x1A,
                       0x80, 0x00, 0x01, 0xE1, 0x01, 0x76, 0x0E, 0x3D, 0x48, 0x00, 0xC9, 0x44, 0x02, 0x00});

        // Boiler -> Thermostat, UBAParameterWW(0x33), telegram: 08 97 33 00 23 24 (#data=2)
        uart_telegram({0x08, 0x98, 0x33, 0x00, 0x23, 0x24});

        // Boiler -> Me, UBAParameterWW(0x33), telegram: 08 0B 33 00 08 FF 34 FB 00 28 00 00 46 00 FF FF 00 (#data=13)
        uart_telegram({0x08, 0x0B, 0x33, 0x00, 0x08, 0xFF, 0x34, 0xFB, 0x00, 0x28, 0x00, 0x00, 0x46, 0x00, 0xFF, 0xFF, 0x00});

        // Thermostat RCPLUSStatusMessage_HC1(0x01A5)
        uart_telegram({0x98, 0x00, 0xFF, 0x00, 0x01, 0xA5, 0x00, 0xCF, 0x21, 0x2E, 0x00, 0x00, 0x2E, 0x24,
                       0x03, 0x25, 0x03, 0x03, 0x01, 0x03, 0x25, 0x00, 0xC8, 0x00, 0x00, 0x11, 0x01, 0x03});

        return true;
    }


//
// the tests take a lot of memory when built for the ESP32
// so only including the full set in standalone, otherwise a limited selection of basic tests
//
#ifdef EMSESP_STANDALONE

    if (cmd == "heat_exchange") {
        EMSESP::logger().notice("Testing heating exchange...");

        add_device(0x08, 219); // Greenstar HIU/Logamax kompakt WS170

        // [emsesp] boiler(0x08) -W-> Me(0x0B), UBAMonitorFastPlus(0xE4), data: 00 01 35 00 00 00 00 00 00 00 00 80 00 00 00 00 00 00 00 80 00 (offset 6)
        uart_telegram({0x08, 0x00, 0xE4, 0x00, //
                       00,   01,   0x35, 00,   00, 00, 00, 00, 00, 00, 00, 0x80, 00, 00, 00, 00, 00, 00, 00, 0x80, 00});

        return true;
    }

    if (cmd == "2thermostats") {
        EMSESP::logger().notice("Testing with multiple thermostats...");

        add_device(0x08, 123); // GB072 boiler

        add_device(0x10, 158); // RC310
        add_device(0x19, 157); // RC200

        // Boiler -> Me, UBAMonitorFast(0x18), telegram: 08 00 18 00 00 02 5A 73 3D 0A 10 65 40 02 1A 80 00 01 E1 01 76 0E 3D 48 00 C9 44 02 00 (#data=25)
        uart_telegram({0x08, 0x00, 0x18, 0x00, 0x00, 0x02, 0x5A, 0x73, 0x3D, 0x0A, 0x10, 0x65, 0x40, 0x02, 0x1A,
                       0x80, 0x00, 0x01, 0xE1, 0x01, 0x76, 0x0E, 0x3D, 0x48, 0x00, 0xC9, 0x44, 0x02, 0x00});

        // Boiler -> Thermostat, UBAParameterWW(0x33), telegram: 08 97 33 00 23 24 (#data=2)
        uart_telegram({0x08, 0x90, 0x33, 0x00, 0x23, 0x24});

        // Boiler -> Me, UBAParameterWW(0x33), telegram: 08 0B 33 00 08 FF 34 FB 00 28 00 00 46 00 FF FF 00 (#data=13)
        uart_telegram({0x08, 0x0B, 0x33, 0x00, 0x08, 0xFF, 0x34, 0xFB, 0x00, 0x28, 0x00, 0x00, 0x46, 0x00, 0xFF, 0xFF, 0x00});

        // Thermostat 0x2A5 for HC1
        uart_telegram({0x10, 00, 0xFF, 00, 01,   0xA5, 0x80, 00, 01, 0x30, 0x28, 00, 0x30, 0x28, 01, 0x54,
                       03,   03, 01,   01, 0x54, 02,   0xA8, 00, 00, 0x11, 01,   03, 0xFF, 0xFF, 00});

        // RC300WWmode2(0x31D), data: 00 00 09 07
        uart_telegram({0x10, 00, 0xFF, 00, 02, 0x1D, 00, 00, 0x09, 0x07});

        // 2nd thermostat on HC2
        // Thermostat RC300Monitor(0x02A6)
        uart_telegram({0x99, 0x00, 0xFF, 0x00, 0x01, 0xA6, 0x00, 0xCF, 0x21, 0x2E, 0x00, 0x00, 0x2E, 0x24,
                       0x03, 0x25, 0x03, 0x03, 0x01, 0x03, 0x25, 0x00, 0xC8, 0x00, 0x00, 0x11, 0x01, 0x03});

        return true;
    }

    if (cmd == "310") {
        EMSESP::logger().notice("Adding a GB072/RC310 combo...");

        add_device(0x08, 123); // GB072
        add_device(0x10, 158); // RC310

        // add some data
        // Boiler -> Me, UBAMonitorFast(0x18), telegram: 08 00 18 00 00 02 5A 73 3D 0A 10 65 40 02 1A 80 00 01 E1 01 76 0E 3D 48 00 C9 44 02 00 (#data=25)
        uart_telegram({0x08, 0x00, 0x18, 0x00, 0x00, 0x02, 0x5A, 0x73, 0x3D, 0x0A, 0x10, 0x65, 0x40, 0x02, 0x1A,
                       0x80, 0x00, 0x01, 0xE1, 0x01, 0x76, 0x0E, 0x3D, 0x48, 0x00, 0xC9, 0x44, 0x02, 0x00});

        // Boiler -> Thermostat, UBAParameterWW(0x33), telegram: 08 97 33 00 23 24 (#data=2)
        uart_telegram({0x08, 0x90, 0x33, 0x00, 0x23, 0x24});

        // Boiler -> Me, UBAParameterWW(0x33), telegram: 08 0B 33 00 08 FF 34 FB 00 28 00 00 46 00 FF FF 00 (#data=13)
        uart_telegram({0x08, 0x0B, 0x33, 0x00, 0x08, 0xFF, 0x34, 0xFB, 0x00, 0x28, 0x00, 0x00, 0x46, 0x00, 0xFF, 0xFF, 0x00});

        // Thermostat 0x2A5 for HC1
        uart_telegram({0x10, 00, 0xFF, 00, 01,   0xA5, 0x80, 00, 01, 0x30, 0x28, 00, 0x30, 0x28, 01, 0x54,
                       03,   03, 01,   01, 0x54, 02,   0xA8, 00, 00, 0x11, 01,   03, 0xFF, 0xFF, 00});

        // RC300WWmode2(0x31D), data: 00 00 09 07
        uart_telegram({0x10, 00, 0xFF, 00, 02, 0x1D, 00, 00, 0x09, 0x07});

        return true;
    }

    if (cmd == "gateway") {
        EMSESP::logger().notice("Adding a Gateway...");

        // add 0x48 KM200, via a version command
        rx_telegram({0x48, 0x0B, 0x02, 0x00, 0xBD, 0x04, 0x06, 00, 00, 00, 00, 00, 00, 00});

        // Boiler(0x08) -> All(0x00), UBADevices(0x07), data: 09 01 00 00 00 00 00 00 01 00 00 00 00
        // check: make sure 0x48 is not detected again !
        rx_telegram({0x08, 0x00, 0x07, 0x00, 0x09, 01, 00, 00, 00, 00, 00, 00, 01, 00, 00, 00, 00});

        // add thermostat - Thermostat: RC300/RC310/Moduline 3000/CW400/Sense II (DeviceID:0x10, ProductID:158, Version:03.03) ** master device **
        add_device(0x10, 158); // RC310

        // simulate incoming telegram
        // Thermostat(0x10) -> 48(0x48), ?(0x26B), data: 6B 08 4F 00 00 00 02 00 00 00 02 00 03 00 03 00 03
        rx_telegram({0x10, 0x48, 0xFF, 00, 01, 0x6B, 00, 0x6B, 0x08, 0x4F, 00, 00, 00, 02, 00, 00, 00, 02, 00, 03, 00, 03, 00, 03});

        return true;
    }

    if (cmd == "mixer") {
        EMSESP::logger().notice("Adding a mixer...");

        // add controller
        add_device(0x09, 114);

        add_device(0x28, 160); // MM100
        add_device(0x29, 161); // MM200
        add_device(0x20, 160); // MM100

        // WWC1 on 0x29
        uart_telegram({0xA9, 0x00, 0xFF, 0x00, 0x02, 0x32, 0x02, 0x6C, 0x00, 0x3C, 0x00, 0x3C, 0x3C, 0x46, 0x02, 0x03, 0x03, 0x00, 0x3C});

        // WWC2 on 0x28
        uart_telegram({0xA8, 0x00, 0xFF, 0x00, 0x02, 0x31, 0x02, 0x35, 0x00, 0x3C, 0x00, 0x3C, 0x3C, 0x46, 0x02, 0x03, 0x03, 0x00, 0x3C});

        // HC1 on 0x20
        uart_telegram({0xA0, 00, 0xFF, 00, 01, 0xD7, 00, 00, 00, 0x80, 00, 00, 00, 00, 03, 0xC5});

        return true;
    }

    if (cmd == "boiler") {
        EMSESP::logger().notice("Adding boiler...");
        add_device(0x08, 123); // Nefit Trendline

        // UBAuptime
        uart_telegram({0x08, 0x0B, 0x14, 00, 0x3C, 0x1F, 0xAC, 0x70});

        // Boiler -> Me, UBAMonitorFast(0x18), telegram: 08 00 18 00 00 02 5A 73 3D 0A 10 65 40 02 1A 80 00 01 E1 01 76 0E 3D 48 00 C9 44 02 00 (#data=25)
        uart_telegram({0x08, 0x00, 0x18, 0x00, 0x00, 0x02, 0x5A, 0x73, 0x3D, 0x0A, 0x10, 0x65, 0x40, 0x02, 0x1A,
                       0x80, 0x00, 0x01, 0xE1, 0x01, 0x76, 0x0E, 0x3D, 0x48, 0x00, 0xC9, 0x44, 0x02, 0x00});

        // Boiler -> Me, UBAParameterWW(0x33), telegram: 08 0B 33 00 08 FF 34 FB 00 28 00 00 46 00 FF FF 00 (#data=13)
        uart_telegram({0x08, 0x0B, 0x33, 0x00, 0x08, 0xFF, 0x34, 0xFB, 0x00, 0x28, 0x00, 0x00, 0x46, 0x00, 0xFF, 0xFF, 0x00});

        return true;
    }

    if (cmd == "thermostat") {
        EMSESP::logger().notice("Adding thermostat...");

        add_device(0x10, 192); // FW120

        // HC1 - 3
        uart_telegram({0x90, 0x00, 0xFF, 0x00, 0x00, 0x6F, 0x03, 0x02, 0x00, 0xCD, 0x00, 0xE4});
        uart_telegram({0x90, 0x00, 0xFF, 0x00, 0x00, 0x70, 0x02, 0x01, 0x00, 0xCE, 0x00, 0xE5});
        uart_telegram({0x90, 0x00, 0xFF, 0x00, 0x00, 0x71, 0x01, 0x02, 0x00, 0xCF, 0x00, 0xE6});

        return true;
    }

    if (cmd == "solar") {
        EMSESP::logger().notice("Adding solar...");

        add_device(0x30, 163); // SM100

        // SM100Monitor - type 0x0362 EMS+ - for SM100 and SM200
        uart_telegram({0xB0, 0x0B, 0xFF, 00, 0x02, 0x62, 00, 0x44, 0x02, 0x7A, 0x80, 00, 0x80, 0x00, 0x80, 00,
                       0x80, 00,   0x80, 00, 0x80, 00,   00, 0x7C, 0x80, 00,   0x80, 00, 0x80, 00,   0x80});

        uart_telegram({0xB0, 0x0B, 0xFF, 0x00, 0x02, 0x62, 0x01, 0x44, 0x03, 0x30, 0x80, 00, 0x80, 00, 0x80, 00,
                       0x80, 00,   0x80, 00,   0x80, 00,   0x80, 00,   0x80, 00,   0x80, 00, 0x80, 00, 0x80, 0x33});

        uart_telegram({0xB0, 00, 0xFF, 0x18, 02, 0x62, 0x80, 00, 0xB8});

        uart_telegram("30 00 FF 00 02 64 00 00 00 04 00 00 FF 00 00 1E 0B 09 64 00 00 00 00"); // SM100 modulation

        return true;
    }

    if (cmd == "heatpump") {
        EMSESP::logger().notice("Adding heatpump...");

        add_device(0x38, 200); // Enviline module
        add_device(0x10, 192); // FW120 thermostat

        uart_telegram({0x90, 0x00, 0xFF, 0x00, 0x00, 0x6F, 0x03, 0x02, 0x00, 0xCD, 0x00, 0xE4});

        uart_telegram("38 0B FF 00 03 7B 0C 34 00 74");

        return true;
    }

#endif

    return false;
}

// These next tests are run from the Console via the test command, so inherit the Shell
void Test::run_test(uuid::console::Shell & shell, const std::string & cmd, const std::string & id1_s, const std::string & id2_s) {
    bool ok = false; // default tests fail

    shell.add_flags(CommandFlags::ADMIN); // switch to su

    // init stuff
    Mqtt::ha_enabled(true);
    Mqtt::entity_format(Mqtt::entityFormat::SINGLE_SHORT); // SINGLE_LONG, SINGLE_SHORT, MULTI_SHORT

    EMSESP::rxservice_.ems_mask(EMSbus::EMS_MASK_BUDERUS);

    // EMSESP::watch(EMSESP::Watch::WATCH_RAW); // raw mode
    EMSESP::watch(EMSESP::Watch::WATCH_ON); // verbose mode

    std::string command(20, '\0');
    if ((cmd.empty()) || (cmd == "default")) {
        command = EMSESP_DEBUG_DEFAULT;
    } else {
        command = cmd;
    }

    // extract params
    int16_t id1 = -1;
    int16_t id2 = -1;
    if (!id1_s.empty()) {
        if (id1_s[0] == '0' && id1_s[1] == 'x') {
            id1 = Helpers::hextoint(id1_s.c_str());
        } else {
            id1 = Helpers::atoint(id1_s.c_str());
        }
    }
    if (!id2_s.empty()) {
        id2 = Helpers::atoint(id2_s.c_str());
    }

    // e.g. "test add 0x10 172"
    if (command == "add") {
        if (id1 == -1 || id2 == -1) {
            shell.printfln("Usage: test add <device_id> <product_id>");
            return;
        }
        shell.printfln("Testing adding a device (deviceID 0x%02X, product_id %d), with all values...", id1, id2);
        test("add", id1, id2);
        shell.invoke_command("show devices");
        ok = true;
    }

    // set the language
    if (command == "locale") {
        shell.printfln("Testing setting locale to %s", id1_s.c_str());
        EMSESP::system_.locale(id1_s.c_str());
        shell.invoke_command("show");
        ok = true;
    }

    if (command == "general") {
        shell.printfln("Testing adding a boiler, thermostat, all sensors, scheduler and custom entities...");

        // add devices
        test("general");

        EMSESP::webCustomEntityService.test();  // custom entities
        EMSESP::webCustomizationService.test(); // set customizations - this will overwrite any settings in the FS
        EMSESP::temperaturesensor_.test();      // add temperature sensors
        EMSESP::webSchedulerService.test();     // run scheduler tests, and conditions

        shell.invoke_command("show values");
        ok = true;
    }

    // https://github.com/emsesp/EMS-ESP32/issues/869
    if (command == "memory") {
        shell.printfln("Testing memory by adding lots of devices and entities...");
        test("memory");
        shell.invoke_command("show values");
        ok = true;
    }

    if (command == "custom") {
        shell.printfln("Adding custom entities...");

        // add some dummy entities
        EMSESP::webCustomEntityService.test();

#ifdef EMSESP_STANDALONE
        AsyncWebServerRequest request;
        request.method(HTTP_GET);
        request.url("/api/custom");
        EMSESP::webAPIService.webAPIService(&request);
        request.url("/api/custom/test_custom");
        EMSESP::webAPIService.webAPIService(&request);
        request.url("/api/custom/test_read_only");
        EMSESP::webAPIService.webAPIService(&request);
        request.url("/api/custom/test_ram");
        EMSESP::webAPIService.webAPIService(&request);
        shell.invoke_command("call custom info");

#endif
        ok = true;
    }

    if (command == "scheduler") {
        shell.printfln("Adding Scheduler items...");

        // add some dummy entities
        EMSESP::webSchedulerService.test();

#ifdef EMSESP_STANDALONE
        AsyncWebServerRequest request;
        request.method(HTTP_GET);
        request.url("/api/scheduler");
        EMSESP::webAPIService.webAPIService(&request);
        shell.invoke_command("call scheduler info");
#endif
        ok = true;
    }

    if (command == "coldshot") {
        shell.printfln("Testing coldshot...");
        test("general");

#ifdef EMSESP_STANDALONE
        AsyncWebServerRequest request;
        request.method(HTTP_GET);
        request.url("/api/boiler/coldshot");
        EMSESP::webAPIService.webAPIService(&request);
#else
        shell.invoke_command("call boiler coldshot");
#endif
        ok = true;
    }

    if (command == "string2minutes") {
        shell.printfln("Testing string2minutes()...");
        std::string time_s = "12:00";
        shell.printfln("Testing %s is %d", time_s.c_str(), Helpers::string2minutes(time_s));
        std::string time_s2 = "12:12";
        shell.printfln("Testing %s is %d", time_s2.c_str(), Helpers::string2minutes(time_s2));
        std::string time_s3 = "00:50";
        shell.printfln("Testing %s is %d", time_s3.c_str(), Helpers::string2minutes(time_s3));
        std::string time_s4 = "03:49";
        shell.printfln("Testing %s is %d", time_s4.c_str(), Helpers::string2minutes(time_s4));
        ok = true;
    }

// THESE ONLY WORK WITH AN ESP32, not in standalone/native mode
#ifndef EMSESP_STANDALONE
    if (command == "ls") {
        listDir(LittleFS, "/", 3);
        ok = true;
    }

    if (command == "upload") {
        // S3 has 16MB flash
        // EMSESP::system_.uploadFirmwareURL("https://github.com/emsesp/EMS-ESP32/releases/download/latest/EMS-ESP-3_7_0-dev_32-ESP32S3-16MB+.bin"); // S3

        // Test for 4MB Tasmota builds
        // EMSESP::system_.uploadFirmwareURL("https://github.com/emsesp/EMS-ESP32/releases/download/latest/EMS-ESP-3_7_2-dev_8-ESP32-4MB.bin"); // S32
        EMSESP::system_.uploadFirmwareURL("https://github.com/emsesp/EMS-ESP32/releases/download/latest/EMS-ESP-3_7_2-dev_8-ESP32-16MB.bin"); // E32

        ok = true;
    }
#endif

    //
    // the tests take a lot of memory when built for the ESP32
    // so only including the full set in standalone, otherwise a limited selection of basic tests
    //

#ifdef EMSESP_STANDALONE
    // all tests with EMSESP_STANDALONE

    if (command == "entity_dump") {
        System::test_set_all_active(true);
        EMSESP::dump_all_entities(shell);
        ok = true;
    }

    if (command == "telegram_dump") {
        System::test_set_all_active(true);
        EMSESP::dump_all_telegrams(shell);
        ok = true;
    }

    if (command == "modes") {
        shell.printfln("Testing thermostat modes...");
        test("general");
        shell.invoke_command("call thermostat mode auto");
        shell.invoke_command("call thermostat mode Manuell"); // DE
        shell.invoke_command("call thermostat mode 1");
        ok = true;
    }

    if (command == "render") {
        shell.printfln("Testing render...");

        // check read_value to make sure it handles all the data type correctly
        uint8_t message_data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9}; // message_length is 9
        auto    telegram       = std::make_shared<Telegram>(Telegram::Operation::RX, 0x10, 0x11, 0x1234, 0, message_data, sizeof(message_data));

        uint8_t uint8b = EMS_VALUE_UINT8_NOTSET;
        telegram->read_value(uint8b, 0);
        shell.printfln("uint8: expecting %02X, got:%02X", 1, uint8b);

        int8_t int8b = EMS_VALUE_INT8_NOTSET;
        telegram->read_value(int8b, 0);
        shell.printfln("int8: expecting %02X, got:%02X", 1, int8b);

        uint16_t uint16b = EMS_VALUE_UINT16_NOTSET;
        telegram->read_value(uint16b, 1);
        shell.printfln("uint16: expecting %02X, got:%02X", 0x0203, uint16b);

        int16_t int16b = EMS_VALUE_INT16_NOTSET;
        telegram->read_value(int16b, 1);
        shell.printfln("int16: expecting %02X, got:%02X", 0x0203, int16b);

        int16_t int16b8 = EMS_VALUE_INT16_NOTSET;
        telegram->read_value(int16b8, 1, 1); // force to 1 byte
        shell.printfln("int16 1 byte: expecting %02X, got:%02X", 0x02, int16b8);

        uint32_t uint32b = EMS_VALUE_UINT24_NOTSET;
        telegram->read_value(uint32b, 1, 3);
        shell.printfln("uint32 3 bytes: expecting %02X, got:%02X", 0x020304, uint32b);

        uint32b = EMS_VALUE_UINT24_NOTSET;
        telegram->read_value(uint32b, 1);
        shell.printfln("uint32 4 bytes: expecting %02X, got:%02X", 0x02030405, uint32b);

        // check out of bounds
        uint16_t uint16 = EMS_VALUE_UINT16_NOTSET;
        telegram->read_value(uint16, 9);
        shell.printfln("uint16 out-of-bounds: was:%02X, new:%02X", EMS_VALUE_UINT16_NOTSET, uint16);
        uint8_t uint8oob = EMS_VALUE_UINT8_NOTSET;
        telegram->read_value(uint8oob, 9);
        shell.printfln("uint8 out-of-bounds: was:%02X, new:%02X", EMS_VALUE_UINT8_NOTSET, uint8oob);

        // check read bit
        uint8_t uint8bitb = EMS_VALUE_UINT8_NOTSET;
        telegram->read_bitvalue(uint8bitb, 1, 1); // value is 0x02 = 0000 0010
        shell.printfln("uint8 bit read: expecting 1, got:%d", uint8bitb);
        uint8bitb = EMS_VALUE_UINT8_NOTSET;
        telegram->read_bitvalue(uint8bitb, 0, 0); // value is 0x01 = 0000 0001
        shell.printfln("uint8 bit read: expecting 1, got:%d", uint8bitb);

        float test_float = 20.56;
        char  result[100];
        Helpers::render_value(result, test_float, 2);
        shell.printfln("Float test from %f to %s", test_float, result);

        uint16_t temp;
        float    doub;
        temp = 0x0201;                               // decimal 513
        doub = Helpers::transformNumFloat(temp, 10); // divide by 10
        shell.printfln("Round test from x%02X to %d to %f", temp, temp, doub);
        doub = Helpers::transformNumFloat(temp, 10); // divide by 10
        shell.printfln("Round test div10 from x%02X to %d to %f", temp, temp, doub);
        temp = 0x63;
        doub = Helpers::transformNumFloat(temp, 2); // divide by 2
        shell.printfln("Round test div2 from x%02X to %d to %f", temp, temp, doub);
        ok = true;
    }

    if (command == "devices") {
        shell.printfln("Testing devices...");

        // A fake response - UBADevices(0x07)
        rx_telegram({0x08, 0x00, 0x07, 0x00, 0x0B, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00});
        ok = true;
    }

    // check for boiler and controller on same product_id
    if (command == "double") {
        shell.printfln("Testing double...");

        add_device(0x08, 206); // Nefit Excellent HR30
        add_device(0x09, 206); // Nefit Excellent HR30 Controller

        // UBAuptime
        uart_telegram({0x08, 0x0B, 0x14, 00, 0x3C, 0x1F, 0xAC, 0x70});
        ok = true;
    }

    if (command == "620") {
        EMSESP::logger().notice("Testing 620...");

        // Version Controller
        uart_telegram({0x09, 0x0B, 0x02, 0x00, 0x5F, 0x06, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00});

        // Version Boiler
        uart_telegram({0x08, 0x0B, 0x02, 0x00, 0x5F, 0x06, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00});
        ok = true;
    }

    // unknown device
    if (command == "unknown") {
        shell.printfln("Testing unknown...");

        // add boiler
        add_device(0x08, 84);

        // add Controller - BC10 GB142 - but using the same product_id to see what happens
        add_device(0x09, 84);

        // simulate getting version information back from an unknown device
        // note there is no brand (byte 9)
        rx_telegram({0x09, 0x0B, 0x02, 0x00, 0x59, 0x09, 0x0a});

        shell.invoke_command("show devices");
        shell.invoke_command("call system report");
        ok = true;
    }

    if (command == "unknown2") {
        shell.printfln("Testing unknown2...");

        // simulate getting version information back from an unknown device
        rx_telegram({0x09, 0x0B, 0x02, 0x00, 0x5A, 0x01, 0x02}); // productID is 90 which doesn't exist
        ok = true;
    }

    if (command == "gateway") {
        shell.printfln("Testing Gateway...");
        test("gateway");
        ok = true;
    }

    if (command == "310") {
        shell.printfln("Testing RC310...");
        test("310");
        shell.invoke_command("show devices");
        shell.invoke_command("show values");
        shell.invoke_command("call system publish");
        shell.invoke_command("show mqtt");
        ok = true;
    }

    if (command == "heat_exchange") {
        shell.printfln("Testing heat exchange...");
        test("heat_exchange");
        shell.invoke_command("show devices");
        shell.invoke_command("show values");
        ok = true;
    }

    if (command == "2thermostats") {
        shell.printfln("Testing multiple thermostats...");
        // adds hc1=(0x10, 158) RC310 and hc2=(0x19, 157) RC200
        test("2thermostats");
        shell.invoke_command("show values");
        // shell.invoke_command("show devices");

        AsyncWebServerRequest request;
        request.method(HTTP_GET);

        request.url("/api/thermostat");
        EMSESP::webAPIService.webAPIService(&request);
        Serial.println();

        request.url("/api/thermostat/hc1/entities");
        EMSESP::webAPIService.webAPIService(&request);
        Serial.println();


        request.url("/api/thermostat/hc2/entities");
        EMSESP::webAPIService.webAPIService(&request);
        Serial.println();

        request.url("/api/thermostat/entities");
        EMSESP::webAPIService.webAPIService(&request);

        ok = true;
    }

    if (command == "web") {
        shell.printfln("Testing Web...");

        Mqtt::enabled(false);    // turn off mqtt
        Mqtt::ha_enabled(false); // turn off ha

        test("boiler");
        test("thermostat");

        JsonDocument doc;
        for (const auto & emsdevice : EMSESP::emsdevices) {
            if (emsdevice) {
                doc.clear();
                JsonObject json = doc.to<JsonObject>();
                Command::call(emsdevice->device_type(), "info", nullptr, true, -1, json);

                Serial.print(COLOR_YELLOW);
                if (json.size() != 0) {
                    serializeJson(doc, Serial);
                }
                shell.println();
                Serial.print(COLOR_RESET);

                doc.clear();
                // JsonObject root = doc.to<JsonObject>();
                // emsdevice->generate_values_web(root);

                JsonArray output = doc.to<JsonArray>();
                emsdevice->generate_values_web_customization(output);

                Serial.print(COLOR_BRIGHT_MAGENTA);
                serializeJson(doc, Serial);
                Serial.print(COLOR_RESET);
                // Serial.println();
                // Serial.print(" measureMsgPack=");
                // Serial.print(measureMsgPack(doc));
                // Serial.print(" measureJson=");
                // Serial.print(measureJson(doc));
                Serial.println(" **");
            }
        }
        ok = true;
    }

    if (command == "board_profile") {
        shell.printfln("Testing board profile...");

        shell.invoke_command("system");
        shell.invoke_command("set board_profile wemos");
        shell.invoke_command("exit");
        shell.invoke_command("call system settings");
        ok = true;
    }

    if (command == "boiler") {
        shell.printfln("Testing boiler...");
        // Mqtt::ha_enabled(false);
        Mqtt::ha_enabled(true);
        Mqtt::nested_format(1);

        test("boiler");
        shell.invoke_command("show devices");
        shell.invoke_command("show values");
        shell.invoke_command("call boiler info");
        shell.invoke_command("call system publish");

        // test all permutations
        EMSESP::mqtt_.incoming("ems-esp/boiler/wwonetime", "1");
        EMSESP::mqtt_.incoming("ems-esp/boiler/wwonetime", "0");
        EMSESP::mqtt_.incoming("ems-esp/boiler/wwonetime", "on");
        EMSESP::mqtt_.incoming("ems-esp/boiler/heatingtemp", "24");
        EMSESP::mqtt_.incoming("ems-esp/boiler/wwonetime", "test"); // should fail
        EMSESP::mqtt_.incoming("ems-esp/boiler", "{\"cmd\":\"flowtemp\",\"id\":0,\"data\":22}");
        EMSESP::mqtt_.incoming("ems-esp/boiler", "{\"cmd\":\"wwonetime\",\"id\":0,\"data\":1}");
        EMSESP::mqtt_.incoming("ems-esp/boiler", "{\"cmd\":\"wwonetime\",\"id\":0,\"data\":\"off\"}");
        EMSESP::mqtt_.incoming("ems-esp/boiler", "{\"cmd\":\"wwonetime\",\"hc\":1,\"data\":\"on\"}");
        EMSESP::mqtt_.incoming("ems-esp/boiler", "{\"cmd\":\"wwonetime\",\"data\":\"on\",\"hc\":1}");
        EMSESP::mqtt_.incoming("ems-esp/boiler", "{\"cmd\":\"heatingactivated\",\"data\":1}");

        shell.invoke_command("show mqtt");
        ok = true;
    }

    if (command == "shower_alert") {
        shell.printfln("Testing Shower Alert...");

        test("boiler");

        // device type, command, data
        Command::call(EMSdevice::DeviceType::BOILER, "tapactivated", "false", 9);
        ok = true;
    }

    if (command == "fr120") {
        shell.printfln("Testing adding a thermostat FR120...");

        add_device(0x10, 191); // FR120 thermostat

        // HC1
        uart_telegram({0x90, 0x00, 0xFF, 0x00, 0x00, 0x6F, 0x01, 0x02, 0x00, 0xCF, 0x00, 0xE6});

        shell.invoke_command("show values");
        shell.invoke_command("show devices");
        ok = true;
    }

    if (command == "ha") {
        shell.printfln("Testing HA mqtt discovery");
        Mqtt::ha_enabled(true);
        // Mqtt::ha_enabled(false);

        Mqtt::nested_format(1); // is nested
        // Mqtt::nested_format(2); // not nested

        test("boiler");
        test("thermostat");
        test("solar");
        test("mixer");

        shell.invoke_command("call system publish");
        shell.invoke_command("show mqtt");

        // shell.invoke_command("call boiler fanwork");
        // shell.invoke_command("call thermostat seltemp"); // sensor.thermostat_hc1_selected_room_temperature
        // shell.invoke_command("call thermostat entities");
        // shell.invoke_command("call boiler entities");
        ok = true;
    }

    if (command == "lastcode") {
        shell.printfln("Testing lastcode");

        Mqtt::ha_enabled(false);
        Mqtt::nested_format(1);
        // Mqtt::send_response(false);

        test("boiler");
        // test("thermostat");

        // 0xC2
        //  [emsesp] Boiler(0x08) -> Me(0x0B), UBAErrorMessage3(0xC2), data: 08 AC 00 10 31 48 30 31 15 80 95 0B 0E 10 38 00 7F FF FF FF 08 AC 00 10 09 41 30
        uart_telegram(
            {0x08, 0x0B, 0xC2, 0, 0x08, 0xAC, 00, 0x10, 0x31, 0x48, 0x30, 0x31, 0x15, 0x80, 0x95, 0x0B, 0x0E, 0x10, 0x38, 00, 0x7F, 0xFF, 0xFF, 0xFF});

        // shell.invoke_command("show values");

        ok = true;
    }

    if (command == "dv") {
        shell.printfln("Testing device value rendering");

        Mqtt::ha_enabled(true);
        // Mqtt::ha_enabled(false);

        Mqtt::nested_format(1);
        // Mqtt::nested_format(0);

        // Mqtt::send_response(false);

        test("boiler");
        test("thermostat");

        shell.invoke_command("call boiler wwseltemp");
        shell.invoke_command("call system publish");
        ok = true;
    }

    if (command == "temperature") {
        shell.printfln("Testing adding Temperature sensor");
        shell.invoke_command("show commands");

        // load some EMS data
        // test("general");

        emsesp::EMSESP::temperaturesensor_.test();

        shell.invoke_command("call temperaturesensor");
        shell.invoke_command("show values");
        shell.invoke_command("call temperaturesensor info");
        shell.invoke_command("call temperaturesensor values");

        AsyncWebServerRequest request;
        request.method(HTTP_GET);
        request.url("/api/temperaturesensor/commands");
        EMSESP::webAPIService.webAPIService(&request);
        request.url("/api/temperaturesensor/info");
        EMSESP::webAPIService.webAPIService(&request);
        request.url("/api/temperaturesensor/01_0203_0405_0607");
        EMSESP::webAPIService.webAPIService(&request);

        ok = true;
    }

    if (command == "temperature_full") {
        shell.printfln("Testing adding and changing Temperature sensor");
        Mqtt::ha_enabled(true);
        Mqtt::nested_format(1);
        // Mqtt::nested_format(0);

        emsesp::EMSESP::temperaturesensor_.test();
        shell.invoke_command("show values");
        shell.invoke_command("call system publish");

        // rename
        EMSESP::temperaturesensor_.update("01_0203_0405_0607", "testtemperature", 2);
        shell.invoke_command("show values");
        shell.invoke_command("call system publish");
        ok = true;
    }

    if (command == "analog") {
        shell.printfln("Testing adding Analog sensor");
        Mqtt::ha_enabled(true);
        // Mqtt::ha_enabled(false);
        Mqtt::nested_format(1);
        // Mqtt::nested_format(0);
        // Mqtt::send_response(false);

        // load some EMS data
        test("general");

        EMSESP::webCustomizationService.test(); // load the analog sensors

        shell.invoke_command("call analogsensor");
        shell.invoke_command("show values");
        shell.invoke_command("call analogsensor info");
        shell.invoke_command("call analogsensor values");

        AsyncWebServerRequest request;
        request.method(HTTP_GET);
        request.url("/api/analogsensor/commands");
        EMSESP::webAPIService.webAPIService(&request);
        request.url("/api/analogsensor/info");
        EMSESP::webAPIService.webAPIService(&request);
        request.url("/api/analogsensor/test_analogsensor1");
        request.url("/api/analogsensor/36");
        EMSESP::webAPIService.webAPIService(&request);

        // test setting a value
        request.method(HTTP_POST);
        JsonDocument doc;

        char data[] = "{\"value\":10,\"id\":33}";
        deserializeJson(doc, data);
        request.url("/api/analogsensor/setvalue");
        EMSESP::webAPIService.webAPIService(&request, doc.as<JsonVariant>());
        shell.invoke_command("call analogsensor test_analogsensor4");

        char data2[] = "{\"value\":11}";
        deserializeJson(doc, data2);
        request.url("/api/analogsensor/test_analogsensor4");
        EMSESP::webAPIService.webAPIService(&request, doc.as<JsonVariant>());

        shell.invoke_command("call analogsensor test_analogsensor4");

        // test renaming it
        // bool update(uint8_t id, const std::string & name, int16_t offset, float factor, uint8_t uom, uint8_t type);
        // EMSESP::analogsensor_.update(36, "test_analog1_new", 2, 0.7, 17, 1);
        // shell.invoke_command("show values");
        // shell.invoke_command("call system publish");
        ok = true;
    }

    if (command == "healthcheck") {
        // n=1 = EMSESP::system_.HEALTHCHECK_NO_BUS
        // n=2 = EMSESP::system_.HEALTHCHECK_NO_NETWORK
        if (id1 == -1) {
            id1 = 0;
        }
        shell.printfln("Testing healthcheck with %d", id1);
        EMSESP::system_.healthcheck(id1);
        ok = true;
    }

    if (command == "customization") {
        shell.printfln("Testing customization renaming entity");

        Mqtt::ha_enabled(true);
        // Mqtt::send_response(false);

        test("thermostat");

        // before
        // shell.invoke_command("call thermostat seltemp");
        // shell.invoke_command("call system publish");

        // find thermostat
        for (const auto & emsdevice : EMSESP::emsdevices) {
            if (emsdevice->unique_id() == 1) {
                Serial.println();
                Serial.print("Custom: ");
                Serial.print(emsdevice->device_type_name());
                Serial.print(" uniqueid=");
                Serial.println(emsdevice->unique_id());
                std::string a = "00hc1/seltemp|new name>5<52";
                emsdevice->setCustomizationEntity(a);
                break;
            }
        }

        shell.invoke_command("call thermostat seltemp");
        shell.invoke_command("call system publish");

        ok = true;
    }

    if (command == "masked") {
        shell.printfln("Testing masked entities");

        Mqtt::ha_enabled(true);
        // Mqtt::send_response(false);

        test("boiler");

        shell.invoke_command("call boiler wwseltemp");
        shell.invoke_command("call system publish");

        // toggle mode
        for (const auto & emsdevice : EMSESP::emsdevices) {
            if (emsdevice->unique_id() == 1) { // boiler
                std::string a = "07wwseltemp";
                emsdevice->setCustomizationEntity(a);
                break;
            }
        }

        shell.invoke_command("call boiler wwseltemp");
        shell.invoke_command("call system publish");
        ok = true;
    }

    if (command == "dv2") {
        shell.printfln("Testing device value lost");

        Mqtt::ha_enabled(true);
        // Mqtt::send_response(false);

        test("boiler");

        shell.invoke_command("call boiler wwseltemp");
        shell.invoke_command("call system publish");

        // Boiler -> Me, UBAParameterWW(0x33)
        // wwseltemp = goes from 52 degrees (0x34) to void (0xFF)
        // it should delete the HA config topic homeassistant/sensor/ems-esp/boiler_wwseltemp/config
        uart_telegram({0x08, 0x0B, 0x33, 0x00, 0x08, 0xFF, 0xFF, 0xFB, 0x00, 0x28, 0x00, 0x00, 0x46, 0x00, 0xFF, 0xFF, 0x00});

        shell.invoke_command("call boiler wwseltemp");
        shell.invoke_command("call system publish");
        ok = true;
    }

    if (command == "api4") {
        shell.printfln("Testing API writing values...");
        EMSESP::system_.bool_format(BOOL_FORMAT_ONOFF_STR);
        // EMSESP::system_.bool_format(BOOL_FORMAT_ONOFF_STR_CAP);

        // load devices
        test("boiler");
        test("thermostat");

        ok = true;
        AsyncWebServerRequest request;
        JsonDocument          doc;
        JsonVariant           json;
        request.method(HTTP_POST);

        shell.invoke_command("call boiler circpump/value"); // initial state is off

        //  call boiler circpump on
        char data1[] = "{\"device\":\"boiler\", \"cmd\":\"circpump\",\"value\":\"on\"}";
        deserializeJson(doc, data1);
        request.url("/api");
        EMSESP::webAPIService.webAPIService(&request, doc.as<JsonVariant>());
        shell.invoke_command("call boiler circpump/value");

        // switch to german
        EMSESP::system_.locale("de");

        //  call boiler circpump off, but using value in DE
        char data2[] = "{\"device\":\"boiler\", \"cmd\":\"circpump\",\"value\":\"aus\"}";
        deserializeJson(doc, data2);
        request.url("/api");
        EMSESP::webAPIService.webAPIService(&request, doc.as<JsonVariant>());
        shell.invoke_command("call boiler circpump/value");

        //  call boiler circpump on, but using value in DE
        char data3[] = "{\"device\":\"boiler\", \"cmd\":\"circpump\",\"value\":\"an\"}";
        deserializeJson(doc, data3);
        request.url("/api");
        EMSESP::webAPIService.webAPIService(&request, doc.as<JsonVariant>());
        shell.invoke_command("call boiler circpump/value");
    }

    if (command == "api3") {
        shell.printfln("Testing API getting values from system");
        EMSESP::system_.bool_format(BOOL_FORMAT_TRUEFALSE); // BOOL_FORMAT_TRUEFALSE_STR

        ok = true;

        bool single;

        single = true;
        // single = false;

        AsyncWebServerRequest request;
        JsonDocument          doc;
        JsonVariant           json;
        request.method(HTTP_GET);

        // load devices
        test("boiler");
        test("thermostat");

        if (single) {
            // run dedicated tests only

            // EMSESP::webCustomEntityService.test();  // custom entities
            // EMSESP::webCustomizationService.test(); // set customizations - this will overwrite any settings in the FS
            // EMSESP::temperaturesensor_.test();      // add temperature sensors
            // EMSESP::webSchedulerService.test();     // run scheduler tests, and conditions

            // request.url("/rest/deviceEntities");
            // EMSESP::webCustomizationService.device_entities(&request);

            // request.url("/rest/dashboardData");
            // EMSESP::webDataService.dashboard_data(&request);

            // COMMANDS
            // shell.invoke_command("call system fetch");
            // request.url("/api/system/fetch");
            // EMSESP::webAPIService.webAPIService(&request);
            // request.url("/api/system/restart");
            // EMSESP::webAPIService.webAPIService(&request);
            // request.url("/api/system/format");
            // EMSESP::webAPIService.webAPIService(&request);

            // request.url("/api/thermostat");
            // EMSESP::webAPIService.webAPIService(&request);
            // request.url("/api/thermostat/hc1");
            // EMSESP::webAPIService.webAPIService(&request);
            // request.url("/api/boiler/comfort/value");
            // EMSESP::webAPIService.webAPIService(&request);

            // this should fail but it doesn't
            // request.url("/api/boiler/bad/value");
            // EMSESP::webAPIService.webAPIService(&request);

            // POST COMMANDS
            request.method(HTTP_POST);

            // char data1[] = "{\"device\":\"system\", \"cmd\":\"restart\",\"id\":-1}";
            // deserializeJson(doc, data1);
            // request.url("/api");
            // EMSESP::webAPIService.webAPIService(&request, doc.as<JsonVariant>());

            // char data2[] = "{\"action\":\"getCustomSupport\", \"param\":\"hello\"}";
            // deserializeJson(doc, data2);
            // request.url("/rest/action");
            // EMSESP::webStatusService.action(&request, doc.as<JsonVariant>());

            // char data3[] = "{\"action\":\"export\", \"param\":\"schedule\"}";
            // deserializeJson(doc, data3);
            // request.url("/rest/action");
            // EMSESP::webStatusService.action(&request, doc.as<JsonVariant>());

            // char data4[] = "{\"action\":\"export\", \"param\":\"allvalues\"}";
            // deserializeJson(doc, data4);
            // request.url("/rest/action");
            // EMSESP::webStatusService.action(&request, doc.as<JsonVariant>());

            // test version checks
            // test with "current_version_s = "3.7.1-dev.8" in WebStatusService::checkUpgrade()
            // request.url("/rest/action");
            // deserializeJson(doc, "{\"action\":\"checkUpgrade\", \"param\":\"3.7.1-dev.9,3.7.0\"}"); // is upgradable
            // EMSESP::webStatusService.action(&request, doc.as<JsonVariant>());
            // deserializeJson(doc, "{\"action\":\"checkUpgrade\", \"param\":\"3.7.1-dev.7,3.7.0\"}"); // is not upgradable
            // EMSESP::webStatusService.action(&request, doc.as<JsonVariant>());

            // test with "current_version_s = "3.6.5" in WebStatusService::checkUpgrade()
            // request.url("/rest/action");
            // deserializeJson(doc, "{\"action\":\"checkUpgrade\", \"param\":\"3.7.1-dev.9,3.6.5\"}"); // is noy upgradable
            // EMSESP::webStatusService.action(&request, doc.as<JsonVariant>());
            // deserializeJson(doc, "{\"action\":\"checkUpgrade\", \"param\":\"3.7.1-dev.7,3.7.0\"}"); // is upgradable
            // EMSESP::webStatusService.action(&request, doc.as<JsonVariant>());

            // char data6[] = "{\"device\":\"system\", \"cmd\":\"read\",\"value\":\"8 2 27 1\"}";
            // deserializeJson(doc, data6);
            // json = doc.as<JsonVariant>();
            // request.url("/api");
            // EMSESP::webAPIService.webAPIService(&request, json);

            // char data7[] = "{\"device\":\"system\", \"cmd\":\"read\",\"value\":\"10 234\"}";
            // deserializeJson(doc, data7);
            // json = doc.as<JsonVariant>();
            // request.url("/api");
            // EMSESP::webAPIService.webAPIService(&request, json);

            // shell.invoke_command("call system read \"8 2 27 1\"");

        } else {
            EMSESP::webCustomEntityService.test();  // custom entities
            EMSESP::webCustomizationService.test(); // set customizations - this will overwrite any settings in the FS
            EMSESP::temperaturesensor_.test();      // add temperature sensors
            EMSESP::webSchedulerService.test();     // run scheduler tests, and conditions

            request.method(HTTP_GET);

            // boiler
            request.url("/api/boiler");
            EMSESP::webAPIService.webAPIService(&request);
            request.url("/api/boiler/commands");
            EMSESP::webAPIService.webAPIService(&request);
            request.url("/api/boiler/values");
            EMSESP::webAPIService.webAPIService(&request);
            request.url("/api/boiler/info");
            EMSESP::webAPIService.webAPIService(&request);
            request.url("/api/boiler/entities");
            EMSESP::webAPIService.webAPIService(&request);
            request.url("/api/boiler/comfort");
            EMSESP::webAPIService.webAPIService(&request);
            request.url("/api/boiler/comfort/value");
            EMSESP::webAPIService.webAPIService(&request);
            request.url("/api/boiler/comfort/fullname");
            EMSESP::webAPIService.webAPIService(&request);
            request.url("/api/boiler/outdoortemp");
            EMSESP::webAPIService.webAPIService(&request);
            request.url("/api/boiler/dhw/chargetype");
            EMSESP::webAPIService.webAPIService(&request);
            request.url("/api/boiler/dhw.chargetype/writeable");
            EMSESP::webAPIService.webAPIService(&request);
            request.url("/api/boiler/flamecurr/value");
            EMSESP::webAPIService.webAPIService(&request);

            // thermostat
            request.url("/api/thermostat");
            EMSESP::webAPIService.webAPIService(&request);
            request.url("/api/thermostat/hc1/values");
            EMSESP::webAPIService.webAPIService(&request);
            request.url("/api/thermostat/hc1/seltemp");
            EMSESP::webAPIService.webAPIService(&request);
            request.url("/api/thermostat/hc2/seltemp");
            EMSESP::webAPIService.webAPIService(&request);

            // custom
            request.url("/api/custom");
            EMSESP::webAPIService.webAPIService(&request);
            request.url("/api/custom/info");
            EMSESP::webAPIService.webAPIService(&request);
            request.url("/api/custom/seltemp");
            EMSESP::webAPIService.webAPIService(&request);

            // system
            request.url("/api/system");
            EMSESP::webAPIService.webAPIService(&request);
            request.url("/api/system/info");
            EMSESP::webAPIService.webAPIService(&request);
            request.url("/api/system/settings/locale");
            EMSESP::webAPIService.webAPIService(&request);
            request.url("/api/system/fetch");
            EMSESP::webAPIService.webAPIService(&request);
            request.url("api/system/network/values");
            EMSESP::webAPIService.webAPIService(&request);

            // scheduler
            request.url("/api/scheduler");
            EMSESP::webAPIService.webAPIService(&request);
            request.url("/api/scheduler/info");
            EMSESP::webAPIService.webAPIService(&request);
            request.url("/api/scheduler/test_scheduler");
            EMSESP::webAPIService.webAPIService(&request);

            // temperaturesensor
            request.url("/api/temperaturesensor");
            EMSESP::webAPIService.webAPIService(&request);
            request.url("/api/temperaturesensor/info");
            EMSESP::webAPIService.webAPIService(&request);
            request.url("/api/temperaturesensor/test_tempsensor2");
            EMSESP::webAPIService.webAPIService(&request);
            request.url("/api/temperaturesensor/0B_0C0D_0E0F_1011");
            EMSESP::webAPIService.webAPIService(&request);
            request.url("/api/temperaturesensor/test_tempsensor2/value");
            EMSESP::webAPIService.webAPIService(&request);

            // analogsensor
            request.url("/api/analogsensor");
            EMSESP::webAPIService.webAPIService(&request);
            request.url("/api/analogsensor/info");
            EMSESP::webAPIService.webAPIService(&request);
            request.url("/api/analogsensor/test_analogsensor1");
            EMSESP::webAPIService.webAPIService(&request);
            request.url("/api/analogsensor/test_analogsensor1/offset");
            EMSESP::webAPIService.webAPIService(&request);

            // system calls with POST
            request.method(HTTP_POST);

            // these next 3 should return empty JSON in their response
            // but there will be a log message
            char data[] = "{\"cmd\":\"send\",\"data\":\"0B 90 FF 13 01 01 B9 01\"}";
            deserializeJson(doc, data);
            json = doc.as<JsonVariant>();
            request.url("/api/system");
            EMSESP::webAPIService.webAPIService(&request, json);

            char data2[] = "{\"device\":\"system\", \"cmd\":\"send\",\"value\":\"0B 90 FF 13 01 01 B9 02\"}";
            deserializeJson(doc, data2);
            json = doc.as<JsonVariant>();
            request.url("/api");
            EMSESP::webAPIService.webAPIService(&request, json);

            char data4[] = "{\"value\":\"0B 90 FF 13 01 01 B9 03\"}";
            deserializeJson(doc, data4);
            json = doc.as<JsonVariant>();
            request.url("/api/system/send");
            EMSESP::webAPIService.webAPIService(&request, json);

            // console commands
            shell.invoke_command("call system fetch");
            shell.invoke_command("call system send \"0B 90 FF 13 01 01 B9\"");

            //
            // This next batch should all fail
            //

            Serial.println();
            Serial.printf("%s**** Testing bad urls ****\n%s", COLOR_RED, COLOR_RESET);

            request.method(HTTP_GET);

            request.url("/api/boiler2");
            EMSESP::webAPIService.webAPIService(&request);

            // boiler
            request.url("/api/boiler/bad");
            EMSESP::webAPIService.webAPIService(&request);
            request.url("/api/boiler/bad/value");
            EMSESP::webAPIService.webAPIService(&request);
            request.url("/api/boiler/comfort/valu");
            EMSESP::webAPIService.webAPIService(&request);

            // system
            request.url("/api/system/settings/locale2");
            EMSESP::webAPIService.webAPIService(&request);
            request.url("/api/system/settings2");
            EMSESP::webAPIService.webAPIService(&request);
            request.url("/api/system/settings2/locale2");
            EMSESP::webAPIService.webAPIService(&request);

            // scheduler
            request.url("/api/scheduler/test_scheduler2");
            EMSESP::webAPIService.webAPIService(&request);
            request.url("/api/scheduler/test_scheduler/val");
            EMSESP::webAPIService.webAPIService(&request);
            request.url("/api/scheduler/test_scheduler2/val2");
            EMSESP::webAPIService.webAPIService(&request);

            // custom
            request.url("/api/custom/seltemp2");
            EMSESP::webAPIService.webAPIService(&request);
            request.url("/api/custom/seltemp/val");
            EMSESP::webAPIService.webAPIService(&request);

            // temperaturesensor
            request.url("/api/temperaturesensor/test_sensor20");
            EMSESP::webAPIService.webAPIService(&request);
            request.url("/api/temperaturesensor/0B_0C0D_0E0F_XXXX");
            EMSESP::webAPIService.webAPIService(&request);
            request.url("/api/temperaturesensor/test_tempsensor2/bad");
            EMSESP::webAPIService.webAPIService(&request);

            // analogsensor
            request.url("/api/analogsensor/test_analogsensor1/bad");
            EMSESP::webAPIService.webAPIService(&request);
            request.url("/api/analogsensor/test_analog10");
            EMSESP::webAPIService.webAPIService(&request);
            request.url("/api/analogsensor/test_analog10/bad2");
            EMSESP::webAPIService.webAPIService(&request);
        }
    }

    if (command == "mqtt_post") {
        shell.printfln("Testing MQTT incoming changes");
        Mqtt::ha_enabled(false);
        Mqtt::nested_format(1);
        // Mqtt::send_response(false);
        EMSESP::system_.bool_format(BOOL_FORMAT_10); // BOOL_FORMAT_10_STR
        // EMSESP::bool_format(BOOL_FORMAT_TRUEFALSE); // BOOL_FORMAT_TRUEFALSE_STR

        test("boiler");
        test("thermostat");

        EMSESP::mqtt_.incoming("ems-esp/boiler/seltemp", "59");
        EMSESP::mqtt_.incoming("badems-esp/boiler/seltemp", "59"); // should fail

        ok = true;
    }

    // https://github.com/emsesp/EMS-ESP32/issues/541
    if (command == "api_wwmode") {
        shell.printfln("Testing API wwmode");
        Mqtt::ha_enabled(false);
        Mqtt::nested_format(1);
        test("310");

        AsyncWebServerRequest request;
        request.method(HTTP_POST);
        JsonDocument doc;
        JsonVariant  json;

        char odata[] = "{\"value\":\"off\"}";
        deserializeJson(doc, odata);
        json = doc.as<JsonVariant>();
        request.url("/api/thermostat/wwmode");
        EMSESP::webAPIService.webAPIService(&request, json);
        ok = true;
    }

    if (command == "api") {
        shell.printfln("Testing API with MQTT and REST, standalone");

        Mqtt::ha_enabled(true);
        // Mqtt::ha_enabled(false);

        Mqtt::nested_format(1);
        // Mqtt::send_response(true);

        test("boiler");
        test("thermostat");

        // test command parse
        int8_t       id_n;
        const char * ncmd;
        char         command_s[100];
        id_n = -1;
        strlcpy(command_s, "hc2/seltemp", sizeof(command_s));
        ncmd = Command::parse_command_string(command_s, id_n);
        shell.printfln("test cmd parse cmd=%s id=%d", ncmd, id_n);
        id_n = -1;
        strlcpy(command_s, "seltemp", sizeof(command_s));
        ncmd = Command::parse_command_string(command_s, id_n);
        shell.printfln("test cmd parse cmd=%s id=%d", ncmd, id_n);
        id_n = -1;
        strlcpy(command_s, "xyz/seltemp", sizeof(command_s));
        ncmd = Command::parse_command_string(command_s, id_n);
        shell.printfln("test cmd parse cmd=%s id=%d", ncmd, id_n);
        id_n = -1;
        strlcpy(command_s, "dhw4/seltemp", sizeof(command_s));
        ncmd = Command::parse_command_string(command_s, id_n);
        shell.printfln("test cmd parse cmd=%s id=%d", ncmd, id_n);
        id_n = -1;
        strlcpy(command_s, "hc3_seltemp", sizeof(command_s));
        ncmd = Command::parse_command_string(command_s, id_n);
        shell.printfln("test cmd parse cmd=%s id=%d", ncmd, id_n);

        // Console tests
        shell.invoke_command("call thermostat entities");
        shell.invoke_command("call thermostat mode auto");

        // MQTT good tests
        EMSESP::mqtt_.incoming("ems-esp/thermostat/mode", "auto");
        EMSESP::mqtt_.incoming("ems-esp/thermostat/hc2/mode", "auto");
        EMSESP::mqtt_.incoming("ems-esp/thermostat/dhw3/mode", "auto");
        EMSESP::mqtt_.incoming("ems-esp/boiler/dhw/circpump", "off");
        EMSESP::mqtt_.incoming("ems-esp/thermostat/seltemp");    // empty payload
        EMSESP::mqtt_.incoming("ems-esp/thermostat_hc1", "22");  // HA only
        EMSESP::mqtt_.incoming("ems-esp/thermostat_hc1", "off"); // HA only
        EMSESP::mqtt_.incoming("ems-esp/system/send", "11 12 13");
        EMSESP::mqtt_.incoming("ems-esp/boiler/syspress"); // empty payload
        EMSESP::mqtt_.incoming("ems-esp/thermostat/mode"); // empty payload
        EMSESP::mqtt_.incoming("ems-esp/system/publish");
        EMSESP::mqtt_.incoming("ems-esp/thermostat/seltemp"); // empty payload

        EMSESP::mqtt_.incoming("ems-esp/boiler/wwseltemp", "59");
        EMSESP::mqtt_.incoming("ems-esp/boiler/wwseltemp");

        EMSESP::mqtt_.incoming("ems-esp/thermostat", "{\"cmd\":\"mode\",\"data\":\"heat\",\"id\":1}");

        // MQTT bad tests - these should all fail
        EMSESP::mqtt_.incoming("ems-esp/thermostate/mode", "auto");     // unknown device
        EMSESP::mqtt_.incoming("ems-esp/thermostat/modee", "auto");     // unknown command
        EMSESP::mqtt_.incoming("ems-esp/thermostat/mode/auto", "auto"); // invalid, not allowed

        // check extended MQTT base
        // Mqtt::base("home/cellar/heating");
        EMSESP::mqtt_.incoming("home/cellar/heating/thermostat/mode"); // empty payload

        // Web API TESTS
        AsyncWebServerRequest request;
        request.method(HTTP_GET);

        request.url("/api/thermostat"); // check if defaults to info
        EMSESP::webAPIService.webAPIService(&request);
        request.url("/api/thermostat/info");
        EMSESP::webAPIService.webAPIService(&request);
        request.url("/api/thermostat/values");
        EMSESP::webAPIService.webAPIService(&request);
        request.url("/api/thermostat/seltemp");
        EMSESP::webAPIService.webAPIService(&request);
        request.url("/api/system/commands");
        EMSESP::webAPIService.webAPIService(&request);
        request.url("/api/system/info");
        EMSESP::webAPIService.webAPIService(&request);
        request.url("/api/boiler/syspress");
        EMSESP::webAPIService.webAPIService(&request);
        request.url("/api/boiler/dhw/curflow");
        EMSESP::webAPIService.webAPIService(&request);

        // POST tests
        request.method(HTTP_POST);
        JsonDocument doc;
        JsonVariant  json;

        // 1
        char data1[] = "{\"entity\":\"seltemp\",\"value\":11}";
        deserializeJson(doc, data1);
        json = doc.as<JsonVariant>();
        request.url("/api/thermostat");
        EMSESP::webAPIService.webAPIService(&request, json);

        // 2
        char data2[] = "{\"value\":12}";
        deserializeJson(doc, data2);
        json = doc.as<JsonVariant>();
        request.url("/api/thermostat/seltemp");
        EMSESP::webAPIService.webAPIService(&request, json);

        // 3
        char data3[] = "{\"device\":\"thermostat\", \"cmd\":\"seltemp\",\"value\":13}";
        deserializeJson(doc, data3);
        json = doc.as<JsonVariant>();
        request.url("/api");
        EMSESP::webAPIService.webAPIService(&request, json);

        // 4 - system call
        char data4[] = "{\"value\":\"0B 88 19 19 02\"}";
        deserializeJson(doc, data4);
        json = doc.as<JsonVariant>();
        request.url("/api/system/send");
        EMSESP::webAPIService.webAPIService(&request, json);

        // 5 - test write value
        //  device=3 cmd=hc2/seltemp value=44
        char data5[] = "{\"device\":\"thermostat\", \"cmd\":\"hc2.seltemp\",\"value\":14}";
        deserializeJson(doc, data5);
        json = doc.as<JsonVariant>();
        request.url("/api");
        EMSESP::webAPIService.webAPIService(&request, json);

        // write value from web - testing hc2/seltemp
        char data6[] = "{\"id\":2,\"devicevalue\":{\"v\":\"44\",\"u\":1,\"n\":\"hc2 selected room temperature\",\"c\":\"hc2/seltemp\"}";
        deserializeJson(doc, data6);
        json = doc.as<JsonVariant>();
        request.url("/rest/writeDeviceValue");
        EMSESP::webDataService.write_device_value(&request, json);

        // call reset command
        char data7[] = "{\"device\":\"boiler\", \"cmd\":\"reset\",\"value\":\"error\"}";
        deserializeJson(doc, data7);
        json = doc.as<JsonVariant>();
        request.url("/api");
        EMSESP::webAPIService.webAPIService(&request, json);

        emsesp::EMSESP::logger().warning("* these next ones should fail *");

        // write value from web - testing hc9/seltemp - should fail!
        char data8[] = "{\"id\":2,\"devicevalue\":{\"v\":\"55\",\"u\":1,\"n\":\"hc2 selected room temperature\",\"c\":\"hc9/seltemp\"}";
        deserializeJson(doc, data8);
        json = doc.as<JsonVariant>();
        request.url("/rest/writeDeviceValue");
        EMSESP::webDataService.write_device_value(&request, json);

        // should fail!
        char data9[] = "{}";
        deserializeJson(doc, data9);
        json = doc.as<JsonVariant>();
        request.url("/api/thermostat/mode/auto");
        EMSESP::webAPIService.webAPIService(&request, json);
        ok = true;
    }

    if (command == "mqtt_nested") {
        shell.printfln("Testing nested MQTT");
        Mqtt::ha_enabled(false); // turn off HA Discovery to stop the chatter

        test("boiler");
        test("thermostat");
        test("solar");
        test("mixer");

        // first with nested
        Mqtt::nested_format(1);
        shell.invoke_command("call system publish");
        shell.invoke_command("show mqtt");

        // then without nested - single mode
        Mqtt::nested_format(2);
        shell.invoke_command("call system publish");
        shell.invoke_command("show mqtt");
        ok = true;
    }

    if (command == "thermostat") {
        shell.printfln("Testing adding a thermostat FW120...");

        test("thermostat");
        shell.invoke_command("show values");
        shell.invoke_command("call system publish");

        EMSESP::mqtt_.incoming("ems-esp/thermostat_hc1", "heat");
        EMSESP::mqtt_.incoming("ems-esp/thermostat_hc2", "28.8");
        EMSESP::mqtt_.incoming("ems-esp/thermostat", "{\"cmd\":\"temp\",\"id\":2,\"data\":22}");
        EMSESP::mqtt_.incoming("ems-esp/thermostat_hc3", "{\"cmd\":\"offsettemp\",\"data\":-3}");
        EMSESP::mqtt_.incoming("ems-esp/thermostat_hc3", "{\"cmd\":\"temp\",\"data\":-3}");

        shell.invoke_command("show mqtt");
        ok = true;
    }

    if (command == "tc100") {
        shell.printfln("Testing adding a TC100 thermostat to the EMS bus...");

        // add a thermostat
        add_device(0x18, 202); // Bosch TC100 - https://github.com/emsesp/EMS-ESP/issues/474

        // 0x0A
        uart_telegram({0x98, 0x0B, 0x0A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00});
        ok = true;
    }

    if (command == "solar") {
        shell.printfln("Testing Solar");
        test("solar");

        uart_telegram("30 00 FF 0A 02 6A 04"); // SM100 pump on (1)sh
        EMSESP::show_device_values(shell);
        uart_telegram("30 00 FF 0A 02 6A 03"); // SM100 pump off (0)
        EMSESP::show_device_values(shell);
        shell.invoke_command("call system publish");

        // EMSESP::txservice_.send_raw("B0 00 FF 18 02 62 80 00 B8");
        ok = true;
    }

    if (command == "heatpump") {
        shell.printfln("Testing Heat Pump");
        test("heatpump");
        shell.invoke_command("call");
        shell.invoke_command("call heatpump info");
        ok = true;
    }

    if (command == "solar200") {
        shell.printfln("Testing Solar SM200");

        add_device(0x30, 164); // SM200

        // SM100Monitor - type 0x0362 EMS+ - for SM100 and SM200
        // B0 0B FF 00 02 62 00 44 02 7A 80 00 80 00 80 00 80 00 80 00 80 00 00 7C 80 00 80 00 80 00 80
        rx_telegram({0xB0, 0x0B, 0xFF, 00, 0x02, 0x62, 00, 0x44, 0x02, 0x7A, 0x80, 00, 0x80, 0x00, 0x80, 00,
                     0x80, 00,   0x80, 00, 0x80, 00,   00, 0x7C, 0x80, 00,   0x80, 00, 0x80, 00,   0x80});

        rx_telegram({0xB0, 0x0B, 0xFF, 0x00, 0x02, 0x62, 0x01, 0x44, 0x03, 0x30, 0x80, 00, 0x80, 00, 0x80, 00,
                     0x80, 00,   0x80, 00,   0x80, 00,   0x80, 00,   0x80, 00,   0x80, 00, 0x80, 00, 0x80, 0x33});

        rx_telegram({0xB0, 00, 0xFF, 0x18, 02, 0x62, 0x80, 00, 0xB8});

        EMSESP::txservice_.send_raw("B0 00 FF 18 02 62 80 00 B8");

        uart_telegram("30 00 FF 0A 02 6A 04");                                                 // SM100 pump on  1
        uart_telegram("30 00 FF 00 02 64 00 00 00 04 00 00 FF 00 00 1E 0B 09 64 00 00 00 00"); // SM100 modulation

        uart_telegram("30 00 FF 0A 02 6A 03"); // SM100 pump off  0

        shell.invoke_command("show values");
        ok = true;
    }

    if (command == "km") {
        shell.printfln("Testing KM200 Gateway");

        add_device(0x10, 158); // RC300
        add_device(0x48, 189); // KM200

        // see https://github.com/emsesp/EMS-ESP/issues/390

        uart_telegram_withCRC("90 48 FF 04 01 A6 5C");
        uart_telegram_withCRC("90 48 FF 00 01 A6 4C");
        uart_telegram_withCRC("90 48 FF 08 01 A7 6D");

        uart_telegram("90 48 F9 00 FF 01 B0 08 0B 00 00 00 14 00 00 00 19 00 00 00 4B 00 00");
        uart_telegram("90 48 F9 00 FF 01 9C 08 03 00 00 00 1E 00 00 00 4B 00 00 00 55 00 00");
        uart_telegram("90 48 F9 00 FF 01 9C 07 03 00 00 00 1E 00 00 00 30 00 00 00 3C 00 00");

        uart_telegram_withCRC("90 48 F9 00 FF 01 9D 00 43 00 00 00 01 00 00 00 02 00 03 00 06 00 03 00 02 05");
        uart_telegram_withCRC("90 48 F9 00 FF 01 9D 07 03 00 00 00 1E 00 00 00 30 00 00 00 3C 00 00 00 30 C4");
        uart_telegram_withCRC("90 48 F9 00 FF 01 9D 08 03 00 00 00 1E 00 00 00 4B 00 00 00 55 00 00 00 4B C8");
        uart_telegram_withCRC("90 48 F9 00 FF 01 B1 08 0B 00 00 00 14 00 00 00 19 00 00 00 4B 00 00 00 19 A2");

        uart_telegram_withCRC("90 48 FF 07 01 A7 51");
        uart_telegram_withCRC("90 48 FF 08 01 A7 6D");
        uart_telegram_withCRC("90 48 FF 00 01 A7 4D");
        uart_telegram_withCRC("90 48 FF 25 01 A6 D8");
        uart_telegram_withCRC("90 48 FF 07 01 A7 51");
        uart_telegram_withCRC("90 0B 06 00 14 06 17 08 03 22 00 01 10 FF 00 18"); // time

        uart_telegram("90 0B FF 00 01 A5 80 00 01 28 17 00 28 2A 05 A0 02 03 03 05 A0 05 A0 00 00 11 01 02 FF FF 00");
        uart_telegram("90 0B FF 00 01 B9 00 2E 26 26 1B 03 00 FF FF 05 28 01 E1 20 01 0F 05 2A");
        uart_telegram("90 0B FF 00 01 A6 90 0B FF 00 01 A6 18");
        uart_telegram("90 0B FF 00 01 B9 00 2E 26 26 1B 03 00 FF FF 05 28 01 E1 20 01 0F 05 2A");
        uart_telegram("90 0B FF 00 01 A6 90 0B FF 00 01 A6 18");
        uart_telegram("90 0B FF 00 01 BA 00 2E 2A 26 1E 03 00 FF FF 05 2A 01 E1 20 01 0F 05 2A");
        uart_telegram("90 0B FF 00 01 A7 90 0B FF 00 01 A7 19");
        uart_telegram("90 0B FF 00 01 BB 00 2E 2A 26 1E 03 00 FF FF 05 2A 01 E1 20 01 0F 05 2A");
        uart_telegram("90 0B FF 00 01 A8 90 0B FF 00 01 A8 16");
        uart_telegram("90 0B FF 00 01 BC 00 2E 2A 26 1E 03 00 FF FF 05 2A 01 E1 20 01 0F 05 2A");
        uart_telegram("90 0B FF 00 01 A5 80 00 01 28 17 00 28 2A 05 A0 02 03 03 05 A0 05 A0 00 00 11 01 02 FF FF 00");

        uart_telegram_withCRC("C8 90 F7 02 01 FF 01 A6 BA");
        uart_telegram_withCRC("90 48 FF 03 01 A6 40");
        uart_telegram_withCRC("C8 90 FF 00 02 01 A6 D0");

        // uart_telegram_withCRC("10 00 FF 00 01 A5 00 D7 21 00 00 00 00 30 01 84 01 01 03 01 84 01 F1 00 00 11 01 00 08 63 00");

        uart_telegram_withCRC("C8 90 F7 02 01 FF 01 A6 BA");

        uart_telegram_withCRC("90 48 FF 04 01 A6 5C");
        uart_telegram_withCRC("90 00 FF 00 01 A5 80 00 01 27 16 00 27 2A 05 A0 02 03 03 05 A0 05 A0 00 00 11 01 02 FF FF 00 9A");
        uart_telegram_withCRC("90 00 FF 19 01 A5 01 04 00 00 00 00 FF 64 2A 00 3C 01 FF 92");
        uart_telegram_withCRC("90 0B FF 00 01 A5 80 00 01 26 15 00 26 2A 05 A0 03 03 03 05 A0 05 A0 00 00 11 01 03 FF FF 00 FE");
        uart_telegram_withCRC("90 00 FF 19 01 A5 01 04 00 00 00 00 FF 64 2A 00 3C 01 FF 92");

        EMSESP::show_ems(shell);
        EMSESP::show_device_values(shell);
        ok = true;
    }

    if (command == "cr100") {
        shell.printfln("Testing CR100");

        add_device(0x18, 157); // Bosch CR100 - https://github.com/emsesp/EMS-ESP/issues/355

        // RCPLUSStatusMessage_HC1(0x01A5)
        // 98 00 FF 00 01 A5 00 CF 21 2E 00 00 2E 24 03 25 03 03 01 03 25 00 C8 00 00 11 01 03 (no CRC)
        uart_telegram({0x98, 0x00, 0xFF, 0x00, 0x01, 0xA5, 0x00, 0xCF, 0x21, 0x2E, 0x00, 0x00, 0x2E, 0x24,
                       0x03, 0x25, 0x03, 0x03, 0x01, 0x03, 0x25, 0x00, 0xC8, 0x00, 0x00, 0x11, 0x01, 0x03});

        uart_telegram("98 00 FF 00 01 A5 00 CF 21 2E 00 00 2E 24 03 25 03 03 01 03 25 00 C8 00 00 11 01 03");            // without CRC
        uart_telegram_withCRC("98 00 FF 00 01 A6 00 CF 21 2E 00 00 2E 24 03 25 03 03 01 03 25 00 C8 00 00 11 01 03 6B"); // with CRC

        shell.loop_all();
        EMSESP::show_device_values(shell);

        shell.invoke_command("call thermostat mode auto");

        shell.loop_all();
        EMSESP::show_ems(shell);
        shell.loop_all();

        EMSESP::txservice_.send(); // send it to UART
        ok = true;
    }

    if (command == "rx2") {
        shell.printfln("Testing Rx2...");
        for (uint8_t i = 0; i < 30; i++) {
            uart_telegram({0x08, 0x0B, 0x33, 0x00, 0x08, 0xFF, 0x34, 0xFB, 0x00, 0x28, 0x00, 0x00, 0x46, 0x00, 0xFF, 0xFF, 0x00});
            ok = true;
        }
    }

    if (command == "rx") {
        shell.printfln("Testing Rx...");

        // fake telegrams. length includes CRC
        // Boiler -> Me, UBAMonitorFast(0x18), telegram: 08 00 18 00 00 02 5A 73 3D 0A 10 65 40 02 1A 80 00 01 E1 01 76 0E 3D 48 00 C9 44 02 00 (#data=25)
        uart_telegram({0x08, 0x00, 0x18, 0x00, 0x00, 0x02, 0x5A, 0x73, 0x3D, 0x0A, 0x10, 0x65, 0x40, 0x02, 0x1A,
                       0x80, 0x00, 0x01, 0xE1, 0x01, 0x76, 0x0E, 0x3D, 0x48, 0x00, 0xC9, 0x44, 0x02, 0x00});

        // Boiler -> Thermostat, UBAParameterWW(0x33), telegram: 08 97 33 00 23 24 (#data=2)
        uart_telegram({0x08, 0x97, 0x33, 0x00, 0x23, 0x24});

        // Boiler -> Me, UBAParameterWW(0x33), telegram: 08 0B 33 00 08 FF 34 FB 00 28 00 00 46 00 FF FF 00 (#data=13)
        uart_telegram({0x08, 0x0B, 0x33, 0x00, 0x08, 0xFF, 0x34, 0xFB, 0x00, 0x28, 0x00, 0x00, 0x46, 0x00, 0xFF, 0xFF, 0x00});

        // Thermostat -> Me, RC20StatusMessage(0x91), telegram: 17 0B 91 05 44 45 46 47 (#data=4)
        uart_telegram({0x17, 0x0B, 0x91, 0x05, 0x44, 0x45, 0x46, 0x47});

        // bad CRC - corrupt telegram - CRC should be 0x8E
        uint8_t t5[] = {0x17, 0x0B, 0x91, 0x05, 0x44, 0x45, 0x46, 0x47, 0x99};
        EMSESP::rxservice_.add(t5, sizeof(t5));

        // simulating a Tx record
        uart_telegram({0x0B, 0x88, 0x07, 0x00, 0x20});

        // Version Boiler
        uart_telegram({0x08, 0x0B, 0x02, 0x00, 0x7B, 0x06, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04});

        // Version Thermostat, device_id 0x11
        uart_telegram({0x11, 0x0B, 0x02, 0x00, 0x4D, 0x03, 0x03});

        // Thermostat -> all, telegram: 10 00 FF 00 01 A5 00 D7 21 00 00 00 00 30 01 84 01 01 03 01 84 01 F1 00 00 11 01 00 08 63 00
        // 0x1A5 test ems+
        uart_telegram({0x10, 0x00, 0xFF, 0x00, 0x01, 0xA5, 0x00, 0xD7, 0x21, 0x00, 0x00, 0x00, 0x00, 0x30, 0x01, 0x84,
                       0x01, 0x01, 0x03, 0x01, 0x84, 0x01, 0xF1, 0x00, 0x00, 0x11, 0x01, 0x00, 0x08, 0x63, 0x00});

        // setting temp from 21.5 to 19.9C
        uart_telegram({0x10, 0x00, 0xFF, 0x00, 0x01, 0xA5, 0x00, 0xC7, 0x21, 0x00, 0x00, 0x00, 0x00, 0x30, 0x01, 0x84,
                       0x01, 0x01, 0x03, 0x01, 0x84, 0x01, 0xF1, 0x00, 0x00, 0x11, 0x01, 0x00, 0x08, 0x63, 0x00});

        // Thermostat -> Boiler, UBAFlags(0x35), telegram: 17 08 35 00 11 00 (#data=2)
        uart_telegram({0x17, 0x08, 0x35, 0x00, 0x11, 0x00});

        // Thermostat -> Boiler, UBASetPoints(0x1A), telegram: 17 08 1A 00 00 00 00 00 (#data=4)
        uart_telegram({0x17, 0x08, 0x1A, 0x00, 0x00, 0x00, 0x00, 0x00});

        // Thermostat -> Me, RC20Set(0xA8), telegram: 17 0B A8 00 01 00 FF F6 01 06 00 01 0D 01 00 FF FF 01 02 02 02 00 00 05 1F 05 1F 02 0E 00 FF (#data=27)
        uart_telegram({0x17, 0x0B, 0xA8, 0x00, 0x01, 0x00, 0xFF, 0xF6, 0x01, 0x06, 0x00, 0x01, 0x0D, 0x01, 0x00, 0xFF,
                       0xFF, 0x01, 0x02, 0x02, 0x02, 0x00, 0x00, 0x05, 0x1F, 0x05, 0x1F, 0x02, 0x0E, 0x00, 0xFF});

        //  Boiler(0x08) -> All(0x00), UBAMonitorWW(0x34), data: 36 01 A5 80 00 21 00 00 01 00 01 3E 8D 03 77 91 00 80 00
        uart_telegram(
            {0x08, 0x00, 0x34, 0x00, 0x36, 0x01, 0xA5, 0x80, 0x00, 0x21, 0x00, 0x00, 0x01, 0x00, 0x01, 0x3E, 0x8D, 0x03, 0x77, 0x91, 0x00, 0x80, 0x00});

        // test 0x2A - DHWStatus3
        uart_telegram({0x88, 00, 0x2A, 00, 00, 00, 00, 00, 00, 00, 00, 00, 0xD2, 00, 00, 0x80, 00, 00, 01, 0x9D, 0x80, 0x00, 0x02, 0x79, 00});
        ok = true;
    }

    if (command == "tx") {
        shell.printfln("Testing Tx...");

        // TX queue example - Me -> Thermostat, (0x91), telegram: 0B 17 91 05 44 45 46 47 (#data=4)
        uint8_t t11[] = {0x44, 0x45, 0x46, 0x47};
        EMSESP::txservice_.add(Telegram::Operation::TX_RAW, 0x17, 0x91, 0x05, t11, sizeof(t11), 0);

        // TX - raw example test
        uint8_t t12[] = {0x10, 0x08, 0x63, 0x04, 0x64};
        EMSESP::txservice_.add(Telegram::Operation::TX_RAW, t12, sizeof(t12), 0);

        // TX - sending raw string
        EMSESP::txservice_.send_raw("10 08 63 03 64 65 66");

        // TX - send a read request
        EMSESP::send_read_request(0x18, 0x08);

        // TX - send a write request
        uint8_t t18[] = {0x52, 0x79};
        EMSESP::send_write_request(0x91, 0x17, 0x00, t18, sizeof(t18), 0x00);

        // TX - send EMS+
        const uint8_t t13[] = {0x90, 0x0B, 0xFF, 00, 01,   0xBA, 00,   0x2E, 0x2A, 0x26, 0x1E, 0x03,
                               00,   0xFF, 0xFF, 05, 0x2A, 01,   0xE1, 0x20, 0x01, 0x0F, 05,   0x2A};
        EMSESP::txservice_.add(Telegram::Operation::TX_RAW, t13, sizeof(t13), 0);

        // EMS+ Junkers read request
        EMSESP::send_read_request(0x16F, 0x10);

        EMSESP::show_ems(shell);

        // process whole Tx queue
        for (uint8_t i = 0; i < 10; i++) {
            EMSESP::txservice_.send(); // send it to UART
        }

        ok = true;
    }

    if (command == "poll") {
        shell.printfln("Testing Poll...");

        // simulate sending a read request
        // uint8_t t16[] = {0x44, 0x45, 0x46, 0x47}; // Me -> Thermostat, (0x91), telegram: 0B 17 91 05 44 45 46 47 (#data=4)
        // EMSESP::txservice_.add(Telegram::Operation::TX_RAW, 0x17, 0x91, 0x05, t16, sizeof(t16), 0);
        EMSESP::send_read_request(0x91, 0x17);
        // EMSESP::txservice_.show_tx_queue();

        // Simulate adding a Poll, so read request is sent
        uint8_t poll[1] = {0x8B};
        EMSESP::incoming_telegram(poll, 1);

        // incoming Rx
        uart_telegram({0x17, 0x08, 0x1A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3A});

        // Simulate adding a Poll - should send retry
        EMSESP::incoming_telegram(poll, 1);

        EMSESP::show_ems(shell);
        uint8_t t2[] = {0x21, 0x22};
        EMSESP::send_write_request(0x91, 0x17, 0x00, t2, sizeof(t2), 0);
        EMSESP::show_ems(shell);

        ok = true;
    }

    if (command == "cmd") {
        shell.printfln("Testing Commands...");

        // add a thermostat with 3 HCs
        add_device(0x10, 192); // FW120

        // HC1 - 3
        uart_telegram({0x90, 0x00, 0xFF, 0x00, 0x00, 0x6F, 0x03, 0x02, 0x00, 0xCD, 0x00, 0xE4});
        uart_telegram({0x90, 0x00, 0xFF, 0x00, 0x00, 0x70, 0x02, 0x01, 0x00, 0xCE, 0x00, 0xE5});
        uart_telegram({0x90, 0x00, 0xFF, 0x00, 0x00, 0x71, 0x01, 0x02, 0x00, 0xCF, 0x00, 0xE6});

        // add room sensor - test for https://github.com/emsesp/EMS-ESP32/issues/699
        // use auto
        uart_telegram("90 00 FF 00 00 65 01 00 02 19 3D 42 02 01 00 03 FF 28 06 01 02 20 27 29 01 01 00");

        shell.invoke_command("help");
        shell.invoke_command("call");
        shell.invoke_command("call system info");

        EMSESP::mqtt_.incoming("ems-esp/system", "{\"cmd\":\"info\"}"); // this should fail

        EMSESP::mqtt_.incoming("ems-esp/thermostat", "{\"cmd\":\"temp\",\"data\":23.45}"); // this should work just fine
        EMSESP::mqtt_.incoming("ems-esp/thermostat", "{\"cmd\":\"TeMP\",\"data\":23.45}"); // test mix cased cmd

        EMSESP::mqtt_.incoming("ems-esp/thermostat_hc1", "20");

        shell.invoke_command("call thermostat wwmode");      // should do nothing
        shell.invoke_command("call thermostat mode auto 2"); // should error, no hc2
        shell.invoke_command("call thermostat temp 22.56");
        ok = true;
    }

    if (command == "pin") {
        shell.printfln("Testing pin...");
        shell.invoke_command("call system pin");
        shell.invoke_command("call system pin 1 true");
        ok = true;
    }

    if (command == "mqtt2") {
        shell.printfln("Testing MQTT large payloads...");

        JsonDocument doc;

        char key[8];
        char value[8];

        // fit it up, to its limit of the Json buffer (which is about 169 records)
        for (uint8_t i = 0; i < 200; i++) {
            snprintf(key, 7, "key%03d", i);
            snprintf(value, 7, "val%03d", i);
            doc[key] = value;
        }
        doc.shrinkToFit();
        JsonObject jo = doc.as<JsonObject>();
        shell.printfln("Length of JSON payload = %d", measureJson(jo));

        Mqtt::queue_publish("test", jo);
        Mqtt::show_mqtt(shell); // show queue
        ok = true;
    }

    if (command == "mqtt") {
        shell.printfln("Testing MQTT...");

        Mqtt::ha_enabled(false);
        Mqtt::enabled(true);

        // add a boiler
        add_device(0x08, 123); // Nefit Trendline

        // add some boiler data
        // Boiler -> Me, UBAMonitorFast(0x18), telegram: 08 00 18 00 00 02 5A 73 3D 0A 10 65 40 02 1A 80 00 01 E1 01 76 0E 3D 48 00 C9 44 02 00 (#data=25)
        uart_telegram({0x08, 0x00, 0x18, 0x00, 0x00, 0x02, 0x5A, 0x73, 0x3D, 0x0A, 0x10, 0x65, 0x40, 0x02, 0x1A,
                       0x80, 0x00, 0x01, 0xE1, 0x01, 0x76, 0x0E, 0x3D, 0x48, 0x00, 0xC9, 0x44, 0x02, 0x00});

        // Boiler -> Thermostat, UBAParameterWW(0x33), telegram: 08 97 33 00 23 24 (#data=2)
        uart_telegram({0x08, 0x98, 0x33, 0x00, 0x23, 0x24});

        // Boiler -> Me, UBAParameterWW(0x33), telegram: 08 0B 33 00 08 FF 34 FB 00 28 00 00 46 00 FF FF 00 (#data=13)
        uart_telegram({0x08, 0x0B, 0x33, 0x00, 0x08, 0xFF, 0x34, 0xFB, 0x00, 0x28, 0x00, 0x00, 0x46, 0x00, 0xFF, 0xFF, 0x00});

        // add a thermostat
        add_device(0x18, 157); // Bosch CR100

        // add some thermostat data
        // RCPLUSStatusMessage_HC1(0x01A5) - HC1
        uart_telegram({0x98, 0x00, 0xFF, 0x00, 0x01, 0xA5, 0x00, 0xCF, 0x21, 0x2E, 0x00, 0x00, 0x2E, 0x24,
                       0x03, 0x25, 0x03, 0x03, 0x01, 0x03, 0x25, 0x00, 0xC8, 0x00, 0x00, 0x11, 0x01, 0x03});
        uart_telegram("98 00 FF 00 01 A5 00 CF 21 2E 00 00 2E 24 03 25 03 03 01 03 25 00 C8 00 00 11 01 03");            // without CRC
        uart_telegram_withCRC("98 00 FF 00 01 A5 00 CF 21 2E 00 00 2E 24 03 25 03 03 01 03 25 00 C8 00 00 11 01 03 13"); // with CRC

        shell.invoke_command("call system publish");
        shell.invoke_command("show mqtt");
        shell.loop_all();

        char boiler_topic[Mqtt::MQTT_TOPIC_MAX_SIZE];
        char thermostat_topic[Mqtt::MQTT_TOPIC_MAX_SIZE];
        char thermostat_topic_hc1[Mqtt::MQTT_TOPIC_MAX_SIZE];
        char system_topic[Mqtt::MQTT_TOPIC_MAX_SIZE];
        Mqtt::show_mqtt(shell); // show queue

        strlcpy(boiler_topic, "ems-esp/boiler", sizeof(boiler_topic));
        strlcpy(thermostat_topic, "ems-esp/thermostat", sizeof(thermostat_topic));
        strlcpy(thermostat_topic_hc1, "ems-esp/thermostat/hc1", sizeof(thermostat_topic));
        strlcpy(system_topic, "ems-esp/system", sizeof(system_topic));

        // test publishing
        EMSESP::mqtt_.queue_publish(boiler_topic, "test me");

        // test receiving
        EMSESP::mqtt_.incoming(boiler_topic, ""); // test if ignore empty payloads, should return values

        // these all should fail
        EMSESP::mqtt_.incoming(boiler_topic, "12345");                                    // error: invalid format
        EMSESP::mqtt_.incoming("bad_topic", "123456");                                    // error: no matching topic
        EMSESP::mqtt_.incoming(boiler_topic, "{\"cmd\":\"garbage\",\"data\":22.52}");     // error: should report error
        EMSESP::mqtt_.incoming(thermostat_topic, "{\"cmd\":\"control\",\"data\":\"1\"}"); // RC35 only, should error

        // these all should pass
        EMSESP::mqtt_.incoming(boiler_topic, "{\"cmd\":\"wwactivated\",\"data\":\"1\"}"); // with quotes
        EMSESP::mqtt_.incoming(boiler_topic, "{\"cmd\":\"wwactivated\",\"data\":1}");     // without quotes
        EMSESP::mqtt_.incoming(boiler_topic, "{\"cmd\":\"selflowtemp\",\"data\":55}");

        // test direct commands
        EMSESP::mqtt_.incoming("ems-esp/boiler/selflowtemp", "56");

        EMSESP::mqtt_.incoming(system_topic, "{\"cmd\":\"send\",\"data\":\"01 02 03 04 05\"}");
        // EMSESP::mqtt_.incoming(system_topic, "{\"cmd\":\"pin\",\"id\":12,\"data\":\"1\"}");

        EMSESP::mqtt_.incoming(thermostat_topic, "{\"cmd\":\"wwmode\",\"data\":\"auto\"}");
        EMSESP::mqtt_.incoming(thermostat_topic, "{\"cmd\":\"mode\",\"data\":\"typo\",\"id\":2}"); // invalid mode
        EMSESP::mqtt_.incoming(thermostat_topic, "{\"cmd\":\"mode\",\"data\":\"auto\",\"id\":2}");
        EMSESP::mqtt_.incoming(thermostat_topic, "{\"cmd\":\"mode\",\"data\":\"auto\",\"hc\":2}");        // hc as number
        EMSESP::mqtt_.incoming(thermostat_topic, "{\"cmd\":\"seltemp\",\"data\":19.5,\"hc\":1}");         // data as number
        EMSESP::mqtt_.incoming(thermostat_topic, "{\"cmd\":\"seltemp\",\"data\":\"auto\",\"hc\":\"2\"}"); // hc as string. should error as no hc2
        EMSESP::mqtt_.incoming(thermostat_topic, "{\"cmd\":\"seltemp\",\"data\":22.56}");
        EMSESP::mqtt_.incoming(thermostat_topic, "{\"cmd\":\"seltemp\",\"data\":22}");
        EMSESP::mqtt_.incoming(thermostat_topic, "{\"cmd\":\"seltemp\",\"data\":\"22.56\"}");
        EMSESP::mqtt_.incoming(thermostat_topic, "{\"cmd\":\"seltemp\",\"id\":2,\"data\":22}");

        // test with hc
        EMSESP::mqtt_.incoming("ems-esp/thermostat/hc1/seltemp", "30");
        EMSESP::mqtt_.incoming("ems-esp/thermostat/hc2/seltemp", "32");

        // test single commands
        EMSESP::mqtt_.incoming(thermostat_topic, "auto");
        EMSESP::mqtt_.incoming(thermostat_topic, "heat");
        EMSESP::mqtt_.incoming(thermostat_topic, "28.8");

        EMSESP::publish_all(true);

        Mqtt::resubscribe();
        Mqtt::show_mqtt(shell); // show queue
        ok = true;
    }

    if (command == "modbus") {
        shell.printfln("Testing Modbus...");

        System::test_set_all_active(true);
        add_device(0x08, 172); // boiler: Enviline/Compress 6000AW/Hybrid 3000-7000iAW/SupraEco/Geo 5xx/WLW196i
        add_device(0x10, 158); // thermostat: RC310

        const auto & boiler_it = std::find_if(EMSESP::emsdevices.begin(), EMSESP::emsdevices.end(), [&](const std::unique_ptr<EMSdevice> & dev) {
            return dev && dev->device_id() == 0x08;
        });

        if (boiler_it == EMSESP::emsdevices.end()) {
            EMSESP::logger().err("ERROR - can not find mocked boiler device");
            return;
        }

        const auto & thermostat_it = std::find_if(EMSESP::emsdevices.begin(), EMSESP::emsdevices.end(), [&](const std::unique_ptr<EMSdevice> & dev) {
            return dev && dev->device_id() == 0x10;
        });

        if (thermostat_it == EMSESP::emsdevices.end()) {
            EMSESP::logger().err("ERROR - can not find mocked thermostat device");
            return;
        }

        const auto & boiler_dev     = *boiler_it;
        const auto & thermostat_dev = *thermostat_it;

        {
            auto test_int8 = [&](const std::unique_ptr<EMSdevice> & device, uint8_t tag, const std::string & shortname) {
                std::vector<uint16_t> modbus_regs(1);
                if (auto result = device->get_modbus_value(tag, shortname, modbus_regs)) {
                    shell.printf("INT8   %s FAILED (ERROR %d)\n", shortname.c_str(), result);
                } else {
                    shell.printf("INT8   %s: %d ", shortname.c_str(), (int8_t)modbus_regs[0]);
                    if ((int8_t)modbus_regs[0] == (int8_t)EMS_VALUE_DEFAULT_INT8_DUMMY)
                        shell.printfln("[OK]");
                    else
                        shell.printfln("[ERROR] - expected %d, got %d", (int8_t)EMS_VALUE_DEFAULT_INT8_DUMMY, (int8_t)modbus_regs[0]);
                }
            };

            auto test_uint8 = [&](const std::unique_ptr<EMSdevice> & device, uint8_t tag, const std::string & shortname) {
                std::vector<uint16_t> modbus_regs(1);
                if (auto result = device->get_modbus_value(tag, shortname, modbus_regs)) {
                    shell.printf("UINT8  %s FAILED (ERROR %d)\n", shortname.c_str(), result);
                } else {
                    shell.printf("UINT8  %s: %d ", shortname.c_str(), (uint8_t)modbus_regs[0]);
                    if ((uint8_t)modbus_regs[0] == (uint8_t)EMS_VALUE_DEFAULT_UINT8_DUMMY)
                        shell.printfln("[OK]");
                    else
                        shell.printfln("[ERROR] - expected %d, got %d", (uint8_t)EMS_VALUE_DEFAULT_UINT8_DUMMY, (uint8_t)modbus_regs[0]);
                }
            };

            auto test_int16 = [&](const std::unique_ptr<EMSdevice> & device, uint8_t tag, const std::string & shortname) {
                std::vector<uint16_t> modbus_regs(1);
                if (auto result = device->get_modbus_value(tag, shortname, modbus_regs)) {
                    shell.printf("INT16  %s FAILED (ERROR %d)\n", shortname.c_str(), result);
                } else {
                    shell.printf("INT16  %s: %d ", shortname.c_str(), (int16_t)modbus_regs[0]);
                    if ((int16_t)modbus_regs[0] == (int16_t)EMS_VALUE_DEFAULT_INT16_DUMMY)
                        shell.printfln("[OK]");
                    else
                        shell.printfln("[ERROR] - expected %d, got %d", (int16_t)EMS_VALUE_DEFAULT_INT16_DUMMY, (int16_t)modbus_regs[0]);
                }
            };

            auto test_uint16 = [&](const std::unique_ptr<EMSdevice> & device, uint8_t tag, const std::string & shortname) {
                std::vector<uint16_t> modbus_regs(1);
                if (auto result = device->get_modbus_value(tag, shortname, modbus_regs)) {
                    shell.printf("UINT16 %s FAILED (ERROR %d)\n", shortname.c_str(), result);
                } else {
                    shell.printf("UINT16 %s: %d ", shortname.c_str(), (uint16_t)modbus_regs[0]);
                    if ((uint16_t)modbus_regs[0] == (uint16_t)EMS_VALUE_DEFAULT_UINT16_DUMMY)
                        shell.printfln("[OK]");
                    else
                        shell.printfln("[ERROR] - expected %d, got %d", (uint16_t)EMS_VALUE_DEFAULT_UINT16_DUMMY, (uint16_t)modbus_regs[0]);
                }
            };

            auto test_uint24 = [&](const std::unique_ptr<EMSdevice> & device, uint8_t tag, const std::string & shortname) {
                std::vector<uint16_t> modbus_regs(2);
                if (auto result = device->get_modbus_value(tag, shortname, modbus_regs)) {
                    shell.printf("UINT24 %s FAILED (ERROR %d)\n", shortname.c_str(), result);
                } else {
                    uint32_t value = ((uint32_t)modbus_regs[0] << 16) | (uint32_t)modbus_regs[1];
                    shell.printf("UINT24 %s: %d ", shortname.c_str(), value);
                    if (value == (uint32_t)EMS_VALUE_DEFAULT_UINT24_DUMMY)
                        shell.printfln("[OK]");
                    else
                        shell.printfln("[ERROR] - expected %d, got %d", (uint32_t)EMS_VALUE_DEFAULT_UINT24_DUMMY, value);
                }
            };

            /* there seem to be no uint32 entities to run this test on.
            auto test_uint32 = [&](const std::unique_ptr<EMSdevice> & device, uint8_t tag, const std::string & shortname) {
                std::vector<uint16_t> modbus_regs(2);
                if (auto result = device->get_modbus_value(tag, shortname, modbus_regs)) {
                    shell.printf("UINT32  %s FAILED (ERROR %d)\n", shortname.c_str(), result);
                } else {
                    uint32_t value = ((uint32_t)modbus_regs[0] << 16) | (uint32_t)modbus_regs[1];
                    shell.printf("UINT32  %s: %d ", shortname.c_str(), value);
                    if (value == (uint32_t)EMS_VALUE_DEFAULT_UINT32_DUMMY)
                        shell.printfln("[OK]");
                    else
                        shell.printfln("[ERROR] - expected %d, got %d", (uint32_t)EMS_VALUE_DEFAULT_UINT32_DUMMY, value);
                }
            };
            */

            auto test_bool = [&](const std::unique_ptr<EMSdevice> & device, uint8_t tag, const std::string & shortname) {
                std::vector<uint16_t> modbus_regs(1);
                if (auto result = device->get_modbus_value(tag, shortname, modbus_regs)) {
                    shell.printf("BOOL   %s FAILED (ERROR %d)\n", shortname.c_str(), result);
                } else {
                    shell.printf("BOOL   %s: %d ", shortname.c_str(), (uint8_t)modbus_regs[0]);
                    if ((uint8_t)modbus_regs[0] == (uint8_t)EMS_VALUE_DEFAULT_BOOL_DUMMY)
                        shell.printfln("[OK]");
                    else
                        shell.printfln("[ERROR] - expected %d, got %d", (uint8_t)EMS_VALUE_DEFAULT_BOOL_DUMMY, (uint8_t)modbus_regs[0]);
                }
            };

            auto test_enum = [&](const std::unique_ptr<EMSdevice> & device, uint8_t tag, const std::string & shortname) {
                std::vector<uint16_t> modbus_regs(1);
                if (auto result = device->get_modbus_value(tag, shortname, modbus_regs)) {
                    shell.printf("ENUM   %s FAILED (ERROR %d)\n", shortname.c_str(), result);
                } else {
                    shell.printf("ENUM   %s: %d ", shortname.c_str(), (uint8_t)modbus_regs[0]);
                    if ((uint8_t)modbus_regs[0] == (uint8_t)EMS_VALUE_DEFAULT_ENUM_DUMMY)
                        shell.printfln("[OK]");
                    else
                        shell.printfln("[ERROR] - expected %d, got %d", (uint8_t)EMS_VALUE_DEFAULT_ENUM_DUMMY, (uint8_t)modbus_regs[0]);
                }
            };

            shell.println();
            shell.printfln("Testing device->get_modbus_value():");
            test_int8(boiler_dev, DeviceValueTAG::TAG_DEVICE_DATA, "mintempsilent");
            test_uint8(boiler_dev, DeviceValueTAG::TAG_DEVICE_DATA, "selflowtemp");
            test_int16(boiler_dev, DeviceValueTAG::TAG_DEVICE_DATA, "outdoortemp");
            test_uint16(boiler_dev, DeviceValueTAG::TAG_DEVICE_DATA, "rettemp");
            // test_uint32(device, DeviceValueTAG::TAG_DEVICE_DATA, "heatstarts"); // apparently there are no uint32 entities?
            test_uint24(boiler_dev, DeviceValueTAG::TAG_DEVICE_DATA, "heatstarts");
            test_bool(boiler_dev, DeviceValueTAG::TAG_DEVICE_DATA, "heatingactivated");
            test_enum(boiler_dev, DeviceValueTAG::TAG_DEVICE_DATA, "pumpmode");
        }

        // modbus_value_to_json
        {
            shell.println();
            shell.printfln("Testing device->modbus_value_to_json():");

            std::vector<uint8_t> modbus_bytes(2);
            JsonDocument         input;
            JsonObject           inputObject = input.to<JsonObject>();
            modbus_bytes[0]                  = 0;
            modbus_bytes[1]                  = EMS_VALUE_DEFAULT_UINT8_DUMMY;
            boiler_dev->modbus_value_to_json(DeviceValueTAG::TAG_DEVICE_DATA, "selflowtemp", modbus_bytes, inputObject);

            std::string jsonString;
            serializeJson(inputObject, jsonString);
            shell.printf("UINT8   %s: %s (%d) ", "selflowtemp", jsonString.c_str(), inputObject["value"].as<int>());

            if (inputObject["value"] == (uint8_t)EMS_VALUE_DEFAULT_UINT8_DUMMY)
                shell.println("[OK]");
            else
                shell.println("[ERROR]");
        }

        // handleRead
        {
            shell.println();
            shell.printfln("Testing modbus->handleRead():");

            uint16_t reg = Modbus::REGISTER_BLOCK_SIZE * DeviceValueTAG::TAG_DEVICE_DATA + 214; // mintempsilent is tag 2 (TAG_DEVICE_DATA), offset 214

            ModbusMessage request({boiler_dev->device_type(), 0x03, static_cast<unsigned char>(reg >> 8), static_cast<unsigned char>(reg & 0xff), 0, 1});
            auto          response = EMSESP::modbus_->handleRead(request);

            if (response.getError() == SUCCESS) {
                shell.print("mintempsilent MODBUS response:");
                for (const auto & d : response._data) {
                    shell.printf(" %d", d);
                }
                if (response._data.size() == 5 && response._data[3] == 0 && response._data[4] == (uint8_t)EMS_VALUE_DEFAULT_INT8_DUMMY) {
                    shell.printf(" [OK]");
                } else {
                    shell.printf(" [ERROR - invalid response]");
                }
                shell.println();
            } else {
                shell.printf("mintempsilent [MODBUS ERROR %d]\n", response.getError());
            }
        }

        // handleWrite boiler
        {
            shell.println();
            shell.printfln("Testing modbus->handleWrite() for boiler:");

            uint16_t reg = Modbus::REGISTER_BLOCK_SIZE * DeviceValueTAG::TAG_DEVICE_DATA + 4; // selflowtemp
            ModbusMessage request({boiler_dev->device_type(), 0x06, static_cast<unsigned char>(reg >> 8), static_cast<unsigned char>(reg & 0xff), 0, 1, 2, 0, 45});
            auto response = EMSESP::modbus_->handleWrite(request);

            if (response.getError() == SUCCESS) {
                shell.print("selflowtemp MODBUS response:");
                for (const auto & d : response._data) {
                    shell.printf(" %d", d);
                }
                shell.println(" [OK]");
            } else {
                shell.printf("selflowtemp [MODBUS ERROR %d]\n", response.getError());
            }
        }

        // handleWrite thermostat
        {
            shell.println();
            shell.printfln("Testing modbus->handleWrite() for thermostat:");

            uint16_t      reg = Modbus::REGISTER_BLOCK_SIZE * DeviceValueTAG::TAG_HC1 + 41; // remotetemp
            ModbusMessage request(
                {thermostat_dev->device_type(), 0x06, static_cast<unsigned char>(reg >> 8), static_cast<unsigned char>(reg & 0xff), 0, 1, 2, 0, 45});
            auto response = EMSESP::modbus_->handleWrite(request);

            if (response.getError() == SUCCESS) {
                shell.print("remotetemp MODBUS response:");
                for (const auto & d : response._data) {
                    shell.printf(" %d", d);
                }
                shell.println(" [OK]");
            } else {
                shell.printf("remotetemp [MODBUS ERROR %d]\n", response.getError());
            }
        }

        ok = true;
    }

    if (command == "poll2") {
        shell.printfln("Testing Tx Sending last message on queue...");

        EMSESP::show_ems(shell);

        uint8_t poll[1] = {0x8B};
        EMSESP::incoming_telegram(poll, 1);

        EMSESP::show_ems(shell);
        ok = true;
    }

    if (command == "rx2") {
        shell.printfln("Testing rx2...");

        uart_telegram({0x1B, 0x5B, 0xFD, 0x2D, 0x9E, 0x3A, 0xB6, 0xE5, 0x02, 0x20, 0x33, 0x30, 0x32, 0x3A, 0x20, 0x5B,
                       0x73, 0xFF, 0xFF, 0xCB, 0xDF, 0xB7, 0xA7, 0xB5, 0x67, 0x77, 0x77, 0xE4, 0xFF, 0xFD, 0x77, 0xFF});
        ok = true;
    }

    // https://github.com/emsesp/EMS-ESP/issues/380#issuecomment-633663007
    if (command == "rx3") {
        shell.printfln("Testing rx3...");

        uart_telegram({0x21, 0x0B, 0xFF, 0x00});
        ok = true;
    }

    // testing the UART tx command, without a queue
    if (command == "tx2") {
        shell.printfln("Testing tx2...");

        uint8_t t[] = {0x0B, 0x88, 0x18, 0x00, 0x20, 0xD4}; // including CRC
        EMSuart::transmit(t, sizeof(t));
        ok = true;
    }

    // send read request with offset
    if (command == "offset") {
        shell.printfln("Testing offset...");

        // send_read_request(0x18, 0x08);
        EMSESP::txservice_.read_request(0x18, 0x08, 27); // no offset
        ok = true;
    }

    if (command == "mixer") {
        shell.printfln("Testing Mixer...");

        test("mixer");

        // check for error "No telegram type handler found for ID 0x255 (src 0x20)"
        uart_telegram({0xA0, 0x00, 0xFF, 0x00, 0x01, 0x55, 0x00, 0x1A});

        shell.invoke_command("show values");
        shell.invoke_command("call mixer info");
        shell.invoke_command("call system publish");
        shell.invoke_command("show mqtt");

        // shell.invoke_command("call mixer dhw1 info");
        // shell.invoke_command("call mixer dhw2 info");

        // test API
        AsyncWebServerRequest request;
        request.url("/api/mixer");
        EMSESP::webAPIService.webAPIService(&request);
        request.url("/api/mixer/hc1/pumpstatus");
        EMSESP::webAPIService.webAPIService(&request);
        request.url("/api/mixer/dhw2/pumpstatus");
        EMSESP::webAPIService.webAPIService(&request);
        ok = true;
    }

    if (command == "crash") {
        shell.printfln("Forcing a crash...");
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdiv-by-zero"
#pragma GCC diagnostic ignored "-Wunused-variable"
        uint8_t a = 2 / 0;
        shell.printfln("Testing %s", a);
#pragma GCC diagnostic pop
        ok = true;
    }
#endif

    if (!ok) {
        shell.printfln("Unknown test %s", command.c_str());
        EMSESP::logger().notice("Unknown test %s", command.c_str());
    }
}

// loop console. simulates what EMSESP::loop() does
void Test::refresh() {
    uuid::loop();
    EMSESP::rxservice_.loop();
    EMSESP::mqtt_.loop();
    Shell::loop_all();
}

// simulates a telegram in the Rx queue, but without the CRC which is added automatically
void Test::rx_telegram(const std::vector<uint8_t> & rx_data) {
    uint8_t len = rx_data.size();
    uint8_t data[50];
    uint8_t i = 0;
    while (len--) {
        data[i] = rx_data[i];
        i++;
    }
    data[i] = EMSESP::rxservice_.calculate_crc(data, i);
    EMSESP::rxservice_.add(data, len + 1);

    refresh();
}

// simulates a telegram straight from UART, but without the CRC which is added automatically
void Test::uart_telegram(const std::vector<uint8_t> & rx_data) {
    uint8_t len = rx_data.size();
    uint8_t data[50];
    uint8_t i = 0;
    while (len--) {
        data[i] = rx_data[i];
        i++;
    }
    data[i] = EMSESP::rxservice_.calculate_crc(data, i);
    EMSESP::incoming_telegram(data, i + 1);

    refresh();
}

// takes raw string, assuming it contains the CRC. This is what is output from 'watch raw'
void Test::uart_telegram_withCRC(const char * rx_data) {
    // since the telegram data is a const, make a copy. add 1 to grab the \0 EOS
    char telegram[(EMS_MAX_TELEGRAM_LENGTH * 3) + 1];
    for (uint8_t i = 0; i < strlen(rx_data); i++) {
        telegram[i] = rx_data[i];
    }
    telegram[strlen(rx_data)] = '\0'; // make sure its terminated

    uint8_t count = 0;
    char *  p;
    char    value[10] = {0};

    uint8_t data[EMS_MAX_TELEGRAM_LENGTH];

    // get first value, which should be the src
    if ((p = strtok(telegram, " ,"))) { // delimiter
        strlcpy(value, p, 10);
        data[0] = (uint8_t)strtol(value, 0, 16);
    }

    // and iterate until end
    while (p != 0) {
        if ((p = strtok(nullptr, " ,"))) {
            strlcpy(value, p, 10);
            uint8_t val   = (uint8_t)strtol(value, 0, 16);
            data[++count] = val;
        }
    }

    if (count == 0) {
        return; // nothing to send
    }

    EMSESP::incoming_telegram(data, count + 1);

    refresh();
}

// takes raw string, adds CRC to end
void Test::uart_telegram(const char * rx_data) {
    // since the telegram data is a const, make a copy. add 1 to grab the \0 EOS
    char telegram[(EMS_MAX_TELEGRAM_LENGTH * 3) + 1];
    for (uint8_t i = 0; i < strlen(rx_data); i++) {
        telegram[i] = rx_data[i];
    }
    telegram[strlen(rx_data)] = '\0'; // make sure its terminated

    uint8_t count = 0;
    char *  p;
    char    value[10] = {0};

    uint8_t data[EMS_MAX_TELEGRAM_LENGTH];

    // get first value, which should be the src
    if ((p = strtok(telegram, " ,"))) { // delimiter
        strlcpy(value, p, 10);
        data[0] = (uint8_t)strtol(value, 0, 16);
    }

    // and iterate until end
    while (p != 0) {
        if ((p = strtok(nullptr, " ,"))) {
            strlcpy(value, p, 10);
            uint8_t val   = (uint8_t)strtol(value, 0, 16);
            data[++count] = val;
        }
    }

    if (count == 0) {
        return; // nothing to send
    }

    data[count + 1] = EMSESP::rxservice_.calculate_crc(data, count + 1); // add CRC

    EMSESP::incoming_telegram(data, count + 2);

    refresh();
}

void Test::add_device(uint8_t device_id, uint8_t product_id) {
    uart_telegram({device_id, EMSESP_DEFAULT_EMS_BUS_ID, EMSdevice::EMS_TYPE_VERSION, 0, product_id, 1, 0});
}

#ifdef EMSESP_TEST
#ifndef EMSESP_STANDALONE
void Test::listDir(fs::FS & fs, const char * dirname, uint8_t levels) {
    Serial.println();
    Serial.printf("%s\r\n", dirname);

    File root = fs.open(dirname);
    if (!root) {
        Serial.println("- failed to open directory");
        return;
    }
    if (!root.isDirectory()) {
        Serial.println(" - not a directory");
        return;
    }

    File file = root.openNextFile();
    while (file) {
        if (file.isDirectory()) {
            Serial.print(file.name());
            Serial.println("/");
            if (levels) {
                // prefix a / to the name to make it a full path
                listDir(fs, ("/" + String(file.name())).c_str(), levels - 1);
            }
            Serial.println();
        } else {
            Serial.print(" ");
            Serial.print(file.name());
            Serial.print(" (");
            Serial.print(file.size());
            Serial.println(" bytes)");
        }
        file = root.openNextFile();
    }
}
#endif
#endif

} // namespace emsesp

#endif
