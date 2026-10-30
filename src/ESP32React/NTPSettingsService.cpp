#include "NTPSettingsService.h"

#include "tz_zones.h"

#include <emsesp.h>

namespace {

String trimmedTzFormat(const String & format) {
    int space = format.indexOf(' ');
    return space < 0 ? format : format.substring(0, static_cast<unsigned int>(space));
}

void padTzFormat(String & format) {
    format.reserve(NTP_TZ_FORMAT_LEN);
    while (format.length() < NTP_TZ_FORMAT_LEN) {
        format += ' ';
    }
}

} // namespace

NTPSettingsService::NTPSettingsService(AsyncWebServer * server, FS * fs, SecurityManager * securityManager)
    : _httpEndpoint(NTPSettings::read, NTPSettings::update, this, server, NTP_SETTINGS_SERVICE_PATH, securityManager)
    , _fsPersistence(NTPSettings::read, NTPSettings::update, this, fs, NTP_SETTINGS_FILE)
    , _connected(false) {
    // POST
    securityManager->addEndpoint(server, TIME_PATH, AuthenticationPredicates::IS_ADMIN, [this](AsyncWebServerRequest * request, JsonVariant json) {
        configureTime(request, json);
    });

    addUpdateHandler([this] { configureNTP(); }, false);
}

void NTPSettingsService::begin() {
    _fsPersistence.readFromFS();

    // After an OTA that updates tz_zones.h, rewrite stored POSIX strings from the
    // saved IANA labels so localtime / thermostat sync use the current rules
    // without the user re-selecting the zone in the WebUI.
    update([](NTPSettings & settings) {
        return NTPSettings::syncFormatsFromLabels(settings) ? StateUpdateResult::CHANGED : StateUpdateResult::UNCHANGED;
    });
}

void NTPSettingsService::loop() {
    if (_connected != emsesp::EMSESP::network_.network_connected()) {
        _connected = emsesp::EMSESP::network_.network_connected();
        configureNTP();
    }
}

// https://werner.rothschopf.net/microcontroller/202103_arduino_esp32_ntp_en.htm
void NTPSettingsService::configureNTP() {
    emsesp::EMSESP::system_.ntp_connected(false);
    if (_connected && _state.enabled) {
        emsesp::EMSESP::logger().info("Starting NTP service");
        esp_sntp_set_sync_interval(3600000); // one hour
        esp_sntp_set_time_sync_notification_cb(ntp_received);
        configTzTime(_state.tzFormat.c_str(), _state.server.c_str());
    } else {
        setenv("TZ", _state.tzFormat.c_str(), 1);
        tzset();
        if (esp_sntp_enabled()) {
            esp_sntp_stop();
        }
    }
}

void NTPSettingsService::configureTime(AsyncWebServerRequest * request, JsonVariant json) {
    if (json.is<JsonObject>()) {
        struct tm tm        = {};
        String    timeLocal = json["local_time"];
        char *    s         = strptime(timeLocal.c_str(), "%Y-%m-%dT%H:%M:%S", &tm);
        if (s != nullptr) {
            tm.tm_isdst         = -1; // not set by strptime, tells mktime to determine daylightsaving
            time_t         time = mktime(&tm);
            struct timeval now  = {.tv_sec = time, .tv_usec = {}};
#if CONFIG_IDF_TARGET_ESP32C3
            // settimeofday and adjtime() does not work, unknown how to set time
            emsesp::EMSESP::logger().warning("manual clock setting not possible");
#else
            settimeofday(&now, nullptr);
#endif
            AsyncWebServerResponse * response = request->beginResponse(200);
            request->send(response);
            return;
        }
    }

    AsyncWebServerResponse * response = request->beginResponse(400);
    request->send(response);
}

void NTPSettingsService::ntp_received(struct timeval * tv) {
    (void)tv;
    // emsesp::EMSESP::logger().info("NTP sync to %d sec", tv->tv_sec);
    emsesp::EMSESP::system_.ntp_connected(true);
}

void NTPSettings::read(NTPSettings & settings, JsonObject root) {
    root["enabled"]         = settings.enabled;
    root["server"]          = settings.server;
    root["tz_label"]        = settings.tzLabel;
    root["tz_format"]       = trimmedTzFormat(settings.tzFormat);
    root["thermostat_sync"] = settings.thermostat_sync;
    root["tz_label_t"]      = settings.tzLabelT;
    root["tz_format_t"]     = trimmedTzFormat(settings.tzFormatT);
}

bool NTPSettings::syncFormatsFromLabels(NTPSettings & settings) {
    bool changed = false;

    auto apply = [&](const String & label, String & format) {
        const char * mapped = emsesp_tz_format_for_label(label.c_str());
        if (mapped == nullptr) {
            return;
        }
        if (trimmedTzFormat(format) == mapped) {
            // Still re-pad in case NTP_TZ_FORMAT_LEN grew since the value was stored.
            if (format.length() != NTP_TZ_FORMAT_LEN) {
                format = mapped;
                padTzFormat(format);
                changed = true;
            }
            return;
        }
        format = mapped;
        padTzFormat(format);
        changed = true;
    };

    apply(settings.tzLabel, settings.tzFormat);
    apply(settings.tzLabelT, settings.tzFormatT);
    return changed;
}

StateUpdateResult NTPSettings::update(JsonObject root, NTPSettings & settings) {
    settings.enabled         = root["enabled"] | FACTORY_NTP_ENABLED;
    settings.server          = root["server"] | FACTORY_NTP_SERVER;
    settings.tzLabel         = root["tz_label"] | FACTORY_NTP_TIME_ZONE_LABEL;
    settings.tzFormat        = root["tz_format"] | FACTORY_NTP_TIME_ZONE_FORMAT;
    settings.thermostat_sync = root["thermostat_sync"] | FACTORY_NTP_THERMOSTAT_SYNC;
    settings.tzLabelT        = root["tz_label_t"] | FACTORY_NTP_TIME_ZONE_LABEL;
    settings.tzFormatT       = root["tz_format_t"] | FACTORY_NTP_TIME_ZONE_FORMAT;

    // Do not call syncFormatsFromLabels here: readFromFS() uses this updater
    // via updateWithoutPropagation, and a sync at that point would not be
    // written back. Boot migration runs in NTPSettingsService::begin() instead.
    // The WebUI always POSTs tz_format from TIME_ZONES when the user saves.

    // The format string must have a fixed length to avoid a setenv leak, #3184.
    padTzFormat(settings.tzFormat);
    padTzFormat(settings.tzFormatT);

    return StateUpdateResult::CHANGED;
}
