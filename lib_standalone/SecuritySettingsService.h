#ifndef SecuritySettingsService_h
#define SecuritySettingsService_h

#include "SecurityManager.h"
#include "HttpEndpoint.h"
#include "FSPersistence.h"

#ifndef FACTORY_ADMIN_USERNAME
#define FACTORY_ADMIN_USERNAME "admin"
#endif

#ifndef FACTORY_ADMIN_PASSWORD
#define FACTORY_ADMIN_PASSWORD "admin"
#endif

#ifndef FACTORY_GUEST_USERNAME
#define FACTORY_GUEST_USERNAME "guest"
#endif

#ifndef FACTORY_GUEST_PASSWORD
#define FACTORY_GUEST_PASSWORD "guest"
#endif

#define SECURITY_SETTINGS_FILE "/config/securitySettings.json"
#define SECURITY_SETTINGS_PATH "/rest/securitySettings"

// who authenticateRequest() reports as the caller, set by unit tests
enum class StandaloneIdentity : uint8_t { NONE, GUEST, ADMIN };

class SecuritySettingsService : public SecurityManager {
  public:
    SecuritySettingsService(AsyncWebServer * server, FS * fs);
    ~SecuritySettingsService();

    // minimal set of functions to support framework with security settings disabled
    Authentication authenticateRequest(AsyncWebServerRequest * request);
    Authentication authenticate(const String & username, const String & password); // the factory admin and guest users

    static void identity(StandaloneIdentity identity) {
        identity_ = identity;
    }

  private:
    static StandaloneIdentity identity_;
};

#endif
