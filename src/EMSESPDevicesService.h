#ifndef EMSESPDevicesService_h
#define EMSESPDevicesService_h

#include <ArduinoJson.h>
#include <AsyncJson.h>
#include <ESPAsyncWebServer.h>
#include <SecurityManager.h>

#define MAX_EMSESP_STATUS_SIZE 1024

#define EMSESP_DEVICES_SERVICE_PATH "/rest/allDevices"
#define SCAN_DEVICES_SERVICE_PATH "/rest/scanDevices"
#define DEVICE_DATA_SERVICE_PATH "/rest/deviceData"

namespace emsesp {

using namespace std::placeholders; // for `_1`

class EMSESPDevicesService {
  public:
    EMSESPDevicesService(AsyncWebServer * server, SecurityManager * securityManager);

  private:
    void all_devices(AsyncWebServerRequest * request);
    void scan_devices(AsyncWebServerRequest * request);

    AsyncCallbackJsonWebHandler _timeHandler;
    void device_data(AsyncWebServerRequest * request, JsonVariant & json);
};

} // namespace emsesp

#endif
