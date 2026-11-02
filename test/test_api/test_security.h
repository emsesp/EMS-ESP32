
#include <Arduino.h>
#include <unity.h>

#include "SecuritySettingsService.h"
#include "settings_secrets.h"
#include "password_hash.h"
#include "login_limiter.h"
#include "password_confirm.h"
#include "firmware_url.h"
#include "mqtt_tls.h"
#include "api_key.h"
#include "origin_check.h"
#include "host_check.h"
#include "shuntingYard.h"

// Permission tests. Run last, they change settings and identity.

static Authentication make_authentication(StandaloneIdentity identity) {
    switch (identity) {
    case StandaloneIdentity::NONE:
        return Authentication();
    case StandaloneIdentity::GUEST:
        return Authentication(User("guest", "guest", false));
    default:
        return Authentication(User("admin", "admin", true));
    }
}

// true if the route's permission rule lets this identity through
static bool route_allows(const char * path, WebRequestMethod method, StandaloneIdentity identity) {
    const StandaloneRoute * route = SecurityManager::findRoute(path, method);
    TEST_ASSERT_NOT_NULL_MESSAGE(route, path);
    Authentication authentication = make_authentication(identity);
    return route->predicate(authentication);
}

// calls the route the way SecurityManager::dispatchRest does, returns the HTTP status sent
static int call_route(const char * path, WebRequestMethod method, StandaloneIdentity identity, const char * body = "{}") {
    const StandaloneRoute * route = SecurityManager::findRoute(path, method);
    TEST_ASSERT_NOT_NULL_MESSAGE(route, path);

    AsyncWebServerRequest request;
    request.method(method);
    request.url(path);
    request._tempObject = nullptr;

    int            code           = 401;
    Authentication authentication = make_authentication(identity);
    if (route->predicate(authentication)) {
        SecuritySettingsService::identity(identity);
        JsonDocument doc;
        deserializeJson(doc, body);
        if (route->jsonHandler) {
            route->jsonHandler(&request, doc.as<JsonVariant>());
        } else if (route->plainHandler) {
            route->plainHandler(&request);
        }
        code = request.sentCode();
        SecuritySettingsService::identity(StandaloneIdentity::ADMIN);
    }

    return code;
}

static bool denied(int code) {
    return code == 401 || code == 403;
}

static void assert_admin_only_write(const char * path) {
    TEST_ASSERT_FALSE_MESSAGE(route_allows(path, HTTP_POST, StandaloneIdentity::NONE), path);
    TEST_ASSERT_FALSE_MESSAGE(route_allows(path, HTTP_POST, StandaloneIdentity::GUEST), path);
    TEST_ASSERT_TRUE_MESSAGE(route_allows(path, HTTP_POST, StandaloneIdentity::ADMIN), path);
}

static int call_action(StandaloneIdentity identity, const char * action, const char * param = nullptr) {
    char body[128];
    if (param) {
        snprintf(body, sizeof(body), "{\"action\":\"%s\",\"param\":\"%s\"}", action, param);
    } else {
        snprintf(body, sizeof(body), "{\"action\":\"%s\"}", action);
    }
    return call_route(EMSESP_ACTION_SERVICE_PATH, HTTP_POST, identity, body);
}

// system settings commands need admin, whichever way they are called
void security_test_settings_commands_need_admin() {
    const char * commands[] = {"/api/system/settings/showertimer",
                               "/api/system/settings/showeralert",
                               "/api/system/settings/hideled",
                               "/api/system/settings/analogenabled",
                               "/api/system/mqtt/enabled",
                               "/api/system/ap/enabled",
                               "/api/system/ntp/enabled",
                               "/api/system/syslog/enabled"};

    for (const char * command : commands) {
        JsonDocument doc_in;
        JsonDocument doc_out;
        JsonObject   input  = doc_in.to<JsonObject>();
        JsonObject   output = doc_out.to<JsonObject>();
        input["value"]      = "true";
        TEST_ASSERT_EQUAL_MESSAGE(CommandRet::NOT_ALLOWED, Command::process(command, false, input, output), command);
    }

    JsonDocument doc_in;
    JsonDocument doc_out;
    JsonObject   input  = doc_in.to<JsonObject>();
    JsonObject   output = doc_out.to<JsonObject>();
    input["value"]      = "false";
    TEST_ASSERT_EQUAL(CommandRet::OK, Command::process("/api/system/settings/showertimer", true, input, output));
}

void security_test_api_without_token() {
    SecuritySettingsService::identity(StandaloneIdentity::NONE);
    const char * response = call_url("/api/system/mqtt/enabled", "{\"value\":false}");
    SecuritySettingsService::identity(StandaloneIdentity::ADMIN);
    TEST_ASSERT_NOT_NULL(strstr(response, "authentication failed"));
}

// "Bypass Access Token authorization" lets calls without a token act like a guest, never an admin
void security_test_api_notoken_is_guest() {
    EMSESP::webSettingsService.update([](WebSettings & settings) {
        settings.notoken_api = true;
        return StateUpdateResult::CHANGED;
    });
    SecuritySettingsService::identity(StandaloneIdentity::NONE);

    std::string setting = call_url("/api/system/mqtt/enabled", "{\"value\":false}");
    std::string guest   = call_url("/api/commands/send_message", "{\"value\":\"1\"}");
    std::string admin   = call_url("/api/commands/fetch_values", "{\"value\":\"1\"}");

    SecuritySettingsService::identity(StandaloneIdentity::ADMIN);
    EMSESP::webSettingsService.update([](WebSettings & settings) {
        settings.notoken_api = false;
        return StateUpdateResult::CHANGED;
    });

    TEST_ASSERT_NOT_EQUAL(std::string::npos, setting.find("authentication failed"));
    TEST_ASSERT_EQUAL(std::string::npos, guest.find("authentication failed"));
    TEST_ASSERT_NOT_EQUAL(std::string::npos, admin.find("authentication failed"));
}

// browsers on other sites may only use /api when the CORS setting allows their origin
void security_test_origin_check() {
    TEST_ASSERT_TRUE(origin_allowed(nullptr, "ems-esp.local", false, ""));
    TEST_ASSERT_TRUE(origin_allowed("http://ems-esp.local", "ems-esp.local", false, ""));
    TEST_ASSERT_TRUE(origin_allowed("http://192.168.1.5:8080", "192.168.1.5:8080", false, ""));
    TEST_ASSERT_FALSE(origin_allowed("http://evil.example", "ems-esp.local", false, ""));
    TEST_ASSERT_FALSE(origin_allowed("null", "ems-esp.local", false, ""));
    TEST_ASSERT_FALSE(origin_allowed("http://evil.example", "ems-esp.local", true, ""));
    TEST_ASSERT_FALSE(origin_allowed("http://evil.example", "ems-esp.local", true, "http://homeassistant.local:8123"));
    TEST_ASSERT_TRUE(origin_allowed("http://homeassistant.local:8123", "ems-esp.local", true, "http://homeassistant.local:8123"));
    TEST_ASSERT_TRUE(origin_allowed("http://evil.example", "ems-esp.local", true, "*"));
}

// a browser may only reach the device by IP address, hostname, <hostname>.local or an extra allowed name
void security_test_host_check() {
    TEST_ASSERT_TRUE(host_allowed(nullptr, "ems-esp", ""));
    TEST_ASSERT_TRUE(host_allowed("192.168.1.5", "ems-esp", ""));
    TEST_ASSERT_TRUE(host_allowed("192.168.1.5:8080", "ems-esp", ""));
    TEST_ASSERT_TRUE(host_allowed("[fe80::1]:80", "ems-esp", ""));
    TEST_ASSERT_TRUE(host_allowed("ems-esp", "ems-esp", ""));
    TEST_ASSERT_TRUE(host_allowed("EMS-ESP.local", "ems-esp", ""));
    TEST_ASSERT_FALSE(host_allowed("evil.example", "ems-esp", ""));
    TEST_ASSERT_FALSE(host_allowed("ems-esp.evil.example", "ems-esp", ""));
    TEST_ASSERT_FALSE(host_allowed("ems-esp.fritz.box", "ems-esp", ""));
    TEST_ASSERT_TRUE(host_allowed("ems-esp.fritz.box", "ems-esp", "heating.example.com, ems-esp.fritz.box"));
    TEST_ASSERT_TRUE(host_allowed("heating.example.com:443", "ems-esp", "heating.example.com,ems-esp.fritz.box"));
    TEST_ASSERT_FALSE(host_allowed("example.com", "ems-esp", "heating.example.com"));
}

void security_test_action_admin_only() {
    TEST_ASSERT_TRUE(denied(call_action(StandaloneIdentity::GUEST, "export", "settings")));
    TEST_ASSERT_TRUE(denied(call_action(StandaloneIdentity::GUEST, "export", "systembackup")));
    TEST_ASSERT_TRUE(denied(call_action(StandaloneIdentity::GUEST, "setPartition", "app0")));
    TEST_ASSERT_TRUE(denied(call_action(StandaloneIdentity::GUEST, "upgradeImportantMessages", "3.9.0")));
    TEST_ASSERT_TRUE(denied(call_action(StandaloneIdentity::NONE, "getVersions")));

    TEST_ASSERT_EQUAL(200, call_action(StandaloneIdentity::ADMIN, "export", "settings"));
}

void security_test_action_guest_allowed() {
    TEST_ASSERT_EQUAL(200, call_action(StandaloneIdentity::GUEST, "getVersions"));
    TEST_ASSERT_EQUAL(200, call_action(StandaloneIdentity::GUEST, "getCustomSupport"));
    TEST_ASSERT_EQUAL(200, call_action(StandaloneIdentity::GUEST, "executeCommand", "send_message"));
    TEST_ASSERT_NOT_EQUAL(200, call_action(StandaloneIdentity::GUEST, "executeCommand", "no_such_command"));
}

// send_message is guest-allowed in the test data, fetch_values is not
void security_test_command_guest_flag() {
    TEST_ASSERT_TRUE(denied(call_action(StandaloneIdentity::GUEST, "executeCommand", "fetch_values")));
    TEST_ASSERT_EQUAL(200, call_action(StandaloneIdentity::ADMIN, "executeCommand", "fetch_values"));

    JsonDocument doc_out;
    JsonObject   output = doc_out.to<JsonObject>();
    JsonDocument doc_in;
    JsonObject   input = doc_in.to<JsonObject>();
    input["value"]     = "1";

    // /api: logged-in guest, anonymous caller, admin. Only admins can pass their own value
    TEST_ASSERT_EQUAL(CommandRet::NOT_ALLOWED, Command::process("/api/commands/send_message", false, input, output, true));
    TEST_ASSERT_EQUAL(CommandRet::NOT_ALLOWED, Command::process("/api/commands/fetch_values", false, input, output, true));
    TEST_ASSERT_EQUAL(CommandRet::NOT_ALLOWED, Command::process("/api/commands/send_message", false, input, output, false));
    TEST_ASSERT_EQUAL(CommandRet::OK, Command::process("/api/commands/fetch_values", true, input, output));

    // without a value a guest runs the command, it does not get the definition
    JsonDocument doc_empty;
    JsonObject   no_value = doc_empty.to<JsonObject>();
    JsonDocument doc_guest_out;
    JsonObject   guest_out = doc_guest_out.to<JsonObject>();
    TEST_ASSERT_NOT_EQUAL(CommandRet::NOT_ALLOWED, Command::process("/api/commands/send_message", false, no_value, guest_out, true));
    TEST_ASSERT_FALSE(guest_out["command"].is<const char *>());
}

// values sent by non-admins can't fetch URLs or read admin-only values through compute()
static uint8_t send_message(const char * value, const bool is_admin) {
    JsonDocument doc_in;
    JsonDocument doc_out;
    JsonObject   input  = doc_in.to<JsonObject>();
    JsonObject   output = doc_out.to<JsonObject>();
    input["value"]      = value;
    return Command::process("/api/system/message", is_admin, input, output, !is_admin);
}

void security_test_guest_value_no_url() {
    TEST_ASSERT_EQUAL(CommandRet::OK, send_message("\"hello\"", false));
    TEST_ASSERT_NOT_EQUAL(CommandRet::OK, send_message("{\"url\":\"http://192.168.1.1/\"}", false));
    TEST_ASSERT_NOT_EQUAL(CommandRet::OK, send_message("commands/fetch_values/value", false));
    TEST_ASSERT_EQUAL(CommandRet::OK, send_message("commands/fetch_values/value", true));
    TEST_ASSERT_TRUE(Command::caller_is_admin());
}

// render_value never writes past the buffer it is given, whatever the value
void security_test_render_value_bounded() {
    char small[8];
    TEST_ASSERT_EQUAL_STRING("1234567", Helpers::render_value(small, 123456789.25, 2));
    char buf[32];
    TEST_ASSERT_EQUAL_STRING("12345678901.5", Helpers::render_value(buf, 12345678901.5, 2));
    TEST_ASSERT_EQUAL_STRING("-20.56", Helpers::render_value(buf, -20.56, 2));
    TEST_ASSERT_EQUAL_STRING("0", Helpers::render_value(buf, 1e30, 2));
}

// large JSON numbers sent to /api are passed on in full
void security_test_api_large_numbers() {
    JsonDocument doc_in;
    JsonDocument doc_out;
    JsonObject   input  = doc_in.to<JsonObject>();
    JsonObject   output = doc_out.to<JsonObject>();
    input["value"]      = 12345678901.5;
    TEST_ASSERT_EQUAL(CommandRet::OK, Command::process("/api/system/message", true, input, output));
    TEST_ASSERT_EQUAL_STRING("12345678901.5", output["api_data"] | "");

    output.clear();
    input["value"] = INT32_MIN;
    TEST_ASSERT_EQUAL(CommandRet::OK, Command::process("/api/system/message", true, input, output));
    TEST_ASSERT_EQUAL_STRING("-2147483648", output["api_data"] | "");
}

// expressions that used to crash the device now fail cleanly
void security_test_compute_bad_input() {
    TEST_ASSERT_EQUAL_STRING("", emsesp::compute("5 % 0").c_str());
    TEST_ASSERT_EQUAL_STRING("", emsesp::compute("hex \"FFFFFFFFFFFF\"").c_str());
    TEST_ASSERT_EQUAL_STRING("255", emsesp::compute("hex \"ff\"").c_str());
    TEST_ASSERT_EQUAL_STRING("", emsesp::compute("1 + )").c_str());
    emsesp::compute(std::string(400, '1') + " + 1");
    emsesp::compute(std::string(300, '(') + "1" + std::string(300, ')'));
}

// a value with printf conversions is logged as text when the command fails
void security_test_value_not_a_format() {
    JsonDocument doc_in;
    JsonDocument doc_out;
    JsonObject   input  = doc_in.to<JsonObject>();
    JsonObject   output = doc_out.to<JsonObject>();
    input["value"]      = "%s%s%s%n%n%ln";
    TEST_ASSERT_EQUAL(CommandRet::ERROR, Command::process("/api/thermostat/hc1/seltemp", true, input, output));
    TEST_ASSERT_NOT_NULL(strstr(output["message"] | "", "%s%s%s%n%n%ln"));
}

// command definitions can hold credentials, only admins may read them
void security_test_command_definitions_admin_only() {
    const char * reads[] = {"/api/commands/values", "/api/commands/info", "/api/commands/entities", "/api/commands/send_message/value"};

    for (const char * path : reads) {
        JsonDocument doc_in;
        JsonDocument doc_out;
        JsonObject   input  = doc_in.to<JsonObject>();
        JsonObject   output = doc_out.to<JsonObject>();
        TEST_ASSERT_EQUAL_MESSAGE(CommandRet::NOT_ALLOWED, Command::process(path, false, input, output, false), path); // anonymous
        TEST_ASSERT_EQUAL_MESSAGE(CommandRet::NOT_ALLOWED, Command::process(path, false, input, output, true), path);  // guest
        TEST_ASSERT_EQUAL_MESSAGE(CommandRet::OK, Command::process(path, true, input, output), path);                  // admin
    }

    JsonDocument doc_in;
    JsonDocument doc_out;
    JsonObject   input  = doc_in.to<JsonObject>();
    JsonObject   output = doc_out.to<JsonObject>();
    TEST_ASSERT_EQUAL(CommandRet::NOT_ALLOWED, Command::process("/api/commands/send_message", false, input, output, false));
}

void security_test_write_endpoints_admin_only() {
    assert_admin_only_write(EMSESP_SCHEDULER_SERVICE_PATH);
    assert_admin_only_write(EMSESP_COMMANDS_SERVICE_PATH);
    assert_admin_only_write(EMSESP_CUSTOMENTITY_SERVICE_PATH);
    assert_admin_only_write(EMSESP_MODULES_SERVICE_PATH);
    assert_admin_only_write(EMSESP_WRITE_DEVICE_NAME_PATH);
    assert_admin_only_write(EMSESP_CUSTOMIZATION_ENTITIES_PATH);
}

void security_test_log_settings() {
    TEST_ASSERT_EQUAL(200, call_route(EMSESP_LOG_SETTINGS_PATH, HTTP_GET, StandaloneIdentity::GUEST));
    TEST_ASSERT_TRUE(denied(call_route(EMSESP_LOG_SETTINGS_PATH, HTTP_POST, StandaloneIdentity::GUEST, "{\"level\":7,\"max_messages\":25,\"compact\":false}")));
}

void security_test_log_stream_needs_login() {
    AsyncWebServerRequest request;
    request.method(HTTP_GET);
    request.url(EMSESP_EVENT_SOURCE_LOG_PATH);

    SecuritySettingsService::identity(StandaloneIdentity::NONE);
    bool anonymous = EMSESP::webLogService.events().filter(&request);
    SecuritySettingsService::identity(StandaloneIdentity::GUEST);
    bool guest = EMSESP::webLogService.events().filter(&request);
    SecuritySettingsService::identity(StandaloneIdentity::ADMIN);

    TEST_ASSERT_FALSE(anonymous);
    TEST_ASSERT_TRUE(guest);
}

// a network console (telnet) has user rights until su
void security_test_console_read_needs_admin() {
    auto network_shell = std::make_shared<EMSESPConsole>(application, stream, false);
    output_buffer[0]   = '\0';
    network_shell->invoke_command("read 08 18");
    TEST_ASSERT_NOT_NULL(strstr(output_buffer, "Command not found"));
}

// telnet users log in with a web user, su is only for the serial console
void security_test_console_su_network_refused() {
    auto network_shell = std::make_shared<EMSESPConsole>(application, stream, false);
    output_buffer[0]   = '\0';
    network_shell->invoke_command("su");
    TEST_ASSERT_NOT_NULL(strstr(output_buffer, "su: only on the serial console"));
    TEST_ASSERT_FALSE(network_shell->has_flags(CommandFlags::ADMIN));
}

// the serial console asks for the Super User password, it does not grant admin on su alone
void security_test_serial_su_needs_password() {
    auto serial_shell = std::make_shared<EMSESPConsole>(application, stream, true);
    output_buffer[0]  = '\0';
    serial_shell->invoke_command("su");
    TEST_ASSERT_FALSE(serial_shell->has_flags(CommandFlags::ADMIN));
    TEST_ASSERT_NULL(strstr(output_buffer, "su: only on the serial console"));
}

// the Web UI console is only for admins
void security_test_web_console_admin_only() {
    assert_admin_only_write(EMSESP_CONSOLE_PATH);
    TEST_ASSERT_EQUAL(200, call_route(EMSESP_CONSOLE_PATH, HTTP_POST, StandaloneIdentity::ADMIN, "{\"command\":\"show users\"}"));
    TEST_ASSERT_EQUAL(400, call_route(EMSESP_CONSOLE_PATH, HTTP_POST, StandaloneIdentity::ADMIN, "{}"));
}

// the Web UI console runs admin commands but can't answer a password prompt
void security_test_web_console_shell() {
    auto web_shell   = std::make_shared<EMSESPWebShell>(application, stream);
    output_buffer[0] = '\0';
    web_shell->run("read 08 18");
    TEST_ASSERT_NULL(strstr(output_buffer, "Command not found"));

    output_buffer[0] = '\0';
    web_shell->run("set admin password; show users");
    TEST_ASSERT_NOT_NULL(strstr(output_buffer, "needs interactive input"));
    TEST_ASSERT_NULL(strstr(output_buffer, "admin"));
}

// exported settings leave out passwords and keys
void security_test_backup_redacts_secrets() {
    JsonDocument doc;
    deserializeJson(doc,
                    R"({"jwt_secret":"s3cret","users":[{"username":"admin","password":"pw","admin":true}],"enabled":true})");
    JsonObject security = doc.as<JsonObject>();
    SettingsSecrets::redact("Security", security);
    TEST_ASSERT_FALSE(security["jwt_secret"].is<JsonVariant>());
    TEST_ASSERT_FALSE(security["users"][0]["password"].is<JsonVariant>());
    TEST_ASSERT_EQUAL_STRING("admin", security["users"][0]["username"]);

    deserializeJson(doc, R"({"ssid":"home","password":"wifi-pw"})");
    JsonObject network = doc.as<JsonObject>();
    SettingsSecrets::redact("Network", network);
    TEST_ASSERT_FALSE(network["password"].is<JsonVariant>());
    TEST_ASSERT_EQUAL_STRING("home", network["ssid"]);

    deserializeJson(doc, R"({"modules":[{"key":"m1","license":"L1","enabled":true}]})");
    JsonObject modules = doc.as<JsonObject>();
    SettingsSecrets::redact("Modules", modules);
    TEST_ASSERT_FALSE(modules["modules"][0]["license"].is<JsonVariant>());

    deserializeJson(doc, R"({"email_pass":"mail-pw","locale":"en"})");
    JsonObject settings = doc.as<JsonObject>();
    SettingsSecrets::redact("Settings", settings);
    TEST_ASSERT_FALSE(settings["email_pass"].is<JsonVariant>());
}

// restoring a backup without secrets keeps the ones on the device, a backup with secrets overwrites them
void security_test_restore_merges_secrets() {
    JsonDocument current;
    deserializeJson(current,
                    R"({"jwt_secret":"device","users":[{"username":"admin","password":"device-pw","admin":true}]})");

    JsonDocument restored;
    deserializeJson(restored,
                    R"({"users":[{"username":"admin","admin":true},{"username":"newuser","admin":false}]})");
    SettingsSecrets::merge("Security", restored.as<JsonObject>(), current.as<JsonObjectConst>());
    TEST_ASSERT_EQUAL_STRING("device", restored["jwt_secret"]);
    TEST_ASSERT_EQUAL_STRING("device-pw", restored["users"][0]["password"]);
    TEST_ASSERT_EQUAL_STRING("", restored["users"][1]["password"]); // can't log in until an admin sets one

    // older backups and full backups carry their own secrets
    deserializeJson(restored, R"({"jwt_secret":"backup","users":[{"username":"admin","password":"backup-pw","admin":true}]})");
    SettingsSecrets::merge("Security", restored.as<JsonObject>(), current.as<JsonObjectConst>());
    TEST_ASSERT_EQUAL_STRING("backup", restored["jwt_secret"]);
    TEST_ASSERT_EQUAL_STRING("backup-pw", restored["users"][0]["password"]);

    deserializeJson(current, R"({"modules":[{"key":"m1","license":"L1"}]})");
    deserializeJson(restored, R"({"modules":[{"key":"m1","enabled":true}]})");
    SettingsSecrets::merge("Modules", restored.as<JsonObject>(), current.as<JsonObjectConst>());
    TEST_ASSERT_EQUAL_STRING("L1", restored["modules"][0]["license"]);
}

// the device signing key and its flag stay on the device, never in a backup
void security_test_private_nvs_keys() {
    TEST_ASSERT_FALSE(SettingsSecrets::is_private_nvs_key("boiler_x"));
    TEST_ASSERT_TRUE(SettingsSecrets::is_private_nvs_key(EMSESP_NVS_JWT_KEY));
    TEST_ASSERT_TRUE(SettingsSecrets::is_private_nvs_key(EMSESP_NVS_TOKENS_RESET));
}

static String hex(const uint8_t * data, size_t len) {
    String out;
    char   byte[3];
    for (size_t i = 0; i < len; i++) {
        snprintf(byte, sizeof(byte), "%02x", data[i]);
        out += byte;
    }
    return out;
}

// PBKDF2-HMAC-SHA256 test vectors, P="password" S="salt"
void security_test_pbkdf2_vectors() {
    const uint8_t salt[] = {'s', 'a', 'l', 't'};
    uint8_t       out[32];
    PasswordHash::pbkdf2("password", salt, sizeof(salt), 1, out);
    TEST_ASSERT_EQUAL_STRING("120fb6cffcf8b32c43e7225256c4f837a86548c92ccc35480805987cb70be17b", hex(out, sizeof(out)).c_str());
    PasswordHash::pbkdf2("password", salt, sizeof(salt), 4096, out);
    TEST_ASSERT_EQUAL_STRING("c5e478d59288c841aa530db6845c4c8d962893a001ce4e11a4963873aa98134a", hex(out, sizeof(out)).c_str());
}

void security_test_password_hash() {
    String stored = PasswordHash::hash("s3cret");
    TEST_ASSERT_TRUE(stored.startsWith("$pbkdf2-sha256$10000$"));
    TEST_ASSERT_TRUE(PasswordHash::is_hashed(stored));
    TEST_ASSERT_TRUE(PasswordHash::verify("s3cret", stored));
    TEST_ASSERT_FALSE(PasswordHash::verify("s3creT", stored));
    TEST_ASSERT_FALSE(PasswordHash::verify("", stored));
    TEST_ASSERT_NOT_EQUAL(0, strcmp(stored.c_str(), PasswordHash::hash("s3cret").c_str())); // random salt

    // a damaged hash is treated as plain text, so it only matches itself
    std::string damaged = stored.c_str();
    damaged.back()      = damaged.back() == '0' ? '1' : '0';
    TEST_ASSERT_FALSE(PasswordHash::verify("s3cret", damaged.c_str()));
    damaged.back() = 'x';
    TEST_ASSERT_FALSE(PasswordHash::is_hashed(damaged.c_str()));
    TEST_ASSERT_FALSE(PasswordHash::is_hashed("$pbkdf2-sha256$0$00$00"));
    TEST_ASSERT_FALSE(PasswordHash::is_hashed(("$pbkdf2-sha256$999999999" + std::string(stored.c_str() + 20)).c_str()));

    // empty never matches, plain text matches until it's hashed at boot
    TEST_ASSERT_FALSE(PasswordHash::verify("", ""));
    TEST_ASSERT_TRUE(PasswordHash::verify("admin", "admin"));
    TEST_ASSERT_FALSE(PasswordHash::verify("admin", "admin2"));
}

// failed logins from one source lock it out with a growing wait, others are not affected
void security_test_login_limiter() {
    LoginLimiter limiter;
    uint32_t     now = 1000;
    for (uint8_t i = 1; i < LoginLimiter::FREE_ATTEMPTS; i++) {
        limiter.failed("10.0.0.1", now);
        TEST_ASSERT_EQUAL_UINT32(0, limiter.retry_after("10.0.0.1", now));
    }
    limiter.failed("10.0.0.1", now);
    TEST_ASSERT_EQUAL_UINT32(LoginLimiter::FIRST_LOCKOUT_MS, limiter.retry_after("10.0.0.1", now));
    TEST_ASSERT_EQUAL_UINT32(0, limiter.retry_after("10.0.0.2", now));

    now += LoginLimiter::FIRST_LOCKOUT_MS;
    TEST_ASSERT_EQUAL_UINT32(0, limiter.retry_after("10.0.0.1", now));
    limiter.failed("10.0.0.1", now);
    TEST_ASSERT_EQUAL_UINT32(LoginLimiter::FIRST_LOCKOUT_MS * 2, limiter.retry_after("10.0.0.1", now));

    // the wait stops growing
    for (int i = 0; i < 20; i++) {
        limiter.failed("10.0.0.1", now);
    }
    TEST_ASSERT_EQUAL_UINT32(LoginLimiter::MAX_LOCKOUT_MS, limiter.retry_after("10.0.0.1", now));

    // forgotten after a while without failures, and on a successful login
    now += LoginLimiter::FORGET_MS;
    limiter.failed("10.0.0.1", now);
    TEST_ASSERT_EQUAL_UINT32(0, limiter.retry_after("10.0.0.1", now));
    limiter.succeeded("10.0.0.1");
    TEST_ASSERT_EQUAL_UINT32(0, limiter.retry_after("10.0.0.1", now));

    // across the millis() wrap around
    LoginLimiter wrap;
    now = 0xFFFFFFFF - 1000;
    for (uint8_t i = 0; i < LoginLimiter::FREE_ATTEMPTS; i++) {
        wrap.failed("10.0.0.1", now);
    }
    TEST_ASSERT_EQUAL_UINT32(LoginLimiter::FIRST_LOCKOUT_MS - 2000, wrap.retry_after("10.0.0.1", now + 2000));
}

// many failures from different sources lock out everyone for a while
void security_test_login_limiter_global() {
    LoginLimiter limiter;
    char         source[16];
    for (uint8_t i = 0; i < LoginLimiter::GLOBAL_FAILURES; i++) {
        snprintf(source, sizeof(source), "10.0.1.%u", i);
        limiter.failed(source, 5000);
    }
    TEST_ASSERT_EQUAL_UINT32(LoginLimiter::GLOBAL_LOCKOUT_MS, limiter.retry_after("10.0.2.1", 5000));
    TEST_ASSERT_EQUAL_UINT32(0, limiter.retry_after("10.0.2.1", 5000 + LoginLimiter::GLOBAL_LOCKOUT_MS));
}

// a console login with a web user, admins get admin rights
void security_test_console_login() {
    auto guest_shell = std::make_shared<EMSESPConsole>(application, stream, false);
    TEST_ASSERT_TRUE(guest_shell->login("guest", "guest") == EMSESPShell::LoginResult::OK);
    TEST_ASSERT_FALSE(guest_shell->has_flags(CommandFlags::ADMIN));

    auto admin_shell = std::make_shared<EMSESPConsole>(application, stream, false);
    TEST_ASSERT_TRUE(admin_shell->login("admin", "wrong") == EMSESPShell::LoginResult::FAILED);
    TEST_ASSERT_FALSE(admin_shell->has_flags(CommandFlags::ADMIN));
    TEST_ASSERT_TRUE(admin_shell->login("admin", "admin") == EMSESPShell::LoginResult::OK);
    TEST_ASSERT_TRUE(admin_shell->has_flags(CommandFlags::ADMIN));
}

// once locked out, even the right password is refused
void security_test_console_login_lockout() {
    auto shell = std::make_shared<EMSESPConsole>(application, stream, false);
    for (uint8_t i = 0; i < LoginLimiter::FREE_ATTEMPTS; i++) {
        shell->login("admin", "wrong");
    }
    TEST_ASSERT_TRUE(shell->login("admin", "admin") == EMSESPShell::LoginResult::LOCKED);
    TEST_ASSERT_FALSE(shell->has_flags(CommandFlags::ADMIN));
    LoginLimiter::instance().succeeded(shell->console_name().c_str());
}

// API keys are random, identified by their first digits and stored only as a hash
void security_test_api_key() {
    const String key = ApiKey::generate();
    TEST_ASSERT_TRUE(key.startsWith("emsesp_"));
    TEST_ASSERT_EQUAL(7 + 40, key.length());
    TEST_ASSERT_TRUE(ApiKey::is_key(key.c_str()));
    TEST_ASSERT_EQUAL(8, ApiKey::id(key.c_str()).length());
    TEST_ASSERT_NOT_EQUAL(0, strcmp(key.c_str(), ApiKey::generate().c_str()));

    const String hash = ApiKey::hash(key.c_str());
    TEST_ASSERT_EQUAL(64, hash.length());
    TEST_ASSERT_TRUE(ApiKey::matches(key.c_str(), hash));
    TEST_ASSERT_FALSE(ApiKey::matches(key.c_str(), ""));

    std::string other = key.c_str();
    other.back()      = other.back() == '0' ? '1' : '0';
    TEST_ASSERT_FALSE(ApiKey::matches(other.c_str(), hash));

    TEST_ASSERT_FALSE(ApiKey::is_key("emsesp_123"));
    TEST_ASSERT_FALSE(ApiKey::is_key("eyJhbGciOiJIUzI1NiJ9.e30.abc")); // a JWT
    TEST_ASSERT_FALSE(ApiKey::is_key(("emsesp_" + std::string(40, 'G')).c_str()));
    TEST_ASSERT_TRUE(ApiKey::id("emsesp_123").isEmpty());
}

// backups leave out the key hashes, restoring keeps the ones on the device
void security_test_backup_api_keys() {
    JsonDocument doc;
    deserializeJson(doc, R"({"api_keys":[{"id":"a1b2c3d4","label":"HA","write":true,"hash":"abc"}]})");
    SettingsSecrets::redact("Security", doc.as<JsonObject>());
    TEST_ASSERT_FALSE(doc["api_keys"][0]["hash"].is<JsonVariant>());

    JsonDocument current;
    deserializeJson(current, R"({"api_keys":[{"id":"a1b2c3d4","hash":"device"}]})");
    SettingsSecrets::merge("Security", doc.as<JsonObject>(), current.as<JsonObjectConst>());
    TEST_ASSERT_EQUAL_STRING("device", doc["api_keys"][0]["hash"]);
}

// the password entered again counts for this sign-in only, and for a few minutes
void security_test_password_confirm() {
    PasswordConfirm confirm;
    const char *    token = "header.payload.signature1";
    uint32_t        now   = 1000;

    TEST_ASSERT_FALSE(confirm.confirmed(token, now));
    confirm.confirm(token, now);
    TEST_ASSERT_TRUE(confirm.confirmed(token, now));
    TEST_ASSERT_TRUE(confirm.confirmed(token, now + PasswordConfirm::DURATION_MS - 1));
    TEST_ASSERT_FALSE(confirm.confirmed(token, now + PasswordConfirm::DURATION_MS));
    TEST_ASSERT_FALSE(confirm.confirmed("header.payload.signature2", now));
    TEST_ASSERT_FALSE(confirm.confirmed("nodot", now));
    TEST_ASSERT_FALSE(confirm.confirmed(nullptr, now));

    // a new sign-in pushes out the one that expires first
    char other[32];
    for (size_t i = 0; i < PasswordConfirm::MAX_SESSIONS; i++) {
        snprintf(other, sizeof(other), "h.p.other%u", static_cast<unsigned>(i));
        confirm.confirm(other, now + 1 + i);
    }
    TEST_ASSERT_FALSE(confirm.confirmed(token, now + 10));
    TEST_ASSERT_TRUE(confirm.confirmed("h.p.other0", now + 10));

    // across the millis() wrap around
    PasswordConfirm wrap;
    now = 0xFFFFFFFF - 1000;
    wrap.confirm(token, now);
    TEST_ASSERT_TRUE(wrap.confirmed(token, now + 2000));
    TEST_ASSERT_FALSE(wrap.confirmed(token, now + PasswordConfirm::DURATION_MS));

    confirm.clear();
    TEST_ASSERT_FALSE(confirm.confirmed("h.p.other0", now + 10));
}

// firmware changes from a Web UI session need the password entered again, other actions don't
void security_test_password_required_actions() {
    SecuritySettingsService::password_confirmed(false);
    TEST_ASSERT_EQUAL(SecurityManager::PASSWORD_REQUIRED, call_action(StandaloneIdentity::ADMIN, "setPartition", "app0"));
    TEST_ASSERT_EQUAL(SecurityManager::PASSWORD_REQUIRED,
                      call_action(StandaloneIdentity::ADMIN, "uploadURL", "https://github.com/emsesp/EMS-ESP32/releases/download/latest/x.bin"));
    TEST_ASSERT_EQUAL(SecurityManager::PASSWORD_REQUIRED,
                      call_route(EMSESP_ACTION_SERVICE_PATH, HTTP_POST, StandaloneIdentity::ADMIN, R"({"action":"export","param":"systembackup","secrets":true})"));
    TEST_ASSERT_EQUAL(200, call_action(StandaloneIdentity::ADMIN, "export", "systembackup"));
    TEST_ASSERT_EQUAL(200, call_action(StandaloneIdentity::ADMIN, "getVersions"));

    // a factory reset, only from a session. API keys, MQTT and the consoles don't have a password to enter
    JsonDocument doc_in;
    JsonDocument doc_out;
    JsonObject   input  = doc_in.to<JsonObject>();
    JsonObject   output = doc_out.to<JsonObject>();
    TEST_ASSERT_EQUAL(CommandRet::PASSWORD_REQUIRED, Command::process("/api/system/format", true, input, output, true, false));
    input["device"] = "system";
    input["cmd"]    = "format";
    TEST_ASSERT_EQUAL(CommandRet::PASSWORD_REQUIRED, Command::process("/api", true, input, output, true, false));

    SecuritySettingsService::password_confirmed(true);
}

// only EMS-ESP releases on GitHub can be installed from a URL
void security_test_firmware_url() {
    TEST_ASSERT_TRUE(FirmwareURL::allowed("https://github.com/emsesp/EMS-ESP32/releases/download/latest/EMS-ESP-3_9_0-ESP32S3-16MB+.bin"));
    TEST_ASSERT_TRUE(FirmwareURL::allowed("https://GitHub.com/emsesp/EMS-ESP32/releases/download/v3.9.0/EMS-ESP-3_9_0-ESP32-16MB.bin"));
    TEST_ASSERT_FALSE(FirmwareURL::allowed("http://github.com/emsesp/EMS-ESP32/releases/download/latest/x.bin"));
    TEST_ASSERT_FALSE(FirmwareURL::allowed("https://github.com/someone/EMS-ESP32/releases/download/latest/x.bin"));
    TEST_ASSERT_FALSE(FirmwareURL::allowed("https://github.com/emsesp/EMS-ESP32/releases/../../someone/repo/releases/x.bin"));
    TEST_ASSERT_FALSE(FirmwareURL::allowed("https://github.com/emsesp/EMS-ESP32/releases/%2e%2e/x.bin"));
    TEST_ASSERT_FALSE(FirmwareURL::allowed("https://github.com/emsesp/EMS-ESP32/releases/x.bin HTTP/1.0\r\nHost: evil"));
    TEST_ASSERT_FALSE(FirmwareURL::allowed("https://github.com@evil.com/emsesp/EMS-ESP32/releases/x.bin"));
    TEST_ASSERT_FALSE(FirmwareURL::allowed("https://github.com.evil.com/emsesp/EMS-ESP32/releases/x.bin"));
    TEST_ASSERT_FALSE(FirmwareURL::allowed("https://github.com/emsesp/EMS-ESP32/releases/"));
    TEST_ASSERT_FALSE(FirmwareURL::allowed("https://example.com/firmware.bin"));
    TEST_ASSERT_FALSE(FirmwareURL::allowed(""));
    TEST_ASSERT_FALSE(FirmwareURL::allowed(nullptr));

    TEST_ASSERT_TRUE(FirmwareURL::redirect_allowed("https://release-assets.githubusercontent.com/github-production-release-asset/1?sig=abc"));
    TEST_ASSERT_TRUE(FirmwareURL::redirect_allowed("https://objects.githubusercontent.com/github-production-release-asset-2e65be/1"));
    TEST_ASSERT_TRUE(FirmwareURL::redirect_allowed("https://github.com/emsesp/EMS-ESP32/releases/download/v3.9.0/x.bin"));
    TEST_ASSERT_FALSE(FirmwareURL::redirect_allowed("http://release-assets.githubusercontent.com/x"));
    TEST_ASSERT_FALSE(FirmwareURL::redirect_allowed("https://release-assets.githubusercontent.com.evil.com/x"));
    TEST_ASSERT_FALSE(FirmwareURL::redirect_allowed("https://evil.com/x.bin"));
    TEST_ASSERT_FALSE(FirmwareURL::redirect_allowed("https://github.com/someone/repo/releases/download/x.bin"));
}

// a number outside the entity's min and max is refused, whichever way it's written
void security_test_write_range() {
    JsonDocument doc_in;
    JsonDocument doc_out;
    JsonObject   input  = doc_in.to<JsonObject>();
    JsonObject   output = doc_out.to<JsonObject>();

    input["value"] = "95"; // selflowtemp is 0 to 90
    TEST_ASSERT_EQUAL(CommandRet::INVALID, Command::process("/api/boiler/selflowtemp", true, input, output));
    TEST_ASSERT_NOT_NULL(strstr(output["message"] | "", "out of range 0 to 90"));
    input["value"] = "-1";
    TEST_ASSERT_EQUAL(CommandRet::INVALID, Command::process("/api/boiler/selflowtemp", true, input, output));
    input["value"] = "60";
    TEST_ASSERT_EQUAL(CommandRet::OK, Command::process("/api/boiler/selflowtemp", true, input, output));
}

// read-only mode also stops raw write telegrams, raw reads still work
void security_test_readonly_raw() {
    EMSESP::system_.readonly_mode(true);
    TEST_ASSERT_FALSE(EMSESP::txservice_.send_raw("0B 08 1A 00 3C"));
    TEST_ASSERT_TRUE(EMSESP::txservice_.send_raw("0B 88 18 00 20"));
    EMSESP::system_.readonly_mode(false);
    TEST_ASSERT_TRUE(EMSESP::txservice_.send_raw("0B 08 1A 00 3C"));
}

// the write log names the caller, nested callers keep theirs
void security_test_caller_source() {
    TEST_ASSERT_NULL(Command::caller_source());
    {
        Command::CallerSource outer("api, HA");
        TEST_ASSERT_EQUAL_STRING("api, HA", Command::caller_source());
        {
            Command::CallerSource inner("mqtt");
            TEST_ASSERT_EQUAL_STRING("mqtt", Command::caller_source());
        }
        TEST_ASSERT_EQUAL_STRING("api, HA", Command::caller_source());
    }
    TEST_ASSERT_NULL(Command::caller_source());
}

// what the MQTT rootCA setting holds decides how the broker is checked
void security_test_mqtt_tls() {
    char fp[MqttTls::FINGERPRINT_LEN + 1];
    TEST_ASSERT_TRUE(MqttTls::parse_fingerprint("AB:CD:EF:01:23:45:67:89:AB:CD:EF:01:23:45:67:89:AB:CD:EF:01", fp));
    TEST_ASSERT_EQUAL_STRING("abcdef0123456789abcdef0123456789abcdef01", fp);
    TEST_ASSERT_TRUE(MqttTls::parse_fingerprint("abcdef0123456789abcdef0123456789abcdef01", fp));
    TEST_ASSERT_FALSE(MqttTls::parse_fingerprint("abcdef0123456789abcdef0123456789abcdef0"));   // too short
    TEST_ASSERT_FALSE(MqttTls::parse_fingerprint("abcdef0123456789abcdef0123456789abcdef0123")); // SHA-256 or too long
    TEST_ASSERT_FALSE(MqttTls::parse_fingerprint("abcdef0123456789abcdef0123456789abcdef0g"));
    TEST_ASSERT_FALSE(MqttTls::parse_fingerprint("a:bcdef0123456789abcdef0123456789abcdef01"));
    TEST_ASSERT_FALSE(MqttTls::parse_fingerprint("ab::cdef0123456789abcdef0123456789abcdef01"));
    TEST_ASSERT_FALSE(MqttTls::parse_fingerprint("abcdef0123456789abcdef0123456789abcdef01:"));

    TEST_ASSERT_TRUE(MqttTls::check("") == MqttTls::Check::NONE);
    TEST_ASSERT_TRUE(MqttTls::check("insecure") == MqttTls::Check::NONE);
    TEST_ASSERT_TRUE(MqttTls::check("abcdef0123456789abcdef0123456789abcdef01") == MqttTls::Check::FINGERPRINT);
    TEST_ASSERT_TRUE(MqttTls::check("MIIDdzCCAl+gAwIBAgIEAgAAuTANBgkqhkiG9w0BAQUFADBaMQswCQYDVQQGEwJJ") == MqttTls::Check::CA);
}

void run_security_tests() {
    EMSESP::webCommandService.load_test_data(); // named commands used by executeCommand

    RUN_TEST(security_test_settings_commands_need_admin);
    RUN_TEST(security_test_api_without_token);
    RUN_TEST(security_test_api_notoken_is_guest);
    RUN_TEST(security_test_origin_check);
    RUN_TEST(security_test_host_check);
    RUN_TEST(security_test_action_admin_only);
    RUN_TEST(security_test_action_guest_allowed);
    RUN_TEST(security_test_command_guest_flag);
    RUN_TEST(security_test_guest_value_no_url);
    RUN_TEST(security_test_render_value_bounded);
    RUN_TEST(security_test_api_large_numbers);
    RUN_TEST(security_test_compute_bad_input);
    RUN_TEST(security_test_value_not_a_format);
    RUN_TEST(security_test_command_definitions_admin_only);
    RUN_TEST(security_test_write_endpoints_admin_only);
    RUN_TEST(security_test_log_settings);
    RUN_TEST(security_test_log_stream_needs_login);
    RUN_TEST(security_test_console_read_needs_admin);
    RUN_TEST(security_test_console_su_network_refused);
    RUN_TEST(security_test_serial_su_needs_password);
    RUN_TEST(security_test_web_console_admin_only);
    RUN_TEST(security_test_web_console_shell);
    RUN_TEST(security_test_backup_redacts_secrets);
    RUN_TEST(security_test_restore_merges_secrets);
    RUN_TEST(security_test_private_nvs_keys);
    RUN_TEST(security_test_pbkdf2_vectors);
    RUN_TEST(security_test_password_hash);
    RUN_TEST(security_test_login_limiter);
    RUN_TEST(security_test_login_limiter_global);
    RUN_TEST(security_test_console_login);
    RUN_TEST(security_test_console_login_lockout);
    RUN_TEST(security_test_api_key);
    RUN_TEST(security_test_backup_api_keys);
    RUN_TEST(security_test_password_confirm);
    RUN_TEST(security_test_password_required_actions);
    RUN_TEST(security_test_firmware_url);
    RUN_TEST(security_test_write_range);
    RUN_TEST(security_test_readonly_raw);
    RUN_TEST(security_test_caller_source);
    RUN_TEST(security_test_mqtt_tls);
}
