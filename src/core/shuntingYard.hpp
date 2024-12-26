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
#include <string>
#include <vector>
#include <deque>
#include <math.h>

class Token {
  public:
    enum class Type : uint8_t {
        Unknown,
        Number,
        String,
        Operator,
        Compare,
        Logic,
        Unary,
        LeftParen,
        RightParen,
    };

    Token(Type type, const std::string & s, int8_t precedence = -1, bool rightAssociative = false)
        : type{type}
        , str(s)
        , precedence{precedence}
        , rightAssociative{rightAssociative} {
    }

    const Type        type;
    const std::string str;
    const int8_t      precedence;
    const bool        rightAssociative;
};

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
            if (*p) {
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
        } else if (strncmp(p, "log", 3) == 0) {
            p += 2;
            tokens.emplace_back(Token::Type::Unary, "l", 5);
        } else if (strncmp(p, "exp", 3) == 0) {
            p += 2;
            tokens.emplace_back(Token::Type::Unary, "e", 5);
        } else if (strncmp(p, "sqrt", 4) == 0) {
            p += 3;
            tokens.emplace_back(Token::Type::Unary, "s", 5);
        } else if (strncmp(p, "pow", 3) == 0) {
            p += 2;
            tokens.emplace_back(Token::Type::Unary, "p", 5);
        } else if (*p >= 'a' && *p <= 'z') {
            const auto * b = p;
            while ((*p >= 'a' && *p <= 'z') || (*p == '_')) {
                ++p;
            }
            const auto s = std::string(b, p);
            tokens.emplace_back(Token::Type::String, s, -2);
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
            tokens.emplace_back(Token::Type::Number, s, -4);
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
                    precedence = 1;
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

// check if string is a number
bool isnum(const std::string & s) {
    if (s.find_first_not_of("0123456789.") == std::string::npos || (s[0] == '-' && s.find_first_not_of("0123456789.", 1) == std::string::npos)) {
        return true;
    }
    return false;
}


// replace commands like "<device>/<hc>/<cmd>" with its value"
std::string commands(std::string & expr, bool quotes = true) {
    for (uint8_t device = 0; device < emsesp::EMSdevice::DeviceType::UNKNOWN; device++) {
        const char * d = emsesp::EMSdevice::device_type_2_device_name(device);
        auto         f = expr.find(d);
        while (f != std::string::npos) {
            // entity names are alphanumeric or _
            auto e = expr.find_first_not_of("/._abcdefghijklmnopqrstuvwxyz0123456789", f);
            if (e == std::string::npos) {
                e = expr.length();
            }
            while (e > 0 && expr[e - 1] == ' ') { // remove blanks from end
                e--;
            }
            char   cmd[COMMAND_MAX_LENGTH];
            size_t l = e - f;
            if (l >= sizeof(cmd) - 1) {
                break;
            }
            expr.copy(cmd, l, f);
            cmd[l] = '\0';
            if (strstr(cmd, "/value") == nullptr) {
                strlcat(cmd, "/value", sizeof(cmd) - 6);
            }
            JsonDocument doc_out;
            JsonDocument doc_in;
            JsonObject   output = doc_out.to<JsonObject>();
            JsonObject   input  = doc_in.to<JsonObject>();
            std::string  cmd_s  = "api/" + std::string(cmd);
            emsesp::Command::process(cmd_s.c_str(), true, input, output);
            if (output["api_data"].is<std::string>()) {
                std::string data = output["api_data"];
                if (!isnum(data) && quotes) {
                    data.insert(data.begin(), '"');
                    data.insert(data.end(), '"');
                }
                expr.replace(f, l, data);
                e = f + data.length();
            } else {
                return expr = "";
            }
            f = expr.find(d, e);
        }
    }
    return expr;
}

// checks for logic value
int to_logic(const std::string & s) {
    if (s[0] == '1' || s == "on" || s == "ON" || s == "true") {
        return 1;
    }
    if (s[0] == '0' || s == "off" || s == "OFF" || s == "false") {
        return 0;
    }
    return -1;
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

// RPN calculator
std::string calculate(const std::string & expr) {
    auto expr_new = emsesp::Helpers::toLower(expr);
    commands(expr_new);

    const auto tokens = exprToTokens(expr_new);
    if (tokens.empty()) {
        return "";
    }
    auto queue = shuntingYard(tokens);
    if (queue.empty()) {
        return "";
    }

    /*
    // debug only print tokens
    #ifdef EMSESP_STANDALONE
    for (const auto & t : queue) {
        emsesp::EMSESP::logger().debug("shunt token: %s(%d)", t.str.c_str(), t.type);
    }
    #endif
    */

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
            switch (token.str[0]) {
            default:
                return "";
                break;
            case 'm': // Special operator name for unary '-'
                if (!isnum(rhs)) {
                    return "";
                }
                stack.push_back(to_string(-1 * std::stod(rhs)));
                break;
            case '!':
                if (to_logic(rhs) < 0) {
                    return "";
                }
                stack.push_back(to_logic(rhs) == 0 ? "1" : "0");
                break;
            case 'i':
                stack.push_back(to_string(std::stoi(rhs)));
                break;
            case 'r':
                stack.push_back(to_string(std::round(std::stod(rhs))));
                break;
            case 'a':
                stack.push_back(to_string(std::abs(std::stod(rhs))));
                break;
            case 'e':
                stack.push_back(to_string(std::exp(std::stod(rhs))));
                break;
            case 'l':
                stack.push_back(to_string(std::log(std::stod(rhs))));
                break;
            case 's':
                stack.push_back(to_string(std::sqrt(std::stod(rhs))));
                break;
            case 'p':
                stack.push_back(to_string(std::pow(std::stod(rhs), 2)));
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
                stack.push_back((lhs == rhs) ? "1" : "0");
                break;
            case '!':
                if (isnum(rhs) && isnum(lhs)) {
                    stack.push_back((std::stod(lhs) != std::stod(rhs)) ? "1" : "0");
                    break;
                }
                stack.push_back((lhs != rhs) ? "1" : "0");
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
            if (stack.empty() || !isnum(stack.back())) {
                return "";
            }
            const auto rhs = std::stod(stack.back());
            stack.pop_back();
            if (stack.empty() || !isnum(stack.back())) {
                return "";
            }
            const auto lhs = std::stod(stack.back());
            stack.pop_back();

            switch (token.str[0]) {
            default:
                return "";
                break;
            case '^':
                stack.push_back(to_string(pow(lhs, rhs)));
                break;
            case '*':
                stack.push_back(to_string(lhs * rhs));
                break;
            case '/':
                stack.push_back(to_string(lhs / rhs));
                break;
            case '%':
                stack.push_back(std::to_string(static_cast<int>(lhs) % static_cast<int>(rhs)));
                break;
            case '+':
                stack.push_back(to_string(lhs + rhs));
                break;
            case '-':
                stack.push_back(to_string(lhs - rhs));
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

// check for multiple instances of <cond> ? <expr1> : <expr2>
std::string compute(const std::string & expr) {
    auto expr_new = emsesp::Helpers::toLower(expr);

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
            HTTPClient  http;
            std::string url = doc["url"] | "";
            if (http.begin(url.c_str())) {
                int httpResult = 0;
                for (JsonPair p : doc["header"].as<JsonObject>()) {
                    http.addHeader(p.key().c_str(), p.value().as<std::string>().c_str());
                }
                std::string value  = doc["value"] | "";
                std::string method = doc["method"] | "GET"; // default GET

                // if there is data, force a POST
                if (value.length() || method == "post") {
                    if (value.find_first_of('{') != std::string::npos) {
                        http.addHeader("Content-Type", "application/json"); // auto-set to JSON
                    }
                    httpResult = http.POST(value.c_str());
                } else {
                    httpResult = http.GET(); // normal GET
                }

                if (httpResult > 0) {
                    std::string result = emsesp::Helpers::toLower(http.getString().c_str());
                    std::string key    = doc["key"] | "";
                    doc.clear();
                    if (key.length() && DeserializationError::Ok == deserializeJson(doc, result)) {
                        result = doc[key.c_str()].as<std::string>();
                    }
                    expr_new.replace(f, e - f, result.c_str());
                }
                http.end();
            }
        }
        f = expr_new.find_first_of('{', e);
    }

    // positions: q-questionmark, c-colon
    auto q = expr_new.find_first_of('?');
    while (q != std::string::npos) {
        // find corresponding colon
        auto c1 = expr_new.find_first_of(':', q + 1);
        auto q1 = expr_new.find_first_of('?', q + 1);
        while (q1 < c1 && q1 != std::string::npos && c1 != std::string::npos) {
            q1 = expr_new.find_first_of('?', q1 + 1);
            c1 = expr_new.find_first_of(':', c1 + 1);
        }
        if (c1 == std::string::npos) {
            return ""; // error: missing colon
        }
        std::string cond = calculate(expr_new.substr(0, q));
        if (cond.length() == 0) {
            return "";
        } else if (cond[0] == '1') {
            expr_new.erase(c1);       // remove second expression after colon
            expr_new.erase(0, q + 1); // remove condition before questionmark
        } else if (cond[0] == '0') {
            expr_new.erase(0, c1 + 1); // remove condition and first expression
        } else {
            return ""; // error
        }
        q = expr_new.find_first_of('?'); // search next instance
    }

    return calculate(expr_new);
}
