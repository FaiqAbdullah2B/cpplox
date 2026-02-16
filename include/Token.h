#pragma once

#include <sstream>
#include <variant>
#include <string>
#include <iostream>
#include <TokenType.h>

using LiteralType = std::variant<std::monostate, double, std::string, bool>;

class Token {  
public: 
    Token(TokenType type, std::string lexeme, LiteralType literal, int line)
        : type(type), lexeme(std::move(lexeme)), literal(std::move(literal)), line(line) {}

    [[nodiscard]] std::string toString() const {

        std::string literalStr = std::visit([](const auto &arg) -> std::string {
            using T = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<T, std::monostate>) {
                return "nil";
            } else if constexpr (std::is_same_v<T, std::string>) {
                return arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                return arg ? "true" : "false";
            } else {
                return std::to_string(arg);
            }
        }, literal);

        return std::string(tokenTypeToString(type)) + " " + lexeme + " " + literalStr;
    } 
private:
    const TokenType type;
    const std::string lexeme;
    const LiteralType literal;
    const int line;
};