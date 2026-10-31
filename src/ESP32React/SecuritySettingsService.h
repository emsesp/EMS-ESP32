#ifndef SecuritySettingsService_h
#define SecuritySettingsService_h

#include "SecurityManager.h"
#include "HttpEndpoint.h"
#include "FSPersistence.h"

#include "../core/password_hash.h"

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
    String            jwtSecret; // the Super User password, the name is kept for older settings files
    std::vector<User> users;
    bool              hashed_on_load = false; // plain text passwords were hashed, the file needs saving

    // the settings file. Passwords are hashed
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

    // the settings file, including ones from older versions or restored backups with plain text passwords
    static StateUpdateResult update(JsonObject root, SecuritySettings & settings) {
        // secret
        settings.jwtSecret = hashed(settings, root["jwt_secret"] | FACTORY_JWT_SECRET);

        // users
        settings.users.clear();
        if (root["users"].is<JsonArray>()) {
            for (JsonObject user : root["users"].as<JsonArray>()) {
                settings.users.emplace_back(user["username"] | "", hashed(settings, user["password"] | ""), user["admin"] | false);
            }
        } else {
            settings.users.emplace_back(FACTORY_ADMIN_USERNAME, hashed(settings, FACTORY_ADMIN_PASSWORD), true);
            settings.users.emplace_back(FACTORY_GUEST_USERNAME, hashed(settings, FACTORY_GUEST_PASSWORD), false);
        }

        return StateUpdateResult::CHANGED;
    }

    // the web UI never gets the stored passwords
    static void readForWeb(SecuritySettings & settings, JsonObject root) {
        read(settings, root);
        root["jwt_secret"] = "";
        for (JsonObject user : root["users"].as<JsonArray>()) {
            user["password"] = "";
        }
    }

    // an empty password from the web UI keeps the current one, anything else is a new password
    static StateUpdateResult updateFromWeb(JsonObject root, SecuritySettings & settings) {
        const String jwt_secret = root["jwt_secret"] | "";
        if (!jwt_secret.isEmpty()) {
            settings.jwtSecret = PasswordHash::hash(jwt_secret);
        }

        if (root["users"].is<JsonArray>()) {
            std::vector<User> current = std::move(settings.users);
            settings.users.clear();
            for (JsonObject user : root["users"].as<JsonArray>()) {
                const String username = user["username"] | "";
                String       password = user["password"] | "";
                if (password.isEmpty()) {
                    for (const User & existing : current) {
                        if (existing.username == username) {
                            password = existing.password;
                            break;
                        }
                    }
                } else {
                    password = PasswordHash::hash(password);
                }
                settings.users.emplace_back(username, password, user["admin"] | false);
            }
        }

        return StateUpdateResult::CHANGED;
    }

  private:
    static String hashed(SecuritySettings & settings, const String & password) {
        if (password.isEmpty() || PasswordHash::is_hashed(password)) {
            return password;
        }
        settings.hashed_on_load = true;
        return PasswordHash::hash(password);
    }
};

class SecuritySettingsService final : public StatefulService<SecuritySettings>, public SecurityManager {
  public:
    SecuritySettingsService(AsyncWebServer * server, FS * fs);

    void begin();

    Authentication authenticate(const String & username, const String & password) override;
    Authentication authenticateRequest(AsyncWebServerRequest * request) override;
    String         generateJWT(const User * user) override;

  private:
    HttpEndpoint<SecuritySettings>  _httpEndpoint;
    FSPersistence<SecuritySettings> _fsPersistence;
    ArduinoJsonJWT                  _jwtHandler;
    FS *                            _fs;
    String                          _adminCredentials; // to detect admin password changes

    void generateToken(AsyncWebServerRequest * request);

    void   loadSigningKey();
    void   rotateSigningKey();
    void   saveSigningKey(const String & key);
    void   onSettingsUpdated();
    String adminCredentials();

    Authentication authenticateJWT(String & jwt); // Lookup the user by JWT

    boolean validatePayload(JsonObject parsedPayload, const User * user); // Verify the payload is correct
};

#endif
