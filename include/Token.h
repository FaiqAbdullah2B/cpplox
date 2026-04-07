#pragma once

#include <sstream>
#include <variant>
#include <string>
#include <iostream>
#include <memory>
#include <TokenType.h>

namespace lox {
    class LoxCallable;
}

using LiteralType = std::variant<std::monostate, double, std::string, bool, std::shared_ptr<lox::LoxCallable>>;

std::string stringifyLiteral(const LiteralType& literal);

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