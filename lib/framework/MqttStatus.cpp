#include <MqttStatus.h>

#include "../../src/emsesp_stub.hpp"

using namespace std::placeholders; // for `_1` etc

MqttStatus::MqttStatus(PsychicHttpServer * server, MqttSettingsService * mqttSettingsService, SecurityManager * securityManager)
    : _mqttSettingsService(mqttSettingsService)
    , _server(server)
    , _securityManager(securityManager) {
}

void MqttStatus::registerURI() {
    _server->on(MQTT_STATUS_SERVICE_PATH,
                HTTP_GET,
                _securityManager->wrapRequest(std::bind(&MqttStatus::mqttStatus, this, _1), AuthenticationPredicates::IS_AUTHENTICATED));
}

esp_err_t MqttStatus::mqttStatus(PsychicRequest * request) {
    PsychicJsonResponse response = PsychicJsonResponse(request, false, MAX_MQTT_STATUS_SIZE);
    JsonObject          root     = response.getRoot();

    root["enabled"]           = _mqttSettingsService->isEnabled();
    root["connected"]         = _mqttSettingsService->isConnected();
    root["client_id"]         = _mqttSettingsService->getClientId();
    root["disconnect_reason"] = (uint8_t)_mqttSettingsService->getDisconnectReason();

    root["mqtt_queued"]   = emsesp::Mqtt::publish_queued();
    root["mqtt_fails"]    = emsesp::Mqtt::publish_fails();
    root["connect_count"] = emsesp::Mqtt::connect_count();

    return response.send();
}
