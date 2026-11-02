#ifdef EMSESP_STANDALONE

#include "SecuritySettingsService.h"

User ADMIN_USER = User(FACTORY_ADMIN_USERNAME, FACTORY_ADMIN_PASSWORD, true);
User GUEST_USER = User(FACTORY_GUEST_USERNAME, FACTORY_GUEST_PASSWORD, false);

StandaloneIdentity SecuritySettingsService::identity_           = StandaloneIdentity::ADMIN;
bool               SecuritySettingsService::password_confirmed_ = true;

SecuritySettingsService::SecuritySettingsService(AsyncWebServer * server, FS * fs)
    : SecurityManager() {
}
SecuritySettingsService::~SecuritySettingsService() {
}

// Return the identity selected with identity(), the admin user by default
Authentication SecuritySettingsService::authenticateRequest(AsyncWebServerRequest * request) {
    if (identity_ == StandaloneIdentity::NONE) {
        return Authentication();
    }

    Authentication authentication(identity_ == StandaloneIdentity::GUEST ? GUEST_USER : ADMIN_USER);
    authentication.session            = true;
    authentication.password_confirmed = password_confirmed_;
    return authentication;
}

Authentication SecuritySettingsService::authenticate(const String & username, const String & password) {
    for (const User * user : {&ADMIN_USER, &GUEST_USER}) {
        if (username == user->username.c_str() && password == user->password.c_str()) {
            return Authentication(*user);
        }
    }
    return Authentication();
}

#endif
