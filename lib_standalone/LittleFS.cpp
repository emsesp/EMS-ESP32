/*
 * mcu-app - Microcontroller application framework
 * Copyright 2022  Simon Arlott
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
#ifdef ENV_NATIVE

#include "Arduino.h"
#include "FS.h"
#include "LittleFS.h"

fs::LittleFSFS LittleFS;

namespace fs {

LittleFSFS::LittleFSFS()
    : FS() {
}
LittleFSFS::~LittleFSFS() {
}
bool LittleFSFS::begin(bool formatOnFail, const char * basePath, uint8_t maxOpenFiles, const char * partitionLabel) {
    return true;
}
bool LittleFSFS::format() {
    return true;
}
size_t LittleFSFS::totalBytes() {
    return SIZE_MAX;
}
size_t LittleFSFS::usedBytes() {
    return 0;
}
void LittleFSFS::end() {
}

} // namespace fs
#endif
