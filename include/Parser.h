#pragma once
#include <vector>
#include <initializer_list>
#include <iostream>

#include "Token.h"
#include "TokenType.h"
#include "Expr.h"

class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens) : tokens(tokens) {}

private:
    const std::vector<Token>& tokens;
    size_t current = 0;

    std::unique_ptr<lox::Expr> expression();
    std::unique_ptr<lox::Expr> equality();
    std::unique_ptr<lox::Expr> comparison();
    std::unique_ptr<lox::Expr> term();
    std::unique_ptr<lox::Expr> factor();
    std::unique_ptr<lox::Expr> unary();
    std::unique_ptr<lox::Expr> primary();

    bool match(std::initializer_list<TokenType> types);
    bool check(TokenType Type);
    Token advance();
    bool isAtEnd() const;
    Token peek() const;
    Token previous() const;
    Token consume(TokenType type, std::string message);
};
