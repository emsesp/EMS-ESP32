// Fuzzes Command::process (the /api and MQTT entry point) and compute() (the expression evaluator)
// with random and mutated input. Built with AddressSanitizer and UBSan in the native-fuzz env, so
// any out-of-bounds access or undefined behaviour stops the run with a report.
// Run with `pio run -e native-fuzz -t exec`. FUZZ_ITERATIONS and FUZZ_SEED override the defaults.

#include <sanitizer/common_interface_defs.h>

#include <cfloat>
#include <cmath>
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>

#include "core/shuntingYard.h"

namespace fuzz {

static uint64_t rng_state = 1;

static uint64_t next() {
    rng_state ^= rng_state << 13;
    rng_state ^= rng_state >> 7;
    rng_state ^= rng_state << 17;
    return rng_state;
}

static uint32_t below(uint32_t n) {
    return n ? next() % n : 0;
}

static bool chance(uint32_t percent) {
    return below(100) < percent;
}

template <size_t N>
static const char * pick(const char * const (&list)[N]) {
    return list[below(N)];
}

static const char * const devices[] = {"boiler",
                                       "thermostat",
                                       "mixer",
                                       "solar",
                                       "heatpump",
                                       "system",
                                       "custom",
                                       "scheduler",
                                       "commands",
                                       "temperaturesensor",
                                       "analogsensor",
                                       "shower",
                                       "unknown",
                                       ""};

static const char * const names[] = {"value",        "info",           "values",         "entities",       "commands",           "metrics",
                                     "settings",     "network",        "message",        "fetch",          "read",               "watch",
                                     "test",         "response",       "seltemp",        "comfort",        "mode",               "flowtemp",
                                     "outdoortemp",  "chargetype",     "flamecurr",      "test_seltemp",   "test_custom",        "test_scheduler",
                                     "test_tempsensor2", "test_analogsensor1", "send_message", "fetch_values", "locale",         "fullname",
                                     "writeable",    "offset",         "hc1",            "hc8",            "hc9",                "dhw",
                                     "dhw1",         "dhw10",          "dhw11",          "hs1",            "hs16",               "src10",
                                     "src17",        "ahs1",           "id1",            "id19",           "0B_0C0D_0E0F_1011", "value/value"};

static std::vector<std::string> registered; // "device/command" paths that really exist, filled at start

static void collect_registered_names() {
    for (const char * device : devices) {
        JsonDocument doc_in;
        JsonDocument doc_out;
        std::string  path = std::string("/api/") + device + "/commands";
        emsesp::Command::process(path.c_str(), true, doc_in.to<JsonObject>(), doc_out.to<JsonObject>());
        for (JsonPair p : doc_out.as<JsonObject>()) {
            registered.emplace_back(std::string(device) + "/" + p.key().c_str());
        }
    }
}

static const char * const separators[] = {"/", "/", "/", ".", "//", "?", "&", "=", "/api/"};

static const char * const keys[] = {"value", "data", "id", "hc", "dhw", "ahs", "hs", "src", "device", "cmd", "entity"};

static const char * const expr_tokens[] = {
    "1",      "0",       "-1",       "2.5",     "1e30",        "-2147483648", "4294967296", "99999999999999999999",
    "+",      "-",       "*",        "/",       "%",           "^",           "(",          ")",
    "?",      ":",       "==",       "!=",      "<",           "<=",          ">",          ">=",
    "&&",     "||",      "!",        "int",     "round",       "abs",         "ln",         "log",
    "exp",    "sqrt",    "pow",      "hex",     "tohex",       "rnd",         "\"text\"",   "'text'",
    "\"",     "'",       "{",        "}",       "{\"url\":\"\"}", "{\"url\":\"http://127.0.0.1/\",\"key\":\"a\"}", "boiler/flowtemp", "custom/test_seltemp",
    "system/settings/locale", "commands/fetch_values/value", "thermostat/hc1/seltemp/value", " ",  "true",  "off",  "0x10", "FFFFFFFFFFFF"};

static const char * const special_values[] = {"",        "-",        ".",          "1e30",       "-1e30",       "-2147483648", "2147483648",
                                              "4294967296", "NaN",   "inf",        "on",         "off",         "true",        "auto",
                                              "reset",   "01.02.2026", "99.99.9999", "boiler/flowtemp", "boiler/",  "/",       "hc1/seltemp",
                                              "commands/send_message/value", "{",   "}",          "\"",          "%",           "\\",          "\xff\xfe"};

// a random string: either built from tokens or random bytes, then mutated
static std::string mutate(std::string s) {
    uint32_t rounds = below(4);
    for (uint32_t i = 0; i < rounds; i++) {
        size_t pos = s.empty() ? 0 : below(s.size() + 1);
        switch (below(6)) {
        case 0:
            s.insert(pos, 1, static_cast<char>(1 + below(255)));
            break;
        case 1:
            if (!s.empty() && pos < s.size()) {
                s.erase(pos, 1 + below(4));
            }
            break;
        case 2:
            if (!s.empty() && pos < s.size()) {
                s[pos] = static_cast<char>(1 + below(255));
            }
            break;
        case 3:
            s.insert(pos, s.substr(0, below(s.size() + 1)));
            break;
        case 4:
            s.insert(pos, std::string(1 + below(chance(5) ? 2000 : 40), "9({/a\"."[below(7)]));
            break;
        default:
            s.insert(pos, pick(expr_tokens));
            break;
        }
    }

    return s;
}

static std::string random_path() {
    std::string path = chance(90) ? "/api/" : "";
    if (!registered.empty() && chance(70)) {
        path += registered[below(registered.size())];
        if (chance(30)) {
            path += "/";
            path += pick(names);
        }
    } else {
        path += pick(devices);
        uint32_t parts = below(5);
        for (uint32_t i = 0; i < parts; i++) {
            path += pick(separators);
            path += pick(names);
        }
    }

    return chance(10) ? mutate(path) : path;
}

static std::string random_expression() {
    std::string expr;
    uint32_t    tokens = 1 + below(12);
    for (uint32_t i = 0; i < tokens; i++) {
        expr += pick(expr_tokens);
        if (chance(50)) {
            expr += ' ';
        }
    }

    return chance(30) ? mutate(expr) : expr;
}

static double random_double() {
    switch (below(6)) {
    case 0:
        return 0.0;
    case 1:
        return DBL_MAX * (chance(50) ? 1 : -1);
    case 2:
        return DBL_MIN;
    default:
        return (static_cast<double>(next() % 2000000) - 1000000.0) * std::pow(10.0, static_cast<int>(below(60)) - 20) + 0.25;
    }
}

static void random_value(JsonVariant v) {
    switch (below(9)) {
    case 0:
        v.set(nullptr);
        break;
    case 1:
        v.set(chance(50));
        break;
    case 2: {
        const int32_t edges[] = {0, 1, -1, INT32_MIN, INT32_MAX, 999999999, 1000000000, -100000000};
        v.set(chance(50) ? edges[below(8)] : static_cast<int32_t>(next()));
        break;
    }
    case 3:
        v.set(static_cast<int64_t>(next()));
        break;
    case 4:
        v.set(random_double());
        break;
    case 5:
        v.set(pick(special_values));
        break;
    case 6:
        v.set(random_expression());
        break;
    case 7: {
        JsonObject o = v.to<JsonObject>();
        o[pick(keys)] = pick(special_values);
        break;
    }
    default:
        v.set(mutate(pick(special_values)));
        break;
    }
}

static std::string last_input;
static uint32_t    return_codes[8];

static void report_last_input() {
    fprintf(stderr, "\nfuzz: last input was:\n%s\n", last_input.c_str());
}

static void fuzz_command() {
    JsonDocument doc_in;
    JsonDocument doc_out;
    JsonObject   input  = doc_in.to<JsonObject>();
    JsonObject   output = doc_out.to<JsonObject>();

    uint32_t n = below(4);
    for (uint32_t i = 0; i < n; i++) {
        random_value(input[pick(keys)].to<JsonVariant>());
    }

    std::string path     = random_path();
    bool        is_admin = chance(50);
    bool        is_user  = chance(50);

    last_input = "Command::process(\"" + path + "\", is_admin=" + (is_admin ? "1" : "0") + ", is_user=" + (is_user ? "1" : "0") + ", ";
    serializeJson(input, last_input);
    last_input += ")";

    return_codes[std::min<uint8_t>(emsesp::Command::process(path.c_str(), is_admin, input, output, is_user), 7)]++;
}

static void fuzz_compute() {
    std::string expr = random_expression();
    bool        is_admin = chance(50);
    last_input       = "compute(\"" + expr + "\"), is_admin=" + (is_admin ? "1" : "0");
    emsesp::Command::CallerRights rights(is_admin);
    emsesp::compute(expr);
}

static int run() {
    uint32_t iterations = 20000;
    if (const char * s = getenv("FUZZ_ITERATIONS")) {
        iterations = strtoul(s, nullptr, 10);
    }

    uint64_t seed = 1;
    if (const char * s = getenv("FUZZ_SEED")) {
        seed = strtoull(s, nullptr, 10);
    }
    rng_state = seed ? seed : 1;

    __sanitizer_set_death_callback(report_last_input);
    collect_registered_names();
    printf("fuzz: %u iterations, seed %llu, %u registered command names\n", iterations, (unsigned long long)seed, (unsigned)registered.size());

    for (uint32_t i = 0; i < iterations; i++) {
        if (chance(60)) {
            fuzz_command();
        } else {
            fuzz_compute();
        }
    }

    printf("fuzz: Command::process results: fail %u, ok %u, not found %u, error %u, not allowed %u, invalid %u, no value %u\n",
           return_codes[0],
           return_codes[1],
           return_codes[2],
           return_codes[3],
           return_codes[4],
           return_codes[5],
           return_codes[6]);
    printf("fuzz: done, no errors\n");
    return 0;
}

} // namespace fuzz
