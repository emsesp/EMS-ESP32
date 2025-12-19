/*
 * EMS-ESP - https://github.com/emsesp/EMS-ESP
 * Copyright 2020-2025  emsesp.org - proddy, MichaelDvP
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

#include <esp32-psram.h>

#ifndef WebSchedulerService_h
#define WebSchedulerService_h

#define EMSESP_SCHEDULER_FILE "/config/emsespScheduler.json"
#define EMSESP_SCHEDULER_SERVICE_PATH "/rest/schedule" // GET and POST

#ifndef EMSESP_SCHEDULER_RUNNING_CORE
#define EMSESP_SCHEDULER_RUNNING_CORE 1
#endif

#ifndef EMSESP_SCHEDULER_STACKSIZE
#define EMSESP_SCHEDULER_STACKSIZE 5120
#endif

#ifndef EMSESP_SCHEDULER_PRIORITY
#define EMSESP_SCHEDULER_PRIORITY 1
#endif

// bit flags for the schedule items. Matches those in interface/src/app/main/SchedulerDialog.tsx
// 0-127 (0->0x7F) is day schedule
// 128/0x80 is timer
// 129/0x81 is on change
// 130/0x82 is on condition
// 132/0x84 is immediate
#define SCHEDULEFLAG_SCHEDULE_TIMER 0x80     // 7th bit for Timer
#define SCHEDULEFLAG_SCHEDULE_ONCHANGE 0x81  // 7th+1st bit for OnChange
#define SCHEDULEFLAG_SCHEDULE_CONDITION 0x82 // 7th+2nd bit for Condition
#define SCHEDULEFLAG_SCHEDULE_IMMEDIATE 0x84 // 7th+3rd bit for Condition

#define MAX_STARTUP_RETRIES 3 // retry the start-up commands x times

namespace emsesp {

class ScheduleItem {
  public:
    boolean     active;
    uint8_t     flags;
    uint16_t    elapsed_min; // total mins from 00:00
    stringPSRAM time;        // HH:MM
    stringPSRAM cmd;
    stringPSRAM value;
    char        name[20];
    uint8_t     retry_cnt;
};

class WebScheduler {
  public:
    std::list<ScheduleItem, AllocatorPSRAM<ScheduleItem>> scheduleItems;

    static void              read(WebScheduler & webScheduler, JsonObject root);
    static StateUpdateResult update(JsonObject root, WebScheduler & webScheduler);
};

class WebSchedulerService : public StatefulService<WebScheduler> {
  public:
    WebSchedulerService(AsyncWebServer * server, FS * fs, SecurityManager * securityManager);

    void begin();
    void loop();
    void publish_single(const char * name, const bool state);
    void publish(const bool force = false);
    bool command_setvalue(const char * value, const int8_t id, const char * name);
    bool get_value_info(JsonObject output, const char * cmd);
    void get_value_json(JsonObject output, const ScheduleItem & scheduleItem);
    void ha_reset() {
        ha_configdone_ = false;
    }
    uint8_t count_entities(bool cmd_only = false);
    bool    onChange(const char * cmd);

    std::string raw_value;
    std::string computed_value;

#if defined(EMSESP_TEST)
    void load_test_data();
#endif

// make all functions public so we can test in the debug and standalone mode
#ifndef EMSESP_STANDALONE
  private:
#endif
    static void scheduler_task(void * pvParameters);

    bool command(const char * name, const std::string & cmd, const std::string & data);
    void condition();

    HttpEndpoint<WebScheduler>  _httpEndpoint;
    FSPersistence<WebScheduler> _fsPersistence;
    bool                        ha_configdone_ = false;

    std::list<ScheduleItem, AllocatorPSRAM<ScheduleItem>> *   scheduleItems_; // pointer to the list of schedule events
    std::list<ScheduleItem *, AllocatorPSRAM<ScheduleItem *>> cmd_changed_;   // pointer to commands in list that are triggered by change
};

} // namespace emsesp

#endif