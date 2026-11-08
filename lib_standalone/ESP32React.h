#ifndef ESP32React_h
#define ESP32React_h

#include <list>

#include "Arduino.h"
#include "ArduinoJson.h"
#include "AsyncJson.h"
#include "ESPAsyncWebServer.h"
#include "FS.h"
#include "SecurityManager.h"
#include "SecuritySettingsService.h"
#include "StatefulService.h"
#include "Network.h"
// #include "IPAddress.h"

#include <espMqttClient.h>

#define AP_SETTINGS_FILE "/config/apSettings.json"
#define MQTT_SETTINGS_FILE "/config/mqttSettings.json"
#define NETWORK_SETTINGS_FILE "/config/networkSettings.json"
#define NTP_SETTINGS_FILE "/config/ntpSettings.json"
#define EMSESP_SETTINGS_FILE "/config/emsespSettings.json"

// AP provision modes, mirrors src/ESP32React/APSettingsService.h
#define AP_MODE_DISCONNECTED 1
#define AP_MODE_NEVER 2

class DummySettings {
  public:
    // SYSTEM
    bool bandwidth20 = false;
    bool nosleep     = true;
    bool eth_10mbit  = false;

    // MQTT
    uint16_t publish_time       = 10;
    uint8_t  mqtt_qos           = 0;
    bool     mqtt_retain        = false;
    bool     enabled            = true;
    uint8_t  nested_format      = 1; // 1=nested 2=single
    String   discovery_prefix   = "homeassistant";
    uint8_t  discovery_type     = 0; // HA
    uint8_t  ha_number_mode     = 0; // box
    bool     ha_enabled         = true;
    String   base               = "ems-esp";
    bool     publish_single     = false;
    bool     publish_single2cmd = false;
    bool     send_response      = false; // don't send response
    String   host               = "192.168.1.4";
    uint16_t port               = 1883;
    String   clientId           = "ems-esp";
    String   username           = "";
    uint16_t keepAlive          = 60;
    bool     cleanSession       = false;
    uint8_t  entity_format      = 1;
    String   CORSOrigin         = "*";
    uint8_t  tx_power           = 0;
    String   bssid              = "";
    String   localIP            = "";
    String   gatewayIP          = "";
    String   subnetMask         = "";
    bool     staticIPConfig     = false;
    String   dnsIP1             = "";
    String   dnsIP2             = "";
    bool     enableMDNS         = true;
    bool     enableCORS         = false;
    uint8_t  channel            = 1;
    bool     ssid_hidden        = false;
    uint8_t  max_clients        = 4;
    bool     ssidHidden         = false;
    uint8_t  maxClients         = 4;

    uint16_t publish_time_boiler     = 10;
    uint16_t publish_time_thermostat = 10;
    uint16_t publish_time_solar      = 10;
    uint16_t publish_time_mixer      = 10;
    uint16_t publish_time_other      = 10;
    uint16_t publish_time_sensor     = 10;
    uint16_t publish_time_heartbeat  = 60;
    uint32_t publish_time_water      = 0;

    String hostname  = "ems-esp";
    String jwtSecret = "ems-esp";
    String ssid      = "ems-esp";
    String password  = "ems-esp";

    // AP
    uint8_t provisionMode = AP_MODE_DISCONNECTED; // always

    // NTP
    String  server          = "pool.ntp.org";
    String  tzLabel         = "Europe/London";
    String  tzFormat        = "GMT0BST,M3.5.0/1,M10.5.0";
    String  tzLabelT        = "Europe/Berlin";
    String  tzFormatT       = "CET-1CEST,M3.5.0,M10.5.0/3";
    uint8_t thermostat_sync = 0;

    static void read(DummySettings & settings, JsonObject root) {};
    static void read(DummySettings & settings) {};

    static StateUpdateResult update(JsonObject root, DummySettings & settings) {
        return StateUpdateResult::CHANGED;
    }
};

class DummySettingsService : public StatefulService<DummySettings> {
  public:
    DummySettingsService(AsyncWebServer * server, FS * fs, SecurityManager * securityManager) {};

    void begin();
    void loop();
};

#define NetworkSettings DummySettings
#define SecuritySettings DummySettings
#define MqttSettings DummySettings
#define NTPSettings DummySettings
#define APSettings DummySettings

class ESP32React {
  public:
    ESP32React(AsyncWebServer * server, FS * fs)
        : _settings(server, fs, nullptr)
        , _securitySettingsService(server, fs) {};

    void begin() {
        _mqttClient = new espMqttClient();
    };
    void loop() {};

    SecurityManager * getSecurityManager() {
        return &_securitySettingsService;
    }

    espMqttClient * getMqttClient() {
        return _mqttClient;
    }

    bool apStatus() {
        return false;
    }

    void setWill(const char * will_topic) {
    }
    void onMessage(espMqttClientTypes::OnMessageCallback callback) {
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

    StatefulService<DummySettings> * getAPSettingsService() {
        return &_settings;
    }

  private:
    DummySettingsService    _settings;
    SecuritySettingsService _securitySettingsService;

    espMqttClient * _mqttClient;
};

class EMSESPSettingsService {
  public:
    EMSESPSettingsService(AsyncWebServer * server, FS * fs, SecurityManager * securityManager);
    void begin();
};

class JsonUtils {
  public:
    static void writeIP(JsonObject root, const String & key, const String & ip) {
        root[key] = ip;
    }
};

#endif