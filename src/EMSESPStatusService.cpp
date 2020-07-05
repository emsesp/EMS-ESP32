#include "EMSESPStatusService.h"
#include "emsesp.h"
#include "mqtt.h"

namespace emsesp {

EMSESPStatusService::EMSESPStatusService(AsyncWebServer * server, SecurityManager * securityManager, AsyncMqttClient * mqttClient)
    : _mqttClient(mqttClient) {
    _mqttClient->onConnect(std::bind(&EMSESPStatusService::init_mqtt, this)); // configure MQTT callback

    server->on(EMSESP_STATUS_SERVICE_PATH,
               HTTP_GET,
               securityManager->wrapRequest(std::bind(&EMSESPStatusService::emsespStatusService, this, std::placeholders::_1),
                                            AuthenticationPredicates::IS_AUTHENTICATED));
}

void EMSESPStatusService::emsespStatusService(AsyncWebServerRequest * request) {
    AsyncJsonResponse * response = new AsyncJsonResponse(false, MAX_EMSESP_STATUS_SIZE);
    JsonObject          root     = response->getRoot();

    root["version"] = EMSESP_APP_VERSION;

    root["status"]      = EMSESP::bus_status(); // 0, 1 or 2
    root["rx_received"] = EMSESP::rxservice_.telegram_count();
    root["tx_sent"]     = EMSESP::txservice_.telegram_read_count() + EMSESP::txservice_.telegram_write_count();
    root["crc_errors"]  = EMSESP::rxservice_.telegram_error_count();
    root["tx_errors"]   = EMSESP::txservice_.telegram_fail_count();
    root["mqtt_fails"]  = Mqtt::publish_fails();
    root["uptime"]      = uuid::log::format_timestamp_ms(uuid::get_uptime_ms(), 3);
    root["free_mem"]    = System::free_mem();

    response->setLength();
    request->send(response);
}

void EMSESPStatusService::init_mqtt() {
    if (!_mqttClient->connected()) {
        return;
    }

    Mqtt::on_connect();
}

} // namespace emsesp