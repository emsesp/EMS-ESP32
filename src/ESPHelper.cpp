/*
    Based off :
    1) ESPHelper.cpp - Copyright (c) 2017 ItKindaWorks Inc All right reserved. github.com/ItKindaWorks
    2) https://github.com/JoaoLopesF/ESP8266-RemoteDebug-Telnet

*/

#include "ESPHelper.h"

WiFiServer telnetServer(TELNET_PORT);

//initializer with single netInfo network
ESPHelper::ESPHelper(netInfo * startingNet) {
    //disconnect from and previous wifi networks
    WiFi.softAPdisconnect();
    WiFi.disconnect();

    //setup current network information
    _currentNet = *startingNet;

    //validate various bits of network/MQTT info

    //network pass
    if (_currentNet.pass[0] == '\0') {
        _passSet = false;
    } else {
        _passSet = true;
    }

    //ssid
    if (_currentNet.ssid[0] == '\0') {
        _ssidSet = false;
    } else {
        _ssidSet = true;
    }

    //mqtt host
    if (_currentNet.mqttHost[0] == '\0') {
        _mqttSet = false;
    } else {
        _mqttSet = true;
    }

    //mqtt port
    if (_currentNet.mqttPort == 0) {
        _currentNet.mqttPort = 1883;
    }

    //mqtt username
    if (_currentNet.mqttUser[0] == '\0') {
        _mqttUserSet = false;
    } else {
        _mqttUserSet = true;
    }

    //mqtt password
    if (_currentNet.mqttPass[0] == '\0') {
        _mqttPassSet = false;
    } else {
        _mqttPassSet = true;
    }

    //disable hopping on single network
    _hoppingAllowed = false;

    //disable ota by default
    _useOTA = false;
}

//start the wifi & mqtt systems and attempt connection (currently blocking)
//true on: parameter check validated
//false on: parameter check failed
bool ESPHelper::begin(const char * hostname) {
#ifdef USE_SERIAL1
    Serial1.begin(115200);
    Serial1.setDebugOutput(true);
#endif

#ifdef USE_SERIAL
    Serial.begin(115200);
    Serial.setDebugOutput(true);
#endif

    // set hostname first
    strcpy(_hostname, hostname);
    OTA_enable();

    setBoottime("<unknown>");

    if (_ssidSet) {
        strcpy(_clientName, hostname);

        /*
        // Generate client name based on MAC address and last 8 bits of microsecond counter

        _clientName += "esp-";
        uint8_t mac[6];
        WiFi.macAddress(mac);
        _clientName += macToStr(mac);
        */

// set hostname
// can ping by <hostname> or on Windows10 it's <hostname>.
#if defined(ESP8266)
        WiFi.hostname(_hostname);
#elif defined(ESP32)
        WiFi.setHostname(_hostname);
#endif

        //set the wifi mode to station and begin the wifi (connect using either ssid or ssid/pass)
        WiFi.mode(WIFI_STA);
        if (_passSet) {
            WiFi.begin(_currentNet.ssid, _currentNet.pass);
        } else {
            WiFi.begin(_currentNet.ssid);
        }

        //as long as an mqtt ip has been set create an instance of PubSub for client
        if (_mqttSet) {
            //make mqtt client use either the secure or non-secure wifi client depending on the setting
            if (_useSecureClient) {
                client = PubSubClient(_currentNet.mqttHost, _currentNet.mqttPort, wifiClientSecure);
            } else {
                client = PubSubClient(_currentNet.mqttHost, _currentNet.mqttPort, wifiClient);
            }

            //set the mqtt message callback if needed
            if (_mqttCallbackSet) {
                client.setCallback(_mqttCallback);
            }
        }

        //define a dummy instance of mqtt so that it is instantiated if no mqtt ip is set
        else {
            //make mqtt client use either the secure or non-secure wifi client depending on the setting
            //(this shouldnt be needed if making a dummy connection since the idea would be that there wont be mqtt in this case)
            if (_useSecureClient) {
                client = PubSubClient("192.168.1.255", _currentNet.mqttPort, wifiClientSecure);
            } else {
                client = PubSubClient("192.168.1.255", _currentNet.mqttPort, wifiClient);
            }
        }

        //ota event handlers
        ArduinoOTA.onStart([]() { /* ota start code */ });
        ArduinoOTA.onEnd([]() {
            //on ota end we disconnect from wifi cleanly before restarting.
            WiFi.softAPdisconnect();
            WiFi.disconnect();
            uint8_t timeout = 0;
            //max timeout of 2seconds before just dropping out and restarting
            while (WiFi.status() != WL_DISCONNECTED && timeout < 200) {
                timeout++;
            }
        });
        ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) { /* ota progress code */ });
        ArduinoOTA.onError([](ota_error_t error) { /* ota error code */ });

        //initially attempt to connect to wifi when we begin (but only block for 2 seconds before timing out)
        uint8_t timeout = 0; //counter for begin connection attempts
        while (((!client.connected() && _mqttSet) || WiFi.status() != WL_CONNECTED)
               && timeout < 200) { //max 2 sec before timeout
            reconnect();
            timeout++;
        }

        //attempt to start ota if needed
        OTA_begin();

        // Initialize the telnet server
        telnetServer.begin();
        telnetServer.setNoDelay(true);

        // init command buffer for console commands
        memset(_command, 0, sizeof(_command));

        consoleShowHelp(); // show this at bootup

        //mark the system as started and return
        _hasBegun = true;

        return true;
    }

    //if no ssid was set even then dont try to begin and return false
    return false;
}

//end the instance of ESPHelper (shutdown wifi, ota, mqtt)
void ESPHelper::end() {
    // Stop telnet Client & Server
    if (telnetClient && telnetClient.connected()) {
        telnetClient.stop();
    }

    // Stop server
    telnetServer.stop();

    OTA_disable();
    WiFi.softAPdisconnect();
    WiFi.disconnect();

    uint8_t timeout = 0;
    while (WiFi.status() != WL_DISCONNECTED && timeout < 200) {
        timeout++;
    }

#ifdef USE_SERIAL
    Serial.flush();
#endif

#ifdef USE_SERIAL1
    Serial1.flush();
#endif
}

//main loop - should be called as often as possible - handles wifi/mqtt connection and mqtt handler
//true on: network/server connected
//false on: network or server disconnected
int ESPHelper::loop() {
    if (_ssidSet) {
        //check for good connections and attempt a reconnect if needed
        if (((_mqttSet && !client.connected()) || setConnectionStatus() < WIFI_ONLY) && _connectionStatus != BROADCAST) {
            reconnect();
        }

        //run the wifi loop as long as the connection status is at a minimum of BROADCAST
        if (_connectionStatus >= BROADCAST) {
            //run the MQTT loop if we have a full connection
            if (_connectionStatus == FULL_CONNECTION) {
                client.loop();
            }

            //check for whether we want to use OTA and whether the system is running
            if (_useOTA && _OTArunning) {
                ArduinoOTA.handle();
            }

            //if we want to use OTA but its not running yet, start it up.
            else if (_useOTA && !_OTArunning) {
                OTA_begin();
                ArduinoOTA.handle();
            }

            // do the telnet stuff
            consoleHandle();

            return _connectionStatus;
        }
    }

    //return -1 for no connection because of bad network info
    return -1;
}

//subscribe to a specific topic (does not add to topic list)
//true on: subscription success
//false on: subscription failed (either from PubSub lib or network is disconnected)
bool ESPHelper::subscribe(const char * topic, uint8_t qos) {
    if (_connectionStatus == FULL_CONNECTION) {
        //set the return value to the output of subscribe
        bool returnVal = client.subscribe(topic, qos);

        //loop mqtt client
        client.loop();
        return returnVal;
    }

    //if not fully connected return false
    else {
        return false;
    }
}

//add a topic to the list of subscriptions and attempt to subscribe to the topic on the spot
//true on: subscription added to list (does not guarantee that the topic was subscribed to, only that it was added to the list)
//false on: subscription not added to list
bool ESPHelper::addSubscription(const char * topic) {
    //default return value is false
    bool subscribed = false;

    //loop through finding the next available slot for a subscription and add it
    for (uint8_t i = 0; i < MAX_SUBSCRIPTIONS; i++) {
        if (_subscriptions[i].isUsed == false) {
            _subscriptions[i].topic  = topic;
            _subscriptions[i].isUsed = true;
            subscribed               = true;
            break;
        }
    }

    //if added to the list, subscribe to the topic
    if (subscribed) {
        subscribe(topic, _qos);
    }

    return subscribed;
}

//loops through list of subscriptions and attempts to subscribe to all topics
void ESPHelper::resubscribe() {
    for (uint8_t i = 0; i < MAX_SUBSCRIPTIONS; i++) {
        if (_subscriptions[i].isUsed) {
            subscribe(_subscriptions[i].topic, _qos);
            yield();
        }
    }
}

//manually unsubscribes from a topic (This is basically just a wrapper for the pubsubclient function)
bool ESPHelper::unsubscribe(const char * topic) {
    return client.unsubscribe(topic);
}

//publish to a specified topic
void ESPHelper::publish(const char * topic, const char * payload) {
    if (_mqttSet) {
        publish(topic, payload, false);
    }
}

//publish to a specified topic with a given retain level
void ESPHelper::publish(const char * topic, const char * payload, bool retain) {
    client.publish(topic, payload, retain);
}

//set the callback function for MQTT
void ESPHelper::setMQTTCallback(MQTT_CALLBACK_SIGNATURE) {
    _mqttCallback = callback;

    //only set the callback if using mqtt AND the system has already been started. Otherwise just save it for later
    if (_hasBegun && _mqttSet) {
        client.setCallback(_mqttCallback);
    }
    _mqttCallbackSet = true;
}

//sets a custom function to run when connection to wifi is established
void ESPHelper::setWifiCallback(void (*callback)()) {
    _wifiCallback    = callback;
    _wifiCallbackSet = true;
}

//attempts to connect to wifi & mqtt server if not connected
void ESPHelper::reconnect() {
    static uint8_t tryCount = 0;

    if (_connectionStatus != BROADCAST && setConnectionStatus() != FULL_CONNECTION) {
        logger(LOG_CONSOLE, "Attempting WiFi Connection...");
        //attempt to connect to the wifi if connection is lost
        if (WiFi.status() != WL_CONNECTED) {
            _connectionStatus = NO_CONNECTION;

            //increment try count each time it cannot connect (this is used to determine when to hop to a new network)
            tryCount++;
            if (tryCount == 20) {
                //change networks (if possible) when we have tried to connect 20 times and failed
                changeNetwork();
                tryCount = 0;
                return;
            }
        }

        // make sure we are connected to WIFI before attempting to reconnect to MQTT
        //----note---- maybe want to reset tryCount whenever we succeed at getting wifi connection?
        if (WiFi.status() == WL_CONNECTED) {
            //if the wifi previously wasnt connected but now is, run the callback
            if (_connectionStatus < WIFI_ONLY && _wifiCallbackSet) {
                _wifiCallback();
            }

            logger(LOG_CONSOLE, "---WiFi Connected!---");
            _connectionStatus = WIFI_ONLY;

            //attempt to connect to mqtt when we finally get connected to WiFi
            if (_mqttSet) {
                static uint8_t timeout = 0; //allow a max of 5 mqtt connection attempts before timing out
                if (!client.connected() && timeout < 5) {
                    logger(LOG_CONSOLE, "Attempting MQTT connection...");

                    uint8_t connected = 0;

                    //connect to mqtt with user/pass
                    if (_mqttUserSet) {
                        connected = client.connect(_clientName, _currentNet.mqttUser, _currentNet.mqttPass);
                    }

                    //connect to mqtt without credentials
                    else {
                        connected = client.connect(_clientName);
                    }

                    //if connected, subscribe to the topic(s) we want to be notified about
                    if (connected) {
                        logger(LOG_CONSOLE, " -- Connected");

                        //if using https, verify the fingerprint of the server before setting full connection (return on fail)
                        // removing this as not supported with ESP32, see https://github.com/espressif/arduino-esp32/issues/278
                        /*
                            if (wifiClientSecure.verify(_fingerprint,
                                                        _currentNet.mqttHost)) {
                                logger(LOG_CONSOLE,
                                       "Certificate Matches - SUCCESS\n");
                            } else {
                                logger(LOG_CONSOLE,
                                       "Certificate Doesn't Match - FAIL\n");
                                return;
                            }
                        }
                        */

                        _connectionStatus = FULL_CONNECTION;
                        resubscribe();
                        timeout = 0;
                    } else {
                        logger(LOG_CONSOLE, " -- Failed\n");
                    }
                    timeout++;
                }

                //if we still cant connect to mqtt after 10 attempts increment the try count
                if (timeout >= 5 && !client.connected()) {
                    timeout = 0;
                    tryCount++;
                    if (tryCount == 20) {
                        changeNetwork();
                        tryCount = 0;
                        return;
                    }
                }
            }
        }

        //reset the reconnect metro
        //reconnectMetro.reset();
    }
}

uint8_t ESPHelper::setConnectionStatus() {
    //assume no connection
    uint8_t returnVal = NO_CONNECTION;

    //make sure were not in broadcast mode
    if (_connectionStatus != BROADCAST) {
        //if connected to wifi set the mode to wifi only and run the callback if needed
        if (WiFi.status() == WL_CONNECTED) {
            if (_connectionStatus < WIFI_ONLY
                && _wifiCallbackSet) { //if the wifi previously wasn't connected but now is, run the callback
                _wifiCallback();
            }
            returnVal = WIFI_ONLY;

            //if mqtt is connected as well then set the status to full connection
            if (client.connected()) {
                returnVal = FULL_CONNECTION;
            }
        }
    }

    else {
        returnVal = BROADCAST;
    }

    //set the connection status and return
    _connectionStatus = returnVal;
    return returnVal;
}

//changes the current network settings to the next listed network if network hopping is allowed
void ESPHelper::changeNetwork() {
    //only attempt to change networks if hopping is allowed
    if (_hoppingAllowed) {
        //change the index/reset to 0 if we've hit the last network setting
        _currentIndex++;
        if (_currentIndex >= _netCount) {
            _currentIndex = 0;
        }

        //set the current netlist to the new network
        _currentNet = *_netList[_currentIndex];

        //verify various bits of network info

        //network password
        if (_currentNet.pass[0] == '\0') {
            _passSet = false;
        } else {
            _passSet = true;
        }

        //ssid
        if (_currentNet.ssid[0] == '\0') {
            _ssidSet = false;
        } else {
            _ssidSet = true;
        }

        //mqtt host
        if (_currentNet.mqttHost[0] == '\0') {
            _mqttSet = false;
        } else {
            _mqttSet = true;
        }

        //mqtt username
        if (_currentNet.mqttUser[0] == '\0') {
            _mqttUserSet = false;
        } else {
            _mqttUserSet = true;
        }

        //mqtt password
        if (_currentNet.mqttPass[0] == '\0') {
            _mqttPassSet = false;
        } else {
            _mqttPassSet = true;
        }

        printf("Trying next network: %s\n", _currentNet.ssid);

        //update the network connection
        updateNetwork();
    }
}

void ESPHelper::updateNetwork() {
    logger(LOG_CONSOLE, "\tDisconnecting from WiFi");
    WiFi.disconnect();
    logger(LOG_CONSOLE, "\tAttempting to begin on new network...");

    //set the wifi mode
    WiFi.mode(WIFI_STA);

    //connect to the network
    if (_passSet && _ssidSet) {
        WiFi.begin(_currentNet.ssid, _currentNet.pass);
    } else if (_ssidSet) {
        WiFi.begin(_currentNet.ssid);
    } else {
        WiFi.begin("NO_SSID_SET");
    }

    logger(LOG_CONSOLE, "\tSetting new MQTT server");
    //setup the mqtt broker info
    if (_mqttSet) {
        client.setServer(_currentNet.mqttHost, _currentNet.mqttPort);
    } else {
        client.setServer("192.168.1.3", 1883);
    }

    logger(LOG_CONSOLE, "\tDone - Ready for next reconnect attempt");
}

//enable use of OTA updates
void ESPHelper::OTA_enable() {
    _useOTA = true;
    ArduinoOTA.setHostname(_hostname);
    OTA_begin();
}

//begin the OTA subsystem but with a check for connectivity and enabled use of OTA
void ESPHelper::OTA_begin() {
    if (_connectionStatus >= BROADCAST && _useOTA) {
        ArduinoOTA.begin();
        _OTArunning = true;
    }
}

//disable use of OTA updates
void ESPHelper::OTA_disable() {
    _useOTA     = false;
    _OTArunning = false;
}

// Is CR or LF ?
bool ESPHelper::isCRLF(char character) {
    return (character == '\r' || character == '\n');
}

// handler for Telnet
void ESPHelper::consoleHandle() {
    // look for Client
    if (telnetServer.hasClient()) {
        if (telnetClient && telnetClient.connected()) {
            // Verify if the IP is same than actual connection
            WiFiClient newClient;
            newClient = telnetServer.available();
            String ip = newClient.remoteIP().toString();

            if (ip == telnetClient.remoteIP().toString()) {
                // Reconnect
                telnetClient.stop();
                telnetClient = newClient;
            } else {
                // Desconnect (not allow more than one connection)
                newClient.stop();
                return;
            }
        } else {
            // New TCP client
            telnetClient = telnetServer.available();
        }

        if (!telnetClient) { // No client yet ???
            return;
        }

        // Set client
        telnetClient.setNoDelay(true); // faster
        telnetClient.flush();          // clear input buffer, to prevent strange characters

        _lastTimeCommand = millis(); // To mark time for inactivity

        // Show the initial message
        consoleShowHelp();

        // Empty buffer
        while (telnetClient.available()) {
            telnetClient.read();
        }
    }

    // Is client connected ? (to reduce overhead in active)
    _telnetConnected = (telnetClient && telnetClient.connected());

    // Get command over telnet
    if (_telnetConnected) {
        char last = ' '; // To avoid processing double "\r\n"

        while (telnetClient.available()) { // get data from Client

            // Get character
            char character = telnetClient.read();

            // Newline (CR or LF) - once one time if (\r\n)
            if (isCRLF(character) == true) {
                if (isCRLF(last) == false) {
                    // Process the command
                    if (strlen(_command) > 0) {
                        consoleProcessCommand();
                    }
                }
                // reset for next command
                memset(_command, 0, sizeof(_command));
            } else if (isPrintable(character)) {
                // Concat char to end of buffer
                uint16_t len      = strlen(_command);
                _command[len]     = character;
                _command[len + 1] = '\0';
            }

            // Last char
            last = character;
        }

        // Inactivity - close connection if not received commands from user in telnet to reduce overheads
        if ((millis() - _lastTimeCommand) > MAX_TIME_INACTIVE) {
            telnetClient.println("* Closing session due to inactivity");
            telnetClient.flush();
            telnetClient.stop();
            _telnetConnected = false;
        }
    }
}

// Set help for commands over telnet
void ESPHelper::consoleSetHelpProjectsCmds(String help) {
    _helpProjectCmds = help;
}

// Set callback of sketch function to process project messages
void ESPHelper::consoleSetCallBackProjectCmds(void (*callback)()) {
    _consoleCallbackProjectCmds = callback;
}

// Set bootime received as a string from HA
void ESPHelper::setBoottime(const char * boottime) {
    strcpy(_boottime, boottime);
}

// overrides the write call to print to the telnet connection
size_t ESPHelper::write(uint8_t character) {
    if (!_verboseMessages)
        return 0;

    //static uint32_t elapsed = 0;

    // If start of a new line, initiate a new string buffer with time counter as a prefix
    if (_newLine) {
        unsigned long upt = millis();
        sprintf(bufferPrint,
                "(%s%02d:%02d:%02d%s) ",
                COLOR_CYAN,
                (uint8_t)((upt / (1000 * 60 * 60)) % 24),
                (uint8_t)((upt / (1000 * 60)) % 60),
                (uint8_t)((upt / 1000) % 60),
                COLOR_RESET);
        _newLine = false;
    }

    // Print ?
    bool doPrint = false;

    // New line ?
    if (character == '\n') {
        _newLine = true;
        doPrint  = true;
    } else if (strlen(bufferPrint) == BUFFER_PRINT - 1) { // Limit of buffer
        doPrint = true;
    }

    // Add character to telnet buffer
    uint16_t len     = strlen(bufferPrint);
    bufferPrint[len] = character;

    if (_newLine) {
        // add additional \r for windows
        bufferPrint[++len] = '\r';
    }

    // terminate string
    bufferPrint[++len] = '\0';

    // Send the characters buffered by print.h
    if (doPrint) {
        if (_telnetConnected) {
            telnetClient.print(bufferPrint);
        }

// echo to Serial if enabled
#ifdef USE_SERIAL
        Serial.print(bufferPrint);
#endif

#ifdef USE_SERIAL1
        Serial1.print(bufferPrint);
#endif

        // Empty the buffer
        bufferPrint[0] = '\0';
    }

    return len + 1;
}

// Show help of commands
void ESPHelper::consoleShowHelp() {
    // Show the initial message
    String help = "";

    help.concat("*\n\r* Remote Debug for ESP8266/ESP32\n\r");
    help.concat("* Device hostname: ");
    help.concat(WiFi.hostname());
    help.concat(", IP: ");
    help.concat(WiFi.localIP().toString());
    help.concat(", MAC address: ");
    help.concat(WiFi.macAddress());
    help.concat("\n\r* Connected to WiFi AP: ");
    help.concat(WiFi.SSID());

    help.concat("\n\r* Boot time: ");
    help.concat(_boottime);
    help.concat("\n\r* Free Heap RAM: ");
    help.concat(ESP.getFreeHeap());
    help.concat(" bytes\n\r");
    help.concat("*\n\r* Commands:\n\r*  ?=help, *=quit, $=memory, !=reboot, "
                "&=toggle verbose messages");

    if (_helpProjectCmds != "" && (_consoleCallbackProjectCmds)) {
        help.concat("\n\r");
        help.concat(_helpProjectCmds);
    }
    help.concat("\n\r");
    telnetClient.print(help);

#ifdef USE_SERIAL
    Serial.print(help);
#endif

#ifdef USE_SERIAL1
    Serial1.print(help);
#endif
}

// reset / restart
void ESPHelper::resetESP() {
    telnetClient.println("* Reboot ESP...");
    telnetClient.flush();
    telnetClient.stop();
    // end();

    // Reset
    ESP.restart();
    // ESP.reset(); // for ESP8266 only
}

// Get last command received
char * ESPHelper::consoleGetLastCommand() {
    return _command;
}

// Process user command over telnet
void ESPHelper::consoleProcessCommand() {
    // Set time of last command received
    _lastTimeCommand = millis();
    uint8_t cmd      = _command[0];

    if (!_verboseMessages) {
        telnetClient.println("Warning, verbose messaging is off. Use v to toggle.");
    }

    // Process the command
    if (cmd == '?') {
        consoleShowHelp();   // Show help
    } else if (cmd == '*') { // quit
        telnetClient.println("* Closing telnet connection...");
        telnetClient.stop();
    } else if (cmd == '$') {
        telnetClient.print("* Free Heap RAM (bytes): ");
        telnetClient.println(ESP.getFreeHeap());
    } else if (cmd == '!') {
        resetESP();
    } else if (cmd == '&') {
        _verboseMessages = !_verboseMessages; // toggle
        telnetClient.printf("Verbose messaging is %s\n\r", _verboseMessages ? "on" : "off");
    } else {
        // custom Project commands
        if (_consoleCallbackProjectCmds) {
            _consoleCallbackProjectCmds();
        }
    }
}

// Logger
// LOG_CONSOLE sends to the Telnet session
// LOG_HA sends to Telnet session plus a MQTT for Home Assistant
// LOG_NONE turns off all logging
void ESPHelper::logger(log_level_t level, const char * message) {
    // do we log to the telnet window?
    if ((level == LOG_CONSOLE) && (telnetClient && telnetClient.connected())) {
        telnetClient.println(message);
        telnetClient.flush();
    } else if (level == LOG_HA) {
        char s[100];
        sprintf(s, "%s: %s\n", _hostname, message); // add new line, for the debug telnet printer
        publish(MQTT_NOTIFICATION, s, false);
    }

// print to Serial if set in platform.io (requires recompile)
#ifdef USE_SERIAL
    Serial.println(message);
#endif

#ifdef USE_SERIAL1
    Serial.println(message);
#endif
}

// send specific command to HA via MQTT
// format is: home/<hostname>/command/<cmd>
void ESPHelper::sendHACommand(const char * cmd) {
    //logger(LOG_CONSOLE, "Sending command to HA...");

    char s[100];
    sprintf(s, "%s%s/%s", MQTT_BASE, _hostname, MQTT_TOPIC_COMMAND);

    publish(s, cmd, false);
}

// send specific start command to HA via MQTT, which returns the boottime
// format is: home/<hostname>/start
void ESPHelper::sendStart() {
    //logger(LOG_CONSOLE, "Sending Start command to HA...");

    char s[100];
    sprintf(s, "%s%s/%s", MQTT_BASE, _hostname, MQTT_TOPIC_START);

    // send initial payload of "start" to kick things off
    publish(s, MQTT_TOPIC_START, false);
}
