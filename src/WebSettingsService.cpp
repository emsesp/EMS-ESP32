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

#include "emsesp.h"

namespace emsesp {

uint8_t WebSettings::flags_;

WebSettingsService::WebSettingsService(AsyncWebServer * server, FS * fs, SecurityManager * securityManager)
    : _httpEndpoint(WebSettings::read, WebSettings::update, this, server, EMSESP_SETTINGS_SERVICE_PATH, securityManager)
    , _fsPersistence(WebSettings::read, WebSettings::update, this, fs, EMSESP_SETTINGS_FILE) {
    addUpdateHandler([&](const String & originId) { onUpdate(); }, false);
}

void WebSettings::read(WebSettings & settings, JsonObject & root) {
    root["tx_mode"]              = settings.tx_mode;
    root["tx_delay"]             = settings.tx_delay;
    root["ems_bus_id"]           = settings.ems_bus_id;
    root["syslog_enabled"]       = settings.syslog_enabled;
    root["syslog_level"]         = settings.syslog_level;
    root["trace_raw"]            = settings.trace_raw;
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
    root["analog_enabled"]       = settings.analog_enabled;
    root["pbutton_gpio"]         = settings.pbutton_gpio;
}

StateUpdateResult WebSettings::update(JsonObject & root, WebSettings & settings) {
    std::string crc_before(40, '\0');
    std::string crc_after(40, '\0');
    reset_flags();

    // tx_mode, rx and tx pins
    snprintf_P(&crc_before[0], crc_before.capacity() + 1, PSTR("%d%d%d"), settings.tx_mode, settings.rx_gpio, settings.tx_gpio);
    settings.tx_mode  = root["tx_mode"] | EMSESP_DEFAULT_TX_MODE;
    settings.tx_delay = root["tx_delay"] | EMSESP_DEFAULT_TX_DELAY;
    settings.rx_gpio  = root["rx_gpio"] | EMSESP_DEFAULT_RX_GPIO;
    settings.tx_gpio  = root["tx_gpio"] | EMSESP_DEFAULT_TX_GPIO;
    snprintf_P(&crc_after[0], crc_after.capacity() + 1, PSTR("%d%d%d"), settings.tx_mode, settings.rx_gpio, settings.tx_gpio);
    if (crc_before != crc_after) {
        add_flags(ChangeFlags::UART);
    }

    // syslog
    snprintf_P(&crc_before[0], crc_before.capacity() + 1, PSTR("%d%d%d%s"), settings.syslog_enabled, settings.syslog_level, settings.syslog_mark_interval, settings.syslog_host.c_str());
    settings.syslog_enabled       = root["syslog_enabled"] | EMSESP_DEFAULT_SYSLOG_ENABLED;
    settings.syslog_level         = root["syslog_level"] | EMSESP_DEFAULT_SYSLOG_LEVEL;
    settings.syslog_mark_interval = root["syslog_mark_interval"] | EMSESP_DEFAULT_SYSLOG_MARK_INTERVAL;
    settings.syslog_host          = root["syslog_host"] | EMSESP_DEFAULT_SYSLOG_HOST;
    settings.trace_raw            = root["trace_raw"] | EMSESP_DEFAULT_TRACELOG_RAW;
    EMSESP::trace_raw(settings.trace_raw);
    snprintf_P(&crc_after[0], crc_after.capacity() + 1, PSTR("%d%d%d%s"), settings.syslog_enabled, settings.syslog_level, settings.syslog_mark_interval, settings.syslog_host.c_str());
    if (crc_before != crc_after) {
        add_flags(ChangeFlags::SYSLOG);
    }

    // adc
    snprintf_P(&crc_before[0], crc_before.capacity() + 1, PSTR("%d"), settings.analog_enabled);
    settings.analog_enabled = root["analog_enabled"] | EMSESP_DEFAULT_ANALOG_ENABLED;
    snprintf_P(&crc_after[0], crc_after.capacity() + 1, PSTR("%d"), settings.analog_enabled);
    if (crc_before != crc_after) {
        add_flags(ChangeFlags::ADC);
    }

    // button
    snprintf_P(&crc_before[0], crc_before.capacity() + 1, PSTR("%d"), settings.pbutton_gpio);
    settings.pbutton_gpio = root["pbutton_gpio"] | EMSESP_DEFAULT_PBUTTON_GPIO;
    snprintf_P(&crc_after[0], crc_after.capacity() + 1, PSTR("%d"), settings.pbutton_gpio);
    if (crc_before != crc_after) {
        add_flags(ChangeFlags::BUTTON);
    }

    // dallas
    snprintf_P(&crc_before[0], crc_before.capacity() + 1, PSTR("%d%d"), settings.dallas_gpio, settings.dallas_parasite);
    settings.dallas_gpio     = root["dallas_gpio"] | EMSESP_DEFAULT_DALLAS_GPIO;
    settings.dallas_parasite = root["dallas_parasite"] | EMSESP_DEFAULT_DALLAS_PARASITE;
    snprintf_P(&crc_after[0], crc_after.capacity() + 1, PSTR("%d%d"), settings.dallas_gpio, settings.dallas_parasite);
    if (crc_before != crc_after) {
        add_flags(ChangeFlags::DALLAS);
    }

    // shower
    snprintf_P(&crc_before[0], crc_before.capacity() + 1, PSTR("%d%d"), settings.shower_timer, settings.shower_alert);
    settings.shower_timer = root["shower_timer"] | EMSESP_DEFAULT_SHOWER_TIMER;
    settings.shower_alert = root["shower_alert"] | EMSESP_DEFAULT_SHOWER_ALERT;
    snprintf_P(&crc_after[0], crc_after.capacity() + 1, PSTR("%d%d"), settings.shower_timer, settings.shower_alert);
    if (crc_before != crc_after) {
        add_flags(ChangeFlags::SHOWER);
    }

    // led
    snprintf_P(&crc_before[0], crc_before.capacity() + 1, PSTR("%d%d"), settings.led_gpio, settings.hide_led);
    settings.led_gpio = root["led_gpio"] | EMSESP_DEFAULT_LED_GPIO;
    settings.hide_led = root["hide_led"] | EMSESP_DEFAULT_HIDE_LED;
    snprintf_P(&crc_after[0], crc_after.capacity() + 1, PSTR("%d%d"), settings.led_gpio, settings.hide_led);
    if (crc_before != crc_after) {
        add_flags(ChangeFlags::LED);
    }

    // these both need reboots to be applied
    settings.ems_bus_id        = root["ems_bus_id"] | EMSESP_DEFAULT_EMS_BUS_ID;
    settings.master_thermostat = root["master_thermostat"] | EMSESP_DEFAULT_MASTER_THERMOSTAT;

    // doesn't need any follow-up actions
    settings.api_enabled = root["api_enabled"] | EMSESP_DEFAULT_API_ENABLED;

    return StateUpdateResult::CHANGED;
}

// this is called after any of the settings have been persisted to the filesystem
// either via the Web UI or via the Console
void WebSettingsService::onUpdate() {
    if (WebSettings::has_flags(WebSettings::ChangeFlags::SHOWER)) {
        EMSESP::shower_.start();
    }

    if (WebSettings::has_flags(WebSettings::ChangeFlags::DALLAS)) {
        EMSESP::dallassensor_.start();
    }

    if (WebSettings::has_flags(WebSettings::ChangeFlags::UART)) {
        EMSESP::init_tx();
    }

    if (WebSettings::has_flags(WebSettings::ChangeFlags::SYSLOG)) {
        EMSESP::system_.syslog_init(true); // reload settings
    }

    if (WebSettings::has_flags(WebSettings::ChangeFlags::ADC)) {
        EMSESP::system_.adc_init(true); // reload settings
    }

    if (WebSettings::has_flags(WebSettings::ChangeFlags::BUTTON)) {
        EMSESP::system_.button_init(true); // reload settings
    }

    if (WebSettings::has_flags(WebSettings::ChangeFlags::LED)) {
        EMSESP::system_.led_init(true); // reload settings
    }
}

void WebSettingsService::begin() {
    _fsPersistence.readFromFS();
}

void WebSettingsService::save() {
    _fsPersistence.writeToFS();
}

} // namespace emsesp