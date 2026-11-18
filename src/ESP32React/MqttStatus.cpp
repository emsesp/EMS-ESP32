#include "MqttStatus.h"

#include <emsesp.h>

MqttStatus::MqttStatus(AsyncWebServer * server, MqttSettingsService * mqttSettingsService, SecurityManager * securityManager)
    : _mqttSettingsService(mqttSettingsService) {
    securityManager->addEndpoint(server, MQTT_STATUS_SERVICE_PATH, AuthenticationPredicates::IS_AUTHENTICATED, [this](AsyncWebServerRequest * request) {
        mqttStatus(request);
    });
}

void MqttStatus::mqttStatus(AsyncWebServerRequest * request) {
    auto *     response = new emsesp::PsramAsyncJsonResponse(false);
    JsonObject root     = response->getRoot();

    root["enabled"]           = _mqttSettingsService->isEnabled();
    root["connected"]         = _mqttSettingsService->isConnected();
    root["client_id"]         = _mqttSettingsService->getClientId();
    root["disconnect_reason"] = (uint8_t)_mqttSettingsService->getDisconnectReason();
    _mqttSettingsService->read([&](MqttSettings & settings) {
        root["host"] = settings.host;
        root["port"] = settings.port;
        root["base"] = settings.base;
    });

    // seconds ago, so the web UI doesn't depend on the device clock
    const uint32_t now         = uuid::get_uptime_sec();
    const uint32_t connectedAt = _mqttSettingsService->getConnectedAt();
    if (connectedAt) {
        root["connected_for"] = now - connectedAt;
    }
    const uint32_t lastDisconnectAt = _mqttSettingsService->getLastDisconnectAt();
    if (lastDisconnectAt) {
        root["last_disconnect_ago"]    = now - lastDisconnectAt;
        root["last_disconnect_reason"] = (uint8_t)_mqttSettingsService->getLastDisconnectReason();
    }

    root["mqtt_queued"]   = emsesp::Mqtt::publish_queued();
    root["mqtt_fails"]    = emsesp::Mqtt::publish_fails();
    root["connect_count"] = emsesp::Mqtt::connect_count();

    response->setLength();
    request->send(response);
}
