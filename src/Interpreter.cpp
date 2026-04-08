#include "Interpreter.h"
#include "Lox.h"

namespace lox {

void Interpreter::interpret(const std::vector<std::unique_ptr<lox::Stmt>>& statements) {
    try {
        for (const auto& stmt : statements) {
            execute(*stmt);
        }
    } catch (const RuntimeError& error) {      
        Lox::runtimeError(error); 
    }
}

LiteralType Interpreter::evaluate(const lox::Expr& expr) {
    return std::visit(*this, expr.value);
}

LiteralType Interpreter::execute(const lox::Stmt& stmt) {
    return std::visit(*this, stmt.value);
}

// LiteralType Interpreter::operator()(const lox::Block& block) {
//     std::shared_ptr<Environment> previous = environment;
//     try {
//         environment = std::make_shared<Environment>(previous);
//         for (const auto& stmt : block.statements) {
//             execute(*stmt);
//         }
//         environment = previous;
//     } catch (...) {
//         environment = previous; // Restore even on error!
//         throw; // Re-throw so interpret() can catch it
//     }
//     return std::monostate{};
// }

// The new helper method
void Interpreter::executeBlock(const std::vector<std::unique_ptr<Stmt>>& statements, 
                               std::shared_ptr<Environment> env) {
    std::shared_ptr<Environment> previous = this->environment;
    try {
        this->environment = env;
        for (const auto& stmt : statements) {
            execute(*stmt);
        }
        this->environment = previous;
    } catch (...) {
        this->environment = previous;
        throw;
    }
}

// Your existing visitor now looks like this:
LiteralType Interpreter::operator()(const lox::Block& block) {
    executeBlock(block.statements, std::make_shared<Environment>(environment));
    return std::monostate{};
}

LiteralType Interpreter::operator()(const lox::Expression& expression) {
    return evaluate(*expression.expression);
}

LiteralType Interpreter::operator()(const lox::Function& function) {
    auto loxFunction = std::make_shared<lox::LoxFunction>(&function, this->environment);
    this->environment->define(function.name.lexeme, loxFunction);
    return std::monostate{};
}

LiteralType Interpreter::operator()(const lox::If& ifStmt) {
    LiteralType condition = evaluate(*ifStmt.condition);

    if (isTruthy(condition)) {
        return execute(*ifStmt.thenBranch);
    } else if (ifStmt.elseBranch) {
        return execute(*ifStmt.elseBranch);
    }

    return std::monostate{};
}

LiteralType Interpreter::operator()(const lox::Print& print) {
    LiteralType value = evaluate(*print.expression);
    std::cout << stringifyLiteral(value) << std::endl;
    return std::monostate{};
}

LiteralType Interpreter::operator()(const lox::Return& returnStmt) {
    LiteralType value = std::monostate{}; // Default to nil

    // If there is a value attached to the return, evaluate it
    if (returnStmt.value != nullptr) {
        value = evaluate(*returnStmt.value);
    }

    // Throw the exception to instantly break out of the function body
    throw ReturnException(std::move(value));
}

LiteralType Interpreter::operator()(const lox::Var& variable) {
    LiteralType value = std::monostate{};
    if (variable.initializer) {
        value = evaluate(*variable.initializer);
    }
    environment->define(variable.name.lexeme, value);
    return std::monostate{};
}

LiteralType Interpreter::operator()(const lox::While& whileStmt) {
    while (isTruthy(evaluate(*whileStmt.condition))) {
        execute(*whileStmt.body);
    }
    return std::monostate{};
}

LiteralType Interpreter::operator()(const lox::Assign& assign) {
    LiteralType value = evaluate(*assign.value);
    environment->assign(assign.name, value);
    return value;
}

LiteralType Interpreter::operator()(const lox::Literal& literal) {
    return literal.value;
}

LiteralType Interpreter::operator()(const lox::Logical& logical) {
    LiteralType left = evaluate(*logical.left);

    if (logical.loxperator.type == TokenType::OR) {
        if (isTruthy(left)) return left;
    } else {
        if (!isTruthy(left)) return left;
    }

    return evaluate(*logical.right);
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

LiteralType Interpreter::operator()(const lox::Call& call) {
    LiteralType callee = evaluate(*call.callee);

    std::vector<LiteralType> arguments{};
    for (const auto &arg : call.arguments) {
        arguments.push_back(evaluate(*arg));
    }

    if (!std::holds_alternative<std::shared_ptr<lox::LoxCallable>>(callee)) {
        throw RuntimeError(call.paren, "Can only call function and classes.");
    }

    auto function = std::get<std::shared_ptr<lox::LoxCallable>>(callee);

    if (arguments.size() != static_cast<size_t>(function->arity())) {
        throw RuntimeError(call.paren, "Expected " + std::to_string(function->arity()) + 
                           " arguments but got " + std::to_string(arguments.size()) + ".");
    }

    return function->call(*this, std::move(arguments));
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

            if (std::holds_alternative<std::string>(left) || 
                std::holds_alternative<std::string>(right)) {
                return stringifyLiteral(left) + stringifyLiteral(right);
            }
        
            throw RuntimeError(binary.loxperator, "Operands must be both numbers or both strings.");
            break;
        case TokenType::SLASH:
            checkNumberOperands(binary.loxperator, left, right);
            if (std::get<double>(right) == 0.0) {
                throw RuntimeError(binary.loxperator, "Division by zero error.");
            }
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

LiteralType Interpreter::operator()(const lox::Variable& variable) {
    return environment->get(variable.name);
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

}
