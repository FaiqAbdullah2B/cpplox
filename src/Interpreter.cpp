#include "Interpreter.h"

LiteralType Interpreter::evaluate(const lox::Expr& expr) {
    return std::visit(*this, expr.value);
}

LiteralType Interpreter::operator()(const lox::Literal& literal) {
    return literal.value;
}

LiteralType Interpreter::operator()(const lox::Grouping& grouping) {
    return evaluate(*grouping.expression);
}

LiteralType Interpreter::operator()(const lox::Unary& unary) {
    LiteralType right = evaluate(*unary.right);

    switch(unary.loxperator.type) {
        case TokenType::BANG:
            return !isTruthy(right);
        case TokenType::MINUS:
            // Assuming it's a double for now. 
            // We'll add the Lox RuntimeError checks here later.
            return -std::get<double>(right);
        default:
            break;
    }

    // Unreachable
    return std::monostate{};
}

// Stubs to keep std::visit happy until you write the logic
LiteralType Interpreter::operator()(const lox::Binary& binary) {
    return std::monostate{};
}

LiteralType Interpreter::operator()(const lox::Ternary& ternary) {
    return std::monostate{};
}

// The truthiness logic Lox uses
bool Interpreter::isTruthy(const LiteralType& val) {
    if (std::holds_alternative<std::monostate>(val)) return false;
    if (std::holds_alternative<bool>(val)) return std::get<bool>(val);
    return true; // Everything else (numbers, strings) is true in Lox
}