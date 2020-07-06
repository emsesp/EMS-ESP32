#include <EMSESPScanDevicesService.h>

#include "emsesp.h"

namespace emsesp {

EMSESPScanDevicesService::EMSESPScanDevicesService(AsyncWebServer * server, SecurityManager * securityManager) {
    server->on(SCAN_DEVICES_SERVICE_PATH,
               HTTP_POST,
               securityManager->wrapRequest(std::bind(&EMSESPScanDevicesService::scan_devices, this, std::placeholders::_1), AuthenticationPredicates::IS_ADMIN));
}

void EMSESPScanDevicesService::scan_devices(AsyncWebServerRequest * request) {
    request->onDisconnect([]() {
        EMSESP::send_read_request(EMSdevice::EMS_TYPE_UBADevices, EMSdevice::EMS_DEVICE_ID_BOILER);
    });
    request->send(200);
}

} // namespace emsesp
