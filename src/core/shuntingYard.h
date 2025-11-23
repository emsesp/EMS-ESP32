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

#ifndef EMSESP_SHUNTING_YARD_H_
#define EMSESP_SHUNTING_YARD_H_

#include <HTTPClient.h>
#include <ArduinoJson.h>

#include <string>
#include <vector>
#include <deque>
#include <math.h>

namespace emsesp {

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
std::deque<Token> exprToTokens(const std::string & expr);

// sort tokens to RPN form
std::deque<Token> shuntingYard(const std::deque<Token> & tokens);

// check if string is a number
bool isnum(const std::string & s);

// replace commands like "<device>/<hc>/<cmd>" with its value"
std::string commands(std::string & expr, bool quotes = true);

// checks for logic value
int to_logic(const std::string & s);

// number to string, remove trailing zeros
std::string to_string(double d);

// number to hex string
std::string to_hex(uint32_t i);

// RPN calculator
std::string calculate(const std::string & expr);

// check for multiple instances of <cond> ? <expr1> : <expr2>
std::string compute(const std::string & expr);

#endif

} // namespace emsesp