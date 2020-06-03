// create some fake test data
// used with the 'test' command, under su/admin
void EMSESP::run_test(uuid::console::Shell & shell, const std::string & command) {
    if (command == "devices") {
        rxservice_.ems_mask(EMSbus::EMS_MASK_BUDERUS); // this is important otherwise nothing will be picked up!

        emsdevices.push_back(EMSFactory::add(EMSdevice::DeviceType::BOILER, EMSdevice::EMS_DEVICE_ID_BOILER, 0, "", "My Boiler", 0, 0));

        // A fake response - UBADevices(0x07)
        uint8_t t[] = {0x08, 0x00, 0x07, 0x00, 0x0B, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x47};
        rxservice_.add(t, sizeof(t));

        return;
    }

    if (command == "boiler2") {
        // question: do we need to set the mask?
        std::string version("1.2.3");
        add_device(0x08, 123, version, EMSdevice::Brand::BUDERUS); // Nefit Trendline

        return;
    }

    if (command == "unknown") {
        // question: do we need to set the mask?
        std::string version("1.2.3");
        add_device(0x09, 89, version, EMSdevice::Brand::BUDERUS);
        rxservice_.loop();

        // simulate getting version information back from an unknown device
        uint8_t t[] = {0x09, 0x0B, 0x02, 0x00, 0x59, 0x01, 0x02, 0x56};
        rxservice_.add(t, sizeof(t));
        rxservice_.loop();

        return;
    }

    if (command == "unknown2") {
        // simulate getting version information back from an unknown device
        uint8_t t[] = {0x09, 0x0B, 0x02, 0x00, 0x5A, 0x01, 0x02, 0x5A}; // product id is 90
        rxservice_.add(t, sizeof(t));
        rxservice_.loop();

        return;
    }

    if (command == "thermostats") {
        shell.printfln(F("Testing adding devices on the EMS bus..."));

        // create some fake devices
        std::string version("1.2.3");

        // add_device(0x10, 165, version, EMSdevice::Brand::BUDERUS);
        // add_device(0x17, 125, version, EMSdevice::Brand::BUDERUS); // test unknown class test
        // add_device(0x17, 93, version, EMSdevice::Brand::BUDERUS);
        // add_device(0x17, 254, version, EMSdevice::Brand::BUDERUS); // test unknown product_id

        add_device(0x18, 157, version, EMSdevice::Brand::BOSCH); // Bosch CR100 - https://github.com/proddy/EMS-ESP/issues/355

        return;
    }

    if (command == "solar") {
        shell.printfln(F("Testing Solar"));

        rxservice_.ems_mask(EMSbus::EMS_MASK_BUDERUS);

        std::string version("1.2.3");
        add_device(0x30, 163, version, EMSdevice::Brand::BUDERUS); // SM100

        rxservice_.loop();

        // SM100Monitor - type 0x0362 EMS+ - for SM100 and SM200
        // B0 0B FF 00 02 62 00 44 02 7A 80 00 80 00 80 00 80 00 80 00 80 00 00 7C 80 00 80 00 80 00 80
        uint8_t t[] = {0xB0, 0x0B, 0xFF, 00, 0x02, 0x62, 00, 0x44, 0x02, 0x7A, 0x80, 00, 0x80, 0x00, 0x80, 00,
                       0x80, 00,   0x80, 00, 0x80, 00,   00, 0x7C, 0x80, 00,   0x80, 00, 0x80, 00,   0x80, 0x89};
        rxservice_.add(t, sizeof(t));
        rxservice_.loop();

        shell.loop_all();

        return;
    }

    if (command == "cr100") {
        shell.printfln(F("Testing CR100"));

        rxservice_.ems_mask(EMSbus::EMS_MASK_HT3); // switch to junkers

        std::string version("1.2.3");
        add_device(0x18, 157, version, EMSdevice::Brand::BOSCH); // Bosch CR100 - https://github.com/proddy/EMS-ESP/issues/355
        rxservice_.loop();

        // RCPLUSStatusMessage_HC1(0x01A5)
        // 98 00 FF 00 01 A5 00 CF 21 2E 00 00 2E 24 03 25 03 03 01 03 25 00 C8 00 00 11 01 03
        uint8_t t[] = {0x98, 0x00, 0xFF, 0x00, 0x01, 0xA5, 0x00, 0xCF, 0x21, 0x2E, 0x00, 0x00, 0x2E, 0x24, 0x03,
                       0x25, 0x03, 0x03, 0x01, 0x03, 0x25, 0x00, 0xC8, 0x00, 0x00, 0x11, 0x01, 0x03, 0x13};
        rxservice_.add(t, sizeof(t));
        rxservice_.loop();

        shell.loop_all();
        rxservice_.loop();
        txservice_.flush_tx_queue();
        shell.loop_all();
        show_values(shell);

        shell.invoke_command("thermostat");
        shell.loop_all();

        // shell.invoke_command("set temp 20");
        shell.invoke_command("set mode auto");

        shell.loop_all();
        show_emsbus(shell);
        shell.loop_all();

        txservice_.send(); // send it to UART
        shell.loop_all();

        return;
    }

    if (command == "rx") {
        shell.printfln(F("Testing Rx..."));

        // fake telegrams. length includes CRC
        // Boiler -> Me, UBAMonitorFast(0x18), telegram: 08 00 18 00 00 02 5A 73 3D 0A 10 65 40 02 1A 80 00 01 E1 01 76 0E 3D 48 00 C9 44 02 00 (#data=25)
        uint8_t t1[] = {0x08, 0x00, 0x18, 0x00, 0x00, 0x02, 0x5A, 0x73, 0x3D, 0x0A, 0x10, 0x65, 0x40, 0x02, 0x1A,
                        0x80, 0x00, 0x01, 0xE1, 0x01, 0x76, 0x0E, 0x3D, 0x48, 0x00, 0xC9, 0x44, 0x02, 0x00, 0xFB};
        rxservice_.add(t1, sizeof(t1));

        // Boiler -> Thermostat, UBAParameterWW(0x33), telegram: 08 97 33 00 23 24 (#data=2)
        uint8_t t2[] = {0x08, 0x97, 0x33, 0x00, 0x23, 0x24, 0x5B};
        rxservice_.add(t2, sizeof(t2));

        // Boiler -> Me, UBAParameterWW(0x33), telegram: 08 0B 33 00 08 FF 34 FB 00 28 00 00 46 00 FF FF 00 (#data=13)
        uint8_t t21[] = {0x08, 0x0B, 0x33, 0x00, 0x08, 0xFF, 0x34, 0xFB, 0x00, 0x28, 0x00, 0x00, 0x46, 0x00, 0xFF, 0xFF, 0x00, 0x5F};
        rxservice_.add(t21, sizeof(t21));

        // write return code 01, 04
        uint8_t t3[] = {0x04};
        rxservice_.add(t3, sizeof(t3));

        // Thermostat -> Me, RC20StatusMessage(0x91), telegram: 17 0B 91 05 44 45 46 47 (#data=4)
        uint8_t t4[] = {0x17, 0x0B, 0x91, 0x05, 0x44, 0x45, 0x46, 0x47, 0x8E};
        rxservice_.add(t4, sizeof(t4));

        // bad CRC - corrupt telegram - CRC should be 0x8E
        uint8_t t5[] = {0x17, 0x0B, 0x91, 0x05, 0x44, 0x45, 0x46, 0x47, 0x99};
        rxservice_.add(t5, sizeof(t5));

        // simulating a Tx record
        uint8_t t7[] = {0x0B, 0x88, 0x07, 0x00, 0x20, 0xA8};
        rxservice_.add(t7, sizeof(t7));

        // Version Boiler
        uint8_t t8[] = {0x08, 0x0B, 0x02, 0x00, 0x7B, 0x06, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x43};
        rxservice_.add(t8, sizeof(t8));

        // Version Thermostat, device_id 0x11
        uint8_t t9[] = {0x11, 0x0B, 0x02, 0x00, 0x4D, 0x03, 0x03, 0x55};
        rxservice_.add(t9, sizeof(t9));

        // Thermostat -> all, telegram: 10 00 FF 00 01 A5 00 D7 21 00 00 00 00 30 01 84 01 01 03 01 84 01 F1 00 00 11 01 00 08 63 00
        // 0x1A5 test ems+
        uint8_t t10a[] = {0x10, 0x00, 0xFF, 0x00, 0x01, 0xA5, 0x00, 0xD7, 0x21, 0x00, 0x00, 0x00, 0x00, 0x30, 0x01, 0x84,
                          0x01, 0x01, 0x03, 0x01, 0x84, 0x01, 0xF1, 0x00, 0x00, 0x11, 0x01, 0x00, 0x08, 0x63, 0x00, 0xCC};
        rxservice_.add(t10a, sizeof(t10a));

        // setting temp from 21.5 to 19.9C
        uint8_t t10b[] = {0x10, 0x00, 0xFF, 0x00, 0x01, 0xA5, 0x00, 0xC7, 0x21, 0x00, 0x00, 0x00, 0x00, 0x30, 0x01, 0x84,
                          0x01, 0x01, 0x03, 0x01, 0x84, 0x01, 0xF1, 0x00, 0x00, 0x11, 0x01, 0x00, 0x08, 0x63, 0x00, 0x46};
        rxservice_.add(t10b, sizeof(t10b));

        // Thermostat -> Boiler, UBAFlags(0x35), telegram: 17 08 35 00 11 00 (#data=2)
        uint8_t t13[] = {0x17, 0x08, 0x35, 0x00, 0x11, 0x00, 0xC1};
        rxservice_.add(t13, sizeof(t13));

        // Thermostat -> Boiler, UBASetPoints(0x1A), telegram: 17 08 1A 00 00 00 00 00 (#data=4)
        uint8_t t14[] = {0x17, 0x08, 0x1A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3A};
        rxservice_.add(t14, sizeof(t14));

        // Thermostat -> Me, RC20Set(0xA8), telegram: 17 0B A8 00 01 00 FF F6 01 06 00 01 0D 01 00 FF FF 01 02 02 02 00 00 05 1F 05 1F 02 0E 00 FF (#data=27)
        uint8_t t15[] = {0x17, 0x0B, 0xA8, 0x00, 0x01, 0x00, 0xFF, 0xF6, 0x01, 0x06, 0x00, 0x01, 0x0D, 0x01, 0x00, 0xFF,
                         0xFF, 0x01, 0x02, 0x02, 0x02, 0x00, 0x00, 0x05, 0x1F, 0x05, 0x1F, 0x02, 0x0E, 0x00, 0xFF, 0xDF};
        rxservice_.add(t15, sizeof(t15));

        //  Boiler(0x08) -> All(0x00), UBAMonitorWW(0x34), data: 36 01 A5 80 00 21 00 00 01 00 01 3E 8D 03 77 91 00 80 00
        uint8_t t16[] = {0x08, 0x00, 0x34, 0x00, 0x36, 0x01, 0xA5, 0x80, 0x00, 0x21, 0x00, 0x00,
                         0x01, 0x00, 0x01, 0x3E, 0x8D, 0x03, 0x77, 0x91, 0x00, 0x80, 0x00, 0x3E};
        rxservice_.add(t16, sizeof(t16));

        return;
    }

    if (command == "send") {
        shell.printfln(F("Sending to Tx..."));
        show_emsbus(shell);
        txservice_.send(); // send it to UART
        return;
    }

    if (command == "tx") {
        shell.printfln(F("Testing Tx..."));

        txservice_.flush_tx_queue();

        // TX queue example - Me -> Thermostat, (0x91), telegram: 0B 17 91 05 44 45 46 47 (#data=4)
        // uint8_t t11[] = {0x44, 0x45, 0x46, 0x47};
        // txservice_.add(Telegram::Operation::TX_RAW, 0x17, 0x91, 0x05, t11, sizeof(t11));

        // TX - raw example test
        // uint8_t t12[] = {0x10, 0x08, 0x63, 0x04, 0x64};
        // txservice_.add(t12, sizeof(t12));

        // TX - sending raw string
        // txservice_.send_raw("10 08 63 03 64 65 66");

        // TX - send a read request
        send_read_request(0x18, 0x08);

        // TX - send a write request
        // uint8_t t18[] = {0x52, 0x79};
        // send_write_request(0x91, 0x17, 0x00, t18, sizeof(t18));

        console_.loop();
        show_emsbus(shell);
        txservice_.send(); // send it to UART

        txservice_.flush_tx_queue();

        return;
    }

    if (command == "poll") {
        shell.printfln(F("Testing Poll..."));

        // check if sending works when a poll comes in, with retries
        txservice_.flush_tx_queue();

        // simulate sending a read request
        // uint8_t t16[] = {0x44, 0x45, 0x46, 0x47}; // Me -> Thermostat, (0x91), telegram: 0B 17 91 05 44 45 46 47 (#data=4)
        // txservice_.add(Telegram::Operation::TX_RAW, 0x17, 0x91, 0x05, t16, sizeof(t16));
        send_read_request(0x91, 0x17);
        // txservice_.show_tx_queue();

        // Simulate adding a Poll, so read request is sent
        uint8_t poll[1] = {0x8B};
        incoming_telegram(poll, 1);

        // incoming Rx
        uint8_t t1[] = {0x17, 0x08, 0x1A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3A};
        incoming_telegram(t1, sizeof(t1));
        rxservice_.loop();

        // Simulate adding a Poll - should send retry
        incoming_telegram(poll, 1);

        show_emsbus(shell);
        uint8_t t2[] = {0x21, 0x22};
        send_write_request(0x91, 0x17, 0x00, t2, sizeof(t2), 0);
        show_emsbus(shell);

        incoming_telegram(t1, sizeof(t1));

        txservice_.flush_tx_queue();

        return;
    }

    if (command == "mqtt1") {
        shell.printfln(F("Testing MQTT..."));

        // MQTT test
        txservice_.flush_tx_queue();
        mqtt_.publish("boiler_cmd", "test me");
        // mqtt_.show_queue();
        // simulate an incoming mqtt msg
        char topic[Mqtt::MQTT_TOPIC_MAX_SIZE];
        char payload[100];

        strcpy(topic, "boiler_cmd");
        strcpy(payload, "12345");
        mqtt_.incoming(topic, payload);
        mqtt_.incoming(payload, payload); // should report error

        strcpy(topic, "thermostat_cmd_mode");
        strcpy(payload, "auto");
        mqtt_.incoming(topic, payload);

        strcpy(topic, "thermostat_cmd_temp");
        strcpy(payload, "20");
        mqtt_.incoming(topic, payload);

        strcpy(topic, "thermostat_cmd");
        strcpy(payload, "{\"cmd\":\"temp\",\"data\":22.52}");
        mqtt_.incoming(topic, payload);

        strcpy(topic, "boiler_cmd_wwtemp");
        strcpy(payload, "66");
        mqtt_.incoming(topic, payload);

        strcpy(topic, "thermostat_cmd");
        strcpy(payload, "{\"cmd\":\"temp\",\"hc\":2,\"data\":22}");
        mqtt_.incoming(topic, payload);

        strcpy(topic, "home/ems-esp/cmd");
        strcpy(payload, "restart");
        mqtt_.incoming(topic, payload);

        // txservice_.show_tx_queue();

        publish_all_values();

        return;
    }

    if (command == "poll2") {
        shell.printfln(F("Testing Tx Sending last message on queue..."));

        show_emsbus(shell);

        uint8_t poll[1] = {0x8B};
        incoming_telegram(poll, 1);

        rxservice_.loop();

        show_emsbus(shell);
        txservice_.flush_tx_queue();

        return;
    }

    if (command == "rx2") {
        // incoming Rx
        uint8_t t[] = {0x1B, 0x5B, 0xFD, 0x2D, 0x9E, 0x3A, 0xB6, 0xE5, 0x02, 0x20, 0x33, 0x30, 0x32, 0x3A, 0x20, 0x5B, 0x73,
                       0xFF, 0xFF, 0xCB, 0xDF, 0xB7, 0xA7, 0xB5, 0x67, 0x77, 0x77, 0xE4, 0xFF, 0xFD, 0x77, 0xFF, 0xD1};
        incoming_telegram(t, sizeof(t));
        return;
    }

    // https://github.com/proddy/EMS-ESP/issues/380#issuecomment-633663007
    if (command == "rx3") {
        // incoming Rx
        uint8_t t[] = {0x21, 0x0B, 0xFF, 0x00, 0xDA};
        incoming_telegram(t, sizeof(t));
        return;
    }

    if (command == "mqtt2") {
        // Mqtt::subscribe("cmd", std::bind(&EMSESP::dummy_mqtt_commands, this, _1));
        for (uint8_t i = 0; i < 30; i++) {
            Mqtt::subscribe("cmd", dummy_mqtt_commands);
        }
        return;
    }


    // testing the UART tx command, without a queue
    if (command == "tx2") {
        uint8_t t[] = {0x0B, 0x88, 0x18, 0x00, 0x20, 0xD4}; // including CRC
        EMSuart::transmit(t, sizeof(t));
        return;
    }

    // send read request with offset
    if (command == "offset") {
        // send_read_request(0x18, 0x08);
        txservice_.read_request(0x18, 0x08, 27); // no offset
        return;
    }

    if (command == "mixing") {
        shell.printfln(F("Mixing Solar"));

        rxservice_.ems_mask(EMSbus::EMS_MASK_BUDERUS);

        std::string version("1.2.3");
        add_device(0x28, 160, version, EMSdevice::Brand::BUDERUS); // MM100, WWC
        add_device(0x29, 161, version, EMSdevice::Brand::BUDERUS); // MM200, WWC

        rxservice_.loop();

        // WWC1 on 0x29
        uint8_t m1[] = {0xA9, 0x00, 0xFF, 0x00, 0x02, 0x32, 0x02, 0x6C, 0x00, 0x3C, 0x00, 0x3C, 0x3C, 0x46, 0x02, 0x03, 0x03, 0x00, 0x3C, 0x57};
        rxservice_.add(m1, sizeof(m1));
        rxservice_.loop();

        // WWC2 on 0x28
        uint8_t m2[] = {0xA8, 0x00, 0xFF, 0x00, 0x02, 0x31, 0x02, 0x35, 0x00, 0x3C, 0x00, 0x3C, 0x3C, 0x46, 0x02, 0x03, 0x03, 0x00, 0x3C, 0x71};
        rxservice_.add(m2, sizeof(m2));
        rxservice_.loop();

        shell.loop_all();

        return;
    }

    shell.printfln(F("[Test] Unknown test command"));
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

void EMSESP::dummy_mqtt_commands(const char * message) {
    //
}

#pragma GCC diagnostic pop
