#ifndef MqttStatus_h
#define MqttStatus_h

#include <WiFi.h>
#include <MqttSettingsService.h>
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <SecurityManager.h>

// #define MAX_MQTT_STATUS_SIZE 1024
#define MQTT_STATUS_SERVICE_PATH "/rest/mqttStatus"

class MqttStatus {
  public:
    MqttStatus(AsyncWebServer * server, MqttSettingsService * mqttSettingsService, SecurityManager * securityManager);

  private:
    MqttSettingsService * _mqttSettingsService;

    void mqttStatus(AsyncWebServerRequest * request);
};

#endif
