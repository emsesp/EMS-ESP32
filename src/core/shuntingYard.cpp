// Shunting-yard Algorithm
// https://en.wikipedia.org/wiki/Shunting-yard_algorithm
//
// Implementation notes for unary operators by Austin Taylor
//  https://stackoverflow.com/a/5240912
//
// Example:
//  https://ideone.com/VocUTq
//
// License:
//  This code uses the following materials.
//  (1) Wikipedia article [Shunting-yard algorithm](https://en.wikipedia.org/wiki/Shunting-yard_algorithm),
//  which is released under the [Creative Commons Attribution-Share-Alike License 3.0](https://creativecommons.org/licenses/by-sa/3.0/).
//  (2) [Implementation notes for unary operators in Shunting-Yard algorithm](https://stackoverflow.com/a/5240912) by Austin Taylor
//  which is released under the [Creative Commons Attribution-Share-Alike License 2.5](https://creativecommons.org/licenses/by-sa/2.5/).
//
// copy from https://gist.github.com/t-mat/b9f681b7591cdae712f6
// modified MDvP, 06.2024
//

#include "emsesp.h"

#include "shuntingYard.h"

#include "emsesp_http_client.h"

namespace emsesp {

// find tokens
std::deque<Token> exprToTokens(const std::string & expr) {
    std::deque<Token> tokens;

    for (const auto * p = expr.c_str(); *p; ++p) {
        if (isblank(*p)) {
            // do nothing
        } else if (*p == '{') { // json is stored as string including {}
            const auto * b = p;
            ++p;
            uint8_t i = 1;
            while (*p && i > 0) {
                i += (*p == '{') ? 1 : (*p == '}') ? -1 : 0;
                ++p;
            }
            const auto s = std::string(b, p);
            tokens.emplace_back(Token::Type::String, s, -3);
            if (*p == '\0') {
                --p;
            }
        } else if (strncmp(p, "int", 3) == 0) {
            p += 2;
            tokens.emplace_back(Token::Type::Unary, "i", 5);
        } else if (strncmp(p, "round", 5) == 0) {
            p += 4;
            tokens.emplace_back(Token::Type::Unary, "r", 5);
        } else if (strncmp(p, "abs", 3) == 0) {
            p += 2;
            tokens.emplace_back(Token::Type::Unary, "a", 5);
        } else if (strncmp(p, "ln", 2) == 0) {
            p += 1;
            tokens.emplace_back(Token::Type::Unary, "l", 5);
        } else if (strncmp(p, "log", 3) == 0) {
            p += 2;
            tokens.emplace_back(Token::Type::Unary, "g", 5);
        } else if (strncmp(p, "exp", 3) == 0) {
            p += 2;
            tokens.emplace_back(Token::Type::Unary, "e", 5);
        } else if (strncmp(p, "sqrt", 4) == 0) {
            p += 3;
            tokens.emplace_back(Token::Type::Unary, "s", 5);
        } else if (strncmp(p, "pow", 3) == 0) {
            p += 2;
            tokens.emplace_back(Token::Type::Unary, "p", 5);
        } else if (strncmp(p, "tohex", 5) == 0) {
            p += 4;
            tokens.emplace_back(Token::Type::Unary, "x", 5);
        } else if (strncmp(p, "hex", 3) == 0) {
            p += 2;
            tokens.emplace_back(Token::Type::Unary, "h", 5);
        } else if (strncmp(p, "rnd", 3) == 0) {
            p += 2;
            tokens.emplace_back(Token::Type::Unary, "d", 5);
        } else if ((*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') || (*p == '_') || (*p & 0x80)) {
            const auto * b = p;
            while ((*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') || (*p == '_') || (*p & 0x80)) {
                ++p;
            }
            const auto s = std::string(b, p);
            tokens.emplace_back(Token::Type::String, s, -3);
            --p;
        } else if (*p == '"') {
            ++p;
            const auto * b = p;
            while (*p && *p != '"') {
                ++p;
            }
            const auto s = std::string(b, p);
            tokens.emplace_back(Token::Type::String, s, -3);
            if (*p == '\0') {
                --p;
            }
        } else if (*p == '\'') {
            ++p;
            const auto * b = p;
            while (*p && *p != '\'') {
                ++p;
            }
            const auto s = std::string(b, p);
            tokens.emplace_back(Token::Type::String, s, -3);
            if (*p == '\0') {
                --p;
            }
        } else if (isdigit(*p)) {
            const auto * b = p;
            while (isdigit(*p) || *p == '.') {
                ++p;
            }
            const auto s = std::string(b, p);
            tokens.emplace_back(Token::Type::Number, s, -2);
            --p;
        } else {
            Token::Type token            = Token::Type::Operator;
            int8_t      precedence       = -1;
            bool        rightAssociative = false;
            char        c                = *p;
            switch (c) {
            default:
                token = Token::Type::Unknown;
                break;
            case '(':
                token = Token::Type::LeftParen;
                break;
            case ')':
                token = Token::Type::RightParen;
                break;
            case '^':
                precedence       = 4;
                rightAssociative = true;
                break;
            case '*':
                precedence = 3;
                break;
            case '/':
                precedence = 3;
                break;
            case '%':
                precedence = 3;
                break;
            case '+':
                precedence = 2;
                break;
            case '-':
                // If current token is '-'
                // and if it is the first token, or preceded by another operator, or left-paren,
                if (tokens.empty() || tokens.back().type == Token::Type::Operator || tokens.back().type == Token::Type::Compare
                    || tokens.back().type == Token::Type::Logic || tokens.back().type == Token::Type::Unary || tokens.back().type == Token::Type::LeftParen) {
                    // it's unary '-'
                    // note#1 : 'm' is a special operator name for unary '-'
                    // note#2 : It has highest precedence than any of the infix operators
                    if (!tokens.empty() && tokens.back().str[0] == 'm') { // double unary minus
                        tokens.pop_back();
                        continue;
                    }
                    token      = Token::Type::Unary;
                    c          = 'm';
                    precedence = 5;
                } else {
                    // otherwise, it's binary '-'
                    precedence = 2;
                }
                break;
            case '&':
                if (p[1] == '&')
                    ++p;
                precedence = 0;
                token      = Token::Type::Logic;
                break;
            case '|':
                if (p[1] == '|')
                    ++p;
                precedence = 0;
                token      = Token::Type::Logic;
                break;
            case '!':
                if (p[1] == '=') {
                    ++p;
                    precedence = 1;
                    token      = Token::Type::Compare;
                } else {
                    precedence = 2;
                    token      = Token::Type::Unary;
                }
                break;
            case '<':
                if (p[1] == '=') {
                    ++p;
                    c = '{';
                }
                precedence = 1;
                token      = Token::Type::Compare;
                break;
            case '>':
                if (p[1] == '=') {
                    ++p;
                    c = '}';
                }
                precedence = 1;
                token      = Token::Type::Compare;
                break;
            case '=':
                if (p[1] == '=')
                    ++p;
                precedence = 1;
                token      = Token::Type::Compare;
                break;
            }
            const auto s = std::string(1, c);
            tokens.emplace_back(token, s, precedence, rightAssociative);
        }
    }

    return tokens;
}

// sort tokens to RPN form
std::deque<Token> shuntingYard(const std::deque<Token> & tokens) {
    std::deque<Token>  queue;
    std::vector<Token> stack;

    // Reserve space for vector to reduce reallocations
    stack.reserve(tokens.size() / 2);

    // While there are tokens to be read:
    for (auto const & token : tokens) {
        // Read a token
        switch (token.type) {
        case Token::Type::Number:
        case Token::Type::String:
            // If the token is a number, then add it to the output queue
            queue.push_back(token);
            break;

        case Token::Type::Unary:
        case Token::Type::Compare:
        case Token::Type::Logic:
        case Token::Type::Operator: {
            // If the token is operator, o1, then:
            const auto o1 = token;

            // while there is an operator token,
            while (!stack.empty()) {
                // o2, at the top of stack, and
                const auto o2 = stack.back();

                // either o1 is left-associative and its precedence is
                // *less than or equal* to that of o2,
                // or o1 if right associative, and has precedence
                // *less than* that of o2,
                if ((!o1.rightAssociative && o1.precedence <= o2.precedence) || (o1.rightAssociative && o1.precedence < o2.precedence)) {
                    // then pop o2 off the stack,
                    stack.pop_back();
                    // onto the output queue;
                    queue.push_back(o2);

                    continue;
                }

                // @@ otherwise, exit.
                break;
            }

            // push o1 onto the stack.
            stack.push_back(o1);
        } break;

        case Token::Type::LeftParen:
            // If token is left parenthesis, then push it onto the stack
            stack.push_back(token);
            break;

        case Token::Type::RightParen: {
            // If token is right parenthesis:
            bool match = false;

            // Until the token at the top of the stack
            // is a left parenthesis,
            while (!stack.empty() && stack.back().type != Token::Type::LeftParen) {
                // pop operators off the stack
                // onto the output queue.
                queue.push_back(stack.back());
                stack.pop_back();
                match = true;
            }

            if (!match && stack.empty()) {
                // If the stack runs out without finding a left parenthesis,
                // then there are mismatched parentheses.
                return {};
            }

            // Pop the left parenthesis from the stack,
            // but not onto the output queue.
            stack.pop_back();
        } break;

        case Token::Type::Unknown:
        default:
            return {};
        }
    }

    // When there are no more tokens to read:
    //   While there are still operator tokens in the stack:
    while (!stack.empty()) {
        // If the operator token on the top of the stack is a parenthesis,
        // then there are mismatched parentheses.
        if (stack.back().type == Token::Type::LeftParen) {
            return {};
        }

        // Pop the operator onto the output queue.
        queue.push_back(std::move(stack.back()));
        stack.pop_back();
    }
    return queue;
}

// check if string is a number. Must contain at least one digit
bool isnum(const std::string & s) {
    if (s.empty() || s.find_first_of("0123456789") == std::string::npos) {
        return false;
    }
    if (s.find_first_not_of("0123456789.") == std::string::npos || (s[0] == '-' && s.find_first_not_of("0123456789.", 1) == std::string::npos)) {
        return true;
    }
    return false;
}

// replace commands like "<device>/<hc>/<cmd>" with its value"
std::string commands(std::string & expr, bool quotes) {
    auto expr_new = Helpers::toLower(expr);
    for (uint8_t device = 0; device < EMSdevice::DeviceType::UNKNOWN; device++) {
        std::string d;
        d.reserve(32); // typical device name length + "/"
        d = EMSdevice::device_type_2_device_name(device);
        d += "/";
        auto f = expr_new.find(d);
        while (f != std::string::npos) {
            // entity names are alphanumeric or _
            auto e = expr_new.find_first_not_of("/._abcdefghijklmnopqrstuvwxyz0123456789", f);
            if (e == std::string::npos) {
                e = expr.length();
            }
            char   cmd[COMMAND_MAX_LENGTH];
            size_t l = e - f;
            if (l >= sizeof(cmd) - 1) {
                break;
            }
            expr_new.copy(cmd, l, f);
            cmd[l] = '\0';

            if (strstr(cmd, "/value") == nullptr) {
                strlcat(cmd, "/value", sizeof(cmd) - 6);
            }
            JsonDocument doc_out;
            JsonDocument doc_in;
            JsonObject   output = doc_out.to<JsonObject>();
            JsonObject   input  = doc_in.to<JsonObject>();
            char         cmd_s[COMMAND_MAX_LENGTH + 5]; // "api/" prefix + cmd
            snprintf(cmd_s, sizeof(cmd_s), "api/%s", cmd);

            auto return_code = Command::process(cmd_s, true, input, output);
            // check for no value (entity is valid but has no value set)
            if (return_code != CommandRet::OK && return_code != CommandRet::NO_VALUE) {
                return expr = "";
            }
            std::string data;
            if (output["api_data"].is<std::string>()) {
                data = output["api_data"].as<std::string>();
                if (!isnum(data) && quotes) {
                    data.insert(data.begin(), '"');
                    data.insert(data.end(), '"');
                }
            } else {
                serializeJson(output, data);
            }
            expr.replace(f, l, data);
            e        = f + data.length();
            expr_new = Helpers::toLower(expr);
            f        = expr_new.find(d, e);
        }
    }
    if (quotes) {
        // remove double quotes
        auto f = expr.find("\"\"");
        while (f != std::string::npos) {
            expr.erase(f, 2);
            f = expr.find("\"\"");
        }
    }
    return expr;
}

// checks for logic value
int to_logic(const std::string & s) {
    bool value_b;
    if (!Helpers::value2bool(s.c_str(), value_b)) {
        return -1; // invalid
    }
    return value_b;

    // if (s.empty()) {
    //     return -1;
    // }
    // auto l = Helpers::toLower(s);
    // if (s[0] == '1' || l == "on" || l == "true") {
    //     return 1;
    // }
    // if (s[0] == '0' || l == "off" || l == "false") {
    //     return 0;
    // }
    // return -1;
}

// number to string, remove trailing zeros
std::string to_string(double d) {
    std::string s = std::to_string(d);
    while (!s.empty() && s.back() == '0') {
        s.pop_back();
    }
    if (!s.empty() && s.back() == '.') {
        s.pop_back();
    }
    return s;
}

// number to hex string
std::string to_hex(uint32_t i) {
    char c[10];
    snprintf(c, 10, "%02X", i);
    std::string s = c;
    return s;
}

// RPN calculator
std::string calculate(const std::string & expr) {
    std::string expr_new = expr;
    // commands(expr_new);

    const auto tokens = exprToTokens(expr_new);
    // for debugging only
    // for (const auto & t : tokens) {
    //     EMSESP::logger().debug("shunt token: %s(%d)", t.str.c_str(), t.type);
    //     Serial.printf("shunt token: %s(%d)\n", t.str.c_str(), t.type);
    //     Serial.println();
    // }
    if (tokens.empty()) {
        return "";
    }

    auto queue = shuntingYard(tokens);
    if (queue.empty()) {
        return "";
    }

    std::vector<std::string> stack;

    while (!queue.empty()) {
        const auto token = queue.front();
        queue.pop_front();
        switch (token.type) {
        case Token::Type::Number:
        case Token::Type::String:
            stack.push_back(token.str);
            break;
        case Token::Type::Unary: {
            if (stack.empty()) {
                return "";
            }
            const auto rhs = stack.back();
            stack.pop_back();
            if (token.str[0] == '!') {
                const auto logic = to_logic(rhs);
                if (logic >= 0) {
                    stack.push_back(logic == 0 ? "1" : "0");
                } else if (isnum(rhs)) {
                    stack.push_back(std::stod(rhs) == 0 ? "1" : "0");
                } else {
                    // usually literal text that was left unquoted, so its '!' got read as a logical NOT
                    EMSESP::logger().warning("'!' needs a boolean or numeric operand, got '%s'. Literal text must be quoted", rhs.c_str());
                    return "";
                }
                break;
            }
            if (token.str[0] == 'h') {
                // hex string to number
                if (rhs.empty() || rhs.find_first_not_of("0123456789abcdefABCDEF") != std::string::npos) {
                    return "";
                }
                stack.push_back(to_string(std::stoi(rhs, 0, 16)));
                break;
            }
            // bail out on non-numeric operand
            if (!isnum(rhs)) {
                return "";
            }
            auto rhd = std::stod(rhs);
            switch (token.str[0]) {
            default:
                return "";
                break;
            case 'm': // Special operator name for unary '-'
                stack.push_back(to_string(-1 * rhd));
                break;
            case 'i':
                stack.push_back(to_string(static_cast<int>(rhd)));
                break;
            case 'r':
                stack.push_back(to_string(std::round(rhd)));
                break;
            case 'a':
                stack.push_back(to_string(std::abs(rhd)));
                break;
            case 'e':
                stack.push_back(to_string(std::exp(rhd)));
                break;
            case 'l':
                stack.push_back(to_string(std::log(rhd)));
                break;
            case 'g':
                stack.push_back(to_string(std::log10(rhd)));
                break;
            case 's':
                stack.push_back(to_string(std::sqrt(rhd)));
                break;
            case 'p':
                stack.push_back(to_string(std::pow(rhd, 2)));
                break;
            case 'x':
                stack.push_back(to_hex(static_cast<int>(rhd)));
                break;
            case 'd':
#ifndef EMSESP_STANDALONE
                stack.push_back(to_string(rhd * esp_random() / UINT32_MAX));
#else
                stack.push_back(to_string(rhd * rand() / RAND_MAX));
#endif
                break;
            }
        } break;
        case Token::Type::Compare: {
            if (stack.size() < 2) {
                return "";
            }
            const auto rhs = stack.back();
            stack.pop_back();
            const auto lhs = stack.back();
            stack.pop_back();
            switch (token.str[0]) {
            default:
                return "";
                break;
            case '<':
                if (isnum(rhs) && isnum(lhs)) {
                    stack.push_back((std::stod(lhs) < std::stod(rhs)) ? "1" : "0");
                    break;
                }
                stack.push_back((lhs < rhs) ? "1" : "0");
                break;
            case '{':
                if (isnum(rhs) && isnum(lhs)) {
                    stack.push_back((std::stod(lhs) <= std::stod(rhs)) ? "1" : "0");
                    break;
                }
                stack.push_back((lhs <= rhs) ? "1" : "0");
                break;
            case '>':
                if (isnum(rhs) && isnum(lhs)) {
                    stack.push_back((std::stod(lhs) > std::stod(rhs)) ? "1" : "0");
                    break;
                }
                stack.push_back((lhs > rhs) ? "1" : "0");
                break;
            case '}':
                if (isnum(rhs) && isnum(lhs)) {
                    stack.push_back((std::stod(lhs) >= std::stod(rhs)) ? "1" : "0");
                    break;
                }
                stack.push_back((lhs >= rhs) ? "1" : "0");
                break;
            case '=':
                if (isnum(rhs) && isnum(lhs)) {
                    stack.push_back((std::stod(lhs) == std::stod(rhs)) ? "1" : "0");
                    break;
                }
                // compare strings lower case
                stack.push_back((Helpers::toLower(lhs) == Helpers::toLower(rhs)) ? "1" : "0");
                break;
            case '!':
                if (isnum(rhs) && isnum(lhs)) {
                    stack.push_back((std::stod(lhs) != std::stod(rhs)) ? "1" : "0");
                    break;
                }
                // compare strings lower case
                stack.push_back((Helpers::toLower(lhs) != Helpers::toLower(rhs)) ? "1" : "0");
                break;
            }
        } break;
        case Token::Type::Logic: {
            // binary operators
            if (stack.size() < 2) {
                return "";
            }
            const auto rhs = to_logic(stack.back());
            stack.pop_back();
            const auto lhs = to_logic(stack.back());
            stack.pop_back();
            if (rhs < 0 || lhs < 0) {
                return "";
            }
            switch (token.str[0]) {
            default:
                return "";
                break;
            case '&':
                stack.push_back((lhs && rhs) ? "1" : "0");
                break;
            case '|':
                stack.push_back((lhs || rhs) ? "1" : "0");
                break;
            }
        } break;
        case Token::Type::Operator: {
            // binary operators
            if (stack.size() < 2) {
                return "";
            }
            const auto rhs = stack.back();
            stack.pop_back();
            const auto lhs = stack.back();
            stack.pop_back();
            if (token.str[0] == '+' && (!isnum(rhs) || !isnum(lhs))) {
                stack.push_back(lhs + rhs);
                break;
            }
            // bail out on non-numeric operands
            if (!isnum(lhs) || !isnum(rhs)) {
                return "";
            }
            auto lhd = std::stod(lhs);
            auto rhd = std::stod(rhs);
            switch (token.str[0]) {
            default:
                return "";
                break;
            case '^':
                stack.push_back(to_string(pow(lhd, rhd)));
                break;
            case '*':
                stack.push_back(to_string(lhd * rhd));
                break;
            case '/':
                stack.push_back(to_string(lhd / rhd));
                break;
            case '%':
                stack.push_back(std::to_string(static_cast<int>(lhd) % static_cast<int>(rhd)));
                break;
            case '+':
                stack.push_back(to_string(lhd + rhd));
                break;
            case '-':
                stack.push_back(to_string(lhd - rhd));
                break;
            }
        } break;
        case Token::Type::LeftParen:
        case Token::Type::RightParen:
        case Token::Type::Unknown:
        default:
            return "";
            break;
        }
    }

    // concatenate all elements in stack to a single string, separated by spaces and return
    std::string result = "";
    for (const auto & s : stack) {
        result += s;
    }
    return result;
}

// find the next occurrence of c at or after `from` that is not inside a quoted string, so that
// literal text like "too hot?" is not mistaken for a ternary. Always scans from the start of the
// string because the quote state at `from` depends on everything before it.
static size_t find_unquoted(const std::string & s, char c, size_t from = 0) {
    bool in_single = false;
    bool in_double = false;
    for (size_t i = 0; i < s.length(); i++) {
        if (!in_single && s[i] == '"') {
            in_double = !in_double;
        } else if (!in_double && s[i] == '\'') {
            in_single = !in_single;
        } else if (!in_single && !in_double && s[i] == c && i >= from) {
            return i;
        }
    }
    return std::string::npos;
}

// check for multiple instances of <cond> ? <expr1> : <expr2>
std::string compute(const std::string & expr) {
    std::string expr_new = expr;
    commands(expr_new); // replace ems-esp commands with values

    // search json with url:
    auto f = expr_new.find_first_of('{');
    while (f != std::string::npos) {
        auto e = f + 1;
        for (uint8_t i = 1; i > 0; e++) {
            if (e >= expr_new.length()) {
                return "";
            } else if (expr_new[e] == '}') {
                i--;
            } else if (expr_new[e] == '{') {
                i++;
            }
        }
        std::string  cmd = expr_new.substr(f, e - f).c_str();
        JsonDocument doc;
        if (DeserializationError::Ok == deserializeJson(doc, cmd)) {
            std::string url, header_s, value_s, method_s, key_s, keys_s;
            // search keys lower case
            for (JsonPair p : doc.as<JsonObject>()) {
                if (Helpers::toLower(p.key().c_str()) == "url") {
                    url = p.value().as<std::string>();
                } else if (Helpers::toLower(p.key().c_str()) == "header") {
                    header_s = p.key().c_str();
                } else if (Helpers::toLower(p.key().c_str()) == "value") {
                    value_s = p.key().c_str();
                } else if (Helpers::toLower(p.key().c_str()) == "method") {
                    method_s = p.key().c_str();
                } else if (Helpers::toLower(p.key().c_str()) == "key") {
                    keys_s = "";
                    key_s  = p.key().c_str();
                } else if (Helpers::toLower(p.key().c_str()) == "keys") {
                    key_s  = "";
                    keys_s = p.key().c_str();
                }
            }
            std::string value  = doc[value_s] | "";
            std::string method = doc[method_s] | "GET";
            std::string result;

            int httpResult = HttpClient::request(url, method, value, doc[header_s].as<JsonObjectConst>(), result);
            if (httpResult == 200) {
                std::string  key = doc[key_s] | "";
                JsonDocument keys_doc; // JsonDocument to hold "keys" after doc is parsed with HTTP body
                if (doc[keys_s].is<JsonArray>()) {
                    keys_doc.set(doc[keys_s].as<JsonArray>());
                }
                JsonArray keys = keys_doc.as<JsonArray>();
                if (key.length() || !keys.isNull()) {
                    doc.clear();
                    if (DeserializationError::Ok == deserializeJson(doc, result)) {
                        if (key.length()) {
                            result = doc[key.c_str()].as<std::string>();
                        } else {
                            JsonVariant json = doc.as<JsonVariant>();
                            for (JsonVariant keys_key : keys) {
                                if (keys_key.is<std::string>() && json.is<JsonObject>()) {
                                    json = json[keys_key.as<std::string>()].as<JsonVariant>();
                                } else if (keys_key.is<int>() && json.is<JsonArray>()) {
                                    json = json[keys_key.as<int>()].as<JsonVariant>();
                                } else {
                                    break; // type mismatch
                                }
                            }
                            result = json.as<std::string>();
                        }
                        expr_new.replace(f, e - f, result.c_str());
                    }
                }
                expr_new.replace(f, e - f, result);
            } else if (httpResult != 0) {
                // httpResult of 0 means no url
                EMSESP::logger().warning("URL command failed with https code: %d, response: %s", httpResult, result.c_str());
            }
        }
        f = expr_new.find_first_of('{', e);
    }

    // positions: q-questionmark, c-colon
    auto q = find_unquoted(expr_new, '?');
    while (q != std::string::npos) {
        // find corresponding colon
        auto c1 = find_unquoted(expr_new, ':', q + 1);
        auto q1 = find_unquoted(expr_new, '?', q + 1);
        while (q1 < c1 && q1 != std::string::npos && c1 != std::string::npos) {
            q1 = find_unquoted(expr_new, '?', q1 + 1);
            c1 = find_unquoted(expr_new, ':', c1 + 1);
        }
        if (c1 == std::string::npos) {
            return ""; // error: missing colon
        }
        auto s  = q; // start search brackets
        int  br = 0; // count brackets
        while (s > 0 && (br || expr_new[s - 1] != '(')) {
            s--;
            br += (expr_new[s] == '(' ? -1 : expr_new[s] == ')' ? 1 : 0);
        }
        std::string cond = calculate(expr_new.substr(s, q - s));
        if (cond.length() == 0) {
            return "";
        } else if (cond[0] == '1') {
            auto e = expr_new.length(); // end
            if (s) {                    // there was a opening bracket, find the closing one
                e  = c1;
                br = 0;
                while (e < expr_new.length() && (br || expr_new[e + 1] != ')')) {
                    e++;
                    br += (expr_new[e] == ')' ? -1 : expr_new[e] == '(' ? 1 : 0);
                }
            }
            expr_new.erase(c1, e + 1 - c1); // remove second expression after colon
            expr_new.erase(s, q + 1 - s);   // remove condition before questionmark
        } else if (cond[0] == '0') {
            expr_new.erase(s, c1 + 1 - s); // remove condition and first expression
        } else {
            return ""; // error
        }
        q = find_unquoted(expr_new, '?'); // search next instance
    }

    return calculate(expr_new);
}

} // namespace emsesp
