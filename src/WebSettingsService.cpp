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

#include "WebSettingsService.h"
#include "emsesp.h"

namespace emsesp {

WebSettingsService::WebSettingsService(AsyncWebServer * server, FS * fs, SecurityManager * securityManager)
    : _httpEndpoint(WebSettings::read, WebSettings::update, this, server, EMSESP_SETTINGS_SERVICE_PATH, securityManager)
    , _fsPersistence(WebSettings::read, WebSettings::update, this, fs, EMSESP_SETTINGS_FILE) {
    addUpdateHandler([&](const String & originId) { onUpdate(); }, false);
}

void WebSettings::read(WebSettings & settings, JsonObject & root) {
    root["tx_mode"]              = settings.tx_mode;
    root["ems_bus_id"]           = settings.ems_bus_id;
    root["syslog_level"]         = settings.syslog_level;
    root["syslog_mark_interval"] = settings.syslog_mark_interval;
    root["syslog_host"]          = settings.syslog_host;
    root["master_thermostat"]    = settings.master_thermostat;
    root["shower_timer"]         = settings.shower_timer;
    root["shower_alert"]         = settings.shower_alert;
    root["rx_gpio"]              = settings.rx_gpio;
    root["tx_gpio"]              = settings.tx_gpio;
    root["dallas_gpio"]          = settings.dallas_gpio;
    root["dallas_parasite"]      = settings.dallas_parasite;
    root["led_gpio"]             = settings.led_gpio;
    root["hide_led"]             = settings.hide_led;
    root["api_enabled"]          = settings.api_enabled;
    root["bool_format"]          = settings.bool_format;
    root["analog_enabled"]       = settings.analog_enabled;
}

StateUpdateResult WebSettings::update(JsonObject & root, WebSettings & settings) {
    settings.tx_mode              = root["tx_mode"] | EMSESP_DEFAULT_TX_MODE;
    settings.ems_bus_id           = root["ems_bus_id"] | EMSESP_DEFAULT_EMS_BUS_ID;
    settings.syslog_level         = root["syslog_level"] | EMSESP_DEFAULT_SYSLOG_LEVEL;
    settings.syslog_mark_interval = root["syslog_mark_interval"] | EMSESP_DEFAULT_SYSLOG_MARK_INTERVAL;
    settings.syslog_host          = root["syslog_host"] | EMSESP_DEFAULT_SYSLOG_HOST;
    settings.master_thermostat    = root["master_thermostat"] | EMSESP_DEFAULT_MASTER_THERMOSTAT;
    settings.shower_timer         = root["shower_timer"] | EMSESP_DEFAULT_SHOWER_TIMER;
    settings.shower_alert         = root["shower_alert"] | EMSESP_DEFAULT_SHOWER_ALERT;
    settings.rx_gpio              = root["rx_gpio"] | EMSESP_DEFAULT_RX_GPIO;
    settings.tx_gpio              = root["tx_gpio"] | EMSESP_DEFAULT_TX_GPIO;
    settings.dallas_gpio          = root["dallas_gpio"] | EMSESP_DEFAULT_DALLAS_GPIO;
    settings.dallas_parasite      = root["dallas_parasite"] | EMSESP_DEFAULT_DALLAS_PARASITE;
    settings.led_gpio             = root["led_gpio"] | EMSESP_DEFAULT_LED_GPIO;
    settings.hide_led             = root["hide_led"] | EMSESP_DEFAULT_HIDE_LED;
    settings.api_enabled          = root["api_enabled"] | EMSESP_DEFAULT_API_ENABLED;
    settings.bool_format          = root["bool_format"] | EMSESP_DEFAULT_BOOL_FORMAT;
    settings.analog_enabled       = root["analog_enabled"] | EMSESP_DEFAULT_ANALOG_ENABLED;

    return StateUpdateResult::CHANGED;
}

// this is called after any of the settings have been persisted to the filesystem
// either via the Web UI or via the Console
void WebSettingsService::onUpdate() {
    EMSESP::shower_.start();
    EMSESP::dallassensor_.start();
    System::init();
}

void WebSettingsService::begin() {
    _fsPersistence.readFromFS();
}

void WebSettingsService::save() {
    _fsPersistence.writeToFS();
}

} // namespace emsesp