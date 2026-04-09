#include "Resolver.h"
#include "Lox.h" // Assuming this is where your global error() function lives

namespace lox {

Resolver::Resolver(Interpreter& interpreterRef) : interpreter(interpreterRef) {}

// --- Core Traversals ---

void Resolver::resolve(const std::vector<StmtPtr>& statements) {
    for (const auto& statement : statements) {
        resolve(statement);
    }
}

void Resolver::resolve(const StmtPtr& stmt) {
    if (stmt) {
        std::visit(*this, stmt->value);
    }
}

void Resolver::resolve(const ExprPtr& expr) {
    if (expr) {
        const lox::Expr* previousExpr = currentExpr;
        currentExpr = expr.get();
        
        std::visit(*this, expr->value);
        
        currentExpr = previousExpr;
    }
}

// --- Scope Management ---

void Resolver::beginScope() {
    scopes.emplace_back();
}

void Resolver::endScope() {
    scopes.pop_back();
}

void Resolver::declare(const Token& name) {
    if (scopes.empty()) return;

    auto& scope = scopes.back();
    
    if (scope.contains(name.lexeme)) {
        Lox::error(name, "Already a variable with this name in this scope.");
    }

    scope[name.lexeme] = false; // "false" means declared but not yet initialized
}

void Resolver::define(const Token& name) {
    if (scopes.empty()) return;
    scopes.back()[name.lexeme] = true; // Fully initialized
}

void Resolver::resolveLocal(const Token& name) {
    // Walk backwards through the stack (inner scope to outer scope)
    for (int i = scopes.size() - 1; i >= 0; i--) {
        if (scopes[i].contains(name.lexeme)) {
            // Pass the exact memory address of the AST wrapper to the interpreter
            interpreter.resolve(currentExpr, scopes.size() - 1 - i);
            return;
        }
    }
    // Not found. Assume it is global.
}

// --- Statements ---

void Resolver::operator()(const lox::Block& stmt) {
    // RAII magic: Scope is pushed here, and automatically popped when this function exits
    ScopeGuard guard(*this);
    resolve(stmt.statements);
}

void Resolver::operator()(const lox::Var& stmt) {
    declare(stmt.name);
    if (stmt.initializer) {
        resolve(stmt.initializer);
    }
    define(stmt.name);
}

void Resolver::operator()(const lox::Function& stmt) {
    declare(stmt.name);
    define(stmt.name); // Define immediately to allow recursion
    resolveFunction(stmt, FunctionType::FUNCTION);
}

void Resolver::operator()(const lox::Expression& stmt) {
    resolve(stmt.expression);
}

void Resolver::operator()(const lox::If& stmt) {
    resolve(stmt.condition);
    resolve(stmt.thenBranch);
    if (stmt.elseBranch) resolve(stmt.elseBranch);
}

void Resolver::operator()(const lox::Print& stmt) {
    resolve(stmt.expression);
}

void Resolver::operator()(const lox::Return& stmt) {
    if (currentFunction == FunctionType::NONE) {
        Lox::error(stmt.keyword, "Can't return from top-level code.");
    }
    if (stmt.value) {
        resolve(stmt.value);
    }
}

void Resolver::operator()(const lox::While& stmt) {
    resolve(stmt.condition);
    resolve(stmt.body);
}

// --- Expressions ---

void Resolver::operator()(const lox::Variable& expr) {
    if (!scopes.empty() && 
        scopes.back().contains(expr.name.lexeme) && 
        scopes.back()[expr.name.lexeme] == false) {
        Lox::error(expr.name, "Can't read local variable in its own initializer.");
    }

    resolveLocal(expr.name);
}

void Resolver::operator()(const lox::Assign& expr) {
    resolve(expr.value);
    resolveLocal(expr.name);
}

void Resolver::operator()(const lox::Binary& expr) {
    resolve(expr.left);
    resolve(expr.right);
}

void Resolver::operator()(const lox::Call& expr) {
    resolve(expr.callee);
    for (const auto& arg : expr.arguments) {
        resolve(arg);
    }
}

void Resolver::operator()(const lox::Grouping& expr) {
    resolve(expr.expression);
}

void Resolver::operator()(const lox::Literal&) {
    // Leaf node, nothing to resolve.
}

void Resolver::operator()(const lox::Logical& expr) {
    resolve(expr.left);
    resolve(expr.right);
}

void Resolver::operator()(const lox::Ternary& expr) {
    resolve(expr.left);
    resolve(expr.middle);
    resolve(expr.right);
}

void Resolver::operator()(const lox::Unary& expr) {
    resolve(expr.right);
}

// --- Helpers ---

void Resolver::resolveFunction(const lox::Function& function, FunctionType type) {
    FunctionType enclosingFunction = currentFunction;
    currentFunction = type;

    ScopeGuard guard(*this); // Safe scoping for the function body
    for (const auto& param : function.params) {
        declare(param);
        define(param);
    }
    resolve(function.body);
    
    currentFunction = enclosingFunction;
}

} // namespace lox