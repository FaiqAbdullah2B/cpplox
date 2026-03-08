#pragma once

#include <sstream>
#include <variant>
#include <string>
#include <iostream>
#include <TokenType.h>

using LiteralType = std::variant<std::monostate, double, std::string, bool>;

inline std::string stringifyLiteral(const LiteralType& literal) {
    return std::visit([](const auto &arg) -> std::string {
        using T = std::decay_t<decltype(arg)>;

        if constexpr (std::is_same_v<T, std::monostate>) {
            return "nil";
        } else if constexpr (std::is_same_v<T, std::string>) {
            return arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            return arg ? "true" : "false";
        } else if constexpr (std::is_same_v<T, double>) {
            // The Lox-compliant double formatter!
            std::string text = std::to_string(arg);
            text.erase(text.find_last_not_of('0') + 1, std::string::npos);
            if (text.back() == '.') {
                text.pop_back();
            }
            return text;
        } else {
            return "unknown";
        }
    }, literal);
}

class Token {  
public: 
    Token(TokenType type, std::string lexeme, LiteralType literal, int line)
        : type(type), lexeme(std::move(lexeme)), literal(std::move(literal)), line(line) {}

    [[nodiscard]] std::string toString() const {
        return std::string(tokenTypeToString(type)) + " " + lexeme + " " + stringifyLiteral(literal);
    }

// private:
    const TokenType type;
    const std::string lexeme;
    const LiteralType literal;
    const int line;
};