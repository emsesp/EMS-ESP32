#ifndef EMSESPDevicesService_h
#define EMSESPDevicesService_h

#include <ArduinoJson.h>
#include <AsyncJson.h>
#include <ESPAsyncWebServer.h>
#include <SecurityManager.h>

// #include <HttpEndpoint.h>
// #include <MqttPubSub.h>
// #include <WebSocketTxRx.h>

#include "version.h"

#define MAX_EMSESP_STATUS_SIZE 1024
#define EMSESP_DEVICES_SERVICE_PATH "/rest/emsespDevices"

namespace emsesp {

class EMSESPDevicesService {
  public:
    EMSESPDevicesService(AsyncWebServer * server, SecurityManager * securityManager);

  private:
    void emsespDevicesService(AsyncWebServerRequest * request);
};

} // namespace emsesp

#endif
