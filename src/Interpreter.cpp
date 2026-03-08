#include "Interpreter.h"

void Interpreter::interpret(const lox::Expr& expression) {
    try {
        LiteralType value = evaluate(expression);
        std::cout << stringifyLiteral(value) << std::endl;
    } catch (const RuntimeError& error) {      
        Lox::runtimeError(error); 
    }
}

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
            checkNumberOperand(unary.loxperator, right);
            return -std::get<double>(right);
        default:
            break;
    }

    // Unreachable
    return std::monostate{};
}

// Stubs to keep std::visit happy until you write the logic
LiteralType Interpreter::operator()(const lox::Binary& binary) {
    LiteralType left = evaluate(*binary.left);
    LiteralType right = evaluate(*binary.right);

    switch (binary.loxperator.type) {
        // --- Arithmetic Operators ---
        case TokenType::MINUS:
            checkNumberOperands(binary.loxperator, left, right);
            return std::get<double>(left) - std::get<double>(right);
        case TokenType::PLUS:
            if (std::holds_alternative<double>(left) &&
            std::holds_alternative<double>(right)) {
                return std::get<double>(left) + std::get<double>(right);
            }
            if (std::holds_alternative<std::string>(left) &&
            std::holds_alternative<std::string>(right)) {
                return std::get<std::string>(left) + std::get<std::string>(right);
            }
            throw RuntimeError(binary.loxperator, "Operands must be both numbers or both strings.");
            break;
        case TokenType::SLASH:
            checkNumberOperands(binary.loxperator, left, right);
            return std::get<double>(left) / std::get<double>(right);
        case TokenType::STAR:
            checkNumberOperands(binary.loxperator, left, right);
            return std::get<double>(left) * std::get<double>(right);

        // --- Comparison Operators ---
        case TokenType::GREATER:
            checkNumberOperands(binary.loxperator, left, right);
            return std::get<double>(left) > std::get<double>(right);
        case TokenType::GREATER_EQUAL:
            checkNumberOperands(binary.loxperator, left, right);
            return std::get<double>(left) >= std::get<double>(right);
        case TokenType::LESS:
            checkNumberOperands(binary.loxperator, left, right);
            return std::get<double>(left) < std::get<double>(right);
        case TokenType::LESS_EQUAL:
            checkNumberOperands(binary.loxperator, left, right);
            return std::get<double>(left) <= std::get<double>(right);

        // --- Equality Operators ---
        case TokenType::BANG_EQUAL:
            return !isEqual(left, right);
        case TokenType::EQUAL_EQUAL:
            return isEqual(left, right);
        default:
            break;
    }

    
    return std::monostate{};
}

LiteralType Interpreter::operator()(const lox::Ternary& ternary) {
    LiteralType condition = evaluate(*ternary.left);

    if (isTruthy(condition)) {
        return evaluate(*ternary.middle);
    } else {
        return evaluate(*ternary.right);
    }
}

bool Interpreter::isTruthy(const LiteralType& val) {
    if (std::holds_alternative<std::monostate>(val)) return false;
    if (std::holds_alternative<bool>(val)) return std::get<bool>(val);
    return true; 
}

bool Interpreter::isEqual(const LiteralType& a, const LiteralType& b) {
    return a == b;
}

void Interpreter::checkNumberOperand(const Token& loxperator, const LiteralType& operand) {
    if (std::holds_alternative<double>(operand)) return;
    throw RuntimeError(loxperator, "Operand must be a number.");
}

void Interpreter::checkNumberOperands(const Token& loxperator, const LiteralType& left, const LiteralType& right) {
    if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right)) return;
    throw RuntimeError(loxperator, "Operands must be numbers.");
}