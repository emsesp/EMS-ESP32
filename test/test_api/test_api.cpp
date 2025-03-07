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

#include <Arduino.h>

#include <unity.h>

#include <emsesp.h>
#include "ESPAsyncWebServer.h"
#include "web/WebAPIService.h"
#include "test_shuntingYard.hpp"

using namespace emsesp;

// forward declarations
void         run_tests();
const char * call_url(const char * url);

AsyncWebServer * webServer;
ESP32React *     esp32React;
WebAPIService *  webAPIService;
EMSESP           application;
FS               dummyFS;

std::shared_ptr<emsesp::EMSESPConsole> shell;
char                                   output_buffer[4096];

class TestStream : public Stream {
  public:
    int available() override {
        return 1;
    }
    int read() override {
        return '\n';
    };
    int peek() override {
        return '\n';
    };
    size_t write(uint8_t data __attribute__((unused))) override {
        return 1;
    }
    size_t write(const uint8_t * buffer __attribute__((unused)), size_t size) override {
        strcat(output_buffer, (const char *)buffer); // store output in our temp buffer, strings only
        return size;
    }
    void flush() override {
        output_buffer[0] = '\0'; // empty the temp buffer
    }
};
static TestStream stream;

// load the tests
// this is generated from this file when compiled with -DEMSESP_UNITY_CREATE
// copy the output to the test_api.h file
#include "test_api.h"

// Unity's setup call - is called before each test - empty for now
void setUp() {
}

// Unity's teardown call - is called after each test - empty for now
void tearDown() {
}

// simulates a telegram straight from UART, but without the CRC which is added automatically
void uart_telegram(const std::vector<uint8_t> & rx_data) {
    uint8_t len = rx_data.size();
    uint8_t data[50];
    uint8_t i = 0;
    while (len--) {
        data[i] = rx_data[i];
        i++;
    }
    data[i] = EMSESP::rxservice_.calculate_crc(data, i);
    EMSESP::incoming_telegram(data, i + 1);
}

// simulates a telegram from a "string" of hex values
void uart_telegram(const char * rx_data) {
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
}

// add an EMS device and register it
void add_device(uint8_t device_id, uint8_t product_id) {
    uart_telegram({device_id, 0x0B, EMSdevice::EMS_TYPE_VERSION, 0, product_id, 1, 0});
}

// add our EMS test devices
void add_devices() {
    //
    // boiler
    //
    add_device(0x08, 123); // Nefit Trendline

    // UBAuptime
    uart_telegram({0x08, 0x0B, 0x14, 00, 0x3C, 0x1F, 0xAC, 0x70});

    // Boiler -> Me, UBAMonitorFast(0x18), telegram: 08 00 18 00 00 02 5A 73 3D 0A 10 65 40 02 1A 80 00 01 E1 01 76 0E 3D 48 00 C9 44 02 00 (#data=25)
    uart_telegram({0x08, 0x00, 0x18, 0x00, 0x00, 0x02, 0x5A, 0x73, 0x3D, 0x0A, 0x10, 0x65, 0x40, 0x02, 0x1A,
                   0x80, 0x00, 0x01, 0xE1, 0x01, 0x76, 0x0E, 0x3D, 0x48, 0x00, 0xC9, 0x44, 0x02, 0x00});

    // Boiler -> Me, UBAParameterWW(0x33), telegram: 08 0B 33 00 08 FF 34 FB 00 28 00 00 46 00 FF FF 00 (#data=13)
    uart_telegram({0x08, 0x0B, 0x33, 0x00, 0x08, 0xFF, 0x34, 0xFB, 0x00, 0x28, 0x00, 0x00, 0x46, 0x00, 0xFF, 0xFF, 0x00});

    //
    // thermostat
    //
    add_device(0x10, 192); // FW120

    // HC1 - 3
    uart_telegram({0x90, 0x00, 0xFF, 0x00, 0x00, 0x6F, 0x03, 0x02, 0x00, 0xCD, 0x00, 0xE4});
    uart_telegram({0x90, 0x00, 0xFF, 0x00, 0x00, 0x70, 0x02, 0x01, 0x00, 0xCE, 0x00, 0xE5});
    uart_telegram({0x90, 0x00, 0xFF, 0x00, 0x00, 0x71, 0x01, 0x02, 0x00, 0xCF, 0x00, 0xE6});

    // send the telegrams
    EMSESP::rxservice_.loop();
}

// call the endpoint and get the response, GET
const char * call_url(const char * url) {
    AsyncWebServerRequest request;
    request.method(HTTP_GET);
    request.url(url);
    webAPIService->webAPIService(&request);
    return webAPIService->getResponse();
}

// call the endpoint and get the response, using a POST
const char * call_url(const char * url, const char * data) {
    JsonDocument doc;
    JsonVariant  json;

    deserializeJson(doc, data);
    json = doc.as<JsonVariant>();

    AsyncWebServerRequest request;
    request.method(HTTP_POST);
    request.url(url);
    webAPIService->webAPIService(&request, json);
    return webAPIService->getResponse();
}

// capture the response and print it out as a test, auto-generates the test functions
// use with -DEMSESP_UNITY_CREATE in the platformio build flags
// only needs to be done once
void capture(const char * url = nullptr) {
    static uint8_t count = 1;

    if (count == 1) {
        Serial.println();
        Serial.println("// ---------- START - CUT HERE ----------");
        Serial.println();
    }

    if (url) {
        // call API, find and replace all double quotes with escaped quotes
        std::string escaped_response = call_url(url);

        size_t pos = 0;
        while ((pos = escaped_response.find("\"", pos)) != std::string::npos) {
            escaped_response.replace(pos, 1, "\\\"");
            pos += 2;
        }

        Serial.printf("void test_%d() {\n", count++);
        Serial.printf("    auto expected_response = \"%s\";\n", escaped_response.c_str());
        Serial.printf("    TEST_ASSERT_EQUAL_STRING(expected_response, call_url(\"%s\"));\n", url);
        Serial.println("}");
        Serial.println();
    } else {
        // no args means last call, create the run_tests function
        Serial.println("void run_tests() {");
        for (uint8_t i = 1; i < count; i++) {
            Serial.printf("    RUN_TEST(test_%d);\n", i);
        }
        Serial.println("}");
        Serial.println();
        Serial.println("// ---------- END - CUT HERE ----------");
        Serial.println();
        Serial.println();
    }
}

// Functions for backup, just in case we don't have generated functions yet
// void test1() {
//     TEST_ASSERT(expected_response != nullptr);
// }
// void run_tests() {
//     RUN_TEST(test1);
// }

void manual_test1() {
    auto expected_response = "[{}]"; // empty is good
    char data[]            = "{\"cmd\":\"send\",\"data\":\"0B 90 FF 13 01 01 B9 01\"}";

    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/system", data));
}

void manual_test2() {
    auto expected_response = "[{}]"; // empty is good
    char data[]            = "{\"value\":12}";

    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/thermostat/seltemp", data));
}

void manual_test3() {
    auto expected_response = "[{}]"; // empty is good
    char data[]            = "{\"device\":\"thermostat\", \"cmd\":\"hc2.seltemp\",\"value\":14}";

    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api", data));
}

void manual_test4() {
    auto expected_response = "[{}]"; // empty is good
    char data[]            = "{\"entity\":\"seltemp\",\"value\":11}";

    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/thermostat", data));
}

void manual_test5() {
    auto expected_response = "[{}]"; // empty is good
    char data[]            = "{\"value\":11}";

    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/analogsensor/test_analogsensor4", data));
}

void manual_test6() {
    auto expected_response = "[{}]"; // empty is good
    char data[]            = "{\"value\":10,\"id\":33}";

    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/analogsensor/setvalue", data));
}


void run_manual_tests() {
    RUN_TEST(manual_test1);
    RUN_TEST(manual_test2);
    RUN_TEST(manual_test3);
    RUN_TEST(manual_test4);
    RUN_TEST(manual_test5);
    RUN_TEST(manual_test6);
}

const char * run_console_command(const char * command) {
    output_buffer[0] = '\0'; // empty the temp buffer
    shell->invoke_command(command);

    // The buffer now contains a prompt, the command, the output and a \r\n
    // remove the \r\n at the end
    char * p = strstr(output_buffer, "\r\n");
    if (p) {
        *p = '\0';
    }

    // Now go to just after the prompt and command
    p = output_buffer + 7 + strlen(command);

    // Serial.println("Output:");
    // Serial.print(p);
    // Serial.println();

    return p;
}

void console_test1() {
    auto expected_response = "Log level: DEBUG";
    TEST_ASSERT_EQUAL_STRING(expected_response, run_console_command("log"));
}

void console_test2() {
    auto expected_response = "";
    TEST_ASSERT_EQUAL_STRING(expected_response, run_console_command("call thermostat mode auto"));
}

void console_test3() {
    // test bad command
    auto expected_response = "Bad syntax. Check arguments.";
    TEST_ASSERT_EQUAL_STRING(expected_response, run_console_command("call thermostat mode bad"));
}

// simulate console commands
void run_console_tests() {
    RUN_TEST(console_test1);
    RUN_TEST(console_test2);
    RUN_TEST(console_test3);
}

// auto-generate the tests
void create_tests() {
    // These match the calls in test_api.h
    // They are all READ calls, no POST or PUT. We use the manual tests for those.

    // this first section should all pass
    capture("/api/boiler");
    capture("/api/boiler/commands");
    capture("/api/boiler/values");
    capture("/api/boiler/info");
    // capture("/api/boiler/entities"); // skipping since payload is too large
    capture("/api/boiler/comfort");
    capture("/api/boiler/comfort/value");
    capture("/api/boiler/comfort/fullname");
    capture("/api/boiler/outdoortemp");
    capture("/api/boiler/dhw/chargetype");
    capture("/api/boiler/dhw.chargetype/writeable");
    capture("/api/boiler/flamecurr/value");

    // thermostat
    capture("/api/thermostat");
    capture("/api/thermostat/hc1/values");
    capture("/api/thermostat/hc1/seltemp");
    capture("/api/thermostat/hc2/seltemp");

    // custom
    capture("/api/custom");
    capture("/api/custom/info");
    capture("/api/custom/seltemp");

    // system
    capture("/api/system");
    capture("/api/system/info");
    capture("/api/system/settings/locale");
    capture("/api/system/fetch");
    capture("api/system/network/values");

    // scheduler
    capture("/api/scheduler");
    capture("/api/scheduler/info");
    capture("/api/scheduler/test_scheduler");

    // temperaturesensor
    capture("/api/temperaturesensor");
    capture("/api/temperaturesensor/info");
    capture("/api/temperaturesensor/test_tempsensor2");
    capture("/api/temperaturesensor/0B_0C0D_0E0F_1011");
    capture("/api/temperaturesensor/test_tempsensor2/value");

    // analogsensor
    capture("/api/analogsensor");
    capture("/api/analogsensor/info");
    capture("/api/analogsensor/test_analogsensor1");
    capture("/api/analogsensor/test_analogsensor1/offset");

    // these tests should all fail...
    capture("/api/boiler2");
    capture("/api/boiler/bad/value");
    capture("/api/boiler/comfort/valu");

    // system
    capture("/api/system/settings/locale2");
    capture("/api/system/settings2");
    capture("/api/system/settings2/locale2");

    // scheduler
    capture("/api/scheduler/test_scheduler2");
    capture("/api/scheduler/test_scheduler/val");
    capture("/api/scheduler/test_scheduler2/val2");

    // custom
    capture("/api/custom/seltemp2");
    capture("/api/custom/seltemp/val");

    // temperaturesensor
    capture("/api/temperaturesensor/test_sensor20");
    capture("/api/temperaturesensor/0B_0C0D_0E0F_XXXX");
    capture("/api/temperaturesensor/test_tempsensor2/bad");

    // analogsensor
    capture("/api/analogsensor/test_analogsensor1/bad");
    capture("/api/analogsensor/test_analog10");
    capture("/api/analogsensor/test_analog10/bad2");

    // **************************************************************************************************
    // Finish
    capture(); // always end with this, this will create the run_test() function
}

// Main entry point
int main() {
    webServer     = new AsyncWebServer(80);
    esp32React    = new ESP32React(webServer, &dummyFS);
    webAPIService = new WebAPIService(webServer, esp32React->getSecurityManager());

    // Serial console for commands
    Serial.begin(115200);
    shell = std::make_shared<EMSESPConsole>(application, stream, true);
    shell->log_level(uuid::log::Level::DEBUG);
    shell->add_flags(CommandFlags::ADMIN);

    application.start(); // calls begin()

    EMSESP::webCustomEntityService.test();  // custom entities
    EMSESP::webCustomizationService.test(); // set customizations - this will overwrite any settings in the FS
    EMSESP::temperaturesensor_.test();      // add temperature sensors
    EMSESP::webSchedulerService.test();     // run scheduler tests, and conditions

    add_devices(); // add devices

#if defined(EMSESP_UNITY_CREATE)
    create_tests();
#endif

    //
    // Run the tests
    //
    UNITY_BEGIN();

    run_tests();              // execute the generated tests
    run_manual_tests();       // execute some other manual tests from this file
    run_console_tests();      // execute some console tests
    run_shuntingYard_tests(); // execute the shuntingYard tests

    return UNITY_END();
}
