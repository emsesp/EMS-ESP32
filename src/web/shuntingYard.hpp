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
//  If you use this code in binary / compiled / un-commented (removing all text comments) form,
//  you can use it under CC0 license.
//
//  But if you use this code as source code / readable text, since main content of this code is
//  their notes, I recommend you to indicate notices which conform CC-BY-SA.  For example,
//
//  --- ---
//  YOUR-CONTENT uses the following materials.
//  (1) Wikipedia article [Shunting-yard algorithm](https://en.wikipedia.org/wiki/Shunting-yard_algorithm),
//  which is released under the [Creative Commons Attribution-Share-Alike License 3.0](https://creativecommons.org/licenses/by-sa/3.0/).
//  (2) [Implementation notes for unary operators in Shunting-Yard algorithm](https://stackoverflow.com/a/5240912) by Austin Taylor
//  which is released under the [Creative Commons Attribution-Share-Alike License 2.5](https://creativecommons.org/licenses/by-sa/2.5/).
//  --- ---
// copy from https://gist.github.com/t-mat/b9f681b7591cdae712f6
// modified MDvP, 06.2024
//
#include <string>
#include <vector>
#include <deque>
#include <math.h>

class Token {
  public:
    enum class Type {
        Unknown,
        Number,
        Operator,
        LeftParen,
        RightParen,
    };

    Token(Type type, const std::string & s, int8_t precedence = -1, bool rightAssociative = false, bool unary = false)
        : type{type}
        , str(s)
        , precedence{precedence}
        , rightAssociative{rightAssociative}
        , unary{unary} {
    }

    const Type        type;
    const std::string str;
    const int8_t      precedence;
    const bool        rightAssociative;
    const bool        unary;
};

std::deque<Token> exprToTokens(const std::string & expr) {
    std::deque<Token> tokens;

    for (const auto * p = expr.c_str(); *p; ++p) {
        if (isblank(*p)) {
            // do nothing
        } else if ((*p >= 'a' && *p <= 'z')) {
            tokens.clear();
            return tokens;
        } else if (isdigit(*p)) {
            const auto * b = p;
            while (isdigit(*p) || *p == '.') {
                ++p;
            }
            const auto s = std::string(b, p);
            tokens.push_back(Token{Token::Type::Number, s});
            --p;
        } else {
            Token::Type token            = Token::Type::Operator;
            int8_t      precedence       = -1;
            bool        rightAssociative = false;
            bool        unary            = false;
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
                if (tokens.empty() || tokens.back().type == Token::Type::Operator || tokens.back().type == Token::Type::LeftParen) {
                    // it's unary '-'
                    // note#1 : 'm' is a special operator name for unary '-'
                    // note#2 : It has highest precedence than any of the infix operators
                    unary      = true;
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
                break;
            case '|':
                if (p[1] == '|')
                    ++p;
                precedence = 0;
                break;
            case '!':
                unary      = true;
                precedence = 1;
                break;
            case '<':
                if (p[1] == '=') {
                    ++p;
                    c = '{';
                }
                precedence = 1;
                break;
            case '>':
                if (p[1] == '=') {
                    ++p;
                    c = '}';
                }
                precedence = 1;
                break;
            case '=':
                if (p[1] == '=')
                    ++p;
                precedence = 1;
                break;
            }
            const auto s = std::string(1, c);
            tokens.push_back(Token{token, s, precedence, rightAssociative, unary});
        }
    }

    return tokens;
}


std::deque<Token> shuntingYard(const std::deque<Token> & tokens) {
    std::deque<Token>  queue;
    std::vector<Token> stack;

    // While there are tokens to be read:
    for (auto token : tokens) {
        // Read a token
        switch (token.type) {
        case Token::Type::Number:
            // If the token is a number, then add it to the output queue
            queue.push_back(token);
            break;

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

        case Token::Type::RightParen:
            // If token is right parenthesis:
            {
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
            }
            break;

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

// replace commands like "<device>/<hc>/<cmd>" with its value"
std::string commands(std::string & expr) {
    for (uint8_t device = 0; device < emsesp::EMSdevice::DeviceType::UNKNOWN; device++) {
        const char * d = emsesp::EMSdevice::device_type_2_device_name(device);
        auto         f = expr.find(d);
        while (f != std::string::npos) {
            auto e = expr.find_first_of(" )=<>|&+-*\0", f);
            if (e == std::string::npos) {
                e = expr.length();
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
            JsonDocument doc_out, doc_in;
            JsonObject   output = doc_out.to<JsonObject>();
            JsonObject   input  = doc_in.to<JsonObject>();
            std::string  cmd_s  = "api/" + std::string(cmd);
            emsesp::Command::process(cmd_s.c_str(), true, input, output);
            if (output.containsKey("api_data")) {
                std::string data = output["api_data"].as<std::string>();
                if (data == "true" || data == "ON" || data == "on") {
                    data = "1";
                }
                if (data == "false" || data == "OFF" || data == "off") {
                    data = "0";
                }
                expr.replace(f, l, data);
                e = f + data.length();
            }
            f = expr.find(d, e);
        }
    }
    return expr;
}

std::string compute(const std::string & expr) {
    auto expr_new = emsesp::Helpers::toLower(expr);
#ifdef EMESESP_DEBUG
    emsesp::EMSESP::logger().debug("calculate: %s", expr_new.c_str());
#endif
    commands(expr_new);
#ifdef EMESESP_DEBUG
    emsesp::EMSESP::logger().debug("calculate: %s", expr_new.c_str());
#endif
    const auto tokens = exprToTokens(expr_new);
    if (tokens.empty()) {
        return "Error: no tokens";
    }
    auto                queue = shuntingYard(tokens);
    std::vector<double> stack;

    while (!queue.empty()) {
        const auto token = queue.front();
        queue.pop_front();
        switch (token.type) {
        case Token::Type::Number:
            stack.push_back(std::stod(token.str));
            break;

        case Token::Type::Operator: {
            if (token.unary) {
                // unray operators
                const auto rhs = stack.back();
                stack.pop_back();
                switch (token.str[0]) {
                default:
                    return "";
                    break;
                case 'm': // Special operator name for unary '-'
                    stack.push_back(-rhs);
                    break;
                case '!':
                    stack.push_back(!(int)rhs);
                    break;
                }
            } else {
                // binary operators
                const auto rhs = stack.back();
                stack.pop_back();
                const auto lhs = stack.back();
                stack.pop_back();

                switch (token.str[0]) {
                default:
                    return "";
                    break;
                case '^':
                    stack.push_back(static_cast<int>(pow(lhs, rhs)));
                    break;
                case '*':
                    stack.push_back(lhs * rhs);
                    break;
                case '/':
                    stack.push_back(lhs / rhs);
                    break;
                case '%':
                    stack.push_back((int)lhs % (int)rhs);
                    break;
                case '+':
                    stack.push_back(lhs + rhs);
                    break;
                case '-':
                    stack.push_back(lhs - rhs);
                    break;
                case '&':
                    stack.push_back(((int)lhs && (int)rhs) ? 1 : 0);
                    break;
                case '|':
                    stack.push_back(((int)lhs || (int)rhs) ? 1 : 0);
                    break;
                case '<':
                    stack.push_back(((int)lhs < (int)rhs) ? 1 : 0);
                    break;
                case '{':
                    stack.push_back(((int)lhs <= (int)rhs) ? 1 : 0);
                    break;
                case '>':
                    stack.push_back(((int)lhs > (int)rhs) ? 1 : 0);
                    break;
                case '}':
                    stack.push_back(((int)lhs >= (int)rhs) ? 1 : 0);
                    break;
                case '=':
                    stack.push_back(((int)lhs == (int)rhs) ? 1 : 0);
                    break;
                }
            }
        } break;

        default:
            return "";
        }
    }
    if (stack.back() == (int)stack.back()) {
        return (std::to_string((int)stack.back()));
    }
    return std::to_string(stack.back());
}
