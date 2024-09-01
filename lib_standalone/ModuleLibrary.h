/*
 * EMS-ESP - https://github.com/emsesp/EMS-ESP
 * Copyright 2020-2024  emsesp.org - proddy, MichaelDvP
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

#ifndef MODULELIBRARY_H
#define MODULELIBRARY_H

#include <Arduino.h>

#include <memory>
#include <vector>

#include <emsesp.h>

class ModuleLibrary {
  public:
    class Modules {
      public:
        Modules(const char * key, std::unique_ptr<Module> module)
            : key(key)
            , module(std::move(module)) {
        }
        const char *            key;
        std::unique_ptr<Module> module;
    };

    void start(emsesp::EMSESP * emsesp_main, bool test_mode = false);
    void loop();
    void list(JsonObject output);
    bool enable(const char * key, const char * license, bool enable);

    static uuid::log::Logger logger_;

  private:
    std::vector<Modules> modules_;
};

#endif
