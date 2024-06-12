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

#ifdef EMSESP_TEST
    moduleLibrary.start(emsesp_, true); // hot load the test modules as well
#else
    moduleLibrary.start(emsesp_);
#endif

    _fsPersistence.readFromFS(); // read the file from FS - this will call the update function WebModules::update()
}

void WebModulesService::loop() {
    moduleLibrary.loop(); // loop the external library modules
}

// this creates the modules settings file, saving it to the file system
// it adds data to an empty 'root' json object
// and also calls when the Modules web page is refreshed/loaded
void WebModules::read(WebModules & webModules, JsonObject root) {
    JsonDocument doc_modules;
    JsonObject   root_modules = doc_modules.to<JsonObject>();
    moduleLibrary.list(root_modules); // get list the external library modules, put in a json object

    JsonArray modules = root["modules"].to<JsonArray>();
    uint8_t   counter = 0;
    for (const JsonObject module : root_modules["modules"].as<JsonArray>()) {
        JsonObject mi = modules.add<JsonObject>();
        mi["id"]      = counter++; // id is only used to render the table and must be unique
        mi["key"]     = module["key"].as<std::string>();
        mi["name"]    = module["name"].as<std::string>();
        mi["author"]  = module["author"].as<std::string>();
        mi["version"] = module["version"].as<std::string>();
        mi["status"]  = module["status"].as<uint8_t>();
        mi["enabled"] = module["enabled"].as<bool>();
        mi["message"] = module["message"].as<std::string>();
        mi["license"] = module["license"].as<std::string>();
    }
}

// read any Module settings from the settings file
// and then apply the enable/disable that is set by the user
// This function is called when ems-esp boots and also on a save from the Modules web page
StateUpdateResult WebModules::update(JsonObject root, WebModules & webModules) {
    if (root["modules"].is<JsonArray>()) {
        for (const JsonObject module : root["modules"].as<JsonArray>()) {
            auto key     = module["key"].as<const char *>();
            auto license = module["license"].as<const char *>();
            auto enable  = module["enabled"].as<bool>();

            if (!moduleLibrary.enable(key, license, enable)) {
                return StateUpdateResult::ERROR; // throw a 400 error
            }
        }
    }

    return StateUpdateResult::CHANGED;
}

} // namespace emsesp
