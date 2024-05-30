/*
 * EMS-ESP - https://github.com/emsesp/EMS-ESP
 * Copyright 2020-2024  Paul Derbyshire
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

#include "emsesp.h"
#include "WebModulesService.h"

namespace emsesp {

ModuleLibrary moduleLibrary; // Module Library
EMSESP *      emsesp_;       // forward declaration

WebModulesService::WebModulesService(AsyncWebServer * server, FS * fs, SecurityManager * securityManager)
    : _httpEndpoint(WebModules::read, WebModules::update, this, server, EMSESP_MODULES_SERVICE_PATH, securityManager, AuthenticationPredicates::IS_AUTHENTICATED)
    , _fsPersistence(WebModules::read, WebModules::update, this, fs, EMSESP_MODULES_FILE) {
}

// load the settings when the service starts
void WebModulesService::begin() {
    EMSESP::logger().info("Starting Modules service");
    moduleLibrary.start(emsesp_);

    _fsPersistence.readFromFS(); // read the file from FS
}

void WebModulesService::loop() {
    moduleLibrary.loop(); // loop the external library modules
}

// this creates the modules file, saving it to the FS
// and also calls when the Modules web page is refreshed
void WebModules::read(WebModules & webModules, JsonObject root) {
#ifdef EMSESP_DEBUG
    EMSESP::logger().debug("module read called");
#endif

    JsonDocument doc_modules;
    JsonObject   root_modules = doc_modules.to<JsonObject>();
    moduleLibrary.list(root_modules); // get list the external library modules, put in root json object

    JsonArray modules = root["modules"].to<JsonArray>();
    uint8_t   counter = 0;
    for (const JsonObject module : root_modules["modules"].as<JsonArray>()) {
        JsonObject mi = modules.add<JsonObject>();
        mi["id"]      = counter++; // id is only used to render the table and must be unique
        mi["name"]    = module["name"].as<std::string>();
        mi["author"]  = module["author"].as<std::string>();
        mi["version"] = module["version"].as<std::string>();
        mi["status"]  = module["status"].as<std::string>();
        mi["enabled"] = module["enabled"].as<bool>();
    }
}

// read any Module settings from the FS settings
// and then apply the enable/disable
// it's also called on a save
StateUpdateResult WebModules::update(JsonObject root, WebModules & webModules) {
#ifdef EMSESP_DEBUG
    EMSESP::logger().debug("module update called");
#endif

    if (root["modules"].is<JsonArray>()) {
        for (const JsonObject module : root["modules"].as<JsonArray>()) {
            // set enabled/disabled
            moduleLibrary.enable(module["name"], module["enabled"].as<bool>());
        }
    }

    return StateUpdateResult::CHANGED_RESTART;
}

} // namespace emsesp
