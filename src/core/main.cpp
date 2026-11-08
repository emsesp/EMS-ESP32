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

#include "emsesp.h"

#ifdef EMSESP_CRASH_TEST
#include <esp_ota_ops.h>
#endif

using namespace emsesp;

static EMSESP application; // the main application

#ifdef EMSESP_CRASH_TEST
// test builds for the bootloader rollback and the recovery fallback, see [env:crashtest_s3_16M_P] in platformio.ini
static void crash_test() {
    esp_ota_img_states_t state = ESP_OTA_IMG_UNDEFINED;
    esp_ota_get_state_partition(esp_ota_get_running_partition(), &state);
    if (EMSESP_CRASH_TEST == 1 || state == ESP_OTA_IMG_VALID) {
        Serial.println("Crash test, aborting");
        Serial.flush();
        abort();
    }
}
#endif

void setup() {
    application.start();
#ifdef EMSESP_CRASH_TEST
    crash_test();
#endif
}

void loop() {
    application.loop();
#ifndef EMSESP_STANDALONE
    delay(1); // block for a tick so the idle task gets to run and the core can clock-gate
#endif
}
