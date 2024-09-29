#ifndef WebStatusService_h
#define WebStatusService_h

#define EMSESP_SYSTEM_STATUS_SERVICE_PATH "/rest/systemStatus"
#define EMSESP_ACTION_SERVICE_PATH "/rest/action"

#include <semver200.h> // for version checking

namespace emsesp {

class WebStatusService {
  public:
    WebStatusService(AsyncWebServer * server, SecurityManager * securityManager);

// make all functions public so we can test in the debug and standalone mode
#ifndef EMSESP_STANDALONE
  protected:
#endif
    void systemStatus(AsyncWebServerRequest * request);
    void action(AsyncWebServerRequest * request, JsonVariant json);

  private:
    bool checkUpgrade(JsonObject root, std::string & latest_version);
    bool exportData(JsonObject root, std::string & type);
    bool customSupport(JsonObject root);
    void allvalues(JsonObject output);
};

} // namespace emsesp

#endif
