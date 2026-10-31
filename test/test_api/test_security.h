
#include <Arduino.h>
#include <unity.h>

#include "SecuritySettingsService.h"
#include "settings_secrets.h"

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
    input["value"]     = "1"; // without a value /api returns the command's info instead of running it

    // /api: logged-in guest, anonymous caller, admin
    TEST_ASSERT_EQUAL(CommandRet::OK, Command::process("/api/commands/send_message", false, input, output, true));
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

// the factory Super User password is public, so su over the network must be refused while it is set
void security_test_console_su_default_password() {
    auto network_shell = std::make_shared<EMSESPConsole>(application, stream, false);
    output_buffer[0]   = '\0';
    network_shell->invoke_command("su");
    TEST_ASSERT_NOT_NULL(strstr(output_buffer, "change the default Super User password"));
    TEST_ASSERT_FALSE(network_shell->has_flags(CommandFlags::ADMIN));
}

// the serial console asks for the Super User password, it does not grant admin on su alone
void security_test_serial_su_needs_password() {
    auto serial_shell = std::make_shared<EMSESPConsole>(application, stream, true);
    output_buffer[0]  = '\0';
    serial_shell->invoke_command("su");
    TEST_ASSERT_FALSE(serial_shell->has_flags(CommandFlags::ADMIN));
    TEST_ASSERT_NULL(strstr(output_buffer, "change the default Super User password"));
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

void run_security_tests() {
    EMSESP::webCommandService.load_test_data(); // named commands used by executeCommand

    RUN_TEST(security_test_settings_commands_need_admin);
    RUN_TEST(security_test_api_without_token);
    RUN_TEST(security_test_action_admin_only);
    RUN_TEST(security_test_action_guest_allowed);
    RUN_TEST(security_test_command_guest_flag);
    RUN_TEST(security_test_command_definitions_admin_only);
    RUN_TEST(security_test_write_endpoints_admin_only);
    RUN_TEST(security_test_log_settings);
    RUN_TEST(security_test_log_stream_needs_login);
    RUN_TEST(security_test_console_read_needs_admin);
    RUN_TEST(security_test_console_su_default_password);
    RUN_TEST(security_test_serial_su_needs_password);
    RUN_TEST(security_test_backup_redacts_secrets);
    RUN_TEST(security_test_restore_merges_secrets);
    RUN_TEST(security_test_private_nvs_keys);
}
