
/*
 * EMS-ESP - https://github.com/proddy/EMS-ESP
 * Copyright 2019  Paul Derbyshire
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


#include "test.h"

namespace emsesp {

// create some fake test data
// used with the 'test' command, under su/admin
void Test::run_test(uuid::console::Shell & shell, const std::string & command) {
    if (command == "render") {
        /*
static constexpr uint8_t EMS_VALUE_BOOL     = 0xFF; // boolean
static constexpr uint8_t EMS_VALUE_BOOL_OFF = 0x00; // boolean false. True can be 0x01 or 0xFF sometimes.

static constexpr uint8_t  EMS_VALUE_BOOL_NOTSET    = 0xFE;   // random number for booleans, that's not 0, 1 or FF
static constexpr uint8_t  EMS_VALUE_UINT_NOTSET    = 0xFF;   // for 8-bit unsigned ints/bytes
static constexpr int8_t   EMS_VALUE_INT_NOTSET     = 0x7F;   // for signed 8-bit ints/bytes
static constexpr int16_t  EMS_VALUE_SHORT_NOTSET   = 0x8300; // -32000: for 2-byte signed shorts
static constexpr uint16_t EMS_VALUE_USHORT_NOTSET  = 0x7D00; //  32000: for 2-byte unsigned shorts
static constexpr uint16_t EMS_VALUE_USHORT_INVALID = 0x8000;
static constexpr int16_t  EMS_VALUE_SHORT_INVALID  = 0x8000;
static constexpr uint32_t EMS_VALUE_ULONG_NOTSET   = 0xFFFFFFFF; // for 3-byte and 4-byte longs
static constexpr uint32_t EMS_VALUE_ULONG_INVALID  = 0x80000000;
*/

        uint8_t  test1 = 12;
        int8_t   test2 = -12;
        uint16_t test3 = 456;
        int16_t  test4 = -456;
        uint8_t  test5 = 1; // bool = on
        uint32_t test6 = 305419896;
        float    test7 = 89.43;

        uint8_t  test1u = EMS_VALUE_UINT_NOTSET;
        int8_t   test2u = EMS_VALUE_INT_NOTSET;
        uint16_t test3u = EMS_VALUE_USHORT_NOTSET;
        int16_t  test4u = EMS_VALUE_USHORT_NOTSET;
        uint8_t  test5u = EMS_VALUE_BOOL_NOTSET;
        uint32_t test6u = EMS_VALUE_ULONG_NOTSET;

        EMSdevice::print_value(shell, 2, F("Selected flow temperature1"), test1, F_(degrees), 1);          // 12
        EMSdevice::print_value(shell, 2, F("Selected flow temperature2"), test2, F_(degrees), 1);          // -12
        EMSdevice::print_value(shell, 2, F("Selected flow temperature3"), test3, F_(degrees), 10);         // 45.6
        EMSdevice::print_value(shell, 2, F("Selected flow temperature4"), test4, F_(degrees), 10);         // -45.6
        EMSdevice::print_value(shell, 2, F("Selected flow temperature5"), test5, nullptr, EMS_VALUE_BOOL); // on
        EMSdevice::print_value(shell, 2, F("Selected flow temperature6"), test6, F_(degrees), 1);          //
        EMSdevice::print_value(shell, 2, F("Selected flow temperature7"), test7, F_(degrees), 2);          // 89.43
        EMSdevice::print_value(shell, 2, F("Warm Water comfort setting"), F("Intelligent"));
        char s[100];
        strcpy(s, "Not very intelligent");
        EMSdevice::print_value(shell, 2, F("Warm Water comfort setting2"), s);

        shell.println();

        EMSdevice::print_value(shell, 2, F("Selected flow temperature1u"), test1u, F_(degrees), 1);
        EMSdevice::print_value(shell, 2, F("Selected flow temperature2u"), test2u, F_(degrees), 1);
        EMSdevice::print_value(shell, 2, F("Selected flow temperature3u"), test3u, F_(degrees), 10);
        EMSdevice::print_value(shell, 2, F("Selected flow temperature4u"), test4u, F_(degrees), 10);
        EMSdevice::print_value(shell, 2, F("Selected flow temperature5u"), test5u, F_(degrees), EMS_VALUE_BOOL);
        EMSdevice::print_value(shell, 2, F("Selected flow temperature6u"), test6u, F_(degrees), 100);

        shell.loop_all();
        return;
    }

    if (command == "devices") {
        EMSESP::rxservice_.ems_mask(EMSbus::EMS_MASK_BUDERUS); // this is important otherwise nothing will be picked up!

        //emsdevices.push_back(EMSFactory::add(EMSdevice::DeviceType::BOILER, EMSdevice::EMS_DEVICE_ID_BOILER, 0, "", "My Boiler", 0, 0));

        // A fake response - UBADevices(0x07)
        rx_telegram({0x08, 0x00, 0x07, 0x00, 0x0B, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00});
    }

    if (command == "boiler2") {
        // question: do we need to set the mask?
        std::string version("1.2.3");
        EMSESP::add_device(0x08, 123, version, EMSdevice::Brand::BUDERUS); // Nefit Trendline
    }

    if (command == "unknown") {
        // question: do we need to set the mask?
        std::string version("1.2.3");
        EMSESP::add_device(0x09, 89, version, EMSdevice::Brand::BUDERUS);
        EMSESP::rxservice_.loop();

        // simulate getting version information back from an unknown device
        rx_telegram({0x09, 0x0B, 0x02, 0x00, 0x59, 0x01, 0x02});
    }

    if (command == "unknown2") {
        // simulate getting version information back from an unknown device
        rx_telegram({0x09, 0x0B, 0x02, 0x00, 0x5A, 0x01, 0x02}); // product id is 90 which doesn't exist
    }

    if ((command == "gateway") || (command == "g")) {
        // add 0x48 KM200, via a version command
        rx_telegram({0x48, 0x0B, 0x02, 0x00, 0xBD, 0x04, 0x06, 00, 00, 00, 00, 00, 00, 00});

        // Boiler(0x08) -> All(0x00), UBADevices(0x07), data: 09 01 00 00 00 00 00 00 01 00 00 00 00
        // check: make sure 0x48 is not detected again !
        rx_telegram({0x08, 0x00, 0x07, 0x00, 0x09, 01, 00, 00, 00, 00, 00, 00, 01, 00, 00, 00, 00});

        // add thermostat - Thermostat: RC300/RC310/Moduline 3000/CW400/Sense II (DeviceID:0x10, ProductID:158, Version:03.03) ** master device **
        std::string version("01.03");
        EMSESP::add_device(0x10, 158, version, EMSdevice::Brand::BUDERUS);
        EMSESP::rxservice_.loop();

        // simulate incoming telegram
        // Thermostat(0x10) -> 48(0x48), ?(0x26B), data: 6B 08 4F 00 00 00 02 00 00 00 02 00 03 00 03 00 03
        rx_telegram({0x10, 0x48, 0xFF, 00, 01, 0x6B, 00, 0x6B, 0x08, 0x4F, 00, 00, 00, 02, 00, 00, 00, 02, 00, 03, 00, 03, 00, 03});
    }

    if (command == "thermostat") {
        shell.printfln(F("Testing adding devices on the EMS bus..."));

        // create some fake devices
        std::string version("1.2.3");

        // add_device(0x10, 165, version, EMSdevice::Brand::BUDERUS);
        // add_device(0x17, 125, version, EMSdevice::Brand::BUDERUS); // test unknown class test
        // add_device(0x17, 93, version, EMSdevice::Brand::BUDERUS);
        // add_device(0x17, 254, version, EMSdevice::Brand::BUDERUS); // test unknown product_id

        EMSESP::add_device(0x18, 157, version, EMSdevice::Brand::BOSCH); // Bosch CR100 - https://github.com/proddy/EMS-ESP/issues/355
    }

    if (command == "solar") {
        shell.printfln(F("Testing Solar"));

        EMSESP::rxservice_.ems_mask(EMSbus::EMS_MASK_BUDERUS);

        std::string version("1.2.3");
        EMSESP::add_device(0x30, 163, version, EMSdevice::Brand::BUDERUS); // SM100

        EMSESP::rxservice_.loop();

        // SM100Monitor - type 0x0362 EMS+ - for SM100 and SM200
        // B0 0B FF 00 02 62 00 44 02 7A 80 00 80 00 80 00 80 00 80 00 80 00 00 7C 80 00 80 00 80 00 80
        rx_telegram({0xB0, 0x0B, 0xFF, 00, 0x02, 0x62, 00, 0x44, 0x02, 0x7A, 0x80, 00, 0x80, 0x00, 0x80, 00,
                     0x80, 00,   0x80, 00, 0x80, 00,   00, 0x7C, 0x80, 00,   0x80, 00, 0x80, 00,   0x80});
        EMSESP::show_values(shell);

        rx_telegram({0xB0, 0x0B, 0xFF, 0x00, 0x02, 0x62, 0x01, 0x44, 0x03, 0x30, 0x80, 00, 0x80, 00, 0x80, 00,
                     0x80, 00,   0x80, 00,   0x80, 00,   0x80, 00,   0x80, 00,   0x80, 00, 0x80, 00, 0x80, 0x33});
        EMSESP::show_values(shell);

        rx_telegram({0xB0, 00, 0xFF, 0x18, 02, 0x62, 0x80, 00, 0xB8});
        EMSESP::show_values(shell);

        EMSESP::send_raw_telegram("B0 00 FF 18 02 62 80 00 B8");
        EMSESP::show_values(shell);
    }

    if (command == "cr100") {
        shell.printfln(F("Testing CR100"));

        EMSESP::rxservice_.ems_mask(EMSbus::EMS_MASK_HT3); // switch to junkers

        std::string version("1.2.3");
        EMSESP::add_device(0x18, 157, version, EMSdevice::Brand::BOSCH); // Bosch CR100 - https://github.com/proddy/EMS-ESP/issues/355
        EMSESP::rxservice_.loop();

        // RCPLUSStatusMessage_HC1(0x01A5)
        // 98 00 FF 00 01 A5 00 CF 21 2E 00 00 2E 24 03 25 03 03 01 03 25 00 C8 00 00 11 01 03
        uart_telegram({0x98, 0x00, 0xFF, 0x00, 0x01, 0xA5, 0x00, 0xCF, 0x21, 0x2E, 0x00, 0x00, 0x2E, 0x24,
                       0x03, 0x25, 0x03, 0x03, 0x01, 0x03, 0x25, 0x00, 0xC8, 0x00, 0x00, 0x11, 0x01, 0x03});

        shell.loop_all();
        EMSESP::rxservice_.loop();
        EMSESP::txservice_.flush_tx_queue();
        shell.loop_all();
        EMSESP::show_values(shell);

        shell.invoke_command("thermostat");
        shell.loop_all();

        // shell.invoke_command("set temp 20");
        shell.invoke_command("set mode auto");

        shell.loop_all();
        EMSESP::show_emsbus(shell);
        shell.loop_all();

        EMSESP::txservice_.send(); // send it to UART
    }

    if (command == "rx") {
        shell.printfln(F("Testing Rx..."));

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
    }

    if (command == "send") {
        shell.printfln(F("Sending to Tx..."));
        EMSESP::show_emsbus(shell);
        EMSESP::txservice_.send(); // send it to UART
    }

    if (command == "tx") {
        shell.printfln(F("Testing Tx..."));

        EMSESP::txservice_.flush_tx_queue();

        // TX queue example - Me -> Thermostat, (0x91), telegram: 0B 17 91 05 44 45 46 47 (#data=4)
        // uint8_t t11[] = {0x44, 0x45, 0x46, 0x47};
        // EMSESP::txservice_.add(Telegram::Operation::TX_RAW, 0x17, 0x91, 0x05, t11, sizeof(t11));

        // TX - raw example test
        // uint8_t t12[] = {0x10, 0x08, 0x63, 0x04, 0x64};
        // EMSESP::txservice_.add(t12, sizeof(t12));

        // TX - sending raw string
        // EMSESP::txservice_.send_raw("10 08 63 03 64 65 66");

        // TX - send a read request
        EMSESP::send_read_request(0x18, 0x08);

        // TX - send a write request
        // uint8_t t18[] = {0x52, 0x79};
        // send_write_request(0x91, 0x17, 0x00, t18, sizeof(t18));

        shell.loop_all();
        EMSESP::show_emsbus(shell);
        EMSESP::txservice_.send(); // send it to UART

        EMSESP::txservice_.flush_tx_queue();
    }

    if (command == "poll") {
        shell.printfln(F("Testing Poll..."));

        // check if sending works when a poll comes in, with retries
        EMSESP::txservice_.flush_tx_queue();

        // simulate sending a read request
        // uint8_t t16[] = {0x44, 0x45, 0x46, 0x47}; // Me -> Thermostat, (0x91), telegram: 0B 17 91 05 44 45 46 47 (#data=4)
        // EMSESP::txservice_.add(Telegram::Operation::TX_RAW, 0x17, 0x91, 0x05, t16, sizeof(t16));
        EMSESP::send_read_request(0x91, 0x17);
        // EMSESP::txservice_.show_tx_queue();

        // Simulate adding a Poll, so read request is sent
        uint8_t poll[1] = {0x8B};
        EMSESP::incoming_telegram(poll, 1);

        // incoming Rx
        uart_telegram({0x17, 0x08, 0x1A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3A});

        // Simulate adding a Poll - should send retry
        EMSESP::incoming_telegram(poll, 1);

        EMSESP::show_emsbus(shell);
        uint8_t t2[] = {0x21, 0x22};
        EMSESP::send_write_request(0x91, 0x17, 0x00, t2, sizeof(t2), 0);
        EMSESP::show_emsbus(shell);

        EMSESP::txservice_.flush_tx_queue();
    }

    if (command == "mqtt1") {
        shell.printfln(F("Testing MQTT..."));

        // MQTT test
        EMSESP::txservice_.flush_tx_queue();
        EMSESP::EMSESP::mqtt_.publish("boiler_cmd", "test me");
        // EMSESP::mqtt_.show_queue();
        // simulate an incoming mqtt msg
        char topic[Mqtt::MQTT_TOPIC_MAX_SIZE];
        char payload[100];

        strcpy(topic, "boiler_cmd");
        strcpy(payload, "12345");
        EMSESP::mqtt_.incoming(topic, payload);
        EMSESP::mqtt_.incoming(payload, payload); // should report error

        strcpy(topic, "thermostat_cmd_mode");
        strcpy(payload, "auto");
        EMSESP::mqtt_.incoming(topic, payload);

        strcpy(topic, "thermostat_cmd_temp");
        strcpy(payload, "20");
        EMSESP::mqtt_.incoming(topic, payload);

        strcpy(topic, "thermostat_cmd");
        strcpy(payload, "{\"cmd\":\"temp\",\"data\":22.52}");
        EMSESP::mqtt_.incoming(topic, payload);

        strcpy(topic, "boiler_cmd_wwtemp");
        strcpy(payload, "66");
        EMSESP::mqtt_.incoming(topic, payload);

        strcpy(topic, "thermostat_cmd");
        strcpy(payload, "{\"cmd\":\"temp\",\"hc\":2,\"data\":22}");
        EMSESP::mqtt_.incoming(topic, payload);

        strcpy(topic, "home/ems-esp/cmd");
        strcpy(payload, "restart");
        EMSESP::mqtt_.incoming(topic, payload);

        // EMSESP::txservice_.show_tx_queue();

        EMSESP::publish_all_values();
    }

    if (command == "poll2") {
        shell.printfln(F("Testing Tx Sending last message on queue..."));

        EMSESP::show_emsbus(shell);

        uint8_t poll[1] = {0x8B};
        EMSESP::incoming_telegram(poll, 1);

        EMSESP::rxservice_.loop();

        EMSESP::show_emsbus(shell);
        EMSESP::txservice_.flush_tx_queue();
    }

    if (command == "rx2") {
        // incoming Rx
        uart_telegram({0x1B, 0x5B, 0xFD, 0x2D, 0x9E, 0x3A, 0xB6, 0xE5, 0x02, 0x20, 0x33, 0x30, 0x32, 0x3A, 0x20, 0x5B,
                       0x73, 0xFF, 0xFF, 0xCB, 0xDF, 0xB7, 0xA7, 0xB5, 0x67, 0x77, 0x77, 0xE4, 0xFF, 0xFD, 0x77, 0xFF});
    }

    // https://github.com/proddy/EMS-ESP/issues/380#issuecomment-633663007
    if (command == "rx3") {
        // incoming Rx
        uart_telegram({0x21, 0x0B, 0xFF, 0x00});
    }

    if (command == "mqtt2") {
        // Mqtt::subscribe("cmd", std::bind(&EMSESP::dummy_EMSESP::mqtt_commands, this, _1));
        for (uint8_t i = 0; i < 30; i++) {
            Mqtt::subscribe("cmd", dummy_mqtt_commands);
        }
    }

    // testing the UART tx command, without a queue
    if (command == "tx2") {
        uint8_t t[] = {0x0B, 0x88, 0x18, 0x00, 0x20, 0xD4}; // including CRC
        EMSuart::transmit(t, sizeof(t));
    }

    // send read request with offset
    if (command == "offset") {
        // send_read_request(0x18, 0x08);
        EMSESP::txservice_.read_request(0x18, 0x08, 27); // no offset
    }

    if (command == "mixing") {
        shell.printfln(F("Testing Mixing..."));

        EMSESP::rxservice_.ems_mask(EMSbus::EMS_MASK_BUDERUS);

        std::string version("1.2.3");
        EMSESP::add_device(0x28, 160, version, EMSdevice::Brand::BUDERUS); // MM100, WWC
        EMSESP::add_device(0x29, 161, version, EMSdevice::Brand::BUDERUS); // MM200, WWC

        EMSESP::add_device(0x20, 160, version, EMSdevice::Brand::BOSCH); // MM100

        EMSESP::rxservice_.loop();

        // WWC1 on 0x29
        rx_telegram({0xA9, 0x00, 0xFF, 0x00, 0x02, 0x32, 0x02, 0x6C, 0x00, 0x3C, 0x00, 0x3C, 0x3C, 0x46, 0x02, 0x03, 0x03, 0x00, 0x3C});

        // WWC2 on 0x28
        rx_telegram({0xA8, 0x00, 0xFF, 0x00, 0x02, 0x31, 0x02, 0x35, 0x00, 0x3C, 0x00, 0x3C, 0x3C, 0x46, 0x02, 0x03, 0x03, 0x00, 0x3C});

        // check for error "[emsesp] No telegram type handler found for ID 0x255 (src 0x20, dest 0x00)"
        rx_telegram({0xA0, 0x00, 0xFF, 0x00, 0x01, 0x55, 0x00, 0x1A});
    }

    // finally dump to console
    shell.loop_all();
}

// simulates a telegram in the Rx queue, but without the CRC which is added automatically
void Test::rx_telegram(const std::vector<uint8_t> & rx_data) {
    uint8_t len = rx_data.size();
    uint8_t data[50];
    std::copy(rx_data.begin(), rx_data.end(), data);
    data[len] = EMSESP::rxservice_.calculate_crc(rx_data.data(), len);
    EMSESP::rxservice_.add(data, len + 1);
    EMSESP::rxservice_.loop();
}

// simulates a telegram straight from UART, but without the CRC which is added automatically
void Test::uart_telegram(const std::vector<uint8_t> & rx_data) {
    uint8_t len = rx_data.size();
    uint8_t data[50];
    std::copy(rx_data.begin(), rx_data.end(), data);
    data[len] = EMSESP::rxservice_.calculate_crc(rx_data.data(), len);
    EMSESP::incoming_telegram(data, len + 1);
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

void Test::dummy_mqtt_commands(const char * message) {
    //
}

#pragma GCC diagnostic pop

} // namespace emsesp
