/*
 * MyESP - my ESP helper class to handle Wifi, MDNS, MQTT and Telnet
 * 
 * Paul Derbyshire - December 2018
 *
 * Ideas borrowed from Espurna https://github.com/xoseperez/espurna
 */

#include "MyESP.h"

// constructor
MyESP::MyESP() {
    _app_hostname           = strdup("MyESP");
    _app_name               = strdup("MyESP");
    _app_version            = strdup("1.0.0");
    _boottime               = strdup("unknown");
    _extern_WIFICallback    = NULL;
    _extern_WIFICallbackSet = false;
    _telnetcommand_callback = NULL;
    _telnet_callback        = NULL;
    _helpProjectCmds        = NULL;
    _helpProjectCmds_count  = 0;
    _mqtt_host              = NULL;
    _mqtt_password          = NULL;
    _mqtt_username          = NULL;
    _wifi_password          = NULL;
    _wifi_ssid              = NULL;
    _mqttbase               = NULL;
    _suspendOutput          = false;
    _mqtt_reconnect_delay   = MQTT_RECONNECT_DELAY_MIN;
    _command                = (char *)malloc(TELNET_MAX_COMMAND_LENGTH); // reserve buffer for Serial/Telnet commands
}

MyESP::~MyESP() {
    end();
}

// end
void MyESP::end() {
    free(_command);
    SerialAndTelnet.end();
    jw.disconnect();
}

// general debug to the telnet or serial channels
void MyESP::myDebug(const char * format, ...) {
    if (_suspendOutput)
        return;

    va_list args;
    va_start(args, format);
    char   test[1];
    int    len    = ets_vsnprintf(test, 1, format, args) + 1;
    char * buffer = new char[len];
    ets_vsnprintf(buffer, len, format, args);
    va_end(args);

    SerialAndTelnet.println(buffer);

    delete[] buffer;
}

// for flashmemory. Must use PSTR()
void MyESP::myDebug_P(PGM_P format_P, ...) {
    if (_suspendOutput)
        return;

    char format[strlen_P(format_P) + 1];
    memcpy_P(format, format_P, sizeof(format));

    va_list args;
    va_start(args, format_P);
    char   test[1];
    int    len    = ets_vsnprintf(test, 1, format, args) + 1;
    char * buffer = new char[len];
    ets_vsnprintf(buffer, len, format, args);
    va_end(args);

    // capture & print timestamp
    char timestamp[10] = {0};
    snprintf_P(timestamp, sizeof(timestamp), PSTR("[%06lu] "), millis() % 1000000);
    SerialAndTelnet.print(timestamp);

    SerialAndTelnet.println(buffer);

    delete[] buffer;
}

// called when WiFi is connected, and used to start MDNS
void MyESP::_wifiCallback(justwifi_messages_t code, char * parameter) {
    if ((code == MESSAGE_CONNECTED)) {
#if defined(ARDUINO_ARCH_ESP32)
        String hostname = String(WiFi.getHostname());
#else
        String hostname = WiFi.hostname();
#endif

        myDebug_P(PSTR("[WIFI] SSID  %s"), WiFi.SSID().c_str());
        myDebug_P(PSTR("[WIFI] CH    %d"), WiFi.channel());
        myDebug_P(PSTR("[WIFI] RSSI  %d"), WiFi.RSSI());
        myDebug_P(PSTR("[WIFI] IP    %s"), WiFi.localIP().toString().c_str());
        myDebug_P(PSTR("[WIFI] MAC   %s"), WiFi.macAddress().c_str());
        myDebug_P(PSTR("[WIFI] GW    %s"), WiFi.gatewayIP().toString().c_str());
        myDebug_P(PSTR("[WIFI] MASK  %s"), WiFi.subnetMask().toString().c_str());
        myDebug_P(PSTR("[WIFI] DNS   %s"), WiFi.dnsIP().toString().c_str());
        myDebug_P(PSTR("[WIFI] HOST  %s"), hostname.c_str());

        // start MDNS
        if (MDNS.begin((char *)hostname.c_str())) {
            myDebug_P(PSTR("[MDNS] OK"));
        } else {
            myDebug_P(PSTR("[MDNS] FAIL"));
        }

        // call any final custom settings
        if (_extern_WIFICallbackSet) {
            _extern_WIFICallback(); // call callback to set any custom things
        }
    }

    if (code == MESSAGE_ACCESSPOINT_CREATED) {
        myDebug_P(PSTR("[WIFI] MODE AP --------------------------------------"));
        myDebug_P(PSTR("[WIFI] SSID  %s"), jw.getAPSSID().c_str());
        myDebug_P(PSTR("[WIFI] IP    %s"), WiFi.softAPIP().toString().c_str());
        myDebug_P(PSTR("[WIFI] MAC   %s"), WiFi.softAPmacAddress().c_str());

        // call any final custom settings
        if (_extern_WIFICallbackSet) {
            _extern_WIFICallback(); // call callback to set any custom things
        }
    }

    if (code == MESSAGE_CONNECTING) {
        myDebug_P(PSTR("[WIFI] Connecting to %s"), parameter);
    }

    if (code == MESSAGE_CONNECT_FAILED) {
        myDebug_P(PSTR("[WIFI] Could not connect to %s"), parameter);
    }

    if (code == MESSAGE_DISCONNECTED) {
        myDebug_P(PSTR("[WIFI] Disconnected"));
    }
}

// received MQTT message
// we send this to the call back function. Important to parse are the event strings such as MQTT_MESSAGE_EVENT and MQTT_CONNECT_EVENT
void MyESP::_mqttOnMessage(char * topic, char * payload, size_t len) {
    if (len == 0)
        return;

    char message[len + 1];
    strlcpy(message, (char *)payload, len + 1);

    // myDebug_P(PSTR("[MQTT] Received %s => %s"), topic, message);

    // check for our default ones
    if ((strcmp(topic, MQTT_HA) == 0) && (strcmp(message, MQTT_TOPIC_START_PAYLOAD) == 0)) {
        myDebug_P(PSTR("[MQTT] HA rebooted - restarting device"));
        resetESP();
        return;
    }

    // topics are in format MQTT_BASE/HOSTNAME/TOPIC
    char * topic_magnitude = strrchr(topic, '/'); // strip out everything until last /
    if (topic_magnitude != nullptr) {
        topic = topic_magnitude + 1;
    }

    // check for bootime, something specific I fetch as an acknowledgement from Home Assistant
    if (strcmp(topic, MQTT_TOPIC_START) == 0) {
        myDebug_P(PSTR("[MQTT] received boottime: %s"), message);
        setBoottime(message);
        return;
    }

    // Send message event to custom service
    (_mqtt_callback)(MQTT_MESSAGE_EVENT, topic, message);
}

// MQTT subscribe
// to MQTT_BASE/app_hostname/topic
void MyESP::mqttSubscribe(const char * topic) {
    if (mqttClient.connected() && (strlen(topic) > 0)) {
        char s[100];
        snprintf(s, sizeof(s), "%s/%s/%s", _mqttbase, _app_hostname, topic);
        unsigned int packetId = mqttClient.subscribe(s, MQTT_QOS);
        myDebug_P(PSTR("[MQTT] Subscribing to %s (PID %d)"), s, packetId);
    }
}

// MQTT unsubscribe
// to MQTT_BASE/app_hostname/topic
void MyESP::mqttUnsubscribe(const char * topic) {
    if (mqttClient.connected() && (strlen(topic) > 0)) {
        char s[100];
        snprintf(s, sizeof(s), "%s/%s/%s", _mqttbase, _app_hostname, topic);
        unsigned int packetId = mqttClient.unsubscribe(s);
        myDebug_P(PSTR("[MQTT] Unsubscribing to %s (PID %d)"), s, packetId);
    }
}

// MQTT Publish
void MyESP::mqttPublish(const char * topic, const char * payload) {
    char s[MQTT_MAX_SIZE];
    snprintf(s, sizeof(s), "%s/%s/%s", _mqttbase, _app_hostname, topic);
    // myDebug_P(PSTR("[MQTT] Sending pubish to %s with payload %s"), s, payload);
    mqttClient.publish(s, MQTT_QOS, false, payload);
}

// MQTT onConnect - when a connect is established automatically subscribe to my HA topics
void MyESP::_mqttOnConnect() {
    myDebug_P(PSTR("[MQTT] Connected"));
    _mqtt_reconnect_delay = MQTT_RECONNECT_DELAY_MIN;

#ifndef NO_HA
    // standard subscribes for HA (Home Assistant)
    mqttClient.subscribe(MQTT_HA, MQTT_QOS); // to "ha"
    mqttSubscribe(MQTT_TOPIC_START);         // to home/<hostname>/start

    // send specific start command to HA via MQTT, which returns the boottime
    char s[48];
    snprintf(s, sizeof(s), "%s/%s/%s", _mqttbase, _app_hostname, MQTT_TOPIC_START);
    mqttClient.publish(s, MQTT_QOS, false, MQTT_TOPIC_START_PAYLOAD);
#endif

    // call custom function to handle mqtt receives
    (_mqtt_callback)(MQTT_CONNECT_EVENT, NULL, NULL);
}

// MQTT setup
void MyESP::_mqtt_setup() {
    if (!_mqtt_host) {
        myDebug_P(PSTR("[MQTT] disabled"));
    }

    _mqtt_reconnect_delay = MQTT_RECONNECT_DELAY_MIN;

    mqttClient.onConnect([this](bool sessionPresent) { _mqttOnConnect(); });

    mqttClient.onDisconnect([this](AsyncMqttClientDisconnectReason reason) {
        if (reason == AsyncMqttClientDisconnectReason::TCP_DISCONNECTED) {
            myDebug_P(PSTR("[MQTT] TCP Disconnected"));
            (_mqtt_callback)(MQTT_DISCONNECT_EVENT, NULL, NULL); // call callback with disconnect
        }
        if (reason == AsyncMqttClientDisconnectReason::MQTT_IDENTIFIER_REJECTED) {
            myDebug_P(PSTR("[MQTT] Identifier Rejected"));
        }
        if (reason == AsyncMqttClientDisconnectReason::MQTT_SERVER_UNAVAILABLE) {
            myDebug_P(PSTR("[MQTT] Server unavailable"));
        }
        if (reason == AsyncMqttClientDisconnectReason::MQTT_MALFORMED_CREDENTIALS) {
            myDebug_P(PSTR("[MQTT] Malformed credentials"));
        }
        if (reason == AsyncMqttClientDisconnectReason::MQTT_NOT_AUTHORIZED) {
            myDebug_P(PSTR("[MQTT] Not authorized"));
        }
    });

    //mqttClient.onSubscribe([this](uint16_t packetId, uint8_t qos) { myDebug_P(PSTR("[MQTT] Subscribe ACK for PID %d"), packetId); });

    //mqttClient.onPublish([this](uint16_t packetId) { myDebug_P(PSTR("[MQTT] Publish ACK for PID %d"), packetId); });

    mqttClient.onMessage(
        [this](char * topic, char * payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
            _mqttOnMessage(topic, payload, len);
        });
}

// WiFI setup
void MyESP::_wifi_setup() {
    jw.setHostname(_app_hostname); // Set WIFI hostname (otherwise it would be ESP-XXXXXX)
    jw.subscribe([this](justwifi_messages_t code, char * parameter) { _wifiCallback(code, parameter); });
    jw.enableAP(false);
    jw.setConnectTimeout(WIFI_CONNECT_TIMEOUT);
    jw.setReconnectTimeout(WIFI_RECONNECT_INTERVAL);
    jw.enableAPFallback(true);                 // AP mode only as fallback
    jw.enableSTA(true);                        // Enable STA mode (connecting to a router)
    jw.enableScan(false);                      // Configure it to scan available networks and connect in order of dBm
    jw.cleanNetworks();                        // Clean existing network configuration
    jw.addNetwork(_wifi_ssid, _wifi_password); // Add a network
}

// MDNS setup
void MyESP::_mdns_setup() {
    MDNS.addService("telnet", "tcp", TELNETSPY_PORT);

    // for OTA discovery
    MDNS.addServiceTxt("arduino", "tcp", "app_name", (const char *)_app_name);
    MDNS.addServiceTxt("arduino", "tcp", "app_version", (const char *)_app_version);
    MDNS.addServiceTxt("arduino", "tcp", "mac", WiFi.macAddress());
    {
        char buffer[6] = {0};
        itoa(ESP.getFlashChipRealSize() / 1024, buffer, 10);
        MDNS.addServiceTxt("arduino", "tcp", "mem_size", (const char *)buffer);
    }
    {
        char buffer[6] = {0};
        itoa(ESP.getFlashChipSize() / 1024, buffer, 10);
        MDNS.addServiceTxt("arduino", "tcp", "sdk_size", (const char *)buffer);
    }
    {
        char buffer[6] = {0};
        itoa(ESP.getFreeSketchSpace(), buffer, 10);
        MDNS.addServiceTxt("arduino", "tcp", "free_space", (const char *)buffer);
    }
}

// OTA Setup
void MyESP::_ota_setup() {
    if (!_wifi_ssid) {
        return;
    }

    ArduinoOTA.setPort(OTA_PORT);
    ArduinoOTA.setHostname(_app_hostname);
    ArduinoOTA.onStart([this]() { myDebug_P(PSTR("[OTA] Start")); });
    ArduinoOTA.onEnd([this]() { myDebug_P(PSTR("[OTA] Done, restarting...")); });
    ArduinoOTA.onProgress([this](unsigned int progress, unsigned int total) {
        static unsigned int _progOld;
        unsigned int        _prog = (progress / (total / 100));
        if (_prog != _progOld) {
            myDebug_P(PSTR("[OTA] Progress: %u%%\r"), _prog);
            _progOld = _prog;
        }
    });

    ArduinoOTA.onError([this](ota_error_t error) {
        if (error == OTA_AUTH_ERROR)
            myDebug_P(PSTR("Auth Failed"));
        else if (error == OTA_BEGIN_ERROR)
            myDebug_P(PSTR("Begin Failed"));
        else if (error == OTA_CONNECT_ERROR)
            myDebug_P(PSTR("Connect Failed"));
        else if (error == OTA_RECEIVE_ERROR)
            myDebug_P(PSTR("Receive Failed"));
        else if (error == OTA_END_ERROR)
            myDebug_P(PSTR("End Failed"));
    });

    ArduinoOTA.begin();
}

// sets boottime
void MyESP::setBoottime(char * boottime) {
    if (_boottime) {
        free(_boottime);
    }
    _boottime = strdup(boottime);
}

// sets boottime
void MyESP::setMQTTbase(char * mqttbase) {
    if (_mqttbase) {
        free(_mqttbase);
    }
    _mqttbase = strdup(mqttbase);
}

// Set callback of sketch function to process project messages
void MyESP::setTelnetCommands(command_t * cmds, uint8_t count, telnetcommand_callback_f callback) {
    _helpProjectCmds        = cmds;     // command list
    _helpProjectCmds_count  = count;    // number of commands
    _telnetcommand_callback = callback; // external function to handle commands
}

void MyESP::setTelnetCallback(telnet_callback_f callback) {
    _telnet_callback = callback;
}

void MyESP::_telnetConnected() {
    myDebug_P(PSTR("[TELNET] Telnet connection established"));
    _consoleShowHelp(); // Show the initial message
    if (_telnet_callback) {
        (_telnet_callback)(TELNET_EVENT_CONNECT); // call callback
    }
}

void MyESP::_telnetDisconnected() {
    myDebug_P(PSTR("[TELNET] Telnet connection closed"));
    if (_telnet_callback) {
        (_telnet_callback)(TELNET_EVENT_DISCONNECT); // call callback
    }
}

// Initialize the telnet server
void MyESP::_telnet_setup() {
    SerialAndTelnet.setWelcomeMsg("");
    SerialAndTelnet.setCallbackOnConnect([this]() { _telnetConnected(); });
    SerialAndTelnet.setCallbackOnDisconnect([this]() { _telnetDisconnected(); });
    SerialAndTelnet.begin(115200);
    SerialAndTelnet.setDebugOutput(false);

#ifndef DEBUG_SUPPORT
    SerialAndTelnet.setSerial(NULL);
#endif

    // init command buffer for console commands
    memset(_command, 0, TELNET_MAX_COMMAND_LENGTH);
}

// Show help of commands
void MyESP::_consoleShowHelp() {
    SerialAndTelnet.printf("\n\r* Connected to: %s version %s\n\r", _app_name, _app_version);

    if (WiFi.getMode() & WIFI_AP) {
        SerialAndTelnet.printf("* ESP8266 is in AP mode with SSID %s\n\r", jw.getAPSSID().c_str());
    } else {
#if defined(ARDUINO_ARCH_ESP32)
        String hostname = String(WiFi.getHostname());
#else
        String hostname = WiFi.hostname();
#endif
        SerialAndTelnet.printf("* Hostname: %s    IP: %s     MAC: %s\n\r",
                               hostname.c_str(),
                               WiFi.localIP().toString().c_str(),
                               WiFi.macAddress().c_str());
        SerialAndTelnet.printf("* Connected to WiFi SSID: %s\n\r", WiFi.SSID().c_str());
        SerialAndTelnet.printf("* Boot time: %s\n\r", _boottime);
    }

    SerialAndTelnet.printf("* Free RAM: %d bytes\n\r", ESP.getFreeHeap());
#ifdef DEBUG_SUPPORT
    SerialAndTelnet.println("* Warning: in DEBUG_SUPPORT mode!");
#endif

    SerialAndTelnet.println("*\n\r* Commands:\n\r*  ?=help, CTRL-D=quit, !=reboot");
    SerialAndTelnet.println(FPSTR("*  set <wifi_ssid | wifi_password | mqtt_host | mqtt_username | mqtt_password> [value]"));
    SerialAndTelnet.println(FPSTR("*  set erase"));
    SerialAndTelnet.println(FPSTR("*"));

    // print custom commands if available. Take from progmem
    if (_telnetcommand_callback) {
        // find the longest key length so we can right align it
        uint8_t max_len = 0;
        for (uint8_t i = 0; i < _helpProjectCmds_count; i++) {
            if (strlen(_helpProjectCmds[i].key) > max_len)
                max_len = strlen(_helpProjectCmds[i].key);
        }

        for (uint8_t i = 0; i < _helpProjectCmds_count; i++) {
            SerialAndTelnet.print(FPSTR("*  "));
            SerialAndTelnet.print(FPSTR(_helpProjectCmds[i].key));
            for (uint8_t j = 0; j < ((max_len + 5) - strlen(_helpProjectCmds[i].key)); j++) { // account for longest string length
                SerialAndTelnet.print(FPSTR(" "));                                            // padding
            }
            SerialAndTelnet.println(FPSTR(_helpProjectCmds[i].description));
        }
    }
}

// reset / restart
void MyESP::resetESP() {
    myDebug_P(PSTR("* Reboot ESP..."));
    end();

#if defined(ARDUINO_ARCH_ESP32)
    ESP.reset(); // for ESP8266 only
#else
    ESP.restart();
#endif
}

// sends a MQTT notification message to Home Assistant (HA)
void MyESP::sendHANotification(const char * message) {
    char payload[48];
    snprintf(payload, sizeof(payload), "%s : %s", _app_hostname, message);
    myDebug_P(PSTR("[MQTT] Sending HA notification %s"), payload);
    mqttClient.publish(MQTT_HA_NOTIFICATION, MQTT_QOS, false, payload);
}

// send specific command to Home Assistant (HA) via MQTT
// format is: home/<hostname>/command with payload <cmd>
void MyESP::sendHACommand(const char * cmd) {
    myDebug_P(PSTR("[MQTT] Sending HA command %s"), cmd);
    char topic[48];
    snprintf(topic, sizeof(topic), "%s/%s/%s", _mqttbase, _app_hostname, MQTT_TOPIC_COMMAND);
    mqttClient.publish(topic, MQTT_QOS, false, cmd);
}

char * MyESP::_telnet_readWord() {
    char * word = strtok(NULL, ", \n");
    return word;
}

// change settings - always as strings
// messy code but effective since we don't have too many settings
void MyESP::_changeSetting(const char * setting, const char * value) {
    bool ok = false;

    // validate 2nd argument
    if (strcmp(setting, "erase") == 0) {
        _fs_eraseConfig();
        return;
    }

    if (strcmp(setting, "wifi_ssid") == 0) {
        if (_wifi_ssid)
            free(_wifi_ssid);
        _wifi_ssid = NULL; // just to be sure
        if (value) {
            _wifi_ssid = strdup(value);
        }
        ok = true;
    }

    if (strcmp(setting, "wifi_password") == 0) {
        if (_wifi_password)
            free(_wifi_password);
        _wifi_password = NULL; // just to be sure
        if (value) {
            _wifi_password = strdup(value);
        }
        ok = true;
    }

    if (strcmp(setting, "mqtt_host") == 0) {
        if (_mqtt_host)
            free(_mqtt_host);
        _mqtt_host = NULL; // just to be sure
        if (value) {
            _mqtt_host = strdup(value);
        }
        ok = true;
    }

    if (strcmp(setting, "mqtt_username") == 0) {
        if (_mqtt_username)
            free(_mqtt_username);
        _mqtt_username = NULL; // just to be sure
        if (value) {
            _mqtt_username = strdup(value);
        }
        ok = true;
    }

    if (strcmp(setting, "mqtt_password") == 0) {
        if (_mqtt_password)
            free(_mqtt_password);
        _mqtt_password = NULL; // just to be sure
        if (value) {
            _mqtt_password = strdup(value);
        }
        ok = true;
    }

    if (!ok) {
        SerialAndTelnet.println("\nInvalid parameter for set command.");
        return;
    }

    // check for 2 params
    if (value == nullptr) {
        SerialAndTelnet.printf("%s setting deleted\n\r", setting);
    } else {
        // 3 params
        SerialAndTelnet.printf("%s changed to %s\n\r", setting, value);
    }

    if (_fs_saveConfig()) {
        SerialAndTelnet.println("Changes will have effect after the next restart. Please reboot using ! command");
    }
}

void MyESP::_telnetCommand(char * commandLine) {
    // count the number of arguments
    char *   str   = commandLine;
    bool     state = false;
    unsigned wc    = 0;
    while (*str) {
        if (*str == ' ' || *str == '\n' || *str == '\t') {
            state = false;
        } else if (state == false) {
            state = true;
            ++wc;
        }
        ++str;
    }

    // check first for reserved commands
    char * temp             = strdup(commandLine); // because strotok kills original string buffer
    char * ptrToCommandName = strtok((char *)temp, ", \n");
    if (strcmp(ptrToCommandName, "set") == 0) {
        if (wc == 1) {
            SerialAndTelnet.println("\n\Stored settings:");
            SerialAndTelnet.printf("  wifi_ssid=%s\n\r", (!_wifi_ssid) ? "<not set>" : _wifi_ssid);
            SerialAndTelnet.printf("  wifi_password=");
            if (!_wifi_password) {
                SerialAndTelnet.print("<not set>");
            } else {
                for (uint8_t i = 0; i < strlen(_wifi_password); i++)
                    SerialAndTelnet.print("*");
            }
            SerialAndTelnet.printf("\n\r  mqtt_host=%s\n\r", (!_mqtt_host) ? "<not set>" : _mqtt_host);
            SerialAndTelnet.printf("  mqtt_username=%s\n\r", (!_mqtt_username) ? "<not set>" : _mqtt_username);
            SerialAndTelnet.printf("  mqtt_password=");
            if (!_mqtt_password) {
                SerialAndTelnet.print("<not set>");
            } else {
                for (uint8_t i = 0; i < strlen(_mqtt_password); i++)
                    SerialAndTelnet.print("*");
            }
            SerialAndTelnet.println("\n\r\n\rUsage: set <setting> <value>");
        } else if (wc == 2) {
            char * setting = _telnet_readWord();
            _changeSetting(setting, NULL);
        } else if (wc == 3) {
            char * setting = _telnet_readWord();
            char * value   = _telnet_readWord();
            _changeSetting(setting, value);
        }
        return;
    }

    // call callback function
    (_telnetcommand_callback)(wc, commandLine);
}

// handler for Telnet
void MyESP::_telnetHandle() {
    SerialAndTelnet.handle();

    static uint8_t charsRead = 0;
    // read asynchronously until full command input
    while (SerialAndTelnet.available()) {
        char c = SerialAndTelnet.read();
#ifdef DEBUG_SUPPORT
        Serial.print(c);
#endif
        switch (c) {
        case '\r': // likely have full command in buffer now, commands are terminated by CR and/or LF
        case '\n':
            _command[charsRead] = '\0'; // null terminate our command char array
            if (charsRead > 0) {
                charsRead      = 0; //  is static, so have to reset
                _suspendOutput = false;
                _telnetCommand(_command);
            }
            break;
        case '\b':               // handle backspace in input: put a space in last char
            if (charsRead > 0) { // and adjust commandLine and charsRead
                _command[--charsRead] = '\0';
                SerialAndTelnet << byte('\b') << byte(' ') << byte('\b'); //no idea how this works, found it on the Internet
            }
            break;
        case '?':
            _consoleShowHelp();
            break;
        case '!':
            resetESP();
            break;
        case 0x04: // EOT
            myDebug_P(PSTR("* exiting telnet session"));
            SerialAndTelnet.disconnectClient();
            break;
        default:
            _suspendOutput = true;
            c              = tolower(c);
            if (charsRead < TELNET_MAX_COMMAND_LENGTH) {
                _command[charsRead++] = c;
            }
            _command[charsRead] = '\0'; // just in case
            break;
        }
    }
}

// sets a custom function to run when wifi is started
void MyESP::setWIFICallback(void (*callback)()) {
    _extern_WIFICallback    = callback;
    _extern_WIFICallbackSet = true;
}

// the mqtt callback for after connecting to subscribe and process incoming messages
void MyESP::setMQTTCallback(mqtt_callback_f callback) {
    _mqtt_callback = callback;
}

// ensure we have a connection to MQTT broker
void MyESP::_mqttConnect() {
    if (!_mqtt_host || mqttClient.connected() || (WiFi.status() != WL_CONNECTED)) {
        return;
    }

    // Check reconnect interval
    static unsigned long last = 0;
    if (millis() - last < _mqtt_reconnect_delay)
        return;
    last = millis();

    // Increase the reconnect delay
    _mqtt_reconnect_delay += MQTT_RECONNECT_DELAY_STEP;
    if (_mqtt_reconnect_delay > MQTT_RECONNECT_DELAY_MAX) {
        _mqtt_reconnect_delay = MQTT_RECONNECT_DELAY_MAX;
    }

    mqttClient.setServer(_mqtt_host, MQTT_PORT);
    mqttClient.setClientId(_app_hostname);

    if (_mqtt_username && _mqtt_password) {
        myDebug_P(PSTR("[MQTT] Connecting to MQTT using user %s"), _mqtt_username);
        mqttClient.setCredentials(_mqtt_username, _mqtt_password);
    } else {
        myDebug_P(PSTR("[MQTT] Connecting to MQTT..."));
    }

    // Connect to the MQTT broker
    mqttClient.connect();
}

// Setup everything we need
void MyESP::setConnection(char * wifi_ssid, char * wifi_password, char * mqtt_host, char * mqtt_username, char * mqtt_password) {
    // Check SSID too long or missing
    if (!wifi_ssid || *wifi_ssid == 0x00 || strlen(wifi_ssid) > 31) {
        _wifi_ssid = NULL;
    } else {
        _wifi_ssid = strdup(wifi_ssid);
    }

    // Check PASS too long
    if (!wifi_password || *wifi_ssid == 0x00 || strlen(wifi_password) > 31) {
        _wifi_password = NULL;
    } else {
        _wifi_password = strdup(wifi_password);
    }

    // can be empty
    if (!mqtt_host || *mqtt_host == 0x00) {
        _mqtt_host = NULL;
    } else {
        _mqtt_host = strdup(mqtt_host);
    }

    // mqtt username and password can be empty
    if (!mqtt_username || *mqtt_username == 0x00) {
        _mqtt_username = NULL;
    } else {
        _mqtt_username = strdup(mqtt_username);
    }

    // can be empty
    if (!mqtt_password || *mqtt_password == 0x00) {
        _mqtt_password = NULL;
    } else {
        _mqtt_password = strdup(mqtt_password);
    }
}

// print contents of file
void MyESP::_fs_printConfig() {
    File configFile = SPIFFS.open("/config.json", "r");

    myDebug_P(PSTR("[FS] Contents...."));

    while (configFile.available()) {
        SerialAndTelnet.print((char)configFile.read());
    }
    SerialAndTelnet.println();
    configFile.close();
}

// format File System
void MyESP::_fs_eraseConfig() {
    myDebug_P(PSTR("[FS] Erasing settings. Please wait. ESP will automatically restart when finished."));

    if (SPIFFS.format()) {
        resetESP();
    }
}

// load from spiffs
bool MyESP::_fs_loadConfig() {
    File configFile = SPIFFS.open("/config.json", "r");
    if (!configFile) {
        myDebug_P(PSTR("[FS] Failed to open config file"));
        return false;
    }

    size_t size = configFile.size();
    if (size > 1024) {
        myDebug_P(PSTR("[FS] Config file size is too large"));
        return false;
    }

    // assign buffer
    std::unique_ptr<char[]> buf(new char[size]);

    // use configFile.readString
    configFile.readBytes(buf.get(), size);

    StaticJsonBuffer<300> jsonBuffer; // https://arduinojson.org/v5/assistant/
    JsonObject &          json = jsonBuffer.parseObject(buf.get());

    const char * value;

    value      = json["wifi_ssid"];
    _wifi_ssid = (value) ? strdup(value) : NULL;

    value          = json["wifi_password"];
    _wifi_password = (value) ? strdup(value) : NULL;

    value      = json["mqtt_host"];
    _mqtt_host = (value) ? strdup(value) : NULL;

    value          = json["mqtt_username"];
    _mqtt_username = (value) ? strdup(value) : NULL;

    value          = json["mqtt_password"];
    _mqtt_password = (value) ? strdup(value) : NULL;

    configFile.close();

    return true;
}

// save settings to spiffs
bool MyESP::_fs_saveConfig() {
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject &          json = jsonBuffer.createObject();

    json["wifi_ssid"]     = _wifi_ssid;
    json["wifi_password"] = _wifi_password;
    json["mqtt_host"]     = _mqtt_host;
    json["mqtt_username"] = _mqtt_username;
    json["mqtt_password"] = _mqtt_password;

    File configFile = SPIFFS.open("/config.json", "w");
    if (!configFile) {
        myDebug_P(PSTR("[FS] Failed to open config file for writing"));
        return false;
    }

    json.printTo(configFile);

    return true;
}

// init the SPIFF file system and load the config
// if it doesn't exist try and create it
void MyESP::_fs_setup() {
    if (!SPIFFS.begin()) {
        myDebug_P(PSTR("[FS] Failed to mount the file system"));
        return;
    }

    // load the config file. if it doesn't exist create it with anything that was specified
    if (!_fs_loadConfig()) {
        _fs_saveConfig();
    }
}

// register new instance
void MyESP::begin(char * app_hostname, char * app_name, char * app_version) {
    _app_hostname = strdup(app_hostname);
    _app_name     = strdup(app_name);
    _app_version  = strdup(app_version);

    // call setup of the services...
    _telnet_setup(); // Telnet setup
    _fs_setup();     // SPIFFS setup
    _wifi_setup();   // WIFI setup
    _mqtt_setup();   // MQTT Setup
    _mdns_setup();   // MDNS setup
    _ota_setup();    // OTA setup
}

/*
 * Loop. This is called as often as possible and it handles wifi, telnet, mqtt etc
 */
void MyESP::loop() {
    jw.loop();       // WiFi
    _telnetHandle(); // Telnet/Debugger

    if (WiFi.getMode() & WIFI_AP) {
        return;
    }

    ArduinoOTA.handle(); // OTA
    _mqttConnect();      // MQTT

    yield(); // ...and breath
}

MyESP myESP;
