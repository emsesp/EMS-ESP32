/*
 * MyESP.h - does all the basics like WiFI/MQTT/NTP/Debug logs etc
 *
 * Paul Derbyshire - first version December 2018
 */

#pragma once

#ifndef MyESP_h
#define MyESP_h

#define MYESP_VERSION "1.2.29"

#include <ArduinoJson.h>
#include <ArduinoOTA.h>
#include <AsyncMqttClient.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <JustWifi.h>
#include <deque> // for MQTT publish queue

// SysLog
#include <uuid/common.h>
#include <uuid/log.h>
#include <uuid/syslog.h>
static uuid::syslog::SyslogService syslog;
enum MYESP_SYSLOG_LEVEL : uint8_t { MYESP_SYSLOG_INFO, MYESP_SYSLOG_ERROR };

// local libraries
#include "Ntp.h"
#include "TelnetSpy.h" // modified from https://github.com/yasheena/telnetspy

#ifdef CRASH
#include <EEPROM_Rotate.h>
#endif

extern "C" {
void custom_crash_callback(struct rst_info *, uint32_t, uint32_t);
#include "user_interface.h"
extern struct rst_info resetInfo;
}

#if defined(ARDUINO_ARCH_ESP32)
#include <SPIFFS.h>             // added for ESP32
#define ets_vsnprintf vsnprintf // added for ESP32
#define OTA_PORT 3232
#else
#include <ESPAsyncTCP.h>
#define OTA_PORT 8266
#endif

// web files
// reference libs
#include "webh/glyphicons-halflings-regular.woff.gz.h"
#include "webh/required.css.gz.h"
#include "webh/required.js.gz.h"

// custom stuff
#include "webh/index.html.gz.h"
#include "webh/myesp.html.gz.h"
#include "webh/myesp.js.gz.h"

#define MYESP_CONFIG_FILE "/myesp.json"
#define MYESP_CUSTOMCONFIG_FILE "/customconfig.json"
#define MYESP_OLD_EVENTLOG_FILE "/eventlog.json" // depreciated
#define MYESP_OLD_CONFIG_FILE "/config.json"     // depreciated

#define MYESP_HTTP_USERNAME "admin" // HTTP username
#define MYESP_HTTP_PASSWORD "admin" // default password

#define MYESP_NTP_SERVER "pool.ntp.org" // default ntp server

#define MYESP_LOADAVG_INTERVAL 30000 // Interval between calculating load average (in ms) = 30 seconds

// WIFI
#define MYESP_WIFI_CONNECT_TIMEOUT 20000     // Connecting timeout for WIFI in ms (20 seconds)
#define MYESP_WIFI_RECONNECT_INTERVAL 600000 // If could not connect to WIFI, retry after this time in ms. 10 minutes

// set to value >0 if the ESP is overheating or there are timing issues. Recommend a value of 1.
#define MYESP_DELAY 1

// MQTT
#define MQTT_PORT 1883                  // MQTT port
#define MQTT_RECONNECT_DELAY_MIN 2000   // Try to reconnect in 3 seconds upon disconnection
#define MQTT_RECONNECT_DELAY_STEP 3000  // Increase the reconnect delay in 3 seconds after each failed attempt
#define MQTT_RECONNECT_DELAY_MAX 120000 // Set reconnect time to 2 minutes at most
#define MQTT_TOPIC_START "start"
#define MQTT_TOPIC_HEARTBEAT "heartbeat"
#define MQTT_TOPIC_START_PAYLOAD "start"
#define MQTT_TOPIC_RESTART "restart"
#define MQTT_WILL_ONLINE_PAYLOAD "online"   // for last will & testament payload
#define MQTT_WILL_OFFLINE_PAYLOAD "offline" // for last will & testament payload
#define MQTT_BASE_DEFAULT "home"            // default MQTT prefix to topics
#define MQTT_RETAIN false                   // default false
#define MQTT_KEEPALIVE 60                   // default keepalive 1 minute
#define MQTT_QOS 0                          // default qos 0
#define MQTT_WILL_TOPIC "status"            // for last will & testament topic name
#define MQTT_MAX_TOPIC_SIZE 50              // max length of MQTT topic
#define MQTT_QUEUE_MAX_SIZE 50              // Size of the MQTT queue
#define MQTT_PUBLISH_WAIT 750               // time in ms before sending MQTT messages
#define MQTT_PUBLISH_MAX_RETRY 4            // max retries for giving up on publishing
#define MYESP_JSON_MAXSIZE_LARGE 2000       // for large Dynamic json files - https://arduinojson.org/v6/assistant/
#define MYESP_JSON_MAXSIZE_MEDIUM 800       // for medium Dynamic json files - https://arduinojson.org/v6/assistant/
#define MYESP_JSON_MAXSIZE_SMALL 200        // for smaller Static json documents - https://arduinojson.org/v6/assistant/

// Internal MQTT events
#define MQTT_CONNECT_EVENT 0
#define MQTT_DISCONNECT_EVENT 1
#define MQTT_MESSAGE_EVENT 2

#define MYESP_MQTT_PAYLOAD_ON '1'  // for MQTT switch on
#define MYESP_MQTT_PAYLOAD_OFF '0' // for MQTT switch off

// Telnet
#define TELNET_SERIAL_BAUD 115200
#define TELNET_MAX_COMMAND_LENGTH 80 // length of a command
#define TELNET_MAX_BUFFER_LENGTH 700 // max length of telnet string
#define TELNET_EVENT_CONNECT 1
#define TELNET_EVENT_DISCONNECT 0
#define TELNET_EVENT_SHOWCMD 10
#define TELNET_EVENT_SHOWSET 20

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

// reset reason codes
PROGMEM const char         custom_reset_hardware[] = "Hardware button";
PROGMEM const char         custom_reset_terminal[] = "Restart from terminal";
PROGMEM const char         custom_reset_mqtt[]     = "Restart from MQTT";
PROGMEM const char         custom_reset_ota[]      = "Restart after successful OTA update";
PROGMEM const char         custom_reset_factory[]  = "Factory reset";
PROGMEM const char * const custom_reset_string[]   = {custom_reset_hardware, custom_reset_terminal, custom_reset_mqtt, custom_reset_ota, custom_reset_factory};
#define CUSTOM_RESET_HARDWARE 1 // Reset from hardware button
#define CUSTOM_RESET_TERMINAL 2 // Reset from terminal
#define CUSTOM_RESET_MQTT 3     // Reset via MQTT
#define CUSTOM_RESET_OTA 4      // Reset after successful OTA update
#define CUSTOM_RESET_FACTORY 5  // Factory reset
#define CUSTOM_RESET_MAX 5

// SPIFFS - max allocation is 1000 KB
// https://arduinojson.org/v6/assistant/
#define MYESP_SPIFFS_MAXSIZE_CONFIG 999 // max size for a config file

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

// Base address of USER RTC memory
// https://github.com/esp8266/esp8266-wiki/wiki/Memory-Map#memmory-mapped-io-registers
#define RTCMEM_ADDR_BASE (0x60001200)

// RTC memory is accessed using blocks of 4 bytes.
// Blocks 0..63 are reserved by the SDK, 64..192 are available to the user.
// Blocks 64..96 are reserved by the eboot 'struct eboot_command' (128 -> (128 / 4) -> 32):
// https://github.com/esp8266/Arduino/blob/master/bootloaders/eboot/eboot_command.h
#define RTCMEM_OFFSET 32u
#define RTCMEM_ADDR (RTCMEM_ADDR_BASE + (RTCMEM_OFFSET * 4u))
#define RTCMEM_BLOCKS 96u
#define RTCMEM_MAGIC 0x45535076

struct RtcmemData {
    uint32_t magic; // RTCMEM_MAGIC
    uint32_t sys;   // system details
};

static_assert(sizeof(RtcmemData) <= (RTCMEM_BLOCKS * 4u), "RTCMEM struct is too big");

#define MYESP_SYSTEM_CHECK_TIME 60000  // The system is considered stable after these many millis (1 min)
#define MYESP_SYSTEM_CHECK_MAX 10      // After this many crashes on boot
#define MYESP_HEARTBEAT_INTERVAL 60000 // in milliseconds, how often the MQTT heartbeat is sent (1 min)

typedef struct {
    bool set; // is it a set command?
    char key[50];
    char description[110];
} command_t;

typedef enum {
    MYESP_FSACTION_SET,
    MYESP_FSACTION_LIST,
    MYESP_FSACTION_SAVE,
    MYESP_FSACTION_LOAD,
    MYESP_FSACTION_ERR,
    MYESP_FSACTION_OK,
    MYESP_FSACTION_RESTART
} MYESP_FSACTION_t;

typedef enum {
    MYESP_BOOTSTATUS_POWERON     = 0,
    MYESP_BOOTSTATUS_BOOTED      = 1,
    MYESP_BOOTSTATUS_BOOTING     = 2,
    MYESP_BOOTSTATUS_RESETNEEDED = 3
} MYESP_BOOTSTATUS_t; // boot messages

typedef std::function<void(unsigned int, const char *, const char *)>                          mqtt_callback_f;
typedef std::function<void()>                                                                  wifi_callback_f;
typedef std::function<void()>                                                                  ota_callback_f;
typedef std::function<void(uint8_t, const char *)>                                             telnetcommand_callback_f;
typedef std::function<void(uint8_t)>                                                           telnet_callback_f;
typedef std::function<bool(MYESP_FSACTION_t, JsonObject json)>                                 fs_loadsave_callback_f;
typedef std::function<MYESP_FSACTION_t(MYESP_FSACTION_t, uint8_t, const char *, const char *)> fs_setlist_callback_f;
typedef std::function<void(JsonObject root)>                                                   web_callback_f;

// calculates size of an 2d array at compile time
template <typename T, size_t N>
constexpr size_t ArraySize(T (&)[N]) {
    return N;
}

#define MYESP_UPTIME_OVERFLOW 4294967295 // Uptime overflow value
#define MYESP_MAX_STR_LEN 16             // web min and max length of wifi ssid and password
#define MYESP_BOOTUP_FLASHDELAY 50       // flash duration for LED at bootup sequence
#define MYESP_BOOTUP_DELAY 2000          // time before we open the window to reset. This is to stop resetting values when uploading firmware via USB

// class definition
class MyESP {
  protected:
    // webserver
    AsyncWebServer * _webServer;
    AsyncWebSocket * _ws;

    // NTP
    NtpClient NTP;

  public:
    MyESP();
    ~MyESP();

    // wifi
    void setWIFICallback(void (*callback)());
    void setWIFI(wifi_callback_f callback);
    bool isWifiConnected();
    bool isAPmode();

    // mqtt
    bool isMQTTConnected();
    bool mqttSubscribe(const char * topic);
    void mqttUnsubscribe(const char * topic);
    void mqttPublish(const char * topic, const char * payload);
    void mqttPublish(const char * topic, const char * payload, bool retain);
    void mqttPublish(const char * topic, JsonDocument payload);
    void mqttPublish(const char * topic, JsonDocument payload, bool retain);
    void setMQTT(mqtt_callback_f callback);
    bool mqttUseNestedJson();

    // OTA
    void setOTA(ota_callback_f OTACallback_pre, ota_callback_f OTACallback_post);

    // debug & telnet
    void myDebug(const char * format, ...);
    void myDebug_P(PGM_P format_P, ...);
    void setTelnet(telnetcommand_callback_f callback_cmd, telnet_callback_f callback);
    bool getUseSerial();
    void setUseSerial(bool toggle);

    // syslog
    void writeLogEvent(const uint8_t type, const char * msg);

    // FS
    void setSettings(fs_loadsave_callback_f loadsave, fs_setlist_callback_f setlist, bool useSerial = true);
    void saveSettings();
    bool fs_saveConfig(JsonObject root);
    bool fs_saveCustomConfig(JsonObject root);
    bool fs_setSettingValue(char ** setting, const char * value, const char * value_default);
    bool fs_setSettingValue(uint16_t * setting, const char * value, uint16_t value_default);
    bool fs_setSettingValue(uint8_t * setting, const char * value, uint8_t value_default);
    bool fs_setSettingValue(int8_t * setting, const char * value, int8_t value_default);
    bool fs_setSettingValue(bool * setting, const char * value, bool value_default);

    // Web
    void setWeb(web_callback_f callback_web);

    // Crash
    void crashClear();
    void crashDump();
    void crashInfo();

    // general
    void          end();
    void          loop();
    void          begin(const char * app_hostname, const char * app_name, const char * app_version, const char * app_url, const char * app_url_api);
    void          resetESP();
    int           getWifiQuality();
    void          showSystemStats();
    bool          getHeartbeat();
    uint32_t      getSystemLoadAverage();
    uint32_t      getSystemResetReason();
    uint8_t       getSystemBootStatus();
    unsigned long getSystemTime();
    void          heartbeatPrint();
    void          heartbeatCheck(bool force = false);

  private:
    // mqtt
    void            _mqttOnMessage(char * topic, char * payload, size_t len);
    void            _mqttOnPublish(uint16_t packetId);
    void            _mqttConnect();
    void            _mqtt_setup();
    void            _mqttOnConnect();
    void            _sendStart();
    char *          _mqttTopic(const char * topic);
    bool            _mqttQueue(const char * topic, const char * payload, bool retain);
    bool            _mqttQueue(const char * topic, JsonDocument payload, bool retain);
    void            _printMQTTLog();
    void            _mqttPublishQueue();
    void            _mqttRemoveLastPublish();
    AsyncMqttClient mqttClient; // the MQTT class
    uint32_t        _mqtt_reconnect_delay;
    mqtt_callback_f _mqtt_callback_f;
    char *          _mqtt_ip;
    char *          _mqtt_user;
    char *          _mqtt_password;
    uint16_t        _mqtt_port;
    char *          _mqtt_base;
    bool            _mqtt_enabled;
    uint16_t        _mqtt_keepalive;
    uint8_t         _mqtt_qos;
    bool            _mqtt_retain;
    char *          _mqtt_will_topic;
    char *          _mqtt_will_online_payload;
    char *          _mqtt_will_offline_payload;
    uint32_t        _mqtt_last_connection;
    bool            _mqtt_connecting;
    bool            _mqtt_heartbeat;
    bool            _mqtt_nestedjson;
    uint16_t        _mqtt_publish_fails;

    // wifi
    void            _wifiCallback(justwifi_messages_t code, char * parameter);
    void            _wifi_setup();
    wifi_callback_f _wifi_callback_f;
    char *          _network_ssid;
    char *          _network_password;
    uint8_t         _network_wmode;
    char *          _network_staticip;
    char *          _network_gatewayip;
    char *          _network_nmask;
    char *          _network_dnsip;
    bool            _wifi_connected;
    String          _getESPhostname();

    // ota
    ota_callback_f _ota_pre_callback_f;
    ota_callback_f _ota_post_callback_f;
    void           _ota_setup();
    void           _OTACallback();

    // crash
    void _eeprom_setup();

    // telnet
    TelnetSpy                SerialAndTelnet;
    void                     _telnetConnected();
    void                     _telnetDisconnected();
    void                     _telnetHandle();
    void                     _telnetCommand(char * commandLine);
    char *                   _telnet_readWord(bool allow_all_chars);
    void                     _telnet_setup();
    char                     _command[TELNET_MAX_COMMAND_LENGTH]; // the input command from either Serial or Telnet
    void                     _consoleShowHelp();
    telnetcommand_callback_f _telnetcommand_callback_f; // Callable for projects commands
    telnet_callback_f        _telnet_callback_f;        // callback for connect/disconnect
    bool                     _changeSetting(uint8_t wc, const char * setting, const char * value);

    // syslog
    void _syslog_setup();

    // fs and settings
    void                   _fs_setup();
    bool                   _fs_loadConfig();
    bool                   _fs_loadCustomConfig();
    void                   _fs_eraseConfig();
    bool                   _fs_writeConfig();
    bool                   _fs_createCustomConfig();
    bool                   _fs_sendConfig();
    size_t                 _fs_validateConfigFile(const char * filename, size_t maxsize, JsonDocument & doc);
    size_t                 _fs_validateLogFile(const char * filename);
    fs_loadsave_callback_f _fs_loadsave_callback_f;
    fs_setlist_callback_f  _fs_setlist_callback_f;
    void                   _printSetCommands();

    // general
    char *        _general_hostname;
    char *        _app_name;
    char *        _app_version;
    char *        _app_url;
    char *        _app_updateurl;
    char *        _app_updateurl_dev;
    bool          _suspendOutput;
    bool          _general_serial;
    bool          _general_log_events;
    char *        _general_log_ip;
    char *        _buildTime;
    bool          _timerequest;
    bool          _formatreq;
    unsigned long _getUptime();
    char *        _getBuildTime();
    bool          _hasValue(const char * s);
    void          _printHeap(const char * s);
    void          _kick();

    // reset reason and rtcmem
    bool    _rtcmem_status;
    bool    _rtcmemStatus();
    bool    _getRtcmemStatus();
    void    _rtcmemInit();
    void    _rtcmemSetup();
    void    _deferredReset(unsigned long delay, uint8_t reason);
    uint8_t _getSystemStabilityCounter();
    void    _setSystemStabilityCounter(uint8_t counter);
    uint8_t _getSystemDropoutCounter();
    void    _setSystemDropoutCounter(uint8_t counter);
    void    _increaseSystemDropoutCounter();
    void    _setSystemResetReason(uint8_t reason);
    uint8_t _getCustomResetReason();
    void    _setCustomResetReason(uint8_t reason);
    uint8_t _getSystemResetReason();
    void    _setSystemBootStatus(uint8_t status);
    bool    _systemStable;
    void    _bootupSequence();
    bool    _getSystemCheck();
    void    _systemCheckLoop();
    void    _setSystemCheck(bool stable);

    // load average (0..100) and heap ram
    void     _calculateLoad();
    uint32_t _load_average;
    uint32_t _getInitialFreeHeap();
    uint32_t _getUsedHeap();

    // web
    web_callback_f _web_callback_f;
    const char *   _http_username;

    // web
    void _onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t * data, size_t len);
    void _procMsg(AsyncWebSocketClient * client, size_t sz);
    void _sendStatus();
    void _sendCustomStatus();
    void _printScanResult(int networksFound);
    void _sendTime();
    void _webserver_setup();

    // ntp
    char *   _ntp_server;
    uint16_t _ntp_interval;
    bool     _ntp_enabled;
    uint8_t  _ntp_timezone;
    bool     _have_ntp_time;
};

extern MyESP myESP;

#endif
