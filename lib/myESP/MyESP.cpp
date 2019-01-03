/*
 * MyESP - my ESP helper class to handle Wifi, MDNS, MQTT and Telnet
 * 
 * Paul Derbyshire - December 2018
 *
 * Some ideas from https://github.com/JoaoLopesF/ESP8266-RemoteDebug-Telnet
 * Ideas from Espurna https://github.com/xoseperez/espurna
 */

#include "MyESP.h"

// constructor
MyESP::MyESP() {
    _app_hostname               = strdup("MyESP");
    _app_name                   = strdup("MyESP");
    _app_version                = strdup("1.0.0");
    _boottime                   = strdup("unknown");
    _extern_WIFICallback        = NULL;
    _extern_WIFICallbackSet     = false;
    _consoleCallbackProjectCmds = NULL;
    _helpProjectCmds            = NULL;
    _helpProjectCmds_count      = 0;
    _mqtt_host                  = NULL;
    _mqtt_password              = NULL;
    _mqtt_username              = NULL;
    _wifi_password              = NULL;
    _wifi_ssid                  = NULL;
    _mqtt_reconnect_delay       = MQTT_RECONNECT_DELAY_MIN;
    _suspendMessages            = true;
    _command                    = (char *)malloc(TELNET_MAX_COMMAND_LENGTH); // reserve buffer for Serial/Telnet commands
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
    if (!_suspendMessages)
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
    if ((code == MESSAGE_CONNECTED) || (code == MESSAGE_ACCESSPOINT_CREATED)) {
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

        if (WiFi.getMode() & WIFI_AP) {
            myDebug_P(PSTR("[WIFI] MODE AP --------------------------------------"));
            myDebug_P(PSTR("[WIFI] SSID  %s"), jw.getAPSSID().c_str());
            myDebug_P(PSTR("[WIFI] IP    %s"), WiFi.softAPIP().toString().c_str());
            myDebug_P(PSTR("[WIFI] MAC   %s"), WiFi.softAPmacAddress().c_str());
        }

        // start MDNS
        if (MDNS.begin((char *)hostname.c_str())) {
            myDebug_P(PSTR("[MDNS] OK"));
        } else {
            myDebug_P(PSTR("[MDNS] FAIL"));
        }

        // call any final custom settings
        if (_extern_WIFICallbackSet) {
            // myDebug_P(PSTR("[WIFI] calling custom wifi settings function"));
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

    // check for bootime, something specific I fetch as an acknolwegdemtn from Home Assistant
    if (strcmp(topic, MQTT_TOPIC_START) == 0) {
        myDebug_P(PSTR("[MQTT] boottime: %s"), message);
        setBoottime(message);
        return;
    }

    // Send message event to custom service
    (_mqtt_callback)(MQTT_MESSAGE_EVENT, topic, message);
}

// MQTT subscribe
void MyESP::mqttSubscribe(const char * topic) {
    if (mqttClient.connected() && (strlen(topic) > 0)) {
        char s[100];
        snprintf(s, sizeof(s), "%s%s/%s", MQTT_BASE, _app_hostname, topic);
        unsigned int packetId = mqttClient.subscribe(s, MQTT_QOS);
        myDebug_P(PSTR("[MQTT] Subscribing to %s (PID %d)"), s, packetId);
    }
}

// MQTT unsubscribe
void MyESP::mqttUnsubscribe(const char * topic) {
    if (mqttClient.connected() && (strlen(topic) > 0)) {
        char s[100];
        snprintf(s, sizeof(s), "%s%s/%s", MQTT_BASE, _app_hostname, topic);
        unsigned int packetId = mqttClient.unsubscribe(s);
        myDebug_P(PSTR("[MQTT] Unsubscribing to %s (PID %d)"), s, packetId);
    }
}

// MQTT Publish
void MyESP::mqttPublish(const char * topic, const char * payload) {
    char s[600];
    snprintf(s, sizeof(s), "%s%s/%s", MQTT_BASE, _app_hostname, topic);
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
    snprintf(s, sizeof(s), "%s%s/%s", MQTT_BASE, _app_hostname, MQTT_TOPIC_START);
    mqttClient.publish(s, MQTT_QOS, false, MQTT_TOPIC_START_PAYLOAD);
#endif

    // call custom
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
    jw.enableAPFallback(true);                 // AP mode only as fallback, but disabled
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

// returns boottime
char * MyESP::getBoottime() {
    return _boottime;
}

// Set callback of sketch function to process project messages
void MyESP::consoleSetCallBackProjectCmds(command_t * cmds, uint8_t count, void (*callback)()) {
    _helpProjectCmds            = cmds;     // command list
    _helpProjectCmds_count      = count;    // number of commands
    _consoleCallbackProjectCmds = callback; // external function to handle commands
}

void MyESP::_telnetConnected() {
    myDebug_P(PSTR("[TELNET] Telnet connection established"));
    _consoleShowHelp(); // Show the initial message
}

void MyESP::_telnetDisconnected() {
    myDebug_P(PSTR("[TELNET] Telnet connection closed"));
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
#if defined(ARDUINO_ARCH_ESP32)
    String hostname = String(WiFi.getHostname());
#else
    String hostname = WiFi.hostname();
#endif


    SerialAndTelnet.println("*********************************");
    SerialAndTelnet.println("*  Console and Log Monitoring   *");
    SerialAndTelnet.println("*********************************");
    SerialAndTelnet.printf("* %s version %s\n\r", _app_name, _app_version);
    SerialAndTelnet.printf("* Hostname: %s    IP: %s     MAC: %s\n\r",
                           hostname.c_str(),
                           WiFi.localIP().toString().c_str(),
                           WiFi.macAddress().c_str());
    SerialAndTelnet.printf("* Connected to WiFi AP: %s\n\r", WiFi.SSID().c_str());
    SerialAndTelnet.printf("* Boot time: %s\n\r", _boottime);
    SerialAndTelnet.printf("* Free RAM: %d bytes\n\r", ESP.getFreeHeap());
#ifdef DEBUG_SUPPORT
    SerialAndTelnet.println("* !! in DEBUG_SUPPORT mode !!\n\r");
#endif

    SerialAndTelnet.println("*\n\r* Commands:\n\r*  ?=this help, CTRL-D=quit, $=show free memory, !=reboot ESP, &=suspend all messages");

    // print custom commands if available. Take from progmem
    if (_consoleCallbackProjectCmds) {
        for (uint8_t i = 0; i < _helpProjectCmds_count; i++) {
            SerialAndTelnet.print(FPSTR("*  "));
            SerialAndTelnet.print(FPSTR(_helpProjectCmds[i].key));
            for (uint8_t j = 0; j < (8 - strlen(_helpProjectCmds[i].key)); j++) {
                SerialAndTelnet.print(FPSTR(" ")); // padding
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

// Get last command received
char * MyESP::consoleGetLastCommand() {
    return _command;
}

// Process user command over telnet
void MyESP::consoleProcessCommand() {
    uint8_t cmd = _command[0];

    // Process the command
    if (cmd == '?') {
        _consoleShowHelp(); // Show help
    } else if (cmd == '$') {
        myDebug("* Free RAM (bytes): %d", ESP.getFreeHeap());
    } else if (cmd == '!') {
        resetESP();
    } else if (cmd == '&') {
        myDebug("Suspend all messages is %s", !_suspendMessages ? "disabled" : "enabled");
        _suspendMessages = !_suspendMessages; // toggle
    } else {
        // custom Project commands
        if (_consoleCallbackProjectCmds) {
            _consoleCallbackProjectCmds();
        }
    }

    if (!_suspendMessages) {
        myDebug("Warning, all log messages have been supsended. Use & to re-enable.");
    }
}

// sends a MQTT notification message to Home Assistant
void MyESP::sendHANotification(const char * message) {
    char payload[48];
    snprintf(payload, sizeof(payload), "%s : %s", _app_hostname, message);
    myDebug_P(PSTR("[MQTT] Sending HA notification %s"), payload);
    mqttClient.publish(MQTT_NOTIFICATION, MQTT_QOS, false, payload);
}

// send specific command to HA via MQTT
// format is: home/<hostname>/command with payload <cmd>
void MyESP::sendHACommand(const char * cmd) {
    myDebug_P(PSTR("[MQTT] Sending HA command %s"), cmd);
    char topic[48];
    snprintf(topic, sizeof(topic), "%s%s/%s", MQTT_BASE, _app_hostname, MQTT_TOPIC_COMMAND);
    mqttClient.publish(topic, MQTT_QOS, false, cmd);
}

// handler for Telnet
void MyESP::_telnetHandle() {
    SerialAndTelnet.handle();

    char last = ' '; // To avoid processing double "\r\n"

    while (SerialAndTelnet.available()) {
        char character = SerialAndTelnet.read(); // Get character

        // check for ctrl-D
        if ((character == 0xEC) || (character == 0x04)) {
            SerialAndTelnet.disconnectClient();
        }

        // if we reached our buffer limit, send what we have
        if (strlen(_command) >= TELNET_MAX_COMMAND_LENGTH) {
            consoleProcessCommand();                        // Process the command
            memset(_command, 0, TELNET_MAX_COMMAND_LENGTH); // reset for next command
        }

        // Check for newline (CR or LF)
        if (_isCRLF(character) == true) {
            if (_isCRLF(last) == false) {
                if (strlen(_command) > 0) {
                    consoleProcessCommand(); // Process the command
                }
            }
            memset(_command, 0, TELNET_MAX_COMMAND_LENGTH); // reset for next command

        } else if (isPrintable(character)) {
            // Concat char to end of buffer
            uint16_t len      = strlen(_command);
            _command[len]     = character;
            _command[len + 1] = '\0';
        }
        last = character; // remember last char
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

// Is CR or LF ?
bool MyESP::_isCRLF(char character) {
    return (character == '\r' || character == '\n');
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
void MyESP::setup(char * app_hostname,
                  char * app_name,
                  char * app_version,
                  char * wifi_ssid,
                  char * wifi_password,
                  char * mqtt_host,
                  char * mqtt_username,
                  char * mqtt_password) {
    // get general params first
    _app_hostname = strdup(app_hostname);
    _app_name     = strdup(app_name);
    _app_version  = strdup(app_version);

    // Check SSID too long or missing
    if (!wifi_ssid || *wifi_ssid == 0x00 || strlen(wifi_ssid) > 31) {
        _wifi_ssid = NULL;
    } else {
        _wifi_ssid = strdup(wifi_ssid);
    }

    // Check PASS too long
    if (wifi_password && strlen(wifi_password) > 63) {
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

    // call setup of the services...
    _telnet_setup(); // Telnet setup
    _wifi_setup();   // WIFI setup
    _mqtt_setup();   // MQTT Setup
    _mdns_setup();   // MDNS setup
    _ota_setup();    // OTA setup
}

/*
 * Loop. This is called as often as possible and it handles wifi, telnet, mqtt etc
 */
void MyESP::loop() {
    jw.loop();           // WiFi
    _telnetHandle();     // Telnet/Debugger
    ArduinoOTA.handle(); // OTA
    _mqttConnect();      // MQTT

    yield(); // ...and breath
}

MyESP myESP;
