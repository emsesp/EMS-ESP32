#include "MqttStatus.h"

#include <emsesp.h>

MqttStatus::MqttStatus(AsyncWebServer * server, MqttSettingsService * mqttSettingsService, SecurityManager * securityManager)
    : _mqttSettingsService(mqttSettingsService) {
    securityManager->addEndpoint(server, MQTT_STATUS_SERVICE_PATH, AuthenticationPredicates::IS_AUTHENTICATED, [this](AsyncWebServerRequest * request) {
        mqttStatus(request);
    });
}

void MqttStatus::mqttStatus(AsyncWebServerRequest * request) {
    auto *     response = new AsyncJsonResponse(false);
    JsonObject root     = response->getRoot();

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
