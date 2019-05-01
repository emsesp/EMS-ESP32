/*
 * MyESP.h
 *
 * Paul Derbyshire - December 2018
 */

#pragma once

#ifndef MyEMS_h
#define MyEMS_h

#define MYESP_VERSION "1.1.8"

#include <ArduinoJson.h>
#include <ArduinoOTA.h>
#include <AsyncMqttClient.h> // https://github.com/marvinroger/async-mqtt-client and for ESP32 see https://github.com/marvinroger/async-mqtt-client/issues/127
#include <DNSServer.h>
#include <FS.h>
#include <JustWifi.h>  // https://github.com/xoseperez/justwifi
#include <TelnetSpy.h> // modified from https://github.com/yasheena/telnetspy

#ifdef CRASH
#include <EEPROM_Rotate.h>
extern "C" {
void custom_crash_callback(struct rst_info *, uint32_t, uint32_t);
}
#endif

#if defined(ARDUINO_ARCH_ESP32)
//#include <ESPmDNS.h>
#include <SPIFFS.h>             // added for ESP32
#define ets_vsnprintf vsnprintf // added for ESP32
#define OTA_PORT 8266
#else
#include <ESPAsyncTCP.h>
#define OTA_PORT 3232
#endif

#define MYEMS_CONFIG_FILE "/config.json"

#define LOADAVG_INTERVAL 30000 // Interval between calculating load average (in ms)

// WIFI
#define WIFI_CONNECT_TIMEOUT 10000    // Connecting timeout for WIFI in ms
#define WIFI_RECONNECT_INTERVAL 60000 // If could not connect to WIFI, retry after this time in ms

// MQTT
#define MQTT_PORT 1883                  // MQTT port
#define MQTT_RECONNECT_DELAY_MIN 2000   // Try to reconnect in 3 seconds upon disconnection
#define MQTT_RECONNECT_DELAY_STEP 3000  // Increase the reconnect delay in 3 seconds after each failed attempt
#define MQTT_RECONNECT_DELAY_MAX 120000 // Set reconnect time to 2 minutes at most
#define MQTT_MAX_TOPIC_SIZE 50          // max length of MQTT message
#define MQTT_TOPIC_START "start"
#define MQTT_TOPIC_START_PAYLOAD "start"
#define MQTT_TOPIC_RESTART "restart"

// Internal MQTT events
#define MQTT_CONNECT_EVENT 0
#define MQTT_DISCONNECT_EVENT 1
#define MQTT_MESSAGE_EVENT 2

// Telnet
#define TELNET_SERIAL_BAUD 115200
#define TELNET_MAX_COMMAND_LENGTH 80 // length of a command
#define TELNET_EVENT_CONNECT 1
#define TELNET_EVENT_DISCONNECT 0

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
#define COLOR_BOLD_ON "\x1B[1m"
#define COLOR_BOLD_OFF "\x1B[22m"
#define COLOR_BRIGHT_BLACK "\x1B[0;90m"
#define COLOR_BRIGHT_RED "\x1B[0;91m"
#define COLOR_BRIGHT_GREEN "\x1B[0;92m"
#define COLOR_BRIGHT_YELLOW "\x1B[0;99m"
#define COLOR_BRIGHT_BLUE "\x1B[0;94m"
#define COLOR_BRIGHT_MAGENTA "\x1B[0;95m"
#define COLOR_BRIGHT_CYAN "\x1B[0;96m"
#define COLOR_BRIGHT_WHITE "\x1B[0;97m"

// SPIFFS
#define SPIFFS_MAXSIZE 800 // https://arduinojson.org/v6/assistant/

// CRASH
/**
 * Structure of the single crash data set
 *
 *  1. Crash time
 *  2. Restart reason
 *  3. Exception cause
 *  4. epc1
 *  5. epc2
 *  6. epc3
 *  7. excvaddr
 *  8. depc
 *  9. address of stack start
 * 10. address of stack end
 * 11. stack trace bytes
 *     ...
 */
#define SAVE_CRASH_EEPROM_OFFSET 0x0100 // initial address for crash data
#define SAVE_CRASH_CRASH_TIME 0x00      // 4 bytes
#define SAVE_CRASH_RESTART_REASON 0x04  // 1 byte
#define SAVE_CRASH_EXCEPTION_CAUSE 0x05 // 1 byte
#define SAVE_CRASH_EPC1 0x06            // 4 bytes
#define SAVE_CRASH_EPC2 0x0A            // 4 bytes
#define SAVE_CRASH_EPC3 0x0E            // 4 bytes
#define SAVE_CRASH_EXCVADDR 0x12        // 4 bytes
#define SAVE_CRASH_DEPC 0x16            // 4 bytes
#define SAVE_CRASH_STACK_START 0x1A     // 4 bytes
#define SAVE_CRASH_STACK_END 0x1E       // 4 bytes
#define SAVE_CRASH_STACK_TRACE 0x22     // variable

typedef struct {
    bool set; // is it a set command
    char key[50];
    char description[100];
} command_t;

typedef enum { MYESP_FSACTION_SET, MYESP_FSACTION_LIST, MYESP_FSACTION_SAVE, MYESP_FSACTION_LOAD } MYESP_FSACTION;

typedef std::function<void(unsigned int, const char *, const char *)>            mqtt_callback_f;
typedef std::function<void()>                                                    wifi_callback_f;
typedef std::function<void()>                                                    ota_callback_f;
typedef std::function<void(uint8_t, const char *)>                               telnetcommand_callback_f;
typedef std::function<void(uint8_t)>                                             telnet_callback_f;
typedef std::function<bool(MYESP_FSACTION, const JsonObject json)>               fs_callback_f;
typedef std::function<bool(MYESP_FSACTION, uint8_t, const char *, const char *)> fs_settings_callback_f;

// calculates size of an 2d array at compile time
template <typename T, size_t N>
constexpr size_t ArraySize(T (&)[N]) {
    return N;
}

#define UPTIME_OVERFLOW 4294967295 // Uptime overflow value

// class definition
class MyESP {
  public:
    MyESP();
    ~MyESP();

    // wifi
    void setWIFICallback(void (*callback)());
    void setWIFI(const char * wifi_ssid, const char * wifi_password, wifi_callback_f callback);
    bool isWifiConnected();
    bool isAPmode();

    // mqtt
    bool isMQTTConnected();
    void mqttSubscribe(const char * topic);
    void mqttUnsubscribe(const char * topic);
    void mqttPublish(const char * topic, const char * payload);
    void setMQTT(const char *    mqtt_host,
                 const char *    mqtt_username,
                 const char *    mqtt_password,
                 const char *    mqtt_base,
                 unsigned long   mqtt_keepalive,
                 unsigned char   mqtt_qos,
                 bool            mqtt_retain,
                 const char *    mqtt_will_topic,
                 const char *    mqtt_will_online_payload,
                 const char *    mqtt_will_offline_payload,
                 mqtt_callback_f callback);

    // OTA
    void setOTA(ota_callback_f OTACallback_pre, ota_callback_f OTACallback_post);

    // debug & telnet
    void myDebug(const char * format, ...);
    void myDebug_P(PGM_P format_P, ...);
    void setTelnet(command_t * cmds, uint8_t count, telnetcommand_callback_f callback_cmd, telnet_callback_f callback);
    bool getUseSerial();
    void setUseSerial(bool toggle);

    // FS
    void setSettings(fs_callback_f callback, fs_settings_callback_f fs_settings_callback);
    bool fs_saveConfig();

    // Crash
    void crashClear();
    void crashDump();
    void crashTest(uint8_t t);
    void crashInfo();

    // general
    void     end();
    void     loop();
    void     begin(const char * app_hostname, const char * app_name, const char * app_version);
    void     setBoottime(const char * boottime);
    void     resetESP();
    uint16_t getSystemLoadAverage();
    int      getWifiQuality();
    void     showSystemStats();

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
    char *          _mqttTopic(const char * topic);
    char *          _mqtt_host;
    char *          _mqtt_username;
    char *          _mqtt_password;
    char *          _mqtt_base;
    unsigned long   _mqtt_keepalive;
    unsigned char   _mqtt_qos;
    bool            _mqtt_retain;
    char *          _mqtt_will_topic;
    char *          _mqtt_will_online_payload;
    char *          _mqtt_will_offline_payload;
    char *          _mqtt_topic;
    unsigned long   _mqtt_last_connection;
    bool            _mqtt_connecting;

    // wifi
    DNSServer       dnsServer; // For Access Point (AP) support
    void            _wifiCallback(justwifi_messages_t code, char * parameter);
    void            _wifi_setup();
    wifi_callback_f _wifi_callback;
    char *          _wifi_ssid;
    char *          _wifi_password;
    bool            _wifi_connected;
    String          _getESPhostname();

    // ota
    ota_callback_f _ota_pre_callback;
    ota_callback_f _ota_post_callback;
    void           _ota_setup();
    void           _OTACallback();

    // crash
    void _eeprom_setup();

    // telnet & debug
    TelnetSpy                SerialAndTelnet;
    void                     _telnetConnected();
    void                     _telnetDisconnected();
    void                     _telnetHandle();
    void                     _telnetCommand(char * commandLine);
    char *                   _telnet_readWord(bool allow_all_chars);
    void                     _telnet_setup();
    char                     _command[TELNET_MAX_COMMAND_LENGTH]; // the input command from either Serial or Telnet
    command_t *              _helpProjectCmds;                    // Help of commands setted by project
    uint8_t                  _helpProjectCmds_count;              // # available commands
    void                     _consoleShowHelp();
    telnetcommand_callback_f _telnetcommand_callback; // Callable for projects commands
    telnet_callback_f        _telnet_callback;        // callback for connect/disconnect
    bool                     _changeSetting(uint8_t wc, const char * setting, const char * value);

    // fs
    void _fs_setup();
    bool _fs_loadConfig();
    void _fs_printConfig();
    void _fs_eraseConfig();

    // settings
    fs_callback_f          _fs_callback;
    fs_settings_callback_f _fs_settings_callback;
    void                   _printSetCommands();

    // general
    char *        _app_hostname;
    char *        _app_name;
    char *        _app_version;
    char *        _boottime;
    bool          _suspendOutput;
    bool          _use_serial;
    unsigned long _getUptime();
    String        _buildTime();

    // load average (0..100)
    void               _calculateLoad();
    unsigned short int _load_average;
};

extern MyESP myESP;

#endif
