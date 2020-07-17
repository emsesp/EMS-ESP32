#include "EMSESPSettingsService.h"
#include "emsesp.h"

namespace emsesp {

EMSESPSettingsService::EMSESPSettingsService(AsyncWebServer * server, FS * fs, SecurityManager * securityManager)
    : _httpEndpoint(EMSESPSettings::read, EMSESPSettings::update, this, server, EMSESP_SETTINGS_SERVICE_PATH, securityManager)
    , _fsPersistence(EMSESPSettings::read, EMSESPSettings::update, this, fs, EMSESP_SETTINGS_FILE) {
}

void EMSESPSettings::read(EMSESPSettings & settings, JsonObject & root) {
    root["tx_mode"]    = settings.tx_mode;
    root["ems_bus_id"] = settings.ems_bus_id;

    root["syslog_level"]         = settings.syslog_level;
    root["syslog_mark_interval"] = settings.syslog_mark_interval;
    root["syslog_host"]          = settings.syslog_host;

    root["master_thermostat"] = settings.master_thermostat;
    root["shower_timer"]      = settings.shower_timer;
    root["shower_alert"]      = settings.shower_alert;
}

StateUpdateResult EMSESPSettings::update(JsonObject & root, EMSESPSettings & settings) {
    EMSESPSettings newSettings = {};
    newSettings.tx_mode        = root["tx_mode"] | EMSESP_DEFAULT_TX_MODE;
    newSettings.ems_bus_id     = root["ems_bus_id"] | EMSESP_DEFAULT_EMS_BUS_ID;

    newSettings.syslog_level         = root["syslog_level"] | EMSESP_DEFAULT_SYSLOG_LEVEL;
    newSettings.syslog_mark_interval = root["syslog_mark_interval"] | EMSESP_DEFAULT_SYSLOG_MARK_INTERVAL;
    newSettings.syslog_host          = root["syslog_host"] | EMSESP_DEFAULT_SYSLOG_HOST;

    newSettings.master_thermostat = root["master_thermostat"] | EMSESP_DEFAULT_MASTER_THERMOSTAT;
    newSettings.shower_timer      = root["shower_timer"] | EMSESP_DEFAULT_SHOWER_TIMER;
    newSettings.shower_alert      = root["shower_alert"] | EMSESP_DEFAULT_SHOWER_ALERT;

    bool changed = false;

    if (newSettings.tx_mode != settings.tx_mode) {
        EMSESP::reset_tx(newSettings.tx_mode); // reset counters
        changed = true;
    }

    if ((newSettings.shower_timer != settings.shower_timer) || (newSettings.shower_alert != settings.shower_alert)) {
        EMSESP::shower_.start();
        changed = true;
    }

    if ((newSettings.syslog_level != settings.syslog_level) || (newSettings.syslog_mark_interval != settings.syslog_mark_interval)
        || !newSettings.syslog_host.equals(settings.syslog_host)) {
        EMSESP::system_.syslog_init();
        changed = true;
    }

    if (changed) {
        settings = newSettings;
        return StateUpdateResult::CHANGED;
    }

    return StateUpdateResult::UNCHANGED;
}

void EMSESPSettingsService::begin() {
    _fsPersistence.readFromFS();
}

} // namespace emsesp