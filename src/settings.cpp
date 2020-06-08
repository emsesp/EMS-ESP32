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

#include "settings.h"

MAKE_PSTR(settings_filename, "/settings.msgpack")
MAKE_PSTR(settings_backup_filename, "/settings.msgpack~")

MAKE_PSTR(logger_name, "settings")

namespace emsesp {

// clang-format off
#define EMSESP_SETTINGS_DATA \
		EMSESP_SETTINGS_SIMPLE(std::string, "", app_version, "", (), "2.0") \
		EMSESP_SETTINGS_SIMPLE(std::string, "", admin_password, "", (), EMSESP_DEFAULT_ADMIN_PASSWORD) \
		EMSESP_SETTINGS_SIMPLE(std::string, "", hostname, "", (), EMSESP_DEFAULT_HOSTNAME) \
		EMSESP_SETTINGS_SIMPLE(std::string, "", wifi_ssid, "", (), "") \
		EMSESP_SETTINGS_SIMPLE(std::string, "", wifi_password, "", (), "") \
        EMSESP_SETTINGS_CUSTOM(std::string, "", syslog_host, "", (), "") \
		EMSESP_SETTINGS_ENUM(uuid::log::Level, "", syslog_level, "", (), uuid::log::Level::OFF) \
		EMSESP_SETTINGS_SIMPLE(unsigned long, "", syslog_mark_interval, "", (), EMSESP_DEFAULT_SYSLOG_INTERVAL) \
        EMSESP_SETTINGS_SIMPLE(uint8_t, "", ems_bus_id, "", (), EMSESP_DEFAULT_BUS_ID) \
        EMSESP_SETTINGS_SIMPLE(uint8_t, "", ems_tx_mode, "", (), EMSESP_DEFAULT_TX_MODE) \
        EMSESP_SETTINGS_SIMPLE(bool, "", ems_read_only, "", (), EMSESP_DEFAULT_EMS_READ_ONLY) \
        EMSESP_SETTINGS_SIMPLE(bool, "", shower_timer, "", (), EMSESP_DEFAULT_SHOWER_TIMER) \
        EMSESP_SETTINGS_SIMPLE(bool, "", shower_alert, "", (), EMSESP_DEFAULT_SHOWER_ALERT) \
        EMSESP_SETTINGS_SIMPLE(uint8_t, "", master_thermostat, "", (), EMSESP_DEFAULT_MASTER_THERMOSTAT) \
        EMSESP_SETTINGS_SIMPLE(uint16_t, "", mqtt_publish_time, "", (), EMSESP_DEFAULT_MQTT_PUBLISH_TIME) \
		EMSESP_SETTINGS_SIMPLE(std::string, "", mqtt_ip, "", (), "") \
		EMSESP_SETTINGS_SIMPLE(std::string, "", mqtt_user, "", (), "") \
		EMSESP_SETTINGS_SIMPLE(std::string, "", mqtt_password, "", (), "") \
		EMSESP_SETTINGS_SIMPLE(uint16_t, "", mqtt_port, "", (), EMSESP_DEFAULT_MQTT_PORT) \
		EMSESP_SETTINGS_SIMPLE(bool, "", mqtt_enabled, "", (), EMSESP_DEFAULT_MQTT_ENABLED) \
		EMSESP_SETTINGS_SIMPLE(std::string, "", mqtt_base, "", (), EMSESP_DEFAULT_MQTT_BASE) \
		EMSESP_SETTINGS_SIMPLE(uint8_t, "", mqtt_qos, "", (), EMSESP_DEFAULT_MQTT_QOS) \
		EMSESP_SETTINGS_SIMPLE(bool, "", mqtt_retain, "", (), EMSESP_DEFAULT_MQTT_RETAIN) \
		EMSESP_SETTINGS_SIMPLE(uint8_t, "", mqtt_format, "", (), EMSESP_DEFAULT_MQTT_FORMAT) \
		EMSESP_SETTINGS_SIMPLE(bool, "", mqtt_heartbeat, "", (), EMSESP_DEFAULT_MQTT_HEARTBEAT) 

#define EMSESP_SETTINGS_SIMPLE EMSESP_SETTINGS_GENERIC
#define EMSESP_SETTINGS_CUSTOM EMSESP_SETTINGS_GENERIC
#define EMSESP_SETTINGS_ENUM EMSESP_SETTINGS_GENERIC

/* Create member data and flash strings */
#define EMSESP_SETTINGS_GENERIC(__type, __key_prefix, __name, __key_suffix, __get_function, __read_default, ...) \
		__type Settings::__name##_; \
		MAKE_PSTR(__name, __key_prefix #__name __key_suffix)
EMSESP_SETTINGS_DATA
#undef EMSESP_SETTINGS_GENERIC

#undef EMSESP_SETTINGS_ENUM

void Settings::read_settings(const ArduinoJson::JsonDocument &doc) {
#define EMSESP_SETTINGS_GENERIC(__type, __key_prefix, __name, __key_suffix, __get_function, __read_default, ...) \
		__name(doc[FPSTR(__pstr__##__name)] | __read_default, ##__VA_ARGS__);
#define EMSESP_SETTINGS_ENUM(__type, __key_prefix, __name, __key_suffix, __get_function, __read_default, ...) \
		__name(static_cast<__type>(doc[FPSTR(__pstr__##__name)] | static_cast<int>(__read_default)), ##__VA_ARGS__);
	EMSESP_SETTINGS_DATA
#undef EMSESP_SETTINGS_GENERIC
#undef EMSESP_SETTINGS_ENUM
}

void Settings::write_settings(ArduinoJson::JsonDocument &doc) {
#define EMSESP_SETTINGS_GENERIC(__type, __key_prefix, __name, __key_suffix, __get_function, __read_default, ...) \
		doc[FPSTR(__pstr__##__name)] = __name __get_function;
#define EMSESP_SETTINGS_ENUM(__type, __key_prefix, __name, __key_suffix, __get_function, __read_default, ...) \
		doc[FPSTR(__pstr__##__name)] = static_cast<int>(__name __get_function);
	EMSESP_SETTINGS_DATA
#undef EMSESP_SETTINGS_GENERIC
}

#undef EMSESP_SETTINGS_GENERIC
#undef EMSESP_SETTINGS_SIMPLE
#undef EMSESP_SETTINGS_CUSTOM
#undef EMSESP_SETTINGS_ENUM

/* Create getters/setters for simple settings items only */
#define EMSESP_SETTINGS_SIMPLE(__type, __key_prefix, __name, __key_suffix, __get_function, __read_default, ...) \
		__type Settings::__name() const { \
			return __name##_; \
		} \
		void Settings::__name(const __type &__name) { \
			__name##_ = __name; \
		}
#define EMSESP_SETTINGS_ENUM(__type, __key_prefix, __name, __key_suffix, __get_function, __read_default, ...) \
		__type Settings::__name() const { \
			return __name##_; \
		} \
		void Settings::__name(__type __name) { \
			__name##_ = __name; \
		}

/* Create getters for settings items with custom setters */
#define EMSESP_SETTINGS_CUSTOM(__type, __key_prefix, __name, __key_suffix, __get_function, __read_default, ...) \
		__type Settings::__name() const { \
			return __name##_; \
		}

EMSESP_SETTINGS_DATA

#undef EMSESP_SETTINGS_SIMPLE
#undef EMSESP_SETTINGS_CUSTOM
#undef EMSESP_SETTINGS_ENUM
// clang-format on

uuid::log::Logger Settings::logger_{F_(logger_name), uuid::log::Facility::DAEMON};

// Settings which we don't persist
bool Settings::mounted_     = false;
bool Settings::unavailable_ = false;
bool Settings::loaded_      = false;

Settings::Settings() {
    if (!unavailable_ && !mounted_) {
        EMSuart::stop(); // temporary suspend UART because is can cause interference on the UART

#ifdef EMSESP_STANDALONE
        if (true) {
#else
#if defined(ESP8266)
        if (EMSESP_FS.begin()) {
#elif defined(ESP32)
        if (EMSESP_FS.begin(true)) {
#endif
#endif
            LOG_INFO(F("Mounted filesystem"));
            mounted_ = true;
        } else {
            LOG_ERROR(F("Unable to mount filesystem"));
            unavailable_ = true;
        }
    }

    if (mounted_ && !loaded_) {
        if (read_settings(uuid::read_flash_string(F_(settings_filename))) || read_settings(uuid::read_flash_string(F_(settings_backup_filename)))) {
            loaded_ = true;
        }
    }

    if (!loaded_) {
        LOG_ERROR(F("Failed to load settings. Using defaults"));
        read_settings(ArduinoJson::StaticJsonDocument<0>());
        loaded_ = true;
    }
    EMSuart::restart();
}

// save settings to FS
void Settings::commit() {
    if (mounted_) {
        std::string filename        = uuid::read_flash_string(F_(settings_filename));
        std::string backup_filename = uuid::read_flash_string(F_(settings_backup_filename));

        EMSuart::stop(); // temporary suspend UART because is can cause interference on the UART

        LOG_DEBUG(F("Saving settings"));
        if (write_settings(filename)) {
            if (read_settings(filename, false)) {
                write_settings(backup_filename);
            }
        }

        EMSuart::restart(); // re-enable EMS bus
    }
}

// read the settings from FS
bool Settings::read_settings(const std::string & filename, bool load) {
#ifndef EMSESP_STANDALONE
    File file = EMSESP_FS.open(filename.c_str(), "r");
    if (file) {
        ArduinoJson::DynamicJsonDocument doc(BUFFER_SIZE);

        auto error = ArduinoJson::deserializeMsgPack(doc, file);

        if (error) {
            LOG_ERROR(F("Failed to parse settings file %s: %s"), filename.c_str(), error.c_str());
            return false;
        } else {
            if (load) {
                LOG_INFO(F("Loading settings from file %s"), filename.c_str());
                read_settings(doc);
            }
            return true;
        }
    } else {
        LOG_ERROR(F("Settings file %s does not exist"), filename.c_str());
        return false;
    }
#else
    LOG_INFO(F("Loading settings from file %s (%d)"), filename.c_str(), load);
    return false;
#endif
}

// write settings from FS
bool Settings::write_settings(const std::string & filename) {
#ifndef EMSESP_STANDALONE
    File file = EMSESP_FS.open(filename.c_str(), "w");
    if (file) {
        ArduinoJson::DynamicJsonDocument doc(BUFFER_SIZE);

        write_settings(doc);

        ArduinoJson::serializeMsgPack(doc, file);

        if (file.getWriteError()) {
            LOG_ERROR(F("Failed to write settings file %s: %u"), filename.c_str(), file.getWriteError());
            return false;
        } else {
            return true;
        }
    } else {
        LOG_ERROR(F("Unable to open settings file %s for writing"), filename.c_str());
        return false;
    }
#else
    LOG_DEBUG(F("Write settings file %s"), filename.c_str());
    return false;
#endif
}

// print the settings JSON to console (only in admin mode)
void Settings::show_settings(uuid::console::Shell & shell) {
#ifdef EMSESP_DEBUG
    if (shell.has_flags(CommandFlags::ADMIN)) {
        shell.printf(F("[DEBUG] Settings JSON: "));
        ArduinoJson::DynamicJsonDocument doc(BUFFER_SIZE);
        write_settings(doc);
        ArduinoJson::serializeJson(doc, shell);
        shell.println();
    }
#endif
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

// format the FS. Wipes everything.
void Settings::format(uuid::console::Shell & shell) {
#ifndef EMSESP_STANDALONE
#if defined(ESP8266)
    EMSuart::stop();
    if (EMSESP_FS.begin()) {
#elif defined(ESP32)
    if (EMSESP_FS.begin(true)) {
#endif
        if (EMSESP_FS.remove(uuid::read_flash_string(F_(settings_filename)).c_str())) {
            (void)EMSESP_FS.remove(uuid::read_flash_string(F_(settings_backup_filename)).c_str());
            auto msg = F("Reset settings file to defaults");
            shell.logger().warning(msg);
            shell.flush();
            System::restart(false);
        } else {
            auto msg = F("Error removing settings file");
            shell.logger().emerg(msg);
        }
    } else {
        auto msg = F("Unable to mount filesystem");
        shell.logger().alert(msg);
    }
#if defined(ESP8266)
    EMSuart::restart();
#endif
#endif
}

void Settings::syslog_host(const std::string & syslog_host) {
#ifndef EMSESP_STANDALONE
    IPAddress addr;

    if (addr.fromString(syslog_host.c_str())) {
        syslog_host_ = syslog_host;
    } else {
        syslog_host_.clear();
    }
#else
    syslog_host_ = std::string("99.99.99.99");
#endif
}

} // namespace emsesp

#pragma GCC diagnostic pop
