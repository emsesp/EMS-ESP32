#ifndef EMSESPStatusService_h
#define EMSESPStatusService_h

#include <ArduinoJson.h>
#include <AsyncJson.h>
#include <ESPAsyncWebServer.h>
#include <SecurityManager.h>
#include <AsyncMqttClient.h>

#define MAX_EMSESP_STATUS_SIZE 1024
#define EMSESP_STATUS_SERVICE_PATH "/rest/emsespStatus"

namespace emsesp {

class EMSESPStatusService {
  public:
    EMSESPStatusService(AsyncWebServer * server, SecurityManager * securityManager);

  private:
    void emsespStatusService(AsyncWebServerRequest * request);

#ifdef ESP32
    static void onStationModeConnected(WiFiEvent_t event, WiFiEventInfo_t info);
    static void onStationModeDisconnected(WiFiEvent_t event, WiFiEventInfo_t info);
    static void onStationModeGotIP(WiFiEvent_t event, WiFiEventInfo_t info);
#elif defined(ESP8266)
    WiFiEventHandler _onStationModeConnectedHandler;
    WiFiEventHandler _onStationModeDisconnectedHandler;
    WiFiEventHandler _onStationModeGotIPHandler;
    static void      onStationModeConnected(const WiFiEventStationModeConnected & event);
    static void      onStationModeDisconnected(const WiFiEventStationModeDisconnected & event);
    static void      onStationModeGotIP(const WiFiEventStationModeGotIP & event);
#endif
};

} // namespace emsesp

#endif
