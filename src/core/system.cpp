/*
 * EMS-ESP - https://github.com/emsesp/EMS-ESP
 * Copyright 2020-2025  emsesp.org
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

#include "system.h"
#include "network.h"
#include "emsesp.h" // for send_raw_telegram() command

#ifndef EMSESP_STANDALONE
#include "esp_image_format.h"
#include "esp_ota_ops.h"
#include "esp_partition.h"
#include <esp_mac.h>
#include "esp_efuse.h"
#include <nvs.h>
#endif

#include <map>

#include "firmwareVersion.h"
#include "shuntingYard.h" // for compute() used by the message and sendmail commands

#if defined(EMSESP_TEST)
#include "../test/test.h"
#endif

#ifndef EMSESP_STANDALONE
#define ENABLE_SMTP
#include <ESP_SSLClient.h>
#include <ReadyMail.h>
#endif

namespace emsesp {

// Languages supported. Note: the order is important
// and must match locale_translations.h and common.h
#if defined(EMSESP_TEST)
// in Test mode use two languages (en & de) to save flash memory needed for the tests
const char * const languages[] = {EMSESP_LOCALE_EN, EMSESP_LOCALE_DE};
#elif defined(EMSESP_EN_ONLY)
// EN only
const char * const languages[] = {EMSESP_LOCALE_EN};
#elif defined(EMSESP_DE_ONLY)
// EN + DE
const char * const languages[] = {EMSESP_LOCALE_EN, EMSESP_LOCALE_DE};
#else
const char * const languages[] = {EMSESP_LOCALE_EN,
                                  EMSESP_LOCALE_DE,
                                  EMSESP_LOCALE_NL,
                                  EMSESP_LOCALE_SV,
                                  EMSESP_LOCALE_PL,
                                  EMSESP_LOCALE_NO,
                                  EMSESP_LOCALE_FR,
                                  EMSESP_LOCALE_TR,
                                  EMSESP_LOCALE_IT,
                                  EMSESP_LOCALE_SK,
                                  EMSESP_LOCALE_CS};
#endif

static constexpr uint8_t NUM_LANGUAGES = sizeof(languages) / sizeof(const char *);

#ifndef EMSESP_STANDALONE
uuid::syslog::SyslogService System::syslog_;
#endif

uuid::log::Logger System::logger_{F_(system), uuid::log::Facility::KERN};

// init statics
PButton  System::myPButton_;
bool     System::test_set_all_active_ = false;
uint32_t System::max_alloc_mem_;
uint32_t System::heap_mem_;
uint32_t System::min_free_mem_;

// GPIOs
std::vector<uint8_t, AllocatorPSRAM<uint8_t>>                     System::valid_system_gpios_;
std::vector<System::GpioUsage, AllocatorPSRAM<System::GpioUsage>> System::used_gpios_;

#ifndef EMSESP_STANDALONE
// TLSHandshakeCallback is a C function pointer, so STARTTLS needs a process-wide client slot.
static ESP_SSLClient * sendmail_ssl_client = nullptr;

static void sendmail_tls_handshake(bool & success) {
    success = sendmail_ssl_client != nullptr && sendmail_ssl_client->connectSSL();
}

// The DNS slots are shared between IPv4 and IPv6, so with IPv6 enabled the first slot can hold an
// IPv6 server. Pick the first IPv4 one so it matches the label we print it under.
static IPAddress dns_ipv4(const NetworkInterface & netif) {
    for (uint8_t i = 0; i < ESP_NETIF_DNS_MAX; i++) {
        IPAddress ip = netif.dnsIP(i);
        if (ip.type() == IPv4 && static_cast<uint32_t>(ip) != 0) {
            return ip;
        }
    }
    return IPAddress();
}
#endif

// find the index of the language
// 0 = EN, 1 = DE, etc...
uint8_t System::language_index() {
    for (uint8_t i = 0; i < NUM_LANGUAGES; i++) {
        if (languages[i] == locale()) {
            return i;
        }
    }
    return 0; // EN only
}

// send raw to ems
bool System::command_send(const char * value, const int8_t id) {
    return EMSESP::txservice_.send_raw(value); // ignore id
}

// send email via SMTP
bool System::command_sendmail(const char * value, const int8_t) {
    bool     enabled = false;
    uint8_t  security;
    uint16_t port;
    String   server, login, pass, sender, recp, subject;
    EMSESP::webSettingsService.read([&](WebSettings & settings) {
        enabled  = settings.email_enabled;
        security = settings.email_security;
        server   = settings.email_server;
        port     = settings.email_port;
        login    = settings.email_login;
        pass     = settings.email_pass;
        sender   = settings.email_sender;
        recp     = settings.email_recp;
        subject  = settings.email_subject;
    });
    if (!enabled) {
        return false;
    }
    LOG_DEBUG("Command sendmail port %d%s called with '%s'",
              port,
              security == EMAIL_SECURITY::SSL        ? " (SSL)"
              : security == EMAIL_SECURITY::STARTTLS ? " (STARTTLS)"
                                                     : " (plain)",
              value);

    // Resolve the message before opening the SMTP session: compute() can do entity lookups and a
    // blocking {url} fetch, which would otherwise leave the connection idle long enough to time out.
    // The value is either a plain body or a JSON envelope overriding the configured subject/to/from.
    JsonDocument doc(PSRAM_DOC);
    String       body = value;
    if (body.length()) {
        auto error = deserializeJson(doc, (const char *)value);
        if (!error && doc.is<JsonObject>()) {
            subject = doc["subject"] | subject;
            recp    = doc["to"] | recp;
            sender  = doc["from"] | sender;
            body    = doc["body"] | body;
        }
    }
    // keep the original body if the calculator returns nothing, so unquoted literal text still gets sent
    std::string computed_body = compute(body.c_str());
    if (!computed_body.empty()) {
        body = computed_body.c_str();
    }

    bool success = false;

#ifndef EMSESP_STANDALONE
    // Do not use ReadyClient: after the 220 greeting it calls connectSSL() whenever
    // STARTTLS is off, so Security=Off still sends a TLS Client Hello on port 25.
    auto *          basic_client = new WiFiClient;
    ESP_SSLClient * ssl_client   = nullptr;
    SMTPClient *    smtp         = nullptr;

    basic_client->setTimeout(5000);

    const bool implicit_ssl = (security == EMAIL_SECURITY::SSL);
    const bool start_tls    = (security == EMAIL_SECURITY::STARTTLS);
    const bool tls_enabled  = (security != EMAIL_SECURITY::NONE);

    if (security == EMAIL_SECURITY::NONE) {
        smtp = new SMTPClient(*basic_client);
    } else {
        ssl_client = new ESP_SSLClient;
        ssl_client->setInsecure();
        ssl_client->setBufferSizes(16384, 1024);
        ssl_client->setTimeout(5);
        // enableSSL is baked in at setClient() — true only for implicit SSL (port 465)
        ssl_client->setClient(basic_client, implicit_ssl);
        if (start_tls) {
            sendmail_ssl_client = ssl_client;
            smtp                = new SMTPClient(*ssl_client, sendmail_tls_handshake, true);
        } else {
            smtp = new SMTPClient(*ssl_client);
        }
    }

    auto cleanup = [&]() {
        delete smtp;
        sendmail_ssl_client = nullptr;
        delete ssl_client;
        delete basic_client;
    };

    if (!smtp->connect(server, port, String(""), static_cast<SMTPResponseCallback>(nullptr), tls_enabled)) {
        LOG_ERROR("send email connection error: %s", smtp->status().text.c_str());
        cleanup();
        return false;
    }
    if (!smtp->isConnected()) {
        LOG_ERROR("send email connection error: %s", smtp->status().text.c_str());
        cleanup();
        return false;
    }

    // internal/open relays often have no AUTH; empty login means skip it
    if (!login.isEmpty()) {
        smtp->authenticate(login, pass, readymail_auth_password);
        if (!smtp->isAuthenticated()) {
            LOG_ERROR("send email authentication error: %s", smtp->status().text.c_str());
            cleanup();
            return false;
        }
    }
    SMTPMessage & msg = smtp->getMessage();
    msg.headers.add(rfc822_subject, subject);
    msg.headers.add(rfc822_from, sender);
    msg.headers.add(rfc822_to, recp);

    // Use addCustom to add custom header e.g. Importance and Priority.
    // msg.headers.addCustom("Importance", PRIORITY);
    // msg.headers.addCustom("X-MSMail-Priority", PRIORITY);
    // msg.headers.addCustom("X-Priority", PRIORITY_NUM);
    msg.text.body(body);

    // bodyText.replace("\r\n", "<br>\r\n");
    // msg.html.body("<html><body><div style=\"color:#cc0066;\">" + bodyText + "</div></body></html>");
    // msg.html.transferEncoding("base64");

    // With embedFile function, the html message will send as attachment.
    // if (EMBED_MESSAGE)
    //    msg.html.embedFile(true, "msg.html", embed_message_type_attachment);

    msg.timestamp = time(nullptr);

    success = smtp->send(msg);
    if (!success) {
        LOG_ERROR("send email failed: %s", smtp->status().text.c_str());
    }

    cleanup();
#endif
    return success;
}

// return string of languages and count
std::string System::languages_string() {
    std::string languages_string = std::to_string(NUM_LANGUAGES) + " languages (";
    for (uint8_t i = 0; i < NUM_LANGUAGES; i++) {
        languages_string += languages[i];
        if (i != NUM_LANGUAGES - 1) {
            languages_string += ",";
        }
    }
    languages_string += ")";
    return languages_string;
}

// returns last response from MQTT
bool System::command_response(const char * value, const int8_t id, JsonObject output) {
    JsonDocument doc;
    if (DeserializationError::Ok == deserializeJson(doc, Mqtt::get_response())) {
        for (JsonPair p : doc.as<JsonObject>()) {
            output[p.key()] = p.value();
        }
    } else {
        output["response"] = Mqtt::get_response();
    }
    return true;
}

// fetch device values
bool System::command_fetch(const char * value, const int8_t id) {
    std::string value_s;
    if (Helpers::value2string(value, value_s)) {
        if (value_s == "all") {
            LOG_INFO("Requesting data from EMS devices");
            EMSESP::fetch_device_values();
        } else if (value_s == F_(boiler)) {
            EMSESP::fetch_device_values_type(EMSdevice::DeviceType::BOILER);
        } else if (value_s == F_(thermostat)) {
            EMSESP::fetch_device_values_type(EMSdevice::DeviceType::THERMOSTAT);
        } else if (value_s == F_(solar)) {
            EMSESP::fetch_device_values_type(EMSdevice::DeviceType::SOLAR);
        } else if (value_s == F_(mixer)) {
            EMSESP::fetch_device_values_type(EMSdevice::DeviceType::MIXER);
        }
    } else {
        EMSESP::fetch_device_values(); // default if no name or id is given
    }

    return true; // always true
}

// mqtt publish
bool System::command_publish(const char * value, const int8_t id) {
    std::string value_s;
    if (Helpers::value2string(value, value_s)) {
        if (value_s == "ha") {
            EMSESP::publish_all(true); // includes HA
            LOG_INFO("Publishing all data to MQTT, including HA configs");
            return true;
        } else if (value_s == (F_(boiler))) {
            EMSESP::publish_device_values(EMSdevice::DeviceType::BOILER);
            return true;
        } else if (value_s == (F_(thermostat))) {
            EMSESP::publish_device_values(EMSdevice::DeviceType::THERMOSTAT);
            return true;
        } else if (value_s == (F_(solar))) {
            EMSESP::publish_device_values(EMSdevice::DeviceType::SOLAR);
            return true;
        } else if (value_s == (F_(mixer))) {
            EMSESP::publish_device_values(EMSdevice::DeviceType::MIXER);
            return true;
        } else if (value_s == (F_(water))) {
            EMSESP::publish_device_values(EMSdevice::DeviceType::WATER);
            return true;
        } else if (value_s == "other") {
            EMSESP::publish_other_values(); // switch and heat pump
            return true;
        } else if ((value_s == (F_(temperaturesensor))) || (value_s == (F_(analogsensor)))) {
            EMSESP::publish_sensor_values(true);
            return true;
        }
    }

    LOG_INFO("Publishing all data to MQTT");
    EMSESP::publish_all();

    return true;
}

// syslog level
// commenting this out - don't see the point on having an API service to change the syslog level
/*
bool System::command_syslog_level(const char * value, const int8_t id) {
    uint8_t s = 0xff;
    if (Helpers::value2enum(value, s, FL_(list_syslog_level))) {
        bool changed = false;
        EMSESP::webSettingsService.update(
            [&](WebSettings & settings) {
                if (settings.syslog_level != (int8_t)s - 1) {
                    settings.syslog_level = (int8_t)s - 1;
                    changed               = true;
                }
                return StateUpdateResult::CHANGED;
            });
        if (changed) {
            EMSESP::system_.syslog_init();
        }
        return true;
    }
    return false;
}
*/

// send message - to system log and MQTT
bool System::command_message(const char * value, const int8_t id, JsonObject output) {
    if (value == nullptr || value[0] == '\0') {
        LOG_WARNING("Message is empty");
        return false; // must have a string value
    }

    // process the message via the Shunting Yard calculator (entity substitution, expressions, optional {url} fetch)
    std::string computed_value = compute(value);
    if (computed_value.empty()) {
        LOG_WARNING("Message result is empty");
        return false;
    }

    LOG_INFO("Message: %s", computed_value.c_str());  // send to log
    Mqtt::queue_publish(F_(message), computed_value); // send to MQTT if enabled
    output["api_data"] = computed_value;              // send to API
    return true;
}

// watch
bool System::command_watch(const char * value, const int8_t id) {
    uint8_t  w = 0xff;
    uint16_t i = Helpers::hextoint(value);
    if (Helpers::value2enum(value, w, FL_(list_watch))) {
        if (w == 0 || EMSESP::watch() == EMSESP::Watch::WATCH_OFF) {
            EMSESP::watch_id(0);
        }
        if (Mqtt::publish_single() && w != EMSESP::watch()) {
            if (Mqtt::publish_single2cmd()) {
                Mqtt::queue_publish("system/watch", EMSESP::system_.enum_format() == ENUM_FORMAT_INDEX ? Helpers::itoa(w) : (FL_(list_watch)[w]));
            } else {
                Mqtt::queue_publish("system_data/watch", EMSESP::system_.enum_format() == ENUM_FORMAT_INDEX ? Helpers::itoa(w) : (FL_(list_watch)[w]));
            }
        }
        EMSESP::watch(w);
        return true;
    } else if (i) {
        if (Mqtt::publish_single() && i != EMSESP::watch_id()) {
            if (Mqtt::publish_single2cmd()) {
                Mqtt::queue_publish("system/watch", Helpers::hextoa(i));
            } else {
                Mqtt::queue_publish("system_data/watch", Helpers::hextoa(i));
            }
        }
        EMSESP::watch_id(i);
        if (EMSESP::watch() == EMSESP::Watch::WATCH_OFF) {
            EMSESP::watch(EMSESP::Watch::WATCH_ON);
        }
        return true;
    }
    return false;
}

void System::store_nvs_values() {
    if (Command::find_command(EMSdevice::DeviceType::BOILER, 0, "nompower", 0) != nullptr) {
        Command::call(EMSdevice::DeviceType::BOILER, "nompower", "-1"); // trigger a write
    }
    EMSESP::analogsensor_.store_counters();
    EMSESP::nvs_.end();
}

// Build up a list of all partitions and their version info
void System::get_partition_info() {
    partition_info_.clear(); // clear existing data

#ifdef EMSESP_STANDALONE
    // dummy data for standalone mode - version, size, install_date in UTC epoch
    partition_info_["app0"]    = {EMSESP_APP_VERSION, 0, 0};
    partition_info_["app1"]    = {"", 0, 0};
    partition_info_["factory"] = {"", 0, 0};
    partition_info_["boot"]    = {"", 0, 0};
#else

    auto current_partition = (const char *)esp_ota_get_running_partition()->label;

    // update the current version and partition name in NVS if not already set
    if (EMSESP::nvs_.getString(current_partition) != EMSESP_APP_VERSION || emsesp::EMSESP::nvs_.getBool(emsesp::EMSESP_NVS_BOOT_NEW_FIRMWARE, true)) {
        EMSESP::nvs_.putBool(emsesp::EMSESP_NVS_BOOT_NEW_FIRMWARE, false);
        EMSESP::nvs_.putString(current_partition, EMSESP_APP_VERSION);
        char c[20];
        snprintf(c, sizeof(c), "d_%s", current_partition);
        auto t = time(nullptr);
        // write timestamp always with new version, if clock is not set, this will be updated with ntp
        EMSESP::nvs_.putULong(c, t);
    }

    // Loop through all available partitions and update map with the version info pulled from NVS
    // Partitions can be app0, app1, factory, boot
    esp_partition_iterator_t it = esp_partition_find(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_ANY, nullptr);
    uint64_t                 buffer;

    while (it != nullptr) {
        bool                    is_valid = true;
        const esp_partition_t * part     = esp_partition_get(it);

        if (part->label != nullptr && part->label[0] != '\0') {
            // check if partition is valid and not empty
            esp_partition_read(part, 0, &buffer, 8);
            if (buffer == 0xFFFFFFFFFFFFFFFF) {
                is_valid = false; // skip this partition
            }
        }

        // get the version from the NVS store, and add to map
        if (is_valid) {
            PartitionInfo p_info;
            // if there is an entry for this partition in NVS, get it's version from NVS
            p_info.version = EMSESP::nvs_.getString(part->label, "").c_str();
            char c[20];
            snprintf(c, sizeof(c), "d_%s", (const char *)part->label);
            time_t d            = EMSESP::nvs_.getULong(c, 0);
            p_info.install_date = d > 1500000000L ? d : 0; // store UTC epoch; formatted to local time at render

            if (!p_info.version.empty()) {
                esp_image_metadata_t meta     = {};
                esp_partition_pos_t  part_pos = {.offset = part->address, .size = part->size};
                if (esp_image_verify(ESP_IMAGE_VERIFY_SILENT, &part_pos, &meta) == ESP_OK) {
                    p_info.size = meta.image_len / 1024; // actual firmware size in KB
                } else {
                    p_info.size = 0;
                }
                partition_info_[part->label] = p_info;
            }
        }

        it = esp_partition_next(it); // loop to next partition
    }
    esp_partition_iterator_release(it);
#endif
}

// set install time/date for the current partition, in UTC
// assumes NTP is connected and working
void System::set_partition_install_date() {
#ifndef EMSESP_STANDALONE
    auto current_partition = (const char *)esp_ota_get_running_partition()->label;
    if (current_partition == nullptr) {
        return; // fail-safe
    }

    char c[20];
    snprintf(c, sizeof(c), "d_%s", current_partition);
    time_t d = EMSESP::nvs_.getULong(c, 0);
    if (d < 1500000000L) {
        LOG_DEBUG("Setting the NTP install date in partition %s", current_partition);
        auto t = time(nullptr) - uuid::get_uptime_sec();
        EMSESP::nvs_.putULong(c, t);
    }
#endif
}

// sets the partition to use on the next restart
bool System::set_partition([[maybe_unused]] const char * partitionname) {
#ifdef EMSESP_STANDALONE
    return true;
#else
    if (partitionname == nullptr) {
        return false;
    }

    // Find the partition by label
    esp_partition_iterator_t it = esp_partition_find(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_ANY, partitionname);
    if (it == nullptr) {
        return false; // partition not found
    }

    const esp_partition_t * partition = esp_partition_get(it);
    esp_partition_iterator_release(it);

    if (partition == nullptr) {
        return false;
    }

    // Set the boot partition
    esp_err_t err = esp_ota_set_boot_partition(partition);
    if (err != ESP_OK) {
        return false;
    }

    // initiate the restart
    EMSESP::system_.systemStatus(SYSTEM_STATUS::SYSTEM_STATUS_RESTART_REQUESTED);
    return true;
#endif
}

// restart EMS-ESP
// app0 or app1, or boot/factory on 16MB boards
void System::system_restart(const char * partitionname) {
    // see if we are forcing a partition to use
    if (partitionname != nullptr) {
#ifndef EMSESP_STANDALONE
        // Factory partition - label will be "factory"
        const esp_partition_t * partition = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_FACTORY, NULL);
        if (partition && !strcmp(partition->label, partitionname)) {
            esp_ota_set_boot_partition(partition);
        } else
            // try and find the partition by name
            if (strcmp(esp_ota_get_running_partition()->label, partitionname)) {
                // not found, get next one in cycle
                partition = esp_ota_get_next_update_partition(nullptr);
                if (!partition) {
                    LOG_ERROR("Partition '%s' not found", partitionname);
                    return;
                }
                if (strcmp(partition->label, partitionname) && strcmp(partitionname, "boot") != 0) {
                    partition = esp_ota_get_next_update_partition(partition);
                    if (!partition || strcmp(partition->label, partitionname)) {
                        LOG_ERROR("Partition '%s' not found", partitionname);
                        return;
                    }
                }
                // error if partition is empty
                uint64_t buffer;
                esp_partition_read(partition, 0, &buffer, 8);
                if (buffer == 0xFFFFFFFFFFFFFFFF) {
                    LOG_ERROR("Partition '%s' is empty, not bootable", partition->label);
                    return;
                }
                // set the boot partition
                esp_ota_set_boot_partition(partition);
            }
#endif
        LOG_INFO("Restarting EMS-ESP from %s partition", partitionname);
    } else {
        LOG_INFO("Restarting EMS-ESP...");
    }

    store_nvs_values(); // save any NVS values

    // flush all the log
    EMSESP::webLogService.loop(); // dump all to web log
    for (int i = 0; i < 10; i++) {
        Shell::loop_all();
        delay(10); // give telnet TCP stack time to transmit
    }
    Serial.flush(); // wait for hardware TX buffer to drain

    Mqtt::disconnect(); // gracefully disconnect MQTT (flushes the DISCONNECT before reboot, needed for QOS1)
    EMSuart::stop();    // stop UART so there is no interference
#ifndef EMSESP_STANDALONE
    delay(1000);   // wait 1 second
    ESP.restart(); // ka-boom! - this is the only place where the ESP32 restart is called
#endif
}

void System::syslog_init() {
    EMSESP::webSettingsService.read([&](WebSettings & settings) {
        syslog_enabled_       = settings.syslog_enabled;
        syslog_level_         = settings.syslog_level;
        syslog_mark_interval_ = settings.syslog_mark_interval;
        syslog_host_          = settings.syslog_host;
        syslog_port_          = settings.syslog_port;
    });
#ifndef EMSESP_STANDALONE
    if (syslog_enabled_) {
        // start & configure syslog
        syslog_.maximum_log_messages(10);
        syslog_.log_level((uuid::log::Level)syslog_level_);
        syslog_.mark_interval(syslog_mark_interval_);
        syslog_.destination(syslog_host_.c_str(), syslog_port_);
        syslog_.hostname(hostname());
        EMSESP::logger().info("Starting Syslog service");
    } else if (syslog_.started()) {
        // in case service is still running, this flushes the queue
        // https://github.com/emsesp/EMS-ESP/issues/496
        EMSESP::logger().info("Stopping Syslog");
        syslog_.loop();
        syslog_.log_level(uuid::log::Level::OFF); // stop server
        syslog_.mark_interval(0);
        // syslog_.destination("");
    }
    if (Mqtt::publish_single()) {
        if (Mqtt::publish_single2cmd()) {
            Mqtt::queue_publish("system/syslog", syslog_enabled_ ? (FL_(list_syslog_level)[syslog_level_ + 1]) : "off");
            if (EMSESP::watch_id() == 0 || EMSESP::watch() == 0) {
                Mqtt::queue_publish("system/watch",
                                    EMSESP::system_.enum_format() == ENUM_FORMAT_INDEX ? Helpers::itoa(EMSESP::watch()) : (FL_(list_watch)[EMSESP::watch()]));
            } else {
                Mqtt::queue_publish("system/watch", Helpers::hextoa(EMSESP::watch_id()));
            }

        } else {
            Mqtt::queue_publish("system_data/syslog", syslog_enabled_ ? (FL_(list_syslog_level)[syslog_level_ + 1]) : "off");
            if (EMSESP::watch_id() == 0 || EMSESP::watch() == 0) {
                Mqtt::queue_publish("system_data/watch",
                                    EMSESP::system_.enum_format() == ENUM_FORMAT_INDEX ? Helpers::itoa(EMSESP::watch()) : (FL_(list_watch)[EMSESP::watch()]));
            } else {
                Mqtt::queue_publish("system_data/watch", Helpers::hextoa(EMSESP::watch_id()));
            }
        }
    }
#endif
}

// start or reconfigure modbus
void System::modbus_init() {
    EMSESP::webSettingsService.read([&](WebSettings & settings) {
        if (settings.modbus_enabled) {
            if (EMSESP::modbus_ == nullptr) {
                EMSESP::modbus_ = new Modbus;
                EMSESP::modbus_->start(1, settings.modbus_port, settings.modbus_max_clients, settings.modbus_timeout * 1000);
            } else if (settings.modbus_port != modbus_port_ || settings.modbus_max_clients != modbus_max_clients_ || settings.modbus_timeout != modbus_timeout_) {
                EMSESP::modbus_->stop();
                EMSESP::modbus_->start(1, settings.modbus_port, settings.modbus_max_clients, settings.modbus_timeout * 1000);
            }
        } else if (EMSESP::modbus_ != nullptr) {
            EMSESP::modbus_->stop();
            delete EMSESP::modbus_;
            EMSESP::modbus_ = nullptr;
        }
        modbus_enabled_     = settings.modbus_enabled;
        modbus_port_        = settings.modbus_port;
        modbus_max_clients_ = settings.modbus_max_clients;
        modbus_timeout_     = settings.modbus_timeout;
    });
}

// read specific major system settings to store locally for faster access
void System::store_settings(WebSettings & settings) {
    rx_gpio_      = settings.rx_gpio;
    tx_gpio_      = settings.tx_gpio;
    pbutton_gpio_ = settings.pbutton_gpio;

    low_clock_      = settings.low_clock;
    board_profile_  = settings.board_profile;
    telnet_enabled_ = settings.telnet_enabled;

    tx_mode_              = settings.tx_mode;
    syslog_enabled_       = settings.syslog_enabled;
    syslog_level_         = settings.syslog_level;
    syslog_mark_interval_ = settings.syslog_mark_interval;
    syslog_host_          = settings.syslog_host;
    syslog_port_          = settings.syslog_port;

    fahrenheit_     = settings.fahrenheit;
    bool_format_    = settings.bool_format;
    bool_dashboard_ = settings.bool_dashboard;
    enum_format_    = settings.enum_format;
    readonly_mode_  = settings.readonly_mode;
    locale_         = settings.locale;
    system_name_    = settings.system_name;
    developer_mode_ = settings.developer_mode;
    disable_reset_  = settings.disable_reset;
}

// Starts up core services
void System::start() {
    get_partition_info(); // get the partition info

#ifndef EMSESP_STANDALONE
    // disable bluetooth module
    // periph_module_disable(PERIPH_BT_MODULE);
    if (low_clock_) {
#if CONFIG_IDF_TARGET_ESP32C3
        setCpuFrequencyMhz(80);
#else
        setCpuFrequencyMhz(160);
#endif
    }

    // get current memory values
    fstotal_ = LittleFS.totalBytes() / 1024; // read only once, it takes 500 ms to read
    appused_ = ESP.getSketchSize() / 1024;
    appfree_ = esp_ota_get_running_partition()->size / 1024 - appused_;
    refreshHeapMem(); // refresh free heap and max alloc heap
#if CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32S2
    temperature_sensor_config_t temp_sensor_config = TEMPERATURE_SENSOR_CONFIG_DEFAULT(-10, 80);
    temperature_sensor_install(&temp_sensor_config, &temperature_handle_);
    temperature_sensor_enable(temperature_handle_);
    temperature_sensor_get_celsius(temperature_handle_, &temperature_);
#endif
#endif

    EMSESP::esp32React.getNetworkSettingsService()->read([&](NetworkSettings & networkSettings) {
        hostname(networkSettings.hostname.c_str()); // sets the hostname
    });

    commands_init();     // console & api commands
    EMSESP::led_.init(); // init LED
    button_init();       // button
    uart_init();         // start UART
    syslog_init();       // start syslog
    modbus_init();       // start modbus
}

// button single click - does nothing in normal operation
// in debug mode, it will trigger a special healthcheck to test the LED monitoring and sequence_led
void System::button_OnClick(PButton & b) {
    LOG_NOTICE("Button pressed - single click");

#ifdef EMSESP_DEBUG
#ifndef EMSESP_STANDALONE
    listDir("/", 3); // show filesystem
#endif
    // used to test LED monitoring and sequence_led. See system_check() for more details.
    EMSESP::system_.healthcheck(99); // 99 = special trigger
#endif
}

// button double click
// reconnect to AP by removing the SSID from the network settings
// note: in v3.9 this is normal behaviour to fallback to AP if the Wifi or Ethernet connection fails
void System::button_OnDblClick(PButton & b) {
    LOG_NOTICE("Button pressed - double click - reset network");
#ifndef EMSESP_STANDALONE
    // set AP mode to always so will join AP if wifi ssid fails to connect
    EMSESP::esp32React.getAPSettingsService()->update([&](APSettings & apSettings) {
        apSettings.provisionMode = AP_MODE_DISCONNECTED;
        return StateUpdateResult::CHANGED;
    });
    // remove SSID from network settings
    EMSESP::esp32React.getNetworkSettingsService()->update([&](NetworkSettings & networkSettings) {
        networkSettings.ssid = "";
        return StateUpdateResult::CHANGED;
    });
    EMSESP::network_.reconnect(); // reconnect to the network
#endif
}

// button long press
void System::button_OnLongPress(PButton & b) {
    LOG_NOTICE("Button pressed - long press - restart EMS-ESP");
    EMSESP::system_.system_restart("boot");
}

// button indefinite press
void System::button_OnVLongPress(PButton & b) {
    if (EMSESP::system_.disable_reset()) {
        LOG_NOTICE("Factory reset disabled");
        return;
    }
    LOG_NOTICE("Button pressed - very long press - perform factory reset");
    EMSESP::led_.start_led_fast_flash(5); // Start LED flash timer for 5 seconds
}

// push button
void System::button_init() {
#ifndef EMSESP_STANDALONE
    if (!myPButton_.init(pbutton_gpio_, HIGH)) {
        LOG_WARNING("Multi-functional button not detected");
        return;
    }
    LOG_DEBUG("Multi-functional button enabled");

    myPButton_.onClick(BUTTON_Debounce, button_OnClick);
    myPButton_.onDblClick(BUTTON_DblClickDelay, button_OnDblClick);
    myPButton_.onLongPress(BUTTON_LongPressDelay, button_OnLongPress);
    myPButton_.onVLongPress(BUTTON_VLongPressDelay, button_OnVLongPress);
#endif
}

// init UART
void System::uart_init() {
    EMSuart::stop();
    EMSuart::start(tx_mode_, rx_gpio_, tx_gpio_); // start UART, GPIOs have already been checked
    EMSESP::txservice_.start();                   // reset counters and send devices request
}

// checks system health and handles LED flashing wizardry
// returns true if the LED flash is active
bool System::loop() {
    // check if we're supposed to do a reset/restart
    if (systemStatus() == SYSTEM_STATUS::SYSTEM_STATUS_RESTART_REQUESTED) {
        system_restart();
    }

    myPButton_.check(); // check button press
    system_check();     // System health check

    // handle the LED
    if (EMSESP::led_.loop(healthcheck_, myPButton_.button_busy())) {
        return true; // restart is pending, skip the rest of the loop
    }

#ifndef EMSESP_STANDALONE
    // syslog service
    if (syslog_enabled_) {
        syslog_.loop();
    }
#endif

    send_info_mqtt();

    return false;
}

// send MQTT info topic appended with the version information as JSON, as a retained flag
// this is only done once when the connection is established
void System::send_info_mqtt() {
    static uint8_t _connection = 0;
    uint8_t        connection  = (EMSESP::network_.ethernet_connected() ? 1 : 0) + (EMSESP::network_.wifi_connected() ? 2 : 0) + (ntp_connected_ ? 4 : 0)
                                 + (EMSESP::network_.has_ipv6() ? 8 : 0);
    // check if connection status has changed
    if (!Mqtt::connected() || connection == _connection) {
        return;
    }
    _connection = connection;
    JsonDocument doc;

    doc["version"]    = EMSESP_APP_VERSION;
    doc["systemName"] = system_name_.isEmpty() ? "EMS-ESP" : system_name_;

    // if NTP is enabled send the boot_time in local time in ISO 8601 format (eg: 2022-11-15 20:46:38)
    // https://github.com/emsesp/EMS-ESP32/issues/751
    if (time(nullptr) > 1500000000L) {
        char   time_string[25];
        time_t now = time(nullptr) - uuid::get_uptime_sec();
        strftime(time_string, 25, "%FT%T%z", localtime(&now));
        doc["bootTime"] = time_string;
    }

#ifndef EMSESP_STANDALONE
    if (EMSESP::network_.ethernet_connected()) {
        doc["network"]  = "ethernet";
        doc["hostname"] = ETH.getHostname();
    } else if (EMSESP::network_.wifi_connected()) {
        doc["network"]         = "wifi";
        doc["hostname"]        = WiFi.getHostname();
        doc["SSID"]            = WiFi.SSID();
        doc["BSSID"]           = WiFi.BSSIDstr();
        doc["MAC"]             = WiFi.macAddress();
        doc["IPv4 address"]    = uuid::printable_to_string(WiFi.localIP()) + "/" + uuid::printable_to_string(WiFi.subnetMask());
        doc["IPv4 gateway"]    = uuid::printable_to_string(WiFi.gatewayIP());
        doc["IPv4 DNS Server"] = uuid::printable_to_string(dns_ipv4(WiFi.STA));
        if (WiFi.linkLocalIPv6().toString() != "0000:0000:0000:0000:0000:0000:0000:0000" && WiFi.linkLocalIPv6().toString() != "::") {
            doc["IPv6 address"] = uuid::printable_to_string(WiFi.linkLocalIPv6());
        }
    }
#endif
    Mqtt::queue_publish_retain(F_(info), doc.as<JsonObject>()); // topic called "info" and it's Retained
}

// create the json for heartbeat
void System::heartbeat_json(JsonObject output) {
    switch (EMSESP::bus_status()) {
    case EMSESP::BUS_STATUS_OFFLINE:
        output["bus_status"] = "connecting"; // EMS-ESP is booting...
        break;
    case EMSESP::BUS_STATUS_TX_ERRORS:
        output["bus_status"] = "txerror";
        break;
    case EMSESP::BUS_STATUS_CONNECTED:
        output["bus_status"] = "connected";
        break;
    default:
        output["bus_status"] = "disconnected";
        break;
    }

    output["uptime"]     = uuid::log::format_timestamp_ms(uuid::get_uptime_ms(), 3);
    output["uptime_sec"] = uuid::get_uptime_sec();

    output["rxreceived"] = EMSESP::rxservice_.telegram_count();
    output["rxfails"]    = EMSESP::rxservice_.telegram_error_count();
    output["txreads"]    = EMSESP::txservice_.telegram_read_count();
    output["txwrites"]   = EMSESP::txservice_.telegram_write_count();
    output["txfails"]    = EMSESP::txservice_.telegram_read_fail_count() + EMSESP::txservice_.telegram_write_fail_count();

    if (Mqtt::enabled()) {
        output["mqttcount"]      = Mqtt::publish_count();
        output["mqttfails"]      = Mqtt::publish_fails();
        output["mqttreconnects"] = Mqtt::connect_count();
    }
    output["apicalls"] = WebAPIService::api_count(); // + WebAPIService::api_fails();
    output["apifails"] = WebAPIService::api_fails();

    if (EMSESP::sensor_enabled() || EMSESP::analog_enabled()) {
        output["sensorreads"] = EMSESP::temperaturesensor_.reads() + EMSESP::analogsensor_.reads();
        output["sensorfails"] = EMSESP::temperaturesensor_.fails() + EMSESP::analogsensor_.fails();
    }

#ifndef EMSESP_STANDALONE
    output["freemem"]   = getHeapMem();
    output["max_alloc"] = getMaxAllocMem();
    // All-time low watermark of free internal heap (KB). Unlike freemem
    // (sampled now), this captures the worst transient dip since boot —
    // the actual metric to watch when measuring the effect of transient
    // allocation optimisations (e.g. JsonDocument on PSRAM).
    output["min_free"] = getMinFreeMem();
#endif
#if CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32S2
    output["temperature"] = (int)temperature_;
#endif

#ifndef EMSESP_STANDALONE
    if (!EMSESP::network_.ethernet_connected()) {
        int8_t rssi              = WiFi.RSSI();
        output["rssi"]           = rssi;
        output["wifistrength"]   = wifi_quality(rssi);
        output["wifireconnects"] = EMSESP::network_.getNetworkReconnects();
    }
#endif

    // see if there is a newer version available
    if (EMSESP::webStatusService.versions_cache_valid()) {
        output["upgradeable"] = EMSESP::webStatusService.current_upgradeable();
    }
}

// send periodic MQTT message with system information
void System::send_heartbeat() {
    refreshHeapMem(); // refresh free heap and max alloc heap

    JsonDocument doc;
    JsonObject   json = doc.to<JsonObject>();

    heartbeat_json(json);
    Mqtt::queue_publish(F_(heartbeat), json); // send to MQTT with retain off. This will add to MQTT queue.
}

// check health of system, done every 5 seconds
void System::system_check() {
    uint32_t current_uptime = uuid::get_uptime();
    if (!last_system_check_ || ((uint32_t)(current_uptime - last_system_check_) >= SYSTEM_CHECK_FREQUENCY)) {
        last_system_check_ = current_uptime;

#ifndef EMSESP_STANDALONE
#if CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32S2
        temperature_sensor_get_celsius(temperature_handle_, &temperature_);
#endif
#endif

#ifdef EMSESP_PINGTEST
        static uint64_t ping_count = 0;
        LOG_NOTICE("Ping test, #%d", ping_count++);
#endif

        if (healthcheck_ != 99) { // skip if we're testing
            // check if we have a valid network connection
            healthcheck_ = (healthcheck_ & ~HEALTHCHECK_NO_NETWORK) | (EMSESP::network_.network_connected() ? 0 : HEALTHCHECK_NO_NETWORK);

            // check if we have a bus connection
            healthcheck_ = (healthcheck_ & ~HEALTHCHECK_NO_BUS) | (EMSbus::bus_connected() ? 0 : HEALTHCHECK_NO_BUS);
        } else {
            LOG_DEBUG("Healthcheck: testing mode");
            healthcheck_ = 0; // make it all look healthy - this is temporary for one cycle
        }

        // see if the healthcheck state has changed, if so send out the new heartbeat
        static uint8_t last_healthcheck_ = 0;
        if (healthcheck_ != last_healthcheck_) {
            last_healthcheck_ = healthcheck_;
            EMSESP::system_.send_heartbeat();
        }
    }
}

// commands - takes static function pointers
// can be called via Console using 'call system <cmd>'
void System::commands_init() {
    // Command::reserve(200);

    Command::add(EMSdevice::DeviceType::SYSTEM, F_(read), MAKE_CF_CB(System::command_read), FL_(read_cmd), CommandFlag::ADMIN_ONLY);
    Command::add(EMSdevice::DeviceType::SYSTEM, F_(send), MAKE_CF_CB(System::command_send), FL_(send_cmd), CommandFlag::ADMIN_ONLY);
    Command::add(EMSdevice::DeviceType::SYSTEM, F_(fetch), MAKE_CF_CB(System::command_fetch), FL_(fetch_cmd), CommandFlag::ADMIN_ONLY);
    Command::add(EMSdevice::DeviceType::SYSTEM, F_(sendmail), MAKE_CF_CB(System::command_sendmail), FL_(sendmail_cmd), CommandFlag::ADMIN_ONLY);
    Command::add(EMSdevice::DeviceType::SYSTEM, F_(restart), MAKE_CF_CB(System::command_restart), FL_(restart_cmd), CommandFlag::ADMIN_ONLY);
    Command::add(EMSdevice::DeviceType::SYSTEM, F_(format), MAKE_CF_CB(System::command_format), FL_(format_cmd), CommandFlag::ADMIN_ONLY);
    Command::add(EMSdevice::DeviceType::SYSTEM, F_(txpause), MAKE_CF_CB(System::command_txpause), FL_(txpause_cmd), CommandFlag::ADMIN_ONLY);
    Command::add(EMSdevice::DeviceType::SYSTEM, F_(led), MAKE_CF_CB(System::command_led), FL_(led_cmd), CommandFlag::ADMIN_ONLY);
    Command::add(EMSdevice::DeviceType::SYSTEM, F_(watch), MAKE_CF_CB(System::command_watch), FL_(watch_cmd));
    Command::add_json(EMSdevice::DeviceType::SYSTEM, F_(message), System::command_message, FL_(message_cmd));
#if defined(EMSESP_TEST)
    Command::add(EMSdevice::DeviceType::SYSTEM, ("test"), MAKE_CF_CB(System::command_test), FL_(test_cmd));
#endif

    // these commands will return data in JSON format
    Command::add_json(EMSdevice::DeviceType::SYSTEM, F("response"), System::command_response, FL_(commands_response));

    // MQTT subscribe "ems-esp/system/#"
    Mqtt::subscribe(EMSdevice::DeviceType::SYSTEM, "system/#", nullptr); // use empty function callback
}

// Return the quality (Received Signal Strength Indicator) of the WiFi network as a %
//  High quality: 90% ~= -55dBm
//  Medium quality: 50% ~= -75dBm
//  Low quality: 30% ~= -85dBm
//  Unusable quality: 8% ~= -96dBm
int8_t System::wifi_quality(int8_t dBm) {
    if (dBm <= -100) {
        return 0;
    }

    if (dBm >= -50) {
        return 100;
    }
    return 2 * (dBm + 100);
}

// print users to console
void System::show_users(uuid::console::Shell & shell) {
    if (!shell.has_flags(CommandFlags::ADMIN)) {
        shell.printfln("Unauthorized. You need to be an admin to view users.");
        return;
    }

    shell.printfln("Users:");

#ifndef EMSESP_STANDALONE
    EMSESP::esp32React.getSecuritySettingsService()->read([&](SecuritySettings & securitySettings) {
        for (const User & user : securitySettings.users) {
            shell.printfln(" username: %s, password: %s, is_admin: %s", user.username.c_str(), user.password.c_str(), user.admin ? ("yes") : ("no"));
        }
    });
#endif

    shell.println();
}

// shell command 'show system'
void System::show_system(uuid::console::Shell & shell) {
    refreshHeapMem(); // refresh free heap and max alloc heap

    shell.println();
    shell.println("System:");
    shell.printfln(" Version: %s", EMSESP_APP_VERSION);
    shell.printfln(" System name: %s", system_name_.c_str());
#ifndef EMSESP_STANDALONE
    shell.printfln(" Platform: %s (%s)", EMSESP_PLATFORM, ESP.getChipModel());
    shell.printfln(" Model: %s", getBBQKeesGatewayDetails().c_str());
#endif
    shell.printfln(" Language: %s", locale().c_str());
    shell.printfln(" Board profile: %s", board_profile().c_str());
    shell.printfln(" Uptime: %s", uuid::log::format_timestamp_ms(uuid::get_uptime_ms(), 3).c_str());
#ifndef EMSESP_STANDALONE
    // https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/misc_system_api.html
    unsigned char mac_base[6] = {0};
    esp_efuse_mac_get_default(mac_base);
    esp_read_mac(mac_base, ESP_MAC_WIFI_STA);
    shell.printfln(" Base MAC Address: %02X:%02X:%02X:%02X:%02X:%02X", mac_base[0], mac_base[1], mac_base[2], mac_base[3], mac_base[4], mac_base[5]);

    shell.printfln(" SDK version: %s", ESP.getSdkVersion());
    shell.printfln(" CPU frequency: %lu MHz", ESP.getCpuFreqMHz());
#if CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32S2
    shell.printfln(" CPU temperature: %d °C", (int)temperature());
#endif
    // Free heap = current; Min free = all-time low watermark (lowest free
    // heap has ever been since boot). Min free is the actual metric that
    // reflects optimisations targeting transient peaks (publishes, /api/system,
    // TLS handshakes). If transient peaks are reduced, min_free goes up.
    shell.printfln(" Free heap/Max alloc/Min free: %lu KB / %lu KB / %lu KB", getHeapMem(), getMaxAllocMem(), getMinFreeMem());
#ifndef EMSESP_STANDALONE
    // Largest contiguous free block of *internal* SRAM. Network stack
    // (LwIP/mbedTLS/AsyncTCP) and JSON output allocations need this to be
    // healthy — total free heap can look fine while this collapses due to
    // fragmentation. Compare before and after a big API call or MQTT publish.
    shell.printfln(" Internal heap free/largest block: %u KB / %u KB",
                   heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT) / 1024,
                   heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT) / 1024);
#endif
    shell.printfln(" App used/free: %lu KB / %lu KB", appUsed(), appFree());
    uint32_t FSused = LittleFS.usedBytes() / 1024;
    shell.printfln(" FS used/free: %lu KB / %lu KB", FSused, FStotal() - FSused);
    shell.printfln(" Flash size: %lu KB", ESP.getFlashChipSize() / 1024);
    if (PSram()) {
#ifndef EMSESP_STANDALONE
        shell.printfln(" PSRAM size/free/largest block: %lu KB / %lu KB / %u KB",
                       PSram(),
                       ESP.getFreePsram() / 1024,
                       heap_caps_get_largest_free_block(MALLOC_CAP_SPIRAM) / 1024);
#else
        shell.printfln(" PSRAM size/free: %lu KB / %lu KB", PSram(), ESP.getFreePsram() / 1024);
#endif
    } else {
        shell.printfln(" PSRAM: not available");
    }
    // GPIOs
    shell.println(" GPIOs:");
    shell.printf("  allowed:");
    for (const auto & gpio : valid_system_gpios_) {
        shell.printf(" %d", gpio);
    }
    shell.printfln(" [total %d]", valid_system_gpios_.size());
    shell.printf("  in use:");
    auto sorted_gpios = used_gpios_;
    std::sort(sorted_gpios.begin(), sorted_gpios.end(), [](const GpioUsage & a, const GpioUsage & b) { return a.pin < b.pin; });
    for (const auto & gpio : sorted_gpios) {
        shell.printf(" %d(%s)", gpio.pin, gpio.source.c_str());
    }
    shell.printfln(" [total %d]", used_gpios_.size());
    auto available = available_gpios();
    shell.printf("  available:");
    for (const auto & gpio : available) {
        shell.printf(" %d", gpio);
    }
    shell.printfln(" [total %d]", available.size());
    // List all partitions and their version info
    shell.println(" Partitions:");
    for (const auto & partition : partition_info_) {
        if (partition.second.version.empty()) {
            continue; // no version, empty string
        }
        std::string installed;
        if (partition.second.install_date > 0) {
            char   time_string[25];
            time_t d = partition.second.install_date;
            strftime(time_string, sizeof(time_string), "%FT%T", localtime(&d));
            installed = std::string(", installed on ") + time_string;
        }
        shell.printfln("  %s: v%s (%d KB%s) %s",
                       partition.first.c_str(),
                       partition.second.version.c_str(),
                       partition.second.size,
                       installed.c_str(),
                       (strcmp(esp_ota_get_running_partition()->label, partition.first.c_str()) == 0) ? "** active **" : "");
    }

    shell.println();
    shell.println("Network:");
    switch (WiFi.status()) {
    case WL_IDLE_STATUS:
        shell.printfln(" WiFi Status: Idle");
        break;

    case WL_NO_SSID_AVAIL:
        shell.printfln(" WiFi Status: Network not found");
        break;

    case WL_SCAN_COMPLETED:
        shell.printfln(" WiFi Status: Network scan complete");
        break;

    case WL_CONNECTED:
        shell.printfln(" WiFi Status: Connected");
        shell.printfln(" SSID: %s", WiFi.SSID().c_str());
        shell.printfln(" BSSID: %s", WiFi.BSSIDstr().c_str());
        shell.printfln(" RSSI: %d dBm (%d %%)", WiFi.RSSI(), wifi_quality(WiFi.RSSI()));
        char result[10];
        shell.printfln(" TxPower: %s dBm", Helpers::render_value(result, (double)(WiFi.getTxPower() / 4), 1));
        shell.printfln(" MAC address: %s", WiFi.macAddress().c_str());
        shell.printfln(" Hostname: %s", WiFi.getHostname());
        shell.printfln(" IPv4 address: %s/%s", uuid::printable_to_string(WiFi.localIP()).c_str(), uuid::printable_to_string(WiFi.subnetMask()).c_str());
        shell.printfln(" IPv4 gateway: %s", uuid::printable_to_string(WiFi.gatewayIP()).c_str());
        shell.printfln(" IPv4 DNS Server: %s", uuid::printable_to_string(dns_ipv4(WiFi.STA)).c_str());
        if (WiFi.linkLocalIPv6().toString() != "0000:0000:0000:0000:0000:0000:0000:0000" && WiFi.linkLocalIPv6().toString() != "::") {
            shell.printfln(" IPv6 address: %s", uuid::printable_to_string(WiFi.linkLocalIPv6()).c_str());
        }
        break;

    case WL_CONNECT_FAILED:
        shell.printfln(" WiFi Network: Connection failed");
        break;

    case WL_CONNECTION_LOST:
        shell.printfln(" WiFi Network: Connection lost");
        break;

    case WL_DISCONNECTED:
        shell.printfln(" WiFi Network: Disconnected");
        break;

    // case WL_NO_SHIELD:
    default:
        shell.printfln(" WiFi MAC address: %s", WiFi.macAddress().c_str());
        shell.printfln(" WiFi Network: not connected");
        break;
    }

    // show Ethernet if connected
    if (EMSESP::network_.ethernet_connected()) {
        shell.println();
        shell.printfln(" Ethernet Status: connected");
        shell.printfln(" Ethernet MAC address: %s", ETH.macAddress().c_str());
        shell.printfln(" Hostname: %s", ETH.getHostname());
        shell.printfln(" IPv4 address: %s/%s", uuid::printable_to_string(ETH.localIP()).c_str(), uuid::printable_to_string(ETH.subnetMask()).c_str());
        shell.printfln(" IPv4 gateway: %s", uuid::printable_to_string(ETH.gatewayIP()).c_str());
        shell.printfln(" IPv4 DNS Server: %s", uuid::printable_to_string(dns_ipv4(ETH)).c_str());
        if (ETH.linkLocalIPv6().toString() != "0000:0000:0000:0000:0000:0000:0000:0000" && ETH.linkLocalIPv6().toString() != "::") {
            shell.printfln(" IPv6 address: %s", uuid::printable_to_string(ETH.linkLocalIPv6()).c_str());
        }
    }

    // show AP is connected
    if (EMSESP::network_.ap_connected()) {
        shell.printfln(" AP Status: connected");
    }

    shell.println();
    shell.println("Syslog:");
    if (!syslog_enabled_) {
        shell.printfln(" Syslog: disabled");
    } else {
        shell.printfln(" Syslog: %s", syslog_.started() ? "started" : "stopped");
        shell.print(" ");
        shell.printfln(F_(host_fmt), !syslog_host_.isEmpty() ? syslog_host_.c_str() : F_(unset));
        shell.printfln(" IP: %s", uuid::printable_to_string(syslog_.ip()).c_str());
        shell.print(" ");
        shell.printfln(F_(port_fmt), syslog_port_);
        shell.print(" ");
        shell.printfln(F_(log_level_fmt), uuid::log::format_level_lowercase(static_cast<uuid::log::Level>(syslog_level_)));
        shell.print(" ");
        shell.printfln(F_(mark_interval_fmt), syslog_mark_interval_);
        shell.printfln(" Queued: %d", syslog_.queued());
    }

    shell.println();
#endif
}

// see if there is a restore of an older settings file that needs to be applied
// note there can be only one file at a time
bool System::check_restore() {
    bool reboot_required = false; // true if we need to reboot

#ifndef EMSESP_STANDALONE
    File new_file = LittleFS.open(TEMP_FILENAME_PATH);
    if (new_file) {
        JsonDocument         jsonDocument(PSRAM_DOC);
        DeserializationError error = deserializeJson(jsonDocument, new_file);
        if (error == DeserializationError::Ok && jsonDocument.is<JsonObject>()) {
            JsonObject input = jsonDocument.as<JsonObject>();
            // see what type of file it is, either settings or customization. anything else is ignored
            std::string settings_type = input["type"];
            LOG_INFO("Restoring '%s' settings...", settings_type.c_str());

            // system backup, which is a consolidated json object with all the settings files
            if (settings_type == "systembackup") {
                reboot_required    = true;
                JsonArray sections = input["systembackup"].as<JsonArray>();
                for (JsonObject section : sections) {
                    std::string section_type = section["type"];
                    LOG_DEBUG("Restoring '%s' section...", section_type.c_str());
                    if (section_type == "settings") {
                        saveSettings(NETWORK_SETTINGS_FILE, section);
                        saveSettings(AP_SETTINGS_FILE, section);
                        saveSettings(MQTT_SETTINGS_FILE, section);
                        saveSettings(NTP_SETTINGS_FILE, section);
                        saveSettings(SECURITY_SETTINGS_FILE, section);

                        // next is application settings
                        // we need to set the EMS Bus ID to 0x49 if it's 0x0B and coming from a version which is < v3.9.0
                        std::string     settingsVersion = section["Settings"]["version"];
                        FirmwareVersion settings_version(settingsVersion);
                        if (settings_version < FirmwareVersion("3.9.0")) {
                            if (section["Settings"]["ems_bus_id"].is<int>()) {
                                int ems_bus_id = section["Settings"]["ems_bus_id"];
                                if (ems_bus_id == 0x0B) {
                                    // set to EMSESP_DEFAULT_EMS_BUS_ID
                                    section["Settings"]["ems_bus_id"] = EMSESP_DEFAULT_EMS_BUS_ID;
                                    LOG_INFO("Overriding EMS Bus ID to %02X (was %02X)", EMSESP_DEFAULT_EMS_BUS_ID, ems_bus_id);
                                }
                            }
                        }
                        // continue processing the rest of the sections
                        saveSettings(EMSESP_SETTINGS_FILE, section);
                    }
                    if (section_type == "commands") {
                        saveSettings(EMSESP_COMMANDS_FILE, section);
                    }
                    if (section_type == "schedule") {
                        saveSettings(EMSESP_SCHEDULER_FILE, section);
                    }
                    if (section_type == "customizations") {
                        saveSettings(EMSESP_CUSTOMIZATION_FILE, section);
                    }
                    if (section_type == "entities") {
                        saveSettings(EMSESP_CUSTOMENTITY_FILE, section);
                    }
                    if (section_type == "modules") {
                        saveSettings(EMSESP_MODULES_FILE, section);
                    }
                    if (section_type == "customSupport") {
                        // it's a custom support, extract json and write to /config/customSupport.json file
                        File customSupportFile = LittleFS.open(EMSESP_CUSTOMSUPPORT_FILE, "w");
                        if (customSupportFile) {
                            serializeJson(section, customSupportFile);
                            customSupportFile.close();
                            LOG_INFO("Custom support file updated");
                        } else {
                            LOG_ERROR("Failed to save custom support file");
                        }
                    }

                    if (section_type == "nvs") {
                        // Restore NVS values
                        JsonArray nvs_entries = section["nvs"].as<JsonArray>();
                        for (JsonObject entry : nvs_entries) {
                            std::string key  = entry["key"] | "";
                            int         type = entry["type"] | NVS_TYPE_ANY;

                            switch (type) {
                            case NVS_TYPE_I8:
                                if (entry["value"].is<JsonVariantConst>()) {
                                    int8_t v = entry["value"];
                                    EMSESP::nvs_.putChar(key.c_str(), v);
                                    LOG_DEBUG("Restored NVS value: %s = %d", key.c_str(), v);
                                }
                                break;
                            case NVS_TYPE_U8:
                                if (entry["value"].is<JsonVariantConst>()) {
                                    uint8_t v = entry["value"];
                                    EMSESP::nvs_.putUChar(key.c_str(), v);
                                    LOG_DEBUG("Restored NVS value: %s = %d", key.c_str(), v);
                                }
                                break;
                            case NVS_TYPE_I32:
                                if (entry["value"].is<JsonVariantConst>()) {
                                    int32_t v = entry["value"];
                                    EMSESP::nvs_.putInt(key.c_str(), v);
                                    LOG_DEBUG("Restored NVS value: %s = %d", key.c_str(), v);
                                }
                                break;
                            case NVS_TYPE_U32:
                                if (entry["value"].is<JsonVariantConst>()) {
                                    uint32_t v = entry["value"];
                                    EMSESP::nvs_.putUInt(key.c_str(), v);
                                    LOG_DEBUG("Restored NVS value: %s = %d", key.c_str(), v);
                                }
                                break;
                            case NVS_TYPE_I64:
                                if (entry["value"].is<JsonVariantConst>()) {
                                    int64_t v = entry["value"];
                                    EMSESP::nvs_.putLong64(key.c_str(), v);
                                    LOG_DEBUG("Restored NVS value: %s = %d", key.c_str(), v);
                                }
                                break;
                            case NVS_TYPE_U64:
                                if (entry["value"].is<JsonVariantConst>()) {
                                    uint64_t v = entry["value"];
                                    EMSESP::nvs_.putULong64(key.c_str(), v);
                                    LOG_DEBUG("Restored NVS value: %s = %d", key.c_str(), v);
                                }
                                break;
                            case NVS_TYPE_BLOB:
                                // used for double values
                                if (entry["value"].is<JsonVariantConst>()) {
                                    double v = entry["value"];
                                    EMSESP::nvs_.putDouble(key.c_str(), v);
                                    LOG_DEBUG("Restored NVS value: %s = %d", key.c_str(), v);
                                }
                                break;
                            case NVS_TYPE_STR:
                            case NVS_TYPE_ANY:
                            default:
                                if (entry["value"].is<JsonVariantConst>()) {
                                    std::string v = entry["value"];
                                    EMSESP::nvs_.putString(key.c_str(), v.c_str());
                                    LOG_DEBUG("Restored NVS value: %s = %s", key.c_str(), v.c_str());
                                }
                                break;
                            }
                        }
                    }
                }
            }

            // It's a single settings file. Parse each section separately. If it's system related it will require a reboot
            else if (settings_type == "settings") {
                reboot_required = saveSettings(NETWORK_SETTINGS_FILE, input);
                reboot_required |= saveSettings(AP_SETTINGS_FILE, input);
                reboot_required |= saveSettings(MQTT_SETTINGS_FILE, input);
                reboot_required |= saveSettings(NTP_SETTINGS_FILE, input);
                reboot_required |= saveSettings(SECURITY_SETTINGS_FILE, input);
                reboot_required |= saveSettings(EMSESP_SETTINGS_FILE, input);
            } else if (settings_type == "customizations") {
                saveSettings(EMSESP_CUSTOMIZATION_FILE, input);
            } else if (settings_type == "schedule") {
                saveSettings(EMSESP_SCHEDULER_FILE, input);
            } else if (settings_type == "entities") {
                saveSettings(EMSESP_CUSTOMENTITY_FILE, input);
            } else if (settings_type == "customSupport") {
                // it's a custom support file - save it to /config
                new_file.close();
                if (LittleFS.rename(TEMP_FILENAME_PATH, EMSESP_CUSTOMSUPPORT_FILE)) {
                    LOG_INFO("Custom support file stored");
                    return false; // no need to reboot
                } else {
                    LOG_ERROR("Failed to save custom support file");
                }
            } else {
                LOG_ERROR("Unrecognized file uploaded");
            }
        } else {
            LOG_ERROR("Unrecognized file uploaded, not json.");
        }

        // close (just in case) and remove the temp file
        new_file.close();
        LittleFS.remove(TEMP_FILENAME_PATH);
    }
#endif

    return reboot_required;
}

// handle upgrades from previous versions
// this function will not be called on a clean install, with no settings files yet created
// returns true if we need a reboot
bool System::check_upgrade() {
    bool        missing_version = true;
    std::string settingsVersion;

    // fetch current version from settings file
    EMSESP::webSettingsService.read([&](WebSettings const & settings) { settingsVersion = settings.version.c_str(); });

    // see if we're missing a version, will be < 3.5.0b13 from Dec 23 2022
    missing_version = (settingsVersion.empty() || (settingsVersion.length() < 5));
    if (missing_version) {
        LOG_WARNING("No version information found. Assuming version 3.5.0");
        settingsVersion = "3.5.0"; // this was the last stable version without version info
    }

    FirmwareVersion settings_version(settingsVersion);
    FirmwareVersion this_version(EMSESP_APP_VERSION);

    std::string settings_version_type = settings_version.prerelease().empty() ? "" : ("-" + settings_version.prerelease());
    std::string this_version_type     = this_version.prerelease().empty() ? "" : ("-" + this_version.prerelease());
    bool        save_version          = true;
    bool        reboot_required       = false;

    LOG_DEBUG("Checking for version upgrades from v%d.%d.%d%s",
              settings_version.major(),
              settings_version.minor(),
              settings_version.patch(),
              settings_version_type.c_str());

    // compare versions
    if (this_version > settings_version) {
        // we need to do an upgrade
        if (missing_version) {
            LOG_NOTICE("Upgrading to version %d.%d.%d%s", this_version.major(), this_version.minor(), this_version.patch(), this_version_type.c_str());
        } else {
            LOG_NOTICE("Upgrading from version %d.%d.%d%s to %d.%d.%d%s",
                       settings_version.major(),
                       settings_version.minor(),
                       settings_version.patch(),
                       settings_version_type.c_str(),
                       this_version.major(),
                       this_version.minor(),
                       this_version.patch(),
                       this_version_type.c_str());
        }

        // if we're coming from 3.4.4 or 3.5.0b14 which had no version stored then we need to apply new settings
        if (missing_version) {
            LOG_INFO("Upgrade: Setting MQTT Entity ID format to older v3.4 format (0)");
            EMSESP::esp32React.getMqttSettingsService()->update([&](MqttSettings & mqttSettings) {
                mqttSettings.entity_format = Mqtt::entityFormat::SINGLE_LONG; // use old Entity ID format from v3.4
                return StateUpdateResult::CHANGED;
            });
        } else if (settings_version.major() == 3 && settings_version.minor() <= 6) {
            EMSESP::esp32React.getMqttSettingsService()->update([&](MqttSettings & mqttSettings) {
                if (mqttSettings.entity_format == 1) {
                    mqttSettings.entity_format = Mqtt::entityFormat::SINGLE_OLD; // use old Entity ID format from v3.6
                    LOG_INFO("Upgrade: Setting MQTT Entity ID format to v3.6 format (3)");
                    return StateUpdateResult::CHANGED;
                } else if (mqttSettings.entity_format == 2) {
                    mqttSettings.entity_format = Mqtt::entityFormat::MULTI_OLD; // use old Entity ID format from v3.6
                    LOG_INFO("Upgrade: Setting MQTT Entity ID format to v3.6 format (4)");
                    return StateUpdateResult::CHANGED;
                }
                return StateUpdateResult::UNCHANGED;
            });
        }

        // changes pre < v3.7.0
        if (settings_version.major() == 3 && settings_version.minor() < 7) {
            // network changes
            // 1) WiFi Tx Power is now using the value * 4 (was 20)
            // 2) WiFi sleep is now off by default (was on)
            EMSESP::esp32React.getNetworkSettingsService()->update([&](NetworkSettings & networkSettings) {
                auto changed = StateUpdateResult::UNCHANGED;
                if (networkSettings.tx_power == 20) {
                    networkSettings.tx_power = WIFI_POWER_19_5dBm; // use 19.5 as we don't have 20 anymore
                    LOG_INFO("Upgrade: Setting WiFi TX Power to Auto");
                    changed = StateUpdateResult::CHANGED;
                }
                if (networkSettings.nosleep != true) {
                    networkSettings.nosleep = true;
                    LOG_INFO("Upgrade: Disabling WiFi nosleep");
                    changed = StateUpdateResult::CHANGED;
                }
                return changed;
            });
            EMSESP::network_.reconnect();
        }

        // capture the raw Scheduler file now, before any upgrade step below rewrites it in the new format.
        // it's needed further down to migrate the pre-v3.9.0-dev.12 inline command format into the Commands Service
#ifndef EMSESP_STANDALONE
        JsonDocument oldScheduleDoc(PSRAM_DOC);
        {
            File schedulerFile = LittleFS.open(EMSESP_SCHEDULER_FILE);
            if (schedulerFile) {
                deserializeJson(oldScheduleDoc, schedulerFile);
                schedulerFile.close();
            }
        }
#endif

        // changes going to v3.9 from an 3.8.x or earlier
        if (settings_version.major() == 3 && settings_version.minor() < 9) {
#ifndef EMSESP_STANDALONE
            // AP_MODE_ALWAYS has been removed
            EMSESP::esp32React.getAPSettingsService()->update([&](APSettings & apSettings) {
                if (apSettings.provisionMode == 0) {
                    apSettings.provisionMode = AP_MODE_DISCONNECTED; // AP_MODE_DISCONNECTED is the new default
                    LOG_INFO("Upgrade: Setting AP provision mode to on disconnect");
                    return StateUpdateResult::CHANGED;
                }
                return StateUpdateResult::UNCHANGED;
            });
            // Scheduler name is now mandatory, update FS if name is empty
            uint8_t i                = 0;
            bool    schedule_changed = false;
            EMSESP::webSchedulerService.update([&](WebScheduler & scheduler) {
                for (ScheduleItem & scheduleItem : scheduler.scheduleItems) {
                    if (scheduleItem.name[0] == '\0') {
                        snprintf(scheduleItem.name, sizeof(scheduleItem.name), "schedule_%d", i++);
                        schedule_changed = true;
                    }
                }
                return schedule_changed ? StateUpdateResult::CHANGED : StateUpdateResult::UNCHANGED;
            });
#endif
        }

        // Core3 3.9.0-dev.12 implements the new Commands Service.
        // versions before that stored the command (cmd) and value inline within each Scheduler entry
#ifndef EMSESP_STANDALONE
        {
            JsonArray oldScheduleItems = oldScheduleDoc["schedule"].as<JsonArray>();

            // only migrate if at least one entry still uses the old inline format (has "cmd" but no "cmd_name")
            bool old_format = false;
            for (JsonObject item : oldScheduleItems) {
                if (!item["cmd"].isNull() && item["cmd_name"].isNull()) {
                    old_format = true;
                    break;
                }
            }

            if (old_format) {
                LOG_INFO("Upgrade: Migrating %d Scheduler entries to the new Commands Service", (int)oldScheduleItems.size());

                // create a Command for each Scheduler entry, reusing the entry's name (generating one if empty)
                EMSESP::webCommandService.update([&](WebCommands & commands) {
                    commands.commandItems.clear();
                    uint8_t idx = 0;
                    for (JsonObject item : oldScheduleItems) {
                        auto ci         = CommandItem();
                        ci.cmd          = item["cmd"].as<std::string>();
                        ci.value        = item["value"].as<std::string>();
                        const char * nm = item["name"];
                        // name could still be empty
                        if (nm != nullptr && nm[0] != '\0') {
                            strlcpy(ci.name, nm, sizeof(ci.name));
                        } else {
                            snprintf(ci.name, sizeof(ci.name), "schedule_%d", idx);
                        }
                        commands.commandItems.push_back(ci);
                        idx++;
                    }
                    return StateUpdateResult::CHANGED;
                });

                // point each Scheduler entry at its new Command via cmd_name
                EMSESP::webSchedulerService.update([&](WebScheduler & scheduler) {
                    uint8_t idx = 0;
                    auto    it  = scheduler.scheduleItems.begin();
                    for (JsonObject item : oldScheduleItems) {
                        if (it == scheduler.scheduleItems.end()) {
                            break;
                        }
                        // flag 132 (0x84) is the old IMMEDIATE format which has no command - erase the entry
                        if (item["flags"].as<uint8_t>() == 0x84) {
                            it = scheduler.scheduleItems.erase(it);
                            idx++;
                            continue;
                        }
                        const char * nm = item["name"];
                        char         cmd_name[sizeof(it->name)];
                        if (nm != nullptr && nm[0] != '\0') {
                            strlcpy(cmd_name, nm, sizeof(cmd_name));
                        } else {
                            snprintf(cmd_name, sizeof(cmd_name), "schedule_%d", idx);
                            strlcpy(it->name, cmd_name, sizeof(it->name)); // keep entry name consistent with its command
                        }
                        it->cmd_name = cmd_name;
                        ++it;
                        idx++;
                    }
                    return StateUpdateResult::CHANGED;
                });

                // reboot so both services reload cleanly in the new format and re-register their commands
                reboot_required = true;
            }
        }
#endif

        // changes to application settings
        EMSESP::webSettingsService.update([&](WebSettings & settings) {
            // force web buffer to 25 for those boards without psram
            if ((EMSESP::system_.PSram() == 0) && (settings.weblog_buffer != 25)) {
                settings.weblog_buffer = 25;
                // if we're coming from < v3.9.0 and the Bus ID is the service key (0x0B), set it to the new default
                if (settings.ems_bus_id == 0x0B && settings_version.major() <= 3 && settings_version.minor() < 9) {
                    settings.ems_bus_id = EMSESP_DEFAULT_EMS_BUS_ID;
                    return StateUpdateResult::CHANGED;
                }
            }
            // Migrate language from cz to cs
            if (settings.locale == "cz") {
                settings.locale = "cs";
                return StateUpdateResult::CHANGED;
            }
            return StateUpdateResult::UNCHANGED;
        });
    } else if (this_version < settings_version) {
        // downgrading
        LOG_NOTICE("Downgrading from version %d.%d.%d%s to version %d.%d.%d%s",
                   settings_version.major(),
                   settings_version.minor(),
                   settings_version.patch(),
                   settings_version_type.c_str(),
                   this_version.major(),
                   this_version.minor(),
                   this_version.patch(),
                   this_version_type.c_str());
    } else {
        save_version = false; // same version, do nothing
    }

    // if we did a change, set the new version and save it, no need to reboot
    if (save_version) {
        EMSESP::webSettingsService.update([&](WebSettings & settings) {
            settings.version = EMSESP_APP_VERSION;
            LOG_DEBUG("Upgrade: Setting version to %s", EMSESP_APP_VERSION);
            return StateUpdateResult::CHANGED;
        });
    }

    if (reboot_required) {
        LOG_INFO("Upgrade: Rebooting to apply changes");
        return true; // need reboot
    }

    return false; // no reboot required
}

#ifndef EMSESP_STANDALONE
// map each config filename to its human-readable section key
static const std::pair<const char *, const char *> SECTION_MAP[] = {
    {NETWORK_SETTINGS_FILE, "Network"},
    {AP_SETTINGS_FILE, "AP"},
    {MQTT_SETTINGS_FILE, "MQTT"},
    {NTP_SETTINGS_FILE, "NTP"},
    {SECURITY_SETTINGS_FILE, "Security"},
    {EMSESP_SETTINGS_FILE, "Settings"},
    {EMSESP_COMMANDS_FILE, "Commands"},
    {EMSESP_SCHEDULER_FILE, "Schedule"},
    {EMSESP_CUSTOMIZATION_FILE, "Customizations"},
    {EMSESP_CUSTOMENTITY_FILE, "Entities"},
    {EMSESP_MODULES_FILE, "Modules"},
};
#endif

// convert a single config file into a section of the output json object
void System::exportSettings(const std::string & type, [[maybe_unused]] const char * filename, JsonObject output) {
    if (type != "settings") {
        output["type"] = type; // add the type to the output, not for settings as it's already added because its grouped
    }

#ifndef EMSESP_STANDALONE
    const char * section = nullptr;
    for (const auto & [f, label] : SECTION_MAP) {
        if (strcmp(f, filename) == 0) {
            section = label;
            break;
        }
    }

    if (!section) {
        return;
    }

    File settingsFile = LittleFS.open(filename);
    if (settingsFile) {
        {
            JsonDocument         jsonDocument(PSRAM_DOC);
            DeserializationError error = deserializeJson(jsonDocument, settingsFile);
            settingsFile.close(); // close early, we no longer need the file
            if (error || !jsonDocument.is<JsonObject>()) {
                LOG_ERROR("Failed to deserialize settings file %s", filename);
                return;
            }
            output[section].set(jsonDocument.as<JsonObject>());
        }
        LOG_DEBUG("Exported %s settings from file %s", section, filename);
    } else {
        LOG_ERROR("No settings file for %s found", filename);
    }
#endif
}

// full system backup of all settings files
void System::exportSystemBackup(JsonObject output) {
    output["type"]    = "systembackup";     // add the type to the output
    output["version"] = EMSESP_APP_VERSION; // add the version to the output

#ifndef EMSESP_STANDALONE
    // add date/time if NTP enabled and active
    if ((esp_sntp_enabled()) && (EMSESP::system_.ntp_connected())) {
        time_t now = time(nullptr);
        if (now > 1500000000L) {
            char t[25];
            strftime(t, sizeof(t), "%FT%T", localtime(&now));
            output["date"] = t;
        }
    }
#endif

    // create an array of objects for each settings file
    JsonArray nodes = output["systembackup"].to<JsonArray>();

    // start with settings by grouping them together
    JsonObject node = nodes.add<JsonObject>();
    node["type"]    = "settings"; // add type once for this group
    exportSettings("settings", NETWORK_SETTINGS_FILE, node);
    exportSettings("settings", AP_SETTINGS_FILE, node);
    exportSettings("settings", MQTT_SETTINGS_FILE, node);
    exportSettings("settings", NTP_SETTINGS_FILE, node);
    exportSettings("settings", SECURITY_SETTINGS_FILE, node);
    exportSettings("settings", EMSESP_SETTINGS_FILE, node);

    node = nodes.add<JsonObject>();
    exportSettings("commands", EMSESP_COMMANDS_FILE, node);
    node = nodes.add<JsonObject>();
    exportSettings("schedule", EMSESP_SCHEDULER_FILE, node);
    node = nodes.add<JsonObject>();
    exportSettings("customizations", EMSESP_CUSTOMIZATION_FILE, node);
    node = nodes.add<JsonObject>();
    exportSettings("entities", EMSESP_CUSTOMENTITY_FILE, node);
    node = nodes.add<JsonObject>();
    exportSettings("modules", EMSESP_MODULES_FILE, node);

#ifndef EMSESP_STANDALONE
    // special case for custom support
    File file = LittleFS.open(EMSESP_CUSTOMSUPPORT_FILE, "r");
    if (file) {
        JsonDocument         jsonDocument(PSRAM_DOC);
        DeserializationError error = deserializeJson(jsonDocument, file);
        file.close(); // close early, we no longer need the file
        if (!error && jsonDocument.is<JsonObject>()) {
            JsonObject support_node = nodes.add<JsonObject>();
            support_node["type"]    = "customSupport";
            support_node["data"].set(jsonDocument.as<JsonObject>());
            LOG_DEBUG("Exported custom support file %s", EMSESP_CUSTOMSUPPORT_FILE);
        } else {
            LOG_ERROR("Failed to deserialize custom support file");
        }
    }

    // Backup NVS values
    node         = nodes.add<JsonObject>();
    node["type"] = "nvs";

    const char *   nvs_part = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_NVS, "nvs1") ? "nvs1" : "nvs"; // nvs1 is on 16MBs
    nvs_iterator_t it       = nullptr;
    esp_err_t      err      = nvs_entry_find(nvs_part, "ems-esp", NVS_TYPE_ANY, &it);
    if (err != ESP_OK) {
        LOG_ERROR("Failed to find NVS entry for %s", nvs_part);
        return;
    }

    JsonArray entries = node["nvs"].to<JsonArray>();
    while (err == ESP_OK) {
        nvs_entry_info_t info;
        nvs_entry_info(it, &info);
        JsonObject entry = entries.add<JsonObject>();
        entry["type"]    = info.type;
        entry["key"]     = info.key;

        LOG_DEBUG("Exporting NVS value: %s = %d", info.key, info.type);

        switch (info.type) {
        case NVS_TYPE_I8:
            entry["value"] = EMSESP::nvs_.getChar(info.key);
            break;
        case NVS_TYPE_U8:
            entry["value"] = EMSESP::nvs_.getUChar(info.key);
            break;
        case NVS_TYPE_I32:
            entry["value"] = EMSESP::nvs_.getInt(info.key);
            break;
        case NVS_TYPE_U32:
            entry["value"] = EMSESP::nvs_.getUInt(info.key);
            break;
        case NVS_TYPE_I64:
            entry["value"] = EMSESP::nvs_.getLong64(info.key);
            break;
        case NVS_TYPE_U64:
            entry["value"] = EMSESP::nvs_.getULong64(info.key);
            break;
        case NVS_TYPE_BLOB:
            entry["value"] = EMSESP::nvs_.getDouble(info.key); // bytes used for double values
            break;
        case NVS_TYPE_STR:
        case NVS_TYPE_ANY:
        default:
            entry["value"] = EMSESP::nvs_.getString(info.key);
            break;
        }
        err = nvs_entry_next(&it);
    }

    if (it != nullptr) {
        nvs_release_iterator(it);
    }
#endif
}

// write a settings file using input from a json object, called from upload/restore
bool System::saveSettings([[maybe_unused]] const char * filename, [[maybe_unused]] JsonObject input) {
#ifndef EMSESP_STANDALONE
    const char * section = nullptr;
    for (const auto & [f, label] : SECTION_MAP) {
        if (strcmp(f, filename) == 0) {
            section = label;
            break;
        }
    }

    if (!section) {
        return false;
    }

    JsonObject section_json = input[section];
    if (section_json) {
        File section_file = LittleFS.open(filename, "w");
        if (section_file) {
            LOG_DEBUG("Applying new uploaded %s data", section);
            serializeJson(section_json, section_file);
            section_file.close();
            return true; // reboot required
        }
    }
#endif

    return false; // not found
}

// set a entity of services 'network', 'settings', 'mqtt', etc.
bool System::command_service(const char * cmd, const char * value) {
    bool ok = false;
    bool b;
    if (Helpers::value2bool(value, b)) {
        if (!strcmp(cmd, "settings/showertimer")) {
            EMSESP::webSettingsService.update([&](WebSettings & settings) {
                settings.shower_timer = b;
                return StateUpdateResult::CHANGED;
            });
            EMSESP::shower_.shower_timer(b);
            ok = true;
        } else if (!strcmp(cmd, "settings/showeralert")) {
            EMSESP::webSettingsService.update([&](WebSettings & settings) {
                settings.shower_alert = b;
                return StateUpdateResult::CHANGED;
            });
            EMSESP::shower_.shower_alert(b);
            ok = true;
        } else if (!strcmp(cmd, "settings/hideled")) {
            EMSESP::webSettingsService.update([&](WebSettings & settings) {
                settings.hide_led = b;
                return StateUpdateResult::CHANGED;
            });
            ok = true;
        } else if (!strcmp(cmd, "settings/analogenabled")) {
            EMSESP::webSettingsService.update([&](WebSettings & settings) {
                settings.analog_enabled = b;
                return StateUpdateResult::CHANGED;
            });
            ok = true;
        } else if (!strcmp(cmd, "mqtt/enabled")) {
            EMSESP::esp32React.getMqttSettingsService()->update([&](MqttSettings & Settings) {
                Settings.enabled = b;
                return StateUpdateResult::CHANGED;
            });
            ok = true;
        } else if (!strcmp(cmd, "ap/enabled")) {
            EMSESP::esp32React.getAPSettingsService()->update([&](APSettings & Settings) {
                Settings.provisionMode = b ? 0 : 2;
                return StateUpdateResult::CHANGED;
            });
            ok = true;
        } else if (!strcmp(cmd, "ntp/enabled")) {
            EMSESP::esp32React.getNTPSettingsService()->update([&](NTPSettings & Settings) {
                Settings.enabled = b;
                return StateUpdateResult::CHANGED;
            });
            ok = true;
        } else if (!strcmp(cmd, "syslog/enabled")) {
            EMSESP::webSettingsService.update([&](WebSettings & settings) {
                settings.syslog_enabled = b;
                return StateUpdateResult::CHANGED;
            });
            EMSESP::system_.syslog_enabled_ = b;
            EMSESP::system_.syslog_init();
            ok = true;
        }
    }

    if (ok) {
        LOG_INFO("System command '%s' with value '%s'", cmd, value);
    }
    return ok;
}

// return back a system value
bool System::get_value_info(JsonObject output, const char * cmd) {
    if (cmd == nullptr || strlen(cmd) == 0) {
        LOG_ERROR("empty system command");
        return false;
    }
    if (!strcmp(cmd, "restart")) { // restart is a command, not an entity
        return false;
    }
    // check for hardcoded "info"/"value"
    if (!strcmp(cmd, F_(info)) || !strcmp(cmd, F_(values))) {
        return command_info("", 0, output);
    }

    // check for metrics
    if (!strcmp(cmd, F_(metrics))) {
        std::string metrics = get_metrics_prometheus();
        if (!metrics.empty()) {
            output["api_data"] = metrics;
            return true;
        }
        return false;
    }

    // fetch all the data from the system in a different json
    JsonDocument doc(PSRAM_DOC);
    JsonObject   root = doc.to<JsonObject>();
    (void)command_info("", 0, root);

    // list all entities
    if (!strcmp(cmd, F_(entities))) {
        for (JsonPair p : root) {
            if (p.value().is<JsonObject>()) {
                const char * p_key = p.key().c_str(); // Cache the key
                for (JsonPair p1 : p.value().as<JsonObject>()) {
                    const char * p1_key = p1.key().c_str(); // Cache the key
                    JsonObject   entity = output[std::string(p_key) + "." + p1_key].to<JsonObject>();
                    get_value_json(entity, p_key, p1_key, p1.value());
                }
            }
        }
        return true;
    }

    char * val = (char *)strstr(cmd, "/value");
    if (val) {
        *val = '\0';
    }

    char * slash = (char *)strchr(cmd, '/');
    if (slash) {
        *slash = '\0';
        slash++;
    }

    // list values for a jsonObject in system, e.g. /api/system/network
    if (!slash || !strcmp(slash, F_(info)) || !strcmp(slash, F_(values))) {
        for (JsonPair p : root) {
            if (Helpers::toLower(p.key().c_str()) == cmd && p.value().is<JsonObject>()) {
                for (JsonPair p1 : p.value().as<JsonObject>()) {
                    output[p1.key().c_str()] = p1.value().as<std::string>();
                }
                return true;
            }
        }
        return false;
    }

    // value info or api_data for a single value
    // Loop through all the key-value pairs in root to find the key, case independent
    if (slash) { // search the top level first
        for (JsonPair p : root) {
            const char * p_key = p.key().c_str(); // Cache the key
            if (p.value().is<JsonObject>() && Helpers::toLower(p_key) == cmd) {
                for (JsonPair p1 : p.value().as<JsonObject>()) {
                    const char * p1_key = p1.key().c_str(); // Cache the key
                    if (Helpers::toLower(p1_key) == slash && !p1.value().is<JsonObject>()) {
                        if (val) {
                            output["api_data"] = p1.value().as<std::string>();
                            return true;
                        }
                        get_value_json(output, p_key, p1_key, p1.value());
                        return true;
                    }
                }
            } // else skip, but we don't have value pairs in system root
        }
    }
    return false;
}

void System::get_value_json(JsonObject output, const std::string & circuit, const std::string & name, JsonVariant val) {
    output["name"] = name;
    if (circuit.length()) {
        output["circuit"] = circuit;
    }
    output["readable"] = true;
    output["writeable"] =
        (name == "txpause" || name == "showerTimer" || name == "showerAlert" || name == "enabled" || name == "hideLed" || name == "analogEnabled");
    output["visible"] = true;
    if (val.is<bool>()) {
        output["value"] = val.as<bool>();
        output["type"]  = "boolean";
    } else if (val.is<float>() || val.is<int>()) {
        output["value"] = val.as<float>();
        output["type"]  = "number";
    } else {
        output["value"] = val.as<std::string>();
        output["type"]  = "string";
    }
}

// generate Prometheus metrics format from system values
std::string System::get_metrics_prometheus() {
    std::string                 result;
    std::map<std::string, bool> seen_metrics;

    result.reserve(16000);

    // get system data
    JsonDocument doc(PSRAM_DOC);
    JsonObject   root = doc.to<JsonObject>();
    (void)command_info("", 0, root);

    // helper function to escape Prometheus label values
    auto escape_label = [](const std::string & str) -> std::string {
        std::string escaped;
        for (char c : str) {
            if (c == '\\') {
                escaped += "\\\\";
            } else if (c == '"') {
                escaped += "\\\"";
            } else if (c == '\n') {
                escaped += "\\n";
            } else {
                escaped += c;
            }
        }
        return escaped;
    };

    // helper function to sanitize metric name (convert to lowercase and replace dots with underscores)
    auto sanitize_name = [](const std::string & name) -> std::string {
        std::string sanitized = name;
        for (char & c : sanitized) {
            if (c == '.') {
                c = '_';
            } else if (isupper(c)) {
                c = tolower(c);
            } else if (!isalnum(c) && c != '_') {
                c = '_';
            }
        }
        return sanitized;
    };

    // helper function to convert label name to lowercase
    auto to_lowercase = [](const std::string & str) -> std::string {
        std::string result = str;
        for (char & c : result) {
            if (isupper(c)) {
                c = tolower(c);
            }
        }
        return result;
    };

    // helper function to check if a field should be ignored
    auto should_ignore = [](const std::string & path, const std::string & key) -> bool {
        if (path == "system" && key == "uptime") {
            return true;
        }
        if (path == "ntp" && key == "timestamp") {
            return true;
        }
        if (path.find("devices[") != std::string::npos) {
            if (key == "handlersReceived" || key == "handlersFetched" || key == "handlersPending" || key == "handlersIgnored") {
                return true;
            }
        }
        return false;
    };

    // helper function to process a JSON object recursively
    std::function<void(const JsonObject, const std::string &)> process_object = [&](const JsonObject obj, const std::string & prefix) {
        std::vector<std::pair<std::string, std::string>> local_info_labels;
        bool                                             has_nested_objects = false;

        for (JsonPair p : obj) {
            std::string key         = p.key().c_str();
            std::string metric_name = prefix.empty() ? key : prefix + "_" + key;

            if (should_ignore(prefix, key)) {
                continue;
            }

            if (p.value().is<JsonObject>()) {
                // recursive call for nested objects
                has_nested_objects = true;
                process_object(p.value().as<JsonObject>(), metric_name);
            } else if (p.value().is<JsonArray>()) {
                // handle arrays (devices)
                if (key == "devices") {
                    JsonArray devices = p.value().as<JsonArray>();
                    for (JsonObject device : devices) {
                        std::vector<std::pair<std::string, std::string>> device_labels;

                        // collect labels from device object
                        for (JsonPair dp : device) {
                            std::string dkey = dp.key().c_str();
                            if (dkey == "type" || dkey == "name" || dkey == "deviceID" || dkey == "brand" || dkey == "version") {
                                if (dp.value().is<const char *>()) {
                                    std::string val = dp.value().as<const char *>();
                                    if (!val.empty()) {
                                        device_labels.push_back({to_lowercase(dkey), val});
                                    }
                                }
                            }
                        }

                        // create productID metric
                        if (device["productID"].is<int>()) {
                            std::string metric = "emsesp_device_productid";
                            if (seen_metrics.find(metric) == seen_metrics.end()) {
                                result += "# HELP emsesp_device_productid productID\n";
                                result += "# TYPE emsesp_device_productid gauge\n";
                                seen_metrics[metric] = true;
                            }

                            result += metric;
                            if (!device_labels.empty()) {
                                result += "{";
                                bool first = true;
                                for (const auto & label : device_labels) {
                                    if (!first) {
                                        result += ", ";
                                    }
                                    result += label.first + "=\"" + escape_label(label.second) + "\"";
                                    first = false;
                                }
                                result += "}";
                            }
                            result += " " + std::to_string(device["productID"].as<int>()) + "\n";
                        }

                        // create entities metric
                        if (device["entities"].is<int>()) {
                            std::string metric = "emsesp_device_entities";
                            if (seen_metrics.find(metric) == seen_metrics.end()) {
                                result += "# HELP emsesp_device_entities entities\n";
                                result += "# TYPE emsesp_device_entities gauge\n";
                                seen_metrics[metric] = true;
                            }

                            result += metric;
                            if (!device_labels.empty()) {
                                result += "{";
                                bool first = true;
                                for (const auto & label : device_labels) {
                                    if (!first) {
                                        result += ", ";
                                    }
                                    result += label.first + "=\"" + escape_label(label.second) + "\"";
                                    first = false;
                                }
                                result += "}";
                            }
                            result += " " + std::to_string(device["entities"].as<int>()) + "\n";
                        }
                    }
                }
            } else {
                // handle primitive values
                bool is_number = p.value().is<int>() || p.value().is<float>();
                bool is_bool   = p.value().is<bool>();
                bool is_string = p.value().is<const char *>();

                if (is_number || is_bool) {
                    // add metric
                    std::string full_metric_name = "emsesp_" + sanitize_name(metric_name);
                    if (seen_metrics.find(full_metric_name) == seen_metrics.end()) {
                        result += "# HELP emsesp_" + sanitize_name(metric_name) + " " + key + "\n";
                        result += "# TYPE emsesp_" + sanitize_name(metric_name) + " gauge\n";
                        seen_metrics[full_metric_name] = true;
                    }

                    result += full_metric_name + " ";
                    if (is_bool) {
                        result += p.value().as<bool>() ? "1" : "0";
                    } else if (p.value().is<int>()) {
                        result += std::to_string(p.value().as<int>());
                    } else {
                        char val_str[30];
                        snprintf(val_str, sizeof(val_str), "%.2f", p.value().as<float>());
                        result += val_str;
                    }
                    result += "\n";
                } else if (is_string) {
                    // collect string for info metric (skip dynamic strings like uptime and timestamp)
                    std::string val = p.value().as<const char *>();
                    if (!val.empty() && key != "uptime" && key != "timestamp") {
                        std::string lower_key = to_lowercase(key);
                        // check if key already exists in local_info_labels
                        bool key_exists = false;
                        for (const auto & label : local_info_labels) {
                            if (label.first == lower_key) {
                                key_exists = true;
                                break;
                            }
                        }
                        if (!key_exists) {
                            local_info_labels.push_back({lower_key, val});
                        }
                    }
                }
            }
        }

        // create _info metric for this object level if we have labels and this is a leaf node (no nested objects)
        if (!local_info_labels.empty() && !prefix.empty() && !has_nested_objects) {
            std::string info_metric = "emsesp_" + sanitize_name(prefix) + "_info";
            if (seen_metrics.find(info_metric) == seen_metrics.end()) {
                result += "# HELP " + info_metric + " info\n";
                result += "# TYPE " + info_metric + " gauge\n";
                seen_metrics[info_metric] = true;
            }

            result += info_metric;
            if (!local_info_labels.empty()) {
                result += "{";
                bool first = true;
                for (const auto & label : local_info_labels) {
                    if (!first) {
                        result += ", ";
                    }
                    result += label.first + "=\"" + escape_label(label.second) + "\"";
                    first = false;
                }
                result += "}";
            }
            result += " 1\n";
        }
    };

    // process root object
    process_object(root, "");

    result.shrink_to_fit();

    return result;
}

// return IP or hostname of the EMS-ESP device
String System::get_ip_or_hostname() {
    String result = "ems-esp";
#ifndef EMSESP_STANDALONE
    EMSESP::esp32React.getNetworkSettingsService()->read([&](NetworkSettings & settings) {
        if (settings.enableMDNS) {
            if (EMSESP::network_.ethernet_connected()) {
                result = ETH.getHostname();
            } else if (WiFi.status() == WL_CONNECTED) {
                result = WiFi.getHostname();
            }
        } else {
            // no DNS, use the IP
            if (EMSESP::network_.ethernet_connected()) {
                result = ETH.localIP().toString();
            } else if (WiFi.status() == WL_CONNECTED) {
                result = WiFi.localIP().toString();
            }
        }
    });
#endif
    return result;
}

// export status information including the device information
// http://ems-esp/api/system/info
bool System::command_info(const char * value, const int8_t id, JsonObject output) {
    JsonObject node;

    // System
    node = output["system"].to<JsonObject>();
// prevent false-negatives in Unity tests every time the version changes
#if defined(EMSESP_UNITY)
    node["version"] = "dev";
#else
    node["version"] = EMSESP_APP_VERSION;
#endif
    node["uptime"]      = uuid::log::format_timestamp_ms(uuid::get_uptime_ms(), 3);
    node["uptimeSec"]   = uuid::get_uptime_sec();
    node["resetReason"] = EMSESP::system_.reset_reason(0) + " / " + EMSESP::system_.reset_reason(1);
#ifndef EMSESP_STANDALONE
    node["platform"]        = EMSESP_PLATFORM;
    node["cpuType"]         = ESP.getChipModel();
    node["arduino"]         = ARDUINO_VERSION;
    node["sdk"]             = ESP.getSdkVersion();
    node["freeMem"]         = getHeapMem();
    node["maxAlloc"]        = getMaxAllocMem();
    node["minFree"]         = getMinFreeMem();                                      // all-time low watermark of internal heap
    node["freeCaps"]        = heap_caps_get_free_size(MALLOC_CAP_8BIT) / 1024;      // includes heap and psram
    node["usedApp"]         = EMSESP::system_.appUsed();                            // kilobytes
    node["freeApp"]         = EMSESP::system_.appFree();                            // kilobytes
    node["partition"]       = (const char *)esp_ota_get_running_partition()->label; // active partition
    node["flash_chip_size"] = ESP.getFlashChipSize() / 1024;                        // kilobytes
    node["psram"]           = (EMSESP::system_.PSram() > 0);                        // make boolean
    if (EMSESP::system_.PSram()) {
        node["psramSize"] = EMSESP::system_.PSram();
        node["freePsram"] = ESP.getFreePsram() / 1024;
    }
    node["model"] = EMSESP::system_.getBBQKeesGatewayDetails();
#if CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32S2
    node["temperature"] = EMSESP::system_.temperature();
#endif
#endif
    node["txpause"] = EMSbus::tx_mode() == EMS_TXMODE_OFF;

    // GPIO information
    std::string gpios_allowed_str;
    for (const auto & gpio : valid_system_gpios_) {
        if (!gpios_allowed_str.empty()) {
            gpios_allowed_str += ", ";
        }
        gpios_allowed_str += Helpers::itoa(gpio);
    }
    node["gpios_allowed"] = gpios_allowed_str;

    std::string gpios_in_use_str;
    auto        sorted_gpios = used_gpios_;
    std::sort(sorted_gpios.begin(), sorted_gpios.end(), [](const GpioUsage & a, const GpioUsage & b) { return a.pin < b.pin; });
    for (const auto & gpio : sorted_gpios) {
        if (!gpios_in_use_str.empty()) {
            gpios_in_use_str += ", ";
        }
        gpios_in_use_str += Helpers::itoa(gpio.pin);
    }
    node["gpios_in_use"] = gpios_in_use_str;

    std::string gpios_available_str;
    for (const auto & gpio : available_gpios()) {
        if (!gpios_available_str.empty()) {
            gpios_available_str += ", ";
        }
        gpios_available_str += Helpers::itoa(gpio);
    }
    node["gpios_available"] = gpios_available_str;

    // Network Status
    node = output["network"].to<JsonObject>();
#ifndef EMSESP_STANDALONE
    if (EMSESP::network_.ethernet_connected()) {
        node["network"]  = "Ethernet";
        node["hostname"] = ETH.getHostname();
        // node["MAC"]             = ETH.macAddress();
        // node["IPv4 address"]    = uuid::printable_to_string(ETH.localIP()) + "/" + uuid::printable_to_string(ETH.subnetMask());
        // node["IPv4 gateway"]    = uuid::printable_to_string(ETH.gatewayIP());
        // node["IPv4 nameserver"] = uuid::printable_to_string(ETH.dnsIP());
        // if (ETH.localIPv6().toString() != "0000:0000:0000:0000:0000:0000:0000:0000") {
        //     node["IPv6 address"] = uuid::printable_to_string(ETH.localIPv6());
        // }
    } else if (WiFi.status() == WL_CONNECTED) {
        node["network"]        = "WiFi";
        node["hostname"]       = WiFi.getHostname();
        node["RSSI"]           = WiFi.RSSI();
        node["WIFIReconnects"] = EMSESP::network_.getNetworkReconnects();
        // node["MAC"]             = WiFi.macAddress();
        // node["IPv4 address"]    = uuid::printable_to_string(WiFi.localIP()) + "/" + uuid::printable_to_string(WiFi.subnetMask());
        // node["IPv4 gateway"]    = uuid::printable_to_string(WiFi.gatewayIP());
        // node["IPv4 nameserver"] = uuid::printable_to_string(WiFi.dnsIP());
        // if (WiFi.localIPv6().toString() != "0000:0000:0000:0000:0000:0000:0000:0000") {
        //     node["IPv6 address"] = uuid::printable_to_string(WiFi.localIPv6());
        // }
    }
#else
    // for testing
    node["network"]  = "WiFi";
    node["hostname"] = "ems-esp";
    node["RSSI"]     = -23;
#endif
    EMSESP::esp32React.getNetworkSettingsService()->read([&](NetworkSettings & settings) {
        if (WiFi.status() == WL_CONNECTED && !settings.bssid.isEmpty()) {
            node["BSSID"] = "set"; // we don't disclose the name
        }
        node["TxPowerSetting"] = settings.tx_power;
        node["staticIP"]       = settings.staticIPConfig;
        node["lowBandwidth"]   = settings.bandwidth20;
        node["disableSleep"]   = settings.nosleep;
        node["enableMDNS"]     = settings.enableMDNS;
        node["enableCORS"]     = settings.enableCORS;
        if (settings.enableCORS) {
            node["CORSOrigin"] = settings.CORSOrigin;
        }
    });

    // NTP status
    node              = output["ntp"].to<JsonObject>();
    node["NTPstatus"] = EMSESP::system_.ntp_connected() ? "connected" : "disconnected";
    EMSESP::esp32React.getNTPSettingsService()->read([&](const NTPSettings & settings) {
#ifndef EMSESP_STANDALONE
        node["enabled"] = settings.enabled;
#else
        node["enabled"] = true;
#endif
        node["server"]  = settings.server;
        node["tzLabel"] = settings.tzLabel;
    });
#ifndef EMSESP_STANDALONE
    node["timestamp"] = time(nullptr);
#endif
    node["NTPStatus"] = EMSESP::system_.ntp_connected() ? "connected" : "disconnected";

    // AP Status
    node = output["ap"].to<JsonObject>();
    EMSESP::esp32React.getAPSettingsService()->read([&](const APSettings & settings) {
        const char * pM[]     = {"always", "disconnected", "never"};
        node["provisionMode"] = pM[settings.provisionMode];
        node["ssid"]          = settings.ssid;
#ifndef EMSESP_STANDALONE
        node["security"]   = settings.password.length() ? "wpa2" : "open";
        node["channel"]    = settings.channel;
        node["ssidHidden"] = settings.ssidHidden;
        node["maxClients"] = settings.maxClients;
        node["localIP"]    = settings.localIP.toString();
        node["gatewayIP"]  = settings.gatewayIP.toString();
        node["subnetMask"] = settings.subnetMask.toString();
#endif
    });

    // MQTT Status
    node               = output["mqtt"].to<JsonObject>();
    node["MQTTStatus"] = Mqtt::connected() ? F_(connected) : F_(disconnected);
    if (Mqtt::enabled()) {
        node["MQTTPublishes"]    = Mqtt::publish_count();
        node["MQTTQueued"]       = Mqtt::publish_queued();
        node["MQTTPublishFails"] = Mqtt::publish_fails();
        node["MQTTReconnects"]   = Mqtt::connect_count();
    }
    EMSESP::esp32React.getMqttSettingsService()->read([&](const MqttSettings & settings) {
        node["enabled"]               = settings.enabled;
        node["clientID"]              = settings.clientId;
        node["keepAlive"]             = settings.keepAlive;
        node["cleanSession"]          = settings.cleanSession;
        node["entityFormat"]          = settings.entity_format;
        node["base"]                  = settings.base;
        node["discoveryPrefix"]       = settings.discovery_prefix;
        node["discoveryType"]         = settings.discovery_type;
        node["nestedFormat"]          = settings.nested_format;
        node["haEnabled"]             = settings.ha_enabled;
        node["mqttQos"]               = settings.mqtt_qos;
        node["mqttRetain"]            = settings.mqtt_retain;
        node["publishTimeHeartbeat"]  = settings.publish_time_heartbeat;
        node["publishTimeBoiler"]     = settings.publish_time_boiler;
        node["publishTimeThermostat"] = settings.publish_time_thermostat;
        node["publishTimeSolar"]      = settings.publish_time_solar;
        node["publishTimeMixer"]      = settings.publish_time_mixer;
        node["publishTimeWater"]      = settings.publish_time_water;
        node["publishTimeOther"]      = settings.publish_time_other;
        node["publishTimeSensor"]     = settings.publish_time_sensor;
        node["publishSingle"]         = settings.publish_single;
        node["publish2command"]       = settings.publish_single2cmd;
        node["sendResponse"]          = settings.send_response;
    });

    // Syslog Status
    node            = output["syslog"].to<JsonObject>();
    node["enabled"] = EMSESP::system_.syslog_enabled_;
#ifndef EMSESP_STANDALONE
    if (EMSESP::system_.syslog_enabled_) {
        node["syslogStarted"] = syslog_.started();
        node["syslogLevel"]   = FL_(list_syslog_level)[syslog_.log_level() + 1];
        node["syslogIP"]      = syslog_.ip();
        node["syslogQueue"]   = syslog_.queued();
    }
#endif

    // Modbus Status
    node            = output["modbus"].to<JsonObject>();
    node["enabled"] = EMSESP::system_.modbus_enabled_;
    if (EMSESP::system_.modbus_enabled_) {
        node["maxClients"] = EMSESP::system_.modbus_max_clients_;
        node["port"]       = EMSESP::system_.modbus_port_;
        node["timeout"]    = EMSESP::system_.modbus_timeout_;
    }

    // Sensor Status
    node = output["sensor"].to<JsonObject>();
    if (EMSESP::sensor_enabled()) {
        node["temperatureSensors"]     = EMSESP::temperaturesensor_.count_entities();
        node["temperatureSensorReads"] = EMSESP::temperaturesensor_.reads();
        node["temperatureSensorFails"] = EMSESP::temperaturesensor_.fails();
    }

    // Analog Status
    node            = output["analog"].to<JsonObject>();
    node["enabled"] = EMSESP::analog_enabled();
    if (EMSESP::analog_enabled()) {
        node["analogSensors"]     = EMSESP::analogsensor_.count_entities();
        node["analogSensorReads"] = EMSESP::analogsensor_.reads();
        node["analogSensorFails"] = EMSESP::analogsensor_.fails();
    }

    // API Status
    node = output["api"].to<JsonObject>();

// if we're generating test data for Unit Tests we dont want to count these API calls as it will pollute the data response
#if defined(EMSESP_UNITY)
    node["APICalls"] = 0;
    node["APIFails"] = 0;
#else
    node["APICalls"] = WebAPIService::api_count();
    node["APIFails"] = WebAPIService::api_fails();
#endif

    // EMS Bus Status
    node = output["bus"].to<JsonObject>();
    switch (EMSESP::bus_status()) {
    case EMSESP::BUS_STATUS_OFFLINE:
        node["busStatus"] = "disconnected";
        break;
    case EMSESP::BUS_STATUS_TX_ERRORS:
        node["busStatus"] = "connected, tx issues - try a different Tx Mode";
        break;
    case EMSESP::BUS_STATUS_CONNECTED:
        node["busStatus"] = "connected";
        break;
    default:
        node["busStatus"] = "unknown";
        break;
    }
    node["busProtocol"]            = EMSbus::is_ht3() ? "HT3" : "Buderus";
    node["busTelegramsReceived"]   = EMSESP::rxservice_.telegram_count();
    node["busReads"]               = EMSESP::txservice_.telegram_read_count();
    node["busWrites"]              = EMSESP::txservice_.telegram_write_count();
    node["busIncompleteTelegrams"] = EMSESP::rxservice_.telegram_error_count();
    node["busReadsFailed"]         = EMSESP::txservice_.telegram_read_fail_count();
    node["busWritesFailed"]        = EMSESP::txservice_.telegram_write_fail_count();
    node["busRxLineQuality"]       = EMSESP::rxservice_.quality();
    node["busTxLineQuality"]       = (EMSESP::txservice_.read_quality() + EMSESP::txservice_.write_quality()) / 2;

    // Settings
    node = output["settings"].to<JsonObject>();
    EMSESP::webSettingsService.read([&](const WebSettings & settings) {
        node["boardProfile"]      = settings.board_profile;
        node["locale"]            = settings.locale;
        node["txMode"]            = settings.tx_mode;
        node["emsBusID"]          = settings.ems_bus_id;
        node["showerTimer"]       = settings.shower_timer;
        node["showerMinDuration"] = settings.shower_min_duration; // seconds
        node["showerAlert"]       = settings.shower_alert;
        if (settings.shower_alert) {
            node["showerAlertColdshot"] = settings.shower_alert_coldshot; // seconds
            node["showerAlertTrigger"]  = settings.shower_alert_trigger;  // minutes
        }
        if (settings.board_profile == "CUSTOM") {
            node["phyType"] = settings.phy_type;
            if (settings.phy_type != PHY_type::PHY_TYPE_NONE) {
                node["ethPower"]      = settings.eth_power;
                node["ethPhyAddr"]    = settings.eth_phy_addr;
                node["ethClockMmode"] = settings.eth_clock_mode;
            }
            node["rxGPIO"]      = settings.rx_gpio;
            node["txGPIO"]      = settings.tx_gpio;
            node["dallasGPIO"]  = settings.dallas_gpio;
            node["pbuttonGPIO"] = settings.pbutton_gpio;
            node["ledGPIO"]     = settings.led_gpio;
            node["ledType"]     = settings.led_type;
        }
        node["hideLed"]         = settings.hide_led;
        node["noTokenApi"]      = settings.notoken_api;
        node["readonlyMode"]    = settings.readonly_mode;
        node["fahrenheit"]      = settings.fahrenheit;
        node["dallasParasite"]  = settings.dallas_parasite;
        node["boolFormat"]      = settings.bool_format;
        node["boolDashboard"]   = settings.bool_dashboard;
        node["enumFormat"]      = settings.enum_format;
        node["analogEnabled"]   = settings.analog_enabled;
        node["telnetEnabled"]   = settings.telnet_enabled;
        node["maxWebLogBuffer"] = settings.weblog_buffer;

        /*
 #if defined(EMSESP_UNITY)
         node["webLogBuffer"] = 0;
 #else
         node["webLogBuffer"] = EMSESP::webLogService.num_log_messages();
 #endif
 */
        node["modbusEnabled"]   = settings.modbus_enabled;
        node["forceHeatingOff"] = settings.boiler_heatingoff;
        node["developerMode"]   = settings.developer_mode;
        node["disableReset"]    = settings.disable_reset;
    });

    // Devices - show EMS devices if we have any
    JsonArray devices = output["devices"].to<JsonArray>();
    if (!EMSESP::emsdevices.empty()) {
        for (const auto & device_class : EMSFactory::device_handlers()) {
            for (const auto & emsdevice : EMSESP::emsdevices) {
                if (emsdevice && (emsdevice->device_type() == device_class.first)) {
                    JsonObject obj   = devices.add<JsonObject>();
                    obj["type"]      = emsdevice->device_type_name(); // non translated name
                    obj["name"]      = emsdevice->name();             // custom name
                    obj["deviceID"]  = Helpers::hextoa(emsdevice->device_id());
                    obj["productID"] = emsdevice->product_id();
                    obj["brand"]     = emsdevice->brand_to_char();
                    obj["version"]   = emsdevice->version();
                    obj["entities"]  = emsdevice->count_entities();
                    char result[1000];
                    (void)emsdevice->show_telegram_handlers(result, sizeof(result), EMSdevice::Handlers::RECEIVED);
                    if (result[0] != '\0') {
                        obj["handlersReceived"] = result; // don't show handlers if there aren't any
                    }
                    (void)emsdevice->show_telegram_handlers(result, sizeof(result), EMSdevice::Handlers::FETCHED);
                    if (result[0] != '\0') {
                        obj["handlersFetched"] = result;
                    }
                    (void)emsdevice->show_telegram_handlers(result, sizeof(result), EMSdevice::Handlers::PENDING);
                    if (result[0] != '\0') {
                        obj["handlersPending"] = result;
                    }
                    (void)emsdevice->show_telegram_handlers(result, sizeof(result), EMSdevice::Handlers::IGNORED);
                    if (result[0] != '\0') {
                        obj["handlersIgnored"] = result;
                    }
                }
            }
        }
    }

    // Also show EMSESP devices if we have any
    if (EMSESP::temperaturesensor_.count_entities()) {
        JsonObject obj  = devices.add<JsonObject>();
        obj["type"]     = F_(temperaturesensor);
        obj["name"]     = F_(temperaturesensor);
        obj["entities"] = EMSESP::temperaturesensor_.count_entities();
    }
    if (EMSESP::analogsensor_.count_entities()) {
        JsonObject obj  = devices.add<JsonObject>();
        obj["type"]     = F_(analogsensor);
        obj["name"]     = F_(analogsensor);
        obj["entities"] = EMSESP::analogsensor_.count_entities();
    }
    if (EMSESP::webSchedulerService.count_entities()) {
        JsonObject obj  = devices.add<JsonObject>();
        obj["type"]     = F_(scheduler);
        obj["name"]     = F_(scheduler);
        obj["entities"] = EMSESP::webSchedulerService.count_entities();
    }
    if (EMSESP::webCommandService.count_entities()) {
        JsonObject obj  = devices.add<JsonObject>();
        obj["type"]     = F_(commands);
        obj["name"]     = F_(commands);
        obj["entities"] = EMSESP::webCommandService.count_entities();
    }
    if (EMSESP::webCustomEntityService.count_entities()) {
        JsonObject obj  = devices.add<JsonObject>();
        obj["type"]     = F_(custom);
        obj["name"]     = F_(custom);
        obj["entities"] = EMSESP::webCustomEntityService.count_entities();
    }

    return true; // this function always returns true!
}

#if defined(EMSESP_TEST)
// run a test, e.g. http://ems-esp/api?device=system&cmd=test&data=boiler
bool System::command_test(const char * value, const int8_t id) {
    if (value) {
        return Test::test(value, id);
    } else {
        return false;
    }
}
#endif

// takes a board profile and populates a data array with GPIO configurations
// returns false if profile is unknown
//
// 0=led, 1=dallas, 2=rx, 3=tx, 4=button, 5=phy_type, 6=eth_power, 7=eth_phy_addr, 8=eth_clock_mode, 9=led_type
//
bool System::load_board_profile(std::vector<int8_t> & data, const std::string & board_profile) {
    if (board_profile == "default") {
        return false; // unknown, return false
    } else if (board_profile == "S32") {
        data                = {2, 18, 23, 5, 0, PHY_type::PHY_TYPE_NONE, 0, 0, 0, 0}; // BBQKees Gateway S32
        valid_system_gpios_ = {0, 2, 5, 18, 23};
    } else if (board_profile == "E32") {
        data                = {2, 4, 5, 17, 33, PHY_type::PHY_TYPE_LAN8720, 16, 1, 0, 0}; // BBQKees Gateway E32
        valid_system_gpios_ = {0, 2, 4, 5, 16, 17, 33};
    } else if (board_profile == "E32V2") {
        data                = {2, 14, 4, 5, 34, PHY_type::PHY_TYPE_LAN8720, 15, 0, 1, 0}; // BBQKees Gateway E32 V2
        valid_system_gpios_ = {0, 2, 4, 5, 14, 15, 34};
    } else if (board_profile == "E32V2_2") {
        data                = {32, 14, 4, 5, 34, PHY_type::PHY_TYPE_LAN8720, 15, 0, 1, 1}; // BBQKees Gateway E32 V2.2, rgb led
        valid_system_gpios_ = {0, 2, 4, 5, 14, 15, 32, 34, 36, 39};                        // system analogs 36, 39, led 2
    } else if (board_profile == "MH-ET") {
        data = {2, 18, 23, 5, 0, PHY_type::PHY_TYPE_NONE, 0, 0, 0, 0}; // MH-ET Live D1 Mini
        // allow only pins that are marked as `can always be used`
        valid_system_gpios_ = {0, 2, 5, 18, 23, 12, 13, 14, 15, 16, 17, 26, 27, 33};
        // can always be used: 12, 13 ,14, 15, 16, 17, 26, 27, 33
        // can be used if no other function 2, 4, 5, 9, 10, 18, 19, 21, 22, 23, 25, 34, 35, 36, 39
    } else if (board_profile == "NODEMCU") {
        data = {2, 18, 23, 5, 0, PHY_type::PHY_TYPE_NONE, 0, 0, 0, 0}; // NodeMCU 32S
        // https://blog.berrybase.de/esp32-node-mcu-module-anfaenger-guide/
        // all available pins, exclude uart0
        valid_system_gpios_ = {0, 2, 5, 18, 23, 4, 12, 13, 14, 15, 16, 17, 21, 22, 25, 26, 27, 32, 33, 34, 35, 36, 39};
    } else if (board_profile == "LOLIN") {
        data = {2, 18, 17, 16, 0, PHY_type::PHY_TYPE_NONE, 0, 0, 0, 0}; // Lolin D32
        // https://www.wemos.cc/en/latest/d32/d32.html
        valid_system_gpios_ = {2, 18, 17, 16, 0, 4, 5, 12, 13, 14, 15, 21, 22, 25, 26, 27, 32, 33, 34, 35, 36, 39};
    } else if (board_profile == "OLIMEX") {
        data = {0, 0, 36, 4, 34, PHY_type::PHY_TYPE_LAN8720, -1, 0, 0, 0}; // Olimex ESP32-EVB (uses U1TXD/U1RXD/BUTTON, no LED or Temperature sensor)
        // https://github.com/OLIMEX/ESP32-EVB/blob/master/HARDWARE/REV-K1/ESP32-EVB_Rev_K1.pdf
        // uart0 = 1, 3; CAN = 5, 35; relais = 32, 33; ir = 12(tx), 39(rx); SD-card = 2, 14, 15, button = 34
        // relais and ir can be configured as analog sensor
        valid_system_gpios_ = {4, 34, 36, 12, 13, 21, 22, 25, 26, 27, 32, 33, 39};
    } else if (board_profile == "OLIMEXPOE") {
        data = {0, 0, 36, 4, 34, PHY_type::PHY_TYPE_LAN8720, 12, 0, 3, 0}; // Olimex ESP32-POE
        // https://github.com/OLIMEX/ESP32-POE/blob/master/HARDWARE/ESP32-PoE-hardware-revision-L1/ESP32-PoE_Rev_L1.pdf
        // uart0 = 1, 3; SD-card = 2, 14, 15; button = 34;
        valid_system_gpios_ = {4, 34, 36, 12, 13, 21, 22, 25, 26, 27, 32, 33, 39};
    } else if (board_profile == "OLIMEXPOEW") {
        data                = {0, 0, 36, 4, 34, PHY_type::PHY_TYPE_LAN8720, 12, 0, 1, 0}; // Olimex ESP32-POE-ISO WROVER (PSRAM)
        valid_system_gpios_ = {4, 34, 36, 12, 13, 21, 22, 25, 26, 27, 32, 33, 39};
    } else if (board_profile == "C3MINI") {
#if defined(BOARD_C3_MINI_V1)
        data = {7, 1, 4, 5, 9, PHY_type::PHY_TYPE_NONE, 0, 0, 0, 0}; // Lolin C3 Mini V1
#else
        data = {7, 1, 4, 5, 9, PHY_type::PHY_TYPE_NONE, 0, 0, 0, 1}; // Lolin C3 Mini with RGB Led
#endif
        // https://www.wemos.cc/en/latest/c3/c3_mini.html
        valid_system_gpios_ = {0, 1, 3, 4, 5, 6, 7, 9, 10, 20, 21};
    } else if (board_profile == "S2MINI") {
        data = {15, 7, 11, 12, 0, PHY_type::PHY_TYPE_NONE, 0, 0, 0, 0}; // Lolin S2 Mini
        // https://www.wemos.cc/en/latest/s2/s2_mini.html
        set_valid_system_gpios();
    } else if (board_profile == "S3MINI") {
        data = {17, 18, 8, 5, 0, PHY_type::PHY_TYPE_NONE, 0, 0, 0, 0}; // Liligo S3
        // https://lilygo.cc/products/t7-s3
        set_valid_system_gpios();
    } else if (board_profile == "S32S3") {
        data                = {2, 18, 5, 17, 0, PHY_type::PHY_TYPE_NONE, 0, 0, 0, 0}; // BBQKees Gateway S3
        valid_system_gpios_ = {0, 2, 5, 17, 18};
    } else {
        return false; // unknown, return false
    }

    return true;
}

// led command
// https://github.com/emsesp/EMS-ESP32/issues/3063
// /api//system/led command that takes an argument in the form [color]:[pattern]
// color is red, green, blue, yellow, white
// pattern is
//  blink1 for 1 time
//  blink2 for 2 times
//  blink3 for 3 times
//  rgb for RGB
// For example: /api/system/led?data=red:blink1
// For older non-RGB models, the colour would default to just being on.
bool System::command_led(const char * value, const int8_t) {
    if (!value) {
        return false; // no argument
    }

    std::string arg = value;
    if (arg.find(':') == std::string::npos) {
        LOG_ERROR("LED command must be in the form [color]:[pattern]");
        return false; // not in the form [color]:[pattern]
    }
    std::string color   = arg.substr(0, arg.find(':'));
    std::string pattern = arg.substr(arg.find(':') + 1);

    // set and validate the color and pattern
    if (!EMSESP::led_.set_custom_led_routine(color, pattern)) {
        LOG_ERROR("Invalid color or pattern.");
        return false;
    }

    return true;
}

// txpause command - temporarily pause the TX, by setting Txmode to 0 (disabled)
bool System::command_txpause(const char * value, const int8_t) {
    bool arg;
    if (!Helpers::value2bool(value, arg)) {
        return false; // argument not recognized
    }

    if (!arg) {
        // arg = false: Tx mode to 0 (disabled) to pause
        if (EMSbus::tx_mode() == EMS_TXMODE_OFF) {
            EMSESP::webSettingsService.read([&](WebSettings & settings) {
                EMSbus::tx_mode(settings.tx_mode);
#ifdef EMSESP_DEBUG
                LOG_INFO("TX mode restored (value %d)", settings.tx_mode);
#else
                LOG_INFO("TX active");
#endif
            });
        }
    } else {
        // pause = true: Tx mode to 0 (disabled) to pause
        if (EMSbus::tx_mode() != EMS_TXMODE_OFF) {
            EMSbus::tx_mode(EMS_TXMODE_OFF);
#ifdef EMSESP_DEBUG
            LOG_INFO("TX mode set to OFF (value %d)", EMS_TXMODE_OFF);
#else
            LOG_INFO("TX paused");
#endif
        }
    }
    return true;
}

// format command - factory reset, removing all settings files
bool System::command_format(const char * value, const int8_t id) {
    if (EMSESP::system_.disable_reset()) {
        LOG_NOTICE("Factory reset disabled");
        return false;
    }

#if !defined(EMSESP_STANDALONE) && !defined(EMSESP_TEST)
    // don't really format the filesystem in test or standalone mode
    if (LittleFS.format()) {
        LOG_INFO("Filesystem formatted successfully. All settings files removed.");
    } else {
        LOG_ERROR("Format failed");
    }
#else
    LOG_ERROR("Format command not available in standalone or test mode");
#endif

    // restart will be handled by the main loop
    EMSESP::system_.systemStatus(SYSTEM_STATUS::SYSTEM_STATUS_RESTART_REQUESTED);
    return true;
}

// restart command - perform a hard reset (system reboot)
bool System::command_restart(const char * value, const int8_t id) {
    if (id == 0) {
        // if it has an id then it's a web call and we need to queue the restart
        // default id is -1 when calling /api/system/restart directly for example
        LOG_INFO("Preparing to restart system");
        EMSESP::system_.systemStatus(SYSTEM_STATUS::SYSTEM_STATUS_PENDING_RESTART);
        return true;
    }

    LOG_INFO("Restarting system...");
    // restart will be handled by the main loop
    EMSESP::system_.systemStatus(SYSTEM_STATUS::SYSTEM_STATUS_RESTART_REQUESTED);
    return true;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch"

std::string System::reset_reason(uint8_t cpu) const {
#ifndef EMSESP_STANDALONE
    switch (esp_rom_get_reset_reason(cpu)) {
    case RESET_REASON_CHIP_POWER_ON:
        return ("Power on reset");
    case 2: // not on esp32
        return ("reset pin");
    case RESET_REASON_CORE_SW:
        return ("Software reset");
    case 4: // not on S2, C3
        return ("Legacy watch dog reset");
    case RESET_REASON_CORE_DEEP_SLEEP:
        return ("Deep sleep reset");
    case 6: // RESET_REASON_CORE_SDIO: // not on S2, S3, C3
        return ("Reset by SDIO");
    case RESET_REASON_CORE_MWDT0:
        return ("Timer group0 watch dog reset");
    case RESET_REASON_CORE_MWDT1:
        return ("Timer group1 watch dog reset");
    case RESET_REASON_CORE_RTC_WDT:
        return ("RTC watch dog reset");
    case 10:
        return ("Intrusion reset CPU");
    case RESET_REASON_CPU0_MWDT0:
        return ("Timer group reset CPU");
    case RESET_REASON_CPU0_SW:
        return ("Software reset CPU");
    case RESET_REASON_CPU0_RTC_WDT:
        return ("RTC watch dog reset: CPU");
    case 14: // RESET_REASON_CPU1_CPU0: // not on S2, S3, C3
        return ("APP CPU reset by PRO CPU");
    case RESET_REASON_SYS_BROWN_OUT:
        return ("Brownout reset");
    case RESET_REASON_SYS_RTC_WDT:
        return ("RTC watch dog reset: CPU+RTC");
    default:
        break;
    }
#endif
    return "Unknown";
}
#pragma GCC diagnostic pop

// set NTP status
void System::ntp_connected(bool b) {
    if (b != ntp_connected_) {
        if (b) {
            LOG_INFO("NTP connected");
            set_partition_install_date();
        } else {
            LOG_WARNING("NTP disconnected"); // if turned off report it
        }
    }

    ntp_connected_  = b;
    ntp_last_check_ = b ? uuid::get_uptime_sec() : 0;
}

// get NTP status
bool System::ntp_connected() {
    // timeout 2 hours, ntp sync is normally every hour.
    if ((uuid::get_uptime_sec() - ntp_last_check_ > 7201) && ntp_connected_) {
        ntp_connected(false);
    }

    return ntp_connected_;
}

// see if its a BBQKees Gateway by checking the eFuse values
String System::getBBQKeesGatewayDetails([[maybe_unused]] uint8_t detail) {
#ifndef EMSESP_STANDALONE
    union {
        struct {
            uint32_t no : 4;
            uint32_t month : 4;
            uint32_t year : 8;
            uint32_t rev_minor : 4;
            uint32_t rev_major : 4;
            uint32_t model : 4;
            uint32_t mfg : 4;
        };
        uint32_t reg;
    } gw;

    for (uint8_t reg = 0; reg < 8; reg++) {
        gw.reg = esp_efuse_read_reg(EFUSE_BLK3, reg);
        if (reg == 7 || esp_efuse_read_reg(EFUSE_BLK3, reg + 1) == 0)
            break;
    }

    const char * mfg[]   = {"unknown", "BBQKees Electronics", "", "", "", "", "", ""};
    const char * model[] = {"unknown", "S3", "E32V2", "E32V2.2", "S32", "E32", "", "", ""};
    const char * board[] = {"CUSTOM", "S32S3", "E32V2", "E32V2_2", "S32", "E32", "", "", ""};

    switch (detail) {
    case FUSE_VALUE::MFG:
        return gw.mfg < 2 ? String(mfg[gw.mfg]) : "unknown";
    case FUSE_VALUE::MODEL:
        return gw.model < 6 ? String(model[gw.model]) : "unknown";
    case FUSE_VALUE::BOARD:
        return gw.model < 6 ? String(board[gw.model]) : board_profile_;
    case FUSE_VALUE::REV:
        return String(gw.rev_major) + "." + String(gw.rev_minor);
    case FUSE_VALUE::BATCH:
        return String(2000 + gw.year) + (gw.month < 10 ? "0" : "") + String(gw.month) + String(gw.no);
    case FUSE_VALUE::FUSE:
        return "0x" + String(gw.reg, 16);
    case FUSE_VALUE::ALL:
    default:
        break;
    }

    if (!gw.reg || gw.mfg > 1 || gw.model > 5) {
        return "";
    }

    return String(mfg[gw.mfg]) + " " + String(model[gw.model]) + " rev." + String(gw.rev_major) + "." + String(gw.rev_minor) + "/" + String(2000 + gw.year)
           + (gw.month < 10 ? "0" : "") + String(gw.month) + String(gw.no);
#else
    return "";
#endif
}

// Stream from an URL and send straight to OTA uploader service.
//
// This function needs to be called twice, 1st pass once with a url to persist it, 2nd pass with no arguments to start the upload
// This is to avoid timeouts in callback functions, like calling from a web hook.
bool System::uploadFirmwareURL(const char * url) {
#ifndef EMSESP_STANDALONE
    static String saved_url;

    if (url && strlen(url) > 0) {
        // if the passed URL is "reset" abort the current upload. This is called when an error happens during OTA
        if (strncmp(url, "reset", 5) == 0) {
            LOG_DEBUG("Firmware upload - resetting");
            saved_url.clear();
            return true;
        }

        // given a URL to download from, save it ready for the 2nd pass
        saved_url = url;
        LOG_INFO("Firmware location: %s", saved_url.c_str());
        EMSESP::system_.systemStatus(SYSTEM_STATUS::SYSTEM_STATUS_PENDING_UPLOAD); // we're ready to start the upload
        return true;
    }

    // check we have a valid URL from the 1st pass
    if (saved_url.isEmpty()) {
        LOG_ERROR("Firmware upload failed - invalid URL");
        return false; // error
    }

    Shell::loop_all(); // flush log buffers so latest messages are shown in console

    String scheme = saved_url.substring(0, 8);
    scheme.toLowerCase();
    const bool is_https   = scheme.startsWith("https://");
    const int  scheme_len = is_https ? 8 : 7; // "https://" vs "http://"

    WiFiClient    basic_client;
    ESP_SSLClient ssl_client;

    Stream * stream        = nullptr;
    int      firmware_size = 0;

    if (is_https) {
        ssl_client.setInsecure(); // no CA validation, matches the rest of the project
        // BearSSL needs a receive buffer large enough to hold one full TLS record.
        // GitHub's release-assets CDN sends standard up-to-16 KB records and does NOT
        // negotiate max_fragment_length, so a small (e.g. 1 KB) RX buffer makes the
        // body unreadable (headers still fit one small record, hence Content-Length
        // looks fine, but the first body record cannot be decoded). 16384 + overhead
        // is the safe value the library itself uses by default; we go a bit smaller
        // to be friendlier to 4 MB / no-PSRAM boards while still big enough for any
        // record the CDN actually sends in practice.
        ssl_client.setBufferSizes(16384, 1024);
        ssl_client.setSessionTimeout(120);
    }
    basic_client.setTimeout(15000);                // socket-level read timeout
    ssl_client.setTimeout(15);                     // Stream::readBytes timeout used by Update
    ssl_client.setClient(&basic_client, is_https); // enableSSL = false for plain HTTP

    const uint16_t port           = is_https ? 443 : 80;
    String         url_remain     = saved_url.substring(scheme_len);
    int            redirect_count = 0;

    while (true) {
        // split url_remain into host and path
        String host;
        String path;
        int    s = url_remain.indexOf('/');
        if (s < 0) {
            host = url_remain;
            path = "/";
        } else {
            host = url_remain.substring(0, s);
            path = url_remain.substring(s);
        }

        LOG_DEBUG("Connecting to %s", host.c_str());
        if (!ssl_client.connect(host.c_str(), port)) {
            LOG_ERROR("Firmware upload failed - connection failed");
            return false;
        }

        // send a minimal HTTP/1.0 GET so we don't have to deal with chunked encoding
        ssl_client.print("GET ");
        ssl_client.print(path);
        ssl_client.println(" HTTP/1.0");
        ssl_client.print("Host: ");
        ssl_client.println(host);
        ssl_client.println("User-Agent: EMS-ESP");
        ssl_client.println("Connection: close");
        ssl_client.print("\r\n");

        // wait for the first byte (up to 8s, matching the previous HTTP timeout)
        uint32_t ms = millis();
        while (ssl_client.connected() && !ssl_client.available() && millis() - ms < 8000) {
            delay(1);
        }

        // parse status line: "HTTP/1.x CODE TEXT"
        String status_line = ssl_client.readStringUntil('\n');
        int    sp          = status_line.indexOf(' ');
        int    http_code   = (sp >= 0) ? status_line.substring(sp + 1, sp + 4).toInt() : 0;

        // parse response headers, looking for Content-Length and Location
        int    content_length = -1;
        String location;
        while (ssl_client.connected() || ssl_client.available()) {
            String line = ssl_client.readStringUntil('\n');
            line.trim();
            if (line.isEmpty()) {
                break; // end of headers
            }
            int colon = line.indexOf(':');
            if (colon < 0) {
                continue;
            }
            String name = line.substring(0, colon);
            name.toLowerCase();
            String val = line.substring(colon + 1);
            val.trim();
            if (name == "content-length") {
                content_length = val.toInt();
            } else if (name == "location") {
                location = val;
            }
        }

        // follow redirects manually (GitHub releases redirect to objects.githubusercontent.com)
        if (http_code == 301 || http_code == 302 || http_code == 303 || http_code == 307 || http_code == 308) {
            ssl_client.stop();
            if (location.isEmpty() || ++redirect_count > 5) {
                LOG_ERROR("Firmware upload failed - too many redirects");
                return false;
            }
            String lower_loc = location;
            lower_loc.toLowerCase();
            if (lower_loc.startsWith("https://") || lower_loc.startsWith("http://")) {
                // scheme-changing redirect is not supported - the SSL state is
                // baked in at setClient() time and we don't want to re-init mid-flight
                const bool new_is_https = lower_loc.startsWith("https://");
                if (new_is_https != is_https) {
                    LOG_ERROR("Firmware upload failed - cross-scheme redirect to %s", location.c_str());
                    return false;
                }
                url_remain = location.substring(new_is_https ? 8 : 7);
            } else if (location.startsWith("/")) {
                url_remain = host + location; // relative redirect, same host
            } else {
                LOG_ERROR("Firmware upload failed - unsupported redirect to %s", location.c_str());
                return false;
            }
            LOG_DEBUG("Following redirect to %s", url_remain.c_str());
            continue;
        }

        if (http_code != 200) {
            ssl_client.stop();
            LOG_ERROR("Firmware upload failed - HTTP code %d", http_code);
            return false;
        }

        if (content_length <= 0) {
            ssl_client.stop();
            LOG_ERROR("Firmware upload failed - missing Content-Length");
            return false;
        }

        // wait for the first byte of the body so the read loop sees real data
        // (headers and body may arrive in separate TLS records)
        uint32_t body_wait = millis();
        while (ssl_client.connected() && !ssl_client.available() && millis() - body_wait < 8000) {
            delay(1);
        }
        if (!ssl_client.available()) {
            ssl_client.stop();
            LOG_ERROR("Firmware upload failed - no body received");
            return false;
        }

        stream        = &ssl_client;
        firmware_size = content_length;
        break;
    }

    // check we have a valid size
    if (firmware_size < 1677721) { // 1.6MB or greater is required
        LOG_ERROR("Firmware upload failed - invalid size");
        return false; // error
    }

    // check we have enough space for the upload in the ota partition
    if (!Update.begin(firmware_size)) {
        LOG_ERROR("Firmware upload failed - no space");
        return false; // error
    }

    LOG_INFO("Firmware uploading (size: %dKB) over %s. Please wait...", firmware_size / 1024, is_https ? "HTTPS" : "HTTP");

    Shell::loop_all(); // flush log buffers so latest messages are shown in console

    // we're about to start the upload, set the status so the Web System Monitor spots it
    EMSESP::system_.systemStatus(SYSTEM_STATUS::SYSTEM_STATUS_UPLOADING);

    // explicit chunked read loop instead of Update.writeStream():
    constexpr size_t   CHUNK_SIZE      = 1024;
    constexpr uint32_t READ_TIMEOUT_MS = 30000; // overall stall timeout per chunk
    uint8_t            buf[CHUNK_SIZE];
    size_t             total_read = 0;
    bool               magic_ok   = false;
    int                last_pct   = -1;

    while (total_read < (size_t)firmware_size) {
        // a cancel is signalled by the WebUI dropping the status below UPLOADING (back to NORMAL)
        // via the systemStatus action, which runs on the AsyncTCP task while we're blocked here
        if (EMSESP::system_.systemStatus() < SYSTEM_STATUS::SYSTEM_STATUS_UPLOADING) {
            LOG_WARNING("Firmware upload cancelled at %u of %d bytes", (unsigned)total_read, firmware_size);
            Update.abort();    // release the OTA partition handle so a later attempt can start cleanly
            ssl_client.stop(); // drop the connection
            saved_url.clear(); // prevent it from downloading again
            EMSESP::system_.systemStatus(SYSTEM_STATUS::SYSTEM_STATUS_NORMAL);
            Shell::loop_all(); // flush log buffers so the cancel message shows in the console
            return true;       // not an error - don't trigger the failure/reset path in emsesp.cpp
        }

        // wait for some data or for the connection to drop
        uint32_t wait_start = millis();
        while (!stream->available()) {
            if (!ssl_client.connected()) {
                break;
            }
            if (millis() - wait_start > READ_TIMEOUT_MS) {
                break;
            }
            // also bail out promptly if a cancel arrives mid-stall
            if (EMSESP::system_.systemStatus() < SYSTEM_STATUS::SYSTEM_STATUS_UPLOADING) {
                break;
            }
            delay(1);
        }

        if (!stream->available()) {
            // if the inner wait broke because of a cancel, loop back so the top-of-loop handler runs
            if (EMSESP::system_.systemStatus() < SYSTEM_STATUS::SYSTEM_STATUS_UPLOADING) {
                continue;
            }
            LOG_ERROR("Firmware upload failed - read stalled at %u of %d bytes", (unsigned)total_read, firmware_size);
            EMSESP::system_.systemStatus(SYSTEM_STATUS::SYSTEM_STATUS_ERROR_UPLOAD);
            return false;
        }

        size_t want = (size_t)firmware_size - total_read;
        if (want > CHUNK_SIZE) {
            want = CHUNK_SIZE;
        }

        size_t n = stream->readBytes(buf, want);
        if (n == 0) {
            LOG_ERROR("Firmware upload failed - read returned 0 at %u of %d bytes", (unsigned)total_read, firmware_size);
            EMSESP::system_.systemStatus(SYSTEM_STATUS::SYSTEM_STATUS_ERROR_UPLOAD);
            return false;
        }

        // verify the ESP image magic byte the very first time so we fail fast with a
        // clear message if the URL points at the wrong asset (HTML, archive, ...)
        if (!magic_ok) {
            if (buf[0] != 0xE9) {
                LOG_ERROR("Firmware upload failed - bad magic byte 0x%02X (expected 0xE9, not an ESP32 firmware image?)", buf[0]);
                EMSESP::system_.systemStatus(SYSTEM_STATUS::SYSTEM_STATUS_ERROR_UPLOAD);
                return false;
            }
            magic_ok = true;
        }

        if (Update.write(buf, n) != n) {
            LOG_ERROR("Firmware upload failed - flash write error at %u of %d bytes: %s", (unsigned)total_read, firmware_size, Update.errorString());
            EMSESP::system_.systemStatus(SYSTEM_STATUS::SYSTEM_STATUS_ERROR_UPLOAD);
            return false;
        }

        total_read += n;

        // update the WebUI status, but only when the percentage actually changes
        int pct = (int)(total_read * 100 / (size_t)firmware_size);
        if (pct != last_pct) {
            EMSESP::system_.systemStatus(SYSTEM_STATUS::SYSTEM_STATUS_UPLOADING + pct);
            last_pct = pct;
        }

        yield();
    }

    if (!Update.end(true)) {
        LOG_ERROR("Firmware upload failed - %s", Update.errorString());
        EMSESP::system_.systemStatus(SYSTEM_STATUS::SYSTEM_STATUS_ERROR_UPLOAD);
        return false; // error
    }

    saved_url.clear(); // prevent from downloading again
    LOG_INFO("Firmware uploaded successfully. Restarting...");
    EMSESP::system_.systemStatus(SYSTEM_STATUS::SYSTEM_STATUS_PENDING_RESTART);
#endif

    return true; // OK
}

// read command, e.g. read <deviceID> <type ID> [offset] [length] from console or API
// from Console use quotes so: call system read "<deviceID> <type ID> [offset] [length]"
bool System::readCommand(const char * data) {
    if (!data) {
        return false;
    }

    // extract <deviceID> <type ID> [offset] [length] from string
    char * p;
    char   value[11];

    // make a copy so we can iterate, max 15 chars (XX XXXX XX XX)
    char data_args[15];
    strlcpy(data_args, data, sizeof(data_args));

    uint8_t  device_id = 0; // is in hex
    uint16_t type_id   = 0; // is in hex
    uint8_t  length    = 0;
    uint8_t  offset    = 0;

    // first check deviceID
    if ((p = strtok(data_args, " ,"))) {               // delimiter comma or space
        strlcpy(value, p, sizeof(value));              // get string
        device_id = (uint8_t)Helpers::hextoint(value); // convert hex to int
        if (!EMSESP::valid_device(device_id)) {
            LOG_ERROR("Invalid device ID (0x%02X) in read command", device_id);
            return false; // invalid device
        }
    }

    // iterate until end
    uint8_t num_args = 0;
    while (p != 0) {
        if ((p = strtok(nullptr, " ,"))) {    // delimiter comma or space
            strlcpy(value, p, sizeof(value)); // get string
            if (num_args == 0) {
                type_id = (uint16_t)Helpers::hextoint(value); // convert hex to int
            } else if (num_args == 1) {
                offset = Helpers::atoint(value); // decimal
            } else if (num_args == 2) {
                length = Helpers::atoint(value); // decimal
            }
            num_args++;
        }
    }

    if (num_args == 0) {
        return false; // invalid number of arguments
    }

    EMSESP::send_read_request(type_id, device_id, offset, length, true);
    EMSESP::set_read_id(type_id);

    return true;
}

// system read command
bool System::command_read(const char * value, const int8_t) {
    return readCommand(value);
}

// set the system status code - SYSTEM_STATUS in system.h
// this is also used in the SystemMonitor.tsx WebUI to show the progress of the firmware upload, start at 100
void System::systemStatus(uint8_t status_code) {
    if (systemStatus_ != status_code) {
        systemStatus_ = status_code;
#ifdef EMSESP_DEBUG
        if (status_code < SYSTEM_STATUS::SYSTEM_STATUS_UPLOADING) {
            LOG_DEBUG("Setting System status code %d", status_code);
        }
#endif
    }
}

uint8_t System::systemStatus() {
    return systemStatus_;
}

// takes two arguments:
//  the first is the full range of pins to consider
//  the second is a string range of GPIOs to exclude, like "6-11, 1, 23, 24-48"
// returns a vector array of GPIOs that are valid for use
std::vector<uint8_t, AllocatorPSRAM<uint8_t>> System::string_range_to_vector(const std::string & range, const std::string & exclude) {
    std::vector<uint8_t, AllocatorPSRAM<uint8_t>> gpios;
    std::string::size_type                        pos  = 0;
    std::string::size_type                        prev = 0;

    auto process_part = [&gpios](std::string part) {
        // trim whitespace
        part.erase(0, part.find_first_not_of(" \t"));
        part.erase(part.find_last_not_of(" \t") + 1);

        // check if it's a range (contains '-')
        std::string::size_type dash_pos = part.find('-');
        if (dash_pos != std::string::npos) {
            // it's a range like "6-11"
            int start = std::stoi(part.substr(0, dash_pos));
            int end   = std::stoi(part.substr(dash_pos + 1));
            for (int i = start; i <= end; i++) {
                gpios.push_back(static_cast<uint8_t>(i));
            }
        } else {
            gpios.push_back(static_cast<uint8_t>(std::stoi(part)));
        }
    };

    while ((pos = range.find(',', prev)) != std::string::npos) {
        process_part(range.substr(prev, pos - prev));
        prev = pos + 1;
    }

    // handle the last part
    process_part(range.substr(prev));

    // if exclude list is provided, parse it and remove excluded GPIOs
    if (!exclude.empty()) {
        std::vector<uint8_t, AllocatorPSRAM<uint8_t>> exclude_gpios;
        pos  = 0;
        prev = 0;

        auto process_exclude = [&exclude_gpios](std::string part) {
            // trim whitespace
            part.erase(0, part.find_first_not_of(" \t"));
            part.erase(part.find_last_not_of(" \t") + 1);

            // check if it's a range (contains '-')
            std::string::size_type dash_pos = part.find('-');
            if (dash_pos != std::string::npos) {
                // it's a range like "6-11"
                int start = std::stoi(part.substr(0, dash_pos));
                int end   = std::stoi(part.substr(dash_pos + 1));
                for (int i = start; i <= end; i++) {
                    exclude_gpios.push_back(static_cast<uint8_t>(i));
                }
            } else {
                exclude_gpios.push_back(static_cast<uint8_t>(std::stoi(part)));
            }
        };

        while ((pos = exclude.find(',', prev)) != std::string::npos) {
            process_exclude(exclude.substr(prev, pos - prev));
            prev = pos + 1;
        }

        // handle the last part
        process_exclude(exclude.substr(prev));

        // remove excluded GPIOs from the main list
        gpios.erase(std::remove_if(gpios.begin(),
                                   gpios.end(),
                                   [&exclude_gpios](uint8_t gpio) { return std::find(exclude_gpios.begin(), exclude_gpios.end(), gpio) != exclude_gpios.end(); }),
                    gpios.end());
    }

    return gpios;
}

// initialize a list of valid GPIOs based on the ESP32 board
// string_to_vector() take two strings, the first is the range of GPIOs to use, the second is a list of GPIOs to exclude
// notes:
//   we always allow 0 (which is usually a strapping pin), because it's used to indicate whether EMS-ESP Dallas or the LED is disabled
//   we allow UART0, 1 and 2 as they are configurable
//   strapping pins are disabled as they can affect boot behaviour
//   we accept GPIOs that are fixed on BBQKees boards
//
void System::set_valid_system_gpios() {
    valid_system_gpios_.clear(); // reset system list
    used_gpios_.clear();         // reset used list

    // get free gpios based on board/platform type
#if CONFIG_IDF_TARGET_ESP32C3
    // https://docs.espressif.com/projects/esp-idf/en/stable/esp32c3/api-reference/peripherals/gpio.html
    // excluded:
    // GPIO2, GPIO8 - GPIO9 = strapping pins
    // GPIO12 - GPIO17 = used for SPI flash and PSRAM
    // GPIO18 - GPIO19 = USB-JTAG
    //
    // notes on what is allowed:
    // GPIO09 = button on BOARD_C3_MINI_V1
    // GPIO20 - GPIO21 = UART0, , no chip connected because native USB
    valid_system_gpios_ = string_range_to_vector("0-21", "2, 8, 12-17, 18-19");

#elif CONFIG_IDF_TARGET_ESP32S2
    // https://docs.espressif.com/projects/esp-idf/en/stable/esp32s2/api-reference/peripherals/gpio.html
    // excluded:
    // GPIO26 - GPIO32 = SPI flash and PSRAM
    // GPIO45 - GPIO46 = strapping pins
    // GPIO39 - GPIO42 = USB-JTAG
    // GPIO22 - GPIO25 = don't exist
    // GPIO19 - GPIO20 = USB
    //
    // notes on what is allowed:
    // GPIO43, GPIO44 = UART0, no chip connected because native USB
    valid_system_gpios_ = string_range_to_vector("0-46", "19, 20, 26-32, 45-46, 39-42, 22-25");

#elif CONFIG_IDF_TARGET_ESP32S3
    // https://docs.espressif.com/projects/esp-idf/en/stable/esp32s3/api-reference/peripherals/gpio.html
    // excluded:
    // GPIO3, GPIO45 - GPIO46 = strapping pins
    // GPIO26 - GPIO32 = SPI flash and PSRAM and not recommended
    // GPIO19 - GPIO20 = USB-JTAG
    // GPIO22 - GPIO25 = don't exist
    //
    // notes on what is allowed:
    // GPIO11 - GPIO19 = ADC analog input only pins
    // GPIO47 - GPIO48 = valid on a Wemos S3
    // GPIO8 = used by Liligo S3 board profile for Rx
    if (ESP.getPsramSize() > 0) {
        // GPIO33 - GPIO37 = Octal flash/PSRAM
        valid_system_gpios_ = string_range_to_vector("0-48", "3, 45-46, 26-32, 33-37, 19-20, 22-25");
    } else {
        valid_system_gpios_ = string_range_to_vector("0-48", "3, 45-46, 26-32, 19-20, 22-25");
    }

#elif CONFIG_IDF_TARGET_ESP32
    // https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/peripherals/gpio.html
    // excluded:
    // GPIO6 - GPIO11, GPIO16 - GPIO17 = used for SPI flash and PSRAM (dio mode only GPIO06-GPIO08, GPIO11)
    // GPIO20, GPIO24, GPIO28 - GPIO31 = don't exist
    // GPIO01, GPIO03 = UART0, normal connected to UART/USB chip
    //
    // notes on known boards:
    // boards have valid gpios depending on pinout and internal used gpios, see: `load_board_profile()`
    //
    // notes on BBQKees boards:
    // *** We block all free GPIOS in load_board_profile() ***
    // GPIO2, GPIO4, GPIO5, GPIO14 = used on BBQKees boards for either LED, Dallas or Rx
    // GPIO12, GPIO13, GPIO35 = BBQKees E32V2_2 internal system pins
    // GPIO33 = BBQKees E32V_2 unused internal NTC system sensor
    // GPIO36 = used on BBQKees boards for supply_voltage (E32V2.2)
    // GPIO39 = used on BBQKees boards for core_voltage (E32V2.2)
    //
    // notes on what is allowed with special functions:
    // GPIO01, GPIO03 = UART0, not always connected to uart/usb chip
    // GPIO12 - GPIO15 = JTAG, normally not used
    // GPIO00, GPIO05, GPIO12, GPIO15 = strapping pins, can be used with care
    // GPIO34, GPIO35, GPIO37 = input only
    // GPIO23 and GPIO18 are used by Ethernet, excluded later by eth config
    // GPIO00, GPIO02, GPIO04, GPIO12 - GPIO15, GPIO25 - GPIO27 = ADC2 (10 ch), used by WiFI-driver
    // GPIO32 - GPIO39 = ADC1 (8 ch), can always be used
    if (ESP.getPsramSize() > 0) {
        // remove SPI0/1 PSRAM pins GPIO16 (CS) and GPIO17 (CLK) from the list
        valid_system_gpios_ = string_range_to_vector("0-39", "6-11, 16, 17, 20, 24, 28-31");
    } else {
        valid_system_gpios_ = string_range_to_vector("0-39", "6-11, 20, 24, 28-31");
    }
#elif CONFIG_IDF_TARGET_ESP32C6
    // https://docs.espressif.com/projects/esp-idf/en/v5.5.3/esp32c6/api-reference/peripherals/gpio.html
    // 24-30 used for flash, 12-13 USB, 16-17 uart0
    valid_system_gpios_ = string_range_to_vector("0-30", "12-13, 16-17, 24-30");
#elif defined(EMSESP_STANDALONE)
    valid_system_gpios_ = string_range_to_vector("0-39");
#endif
    valid_system_gpios_.shrink_to_fit();
}

// check if a pin is valid ESP32 pin and if not already used, add to the used gpio list
// return false if not allowed or already used
bool System::add_gpio(uint8_t pin, const char * source_name) {
    // check if this is a valid user GPIO
    if (std::find(valid_system_gpios_.begin(), valid_system_gpios_.end(), pin) != valid_system_gpios_.end()) {
        // It's valid now check if it's already in the used list
        auto it = std::find_if(used_gpios_.begin(), used_gpios_.end(), [pin](const GpioUsage & usage) { return usage.pin == pin; });
        if (it != used_gpios_.end()) {
            LOG_WARNING("GPIO %d for %s is already in use by %s", pin, source_name, it->source.c_str());
            return false; // Pin is already used
        }
    } else {
        // not valid
        LOG_WARNING("GPIO %d for %s is not valid", pin, source_name);
        return false;
    }

    // remove the old pin, if exists from used list
    remove_gpio(pin);

    LOG_DEBUG("Adding GPIO %d for %s to used gpio list", pin, source_name);
    used_gpios_.push_back({pin, source_name}); // add to used list

    return true;
}

// remove a gpio from both valid and used lists
void System::remove_gpio(uint8_t pin, bool also_system) {
    auto it = std::find_if(used_gpios_.begin(), used_gpios_.end(), [pin](const GpioUsage & usage) { return usage.pin == pin; });
    if (it != used_gpios_.end()) {
        LOG_DEBUG("GPIO %d removed from used gpio list", pin);
        used_gpios_.erase(it);
    }

    if (also_system) {
        auto it_sys = std::find(valid_system_gpios_.begin(), valid_system_gpios_.end(), pin);
        if (it_sys != valid_system_gpios_.end()) {
            LOG_DEBUG("GPIO %d removed from valid gpio list", pin);
            valid_system_gpios_.erase(it_sys);
        }
    }
}

// remove a gpio that has 0 for disable
void System::remove_optional_gpio(uint8_t pin) {
    if (pin) {
        remove_gpio(pin, false);
    }
}

// set unused gpios to default state input high-Z
void System::reset_unused_gpios() {
    for (const auto & pin : valid_system_gpios_) {
        auto it = std::find_if(used_gpios_.begin(), used_gpios_.end(), [pin](const GpioUsage & usage) { return usage.pin == pin; });
        if (it == used_gpios_.end()) {
            LOG_DEBUG("reset pin %d", pin);
            pinMode(pin, INPUT);
        }
    }
}

// return a list of GPIO's available for use
std::vector<uint8_t> System::available_gpios() {
    std::vector<uint8_t> gpios;
    for (const auto & gpio : valid_system_gpios_) {
        if (std::find_if(used_gpios_.begin(), used_gpios_.end(), [gpio](const GpioUsage & usage) { return usage.pin == gpio; }) == used_gpios_.end()) {
            gpios.push_back(gpio); // didn't find it in used_gpios_, so it's available
        }
    }
    return gpios;
}

// make a snapshot of the current GPIOs
void System::make_snapshot_gpios(std::vector<int8_t> & u_gpios, std::vector<int8_t> & s_gpios) {
    for (const auto & usage : used_gpios_) {
        u_gpios.push_back(usage.pin);
    }
    for (const auto & gpio : valid_system_gpios_) {
        s_gpios.push_back(gpio);
    }
}

// restore the GPIOs from the snapshot
void System::restore_snapshot_gpios(std::vector<int8_t> & u_gpios, std::vector<int8_t> & s_gpios) {
    used_gpios_.clear();
    for (const auto & gpio : u_gpios) {
        used_gpios_.push_back({static_cast<uint8_t>(gpio), "restored"});
    }

    valid_system_gpios_.clear();
    for (const auto & gpio : s_gpios) {
        valid_system_gpios_.push_back(gpio);
    }
}

// show the contents of a directory in the LittleFS filesystem
void System::listDir([[maybe_unused]] const char * dirname, [[maybe_unused]] uint8_t levels) {
#if defined(EMSESP_DEBUG)
#ifndef EMSESP_STANDALONE

    File root = LittleFS.open(dirname);
    if (!root) {
        LOG_DEBUG("Failed to open directory %s", dirname);
        return;
    }
    if (!root.isDirectory()) {
        LOG_DEBUG("%s is not a directory", dirname);
        return;
    }

    LOG_DEBUG("(directory) %s", dirname);

    File file = root.openNextFile();
    while (file) {
        if (file.isDirectory()) {
            std::string line = std::string(file.name()) + "/";
            if (levels) {
                // prefix a / to the name to make it a full path
                listDir(("/" + String(file.name())).c_str(), levels - 1);
            }
        } else {
            std::string line = "  (file) " + std::string(file.name()) + " (" + std::to_string(file.size()) + " bytes)";
            LOG_DEBUG("%s", line.c_str());
        }
        file = root.openNextFile();
    }
#endif
#endif
}

} // namespace emsesp
