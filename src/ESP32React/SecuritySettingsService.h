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

#define GENERATE_TOKEN_SIZE 512
#define GENERATE_TOKEN_PATH "/rest/generateToken"

class SecuritySettings {
  public:
    String            jwtSecret;
    std::vector<User> users;

    static void read(SecuritySettings & settings, JsonObject root) {
        // secret
        root["jwt_secret"] = settings.jwtSecret;

        // users
        JsonArray users = root["users"].to<JsonArray>();
        for (const User & user : settings.users) {
            JsonObject userRoot  = users.add<JsonObject>();
            userRoot["username"] = user.username;
            userRoot["password"] = user.password;
            userRoot["admin"]    = user.admin;
        }
    }

    static StateUpdateResult update(JsonObject root, SecuritySettings & settings) {
        // secret
        settings.jwtSecret = root["jwt_secret"] | FACTORY_JWT_SECRET;

        // users
        settings.users.clear();
        if (root["users"].is<JsonArray>()) {
            for (JsonVariant user : root["users"].as<JsonArray>()) {
                settings.users.emplace_back(user["username"], user["password"], user["admin"]);
            }
        } else {
            settings.users.emplace_back(FACTORY_ADMIN_USERNAME, FACTORY_ADMIN_PASSWORD, true);
            settings.users.emplace_back(FACTORY_GUEST_USERNAME, FACTORY_GUEST_PASSWORD, false);
        }
        return StateUpdateResult::CHANGED;
    }
};

class SecuritySettingsService final : public StatefulService<SecuritySettings>, public SecurityManager {
  public:
    SecuritySettingsService(AsyncWebServer * server, FS * fs);

    void begin();

    Authentication               authenticate(const String & username, const String & password) override;
    Authentication               authenticateRequest(AsyncWebServerRequest * request) override;
    String                       generateJWT(const User * user) override;
    ArRequestFilterFunction      filterRequest(AuthenticationPredicate predicate) override;
    ArRequestHandlerFunction     wrapRequest(ArRequestHandlerFunction onRequest, AuthenticationPredicate predicate) override;
    ArJsonRequestHandlerFunction wrapCallback(ArJsonRequestHandlerFunction callback, AuthenticationPredicate predicate) override;

  private:
    HttpEndpoint<SecuritySettings>  _httpEndpoint;
    FSPersistence<SecuritySettings> _fsPersistence;
    ArduinoJsonJWT                  _jwtHandler;

    void generateToken(AsyncWebServerRequest * request);

    void configureJWTHandler();

    Authentication authenticateJWT(String & jwt); // Lookup the user by JWT

    boolean validatePayload(JsonObject parsedPayload, const User * user); // Verify the payload is correct
};

#endif