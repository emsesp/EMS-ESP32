/*
 * EMS-ESP - https://github.com/emsesp/EMS-ESP
 * Copyright 2020-2023  Paul Derbyshire
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

#ifndef WebSchedulerService_h
#define WebSchedulerService_h

#define EMSESP_SCHEDULER_FILE "/config/emsespScheduler.json"
#define EMSESP_SCHEDULER_SERVICE_PATH "/rest/schedule" // GET and POST

#define SCHEDULEFLAG_SCHEDULE_TIMER 0x80               // 7th bit for Timer
#define MAX_STARTUP_RETRIES 3                          // retry the start-up commands x times

namespace emsesp {

class ScheduleItem {
  public:
    boolean     active;
    uint8_t     flags;
    uint16_t    elapsed_min; // total mins from 00:00
    std::string time;        // HH:MM
    std::string cmd;
    std::string value;
    std::string name;
    uint8_t     retry_cnt;
};

class WebScheduler {
  public:
    std::list<ScheduleItem> scheduleItems;

    static void              read(WebScheduler & webScheduler, JsonObject & root);
    static StateUpdateResult update(JsonObject & root, WebScheduler & webScheduler);
};

class WebSchedulerService : public StatefulService<WebScheduler> {
  public:
    WebSchedulerService(AsyncWebServer * server, FS * fs, SecurityManager * securityManager);

    void begin();
    void loop();
    void publish_single(const char * name, const bool state);
    void publish(const bool force = false);
    bool has_commands();
    bool command_setvalue(const char * value, const std::string name);
    bool get_value_info(JsonObject & output, const char * cmd);

// make all functions public so we can test in the debug and standalone mode
#ifndef EMSESP_STANDALONE
  private:
#endif
    bool command(const char * cmd, const char * data);

    HttpEndpoint<WebScheduler>  _httpEndpoint;
    FSPersistence<WebScheduler> _fsPersistence;

    std::list<ScheduleItem> * scheduleItems; // pointer to the list of schedule events
};

} // namespace emsesp

#endif