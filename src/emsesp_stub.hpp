/*
 * EMS-ESP - https://github.com/proddy/EMS-ESP
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

// forward declarator
// used to bind EMS-ESP functions to external frameworks
#include "system.h"
#include "mqtt.h"
#include "dallassensor.h"
#include "version.h"

namespace emsesp {
class EMSESP {
  public:
    static Mqtt         mqtt_;
    static System       system_;
    static DallasSensor dallassensor_;
};

} // namespace emsesp

#endif
