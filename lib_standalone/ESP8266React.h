#ifndef ESP8266React_h
#define ESP8266React_h

#include <Arduino.h>
#include <ArduinoJson.h>
#include <AsyncJson.h>
#include <SecurityManager.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <AsyncMqttClient.h>
#include <StatefulService.h>

class DummySettings {
  public:
    uint8_t  tx_mode;
    uint8_t  ems_bus_id;
    bool     system_heartbeat;
    int8_t   syslog_level; // uuid::log::Level
    uint32_t syslog_mark_interval;
    String   syslog_host;
    uint8_t  master_thermostat;
    bool     shower_timer;
    bool     shower_alert;

    uint16_t publish_time; // seconds
    uint8_t  mqtt_format;  // 1=single, 2=nested, 3=ha, 4=custom
    uint8_t  mqtt_qos;

    String   hostname;
    String   jwtSecret;
    String   ssid;
    String   password;

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

  private:
};

#define WiFiSettings DummySettings
#define SecuritySettings DummySettings
#define MqttSettings DummySettings

class ESP8266React {
  public:
    ESP8266React(AsyncWebServer * server, FS * fs)
        : _settings(server, fs, nullptr){};

    void begin(){};
    void loop(){};

    SecurityManager * getSecurityManager() {
        return nullptr;
    }

    AsyncMqttClient * getMqttClient() {
        return _mqttClient;
    }

    StatefulService<DummySettings> * getWiFiSettingsService() {
        return &_settings;
    }

    StatefulService<DummySettings> * getSecuritySettingsService() {
        return &_settings;
    }

    StatefulService<DummySettings> * getMqttSettingsService() {
        return &_settings;
    }

  private:
    DummySettingsService _settings;
    AsyncMqttClient *    _mqttClient;
};

class EMSESPSettingsService {
  public:
    EMSESPSettingsService(AsyncWebServer * server, FS * fs, SecurityManager * securityManager);

    void begin();

  private:
    // HttpEndpoint<EMSESPSettings>  _httpEndpoint;
    // FSPersistence<EMSESPSettings> _fsPersistence;
};

#endif
