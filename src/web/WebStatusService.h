#ifndef WebStatusService_h
#define WebStatusService_h

#define ESPSYSTEM_STATUS_SERVICE_PATH "/rest/ESPSystemStatus"
#define SYSTEM_STATUS_SERVICE_PATH "/rest/systemStatus"

namespace emsesp {

class WebStatusService {
  public:
    WebStatusService(AsyncWebServer * server, SecurityManager * securityManager);

  private:
    void systemStatus(AsyncWebServerRequest * request);
    void ESPsystemStatus(AsyncWebServerRequest * request);
};

} // namespace emsesp

#endif
