/*
 * EMS-ESP - https://github.com/proddy/EMS-ESP
 * Copyright 2019  Paul Derbyshire
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

#ifndef EMSESPSettingsConfig_h
#define EMSESPSettingsConfig_h

#include <HttpEndpoint.h>
#include <FSPersistence.h>

#define EMSESP_SETTINGS_FILE "/config/emsespSettings.json"
#define EMSESP_SETTINGS_SERVICE_PATH "/rest/emsespSettings"

#define EMSESP_DEFAULT_TX_MODE 1       // EMS1.0
#define EMSESP_DEFAULT_EMS_BUS_ID 0x0B // service key

#define EMSESP_DEFAULT_SYSLOG_LEVEL -1 // OFF
#define EMSESP_DEFAULT_SYSLOG_MARK_INTERVAL 0
#define EMSESP_DEFAULT_SYSLOG_HOST ""

#define EMSESP_DEFAULT_MASTER_THERMOSTAT 0 // not set
#define EMSESP_DEFAULT_SHOWER_TIMER false
#define EMSESP_DEFAULT_SHOWER_ALERT false

namespace emsesp {

enum MQTT_format : uint8_t { SINGLE = 1, NESTED, HA, CUSTOM };

class EMSESPSettings {
  public:
    uint8_t tx_mode;
    uint8_t ems_bus_id;
    uint8_t master_thermostat;
    bool    shower_timer;
    bool    shower_alert;

    // syslog
    int8_t   syslog_level; // uuid::log::Level
    uint32_t syslog_mark_interval;
    String   syslog_host;

    static void              read(EMSESPSettings & settings, JsonObject & root);
    static StateUpdateResult update(JsonObject & root, EMSESPSettings & settings);
};

class EMSESPSettingsService : public StatefulService<EMSESPSettings> {
  public:
    EMSESPSettingsService(AsyncWebServer * server, FS * fs, SecurityManager * securityManager);

    void begin();
    void save();

  private:
    HttpEndpoint<EMSESPSettings>  _httpEndpoint;
    FSPersistence<EMSESPSettings> _fsPersistence;

    void onUpdate();
};

} // namespace emsesp

#endif