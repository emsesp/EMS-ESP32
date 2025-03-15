// **************************************************************************************************
//
// Compile with -DEMSESP_UNITY_CREATE to generate the test functions, copy the output and paste below.
//
// TODO convert output to JSON and compare, showing differences
//
// You can also manually compare the differences using https://www.diffchecker.com/text-compare/
//
// **************************************************************************************************

// ---------- START - CUT HERE ----------

void test_1() {
    auto expected_response =
        "[{\"reset\":\"\",\"heatingoff\":\"off\",\"heatingactive\":\"off\",\"tapwateractive\":\"on\",\"selflowtemp\":0,\"curflowtemp\":60.2,\"rettemp\":48.1,"
        "\"syspress\":1.4,\"burngas\":\"on\",\"burngas2\":\"off\",\"flamecurr\":37.4,\"fanwork\":\"on\",\"ignwork\":\"off\",\"oilpreheat\":\"off\","
        "\"heatingpump\":\"on\",\"selburnpow\":115,\"curburnpow\":61,\"ubauptime\":3940268,\"servicecode\":\"=H\",\"servicecodenumber\":201,\"nompower\":0,"
        "\"nrgtotal\":0.0,\"nrgheat\":0.0,\"dhw\":{\"seltemp\":52,\"comfort\":\"hot\",\"flowtempoffset\":40,\"circpump\":\"off\",\"chargetype\":\"3-way "
        "valve\",\"hyston\":-5,\"hystoff\":0,\"disinfectiontemp\":70,\"circmode\":\"off\",\"circ\":\"off\",\"storagetemp1\":53.8,\"activated\":\"on\","
        "\"3wayvalve\":\"on\",\"nrg\":0.0}}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/boiler"));
}

void test_2() {
    auto expected_response =
        "[{\"info\":\"list all values (verbose)\",\"values\":\"list all values\",\"commands\":\"list all commands\",\"entities\":\"list all "
        "entities\",\"boil2hystoff\":\"hysteresis stage 2 off temperature\",\"boil2hyston\":\"hysteresis stage 2 on temperature\",\"boilhystoff\":\"hysteresis "
        "off temperature\",\"boilhyston\":\"hysteresis on temperature\",\"burnmaxpower\":\"burner max power\",\"burnminperiod\":\"burner min "
        "period\",\"burnminpower\":\"burner min power\",\"coldshot\":\"send a cold shot of water\",\"curvebase\":\"heatingcurve "
        "base\",\"curveend\":\"heatingcurve end\",\"curveon\":\"heatingcurve on\",\"dhw[n].activated\":\"activated\",\"dhw[n].chargeoptimization\":\"charge "
        "optimization\",\"dhw[n].circ\":\"circulation active\",\"dhw[n].circmode\":\"circulation pump mode\",\"dhw[n].circpump\":\"circulation pump "
        "available\",\"dhw[n].comfort\":\"comfort\",\"dhw[n].comfort1\":\"comfort "
        "mode\",\"dhw[n].disinfecting\":\"disinfecting\",\"dhw[n].disinfectiontemp\":\"disinfection temperature\",\"dhw[n].flowtempoffset\":\"flow temperature "
        "offset\",\"dhw[n].hystoff\":\"hysteresis off temperature\",\"dhw[n].hyston\":\"hysteresis on temperature\",\"dhw[n].maxpower\":\"max "
        "power\",\"dhw[n].maxtemp\":\"maximum temperature\",\"dhw[n].nrg\":\"energy\",\"dhw[n].onetime\":\"one time charging\",\"dhw[n].seltemp\":\"selected "
        "temperature\",\"dhw[n].seltemplow\":\"selected lower temperature\",\"dhw[n].seltempsingle\":\"single charge "
        "temperature\",\"dhw[n].tapactivated\":\"turn on/off\",\"dhw[n].tempecoplus\":\"selected eco+ temperature\",\"emergencyops\":\"emergency "
        "operation\",\"emergencytemp\":\"emergency temperature\",\"heatingactivated\":\"heating activated\",\"heatingoff\":\"force heating "
        "off\",\"heatingtemp\":\"heating temperature\",\"maintenance\":\"maintenance scheduled\",\"maintenancedate\":\"next maintenance "
        "date\",\"maintenancetime\":\"time to next maintenance\",\"nofrostmode\":\"nofrost mode\",\"nofrosttemp\":\"nofrost "
        "temperature\",\"nompower\":\"nominal Power\",\"nrgheat\":\"energy heating\",\"pumpcharacter\":\"boiler pump characteristic\",\"pumpdelay\":\"pump "
        "delay\",\"pumpmode\":\"boiler pump mode\",\"pumpmodmax\":\"boiler pump max power\",\"pumpmodmin\":\"boiler pump min power\",\"pumpontemp\":\"pump "
        "logic temperature\",\"reset\":\"reset\",\"selburnpow\":\"burner selected max power\",\"selflowtemp\":\"selected flow "
        "temperature\",\"summertemp\":\"summer temperature\"}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/boiler/commands"));
}

void test_3() {
    auto expected_response =
        "[{\"reset\":\"\",\"heatingoff\":\"off\",\"heatingactive\":\"off\",\"tapwateractive\":\"on\",\"selflowtemp\":0,\"curflowtemp\":60.2,\"rettemp\":48.1,"
        "\"syspress\":1.4,\"burngas\":\"on\",\"burngas2\":\"off\",\"flamecurr\":37.4,\"fanwork\":\"on\",\"ignwork\":\"off\",\"oilpreheat\":\"off\","
        "\"heatingpump\":\"on\",\"selburnpow\":115,\"curburnpow\":61,\"ubauptime\":3940268,\"servicecode\":\"=H\",\"servicecodenumber\":201,\"nompower\":0,"
        "\"nrgtotal\":0.0,\"nrgheat\":0.0,\"dhw\":{\"seltemp\":52,\"comfort\":\"hot\",\"flowtempoffset\":40,\"circpump\":\"off\",\"chargetype\":\"3-way "
        "valve\",\"hyston\":-5,\"hystoff\":0,\"disinfectiontemp\":70,\"circmode\":\"off\",\"circ\":\"off\",\"storagetemp1\":53.8,\"activated\":\"on\","
        "\"3wayvalve\":\"on\",\"nrg\":0.0}}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/boiler/values"));
}

void test_4() {
    auto expected_response =
        "[{\"reset (reset)\":\"\",\"force heating off (heatingoff)\":\"off\",\"is my heating on? (heatingactive)\":\"off\",\"tapwater active "
        "(tapwateractive)\":\"on\",\"selected flow temperature (selflowtemp)\":0,\"current flow temperature (curflowtemp)\":60.2,\"return temperature "
        "(rettemp)\":48.1,\"system pressure (syspress)\":1.4,\"gas (burngas)\":\"on\",\"gas stage 2 (burngas2)\":\"off\",\"flame current "
        "(flamecurr)\":37.4,\"fan (fanwork)\":\"on\",\"ignition (ignwork)\":\"off\",\"oil preheating (oilpreheat)\":\"off\",\"heating pump "
        "(heatingpump)\":\"on\",\"burner selected max power (selburnpow)\":115,\"burner current power (curburnpow)\":61,\"total UBA operating time "
        "(ubauptime)\":\"2736 days 7 hours 8 minutes\",\"service code (servicecode)\":\"=H\",\"service code number (servicecodenumber)\":201,\"dhw selected "
        "temperature (seltemp)\":52,\"dhw comfort (comfort)\":\"hot\",\"dhw flow temperature offset (flowtempoffset)\":40,\"dhw circulation pump available "
        "(circpump)\":\"off\",\"dhw charging type (chargetype)\":\"3-way valve\",\"dhw hysteresis on temperature (hyston)\":-5,\"dhw hysteresis off "
        "temperature (hystoff)\":0,\"dhw disinfection temperature (disinfectiontemp)\":70,\"dhw circulation pump mode (circmode)\":\"off\",\"dhw circulation "
        "active (circ)\":\"off\",\"dhw storage intern temperature (storagetemp1)\":53.8,\"dhw activated (activated)\":\"on\",\"dhw 3-way valve active "
        "(3wayvalve)\":\"on\",\"nominal Power (nompower)\":0,\"total energy (nrgtotal)\":0.0,\"energy heating (nrgheat)\":0.0,\"dhw energy (nrg)\":0.0}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/boiler/info"));
}

void test_5() {
    auto expected_response = "[{\"name\":\"comfort\",\"fullname\":\"dhw "
                             "comfort\",\"circuit\":\"dhw\",\"index\":0,\"enum\":[\"hot\",\"eco\",\"intelligent\"],\"value\":\"hot\",\"type\":\"enum\","
                             "\"readable\":true,\"writeable\":true,\"visible\":true}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/boiler/comfort"));
}

void test_6() {
    auto expected_response = "[{\"api_data\":\"hot\"}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/boiler/comfort/value"));
}

void test_7() {
    auto expected_response = "[{\"api_data\":\"dhw comfort\"}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/boiler/comfort/fullname"));
}

void test_8() {
    auto expected_response = "[{\"name\":\"outdoortemp\",\"fullname\":\"outside "
                             "temperature\",\"circuit\":\"\",\"type\":\"number\",\"uom\":\"°C\",\"state_class\":\"measurement\",\"device_class\":"
                             "\"temperature\",\"readable\":true,\"writeable\":false,\"visible\":true}]";

    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/boiler/outdoortemp"));
}

void test_9() {
    auto expected_response = "[{\"name\":\"chargetype\",\"fullname\":\"dhw charging type\",\"circuit\":\"dhw\",\"index\":1,\"enum\":[\"chargepump\",\"3-way "
                             "valve\"],\"value\":\"3-way valve\",\"type\":\"enum\",\"readable\":true,\"writeable\":false,\"visible\":true}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/boiler/dhw/chargetype"));
}

void test_10() {
    auto expected_response = "[{\"api_data\":\"false\"}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/boiler/dhw.chargetype/writeable"));
}

void test_11() {
    auto expected_response = "[{\"api_data\":\"37.4\"}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/boiler/flamecurr/value"));
}

void test_12() {
    auto expected_response = "[{\"hc1\":{\"seltemp\":20.5,\"currtemp\":22.8,\"haclimate\":\"roomTemp\",\"modetype\":\"heat\",\"remotetemp\":null},\"hc2\":{"
                             "\"seltemp\":20.6,\"currtemp\":22.9,\"haclimate\":\"roomTemp\",\"modetype\":\"eco\",\"remotetemp\":null},\"hc3\":{\"seltemp\":20."
                             "7,\"currtemp\":23.0,\"haclimate\":\"roomTemp\",\"modetype\":\"nofrost\",\"remotetemp\":null},\"dhw\":{}}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/thermostat"));
}

void test_13() {
    auto expected_response = "[{\"seltemp\":20.5,\"currtemp\":22.8,\"haclimate\":\"roomTemp\",\"modetype\":\"heat\",\"remotetemp\":null}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/thermostat/hc1/values"));
}

void test_14() {
    auto expected_response = "[{\"name\":\"seltemp\",\"fullname\":\"hc1 selected room "
                             "temperature\",\"circuit\":\"hc1\",\"value\":20.5,\"type\":\"number\",\"min\":0,\"max\":30,\"uom\":\"°C\",\"state_class\":"
                             "\"measurement\",\"device_class\":\"temperature\",\"readable\":true,\"writeable\":true,\"visible\":true}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/thermostat/hc1/seltemp"));
}

void test_15() {
    auto expected_response = "[{\"name\":\"seltemp\",\"fullname\":\"hc2 selected room "
                             "temperature\",\"circuit\":\"hc2\",\"value\":20.6,\"type\":\"number\",\"min\":0,\"max\":30,\"uom\":\"°C\",\"state_class\":"
                             "\"measurement\",\"device_class\":\"temperature\",\"readable\":true,\"writeable\":true,\"visible\":true}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/thermostat/hc2/seltemp"));
}

void test_16() {
    auto expected_response = "[{\"test_custom\":0.00,\"test_read_only\":0.00,\"test_ram\":\"14\",\"seltemp\":\"14\"}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/custom"));
}

void test_17() {
    auto expected_response = "[{\"test_custom\":0.00,\"test_read_only\":0.00,\"test_ram\":\"14\",\"seltemp\":\"14\"}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/custom/info"));
}

void test_18() {
    auto expected_response = "[{\"name\":\"seltemp\",\"fullname\":\"seltemp\",\"storage\":\"ram\",\"type\":\"number\",\"readable\":true,\"writeable\":true,"
                             "\"visible\":true,\"value\":\"14\"}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/custom/seltemp"));
}

void test_19() {
    auto expected_response =
        "[{\"system\":{\"version\":\"dev\",\"uptime\":\"000+00:00:00.000\",\"uptimeSec\":0,\"resetReason\":\"Unknown / "
        "Unknown\"},\"network\":{\"network\":\"WiFi\",\"hostname\":\"ems-esp\",\"RSSI\":-23,\"TxPowerSetting\":0,\"staticIP\":false,\"lowBandwidth\":false,"
        "\"disableSleep\":true,\"enableMDNS\":true,\"enableCORS\":false},\"ntp\":{},\"mqtt\":{\"MQTTStatus\":\"disconnected\",\"MQTTPublishes\":0,"
        "\"MQTTQueued\":0,\"MQTTPublishFails\":0,\"MQTTReconnects\":0,\"enabled\":true,\"clientID\":\"ems-esp\",\"keepAlive\":60,\"cleanSession\":false,"
        "\"entityFormat\":1,\"base\":\"ems-esp\",\"discoveryPrefix\":\"homeassistant\",\"discoveryType\":0,\"nestedFormat\":1,\"haEnabled\":true,\"mqttQos\":0,"
        "\"mqttRetain\":false,\"publishTimeHeartbeat\":60,\"publishTimeBoiler\":10,\"publishTimeThermostat\":10,\"publishTimeSolar\":10,\"publishTimeMixer\":"
        "10,\"publishTimeWater\":0,\"publishTimeOther\":10,\"publishTimeSensor\":10,\"publishSingle\":false,\"publish2command\":false,\"sendResponse\":false},"
        "\"syslog\":{\"enabled\":false},\"sensor\":{\"temperatureSensors\":2,\"temperatureSensorReads\":0,\"temperatureSensorFails\":0,\"analogSensors\":4,"
        "\"analogSensorReads\":0,\"analogSensorFails\":0},\"api\":{\"APICalls\":0,\"APIFails\":0},\"bus\":{\"busStatus\":\"connected\",\"busProtocol\":"
        "\"Buderus\",\"busTelegramsReceived\":8,\"busReads\":0,\"busWrites\":0,\"busIncompleteTelegrams\":0,\"busReadsFailed\":0,\"busWritesFailed\":0,"
        "\"busRxLineQuality\":100,\"busTxLineQuality\":100},\"settings\":{\"boardProfile\":\"S32\",\"locale\":\"en\",\"txMode\":8,\"emsBusID\":11,"
        "\"showerTimer\":false,\"showerMinDuration\":180,\"showerAlert\":false,\"hideLed\":false,\"noTokenApi\":false,\"readonlyMode\":false,\"fahrenheit\":"
        "false,\"dallasParasite\":false,\"boolFormat\":1,\"boolDashboard\":1,\"enumFormat\":1,\"analogEnabled\":true,\"telnetEnabled\":true,"
        "\"maxWebLogBuffer\":25,\"modbusEnabled\":false,\"forceHeatingOff\":false,\"developerMode\":false},\"devices\":[{\"type\":"
        "\"boiler\",\"name\":\"My Custom "
        "Boiler\",\"deviceID\":\"0x08\",\"productID\":123,\"brand\":\"\",\"version\":\"01.00\",\"entities\":37,\"handlersReceived\":\"0x18\","
        "\"handlersFetched\":\"0x14 0x33\",\"handlersPending\":\"0xBF 0x10 0x11 0xC2 0x15 0x1C 0x19 0x1A 0x35 0x34 0x2A 0xD1 0xE3 0xE4 0xE5 0xE9 0x2E "
        "0x3B\"},{\"type\":\"thermostat\",\"name\":\"FW120\",\"deviceID\":\"0x10\",\"productID\":192,\"brand\":\"\",\"version\":\"01.00\",\"entities\":15,"
        "\"handlersReceived\":\"0x016F\",\"handlersFetched\":\"0x0170 0x0171\",\"handlersPending\":\"0xA3 0x06 0xA2 0x12 0x13 0x0172 0x0165 "
        "0x0168\"},{\"type\":\"temperaturesensor\",\"name\":\"temperaturesensor\",\"entities\":2},{\"type\":\"analogsensor\",\"name\":\"analogsensor\","
        "\"entities\":4},{\"type\":\"scheduler\",\"name\":\"scheduler\",\"entities\":2},{\"type\":\"custom\",\"name\":\"custom\",\"entities\":4}]}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/system"));
}

void test_20() {
    auto expected_response =
        "[{\"system\":{\"version\":\"dev\",\"uptime\":\"000+00:00:00.000\",\"uptimeSec\":0,\"resetReason\":\"Unknown / "
        "Unknown\"},\"network\":{\"network\":\"WiFi\",\"hostname\":\"ems-esp\",\"RSSI\":-23,\"TxPowerSetting\":0,\"staticIP\":false,\"lowBandwidth\":false,"
        "\"disableSleep\":true,\"enableMDNS\":true,\"enableCORS\":false},\"ntp\":{},\"mqtt\":{\"MQTTStatus\":\"disconnected\",\"MQTTPublishes\":0,"
        "\"MQTTQueued\":0,\"MQTTPublishFails\":0,\"MQTTReconnects\":0,\"enabled\":true,\"clientID\":\"ems-esp\",\"keepAlive\":60,\"cleanSession\":false,"
        "\"entityFormat\":1,\"base\":\"ems-esp\",\"discoveryPrefix\":\"homeassistant\",\"discoveryType\":0,\"nestedFormat\":1,\"haEnabled\":true,\"mqttQos\":0,"
        "\"mqttRetain\":false,\"publishTimeHeartbeat\":60,\"publishTimeBoiler\":10,\"publishTimeThermostat\":10,\"publishTimeSolar\":10,\"publishTimeMixer\":"
        "10,\"publishTimeWater\":0,\"publishTimeOther\":10,\"publishTimeSensor\":10,\"publishSingle\":false,\"publish2command\":false,\"sendResponse\":false},"
        "\"syslog\":{\"enabled\":false},\"sensor\":{\"temperatureSensors\":2,\"temperatureSensorReads\":0,\"temperatureSensorFails\":0,\"analogSensors\":4,"
        "\"analogSensorReads\":0,\"analogSensorFails\":0},\"api\":{\"APICalls\":0,\"APIFails\":0},\"bus\":{\"busStatus\":\"connected\",\"busProtocol\":"
        "\"Buderus\",\"busTelegramsReceived\":8,\"busReads\":0,\"busWrites\":0,\"busIncompleteTelegrams\":0,\"busReadsFailed\":0,\"busWritesFailed\":0,"
        "\"busRxLineQuality\":100,\"busTxLineQuality\":100},\"settings\":{\"boardProfile\":\"S32\",\"locale\":\"en\",\"txMode\":8,\"emsBusID\":11,"
        "\"showerTimer\":false,\"showerMinDuration\":180,\"showerAlert\":false,\"hideLed\":false,\"noTokenApi\":false,\"readonlyMode\":false,\"fahrenheit\":"
        "false,\"dallasParasite\":false,\"boolFormat\":1,\"boolDashboard\":1,\"enumFormat\":1,\"analogEnabled\":true,\"telnetEnabled\":true,"
        "\"maxWebLogBuffer\":25,\"modbusEnabled\":false,\"forceHeatingOff\":false,\"developerMode\":false},\"devices\":[{\"type\":"
        "\"boiler\",\"name\":\"My Custom "
        "Boiler\",\"deviceID\":\"0x08\",\"productID\":123,\"brand\":\"\",\"version\":\"01.00\",\"entities\":37,\"handlersReceived\":\"0x18\","
        "\"handlersFetched\":\"0x14 0x33\",\"handlersPending\":\"0xBF 0x10 0x11 0xC2 0x15 0x1C 0x19 0x1A 0x35 0x34 0x2A 0xD1 0xE3 0xE4 0xE5 0xE9 0x2E "
        "0x3B\"},{\"type\":\"thermostat\",\"name\":\"FW120\",\"deviceID\":\"0x10\",\"productID\":192,\"brand\":\"\",\"version\":\"01.00\",\"entities\":15,"
        "\"handlersReceived\":\"0x016F\",\"handlersFetched\":\"0x0170 0x0171\",\"handlersPending\":\"0xA3 0x06 0xA2 0x12 0x13 0x0172 0x0165 "
        "0x0168\"},{\"type\":\"temperaturesensor\",\"name\":\"temperaturesensor\",\"entities\":2},{\"type\":\"analogsensor\",\"name\":\"analogsensor\","
        "\"entities\":4},{\"type\":\"scheduler\",\"name\":\"scheduler\",\"entities\":2},{\"type\":\"custom\",\"name\":\"custom\",\"entities\":4}]}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/system/info"));
}

void test_21() {
    auto expected_response =
        "[{\"name\":\"locale\",\"circuit\":\"settings\",\"readable\":true,\"writeable\":false,\"visible\":true,\"value\":\"en\",\"type\":\"string\"}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/system/settings/locale"));
}

void test_22() {
    auto expected_response = "[{}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/system/fetch"));
}

void test_23() {
    auto expected_response = "[{\"network\":\"WiFi\",\"hostname\":\"ems-esp\",\"RSSI\":\"-23\",\"TxPowerSetting\":\"0\",\"staticIP\":\"false\","
                             "\"lowBandwidth\":\"false\",\"disableSleep\":\"true\",\"enableMDNS\":\"true\",\"enableCORS\":\"false\"}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("api/system/network/values"));
}

void test_24() {
    auto expected_response = "[{\"test_scheduler\":\"on\"}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/scheduler"));
}

void test_25() {
    auto expected_response = "[{\"test_scheduler\":\"on\"}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/scheduler/info"));
}

void test_26() {
    auto expected_response = "[{\"name\":\"test_scheduler\",\"fullname\":\"test_scheduler\",\"type\":\"boolean\",\"value\":\"on\",\"time\":\"12:00\","
                             "\"command\":\"system/fetch\",\"cmd_data\":\"10\",\"readable\":true,\"writeable\":true,\"visible\":true}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/scheduler/test_scheduler"));
}

void test_27() {
    auto expected_response = "[{\"test_tempsensor1\":12.3,\"test_tempsensor2\":45.6}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/temperaturesensor"));
}

void test_28() {
    auto expected_response = "[{\"test_tempsensor1\":12.3,\"test_tempsensor2\":45.6}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/temperaturesensor/info"));
}

void test_29() {
    auto expected_response = "[{\"id\":\"0B_0C0D_0E0F_1011\",\"name\":\"test_tempsensor2\",\"fullname\":\"test_tempsensor2\",\"value\":45.6,\"type\":"
                             "\"number\",\"uom\":\"°C\",\"readable\":true,\"writeable\":false,\"visible\":true}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/temperaturesensor/test_tempsensor2"));
}

void test_30() {
    auto expected_response = "[{\"id\":\"0B_0C0D_0E0F_1011\",\"name\":\"test_tempsensor2\",\"fullname\":\"test_tempsensor2\",\"value\":45.6,\"type\":"
                             "\"number\",\"uom\":\"°C\",\"readable\":true,\"writeable\":false,\"visible\":true}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/temperaturesensor/0B_0C0D_0E0F_1011"));
}

void test_31() {
    auto expected_response = "[{\"api_data\":\"45.6\"}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/temperaturesensor/test_tempsensor2/value"));
}

void test_32() {
    auto expected_response = "[{\"test_analogsensor1\":0,\"test_analogsensor2\":1,\"test_analogsensor3\":0,\"test_analogsensor4\":0}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/analogsensor"));
}

void test_33() {
    auto expected_response = "[{\"test_analogsensor1\":0,\"test_analogsensor2\":1,\"test_analogsensor3\":0,\"test_analogsensor4\":0}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/analogsensor/info"));
}

void test_34() {
    auto expected_response = "[{\"name\":\"test_analogsensor1\",\"fullname\":\"test_analogsensor1\",\"gpio\":36,\"type\":\"number\",\"analog\":\"adc\","
                             "\"value\":0,\"readable\":true,\"writeable\":false,\"visible\":true,\"offset\":0,\"factor\":0.1,\"uom\":\"mV\"}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/analogsensor/test_analogsensor1"));
}

void test_35() {
    auto expected_response = "[{\"api_data\":\"0\"}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/analogsensor/test_analogsensor1/offset"));
}

void test_36() {
    auto expected_response = "[{\"message\":\"unknown device boiler2\"}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/boiler2"));
}

void test_37() {
    auto expected_response = "[{}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/boiler/bad/value"));
}

void test_38() {
    auto expected_response = "[{\"message\":\"no attribute 'valu' in comfort\"}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/boiler/comfort/valu"));
}

void test_39() {
    auto expected_response = "[{\"message\":\"no entity 'settings' in system\"}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/system/settings/locale2"));
}

void test_40() {
    auto expected_response = "[{\"message\":\"no entity 'settings2' in system\"}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/system/settings2"));
}

void test_41() {
    auto expected_response = "[{\"message\":\"no entity 'settings2' in system\"}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/system/settings2/locale2"));
}

void test_42() {
    auto expected_response = "[{\"message\":\"no entity 'test_scheduler2' in scheduler\"}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/scheduler/test_scheduler2"));
}

void test_43() {
    auto expected_response = "[{\"message\":\"no attribute 'val' in test_scheduler\"}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/scheduler/test_scheduler/val"));
}

void test_44() {
    auto expected_response = "[{\"message\":\"no entity 'test_scheduler2' in scheduler\"}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/scheduler/test_scheduler2/val2"));
}

void test_45() {
    auto expected_response = "[{\"message\":\"no entity 'seltemp2' in custom\"}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/custom/seltemp2"));
}

void test_46() {
    auto expected_response = "[{\"message\":\"no attribute 'val' in seltemp\"}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/custom/seltemp/val"));
}

void test_47() {
    auto expected_response = "[{\"message\":\"no entity 'test_sensor20' in temperaturesensor\"}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/temperaturesensor/test_sensor20"));
}

void test_48() {
    auto expected_response = "[{\"message\":\"no entity '0b_0c0d_0e0f_xxxx' in temperaturesensor\"}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/temperaturesensor/0B_0C0D_0E0F_XXXX"));
}

void test_49() {
    auto expected_response = "[{\"message\":\"no attribute 'bad' in test_tempsensor2\"}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/temperaturesensor/test_tempsensor2/bad"));
}

void test_50() {
    auto expected_response = "[{\"message\":\"no attribute 'bad' in test_analogsensor1\"}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/analogsensor/test_analogsensor1/bad"));
}

void test_51() {
    auto expected_response = "[{\"message\":\"no entity 'test_analog10' in analogsensor\"}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/analogsensor/test_analog10"));
}

void test_52() {
    auto expected_response = "[{\"message\":\"no entity 'test_analog10' in analogsensor\"}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/analogsensor/test_analog10/bad2"));
}

void run_tests() {
    RUN_TEST(test_1);
    RUN_TEST(test_2);
    RUN_TEST(test_3);
    RUN_TEST(test_4);
    RUN_TEST(test_5);
    RUN_TEST(test_6);
    RUN_TEST(test_7);
    RUN_TEST(test_8);
    RUN_TEST(test_9);
    RUN_TEST(test_10);
    RUN_TEST(test_11);
    RUN_TEST(test_12);
    RUN_TEST(test_13);
    RUN_TEST(test_14);
    RUN_TEST(test_15);
    RUN_TEST(test_16);
    RUN_TEST(test_17);
    RUN_TEST(test_18);
    RUN_TEST(test_19);
    RUN_TEST(test_20);
    RUN_TEST(test_21);
    RUN_TEST(test_22);
    RUN_TEST(test_23);
    RUN_TEST(test_24);
    RUN_TEST(test_25);
    RUN_TEST(test_26);
    RUN_TEST(test_27);
    RUN_TEST(test_28);
    RUN_TEST(test_29);
    RUN_TEST(test_30);
    RUN_TEST(test_31);
    RUN_TEST(test_32);
    RUN_TEST(test_33);
    RUN_TEST(test_34);
    RUN_TEST(test_35);
    RUN_TEST(test_36);
    RUN_TEST(test_37);
    RUN_TEST(test_38);
    RUN_TEST(test_39);
    RUN_TEST(test_40);
    RUN_TEST(test_41);
    RUN_TEST(test_42);
    RUN_TEST(test_43);
    RUN_TEST(test_44);
    RUN_TEST(test_45);
    RUN_TEST(test_46);
    RUN_TEST(test_47);
    RUN_TEST(test_48);
    RUN_TEST(test_49);
    RUN_TEST(test_50);
    RUN_TEST(test_51);
    RUN_TEST(test_52);
}

// ---------- END - CUT HERE ----------
