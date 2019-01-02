/*
 * MyEsp.h
 *
 * Paul Derbyshire - December 2018
 */

#pragma once

#ifndef MyEMS_h
#define MyEMS_h

#include <ArduinoJson.h>
#include <ArduinoOTA.h>
#include <AsyncMqttClient.h> // https://github.com/marvinroger/async-mqtt-client
#include <DNSServer.h>
#include <ESPAsyncTCP.h> // https://github.com/me-no-dev/ESPAsyncTCP
#include <JustWifi.h>    // https://github.com/xoseperez/justwifi
#include <TelnetSpy.h>   // modified from https://github.com/yasheena/telnetspy

#if defined(ARDUINO_ARCH_ESP32)
#include <ESPmDNS.h>
#else
#include <ESP8266mDNS.h>
#endif

// WIFI
#define WIFI_CONNECT_TIMEOUT 10000    // Connecting timeout for WIFI in ms
#define WIFI_RECONNECT_INTERVAL 60000 // If could not connect to WIFI, retry after this time in ms

// OTA
#define OTA_PORT 8266 // OTA port

// MQTT
#define MQTT_BASE "home/"
#define MQTT_NOTIFICATION MQTT_BASE "notification"
#define MQTT_TOPIC_COMMAND "command"
#define MQTT_TOPIC_START "start"
#define MQTT_TOPIC_START_PAYLOAD "start"
#define MQTT_HA MQTT_BASE "ha"
#define MQTT_PORT 1883 // MQTT port
#define MQTT_QOS 1
#define MQTT_RECONNECT_DELAY_MIN 5000   // Try to reconnect in 5 seconds upon disconnection
#define MQTT_RECONNECT_DELAY_STEP 5000  // Increase the reconnect delay in 5 seconds after each failed attempt
#define MQTT_RECONNECT_DELAY_MAX 120000 // Set reconnect time to 2 minutes at most
// Internal MQTT events
#define MQTT_CONNECT_EVENT 0
#define MQTT_DISCONNECT_EVENT 1
#define MQTT_MESSAGE_EVENT 2

// Telnet
#define TELNET_MAX_COMMAND_LENGTH 80 // length of a command
#define COLOR_RESET "\x1B[0m"
#define COLOR_BLACK "\x1B[0;30m"
#define COLOR_RED "\x1B[0;31m"
#define COLOR_GREEN "\x1B[0;32m"
#define COLOR_YELLOW "\x1B[0;33m"
#define COLOR_BLUE "\x1B[0;34m"
#define COLOR_MAGENTA "\x1B[0;35m"
#define COLOR_CYAN "\x1B[0;36m"
#define COLOR_WHITE "\x1B[0;37m"

typedef struct {
    char key[10];
    char description[400];
} command_t;

typedef std::function<void(unsigned int, const char *, const char *)> mqtt_callback_f;

// calculates size of an 2d array at compile time
template <typename T, size_t N>
constexpr size_t ArraySize(T (&)[N]) {
    return N;
}

// class definition
class MyESP {
  public:
    MyESP();
    ~MyESP();

    // wifi
    void setWIFICallback(void (*callback)());
    void setMQTTCallback(mqtt_callback_f callback);

    // ha
    void sendHACommand(const char * cmd);
    void sendHANotification(const char * message);

    // mqtt
    void mqttSubscribe(const char * topic);
    void mqttUnsubscribe(const char * topic);
    void mqttPublish(const char * topic, const char * payload);

    // debug & telnet
    void   myDebug(const char * format, ...);
    void   myDebug_P(PGM_P format_P, ...);
    void   consoleSetCallBackProjectCmds(command_t * cmds, uint8_t count, void (*callback)());
    char * consoleGetLastCommand();
    void   consoleProcessCommand();

    void end();
    void loop();
    void setup(char * app_hostname,
               char * app_name,
               char * app_version,
               char * wifi_ssid,
               char * wifi_password,
               char * mqtt_host,
               char * mqtt_username,
               char * mqtt_password);

    char * getBoottime();
    void   setBoottime(char * boottime);
    void   resetESP();

  private:
    // mqtt
    AsyncMqttClient mqttClient;
    unsigned long   _mqtt_reconnect_delay;
    void            _mqttOnMessage(char * topic, char * payload, size_t len);
    void            _mqttConnect();
    void            _mqtt_setup();
    mqtt_callback_f _mqtt_callback;
    void            _mqttOnConnect();
    void            _sendStart();
    char *          _mqtt_host;
    char *          _mqtt_username;
    char *          _mqtt_password;
    char *          _boottime;

    // wifi
    DNSServer dnsServer; // For Access Point (AP) support
    void      _wifiCallback(justwifi_messages_t code, char * parameter);
    void      _wifi_setup();
    void (*_extern_WIFICallback)();
    bool   _extern_WIFICallbackSet;
    char * _wifi_ssid;
    char * _wifi_password;

    // mdns
    void _mdns_setup();

    // ota
    void _ota_setup();

    // telnet & debug
    TelnetSpy   SerialAndTelnet;
    void        _telnetConnected();
    void        _telnetDisconnected();
    void        _telnetHandle();
    void        _telnet_setup();
    char *      _command;               // the input command from either Serial or Telnet
    command_t * _helpProjectCmds;       // Help of commands setted by project
    uint8_t     _helpProjectCmds_count; // # available commands
    void        _consoleShowHelp();
    void (*_consoleCallbackProjectCmds)(); // Callable for projects commands
    void _consoleProcessCommand();
    bool _isCRLF(char character);
    bool _suspendMessages;

    // general
    char * _app_hostname;
    char * _app_name;
    char * _app_version;
};

extern MyESP myESP;

#endif
