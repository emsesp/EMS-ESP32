#ifndef WebStatusService_h
#define WebStatusService_h

#define EMSESP_SYSTEM_STATUS_SERVICE_PATH "/rest/systemStatus"
#define EMSESP_CHECK_UPGRADE_PATH "/rest/checkUpgrade"
#define EMSESP_EXPORT_DATA_SERVICE_PATH "/rest/exportData"

#include <semver200.h> // for version checking

namespace emsesp {

class WebStatusService {
  public:
    WebStatusService(AsyncWebServer * server, SecurityManager * securityManager);

  private:
    void systemStatus(AsyncWebServerRequest * request);
    void checkUpgrade(AsyncWebServerRequest * request, JsonVariant json);
    void exportData(AsyncWebServerRequest * request);
};

} // namespace emsesp

#endif
