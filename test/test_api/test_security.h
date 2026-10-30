
#include <Arduino.h>
#include <unity.h>

#include "SecuritySettingsService.h"

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

void run_security_tests() {
    EMSESP::webCommandService.load_test_data(); // named commands used by executeCommand

    RUN_TEST(security_test_settings_commands_need_admin);
    RUN_TEST(security_test_api_without_token);
    RUN_TEST(security_test_action_admin_only);
    RUN_TEST(security_test_action_guest_allowed);
    RUN_TEST(security_test_write_endpoints_admin_only);
    RUN_TEST(security_test_log_settings);
    RUN_TEST(security_test_log_stream_needs_login);
    RUN_TEST(security_test_console_read_needs_admin);
}
