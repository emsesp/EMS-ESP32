#include "EMSESPDevicesService.h"
#include "emsesp.h"
#include "mqtt.h"

namespace emsesp {

EMSESPDevicesService::EMSESPDevicesService(AsyncWebServer * server, SecurityManager * securityManager) {
    server->on(EMSESP_DEVICES_SERVICE_PATH,
               HTTP_GET,
               securityManager->wrapRequest(std::bind(&EMSESPDevicesService::emsespDevicesService, this, std::placeholders::_1),
                                            AuthenticationPredicates::IS_AUTHENTICATED));
}

void EMSESPDevicesService::emsespDevicesService(AsyncWebServerRequest * request) {
    AsyncJsonResponse * response = new AsyncJsonResponse(false, MAX_EMSESP_STATUS_SIZE);
    JsonObject          root     = response->getRoot();

    JsonArray devices = root.createNestedArray("devices");

    for (const auto & emsdevice : EMSESP::emsdevices) {
        if (emsdevice) {
            JsonObject deviceRoot   = devices.createNestedObject();
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

} // namespace emsesp