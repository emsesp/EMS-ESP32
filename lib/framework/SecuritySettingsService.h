#ifndef SecuritySettingsService_h
#define SecuritySettingsService_h

#include <Features.h>
#include <SecurityManager.h>
#include <HttpEndpoint.h>
#include <FSPersistence.h>

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

#define GENERATE_TOKEN_SIZE 512
#define GENERATE_TOKEN_PATH "/rest/generateToken"

class SecuritySettings {
  public:
    String            jwtSecret;
    std::vector<User> users;

    static void              read(SecuritySettings & settings, JsonObject root);
    static StateUpdateResult update(JsonObject root, SecuritySettings & settings);
};

class SecuritySettingsService : public StatefulService<SecuritySettings>, public SecurityManager {
  public:
    SecuritySettingsService(AsyncWebServer * server, FS * fs);

    void begin();

    // Functions to implement SecurityManager
    Authentication               authenticate(const String & username, const String & password);
    Authentication               authenticateRequest(AsyncWebServerRequest * request);
    String                       generateJWT(User * user);
    ArRequestFilterFunction      filterRequest(AuthenticationPredicate predicate);
    ArRequestHandlerFunction     wrapRequest(ArRequestHandlerFunction onRequest, AuthenticationPredicate predicate);
    ArJsonRequestHandlerFunction wrapCallback(ArJsonRequestHandlerFunction callback, AuthenticationPredicate predicate);

  private:
    HttpEndpoint<SecuritySettings>  _httpEndpoint;
    FSPersistence<SecuritySettings> _fsPersistence;
    ArduinoJsonJWT                  _jwtHandler;

    void generateToken(AsyncWebServerRequest * request);

    void configureJWTHandler();

    /*
   * Lookup the user by JWT
   */
    Authentication authenticateJWT(String & jwt);

    /*
   * Verify the payload is correct
   */
    boolean validatePayload(JsonObject parsedPayload, User * user);
};

#endif