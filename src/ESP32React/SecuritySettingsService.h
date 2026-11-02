#ifndef SecuritySettingsService_h
#define SecuritySettingsService_h

#include "SecurityManager.h"
#include "HttpEndpoint.h"
#include "FSPersistence.h"

#include "../core/api_key.h"
#include "../core/password_hash.h"
#include "../core/password_confirm.h"

#ifndef FACTORY_ADMIN_USERNAME
#define FACTORY_ADMIN_USERNAME "admin"
#endif

#ifndef FACTORY_ADMIN_PASSWORD
#define FACTORY_ADMIN_PASSWORD "admin"
#endif

#define SECURITY_SETTINGS_FILE "/config/securitySettings.json"
#define SECURITY_SETTINGS_PATH "/rest/securitySettings"

#define API_KEYS_PATH "/rest/apiKeys"

#define SESSION_TOKEN_LIFETIME (30 * 24 * 3600) // seconds

struct ApiKeyEntry {
    String   id; // the first hex digits of the key
    String   label;
    bool     write;   // read+write, otherwise read only
    uint32_t created; // unix time, 0 if the clock wasn't set
    String   hash;
};

class SecuritySettings {
  public:
    std::vector<User>        users;
    std::vector<ApiKeyEntry> api_keys;
    bool              hashed_on_load  = false; // plain text passwords were hashed, the file needs saving
    bool              dropped_on_load = false; // the old Super User password was in the file, the file needs saving

    // the settings file. Passwords are hashed
    static void read(SecuritySettings & settings, JsonObject root) {
        // users
        JsonArray users = root["users"].to<JsonArray>();
        for (const User & user : settings.users) {
            JsonObject userRoot  = users.add<JsonObject>();
            userRoot["username"] = user.username;
            userRoot["password"] = user.password;
            userRoot["admin"]    = user.admin;
            userRoot["ver"]      = user.ver;
        }

        JsonArray keys = root["api_keys"].to<JsonArray>();
        for (const ApiKeyEntry & key : settings.api_keys) {
            JsonObject keyRoot = keys.add<JsonObject>();
            keyRoot["id"]      = key.id;
            keyRoot["label"]   = key.label;
            keyRoot["write"]   = key.write;
            keyRoot["created"] = key.created;
            keyRoot["hash"]    = key.hash;
        }
    }

    // the settings file, including ones from older versions or restored backups with plain text passwords
    static StateUpdateResult update(JsonObject root, SecuritySettings & settings) {
        // the Super User password of older versions is no longer used
        settings.dropped_on_load = root["jwt_secret"].is<const char *>();

        // users
        settings.users.clear();
        if (root["users"].is<JsonArray>()) {
            for (JsonObject user : root["users"].as<JsonArray>()) {
                settings.users.emplace_back(user["username"] | "", hashed(settings, user["password"] | ""), user["admin"] | false, user["ver"] | 0u);
            }
        } else {
            // a fresh install only has the admin user, the setup screen in the Web UI asks for new passwords
            settings.users.emplace_back(FACTORY_ADMIN_USERNAME, hashed(settings, FACTORY_ADMIN_PASSWORD), true);
        }

        settings.api_keys.clear();
        for (JsonObject key : root["api_keys"].as<JsonArray>()) {
            settings.api_keys.push_back({key["id"] | "", key["label"] | "", key["write"] | false, key["created"] | 0u, key["hash"] | ""});
        }

        return StateUpdateResult::CHANGED;
    }

    // the web UI never gets the stored passwords
    static void readForWeb(SecuritySettings & settings, JsonObject root) {
        read(settings, root);
        root.remove("api_keys"); // listed by API_KEYS_PATH, without the hashes
        for (JsonObject user : root["users"].as<JsonArray>()) {
            user["password"] = "";
            user.remove("ver");
        }
    }

    // an empty password from the web UI keeps the current one, anything else is a new password
    static StateUpdateResult updateFromWeb(JsonObject root, SecuritySettings & settings) {
        if (root["users"].is<JsonArray>()) {
            std::vector<User> current = std::move(settings.users);
            settings.users.clear();
            for (JsonObject user : root["users"].as<JsonArray>()) {
                const String username = user["username"] | "";
                String       password = user["password"] | "";
                const User * existing = nullptr;
                for (const User & u : current) {
                    if (u.username == username) {
                        existing = &u;
                        break;
                    }
                }

                // a new password signs the user out everywhere. A new user doesn't get the tokens of a removed one with the same name
                uint32_t ver;
                if (!existing) {
                    PasswordHash::random_bytes(reinterpret_cast<uint8_t *>(&ver), sizeof(ver));
                } else {
                    ver = password.isEmpty() ? existing->ver : existing->ver + 1;
                }
                if (password.isEmpty()) {
                    password = existing ? existing->password : "";
                } else {
                    password = PasswordHash::hash(password);
                }
                settings.users.emplace_back(username, password, user["admin"] | false, ver);
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
    Authentication authenticateApiRequest(AsyncWebServerRequest * request) override;
    String         generateJWT(const User * user) override;
    bool           confirmPassword(AsyncWebServerRequest * request, const String & password) override;

  private:
    HttpEndpoint<SecuritySettings>  _httpEndpoint;
    FSPersistence<SecuritySettings> _fsPersistence;
    ArduinoJsonJWT                  _jwtHandler;
    FS *                            _fs;
    String                          _adminCredentials; // to detect admin password changes
    PasswordConfirm                 _passwordConfirm;

    void listApiKeys(AsyncWebServerRequest * request);
    void apiKeyAction(AsyncWebServerRequest * request, JsonVariant json);

    void   loadSigningKey();
    void   rotateSigningKey();
    void   saveSigningKey(const String & key);
    void   onSettingsUpdated();
    String adminCredentials();

    Authentication authenticateJWT(String & jwt); // Lookup the user by JWT
    static String  bearerToken(AsyncWebServerRequest * request);

    boolean validatePayload(JsonObject parsedPayload, const User * user); // Verify the payload is correct
};

#endif
