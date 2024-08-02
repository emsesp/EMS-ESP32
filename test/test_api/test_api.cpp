#include <Arduino.h>
#include <unity.h>

#include <emsesp.h>
#include "ESPAsyncWebServer.h"
#include "ESP8266React.h"
#include "web/WebAPIService.h"

using namespace emsesp;

AsyncWebServer *      webServer;
ESP8266React *        esp8266React;
WebAPIService *       webAPIService;
AsyncWebServerRequest request;
EMSESP                application;
FS                    dummyFS;

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

void add_device(uint8_t device_id, uint8_t product_id) {
    uart_telegram({device_id, 0x0B, EMSdevice::EMS_TYPE_VERSION, 0, product_id, 1, 0});
}

void setUp() {
    JsonDocument doc;
    JsonVariant  json;

    webServer     = new AsyncWebServer(80);
    esp8266React  = new ESP8266React(webServer, &dummyFS);
    webAPIService = new WebAPIService(webServer, esp8266React->getSecurityManager());

    application.start(); // calls begin()

    EMSESP::webCustomEntityService.test();  // custom entities
    EMSESP::webCustomizationService.test(); // set customizations - this will overwrite any settings in the FS
    EMSESP::temperaturesensor_.test();      // add temperature sensors
    EMSESP::webSchedulerService.test();     // run scheduler tests, and conditions

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

void tearDown() {
}

void get_url(const char * url) {
    request.method(HTTP_GET);
    request.url(url);
    webAPIService->webAPIService(&request);
}

void test1() {
    get_url("/api/system");

    // escape strings with https://dolitools.com/text-tools/escape-unescape-string/
    auto response =
        "[{\"system\":{\"version\":\"3.7.0-dev.29\",\"uptime\":\"000+00:00:00.000\",\"uptimeSec\":0,\"resetReason\":\"Unknown / "
        "Unknown\"},\"network\":{\"network\":\"WiFi\",\"hostname\":\"ems-esp\",\"RSSI\":-23,\"TxPowerSetting\":0,\"staticIP\":false,\"lowBandwidth\":false,"
        "\"disableSleep\":false,\"enableMDNS\":true,\"enableCORS\":false},\"ntp\":{},\"mqtt\":{\"MQTTStatus\":\"disconnected\",\"MQTTPublishes\":0,"
        "\"MQTTQueued\":"
        "0,\"MQTTPublishFails\":0,\"MQTTConnects\":1,\"enabled\":true,\"clientID\":\"ems-esp\",\"keepAlive\":60,\"cleanSession\":false,\"entityFormat\":1,"
        "\"base\":"
        "\"ems-esp\",\"discoveryPrefix\":\"homeassistant\",\"discoveryType\":0,\"nestedFormat\":1,\"haEnabled\":true,\"mqttQos\":0,\"mqttRetain\":false,"
        "\"publishTimeHeartbeat\":60,\"publishTimeBoiler\":10,\"publishTimeThermostat\":10,\"publishTimeSolar\":10,\"publishTimeMixer\":10,"
        "\"publishTimeWater\":0,"
        "\"publishTimeOther\":10,\"publishTimeSensor\":10,\"publishSingle\":false,\"publish2command\":false,\"sendResponse\":false},\"syslog\":{\"enabled\":"
        "false},"
        "\"sensor\":{\"temperatureSensors\":2,\"temperatureSensorReads\":0,\"temperatureSensorFails\":0,\"analogSensors\":2,\"analogSensorReads\":0,"
        "\"analogSensorFails\":0},\"api\":{\"APICalls\":0,\"APIFails\":0},\"bus\":{\"busStatus\":\"connected\",\"busProtocol\":\"Buderus\","
        "\"busTelegramsReceived\":8,\"busReads\":0,\"busWrites\":0,\"busIncompleteTelegrams\":0,\"busReadsFailed\":0,\"busWritesFailed\":0,"
        "\"busRxLineQuality\":"
        "100,\"busTxLineQuality\":100},\"settings\":{\"boardProfile\":\"S32\",\"locale\":\"en\",\"txMode\":8,\"emsBusID\":11,\"showerTimer\":false,"
        "\"showerMinDuration\":180,\"showerAlert\":false,\"hideLed\":false,\"noTokenApi\":false,\"readonlyMode\":false,\"fahrenheit\":false,\"dallasParasite\":"
        "false,\"boolFormat\":1,\"boolDashboard\":1,\"enumFormat\":1,\"analogEnabled\":true,\"telnetEnabled\":true,\"maxWebLogBuffer\":50,\"webLogBuffer\":33,"
        "\"modbusEnabled\":false},\"devices\":[{\"type\":\"boiler\",\"name\":\"Custom "
        "Name!!\",\"deviceID\":\"0x08\",\"productID\":123,\"brand\":\"\",\"version\":\"01.00\",\"entities\":37,\"handlersReceived\":\"0x18\","
        "\"handlersFetched\":"
        "\"0x14 0x33\",\"handlersPending\":\"0xBF 0x10 0x11 0xC2 0x15 0x1C 0x19 0x1A 0x35 0x34 0x2A 0xD1 0xE3 0xE4 0xE5 0xE9 0x2E "
        "0x3B\"},{\"type\":\"thermostat\",\"name\":\"FW120\",\"deviceID\":\"0x10\",\"productID\":192,\"brand\":\"\",\"version\":\"01.00\",\"entities\":15,"
        "\"handlersReceived\":\"0x016F\",\"handlersFetched\":\"0x0170 0x0171\",\"handlersPending\":\"0xA3 0x06 0xA2 0x12 0x13 0x0172 0x0165 0x0168\"}]}]";

    TEST_ASSERT_EQUAL_STRING(response, webAPIService->getResponse());
}

void test_custom1() {
    get_url("/api/custom");
    auto response = "[{\"test_custom\":0.00,\"test_read_only\":0.00,\"test_ram\":\"14\",\"seltemp\":\"14\"}]";
    TEST_ASSERT_EQUAL_STRING(response, webAPIService->getResponse());
}

void test_custom2() {
    get_url("/api/custom/seltemp");
    auto response = "[{\"name\":\"seltemp\",\"storage\":\"ram\",\"type\":\"number\",\"readable\":true,\"writeable\":true,\"visible\":true,\"value\":\"14\"}]";
    TEST_ASSERT_EQUAL_STRING(response, webAPIService->getResponse());
}

int main() {
    UNITY_BEGIN();

    RUN_TEST(test1);
    RUN_TEST(test_custom1);
    RUN_TEST(test_custom2);

    // TODO add all the remaining tests from test.cpp "api3"
    // and Michaels' tests...

    /*
api/device
api/device/info
api/device/value
api/device/entities
api/device/commands
api/thermostat/hc2/values (info/entities) to get single circuit info
api/system/network/values (info/entities) to get single circuit info same as for ems devices
api/device/name
api/device/name/attribute
api/device/circuit/name
api/device/circuit/name/attribute
*/


    return UNITY_END();
}
