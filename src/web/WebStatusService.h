#ifndef WebStatusService_h
#define WebStatusService_h

#define EMSESP_SYSTEM_STATUS_SERVICE_PATH "/rest/systemStatus"

#define EMSESP_ACTION_SERVICE_PATH "/rest/action"

#include <semver200.h> // for version checking

namespace emsesp {

class WebStatusService {
  public:
    WebStatusService(AsyncWebServer * server, SecurityManager * securityManager);

  protected:
    void systemStatus(AsyncWebServerRequest * request);
    void action(AsyncWebServerRequest * request, JsonVariant json);

  private:
    bool checkUpgrade(JsonObject root, std::string & latest_version);
    bool exportData(JsonObject root, std::string & type);
    bool customSupport(JsonObject root);
};

} // namespace emsesp

#endif
