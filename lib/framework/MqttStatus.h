#ifndef MqttStatus_h
#define MqttStatus_h

#include <WiFi.h>
#include <MqttSettingsService.h>
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <SecurityManager.h>

#define MQTT_STATUS_SERVICE_PATH "/rest/mqttStatus"

class MqttStatus {
  public:
    MqttStatus(AsyncWebServer * server, MqttSettingsService * mqttSettingsService, SecurityManager * securityManager);

  private:
    MqttSettingsService * _mqttSettingsService;

    void mqttStatus(AsyncWebServerRequest * request);
};

#endif
