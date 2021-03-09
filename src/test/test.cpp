
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

#if defined(EMSESP_TEST)

#include "test.h"

// create some fake test data

namespace emsesp {

// no shell
bool Test::run_test(const char * command, int8_t id) {
    if ((command == nullptr) || (strlen(command) == 0)) {
        return false;
    }

    if (strcmp(command, "general") == 0) {
        EMSESP::logger().info(F("Testing general..."));

        add_device(0x08, 123); // Nefit Trendline
        add_device(0x18, 157); // Bosch CR100

        // add some data
        // Boiler -> Me, UBAMonitorFast(0x18), telegram: 08 00 18 00 00 02 5A 73 3D 0A 10 65 40 02 1A 80 00 01 E1 01 76 0E 3D 48 00 C9 44 02 00 (#data=25)
        uart_telegram({0x08, 0x00, 0x18, 0x00, 0x00, 0x02, 0x5A, 0x73, 0x3D, 0x0A, 0x10, 0x65, 0x40, 0x02, 0x1A, 0x80, 0x00, 0x01, 0xE1, 0x01, 0x76, 0x0E, 0x3D, 0x48, 0x00, 0xC9, 0x44, 0x02, 0x00});

        // Boiler -> Thermostat, UBAParameterWW(0x33), telegram: 08 97 33 00 23 24 (#data=2)
        uart_telegram({0x08, 0x98, 0x33, 0x00, 0x23, 0x24});

        // Boiler -> Me, UBAParameterWW(0x33), telegram: 08 0B 33 00 08 FF 34 FB 00 28 00 00 46 00 FF FF 00 (#data=13)
        uart_telegram({0x08, 0x0B, 0x33, 0x00, 0x08, 0xFF, 0x34, 0xFB, 0x00, 0x28, 0x00, 0x00, 0x46, 0x00, 0xFF, 0xFF, 0x00});

        // Thermostat RCPLUSStatusMessage_HC1(0x01A5)
        uart_telegram({0x98, 0x00, 0xFF, 0x00, 0x01, 0xA5, 0x00, 0xCF, 0x21, 0x2E, 0x00, 0x00, 0x2E, 0x24, 0x03, 0x25, 0x03, 0x03, 0x01, 0x03, 0x25, 0x00, 0xC8, 0x00, 0x00, 0x11, 0x01, 0x03});

        return true;
    }

    if (strcmp(command, "general2") == 0) {
        EMSESP::logger().info(F("Testing GB072/RC310..."));

        add_device(0x08, 123); // GB072
        add_device(0x10, 158); // RC310

        // add some data
        // Boiler -> Me, UBAMonitorFast(0x18), telegram: 08 00 18 00 00 02 5A 73 3D 0A 10 65 40 02 1A 80 00 01 E1 01 76 0E 3D 48 00 C9 44 02 00 (#data=25)
        uart_telegram({0x08, 0x00, 0x18, 0x00, 0x00, 0x02, 0x5A, 0x73, 0x3D, 0x0A, 0x10, 0x65, 0x40, 0x02, 0x1A, 0x80, 0x00, 0x01, 0xE1, 0x01, 0x76, 0x0E, 0x3D, 0x48, 0x00, 0xC9, 0x44, 0x02, 0x00});

        // Boiler -> Thermostat, UBAParameterWW(0x33), telegram: 08 97 33 00 23 24 (#data=2)
        uart_telegram({0x08, 0x90, 0x33, 0x00, 0x23, 0x24});

        // Boiler -> Me, UBAParameterWW(0x33), telegram: 08 0B 33 00 08 FF 34 FB 00 28 00 00 46 00 FF FF 00 (#data=13)
        uart_telegram({0x08, 0x0B, 0x33, 0x00, 0x08, 0xFF, 0x34, 0xFB, 0x00, 0x28, 0x00, 0x00, 0x46, 0x00, 0xFF, 0xFF, 0x00});

        // Thermostat 0x2A5 for HC1
        uart_telegram({0x10, 00, 0xFF, 00, 01, 0xA5, 0x80, 00, 01, 0x30, 0x28, 00, 0x30, 0x28, 01, 0x54, 03, 03, 01, 01, 0x54, 02, 0xA8, 00, 00, 0x11, 01, 03, 0xFF, 0xFF, 00});

        return true;
    }

    if (strcmp(command, "gateway") == 0) {
        EMSESP::logger().info(F("Testing gateway..."));

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

    if (strcmp(command, "mixer") == 0) {
        EMSESP::logger().info(F("Testing mixer..."));

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

    if (strcmp(command, "boiler") == 0) {
        EMSESP::logger().info(F("Testing boiler..."));
        add_device(0x08, 123); // Nefit Trendline

        // UBAuptime
        uart_telegram({0x08, 0x0B, 0x14, 00, 0x3C, 0x1F, 0xAC, 0x70});

        // Boiler -> Me, UBAMonitorFast(0x18), telegram: 08 00 18 00 00 02 5A 73 3D 0A 10 65 40 02 1A 80 00 01 E1 01 76 0E 3D 48 00 C9 44 02 00 (#data=25)
        uart_telegram({0x08, 0x00, 0x18, 0x00, 0x00, 0x02, 0x5A, 0x73, 0x3D, 0x0A, 0x10, 0x65, 0x40, 0x02, 0x1A, 0x80, 0x00, 0x01, 0xE1, 0x01, 0x76, 0x0E, 0x3D, 0x48, 0x00, 0xC9, 0x44, 0x02, 0x00});

        // Boiler -> Me, UBAParameterWW(0x33), telegram: 08 0B 33 00 08 FF 34 FB 00 28 00 00 46 00 FF FF 00 (#data=13)
        uart_telegram({0x08, 0x0B, 0x33, 0x00, 0x08, 0xFF, 0x34, 0xFB, 0x00, 0x28, 0x00, 0x00, 0x46, 0x00, 0xFF, 0xFF, 0x00});

        return true;
    }

    if (strcmp(command, "thermostat") == 0) {
        EMSESP::logger().info(F("Testing thermostat..."));

        add_device(0x10, 192); // FW120

        // HC1 - 3
        uart_telegram({0x90, 0x00, 0xFF, 0x00, 0x00, 0x6F, 0x03, 0x02, 0x00, 0xCD, 0x00, 0xE4});
        uart_telegram({0x90, 0x00, 0xFF, 0x00, 0x00, 0x70, 0x02, 0x01, 0x00, 0xCE, 0x00, 0xE5});
        uart_telegram({0x90, 0x00, 0xFF, 0x00, 0x00, 0x71, 0x01, 0x02, 0x00, 0xCF, 0x00, 0xE6});

        return true;
    }

    if (strcmp(command, "solar") == 0) {
        EMSESP::logger().info(F("Testing solar..."));

        add_device(0x30, 163); // SM100

        // SM100Monitor - type 0x0362 EMS+ - for SM100 and SM200
        uart_telegram({0xB0, 0x0B, 0xFF, 00, 0x02, 0x62, 00, 0x44, 0x02, 0x7A, 0x80, 00, 0x80, 0x00, 0x80, 00, 0x80, 00, 0x80, 00, 0x80, 00, 00, 0x7C, 0x80, 00, 0x80, 00, 0x80, 00, 0x80});

        uart_telegram({0xB0, 0x0B, 0xFF, 0x00, 0x02, 0x62, 0x01, 0x44, 0x03, 0x30, 0x80, 00, 0x80, 00, 0x80, 00, 0x80, 00, 0x80, 00, 0x80, 00, 0x80, 00, 0x80, 00, 0x80, 00, 0x80, 00, 0x80, 0x33});

        uart_telegram({0xB0, 00, 0xFF, 0x18, 02, 0x62, 0x80, 00, 0xB8});

        uart_telegram("30 00 FF 00 02 64 00 00 00 04 00 00 FF 00 00 1E 0B 09 64 00 00 00 00"); // SM100 modulation

        return true;
    }

    if (strcmp(command, "heatpump") == 0) {
        EMSESP::logger().info(F("Testing heatpump..."));

        add_device(0x38, 200); // Enviline module
        add_device(0x10, 192); // FW120 thermostat

        uart_telegram({0x90, 0x00, 0xFF, 0x00, 0x00, 0x6F, 0x03, 0x02, 0x00, 0xCD, 0x00, 0xE4});

        uart_telegram("38 0B FF 00 03 7B 0C 34 00 74");

        return true;
    }

    return false;
}

// used with the 'test' command, under su/admin
void Test::run_test(uuid::console::Shell & shell, const std::string & cmd) {
    // switch to su
    shell.add_flags(CommandFlags::ADMIN);

    // init stuff
    Mqtt::ha_enabled(true);
    Mqtt::dallas_format(1);
    Mqtt::ha_climate_format(1);
    EMSESP::rxservice_.ems_mask(EMSbus::EMS_MASK_BUDERUS);
    EMSESP::watch(EMSESP::Watch::WATCH_RAW); // raw

    std::string command(20, '\0');
    if ((cmd.empty()) || (cmd == "default")) {
        command = EMSESP_TEST_DEFAULT;
    } else {
        command = cmd;
    }

    if (command == "general") {
        shell.printfln(F("Testing adding a general boiler & thermostat..."));
        run_test("general");
        shell.invoke_command("show devices");
        shell.invoke_command("show");
        shell.invoke_command("call system publish");
        shell.invoke_command("show mqtt");
    }

    if (command == "render") {
        shell.printfln(F("Testing render..."));

        // check read_value to make sure it handles all the data type correctly
        uint8_t message_data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9}; // message_length is 9
        auto    telegram       = std::make_shared<Telegram>(Telegram::Operation::RX, 0x10, 0x11, 0x1234, 0, message_data, sizeof(message_data));

        uint8_t uint8b = EMS_VALUE_UINT_NOTSET;
        telegram->read_value(uint8b, 0);
        shell.printfln("uint8: expecting %02X, got:%02X", 1, uint8b);

        int8_t int8b = EMS_VALUE_INT_NOTSET;
        telegram->read_value(int8b, 0);
        shell.printfln("int8: expecting %02X, got:%02X", 1, int8b);

        uint16_t uint16b = EMS_VALUE_USHORT_NOTSET;
        telegram->read_value(uint16b, 1);
        shell.printfln("uint16: expecting %02X, got:%02X", 0x0203, uint16b);

        int16_t int16b = EMS_VALUE_SHORT_NOTSET;
        telegram->read_value(int16b, 1);
        shell.printfln("int16: expecting %02X, got:%02X", 0x0203, int16b);

        int16_t int16b8 = EMS_VALUE_SHORT_NOTSET;
        telegram->read_value(int16b8, 1, 1); // force to 1 byte
        shell.printfln("int16 1 byte: expecting %02X, got:%02X", 0x02, int16b8);

        uint32_t uint32b = EMS_VALUE_ULONG_NOTSET;
        telegram->read_value(uint32b, 1, 3);
        shell.printfln("uint32 3 bytes: expecting %02X, got:%02X", 0x020304, uint32b);

        uint32b = EMS_VALUE_ULONG_NOTSET;
        telegram->read_value(uint32b, 1);
        shell.printfln("uint32 4 bytes: expecting %02X, got:%02X", 0x02030405, uint32b);

        // check out of bounds
        uint16_t uint16 = EMS_VALUE_USHORT_NOTSET;
        telegram->read_value(uint16, 9);
        shell.printfln("uint16 out-of-bounds: was:%02X, new:%02X", EMS_VALUE_USHORT_NOTSET, uint16);
        uint8_t uint8oob = EMS_VALUE_UINT_NOTSET;
        telegram->read_value(uint8oob, 9);
        shell.printfln("uint8 out-of-bounds: was:%02X, new:%02X", EMS_VALUE_UINT_NOTSET, uint8oob);

        // check read bit
        uint8_t uint8bitb = EMS_VALUE_UINT_NOTSET;
        telegram->read_bitvalue(uint8bitb, 1, 1); // value is 0x02 = 0000 0010
        shell.printfln("uint8 bit read: expecting 1, got:%d", uint8bitb);
        uint8bitb = EMS_VALUE_UINT_NOTSET;
        telegram->read_bitvalue(uint8bitb, 0, 0); // value is 0x01 = 0000 0001
        shell.printfln("uint8 bit read: expecting 1, got:%d", uint8bitb);

        float test_float = 20.56;
        char  result[100];
        Helpers::render_value(result, test_float, 2);
        shell.printfln("Float test from %f to %s", test_float, result);
    }

    if (command == "devices") {
        shell.printfln(F("Testing devices..."));

        // A fake response - UBADevices(0x07)
        rx_telegram({0x08, 0x00, 0x07, 0x00, 0x0B, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00});
    }

    // check for boiler and controller on same product_id
    if (command == "double") {
        shell.printfln(F("Testing double..."));

        add_device(0x08, 206); // Nefit Excellent HR30
        add_device(0x09, 206); // Nefit Excellent HR30 Controller

        // UBAuptime
        uart_telegram({0x08, 0x0B, 0x14, 00, 0x3C, 0x1F, 0xAC, 0x70});
    }

    if (command == "620") {
        EMSESP::logger().info(F("Testing 620..."));

        // Version Controller
        uart_telegram({0x09, 0x0B, 0x02, 0x00, 0x5F, 0x06, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00});

        // Version Boiler
        uart_telegram({0x08, 0x0B, 0x02, 0x00, 0x5F, 0x06, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00});
    }

    // unknown device
    if (command == "unknown") {
        shell.printfln(F("Testing unknown..."));

        // add boiler
        add_device(0x08, 84);

        // add Controller - BC10 GB142 - but using the same product_id to see what happens
        add_device(0x09, 84);

        // simulate getting version information back from an unknown device
        // note there is no brand (byte 9)
        rx_telegram({0x09, 0x0B, 0x02, 0x00, 0x59, 0x09, 0x0a});

        shell.invoke_command("show devices");
        shell.invoke_command("call system report");
    }

    if (command == "unknown2") {
        shell.printfln(F("Testing unknown2..."));

        // simulate getting version information back from an unknown device
        rx_telegram({0x09, 0x0B, 0x02, 0x00, 0x5A, 0x01, 0x02}); // product id is 90 which doesn't exist
    }

    if (command == "gateway") {
        shell.printfln(F("Testing Gateway..."));
        run_test("gateway");
    }

    if (command == "web") {
        shell.printfln(F("Testing Web..."));

        add_device(0x08, 123); // Nefit Trendline
        add_device(0x18, 157); // Bosch CR100

        // add some data
        // Boiler -> Me, UBAMonitorFast(0x18), telegram: 08 00 18 00 00 02 5A 73 3D 0A 10 65 40 02 1A 80 00 01 E1 01 76 0E 3D 48 00 C9 44 02 00 (#data=25)
        uart_telegram({0x08, 0x00, 0x18, 0x00, 0x00, 0x02, 0x5A, 0x73, 0x3D, 0x0A, 0x10, 0x65, 0x40, 0x02, 0x1A, 0x80, 0x00, 0x01, 0xE1, 0x01, 0x76, 0x0E, 0x3D, 0x48, 0x00, 0xC9, 0x44, 0x02, 0x00});

        // Boiler -> Thermostat, UBAParameterWW(0x33), telegram: 08 97 33 00 23 24 (#data=2)
        uart_telegram({0x08, 0x98, 0x33, 0x00, 0x23, 0x24});

        // Boiler -> Me, UBAParameterWW(0x33), telegram: 08 0B 33 00 08 FF 34 FB 00 28 00 00 46 00 FF FF 00 (#data=13)
        uart_telegram({0x08, 0x0B, 0x33, 0x00, 0x08, 0xFF, 0x34, 0xFB, 0x00, 0x28, 0x00, 0x00, 0x46, 0x00, 0xFF, 0xFF, 0x00});

        // Thermostat RCPLUSStatusMessage_HC1(0x01A5)
        uart_telegram({0x98, 0x00, 0xFF, 0x00, 0x01, 0xA5, 0x00, 0xCF, 0x21, 0x2E, 0x00, 0x00, 0x2E, 0x24, 0x03, 0x25, 0x03, 0x03, 0x01, 0x03, 0x25, 0x00, 0xC8, 0x00, 0x00, 0x11, 0x01, 0x03});

        // time
        uart_telegram({0x98, 0x00, 0x06, 0x00, 0x00, 0x03, 0x04, 0x0C, 0x02, 0x33, 0x06, 00, 00, 00, 00, 00, 00});

        shell.invoke_command("show");
        shell.invoke_command("call boiler info");

        // test call
        DynamicJsonDocument doc(EMSESP_JSON_SIZE_XLARGE_DYN);
        JsonObject          json = doc.to<JsonObject>();
        (void)emsesp::Command::call(EMSdevice::DeviceType::BOILER, "info", nullptr, -1, json);
// bool has_data = emsesp::Command::call(EMSdevice::DeviceType::SYSTEM, "test", "boiler", -1, json);
#if defined(EMSESP_STANDALONE)
        Serial.print(COLOR_BRIGHT_MAGENTA);
        if (json.size() != 0) {
            serializeJson(doc, Serial);
        }
        shell.println();
        Serial.print(COLOR_RESET);
#endif

        for (const auto & emsdevice : EMSESP::emsdevices) {
            if (emsdevice) {
                if (emsdevice->unique_id() == 1) {
                    DynamicJsonDocument doc(EMSESP_JSON_SIZE_XLARGE_DYN);

                    JsonObject root = doc.to<JsonObject>();
                    emsdevice->generate_values_json_web(root);

#if defined(EMSESP_STANDALONE)
                    Serial.print(COLOR_BRIGHT_MAGENTA);
                    Serial.print("memoryUsage=");
                    Serial.print(doc.memoryUsage());
                    Serial.println();
                    Serial.print("measureMsgPack=");
                    Serial.print(measureMsgPack(doc));
                    Serial.println();
                    Serial.print("measureJson=");
                    Serial.print(measureJson(doc));
                    Serial.println();
                    serializeJson(doc, Serial);
                    Serial.print(COLOR_RESET);
                    Serial.println();
#endif
                }
            }
        }
        return;
    }

    if (command == "boiler") {
        shell.printfln(F("Testing boiler..."));
        run_test("boiler");
        shell.invoke_command("show devices");
        shell.invoke_command("show");
        // shell.invoke_command("call boiler info");
        // shell.invoke_command("call system publish");
        // shell.invoke_command("show mqtt");
    }

    if (command == "fr120") {
        shell.printfln(F("Testing adding a thermostat FR120..."));

        add_device(0x10, 191); // FR120 thermostat

        // HC1
        uart_telegram({0x90, 0x00, 0xFF, 0x00, 0x00, 0x6F, 0x01, 0x02, 0x00, 0xCF, 0x00, 0xE6});

        shell.invoke_command("show");
        shell.invoke_command("show devices");
    }

    if (command == "mqtt_nested") {
        shell.printfln(F("Testing nested MQTT"));
        Mqtt::ha_enabled(false); // turn off HA Discovery

        run_test("boiler");
        run_test("thermostat");
        run_test("solar");
        run_test("mixer");

        shell.invoke_command("call system publish");
        shell.invoke_command("show mqtt");
    }

    if (command == "thermostat") {
        shell.printfln(F("Testing adding a thermostat FW120..."));

        run_test("thermostat");
        shell.invoke_command("show");
        shell.invoke_command("call system publish");

        EMSESP::mqtt_.incoming("ems-esp/thermostat_hc1", "heat");
        EMSESP::mqtt_.incoming("ems-esp/thermostat_hc2", "28.8");
        EMSESP::mqtt_.incoming("ems-esp/thermostat", "{\"cmd\":\"temp\",\"id\":2,\"data\":22}");
        EMSESP::mqtt_.incoming("ems-esp/thermostat_hc3", "{\"cmd\":\"offsettemp\",\"data\":-3}");
        EMSESP::mqtt_.incoming("ems-esp/thermostat_hc3", "{\"cmd\":\"temp\",\"data\":-3}");

        shell.invoke_command("show mqtt");
    }

    if (command == "tc100") {
        shell.printfln(F("Testing adding a TC100 thermostat to the EMS bus..."));

        // add a thermostat
        add_device(0x18, 202); // Bosch TC100 - https://github.com/proddy/EMS-ESP/issues/474

        // 0x0A
        uart_telegram({0x98, 0x0B, 0x0A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00});
    }

    if (command == "solar") {
        shell.printfln(F("Testing Solar"));
        run_test("solar");

        uart_telegram("30 00 FF 0A 02 6A 04"); // SM100 pump on (1)sh
        EMSESP::show_device_values(shell);
        uart_telegram("30 00 FF 0A 02 6A 03"); // SM100 pump off (0)
        EMSESP::show_device_values(shell);
        shell.invoke_command("call system publish");

        // EMSESP::send_raw_telegram("B0 00 FF 18 02 62 80 00 B8");
    }

    if (command == "heatpump") {
        shell.printfln(F("Testing Heat Pump"));
        run_test("heatpump");
        shell.invoke_command("call");
        shell.invoke_command("call heatpump info");
    }

    if (command == "solar200") {
        shell.printfln(F("Testing Solar SM200"));

        add_device(0x30, 164); // SM200

        // SM100Monitor - type 0x0362 EMS+ - for SM100 and SM200
        // B0 0B FF 00 02 62 00 44 02 7A 80 00 80 00 80 00 80 00 80 00 80 00 00 7C 80 00 80 00 80 00 80
        rx_telegram({0xB0, 0x0B, 0xFF, 00, 0x02, 0x62, 00, 0x44, 0x02, 0x7A, 0x80, 00, 0x80, 0x00, 0x80, 00, 0x80, 00, 0x80, 00, 0x80, 00, 00, 0x7C, 0x80, 00, 0x80, 00, 0x80, 00, 0x80});

        rx_telegram({0xB0, 0x0B, 0xFF, 0x00, 0x02, 0x62, 0x01, 0x44, 0x03, 0x30, 0x80, 00, 0x80, 00, 0x80, 00, 0x80, 00, 0x80, 00, 0x80, 00, 0x80, 00, 0x80, 00, 0x80, 00, 0x80, 00, 0x80, 0x33});

        rx_telegram({0xB0, 00, 0xFF, 0x18, 02, 0x62, 0x80, 00, 0xB8});

        EMSESP::send_raw_telegram("B0 00 FF 18 02 62 80 00 B8");

        uart_telegram("30 00 FF 0A 02 6A 04");                                                 // SM100 pump on  1
        uart_telegram("30 00 FF 00 02 64 00 00 00 04 00 00 FF 00 00 1E 0B 09 64 00 00 00 00"); // SM100 modulation

        uart_telegram("30 00 FF 0A 02 6A 03"); // SM100 pump off  0

        shell.invoke_command("show");
    }

    if (command == "km") {
        shell.printfln(F("Testing KM200 Gateway"));

        add_device(0x10, 158); // RC300
        add_device(0x48, 189); // KM200

        // see https://github.com/proddy/EMS-ESP/issues/390

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
    }

    if (command == "cr100") {
        shell.printfln(F("Testing CR100"));

        add_device(0x18, 157); // Bosch CR100 - https://github.com/proddy/EMS-ESP/issues/355

        // RCPLUSStatusMessage_HC1(0x01A5)
        // 98 00 FF 00 01 A5 00 CF 21 2E 00 00 2E 24 03 25 03 03 01 03 25 00 C8 00 00 11 01 03 (no CRC)
        uart_telegram({0x98, 0x00, 0xFF, 0x00, 0x01, 0xA5, 0x00, 0xCF, 0x21, 0x2E, 0x00, 0x00, 0x2E, 0x24, 0x03, 0x25, 0x03, 0x03, 0x01, 0x03, 0x25, 0x00, 0xC8, 0x00, 0x00, 0x11, 0x01, 0x03});

        uart_telegram("98 00 FF 00 01 A5 00 CF 21 2E 00 00 2E 24 03 25 03 03 01 03 25 00 C8 00 00 11 01 03");            // without CRC
        uart_telegram_withCRC("98 00 FF 00 01 A6 00 CF 21 2E 00 00 2E 24 03 25 03 03 01 03 25 00 C8 00 00 11 01 03 6B"); // with CRC

        shell.loop_all();
        EMSESP::show_device_values(shell);

        shell.invoke_command("call thermostat mode auto");

        shell.loop_all();
        EMSESP::show_ems(shell);
        shell.loop_all();

        EMSESP::txservice_.send(); // send it to UART
    }

    if (command == "rx2") {
        shell.printfln(F("Testing Rx2..."));
        for (uint8_t i = 0; i < 30; i++) {
            uart_telegram({0x08, 0x0B, 0x33, 0x00, 0x08, 0xFF, 0x34, 0xFB, 0x00, 0x28, 0x00, 0x00, 0x46, 0x00, 0xFF, 0xFF, 0x00});
        }
    }

    if (command == "rx") {
        shell.printfln(F("Testing Rx..."));

        // fake telegrams. length includes CRC
        // Boiler -> Me, UBAMonitorFast(0x18), telegram: 08 00 18 00 00 02 5A 73 3D 0A 10 65 40 02 1A 80 00 01 E1 01 76 0E 3D 48 00 C9 44 02 00 (#data=25)
        uart_telegram({0x08, 0x00, 0x18, 0x00, 0x00, 0x02, 0x5A, 0x73, 0x3D, 0x0A, 0x10, 0x65, 0x40, 0x02, 0x1A, 0x80, 0x00, 0x01, 0xE1, 0x01, 0x76, 0x0E, 0x3D, 0x48, 0x00, 0xC9, 0x44, 0x02, 0x00});

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
        uart_telegram({0x10, 0x00, 0xFF, 0x00, 0x01, 0xA5, 0x00, 0xD7, 0x21, 0x00, 0x00, 0x00, 0x00, 0x30, 0x01, 0x84, 0x01, 0x01, 0x03, 0x01, 0x84, 0x01, 0xF1, 0x00, 0x00, 0x11, 0x01, 0x00, 0x08, 0x63, 0x00});

        // setting temp from 21.5 to 19.9C
        uart_telegram({0x10, 0x00, 0xFF, 0x00, 0x01, 0xA5, 0x00, 0xC7, 0x21, 0x00, 0x00, 0x00, 0x00, 0x30, 0x01, 0x84, 0x01, 0x01, 0x03, 0x01, 0x84, 0x01, 0xF1, 0x00, 0x00, 0x11, 0x01, 0x00, 0x08, 0x63, 0x00});

        // Thermostat -> Boiler, UBAFlags(0x35), telegram: 17 08 35 00 11 00 (#data=2)
        uart_telegram({0x17, 0x08, 0x35, 0x00, 0x11, 0x00});

        // Thermostat -> Boiler, UBASetPoints(0x1A), telegram: 17 08 1A 00 00 00 00 00 (#data=4)
        uart_telegram({0x17, 0x08, 0x1A, 0x00, 0x00, 0x00, 0x00, 0x00});

        // Thermostat -> Me, RC20Set(0xA8), telegram: 17 0B A8 00 01 00 FF F6 01 06 00 01 0D 01 00 FF FF 01 02 02 02 00 00 05 1F 05 1F 02 0E 00 FF (#data=27)
        uart_telegram({0x17, 0x0B, 0xA8, 0x00, 0x01, 0x00, 0xFF, 0xF6, 0x01, 0x06, 0x00, 0x01, 0x0D, 0x01, 0x00, 0xFF, 0xFF, 0x01, 0x02, 0x02, 0x02, 0x00, 0x00, 0x05, 0x1F, 0x05, 0x1F, 0x02, 0x0E, 0x00, 0xFF});

        //  Boiler(0x08) -> All(0x00), UBAMonitorWW(0x34), data: 36 01 A5 80 00 21 00 00 01 00 01 3E 8D 03 77 91 00 80 00
        uart_telegram({0x08, 0x00, 0x34, 0x00, 0x36, 0x01, 0xA5, 0x80, 0x00, 0x21, 0x00, 0x00, 0x01, 0x00, 0x01, 0x3E, 0x8D, 0x03, 0x77, 0x91, 0x00, 0x80, 0x00});

        // test 0x2A - DHWStatus3
        uart_telegram({0x88, 00, 0x2A, 00, 00, 00, 00, 00, 00, 00, 00, 00, 0xD2, 00, 00, 0x80, 00, 00, 01, 0x9D, 0x80, 0x00, 0x02, 0x79, 00});
    }

    if (command == "tx") {
        shell.printfln(F("Testing Tx..."));

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
        const uint8_t t13[] = {0x90, 0x0B, 0xFF, 00, 01, 0xBA, 00, 0x2E, 0x2A, 0x26, 0x1E, 0x03, 00, 0xFF, 0xFF, 05, 0x2A, 01, 0xE1, 0x20, 0x01, 0x0F, 05, 0x2A};
        EMSESP::txservice_.add(Telegram::Operation::TX_RAW, t13, sizeof(t13), 0);

        // EMS+ Junkers read request
        EMSESP::send_read_request(0x16F, 0x10);

        EMSESP::show_ems(shell);

        // process whole Tx queue
        for (uint8_t i = 0; i < 10; i++) {
            EMSESP::txservice_.send(); // send it to UART
        }
    }

    if (command == "poll") {
        shell.printfln(F("Testing Poll..."));

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
    }

    if (command == "cmd") {
        shell.printfln(F("Testing Commands..."));

        // add a thermostat with 3 HCs
        add_device(0x10, 192); // FW120

        // HC1 - 3
        uart_telegram({0x90, 0x00, 0xFF, 0x00, 0x00, 0x6F, 0x03, 0x02, 0x00, 0xCD, 0x00, 0xE4});
        uart_telegram({0x90, 0x00, 0xFF, 0x00, 0x00, 0x70, 0x02, 0x01, 0x00, 0xCE, 0x00, 0xE5});
        uart_telegram({0x90, 0x00, 0xFF, 0x00, 0x00, 0x71, 0x01, 0x02, 0x00, 0xCF, 0x00, 0xE6});

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
    }

    if (command == "pin") {
        shell.printfln(F("Testing pin..."));
        shell.invoke_command("call system pin");
        shell.invoke_command("call system pin 1 true");
    }

    if (command == "mqtt2") {
        shell.printfln(F("Testing MQTT large payloads..."));

        DynamicJsonDocument doc(EMSESP_JSON_SIZE_XXLARGE_DYN);

        char key[6];
        char value[6];

        // fit it up, to its limit of the Json buffer (which is about 169 records)
        for (uint8_t i = 0; i < 200; i++) {
            snprintf_P(key, 7, PSTR("key%03d"), i);
            snprintf_P(value, 7, PSTR("val%03d"), i);
            doc[key] = value;
        }
        doc.shrinkToFit();
        JsonObject jo = doc.as<JsonObject>();
        shell.printfln(F("Size of JSON payload = %d"), jo.memoryUsage());
        shell.printfln(F("Length of JSON payload = %d"), measureJson(jo));

        Mqtt::publish("test", jo);
        Mqtt::show_mqtt(shell); // show queue
    }

    if (command == "mqtt") {
        shell.printfln(F("Testing MQTT..."));

        // add a boiler
        add_device(0x08, 123); // Nefit Trendline

        // add a thermostat
        add_device(0x18, 157); // Bosch CR100 - https://github.com/proddy/EMS-ESP/issues/355

        // RCPLUSStatusMessage_HC1(0x01A5) - HC1
        uart_telegram({0x98, 0x00, 0xFF, 0x00, 0x01, 0xA5, 0x00, 0xCF, 0x21, 0x2E, 0x00, 0x00, 0x2E, 0x24, 0x03, 0x25, 0x03, 0x03, 0x01, 0x03, 0x25, 0x00, 0xC8, 0x00, 0x00, 0x11, 0x01, 0x03});
        uart_telegram("98 00 FF 00 01 A5 00 CF 21 2E 00 00 2E 24 03 25 03 03 01 03 25 00 C8 00 00 11 01 03");            // without CRC
        uart_telegram_withCRC("98 00 FF 00 01 A5 00 CF 21 2E 00 00 2E 24 03 25 03 03 01 03 25 00 C8 00 00 11 01 03 13"); // with CRC

        shell.loop_all();

        char boiler_topic[Mqtt::MQTT_TOPIC_MAX_SIZE];
        char thermostat_topic[Mqtt::MQTT_TOPIC_MAX_SIZE];
        char system_topic[Mqtt::MQTT_TOPIC_MAX_SIZE];
        EMSESP::EMSESP::mqtt_.publish("boiler", "test me");
        Mqtt::show_mqtt(shell); // show queue

        strcpy(boiler_topic, "ems-esp/boiler");
        strcpy(thermostat_topic, "ems-esp/thermostat");
        strcpy(system_topic, "ems-esp/system");

        EMSESP::mqtt_.incoming(boiler_topic, ""); // test if ignore empty payloads                         // invalid format

        EMSESP::mqtt_.incoming(boiler_topic, "12345");                                // invalid format
        EMSESP::mqtt_.incoming("bad_topic", "12345");                                 // no matching topic
        EMSESP::mqtt_.incoming(boiler_topic, "{\"cmd\":\"garbage\",\"data\":22.52}"); // should report error

        EMSESP::mqtt_.incoming(boiler_topic, "{\"cmd\":\"comfort\",\"data\":\"eco\"}");
        EMSESP::mqtt_.incoming(boiler_topic, "{\"cmd\":\"wwactivated\",\"data\":\"1\"}"); // with quotes
        EMSESP::mqtt_.incoming(boiler_topic, "{\"cmd\":\"wwactivated\",\"data\":1}");     // without quotes
        EMSESP::mqtt_.incoming(boiler_topic, "{\"cmd\":\"flowtemp\",\"data\":55}");

        EMSESP::mqtt_.incoming(system_topic, "{\"cmd\":\"send\",\"data\":\"01 02 03 04 05\"}");
        EMSESP::mqtt_.incoming(system_topic, "{\"cmd\":\"pin\",\"id\":12,\"data\":\"1\"}");

        EMSESP::mqtt_.incoming(thermostat_topic, "{\"cmd\":\"wwmode\",\"data\":\"auto\"}");
        EMSESP::mqtt_.incoming(thermostat_topic, "{\"cmd\":\"control\",\"data\":\"1\"}");          // RC35 only, should error
        EMSESP::mqtt_.incoming(thermostat_topic, "{\"cmd\":\"mode\",\"data\":\"poep\",\"id\":2}"); // invalid mode
        EMSESP::mqtt_.incoming(thermostat_topic, "{\"cmd\":\"mode\",\"data\":\"auto\",\"id\":2}");
        EMSESP::mqtt_.incoming(thermostat_topic, "{\"cmd\":\"mode\",\"data\":\"auto\",\"hc\":2}");     // hc as number
        EMSESP::mqtt_.incoming(thermostat_topic, "{\"cmd\":\"temp\",\"data\":19.5,\"hc\":1}");         // data as number
        EMSESP::mqtt_.incoming(thermostat_topic, "{\"cmd\":\"mode\",\"data\":\"auto\",\"hc\":\"2\"}"); // hc as string. should error as no hc2
        EMSESP::mqtt_.incoming(thermostat_topic, "{\"cmd\":\"temp\",\"data\":22.56}");
        EMSESP::mqtt_.incoming(thermostat_topic, "{\"cmd\":\"temp\",\"data\":22}");
        EMSESP::mqtt_.incoming(thermostat_topic, "{\"cmd\":\"temp\",\"data\":\"22.56\"}");
        EMSESP::mqtt_.incoming(thermostat_topic, "{\"cmd\":\"temp\",\"id\":2,\"data\":22}");

        // test single commands
        EMSESP::mqtt_.incoming(thermostat_topic, "auto");
        EMSESP::mqtt_.incoming(thermostat_topic, "heat");
        EMSESP::mqtt_.incoming(thermostat_topic, "28.8");

        EMSESP::publish_all(true);

        Mqtt::resubscribe();
        Mqtt::show_mqtt(shell); // show queue
    }

    if (command == "poll2") {
        shell.printfln(F("Testing Tx Sending last message on queue..."));

        EMSESP::show_ems(shell);

        uint8_t poll[1] = {0x8B};
        EMSESP::incoming_telegram(poll, 1);

        EMSESP::show_ems(shell);
    }

    if (command == "rx2") {
        shell.printfln(F("Testing rx2..."));

        uart_telegram({0x1B, 0x5B, 0xFD, 0x2D, 0x9E, 0x3A, 0xB6, 0xE5, 0x02, 0x20, 0x33, 0x30, 0x32, 0x3A, 0x20, 0x5B, 0x73, 0xFF, 0xFF, 0xCB, 0xDF, 0xB7, 0xA7, 0xB5, 0x67, 0x77, 0x77, 0xE4, 0xFF, 0xFD, 0x77, 0xFF});
    }

    // https://github.com/proddy/EMS-ESP/issues/380#issuecomment-633663007
    if (command == "rx3") {
        shell.printfln(F("Testing rx3..."));

        uart_telegram({0x21, 0x0B, 0xFF, 0x00});
    }

    // testing the UART tx command, without a queue
    if (command == "tx2") {
        shell.printfln(F("Testing tx2..."));

        uint8_t t[] = {0x0B, 0x88, 0x18, 0x00, 0x20, 0xD4}; // including CRC
        EMSuart::transmit(t, sizeof(t));
    }

    // send read request with offset
    if (command == "offset") {
        shell.printfln(F("Testing offset..."));

        // send_read_request(0x18, 0x08);
        EMSESP::txservice_.read_request(0x18, 0x08, 27); // no offset
    }

    if (command == "mixer") {
        shell.printfln(F("Testing Mixer..."));

        run_test("mixer");

        // check for error "No telegram type handler found for ID 0x255 (src 0x20)"
        uart_telegram({0xA0, 0x00, 0xFF, 0x00, 0x01, 0x55, 0x00, 0x1A});

        shell.invoke_command("show");
        shell.invoke_command("call mixer info");
        shell.invoke_command("call system publish");
        shell.invoke_command("show mqtt");
    }
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

#if defined(EMSESP_STANDALONE)
    EMSESP::loop();
#endif
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

#if defined(EMSESP_STANDALONE)
    EMSESP::loop();
#endif
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

#if defined(EMSESP_STANDALONE)
    EMSESP::loop();
#endif
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

#if defined(EMSESP_STANDALONE)
    EMSESP::loop();
#endif
}

// Sends version telegram. Version is hardcoded to 1.0
void Test::add_device(uint8_t device_id, uint8_t product_id) {
    // Send version: 09 0B 02 00 PP V1 V2
    uart_telegram({device_id, EMSESP_DEFAULT_EMS_BUS_ID, EMSdevice::EMS_TYPE_VERSION, 0, product_id, 1, 0});
}

#ifndef EMSESP_STANDALONE
void Test::listDir(fs::FS & fs, const char * dirname, uint8_t levels) {
    Serial.println();
    Serial.printf("Listing directory: %s\r\n", dirname);

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
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if (levels) {
                listDir(fs, file.name(), levels - 1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("\tSIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}
#endif

void Test::debug(uuid::console::Shell & shell, const std::string & cmd) {
    // shell.add_flags(CommandFlags::ADMIN); // switch to su

    std::string command(20, '\0');
    if ((cmd.empty()) || (cmd == "default")) {
        command = "ls";
    } else {
        command = cmd;
    }

#ifndef EMSESP_STANDALONE
    if (command == "ls") {
        listDir(LITTLEFS, "/", 3);
    }
#endif
}

} // namespace emsesp

#endif
