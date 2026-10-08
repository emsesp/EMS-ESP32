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

#ifndef EMSESP_SYSTEM_H_
#define EMSESP_SYSTEM_H_

#include <Arduino.h>
#include <ArduinoJson.h>

#include "helpers.h"
#include "console.h"
#include "mqtt.h"
#include "telegram.h"
#include "led.h"

#ifndef EMSESP_STANDALONE
#include <esp_wifi.h>
#include <ETH.h>
#include <uuid/syslog.h>
#endif

#include <esp32-psram.h>
#include <uuid/log.h>
#include <PButton.h>

#if CONFIG_IDF_TARGET_ESP32
// there is no official API available on the original ESP32
extern "C" {
uint8_t temprature_sens_read();
}
#elif CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32S2
#include "driver/temperature_sensor.h"
#endif

using uuid::console::Shell;

#define EMSESP_FS_CONFIG_DIRECTORY "/config"

#define EMSESP_CUSTOMSUPPORT_FILE "/config/customSupport.json"

namespace emsesp {

enum PHY_type : uint8_t { PHY_TYPE_NONE = 0, PHY_TYPE_LAN8720, PHY_TYPE_TLK110, PHY_TYPE_RTL8201 };

enum SYSTEM_STATUS : uint8_t {
    SYSTEM_STATUS_NORMAL            = 0,
    SYSTEM_STATUS_PENDING_UPLOAD    = 1,
    SYSTEM_STATUS_UPLOADING         = 100,
    SYSTEM_STATUS_ERROR_UPLOAD      = 3,
    SYSTEM_STATUS_PENDING_RESTART   = 4,
    SYSTEM_STATUS_RESTART_REQUESTED = 5,
    SYSTEM_STATUS_INVALID_GPIO      = 6
};

enum FUSE_VALUE : uint8_t { ALL = 0, MFG = 1, MODEL = 2, BOARD = 3, REV = 4, BATCH = 5, FUSE = 6 };

enum EMAIL_SECURITY : uint8_t { NONE = 0, SSL = 1, STARTTLS = 2 };
struct PartitionInfo {
    std::string version;
    size_t      size;
    time_t      install_date; // UTC epoch seconds; 0 if unknown. Format with localtime() at render time so it honors the current TZ.
};

class System {
  public:
    void start();
    bool loop(); // returns true if the LED flash is active

    // commands
    static bool command_read(const char * value, const int8_t id);
    static bool command_send(const char * value, const int8_t id);
    static bool command_publish(const char * value, const int8_t id);
    static bool command_fetch(const char * value, const int8_t id);
    static bool command_restart(const char * value, const int8_t id);
    static bool command_format(const char * value, const int8_t id);
    static bool command_watch(const char * value, const int8_t id);
    static bool command_message(const char * value, const int8_t id, JsonObject output);
    static bool command_info(const char * value, const int8_t id, JsonObject output);
    static bool command_response(const char * value, const int8_t id, JsonObject output);
    static bool command_service(const char * cmd, const char * value);
    static bool command_sendmail(const char * value, const int8_t id);
    static bool command_txpause(const char * value, const int8_t id);
    static bool command_led(const char * value, const int8_t id);

    static bool        get_value_info(JsonObject root, const char * cmd);
    static void        get_value_json(JsonObject output, const std::string & circuit, const std::string & name, JsonVariant val);
    static std::string get_metrics_prometheus();

    static void listDir(const char * dirname, uint8_t levels);

#if defined(EMSESP_TEST)
    static bool command_test(const char * value, const int8_t id);
#endif

    std::string reset_reason(uint8_t cpu) const;

    void store_nvs_values();
    void system_restart(const char * partition = nullptr);
    void get_partition_info();

    void show_mem(const char * note);
    void store_settings(class WebSettings & settings);
    void syslog_init();
    void modbus_init();
    bool check_upgrade();
    bool check_restore();
    void heartbeat_json(JsonObject output);

    void send_heartbeat();
    void send_info_mqtt();

    bool syslog_enabled() {
        return syslog_enabled_;
    }

#ifndef EMSESP_STANDALONE
    unsigned long syslog_count() {
        return syslog_.message_count();
    }

    unsigned long syslog_fails() {
        return syslog_.message_fails();
    }
#endif

    String getBBQKeesGatewayDetails(uint8_t detail = 0);

    static bool uploadFirmwareURL(const char * url = nullptr);

    void button_init();
    void commands_init();
    void uart_init();

    void    systemStatus(uint8_t status_code);
    uint8_t systemStatus();

    static void exportSettings(const std::string & type, const char * filename, JsonObject output);
    static void exportSystemBackup(JsonObject output);
    static bool saveSettings(const char * filename, JsonObject input);

    static bool                 add_gpio(uint8_t pin, const char * source_name);
    static std::vector<uint8_t> available_gpios();
    static bool                 load_board_profile(std::vector<int8_t> & data, const std::string & board_profile);
    static void                 make_snapshot_gpios(std::vector<int8_t> & u_gpios, std::vector<int8_t> & s_gpios);
    static void                 restore_snapshot_gpios(std::vector<int8_t> & u_gpios, std::vector<int8_t> & s_gpios);

    static bool readCommand(const char * data);

    static String get_ip_or_hostname();

    bool telnet_enabled() {
        return telnet_enabled_;
    }

    bool modbus_enabled() {
        return modbus_enabled_;
    }

    uint16_t modbus_port() {
        return modbus_port_;
    }

    uint8_t modbus_max_clients() {
        return modbus_max_clients_;
    }

    uint32_t modbus_timeout() {
        return modbus_timeout_;
    }

    bool readonly_mode() {
        return readonly_mode_;
    }

    void readonly_mode(bool readonly_mode) {
        readonly_mode_ = readonly_mode;
    }

    bool developer_mode() {
        return developer_mode_;
    }

    void developer_mode(bool developer_mode) {
        developer_mode_ = developer_mode;
    }

    bool disable_reset() {
        return disable_reset_;
    }

    void disable_reset(bool disable_reset) {
        disable_reset_ = disable_reset;
    }

    bool auto_fw_check() {
        return auto_fw_check_;
    }

    void auto_fw_check(bool auto_fw_check) {
        auto_fw_check_ = auto_fw_check;
    }

    // Boolean Format API/MQTT
    uint8_t bool_format() {
        return bool_format_;
    }

    // Boolean Format Web
    uint8_t bool_dashboard() {
        return bool_dashboard_;
    }

    // see default_settings.h
    // BOOL_FORMAT_ONOFF_STR = 1,
    // BOOL_FORMAT_ONOFF_STR_CAP = 2
    // BOOL_FORMAT_TRUEFALSE_STR = 3
    // BOOL_FORMAT_TRUEFALSE = 4
    // BOOL_FORMAT_10_STR = 5
    // BOOL_FORMAT_10 = 6
    void bool_format(uint8_t format) {
        bool_format_ = format;
    }

    void bool_dashboard(uint8_t format) {
        bool_dashboard_ = format;
    }

    uint8_t enum_format() {
        return enum_format_;
    }

    void enum_format(uint8_t format) {
        enum_format_ = format;
    }

    std::string board_profile() {
        return std::string(board_profile_.c_str());
    }

    std::string hostname() {
        return hostname_;
    }

    void hostname(const std::string hostname) {
        hostname_ = hostname;
    }

    void ntp_connected(bool b);
    bool ntp_connected();

    void fahrenheit(bool b) {
        fahrenheit_ = b;
    }

    bool fahrenheit() {
        return fahrenheit_;
    }

    uint8_t language_index();

    void locale(String locale) {
        locale_ = locale;
    }

    std::string locale() {
        return std::string(locale_.c_str());
    }

    std::string system_name() {
        return std::string(system_name_.c_str());
    }

    void healthcheck(uint8_t healthcheck) {
        healthcheck_ = healthcheck;
    }

    void show_system(uuid::console::Shell & shell);
    void show_users(uuid::console::Shell & shell);

    static std::string languages_string();

    uint32_t FStotal() {
        return fstotal_;
    }

    void PSram(uint32_t psram) {
        psram_ = psram / 1024;
    }
    uint32_t PSram() {
        return psram_;
    }

    uint32_t appFree() {
        return appfree_;
    }
    uint32_t appUsed() {
        return appused_;
    }

    // memory in kb
    static uint32_t getMaxAllocMem() {
        return max_alloc_mem_;
    }
    static uint32_t getHeapMem() {
        return heap_mem_;
    }
    // All-time low watermark of free internal heap (KB).
    // Unlike getHeapMem() (sampled now), this captures the *lowest* free heap
    // has ever been since boot — i.e. the worst transient dip during MQTT
    // publishes, HA discovery, /api/system calls, TLS handshakes, etc.
    // This is the number that actually reflects optimisations targeting
    // transient JSON / buffer peaks (e.g. Phase C PSRAM JsonDocuments).
    static uint32_t getMinFreeMem() {
        return min_free_mem_;
    }
    static void refreshHeapMem() {
#ifndef EMSESP_STANDALONE
        max_alloc_mem_ = ESP.getMaxAllocHeap() / 1024;
        heap_mem_      = ESP.getFreeHeap() / 1024;
        min_free_mem_  = ESP.getMinFreeHeap() / 1024;
#endif
    }

    static bool test_set_all_active() {
        return test_set_all_active_;
    }
    static void test_set_all_active(bool n) {
#if defined(EMSESP_TEST)
        if (n) {
            logger_.debug("Using dummy entity values");
        }
#endif
        test_set_all_active_ = n;
    }

    static void set_valid_system_gpios();

#if CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32S2
    float temperature() {
        return temperature_;
    }
#endif

    static void remove_gpio(uint8_t pin, bool also_system = false); // remove a gpio from both valid (optional) and used lists
    static void remove_optional_gpio(uint8_t pin);
    static void reset_unused_gpios();

    // Partition info map: partition name -> {version, size, install_date}
    std::map<std::string, PartitionInfo, std::less<>, AllocatorPSRAM<std::pair<const std::string, PartitionInfo>>> partition_info_;

    static bool set_partition(const char * partitionname);

    // healthcheck flags - shared with LED for status visualization
    static constexpr uint8_t HEALTHCHECK_NO_BUS     = (1 << 0); // 1
    static constexpr uint8_t HEALTHCHECK_NO_NETWORK = (1 << 1); // 2
    static constexpr uint8_t HEALTHCHECK_RESET      = (1 << 7); // 128

    // smallest .bin accepted as a firmware image, for both URL and web uploads - 1.6 MB
    static constexpr size_t MIN_FIRMWARE_SIZE = 1677721;

  private:
    static uuid::log::Logger logger_;

    static bool     test_set_all_active_; // force all entities in a device to have a value
    static uint32_t max_alloc_mem_;
    static uint32_t heap_mem_;
    static uint32_t min_free_mem_;

    volatile uint8_t systemStatus_; // uses SYSTEM_STATUS enum - written from the AsyncTCP task (e.g. cancel) and read from the main loop during OTA

    void set_partition_install_date();

    // button
    static PButton            myPButton_; // PButton instance
    static void               button_OnClick(PButton & b);
    static void               button_OnDblClick(PButton & b);
    static void               button_OnLongPress(PButton & b);
    static void               button_OnVLongPress(PButton & b);
    static constexpr uint32_t BUTTON_Debounce      = 40;  // Debounce period to prevent flickering when pressing or releasing the button (in ms)
    static constexpr uint32_t BUTTON_DblClickDelay = 250; // Max period between clicks for a double click event (in ms)

    // button press delays
    static constexpr uint32_t BUTTON_LongPressDelay  = 3000; // Hold period for a long press event (in ms) - ~3 seconds
    static constexpr uint32_t BUTTON_VLongPressDelay = 9500; // Hold period for a very long press event (in ms) - !10 seconds

    // healthcheck
#ifdef EMSESP_PINGTEST
    static constexpr uint32_t SYSTEM_CHECK_FREQUENCY = 500; // do a system check every 1/2 second
#else
    static constexpr uint32_t SYSTEM_CHECK_FREQUENCY = 5000; // do a system check every 5 seconds
#endif

    // NTP syncs about every hour, so allow one missed sync before treating it as lost (in seconds) - 2 hours
    static constexpr uint32_t NTP_TIMEOUT_SEC = 7201;

    // email
    static constexpr uint32_t SMTP_TIMEOUT_MS = 5000; // socket read timeout (in ms) - 5 seconds

    // firmware upload from URL
    static constexpr uint32_t FIRMWARE_UPLOAD_RESPONSE_TIMEOUT = 8000;  // max wait for the first byte of the response headers/body (in ms) - 8 seconds
    static constexpr uint32_t FIRMWARE_UPLOAD_READ_TIMEOUT_S   = 15;    // socket and Stream::readBytes timeout used by Update (in seconds)
    static constexpr uint32_t FIRMWARE_UPLOAD_STALL_TIMEOUT    = 30000; // abort if no data arrives for a chunk (in ms) - 30 seconds
    static constexpr size_t   FIRMWARE_UPLOAD_CHUNK_SIZE       = 1024;  // read buffer, on the stack

#ifndef EMSESP_STANDALONE
    static uuid::syslog::SyslogService syslog_;
#endif

    void system_check();

    static std::vector<uint8_t, AllocatorPSRAM<uint8_t>> string_range_to_vector(const std::string & range, const std::string & exclude = "");

    // GPIOs
    struct GpioUsage {
        uint8_t     pin;
        std::string source;
    };
    static std::vector<uint8_t, AllocatorPSRAM<uint8_t>>     valid_system_gpios_; // list of valid GPIOs for the ESP32 board that can be used
    static std::vector<GpioUsage, AllocatorPSRAM<GpioUsage>> used_gpios_;         // list of GPIOs used by the application

    int8_t wifi_quality(int8_t dBm);

    uint8_t  healthcheck_       = HEALTHCHECK_NO_NETWORK | HEALTHCHECK_NO_BUS; // start with all flags set, no wifi and no ems bus connection
    uint32_t last_system_check_ = 0;

    bool upload_isrunning_ = false; // true if we're in the middle of a OTA firmware upload

    bool     ntp_connected_  = false;
    uint32_t ntp_last_check_ = 0;

    // EMS-ESP settings
    std::string hostname_;
    String      system_name_;
    String      locale_;
    bool        low_clock_;
    String      board_profile_;
    uint8_t     pbutton_gpio_;
    uint8_t     rx_gpio_;
    uint8_t     tx_gpio_;
    uint8_t     tx_mode_;
    bool        telnet_enabled_;
    bool        syslog_enabled_;
    int8_t      syslog_level_;
    uint32_t    syslog_mark_interval_;
    String      syslog_host_;
    uint16_t    syslog_port_;
    bool        fahrenheit_;
    uint8_t     bool_dashboard_;
    uint8_t     bool_format_;
    uint8_t     enum_format_;
    bool        readonly_mode_;
    bool        modbus_enabled_;
    uint16_t    modbus_port_;
    uint8_t     modbus_max_clients_;
    uint32_t    modbus_timeout_;
    bool        developer_mode_;
    bool        disable_reset_;
    bool        auto_fw_check_;
    uint32_t    fstotal_;
    uint32_t    psram_;
    uint32_t    appused_;
    uint32_t    appfree_;

#if CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32S2
    temperature_sensor_handle_t temperature_handle_ = NULL;
#endif
    float temperature_ = 0;
};

} // namespace emsesp

#endif
