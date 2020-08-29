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

#include "EMSESPSettingsService.h"
#include "emsesp.h"

namespace emsesp {

EMSESPSettingsService::EMSESPSettingsService(AsyncWebServer * server, FS * fs, SecurityManager * securityManager)
    : _httpEndpoint(EMSESPSettings::read, EMSESPSettings::update, this, server, EMSESP_SETTINGS_SERVICE_PATH, securityManager)
    , _fsPersistence(EMSESPSettings::read, EMSESPSettings::update, this, fs, EMSESP_SETTINGS_FILE) {
    addUpdateHandler([&](const String & originId) { onUpdate(); }, false);
}

void EMSESPSettings::read(EMSESPSettings & settings, JsonObject & root) {
    root["tx_mode"]              = settings.tx_mode;
    root["ems_bus_id"]           = settings.ems_bus_id;
    root["syslog_level"]         = settings.syslog_level;
    root["syslog_mark_interval"] = settings.syslog_mark_interval;
    root["syslog_host"]          = settings.syslog_host;
    root["master_thermostat"]    = settings.master_thermostat;
    root["shower_timer"]         = settings.shower_timer;
    root["shower_alert"]         = settings.shower_alert;
    root["hide_led"]             = settings.hide_led;
    root["rx_gpio"]              = settings.rx_gpio;
    root["tx_gpio"]              = settings.tx_gpio;
    root["dallas_gpio"]          = settings.dallas_gpio;
    root["led_gpio"]             = settings.led_gpio;
}

StateUpdateResult EMSESPSettings::update(JsonObject & root, EMSESPSettings & settings) {
    settings.tx_mode              = root["tx_mode"] | EMSESP_DEFAULT_TX_MODE;
    settings.ems_bus_id           = root["ems_bus_id"] | EMSESP_DEFAULT_EMS_BUS_ID;
    settings.syslog_level         = root["syslog_level"] | EMSESP_DEFAULT_SYSLOG_LEVEL;
    settings.syslog_mark_interval = root["syslog_mark_interval"] | EMSESP_DEFAULT_SYSLOG_MARK_INTERVAL;
    settings.syslog_host          = root["syslog_host"] | EMSESP_DEFAULT_SYSLOG_HOST;
    settings.master_thermostat    = root["master_thermostat"] | EMSESP_DEFAULT_MASTER_THERMOSTAT;
    settings.shower_timer         = root["shower_timer"] | EMSESP_DEFAULT_SHOWER_TIMER;
    settings.shower_alert         = root["shower_alert"] | EMSESP_DEFAULT_SHOWER_ALERT;
    settings.hide_led             = root["hide_led"] | EMSESP_DEFAULT_HIDE_LED;
    settings.rx_gpio              = root["rx_gpio"] | EMSESP_DEFAULT_RX_GPIO;
    settings.tx_gpio              = root["tx_gpio"] | EMSESP_DEFAULT_TX_GPIO;
    settings.dallas_gpio          = root["dallas_gpio"] | EMSESP_DEFAULT_DALLAS_GPIO;
    settings.led_gpio             = root["led_gpio"] | EMSESP_DEFAULT_LED_GPIO;

    return StateUpdateResult::CHANGED;
}

// this is called after any of the settings have been persisted to the filesystem
// either via the Web UI or via the Console
void EMSESPSettingsService::onUpdate() {
    EMSESP::shower_.start();
    // EMSESP::system_.syslog_init(); // changing SysLog will require a restart
    EMSESP::init_tx();
    System::set_led();
}

void EMSESPSettingsService::begin() {
    _fsPersistence.readFromFS();
}

void EMSESPSettingsService::save() {
    _fsPersistence.writeToFS();
}

} // namespace emsesp