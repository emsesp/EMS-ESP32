#ifdef EMSESP_STANDALONE

#include "SecuritySettingsService.h"

User ADMIN_USER = User(FACTORY_ADMIN_USERNAME, FACTORY_ADMIN_PASSWORD, true);

SecuritySettingsService::SecuritySettingsService(AsyncWebServer * server, FS * fs)
    : SecurityManager() {
}
SecuritySettingsService::~SecuritySettingsService() {
}

// Return the admin user on all requests - disabling security features
Authentication SecuritySettingsService::authenticateRequest(AsyncWebServerRequest * request) {
    return Authentication(ADMIN_USER);
}

#endif