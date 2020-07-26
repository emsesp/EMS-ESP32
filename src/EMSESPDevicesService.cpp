#include "EMSESPDevicesService.h"
#include "emsesp.h"
#include "mqtt.h"

namespace emsesp {

EMSESPDevicesService::EMSESPDevicesService(AsyncWebServer * server, SecurityManager * securityManager)
    : _timeHandler(DEVICE_DATA_SERVICE_PATH,
                   securityManager->wrapCallback(std::bind(&EMSESPDevicesService::device_data, this, _1, _2), AuthenticationPredicates::IS_AUTHENTICATED)) {
    // body
    server->on(EMSESP_DEVICES_SERVICE_PATH,
               HTTP_GET,
               securityManager->wrapRequest(std::bind(&EMSESPDevicesService::all_devices, this, _1), AuthenticationPredicates::IS_AUTHENTICATED));

    server->on(SCAN_DEVICES_SERVICE_PATH,
               HTTP_GET,
               securityManager->wrapRequest(std::bind(&EMSESPDevicesService::scan_devices, this, _1), AuthenticationPredicates::IS_AUTHENTICATED));

    _timeHandler.setMethod(HTTP_POST);
    _timeHandler.setMaxContentLength(256);
    server->addHandler(&_timeHandler);
}

void EMSESPDevicesService::scan_devices(AsyncWebServerRequest * request) {
    EMSESP::send_read_request(EMSdevice::EMS_TYPE_UBADevices, EMSdevice::EMS_DEVICE_ID_BOILER);
    request->send(200);
}

void EMSESPDevicesService::all_devices(AsyncWebServerRequest * request) {
    AsyncJsonResponse * response = new AsyncJsonResponse(false, MAX_EMSESP_STATUS_SIZE);
    JsonObject          root     = response->getRoot();

    JsonArray devices = root.createNestedArray("devices");
    for (const auto & emsdevice : EMSESP::emsdevices) {
        if (emsdevice) {
            JsonObject deviceRoot   = devices.createNestedObject();
            deviceRoot["id"]        = emsdevice->unique_id();
            deviceRoot["type"]      = emsdevice->device_type_name();
            deviceRoot["brand"]     = emsdevice->brand_to_string();
            deviceRoot["name"]      = emsdevice->name();
            deviceRoot["deviceid"]  = emsdevice->device_id();
            deviceRoot["productid"] = emsdevice->product_id();
            deviceRoot["version"]   = emsdevice->version();
        }
    }

    response->setLength();
    request->send(response);
}

void EMSESPDevicesService::device_data(AsyncWebServerRequest * request, JsonVariant & json) {
    if (json.is<JsonObject>()) {
        uint8_t id = json["id"]; // get id from selected table row

        AsyncJsonResponse * response = new AsyncJsonResponse(false, 1024);
        EMSESP::device_info(id, (JsonObject &)response->getRoot());
        response->setLength();
        request->send(response);
    } else {
        AsyncWebServerResponse * response = request->beginResponse(200);
        request->send(response);
    }
}

} // namespace emsesp