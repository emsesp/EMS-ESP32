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

#ifndef EMSESP_SETTINGS_SECRETS_H
#define EMSESP_SETTINGS_SECRETS_H

#include <ArduinoJson.h>

namespace emsesp {

// passwords and keys in the settings files, keyed by the section name used in exports (see SECTION_MAP)
// exports leave them out unless asked to include them, and a restore keeps the device's current
// value for any that are missing from the backup
class SettingsSecrets {
  public:
    static void redact(const char * section, JsonObject settings);
    static void merge(const char * section, JsonObject restored, JsonObjectConst current);

    // NVS keys that never leave the device and are never restored
    static bool is_private_nvs_key(const char * key);
};

} // namespace emsesp

#endif
