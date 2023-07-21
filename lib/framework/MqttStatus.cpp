#include <MqttStatus.h>

#include "../../src/emsesp_stub.hpp"

using namespace std::placeholders; // for `_1` etc

MqttStatus::MqttStatus(AsyncWebServer * server, MqttSettingsService * mqttSettingsService, SecurityManager * securityManager)
    : _mqttSettingsService(mqttSettingsService) {
    server->on(MQTT_STATUS_SERVICE_PATH,
               HTTP_GET,
               securityManager->wrapRequest(std::bind(&MqttStatus::mqttStatus, this, _1), AuthenticationPredicates::IS_AUTHENTICATED));
}

void MqttStatus::mqttStatus(AsyncWebServerRequest * request) {
    AsyncJsonResponse * response = new AsyncJsonResponse(false, MAX_MQTT_STATUS_SIZE);
    JsonObject          root     = response->getRoot();

    root["enabled"]           = _mqttSettingsService->isEnabled();
    root["connected"]         = _mqttSettingsService->isConnected();
    root["client_id"]         = _mqttSettingsService->getClientId();
    root["disconnect_reason"] = (uint8_t)_mqttSettingsService->getDisconnectReason();

    root["mqtt_queued"]   = emsesp::Mqtt::publish_queued();
    root["mqtt_fails"]    = emsesp::Mqtt::publish_fails();
    root["connect_count"] = emsesp::Mqtt::connect_count();

    response->setLength();
    request->send(response);
}
