#ifndef ESP8266React_h
#define ESP8266React_h

#include <Arduino.h>

#include <ArduinoJson.h>
#include <AsyncJson.h>

#include <AsyncMqttClient.h>
#include <ESPAsyncWebServer.h>

#include <list>

#include <FS.h>
#include <SecurityManager.h>
#include <SecuritySettingsService.h>
#include <StatefulService.h>
#include <Network.h>

class DummySettings {
  public:
    uint8_t  tx_mode              = 1;
    uint8_t  ems_bus_id           = 0x0B;
    bool     syslog_enabled       = false;
    int8_t   syslog_level         = 3; // uuid::log::Level
    uint32_t syslog_mark_interval = 0;
    String   syslog_host          = "192.168.1.4";
    uint16_t syslog_port          = 514;
    uint8_t  master_thermostat    = 0;
    bool     shower_timer         = true;
    bool     shower_alert         = false;
    bool     hide_led             = false;
    bool     notoken_api          = false;
    bool     readonly_mode        = false;
    uint8_t  bool_format          = 1; // using "on" and "off"
    uint8_t  enum_format          = 1;
    bool     nosleep              = false;
    bool     fahrenheit           = false;
    bool     bandwidth20          = false;
    bool     telnet_enabled       = false;
    String   board_profile        = "CUSTOM";
    bool     trace_raw            = false;
    bool     analog_enabled       = true; // analog is enabled
    int8_t   weblog_level         = 1;
    uint8_t  weblog_buffer        = 50;
    bool     weblog_compact       = true;
    uint8_t  rx_gpio              = 0;
    uint8_t  tx_gpio              = 0;
    uint8_t  dallas_gpio          = 16; // to ensure its enabled
    bool     dallas_parasite      = false;
    uint8_t  led_gpio             = 0;
    bool     low_clock            = false;
    uint8_t  pbutton_gpio         = false;
    uint8_t  solar_maxflow        = 30;

    // MQTT
    uint16_t publish_time     = 10;
    uint8_t  mqtt_qos         = 0;
    bool     mqtt_retain      = false;
    bool     enabled          = true;
    uint8_t  nested_format    = 1; // 1=nested 2=single
    String   discovery_prefix = "homeassistant";
    bool     ha_enabled       = true;
    String   base             = "ems-esp";
    bool     publish_single   = false;
    bool     send_response    = true;
    String   host             = "192.168.1.4";
    uint16_t port             = 1883;
    String   clientId         = "ems-esp";
    String   username         = "";
    uint16_t keepAlive        = 60;
    bool     cleanSession     = false;

    uint16_t publish_time_boiler     = 10;
    uint16_t publish_time_thermostat = 10;
    uint16_t publish_time_solar      = 10;
    uint16_t publish_time_mixer      = 10;
    uint16_t publish_time_other      = 10;
    uint16_t publish_time_sensor     = 10;

    String hostname       = "ems-esp";
    String jwtSecret      = "ems-esp";
    String ssid           = "ems-esp";
    String password       = "ems-esp";
    String localIP        = "";
    String gatewayIP      = "";
    String subnetMask     = "";
    bool   staticIPConfig = false;
    String dnsIP1         = "";
    String dnsIP2         = "";
    bool   enableIPv6     = false;

    uint8_t phy_type       = 0;
    uint8_t eth_power      = 0; // 0 means -1
    uint8_t eth_phy_addr   = 0;
    uint8_t eth_clock_mode = 0;

#define FACTORY_MQTT_MAX_TOPIC_LENGTH 128

    static void read(DummySettings & settings, JsonObject & root){};
    static void read(DummySettings & settings){};

    static StateUpdateResult update(JsonObject & root, DummySettings & settings) {
        return StateUpdateResult::CHANGED;
    }
};

class DummySettingsService : public StatefulService<DummySettings> {
  public:
    DummySettingsService(AsyncWebServer * server, FS * fs, SecurityManager * securityManager){};

    void begin();
    void loop();
};

#define NetworkSettings DummySettings
#define SecuritySettings DummySettings
#define MqttSettings DummySettings
#define NTPSettings DummySettings

class ESP8266React {
  public:
    ESP8266React(AsyncWebServer * server, FS * fs)
        : _settings(server, fs, nullptr)
        , _securitySettingsService(server, fs){};

    void begin(){};
    void loop(){};

    SecurityManager * getSecurityManager() {
        return &_securitySettingsService;
    }

    AsyncMqttClient * getMqttClient() {
        return _mqttClient;
    }

    StatefulService<DummySettings> * getNetworkSettingsService() {
        return &_settings;
    }

    StatefulService<DummySettings> * getSecuritySettingsService() {
        return &_settings;
    }

    StatefulService<DummySettings> * getMqttSettingsService() {
        return &_settings;
    }

    StatefulService<DummySettings> * getNTPSettingsService() {
        return &_settings;
    }

  private:
    DummySettingsService    _settings;
    SecuritySettingsService _securitySettingsService;

    AsyncMqttClient * _mqttClient;
};

class EMSESPSettingsService {
  public:
    EMSESPSettingsService(AsyncWebServer * server, FS * fs, SecurityManager * securityManager);
    void begin();
};

class JsonUtils {
  public:
    static void writeIP(JsonObject & root, const String & key, const String & ip) {
        root[key] = ip;
    }
};

#endif
