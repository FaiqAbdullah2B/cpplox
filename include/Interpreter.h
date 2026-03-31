#pragma once
#include <variant>
#include <vector>

#include "Token.h"
#include "Expr.h"
#include "RuntimeError.h"
#include "Lox.h"
#include "Stmt.h"

class Interpreter {
public:
    void interpret(const std::vector<std::unique_ptr<lox::Stmt>>& statements);

    LiteralType evaluate(const lox::Expr& expr);
    LiteralType evaluate(const lox::Stmt& stmt);

    LiteralType operator()(const lox::Expression& expression);
    LiteralType operator()(const lox::Print& print);

    LiteralType operator()(const lox::Literal& literal);
    LiteralType operator()(const lox::Grouping& grouping);
    LiteralType operator()(const lox::Unary& unary);
    LiteralType operator()(const lox::Binary& binary);
    LiteralType operator()(const lox::Ternary& ternary);

private:
    // Helper methods
    bool isTruthy(const LiteralType& val);
    bool isEqual(const LiteralType& left, const LiteralType& right);

    void checkNumberOperand(const Token& loxperator, const LiteralType& operand);
    void checkNumberOperands(const Token& loxperator, const LiteralType& left, const LiteralType& right);
};