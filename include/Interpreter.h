#pragma once
#include <variant>

#include "Token.h"
#include "Expr.h"

class Interpreter {
public:
    LiteralType evaluate(const lox::Expr& expr);

    
    LiteralType operator()(const lox::Literal& literal);
    LiteralType operator()(const lox::Grouping& grouping);
    LiteralType operator()(const lox::Unary& unary);
    LiteralType operator()(const lox::Binary& binary);
    LiteralType operator()(const lox::Ternary& ternary);

private:
    // Helper methods
    bool isTruthy(const LiteralType& val);
};