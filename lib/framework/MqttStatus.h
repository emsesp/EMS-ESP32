#ifndef MqttStatus_h
#define MqttStatus_h

#include <WiFi.h>
#include <MqttSettingsService.h>
#include <ArduinoJson.h>
#include <PsychicHttp.h>
#include <SecurityManager.h>

#define MAX_MQTT_STATUS_SIZE 1024
#define MQTT_STATUS_SERVICE_PATH "/rest/mqttStatus"

class MqttStatus {
  public:
    MqttStatus(PsychicHttpServer * server, MqttSettingsService * mqttSettingsService, SecurityManager * securityManager);
    void registerURI();

  private:
    SecurityManager *   _securityManager;
    PsychicHttpServer * _server;

    MqttSettingsService * _mqttSettingsService;

    esp_err_t mqttStatus(PsychicRequest * request);
};

#endif
