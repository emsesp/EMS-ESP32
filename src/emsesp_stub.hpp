/*
 * EMS-ESP - https://github.com/emsesp/EMS-ESP
 * Copyright 2020  Paul Derbyshire
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
#ifndef EMSESP_EMSESP_STUB_H
#define EMSESP_EMSESP_STUB_H

#include "system.h"
#include "mqtt.h"
#include "dallassensor.h"
#include "version.h"
#include "default_settings.h"
#include <ESP8266React.h>

#include <uuid/log.h>

// forward declarators
// used to bind EMS-ESP functions to external frameworks
namespace emsesp {
class EMSESP {
  public:
    static Mqtt         mqtt_;
    static System       system_;
    static DallasSensor dallassensor_;

    static uuid::log::Logger logger();
    static ESP8266React      esp8266React;
};

} // namespace emsesp

#endif
