/*
 * EMS-ESP - https://github.com/emsesp/EMS-ESP
 * Copyright 2020-2026  emsesp.org
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "settings_secrets.h"
#include "emsesp.h"

namespace emsesp {

struct SecretField {
    const char * section;
    const char * field;
};

static constexpr SecretField SECRET_FIELDS[] = {
    {"Network", "password"},
    {"AP", "password"},
    {"MQTT", "password"},
    {"Security", "jwt_secret"},
    {"Settings", "email_pass"},
};

// secrets inside arrays: the array, the field that identifies an entry, and the secret field
struct SecretListField {
    const char * section;
    const char * list;
    const char * id;
    const char * field;
};

static constexpr SecretListField SECRET_LIST_FIELDS[] = {
    {"Security", "users", "username", "password"},
    {"Security", "api_keys", "id", "hash"},
    {"Modules", "modules", "key", "license"},
};

void SettingsSecrets::redact(const char * section, JsonObject settings) {
    for (const auto & s : SECRET_FIELDS) {
        if (!strcmp(section, s.section)) {
            settings.remove(s.field);
        }
    }

    for (const auto & s : SECRET_LIST_FIELDS) {
        if (!strcmp(section, s.section)) {
            JsonArray list = settings[s.list];
            for (JsonObject entry : list) {
                entry.remove(s.field);
            }
        }
    }
}

void SettingsSecrets::merge(const char * section, JsonObject restored, JsonObjectConst current) {
    for (const auto & s : SECRET_FIELDS) {
        if (!strcmp(section, s.section) && !restored[s.field].is<JsonVariantConst>() && current[s.field].is<JsonVariantConst>()) {
            restored[s.field] = current[s.field];
        }
    }

    for (const auto & s : SECRET_LIST_FIELDS) {
        if (strcmp(section, s.section)) {
            continue;
        }

        JsonArray      list          = restored[s.list];
        JsonArrayConst current_list = current[s.list];
        for (JsonObject entry : list) {
            if (entry[s.field].is<JsonVariantConst>()) {
                continue; // backup made with secrets, or from a version without redaction
            }

            const char * id = entry[s.id] | "";
            for (JsonObjectConst existing : current_list) {
                if (!strcmp(id, existing[s.id] | "") && existing[s.field].is<JsonVariantConst>()) {
                    entry[s.field] = existing[s.field];
                    break;
                }
            }

            if (!entry[s.field].is<JsonVariantConst>()) {
                // an empty password never authenticates, an admin has to set one
                entry[s.field] = "";
                EMSESP::logger().warning("Restore: no %s for '%s' in the backup, set it after restarting", s.field, id);
            }
        }
    }
}

bool SettingsSecrets::is_private_nvs_key(const char * key) {
    return !strcmp(key, EMSESP_NVS_JWT_KEY) || !strcmp(key, EMSESP_NVS_TOKENS_RESET);
}

} // namespace emsesp
