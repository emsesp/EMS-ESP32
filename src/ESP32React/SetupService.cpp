#include "SetupService.h"

#include "../core/psram_async_json_response.h"

#include <emsesp.h>

static constexpr size_t AP_PASSWORD_MIN = 8; // WPA2
static constexpr size_t AP_PASSWORD_MAX = 63;

SetupService::SetupService(AsyncWebServer * server, SecuritySettingsService * security, APSettingsService * ap, NetworkSettingsService * network)
    : _security(security)
    , _ap(ap)
    , _network(network) {
    security->addEndpoint(server, SETUP_SERVICE_PATH, AuthenticationPredicates::IS_ADMIN, [this](AsyncWebServerRequest * request) { getSetup(request); });
    security->addEndpoint(server, SETUP_SERVICE_PATH, AuthenticationPredicates::IS_ADMIN, [this](AsyncWebServerRequest * request, JsonVariant json) {
        saveSetup(request, json);
    });
}

bool SetupService::isFactory(const char * hash, const char * factory, FactoryCheck & check) {
    const PasswordHash::Digest digest = PasswordHash::digest(hash);
    if (check.hash != digest) {
        check.hash    = digest;
        check.factory = PasswordHash::verify(factory, hash);
    }
    return check.factory;
}

SetupService::State SetupService::state(const String & username) {
    State       s;
    stringPSRAM admin_hash;
    _security->read([&](const SecuritySettings & settings) {
        for (const User & user : settings.users) {
            if (user.username == username) {
                admin_hash = user.password;
            }
        }
    });
    s.admin = !admin_hash.empty() && isFactory(admin_hash.c_str(), FACTORY_ADMIN_PASSWORD, _adminCheck);

    _ap->read([&](const APSettings & settings) { s.ap = settings.password == FACTORY_AP_PASSWORD; });
    bool has_wifi = false;
    _network->read([&](const NetworkSettings & settings) { has_wifi = !settings.ssid.isEmpty(); });
    s.ap_required = s.ap && !has_wifi;
    return s;
}

void SetupService::getSetup(AsyncWebServerRequest * request) {
    const Authentication authentication = _security->authenticateRequest(request);
    const State          s              = state(authentication.user->username);

    auto *     response = new emsesp::PsramAsyncJsonResponse(false);
    JsonObject root     = response->getRoot();
    root["admin"]       = s.admin;
    root["ap"]          = s.ap;
    root["ap_required"] = s.ap_required;
    response->setLength();
    request->send(response);
}

// sets the new passwords and returns a new token, as an admin password change signs everyone out
void SetupService::saveSetup(AsyncWebServerRequest * request, JsonVariant json) {
    const Authentication authentication = _security->authenticateRequest(request);
    const String         username       = authentication.user->username;
    const State          s              = state(username);

    const String password      = json["password"] | "";
    const String ap_password   = json["ap_password"] | "";
    const String view_username = json["view_username"] | "";
    const String view_password = json["view_password"] | "";

    bool valid = true;
    if (s.admin && (password.isEmpty() || password == FACTORY_ADMIN_PASSWORD)) {
        valid = false;
    }
    if (s.ap_required && ap_password.isEmpty()) {
        valid = false;
    }
    if (!ap_password.isEmpty() && (ap_password.length() < AP_PASSWORD_MIN || ap_password.length() > AP_PASSWORD_MAX || ap_password == FACTORY_AP_PASSWORD)) {
        valid = false;
    }
    if (view_username.isEmpty() != view_password.isEmpty()) {
        valid = false;
    }
    if (!view_username.isEmpty()) {
        _security->read([&](const SecuritySettings & settings) {
            for (const User & user : settings.users) {
                if (user.username == view_username) {
                    valid = false;
                }
            }
        });
    }
    if (!valid) {
        request->send(400);
        return;
    }

    _security->update([&](SecuritySettings & settings) {
        if (!password.isEmpty()) {
            for (User & user : settings.users) {
                if (user.username == username) {
                    user.password = PasswordHash::hash(password).c_str();
                    user.ver++;
                }
            }
        }
        if (!view_username.isEmpty()) {
            uint32_t ver;
            PasswordHash::random_bytes(reinterpret_cast<uint8_t *>(&ver), sizeof(ver));
            settings.users.emplace_back(view_username, PasswordHash::hash(view_password), false, ver);
        }
        return StateUpdateResult::CHANGED;
    });

    if (!ap_password.isEmpty()) {
        _ap->update([&](APSettings & settings) {
            settings.password = ap_password;
            return StateUpdateResult::CHANGED;
        });
    }

    emsesp::EMSESP::logger().notice("Setup: factory passwords replaced by %s", username.c_str());

    auto *     response = new emsesp::PsramAsyncJsonResponse(false);
    JsonObject root     = response->getRoot();
    _security->read([&](const SecuritySettings & settings) {
        for (const User & user : settings.users) {
            if (user.username == username) {
                root["access_token"] = _security->generateJWT(&user);
            }
        }
    });
    response->setLength();
    request->send(response);
}
