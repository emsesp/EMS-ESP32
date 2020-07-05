#ifndef MqttStatus_h
#define MqttStatus_h

#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif

#include <MqttSettingsService.h>
#include <ArduinoJson.h>
#include <AsyncJson.h>
#include <ESPAsyncWebServer.h>
#include <SecurityManager.h>

#define MAX_MQTT_STATUS_SIZE 1024
#define MQTT_STATUS_SERVICE_PATH "/rest/mqttStatus"

class MqttStatus {
 public:
  MqttStatus(AsyncWebServer* server, MqttSettingsService* mqttSettingsService, SecurityManager* securityManager);

 private:
  MqttSettingsService* _mqttSettingsService;

  void mqttStatus(AsyncWebServerRequest* request);
};

#endif  // end MqttStatus_h
