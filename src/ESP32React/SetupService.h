#ifndef SetupService_h
#define SetupService_h

#include "APSettingsService.h"
#include "NetworkSettingsService.h"
#include "SecuritySettingsService.h"

#define SETUP_SERVICE_PATH "/rest/setup"

// the Web UI blocks an admin until the factory passwords are replaced (no universal default passwords)
class SetupService {
  public:
    SetupService(AsyncWebServer * server, SecuritySettingsService * security, APSettingsService * ap, NetworkSettingsService * network);

  private:
    SecuritySettingsService * _security;
    APSettingsService *       _ap;
    NetworkSettingsService *  _network;

    // checking a hash takes a PBKDF2 run, so the last result is kept until the stored hash changes
    struct FactoryCheck {
        String hash;
        bool   factory = false;
    };
    FactoryCheck _adminCheck;
    FactoryCheck _suCheck;

    struct State {
        bool admin       = false; // the signed-in user still has the factory admin password
        bool su          = false; // the Super User password is the factory one
        bool ap          = false; // the access point password is the factory one
        bool ap_required = false; // no WiFi network is set, so the access point is the way in
    };

    State state(const String & username);
    bool  isFactory(const String & hash, const char * factory, FactoryCheck & check);
    void  getSetup(AsyncWebServerRequest * request);
    void  saveSetup(AsyncWebServerRequest * request, JsonVariant json);
};

#endif
