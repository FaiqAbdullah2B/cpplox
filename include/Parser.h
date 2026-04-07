#pragma once
#include <vector>
#include <initializer_list>
#include <iostream>
#include <memory>

#include "Token.h"
#include "TokenType.h"
#include "Expr.h"
#include "Lox.h"
#include "Stmt.h"

class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens) : tokens(std::move(tokens)) {}
    std::vector<std::unique_ptr<lox::Stmt>> parse();
private:
    class ParseError : public std::runtime_error {
    public:
        ParseError() : std::runtime_error("Parse error") {}
    };
    
    const std::vector<Token>& tokens;
    size_t current = 0;

    std::vector<std::unique_ptr<lox::Stmt>> block();
    std::unique_ptr<lox::Stmt> declaration();
    std::unique_ptr<lox::Stmt> forStatement();
    std::unique_ptr<lox::Stmt> varDeclaration();
    std::unique_ptr<lox::Stmt> statement();
    std::unique_ptr<lox::Stmt> ifStatement();
    std::unique_ptr<lox::Stmt> printStatement();
    std::unique_ptr<lox::Stmt> returnStatement();
    std::unique_ptr<lox::Stmt> expressionStatement();
    std::unique_ptr<lox::Stmt> function(const std::string& kind);
    std::unique_ptr<lox::Stmt> whileStatement();

    std::unique_ptr<lox::Expr> expression();
    std::unique_ptr<lox::Expr> assignment();
    std::unique_ptr<lox::Expr> comma();
    std::unique_ptr<lox::Expr> ternary();
    std::unique_ptr<lox::Expr> logical_or();
    std::unique_ptr<lox::Expr> logical_and();
    std::unique_ptr<lox::Expr> equality();
    std::unique_ptr<lox::Expr> comparison();
    std::unique_ptr<lox::Expr> term();
    std::unique_ptr<lox::Expr> factor();
    std::unique_ptr<lox::Expr> unary();
    std::unique_ptr<lox::Expr> finishCall(std::unique_ptr<lox::Expr> callee);
    std::unique_ptr<lox::Expr> call();
    std::unique_ptr<lox::Expr> primary();

    bool match(std::initializer_list<TokenType> types);
    bool check(TokenType Type);
    Token advance();
    bool isAtEnd() const;
    Token peek() const;
    Token previous() const;
    Token consume(TokenType type, std::string message);

    ParseError error(const Token& token, std::string_view message);

    void synchronize();

};
