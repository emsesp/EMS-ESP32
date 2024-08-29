#ifndef WebStatusService_h
#define WebStatusService_h

#define EMSESP_HARDWARE_STATUS_SERVICE_PATH "/rest/hardwareStatus"
#define EMSESP_SYSTEM_STATUS_SERVICE_PATH "/rest/systemStatus"
#define EMSESP_CHECK_UPGRADE_PATH "/rest/checkUpgrade"

#include <semver200.h>

namespace emsesp {

class WebStatusService {
  public:
    WebStatusService(AsyncWebServer * server, SecurityManager * securityManager);

  private:
    void systemStatus(AsyncWebServerRequest * request);
    void hardwareStatus(AsyncWebServerRequest * request);
    void checkUpgrade(AsyncWebServerRequest * request, JsonVariant json);
};

} // namespace emsesp

#endif
