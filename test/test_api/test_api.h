// ---------- START - CUT HERE ----------

void test_1() {
    auto expected_response =
        "[{\"reset\":\"\",\"chimneysweeper\":\"\",\"heatingoff\":\"off\",\"heatingactive\":\"off\",\"tapwateractive\":\"on\",\"selflowtemp\":0,\"curflowtemp\":"
        "60.2,\"rettemp\":48.1,\"syspress\":1.4,\"burngas\":\"on\",\"burngas2\":\"off\",\"flamecurr\":37.4,\"fanwork\":\"on\",\"ignwork\":\"off\","
        "\"oilpreheat\":\"off\",\"heatingpump\":\"on\",\"selburnpow\":115,\"curburnpow\":61,\"ubauptime\":3940268,\"servicecode\":\"=H\",\"servicecodenumber\":"
        "201,\"nompower\":0,\"nrgtotal\":0,\"nrgheat\":0,\"dhw\":{\"seltemp\":52,\"comfort\":\"hot\",\"flowtempoffset\":40,\"chargeoptimization\":\"off\","
        "\"circpump\":\"off\",\"chargetype\":\"3-way "
        "valve\",\"hyston\":-5,\"disinfectiontemp\":70,\"circmode\":\"off\",\"circ\":\"off\",\"storagetemp1\":53.8,\"activated\":\"on\",\"3wayvalve\":\"on\","
        "\"chargepump\":\"off\",\"nrg\":0}}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/boiler"));
}

void test_2() {
    auto expected_response =
        "[{\"info\":\"list all values (verbose)\",\"values\":\"list all values\",\"commands\":\"list all commands\",\"entities\":\"list all "
        "entities\",\"boil2hystoff\":\"hysteresis stage 2 off temperature\",\"boil2hyston\":\"hysteresis stage 2 on temperature\",\"boilhystoff\":\"hysteresis "
        "off temperature\",\"boilhyston\":\"hysteresis on temperature\",\"burnmaxpower\":\"burner max power\",\"burnminperiod\":\"burner min "
        "period\",\"chimneysweeper\":\"chimney sweeper\",\"coldshot\":\"send a cold shot of water\",\"curvebase\":\"heatingcurve "
        "base\",\"curveend\":\"heatingcurve end\",\"curveon\":\"heatingcurve on\",\"dhw[n].activated\":\"activated\",\"dhw[n].chargeoptimization\":\"charge "
        "optimization\",\"dhw[n].circ\":\"circulation active\",\"dhw[n].circmode\":\"circulation pump mode\",\"dhw[n].circpump\":\"circulation pump "
        "available\",\"dhw[n].comfort\":\"comfort\",\"dhw[n].comfort1\":\"comfort mode\",\"dhw[n].dhwprio\":\"dhw "
        "priority\",\"dhw[n].disinfecting\":\"disinfecting\",\"dhw[n].disinfectiontemp\":\"disinfection temperature\",\"dhw[n].flowtempoffset\":\"flow "
        "temperature offset\",\"dhw[n].hystoff\":\"hysteresis off temperature\",\"dhw[n].hyston\":\"hysteresis on temperature\",\"dhw[n].maxpower\":\"max "
        "power\",\"dhw[n].maxtemp\":\"maximum temperature\",\"dhw[n].nrg\":\"energy\",\"dhw[n].onetime\":\"one time charging\",\"dhw[n].seltemp\":\"selected "
        "temperature\",\"dhw[n].seltemplow\":\"selected lower temperature\",\"dhw[n].seltempsingle\":\"single charge "
        "temperature\",\"dhw[n].tapactivated\":\"turn on/off\",\"dhw[n].tempecoplus\":\"selected eco+ temperature\",\"emergencyops\":\"emergency "
        "operation\",\"emergencytemp\":\"emergency temperature\",\"heatingactivated\":\"heating activated\",\"heatingoff\":\"force heating "
        "off\",\"heatingtemp\":\"heating temperature\",\"maintenance\":\"maintenance scheduled\",\"maintenancedate\":\"next maintenance "
        "date\",\"maintenancetime\":\"time to next maintenance\",\"nofrostmode\":\"nofrost mode\",\"nofrosttemp\":\"nofrost "
        "temperature\",\"nompower\":\"nominal Power\",\"nrgheat\":\"energy heating\",\"pumpcharacter\":\"boiler pump characteristic\",\"pumpdelay\":\"pump "
        "delay\",\"pumpkickday\":\"pump kick day\",\"pumpkickdelay\":\"pump kick delay\",\"pumpkickhour\":\"pump kick hour\",\"pumpmode\":\"boiler pump "
        "mode\",\"pumpmodmax\":\"boiler pump max power\",\"pumpmodmin\":\"boiler pump min power\",\"pumpontemp\":\"pump logic "
        "temperature\",\"reset\":\"reset\",\"selburnpow\":\"burner selected max power\",\"selflowtemp\":\"selected flow temperature\",\"summertemp\":\"summer "
        "temperature\"}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/boiler/commands"));
}

void test_3() {
    auto expected_response =
        "[{\"reset\":\"\",\"chimneysweeper\":\"\",\"heatingoff\":\"off\",\"heatingactive\":\"off\",\"tapwateractive\":\"on\",\"selflowtemp\":0,\"curflowtemp\":"
        "60.2,\"rettemp\":48.1,\"syspress\":1.4,\"burngas\":\"on\",\"burngas2\":\"off\",\"flamecurr\":37.4,\"fanwork\":\"on\",\"ignwork\":\"off\","
        "\"oilpreheat\":\"off\",\"heatingpump\":\"on\",\"selburnpow\":115,\"curburnpow\":61,\"ubauptime\":3940268,\"servicecode\":\"=H\",\"servicecodenumber\":"
        "201,\"nompower\":0,\"nrgtotal\":0,\"nrgheat\":0,\"dhw\":{\"seltemp\":52,\"comfort\":\"hot\",\"flowtempoffset\":40,\"chargeoptimization\":\"off\","
        "\"circpump\":\"off\",\"chargetype\":\"3-way "
        "valve\",\"hyston\":-5,\"disinfectiontemp\":70,\"circmode\":\"off\",\"circ\":\"off\",\"storagetemp1\":53.8,\"activated\":\"on\",\"3wayvalve\":\"on\","
        "\"chargepump\":\"off\",\"nrg\":0}}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/boiler/values"));
}

void test_4() {
    auto expected_response =
        "[{\"reset (reset)\":\"\",\"chimney sweeper (chimneysweeper)\":\"\",\"force heating off (heatingoff)\":\"off\",\"is my heating on? "
        "(heatingactive)\":\"off\",\"tapwater active (tapwateractive)\":\"on\",\"selected flow temperature (selflowtemp)\":0,\"current flow temperature "
        "(curflowtemp)\":60.2,\"return temperature (rettemp)\":48.1,\"system pressure (syspress)\":1.4,\"gas (burngas)\":\"on\",\"gas stage 2 "
        "(burngas2)\":\"off\",\"flame current (flamecurr)\":37.4,\"fan (fanwork)\":\"on\",\"ignition (ignwork)\":\"off\",\"oil preheating "
        "(oilpreheat)\":\"off\",\"heating pump (heatingpump)\":\"on\",\"burner selected max power (selburnpow)\":115,\"burner current power "
        "(curburnpow)\":61,\"total UBA operating time (ubauptime)\":\"2736 days 7 hours 8 minutes\",\"service code (servicecode)\":\"=H\",\"service code "
        "number (servicecodenumber)\":201,\"dhw selected temperature (seltemp)\":52,\"dhw comfort (comfort)\":\"hot\",\"dhw flow temperature offset "
        "(flowtempoffset)\":40,\"dhw charge optimization (chargeoptimization)\":\"off\",\"dhw circulation pump available (circpump)\":\"off\",\"dhw charging "
        "type (chargetype)\":\"3-way valve\",\"dhw hysteresis on temperature (hyston)\":-5,\"dhw disinfection temperature (disinfectiontemp)\":70,\"dhw "
        "circulation pump mode (circmode)\":\"off\",\"dhw circulation active (circ)\":\"off\",\"dhw storage intern temperature (storagetemp1)\":53.8,\"dhw "
        "activated (activated)\":\"on\",\"dhw 3-way valve active (3wayvalve)\":\"on\",\"dhw charge pump (chargepump)\":\"off\",\"nominal Power "
        "(nompower)\":0,\"total energy (nrgtotal)\":0,\"energy heating (nrgheat)\":0,\"dhw energy (nrg)\":0}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/boiler/info"));
}

void test_5() {
    auto expected_response =
        "[{\"api_data\":\"# HELP emsesp_heatingoff force heating off, boolean, readable, writeable, visible\\n# TYPE emsesp_heatingoff "
        "gauge\\nemsesp_heatingoff 0\\n# HELP emsesp_heatingactive is my heating on?, boolean, readable, visible\\n# TYPE emsesp_heatingactive "
        "gauge\\nemsesp_heatingactive 0\\n# HELP emsesp_tapwateractive tapwater active, boolean, readable, visible\\n# TYPE emsesp_tapwateractive "
        "gauge\\nemsesp_tapwateractive 1\\n# HELP emsesp_selflowtemp selected flow temperature, °C, readable, writeable, visible\\n# TYPE emsesp_selflowtemp "
        "gauge\\nemsesp_selflowtemp 0\\n# HELP emsesp_curflowtemp current flow temperature, °C, readable, visible\\n# TYPE emsesp_curflowtemp "
        "gauge\\nemsesp_curflowtemp 60.20\\n# HELP emsesp_rettemp return temperature, °C, readable, visible\\n# TYPE emsesp_rettemp gauge\\nemsesp_rettemp "
        "48.10\\n# HELP emsesp_syspress system pressure, bar, readable, visible\\n# TYPE emsesp_syspress gauge\\nemsesp_syspress 1.40\\n# HELP emsesp_burngas "
        "gas, boolean, readable, visible\\n# TYPE emsesp_burngas gauge\\nemsesp_burngas 1\\n# HELP emsesp_burngas2 gas stage 2, boolean, readable, visible\\n# "
        "TYPE emsesp_burngas2 gauge\\nemsesp_burngas2 0\\n# HELP emsesp_flamecurr flame current, µA, readable, visible\\n# TYPE emsesp_flamecurr "
        "gauge\\nemsesp_flamecurr 37.40\\n# HELP emsesp_fanwork fan, boolean, readable, visible\\n# TYPE emsesp_fanwork gauge\\nemsesp_fanwork 1\\n# HELP "
        "emsesp_ignwork ignition, boolean, readable, visible\\n# TYPE emsesp_ignwork gauge\\nemsesp_ignwork 0\\n# HELP emsesp_oilpreheat oil preheating, "
        "boolean, readable, visible\\n# TYPE emsesp_oilpreheat gauge\\nemsesp_oilpreheat 0\\n# HELP emsesp_heatingpump heating pump, boolean, readable, "
        "visible\\n# TYPE emsesp_heatingpump gauge\\nemsesp_heatingpump 1\\n# HELP emsesp_selburnpow burner selected max power, %, readable, writeable, "
        "visible\\n# TYPE emsesp_selburnpow gauge\\nemsesp_selburnpow 115\\n# HELP emsesp_curburnpow burner current power, %, readable, visible\\n# TYPE "
        "emsesp_curburnpow gauge\\nemsesp_curburnpow 61\\n# HELP emsesp_ubauptime total UBA operating time, minutes, readable, visible\\n# TYPE "
        "emsesp_ubauptime gauge\\nemsesp_ubauptime 3940268\\n# HELP emsesp_servicecodenumber service code number, readable, visible\\n# TYPE "
        "emsesp_servicecodenumber gauge\\nemsesp_servicecodenumber 201\\n# HELP emsesp_seltemp selected temperature, °C, readable, writeable, visible\\n# TYPE "
        "emsesp_seltemp gauge\\nemsesp_seltemp{circuit=\\\"dhw\\\"} 52\\n# HELP emsesp_comfort comfort, enum, (0: hot; 1: eco; 2: intelligent), readable, "
        "writeable, visible\\n# TYPE emsesp_comfort gauge\\nemsesp_comfort{circuit=\\\"dhw\\\"} 0\\n# HELP emsesp_flowtempoffset flow temperature offset, °C, "
        "readable, writeable, visible\\n# TYPE emsesp_flowtempoffset gauge\\nemsesp_flowtempoffset{circuit=\\\"dhw\\\"} 40\\n# HELP emsesp_chargeoptimization "
        "charge optimization, boolean, readable, writeable, visible\\n# TYPE emsesp_chargeoptimization gauge\\nemsesp_chargeoptimization{circuit=\\\"dhw\\\"} "
        "0\\n# HELP emsesp_circpump circulation pump available, boolean, readable, writeable, visible\\n# TYPE emsesp_circpump "
        "gauge\\nemsesp_circpump{circuit=\\\"dhw\\\"} 0\\n# HELP emsesp_chargetype charging type, enum, (0: chargepump; 1: 3-way valve), readable, visible\\n# "
        "TYPE emsesp_chargetype gauge\\nemsesp_chargetype{circuit=\\\"dhw\\\"} 1\\n# HELP emsesp_hyston hysteresis on temperature, °C, readable, writeable, "
        "visible\\n# TYPE emsesp_hyston gauge\\nemsesp_hyston{circuit=\\\"dhw\\\"} -5\\n# HELP emsesp_disinfectiontemp disinfection temperature, °C, readable, "
        "writeable, visible\\n# TYPE emsesp_disinfectiontemp gauge\\nemsesp_disinfectiontemp{circuit=\\\"dhw\\\"} 70\\n# HELP emsesp_circmode circulation pump "
        "mode, enum, (0: off; 1: 1x3min; 2: 2x3min; 3: 3x3min; 4: 4x3min; 5: 5x3min; 6: 6x3min; 7: continuous), readable, writeable, visible\\n# TYPE "
        "emsesp_circmode gauge\\nemsesp_circmode{circuit=\\\"dhw\\\"} 0\\n# HELP emsesp_circ circulation active, boolean, readable, writeable, visible\\n# "
        "TYPE emsesp_circ gauge\\nemsesp_circ{circuit=\\\"dhw\\\"} 0\\n# HELP emsesp_storagetemp1 storage intern temperature, °C, readable, visible\\n# TYPE "
        "emsesp_storagetemp1 gauge\\nemsesp_storagetemp1{circuit=\\\"dhw\\\"} 53.80\\n# HELP emsesp_activated activated, boolean, readable, writeable, "
        "visible\\n# TYPE emsesp_activated gauge\\nemsesp_activated{circuit=\\\"dhw\\\"} 1\\n# HELP emsesp_3wayvalve 3-way valve active, boolean, readable, "
        "visible\\n# TYPE emsesp_3wayvalve gauge\\nemsesp_3wayvalve{circuit=\\\"dhw\\\"} 1\\n# HELP emsesp_chargepump charge pump, boolean, readable, "
        "visible\\n# TYPE emsesp_chargepump gauge\\nemsesp_chargepump{circuit=\\\"dhw\\\"} 0\\n# HELP emsesp_nompower nominal Power, kW, readable, writeable, "
        "visible\\n# TYPE emsesp_nompower gauge\\nemsesp_nompower 0\\n# HELP emsesp_nrgtotal total energy, kWh, readable, visible\\n# TYPE emsesp_nrgtotal "
        "gauge\\nemsesp_nrgtotal 0\\n# HELP emsesp_nrgheat energy heating, kWh, readable, writeable, visible\\n# TYPE emsesp_nrgheat gauge\\nemsesp_nrgheat "
        "0\\n# HELP emsesp_nrg energy, kWh, readable, writeable, visible\\n# TYPE emsesp_nrg gauge\\nemsesp_nrg{circuit=\\\"dhw\\\"} 0\\n\"}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/boiler/metrics"));
}

void test_6() {
    auto expected_response = "[{\"name\":\"comfort\",\"fullname\":\"dhw "
                             "comfort\",\"circuit\":\"dhw\",\"index\":0,\"enum\":[\"hot\",\"eco\",\"intelligent\"],\"value\":\"hot\",\"type\":\"enum\","
                             "\"readable\":true,\"writeable\":true,\"visible\":true}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/boiler/comfort"));
}

void test_7() {
    auto expected_response = "[{\"api_data\":\"hot\"}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/boiler/comfort/value"));
}

void test_8() {
    auto expected_response = "[{\"api_data\":\"dhw comfort\"}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/boiler/comfort/fullname"));
}

void test_9() {
    auto expected_response = "[{\"name\":\"outdoortemp\",\"fullname\":\"outside "
                             "temperature\",\"circuit\":\"\",\"type\":\"number\",\"uom\":\"°C\",\"state_class\":\"measurement\",\"device_class\":"
                             "\"temperature\",\"readable\":true,\"writeable\":false,\"visible\":true}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/boiler/outdoortemp"));
}

void test_10() {
    auto expected_response = "[{\"name\":\"chargetype\",\"fullname\":\"dhw charging type\",\"circuit\":\"dhw\",\"index\":1,\"enum\":[\"chargepump\",\"3-way "
                             "valve\"],\"value\":\"3-way valve\",\"type\":\"enum\",\"readable\":true,\"writeable\":false,\"visible\":true}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/boiler/dhw/chargetype"));
}

void test_11() {
    auto expected_response = "[{\"api_data\":\"false\"}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/boiler/dhw.chargetype/writeable"));
}

void test_12() {
    auto expected_response = "[{\"api_data\":\"37.4\"}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/boiler/flamecurr/value"));
}

void test_13() {
    auto expected_response =
        "[{\"hc1\":{\"seltemp\":20.5,\"currtemp\":22.8,\"modetype\":\"heat\",\"remotetemp\":null},\"hc2\":{\"seltemp\":20.6,\"currtemp\":22.9,\"modetype\":"
        "\"eco\",\"remotetemp\":null},\"hc3\":{\"seltemp\":20.7,\"currtemp\":23,\"modetype\":\"nofrost\",\"remotetemp\":null},\"dhw\":{}}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/thermostat"));
}

void test_14() {
    auto expected_response = "[{\"seltemp\":20.5,\"currtemp\":22.8,\"modetype\":\"heat\",\"remotetemp\":null}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/thermostat/hc1/values"));
}

void test_15() {
    auto expected_response =
        "[{\"api_data\":\"# HELP emsesp_seltemp selected room temperature, °C, readable, writeable, visible\\n# TYPE emsesp_seltemp "
        "gauge\\nemsesp_seltemp{circuit=\\\"hc1\\\"} 20.50\\n# HELP emsesp_currtemp current room temperature, °C, readable, visible\\n# TYPE emsesp_currtemp "
        "gauge\\nemsesp_currtemp{circuit=\\\"hc1\\\"} 22.80\\n# HELP emsesp_modetype mode type, enum, (0: nofrost; 1: eco; 2: heat), readable, visible\\n# "
        "TYPE emsesp_modetype gauge\\nemsesp_modetype{circuit=\\\"hc1\\\"} 2\\nemsesp_seltemp{circuit=\\\"hc2\\\"} "
        "20.60\\nemsesp_currtemp{circuit=\\\"hc2\\\"} 22.90\\nemsesp_modetype{circuit=\\\"hc2\\\"} 1\\nemsesp_seltemp{circuit=\\\"hc3\\\"} "
        "20.70\\nemsesp_currtemp{circuit=\\\"hc3\\\"} 23\\nemsesp_modetype{circuit=\\\"hc3\\\"} 0\\n\"}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/thermostat/metrics"));
}

void test_16() {
    auto expected_response = "[{\"name\":\"seltemp\",\"fullname\":\"hc1 selected room "
                             "temperature\",\"circuit\":\"hc1\",\"value\":20.5,\"type\":\"number\",\"min\":0,\"max\":30,\"uom\":\"°C\",\"state_class\":"
                             "\"measurement\",\"device_class\":\"temperature\",\"readable\":true,\"writeable\":true,\"visible\":true}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/thermostat/hc1/seltemp"));
}

void test_17() {
    auto expected_response = "[{\"name\":\"seltemp\",\"fullname\":\"hc2 selected room "
                             "temperature\",\"circuit\":\"hc2\",\"value\":20.6,\"type\":\"number\",\"min\":0,\"max\":30,\"uom\":\"°C\",\"state_class\":"
                             "\"measurement\",\"device_class\":\"temperature\",\"readable\":true,\"writeable\":true,\"visible\":true}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/thermostat/hc2/seltemp"));
}

void test_18() {
    auto expected_response = "[{\"test_custom\":0,\"test_read_only\":70,\"test_ram\":\"14\",\"test_seltemp\":\"14\"}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/custom"));
}

void test_19() {
    auto expected_response = "[{\"test_custom\":0,\"test_read_only\":70,\"test_ram\":\"14\",\"test_seltemp\":\"14\"}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/custom/info"));
}

void test_20() {
    auto expected_response = "[{\"name\":\"test_seltemp\",\"fullname\":\"test_seltemp\",\"storage\":\"ram\",\"type\":\"number\",\"readable\":true,"
                             "\"writeable\":true,\"visible\":true,\"ent_cat\":\"diagnostic\",\"value\":\"14\"}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/custom/test_seltemp"));
}

void test_21() {
    auto expected_response = "[{\"api_data\":\"14\"}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/custom/test_seltemp/value"));
}

void test_22() {
    auto expected_response = "[{\"name\":\"test_custom\",\"fullname\":\"test_custom\",\"storage\":\"ems\",\"type\":\"number\",\"readable\":true,\"writeable\":"
                             "true,\"visible\":true,\"device_id\":\"0x08\",\"type_id\":\"0x18\",\"offset\":0,\"factor\":1,\"ent_cat\":\"diagnostic\",\"uom\":"
                             "\"°C\",\"state_class\":\"measurement\",\"device_class\":\"temperature\",\"value\":0}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/custom/test_custom"));
}

void test_23() {
    auto expected_response =
        "[{\"system\":{\"version\":\"dev\",\"uptime\":\"000+00:00:00.000\",\"uptimeSec\":0,\"resetReason\":\"Unknown / "
        "Unknown\",\"txpause\":false,\"gpios_allowed\":\"0, 2, 5, 18, 23\",\"gpios_in_use\":\"0, 2, 5, 18, "
        "23\",\"gpios_available\":\"\"},\"network\":{\"network\":\"WiFi\",\"hostname\":\"ems-esp\",\"RSSI\":-23,\"TxPowerSetting\":0,\"staticIP\":false,"
        "\"lowBandwidth\":false,\"disableSleep\":true,\"enableMDNS\":true,\"enableCORS\":false},\"ntp\":{\"NTPstatus\":\"disconnected\",\"enabled\":true,"
        "\"server\":\"pool.ntp.org\",\"tzLabel\":\"Europe/"
        "London\",\"NTPStatus\":\"disconnected\"},\"ap\":{\"provisionMode\":\"always\",\"ssid\":\"ems-esp\"},\"mqtt\":{\"MQTTStatus\":\"disconnected\","
        "\"MQTTPublishes\":0,\"MQTTQueued\":0,\"MQTTPublishFails\":0,\"MQTTReconnects\":0,\"enabled\":true,\"clientID\":\"ems-esp\",\"keepAlive\":60,"
        "\"cleanSession\":false,\"entityFormat\":1,\"base\":\"ems-esp\",\"discoveryPrefix\":\"homeassistant\",\"discoveryType\":0,\"nestedFormat\":1,"
        "\"haEnabled\":true,\"mqttQos\":0,\"mqttRetain\":false,\"publishTimeHeartbeat\":60,\"publishTimeBoiler\":10,\"publishTimeThermostat\":10,"
        "\"publishTimeSolar\":10,\"publishTimeMixer\":10,\"publishTimeWater\":0,\"publishTimeOther\":10,\"publishTimeSensor\":10,\"publishSingle\":false,"
        "\"publish2command\":false,\"sendResponse\":false},\"syslog\":{\"enabled\":false},\"modbus\":{\"enabled\":false},\"sensor\":{\"temperatureSensors\":3,"
        "\"temperatureSensorReads\":0,\"temperatureSensorFails\":0},\"analog\":{\"enabled\":true,\"analogSensors\":5,\"analogSensorReads\":0,"
        "\"analogSensorFails\":0},\"api\":{\"APICalls\":0,\"APIFails\":0},\"bus\":{\"busStatus\":\"connected\",\"busProtocol\":\"Buderus\","
        "\"busTelegramsReceived\":10,\"busReads\":0,\"busWrites\":0,\"busIncompleteTelegrams\":0,\"busReadsFailed\":0,\"busWritesFailed\":0,"
        "\"busRxLineQuality\":100,\"busTxLineQuality\":100},\"settings\":{\"boardProfile\":\"S32\",\"locale\":\"en\",\"txMode\":5,\"emsBusID\":73,"
        "\"showerTimer\":false,\"showerMinDuration\":180,\"showerAlert\":false,\"hideLed\":false,\"noTokenApi\":false,\"readonlyMode\":false,\"fahrenheit\":"
        "false,\"dallasParasite\":false,\"boolFormat\":1,\"boolDashboard\":1,\"enumFormat\":1,\"analogEnabled\":true,\"telnetEnabled\":true,"
        "\"maxWebLogBuffer\":25,\"modbusEnabled\":false,\"forceHeatingOff\":false,\"developerMode\":false},\"devices\":[{\"type\":\"boiler\",\"name\":\"My "
        "Custom "
        "Boiler\",\"deviceID\":\"0x08\",\"productID\":123,\"brand\":\"\",\"version\":\"01.00\",\"entities\":39,\"handlersReceived\":\"0x18\","
        "\"handlersFetched\":\"0x14 0x33\",\"handlersPending\":\"0xBF 0x10 0x11 0xC2 0xC6 0x15 0x1C 0x19 0x1A 0x35 0x34 0x2A 0xD1 0xE3 0xE4 0xE5 0xE9 0x02E0 "
        "0x2E "
        "0x3B\"},{\"type\":\"thermostat\",\"name\":\"FW120\",\"deviceID\":\"0x10\",\"productID\":192,\"brand\":\"\",\"version\":\"01.00\",\"entities\":12,"
        "\"handlersReceived\":\"0x016F\",\"handlersFetched\":\"0x0170 0x0171\",\"handlersPending\":\"0xA3 0x06 0xA2 0x12 0x13 0x0172 0x0165 "
        "0x0168\"},{\"type\":\"water\",\"name\":\"SM200, "
        "MS200\",\"deviceID\":\"0x28\",\"productID\":164,\"brand\":\"\",\"version\":\"01.00\",\"entities\":3,\"handlersReceived\":\"0x0331\"},{\"type\":"
        "\"temperaturesensor\",\"name\":\"temperaturesensor\",\"entities\":3},{\"type\":\"analogsensor\",\"name\":\"analogsensor\",\"entities\":5},{\"type\":"
        "\"scheduler\",\"name\":\"scheduler\",\"entities\":2},{\"type\":\"custom\",\"name\":\"custom\",\"entities\":4}]}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/system"));
}

void test_24() {
    auto expected_response =
        "[{\"system\":{\"version\":\"dev\",\"uptime\":\"000+00:00:00.000\",\"uptimeSec\":0,\"resetReason\":\"Unknown / "
        "Unknown\",\"txpause\":false,\"gpios_allowed\":\"0, 2, 5, 18, 23\",\"gpios_in_use\":\"0, 2, 5, 18, "
        "23\",\"gpios_available\":\"\"},\"network\":{\"network\":\"WiFi\",\"hostname\":\"ems-esp\",\"RSSI\":-23,\"TxPowerSetting\":0,\"staticIP\":false,"
        "\"lowBandwidth\":false,\"disableSleep\":true,\"enableMDNS\":true,\"enableCORS\":false},\"ntp\":{\"NTPstatus\":\"disconnected\",\"enabled\":true,"
        "\"server\":\"pool.ntp.org\",\"tzLabel\":\"Europe/"
        "London\",\"NTPStatus\":\"disconnected\"},\"ap\":{\"provisionMode\":\"always\",\"ssid\":\"ems-esp\"},\"mqtt\":{\"MQTTStatus\":\"disconnected\","
        "\"MQTTPublishes\":0,\"MQTTQueued\":0,\"MQTTPublishFails\":0,\"MQTTReconnects\":0,\"enabled\":true,\"clientID\":\"ems-esp\",\"keepAlive\":60,"
        "\"cleanSession\":false,\"entityFormat\":1,\"base\":\"ems-esp\",\"discoveryPrefix\":\"homeassistant\",\"discoveryType\":0,\"nestedFormat\":1,"
        "\"haEnabled\":true,\"mqttQos\":0,\"mqttRetain\":false,\"publishTimeHeartbeat\":60,\"publishTimeBoiler\":10,\"publishTimeThermostat\":10,"
        "\"publishTimeSolar\":10,\"publishTimeMixer\":10,\"publishTimeWater\":0,\"publishTimeOther\":10,\"publishTimeSensor\":10,\"publishSingle\":false,"
        "\"publish2command\":false,\"sendResponse\":false},\"syslog\":{\"enabled\":false},\"modbus\":{\"enabled\":false},\"sensor\":{\"temperatureSensors\":3,"
        "\"temperatureSensorReads\":0,\"temperatureSensorFails\":0},\"analog\":{\"enabled\":true,\"analogSensors\":5,\"analogSensorReads\":0,"
        "\"analogSensorFails\":0},\"api\":{\"APICalls\":0,\"APIFails\":0},\"bus\":{\"busStatus\":\"connected\",\"busProtocol\":\"Buderus\","
        "\"busTelegramsReceived\":10,\"busReads\":0,\"busWrites\":0,\"busIncompleteTelegrams\":0,\"busReadsFailed\":0,\"busWritesFailed\":0,"
        "\"busRxLineQuality\":100,\"busTxLineQuality\":100},\"settings\":{\"boardProfile\":\"S32\",\"locale\":\"en\",\"txMode\":5,\"emsBusID\":73,"
        "\"showerTimer\":false,\"showerMinDuration\":180,\"showerAlert\":false,\"hideLed\":false,\"noTokenApi\":false,\"readonlyMode\":false,\"fahrenheit\":"
        "false,\"dallasParasite\":false,\"boolFormat\":1,\"boolDashboard\":1,\"enumFormat\":1,\"analogEnabled\":true,\"telnetEnabled\":true,"
        "\"maxWebLogBuffer\":25,\"modbusEnabled\":false,\"forceHeatingOff\":false,\"developerMode\":false},\"devices\":[{\"type\":\"boiler\",\"name\":\"My "
        "Custom "
        "Boiler\",\"deviceID\":\"0x08\",\"productID\":123,\"brand\":\"\",\"version\":\"01.00\",\"entities\":39,\"handlersReceived\":\"0x18\","
        "\"handlersFetched\":\"0x14 0x33\",\"handlersPending\":\"0xBF 0x10 0x11 0xC2 0xC6 0x15 0x1C 0x19 0x1A 0x35 0x34 0x2A 0xD1 0xE3 0xE4 0xE5 0xE9 0x02E0 "
        "0x2E "
        "0x3B\"},{\"type\":\"thermostat\",\"name\":\"FW120\",\"deviceID\":\"0x10\",\"productID\":192,\"brand\":\"\",\"version\":\"01.00\",\"entities\":12,"
        "\"handlersReceived\":\"0x016F\",\"handlersFetched\":\"0x0170 0x0171\",\"handlersPending\":\"0xA3 0x06 0xA2 0x12 0x13 0x0172 0x0165 "
        "0x0168\"},{\"type\":\"water\",\"name\":\"SM200, "
        "MS200\",\"deviceID\":\"0x28\",\"productID\":164,\"brand\":\"\",\"version\":\"01.00\",\"entities\":3,\"handlersReceived\":\"0x0331\"},{\"type\":"
        "\"temperaturesensor\",\"name\":\"temperaturesensor\",\"entities\":3},{\"type\":\"analogsensor\",\"name\":\"analogsensor\",\"entities\":5},{\"type\":"
        "\"scheduler\",\"name\":\"scheduler\",\"entities\":2},{\"type\":\"custom\",\"name\":\"custom\",\"entities\":4}]}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/system/info"));
}

void test_25() {
    auto expected_response =
        "[{\"api_data\":\"# HELP emsesp_system_uptimesec uptimeSec\\n# TYPE emsesp_system_uptimesec gauge\\nemsesp_system_uptimesec 0\\n# HELP "
        "emsesp_system_txpause txpause\\n# TYPE emsesp_system_txpause gauge\\nemsesp_system_txpause 0\\n# HELP emsesp_system_info info\\n# TYPE "
        "emsesp_system_info gauge\\nemsesp_system_info{version=\\\"dev\\\", resetreason=\\\"Unknown / Unknown\\\", gpios_allowed=\\\"0, 2, 5, 18, 23\\\", "
        "gpios_in_use=\\\"0, 2, 5, 18, 23\\\"} 1\\n# HELP emsesp_network_rssi RSSI\\n# TYPE emsesp_network_rssi gauge\\nemsesp_network_rssi -23\\n# HELP "
        "emsesp_network_txpowersetting TxPowerSetting\\n# TYPE emsesp_network_txpowersetting gauge\\nemsesp_network_txpowersetting 0\\n# HELP "
        "emsesp_network_staticip staticIP\\n# TYPE emsesp_network_staticip gauge\\nemsesp_network_staticip 0\\n# HELP emsesp_network_lowbandwidth "
        "lowBandwidth\\n# TYPE emsesp_network_lowbandwidth gauge\\nemsesp_network_lowbandwidth 0\\n# HELP emsesp_network_disablesleep disableSleep\\n# TYPE "
        "emsesp_network_disablesleep gauge\\nemsesp_network_disablesleep 1\\n# HELP emsesp_network_enablemdns enableMDNS\\n# TYPE emsesp_network_enablemdns "
        "gauge\\nemsesp_network_enablemdns 1\\n# HELP emsesp_network_enablecors enableCORS\\n# TYPE emsesp_network_enablecors "
        "gauge\\nemsesp_network_enablecors 0\\n# HELP emsesp_network_info info\\n# TYPE emsesp_network_info gauge\\nemsesp_network_info{network=\\\"WiFi\\\", "
        "hostname=\\\"ems-esp\\\"} 1\\n# HELP emsesp_ntp_enabled enabled\\n# TYPE emsesp_ntp_enabled gauge\\nemsesp_ntp_enabled 1\\n# HELP emsesp_ntp_info "
        "info\\n# TYPE emsesp_ntp_info gauge\\nemsesp_ntp_info{ntpstatus=\\\"disconnected\\\", server=\\\"pool.ntp.org\\\", tzlabel=\\\"Europe/London\\\"} "
        "1\\n# HELP emsesp_ap_info info\\n# TYPE emsesp_ap_info gauge\\nemsesp_ap_info{provisionmode=\\\"always\\\", ssid=\\\"ems-esp\\\"} 1\\n# HELP "
        "emsesp_mqtt_mqttpublishes MQTTPublishes\\n# TYPE emsesp_mqtt_mqttpublishes gauge\\nemsesp_mqtt_mqttpublishes 0\\n# HELP emsesp_mqtt_mqttqueued "
        "MQTTQueued\\n# TYPE emsesp_mqtt_mqttqueued gauge\\nemsesp_mqtt_mqttqueued 0\\n# HELP emsesp_mqtt_mqttpublishfails MQTTPublishFails\\n# TYPE "
        "emsesp_mqtt_mqttpublishfails gauge\\nemsesp_mqtt_mqttpublishfails 0\\n# HELP emsesp_mqtt_mqttreconnects MQTTReconnects\\n# TYPE "
        "emsesp_mqtt_mqttreconnects gauge\\nemsesp_mqtt_mqttreconnects 0\\n# HELP emsesp_mqtt_enabled enabled\\n# TYPE emsesp_mqtt_enabled "
        "gauge\\nemsesp_mqtt_enabled 1\\n# HELP emsesp_mqtt_keepalive keepAlive\\n# TYPE emsesp_mqtt_keepalive gauge\\nemsesp_mqtt_keepalive 60\\n# HELP "
        "emsesp_mqtt_cleansession cleanSession\\n# TYPE emsesp_mqtt_cleansession gauge\\nemsesp_mqtt_cleansession 0\\n# HELP emsesp_mqtt_entityformat "
        "entityFormat\\n# TYPE emsesp_mqtt_entityformat gauge\\nemsesp_mqtt_entityformat 1\\n# HELP emsesp_mqtt_discoverytype discoveryType\\n# TYPE "
        "emsesp_mqtt_discoverytype gauge\\nemsesp_mqtt_discoverytype 0\\n# HELP emsesp_mqtt_nestedformat nestedFormat\\n# TYPE emsesp_mqtt_nestedformat "
        "gauge\\nemsesp_mqtt_nestedformat 1\\n# HELP emsesp_mqtt_haenabled haEnabled\\n# TYPE emsesp_mqtt_haenabled gauge\\nemsesp_mqtt_haenabled 1\\n# HELP "
        "emsesp_mqtt_mqttqos mqttQos\\n# TYPE emsesp_mqtt_mqttqos gauge\\nemsesp_mqtt_mqttqos 0\\n# HELP emsesp_mqtt_mqttretain mqttRetain\\n# TYPE "
        "emsesp_mqtt_mqttretain gauge\\nemsesp_mqtt_mqttretain 0\\n# HELP emsesp_mqtt_publishtimeheartbeat publishTimeHeartbeat\\n# TYPE "
        "emsesp_mqtt_publishtimeheartbeat gauge\\nemsesp_mqtt_publishtimeheartbeat 60\\n# HELP emsesp_mqtt_publishtimeboiler publishTimeBoiler\\n# TYPE "
        "emsesp_mqtt_publishtimeboiler gauge\\nemsesp_mqtt_publishtimeboiler 10\\n# HELP emsesp_mqtt_publishtimethermostat publishTimeThermostat\\n# TYPE "
        "emsesp_mqtt_publishtimethermostat gauge\\nemsesp_mqtt_publishtimethermostat 10\\n# HELP emsesp_mqtt_publishtimesolar publishTimeSolar\\n# TYPE "
        "emsesp_mqtt_publishtimesolar gauge\\nemsesp_mqtt_publishtimesolar 10\\n# HELP emsesp_mqtt_publishtimemixer publishTimeMixer\\n# TYPE "
        "emsesp_mqtt_publishtimemixer gauge\\nemsesp_mqtt_publishtimemixer 10\\n# HELP emsesp_mqtt_publishtimewater publishTimeWater\\n# TYPE "
        "emsesp_mqtt_publishtimewater gauge\\nemsesp_mqtt_publishtimewater 0\\n# HELP emsesp_mqtt_publishtimeother publishTimeOther\\n# TYPE "
        "emsesp_mqtt_publishtimeother gauge\\nemsesp_mqtt_publishtimeother 10\\n# HELP emsesp_mqtt_publishtimesensor publishTimeSensor\\n# TYPE "
        "emsesp_mqtt_publishtimesensor gauge\\nemsesp_mqtt_publishtimesensor 10\\n# HELP emsesp_mqtt_publishsingle publishSingle\\n# TYPE "
        "emsesp_mqtt_publishsingle gauge\\nemsesp_mqtt_publishsingle 0\\n# HELP emsesp_mqtt_publish2command publish2command\\n# TYPE "
        "emsesp_mqtt_publish2command gauge\\nemsesp_mqtt_publish2command 0\\n# HELP emsesp_mqtt_sendresponse sendResponse\\n# TYPE emsesp_mqtt_sendresponse "
        "gauge\\nemsesp_mqtt_sendresponse 0\\n# HELP emsesp_mqtt_info info\\n# TYPE emsesp_mqtt_info gauge\\nemsesp_mqtt_info{mqttstatus=\\\"disconnected\\\", "
        "clientid=\\\"ems-esp\\\", base=\\\"ems-esp\\\", discoveryprefix=\\\"homeassistant\\\"} 1\\n# HELP emsesp_syslog_enabled enabled\\n# TYPE "
        "emsesp_syslog_enabled gauge\\nemsesp_syslog_enabled 0\\n# HELP emsesp_modbus_enabled enabled\\n# TYPE emsesp_modbus_enabled "
        "gauge\\nemsesp_modbus_enabled 0\\n# HELP emsesp_sensor_temperaturesensors temperatureSensors\\n# TYPE emsesp_sensor_temperaturesensors "
        "gauge\\nemsesp_sensor_temperaturesensors 3\\n# HELP emsesp_sensor_temperaturesensorreads temperatureSensorReads\\n# TYPE "
        "emsesp_sensor_temperaturesensorreads gauge\\nemsesp_sensor_temperaturesensorreads 0\\n# HELP emsesp_sensor_temperaturesensorfails "
        "temperatureSensorFails\\n# TYPE emsesp_sensor_temperaturesensorfails gauge\\nemsesp_sensor_temperaturesensorfails 0\\n# HELP emsesp_analog_enabled "
        "enabled\\n# TYPE emsesp_analog_enabled gauge\\nemsesp_analog_enabled 1\\n# HELP emsesp_analog_analogsensors analogSensors\\n# TYPE "
        "emsesp_analog_analogsensors gauge\\nemsesp_analog_analogsensors 5\\n# HELP emsesp_analog_analogsensorreads analogSensorReads\\n# TYPE "
        "emsesp_analog_analogsensorreads gauge\\nemsesp_analog_analogsensorreads 0\\n# HELP emsesp_analog_analogsensorfails analogSensorFails\\n# TYPE "
        "emsesp_analog_analogsensorfails gauge\\nemsesp_analog_analogsensorfails 0\\n# HELP emsesp_api_apicalls APICalls\\n# TYPE emsesp_api_apicalls "
        "gauge\\nemsesp_api_apicalls 0\\n# HELP emsesp_api_apifails APIFails\\n# TYPE emsesp_api_apifails gauge\\nemsesp_api_apifails 0\\n# HELP "
        "emsesp_bus_bustelegramsreceived busTelegramsReceived\\n# TYPE emsesp_bus_bustelegramsreceived gauge\\nemsesp_bus_bustelegramsreceived 10\\n# HELP "
        "emsesp_bus_busreads busReads\\n# TYPE emsesp_bus_busreads gauge\\nemsesp_bus_busreads 0\\n# HELP emsesp_bus_buswrites busWrites\\n# TYPE "
        "emsesp_bus_buswrites gauge\\nemsesp_bus_buswrites 0\\n# HELP emsesp_bus_busincompletetelegrams busIncompleteTelegrams\\n# TYPE "
        "emsesp_bus_busincompletetelegrams gauge\\nemsesp_bus_busincompletetelegrams 0\\n# HELP emsesp_bus_busreadsfailed busReadsFailed\\n# TYPE "
        "emsesp_bus_busreadsfailed gauge\\nemsesp_bus_busreadsfailed 0\\n# HELP emsesp_bus_buswritesfailed busWritesFailed\\n# TYPE emsesp_bus_buswritesfailed "
        "gauge\\nemsesp_bus_buswritesfailed 0\\n# HELP emsesp_bus_busrxlinequality busRxLineQuality\\n# TYPE emsesp_bus_busrxlinequality "
        "gauge\\nemsesp_bus_busrxlinequality 100\\n# HELP emsesp_bus_bustxlinequality busTxLineQuality\\n# TYPE emsesp_bus_bustxlinequality "
        "gauge\\nemsesp_bus_bustxlinequality 100\\n# HELP emsesp_bus_info info\\n# TYPE emsesp_bus_info gauge\\nemsesp_bus_info{busstatus=\\\"connected\\\", "
        "busprotocol=\\\"Buderus\\\"} 1\\n# HELP emsesp_settings_txmode txMode\\n# TYPE emsesp_settings_txmode gauge\\nemsesp_settings_txmode 5\\n# HELP "
        "emsesp_settings_emsbusid emsBusID\\n# TYPE emsesp_settings_emsbusid gauge\\nemsesp_settings_emsbusid 73\\n# HELP emsesp_settings_showertimer "
        "showerTimer\\n# TYPE emsesp_settings_showertimer gauge\\nemsesp_settings_showertimer 0\\n# HELP emsesp_settings_showerminduration "
        "showerMinDuration\\n# TYPE emsesp_settings_showerminduration gauge\\nemsesp_settings_showerminduration 180\\n# HELP emsesp_settings_showeralert "
        "showerAlert\\n# TYPE emsesp_settings_showeralert gauge\\nemsesp_settings_showeralert 0\\n# HELP emsesp_settings_hideled hideLed\\n# TYPE "
        "emsesp_settings_hideled gauge\\nemsesp_settings_hideled 0\\n# HELP emsesp_settings_notokenapi noTokenApi\\n# TYPE emsesp_settings_notokenapi "
        "gauge\\nemsesp_settings_notokenapi 0\\n# HELP emsesp_settings_readonlymode readonlyMode\\n# TYPE emsesp_settings_readonlymode "
        "gauge\\nemsesp_settings_readonlymode 0\\n# HELP emsesp_settings_fahrenheit fahrenheit\\n# TYPE emsesp_settings_fahrenheit "
        "gauge\\nemsesp_settings_fahrenheit 0\\n# HELP emsesp_settings_dallasparasite dallasParasite\\n# TYPE emsesp_settings_dallasparasite "
        "gauge\\nemsesp_settings_dallasparasite 0\\n# HELP emsesp_settings_boolformat boolFormat\\n# TYPE emsesp_settings_boolformat "
        "gauge\\nemsesp_settings_boolformat 1\\n# HELP emsesp_settings_booldashboard boolDashboard\\n# TYPE emsesp_settings_booldashboard "
        "gauge\\nemsesp_settings_booldashboard 1\\n# HELP emsesp_settings_enumformat enumFormat\\n# TYPE emsesp_settings_enumformat "
        "gauge\\nemsesp_settings_enumformat 1\\n# HELP emsesp_settings_analogenabled analogEnabled\\n# TYPE emsesp_settings_analogenabled "
        "gauge\\nemsesp_settings_analogenabled 1\\n# HELP emsesp_settings_telnetenabled telnetEnabled\\n# TYPE emsesp_settings_telnetenabled "
        "gauge\\nemsesp_settings_telnetenabled 1\\n# HELP emsesp_settings_maxweblogbuffer maxWebLogBuffer\\n# TYPE emsesp_settings_maxweblogbuffer "
        "gauge\\nemsesp_settings_maxweblogbuffer 25\\n# HELP emsesp_settings_modbusenabled modbusEnabled\\n# TYPE emsesp_settings_modbusenabled "
        "gauge\\nemsesp_settings_modbusenabled 0\\n# HELP emsesp_settings_forceheatingoff forceHeatingOff\\n# TYPE emsesp_settings_forceheatingoff "
        "gauge\\nemsesp_settings_forceheatingoff 0\\n# HELP emsesp_settings_developermode developerMode\\n# TYPE emsesp_settings_developermode "
        "gauge\\nemsesp_settings_developermode 0\\n# HELP emsesp_settings_info info\\n# TYPE emsesp_settings_info "
        "gauge\\nemsesp_settings_info{boardprofile=\\\"S32\\\", locale=\\\"en\\\"} 1\\n# HELP emsesp_device_productid productID\\n# TYPE "
        "emsesp_device_productid gauge\\nemsesp_device_productid{type=\\\"boiler\\\", name=\\\"My Custom Boiler\\\", deviceid=\\\"0x08\\\", "
        "version=\\\"01.00\\\"} 123\\n# HELP emsesp_device_entities entities\\n# TYPE emsesp_device_entities "
        "gauge\\nemsesp_device_entities{type=\\\"boiler\\\", name=\\\"My Custom Boiler\\\", deviceid=\\\"0x08\\\", version=\\\"01.00\\\"} "
        "39\\nemsesp_device_productid{type=\\\"thermostat\\\", name=\\\"FW120\\\", deviceid=\\\"0x10\\\", version=\\\"01.00\\\"} "
        "192\\nemsesp_device_entities{type=\\\"thermostat\\\", name=\\\"FW120\\\", deviceid=\\\"0x10\\\", version=\\\"01.00\\\"} "
        "12\\nemsesp_device_productid{type=\\\"water\\\", name=\\\"SM200, MS200\\\", deviceid=\\\"0x28\\\", version=\\\"01.00\\\"} "
        "164\\nemsesp_device_entities{type=\\\"water\\\", name=\\\"SM200, MS200\\\", deviceid=\\\"0x28\\\", version=\\\"01.00\\\"} "
        "3\\nemsesp_device_entities{type=\\\"temperaturesensor\\\", name=\\\"temperaturesensor\\\"} 3\\nemsesp_device_entities{type=\\\"analogsensor\\\", "
        "name=\\\"analogsensor\\\"} 5\\nemsesp_device_entities{type=\\\"scheduler\\\", name=\\\"scheduler\\\"} 2\\nemsesp_device_entities{type=\\\"custom\\\", "
        "name=\\\"custom\\\"} 4\\n\"}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/system/metrics"));
}

void test_26() {
    auto expected_response =
        "[{\"name\":\"locale\",\"circuit\":\"settings\",\"readable\":true,\"writeable\":false,\"visible\":true,\"value\":\"en\",\"type\":\"string\"}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/system/settings/locale"));
}

void test_27() {
    auto expected_response = "[{}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/system/fetch"));
}

void test_28() {
    auto expected_response = "[{\"network\":\"WiFi\",\"hostname\":\"ems-esp\",\"RSSI\":\"-23\",\"TxPowerSetting\":\"0\",\"staticIP\":\"false\","
                             "\"lowBandwidth\":\"false\",\"disableSleep\":\"true\",\"enableMDNS\":\"true\",\"enableCORS\":\"false\"}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/system/network/values"));
}

void test_29() {
    auto expected_response = "[{\"test_scheduler\":\"on\"}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/scheduler"));
}

void test_30() {
    auto expected_response = "[{\"test_scheduler\":\"on\"}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/scheduler/info"));
}

void test_31() {
    auto expected_response = "[{\"name\":\"test_scheduler\",\"fullname\":\"test_scheduler\",\"type\":\"boolean\",\"value\":\"on\",\"time\":\"12:00\","
                             "\"command\":\"system/fetch\",\"cmd_data\":\"10\",\"readable\":true,\"writeable\":true,\"visible\":true}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/scheduler/test_scheduler"));
}

void test_32() {
    auto expected_response = "[{\"test_tempsensor1\":12.3,\"test_tempsensor2\":45.6,\"gateway_temperature\":28.1}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/temperaturesensor"));
}

void test_33() {
    auto expected_response = "[{\"test_tempsensor1\":12.3,\"test_tempsensor2\":45.6,\"gateway_temperature\":28.1}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/temperaturesensor/info"));
}

void test_34() {
    auto expected_response = "[{\"id\":\"0B_0C0D_0E0F_1011\",\"name\":\"test_tempsensor2\",\"fullname\":\"test_tempsensor2\",\"value\":45.6,\"type\":"
                             "\"number\",\"uom\":\"°C\",\"readable\":true,\"writeable\":false,\"visible\":true,\"is_system\":false}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/temperaturesensor/test_tempsensor2"));
}

void test_35() {
    auto expected_response = "[{\"id\":\"0B_0C0D_0E0F_1011\",\"name\":\"test_tempsensor2\",\"fullname\":\"test_tempsensor2\",\"value\":45.6,\"type\":"
                             "\"number\",\"uom\":\"°C\",\"readable\":true,\"writeable\":false,\"visible\":true,\"is_system\":false}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/temperaturesensor/0B_0C0D_0E0F_1011"));
}

void test_36() {
    auto expected_response = "[{\"api_data\":\"45.6\"}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/temperaturesensor/test_tempsensor2/value"));
}

void test_37() {
    auto expected_response = "[{\"test_analogsensor1\":0,\"test_analogsensor2\":1,\"test_analogsensor3\":0,\"test_analogsensor4\":0,\"test_analogsensor5\":0}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/analogsensor"));
}

void test_38() {
    auto expected_response = "[{\"test_analogsensor1\":0,\"test_analogsensor2\":1,\"test_analogsensor3\":0,\"test_analogsensor4\":0,\"test_analogsensor5\":0}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/analogsensor/info"));
}

void test_39() {
    auto expected_response =
        "[{\"name\":\"test_analogsensor1\",\"fullname\":\"test_analogsensor1\",\"gpio\":36,\"type\":\"number\",\"analog\":\"adc\",\"value\":0,\"readable\":"
        "true,\"writeable\":false,\"visible\":true,\"is_system\":false,\"offset\":0,\"factor\":0.2,\"uom\":\"mV\"}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/analogsensor/test_analogsensor1"));
}

void test_40() {
    auto expected_response = "[{\"api_data\":\"0\"}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/analogsensor/test_analogsensor1/offset"));
}

void test_41() {
    auto expected_response = "[{\"message\":\"unknown device boiler2\"}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/boiler2"));
}

void test_42() {
    auto expected_response = "[{}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/boiler/bad/value"));
}

void test_43() {
    auto expected_response = "[{\"message\":\"no attribute 'valu' in comfort\"}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/boiler/comfort/valu"));
}

void test_44() {
    auto expected_response = "[{\"message\":\"no 'settings' in system\"}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/system/settings/locale2"));
}

void test_45() {
    auto expected_response = "[{\"message\":\"no 'settings2' in system\"}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/system/settings2"));
}

void test_46() {
    auto expected_response = "[{\"message\":\"no 'settings2' in system\"}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/system/settings2/locale2"));
}

void test_47() {
    auto expected_response = "[{\"message\":\"no 'test_scheduler2' in scheduler\"}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/scheduler/test_scheduler2"));
}

void test_48() {
    auto expected_response = "[{\"message\":\"no attribute 'val' in test_scheduler\"}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/scheduler/test_scheduler/val"));
}

void test_49() {
    auto expected_response = "[{\"message\":\"no 'test_scheduler2' in scheduler\"}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/scheduler/test_scheduler2/val2"));
}

void test_50() {
    auto expected_response = "[{\"message\":\"no 'test_seltemp2' in custom\"}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/custom/test_seltemp2"));
}

void test_51() {
    auto expected_response = "[{\"message\":\"Command test_seltemp failed (Error)\"}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/custom/test_seltemp/val"));
}

void test_52() {
    auto expected_response = "[{\"message\":\"no 'test_sensor20' in temperaturesensor\"}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/temperaturesensor/test_sensor20"));
}

void test_53() {
    auto expected_response = "[{\"message\":\"no '0b_0c0d_0e0f_xxxx' in temperaturesensor\"}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/temperaturesensor/0B_0C0D_0E0F_XXXX"));
}

void test_54() {
    auto expected_response = "[{\"message\":\"no attribute 'bad' in test_tempsensor2\"}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/temperaturesensor/test_tempsensor2/bad"));
}

void test_55() {
    auto expected_response = "[{\"message\":\"no attribute 'bad' in test_analogsensor1\"}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/analogsensor/test_analogsensor1/bad"));
}

void test_56() {
    auto expected_response = "[{\"message\":\"no 'test_analog10' in analogsensor\"}]";
    TEST_ASSERT_EQUAL_STRING(expected_response, call_url("/api/analogsensor/test_analog10"));
}

void test_57() {
    auto expected_response = "[{\"message\":\"no 'test_analog10' in analogsensor\"}]";
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
    RUN_TEST(test_53);
    RUN_TEST(test_54);
    RUN_TEST(test_55);
    RUN_TEST(test_56);
    RUN_TEST(test_57);
}

// ---------- END - CUT HERE ----------
