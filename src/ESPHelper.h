/*
    ESPHelper.h
    Copyright (c) 2017 ItKindaWorks Inc All right reserved.
    github.com/ItKindaWorks

    This file is part of ESPHelper

    ESPHelper is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    ESPHelper is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with ESPHelper.  If not, see <http://www.gnu.org/licenses/>.
*/
#pragma once

#include <ArduinoOTA.h>
#include <ESP8266WiFi.h> //https://github.com/esp8266/Arduino
#include <ESP8266mDNS.h>
#include <Print.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include <WiFiUdp.h>

// MQTT stuff
#define DEFAULT_QOS 1 //at least once - devices are guarantee to get a message.
#define MQTT_BASE "home/"
#define MQTT_NOTIFICATION MQTT_BASE "notification"
#define MQTT_TOPIC_COMMAND "command"
#define MQTT_TOPIC_START "start"
#define MQTT_HA MQTT_BASE "ha"

#define MAX_SUBSCRIPTIONS 25     // max # of subscriptions
#define MAX_TIME_INACTIVE 600000 // Max time for inactivity (ms) - 10 mins
#define TELNET_PORT 23           // telnet port
#define BUFFER_PRINT 500         // length of telnet buffer (default was 150)
#define COMMAND_LENGTH 20        // length of a command

// ANSI Colors
#define COLOR_RESET "\x1B[0m"
#define COLOR_BLACK "\x1B[0;30m"
#define COLOR_RED "\x1B[0;31m"
#define COLOR_GREEN "\x1B[0;32m"
#define COLOR_YELLOW "\x1B[0;33m"
#define COLOR_BLUE "\x1B[0;34m"
#define COLOR_MAGENTA "\x1B[0;35m"
#define COLOR_CYAN "\x1B[0;36m"
#define COLOR_WHITE "\x1B[0;37m"

// Logger
typedef enum { LOG_NONE, LOG_CONSOLE, LOG_HA } log_level_t;

enum connStatus { NO_CONNECTION, BROADCAST, WIFI_ONLY, FULL_CONNECTION };

struct netInfo {
    const char * mqttHost;
    const char * mqttUser;
    const char * mqttPass;
    uint16_t     mqttPort;
    const char * ssid;
    const char * pass;
};
typedef struct netInfo netInfo;

struct subscription {
    bool         isUsed = false;
    const char * topic;
};
typedef struct subscription subscription;

// class ESPHelper {
class ESPHelper : public Print {
  public:
    void   consoleSetHelpProjectsCmds(String help);
    void   consoleSetCallBackProjectCmds(void (*callback)());
    char * consoleGetLastCommand();
    void   resetESP();
    void   logger(log_level_t level, const char * message);

    virtual size_t write(uint8_t);

    ESPHelper(netInfo * startingNet);

    bool begin(const char * hostname);
    void end();

    void useSecureClient(const char * fingerprint);

    int loop();

    bool subscribe(const char * topic, uint8_t qos);
    bool addSubscription(const char * topic);
    bool removeSubscription(const char * topic);
    bool unsubscribe(const char * topic);
    bool addHASubscription(const char * topic);

    void publish(const char * topic, const char * payload);
    void publish(const char * topic, const char * payload, bool retain);

    bool setCallback(MQTT_CALLBACK_SIGNATURE);
    void setMQTTCallback(MQTT_CALLBACK_SIGNATURE);

    void setWifiCallback(void (*callback)());

    void sendHACommand(const char * s);
    void sendStart();

    void reconnect();

    void updateNetwork();

    const char * getSSID();
    void         setSSID(const char * ssid);

    const char * getPASS();
    void         setPASS(const char * pass);

    const char * getMQTTIP();
    void         setMQTTIP(const char * mqttIP);
    void         setMQTTIP(const char * mqttIP, const char * mqttUser, const char * mqttPass);

    uint8_t getMQTTQOS();
    void    setMQTTQOS(uint8_t qos);

    String    getIP();
    IPAddress getIPAddress();

    uint8_t getStatus();

    void      setNetInfo(netInfo newNetwork);
    void      setNetInfo(netInfo * newNetwork);
    netInfo * getNetInfo();

    void setHopping(bool canHop);

    void listSubscriptions();

    void OTA_enable();
    void OTA_disable();
    void OTA_begin();

    void setBoottime(const char * boottime);

    void consoleHandle();

  private:
    netInfo          _currentNet;
    PubSubClient     client;
    WiFiClient       wifiClient;
    WiFiClientSecure wifiClientSecure;
    const char *     _fingerprint;
    bool             _useSecureClient = false;
    char             _clientName[40];
    void (*_wifiCallback)();
    bool _wifiCallbackSet = false;

    std::function<void(char *, uint8_t *, uint8_t)> _mqttCallback;

    bool         _mqttCallbackSet  = false;
    uint8_t      _connectionStatus = NO_CONNECTION;
    uint8_t      _netCount         = 0;
    uint8_t      _currentIndex     = 0;
    bool         _ssidSet          = false;
    bool         _passSet          = false;
    bool         _mqttSet          = false;
    bool         _mqttUserSet      = false;
    bool         _mqttPassSet      = false;
    bool         _useOTA           = false;
    bool         _OTArunning       = false;
    bool         _hoppingAllowed   = false;
    bool         _hasBegun         = false;
    netInfo **   _netList;
    bool         _verboseMessages = true;
    subscription _subscriptions[MAX_SUBSCRIPTIONS];
    char         _hostname[64];
    uint8_t      _qos  = DEFAULT_QOS;
    IPAddress    _apIP = IPAddress(192, 168, 1, 254);
    void         changeNetwork();
    String       macToStr(const uint8_t * mac);
    bool         checkParams();
    void         resubscribe();
    uint8_t      setConnectionStatus();

    char _boottime[50];

    // console/telnet specific
    WiFiClient telnetClient;

    bool _telnetConnected = false; // Client is connected ?
    bool _newLine         = true;  // New line write ?

    char     _command[COMMAND_LENGTH];    // Command received, includes options seperated by a space
    uint32_t _lastTimeCommand = millis(); // Last time command received

    String _helpProjectCmds = ""; // Help of commands setted by project

    void (*_consoleCallbackProjectCmds)(); // Callable for projects commands
    void consoleShowHelp();
    void consoleProcessCommand();
    bool isCRLF(char character);

    char bufferPrint[BUFFER_PRINT];
};
