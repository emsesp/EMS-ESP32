#ifdef EMSESP_STANDALONE

#include <SecuritySettingsService.h>

User ADMIN_USER = User(FACTORY_ADMIN_USERNAME, FACTORY_ADMIN_PASSWORD, true);

SecuritySettingsService::SecuritySettingsService(AsyncWebServer * server, FS * fs)
    : SecurityManager() {
}
SecuritySettingsService::~SecuritySettingsService() {
}

ArRequestFilterFunction SecuritySettingsService::filterRequest(AuthenticationPredicate predicate) {
    return [this, predicate](AsyncWebServerRequest * request) { return true; };
}

// Return the admin user on all request - disabling security features
Authentication SecuritySettingsService::authenticateRequest(AsyncWebServerRequest * request) {
    return Authentication(ADMIN_USER);
}

// Return the function unwrapped
ArRequestHandlerFunction SecuritySettingsService::wrapRequest(ArRequestHandlerFunction onRequest, AuthenticationPredicate predicate) {
    return onRequest;
}

ArJsonRequestHandlerFunction SecuritySettingsService::wrapCallback(ArJsonRequestHandlerFunction onRequest, AuthenticationPredicate predicate) {
    return onRequest;
}

#endif