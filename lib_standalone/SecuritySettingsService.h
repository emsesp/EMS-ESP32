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

class SecuritySettingsService : public SecurityManager {
  public:
    SecuritySettingsService(AsyncWebServer * server, FS * fs);
    ~SecuritySettingsService();

    // minimal set of functions to support framework with security settings disabled
    Authentication               authenticateRequest(AsyncWebServerRequest * request);
    ArRequestFilterFunction      filterRequest(AuthenticationPredicate predicate);
    ArRequestHandlerFunction     wrapRequest(ArRequestHandlerFunction onRequest, AuthenticationPredicate predicate);
    ArJsonRequestHandlerFunction wrapCallback(ArJsonRequestHandlerFunction onRequest, AuthenticationPredicate predicate);
};

#endif
