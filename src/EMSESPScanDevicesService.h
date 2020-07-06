#ifndef EMSESPScanDevicesService_h
#define EMSESPScanDevicesService_h

#include <ESPAsyncWebServer.h>
#include <SecurityManager.h>

#define SCAN_DEVICES_SERVICE_PATH "/rest/scanDevices"

namespace emsesp {

class EMSESPScanDevicesService {
  public:
    EMSESPScanDevicesService(AsyncWebServer * server, SecurityManager * securityManager);

  private:
    void scan_devices(AsyncWebServerRequest * request);
};

} // namespace emsesp

#endif
