#pragma once
#include <variant>

#include "Token.h"
#include "Expr.h"
#include "RuntimeError.h"
#include "Lox.h"

class Interpreter {
public:
    void interpret(const lox::Expr& expression);

    LiteralType evaluate(const lox::Expr& expr);

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