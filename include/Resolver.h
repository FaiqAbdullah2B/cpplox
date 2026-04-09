#pragma once
#include <vector>
#include <unordered_map>
#include <string>
#include <memory>

#include "Expr.h"
#include "Stmt.h"
#include "Interpreter.h"


namespace lox {

enum class FunctionType {
    NONE,
    FUNCTION
};

using StmtPtr = std::unique_ptr<Stmt>;
using ExprPtr = std::unique_ptr<Expr>;

class Resolver {
public:
    explicit Resolver(Interpreter& interpreterRef);

    // Entry points
    void resolve(const std::vector<StmtPtr>& statements);
    void resolve(const StmtPtr& stmt);
    void resolve(const ExprPtr& expr);

    // --- Statement Overloads ---
    void operator()(const lox::Block& stmt);
    void operator()(const lox::Var& stmt);
    void operator()(const lox::Function& stmt);
    void operator()(const lox::Expression& stmt);
    void operator()(const lox::If& stmt);
    void operator()(const lox::Print& stmt);
    void operator()(const lox::Return& stmt);
    void operator()(const lox::While& stmt);

    // --- Expression Overloads ---
    void operator()(const lox::Variable& expr);
    void operator()(const lox::Assign& expr);
    void operator()(const lox::Binary& expr);
    void operator()(const lox::Call& expr);
    void operator()(const lox::Grouping& expr);
    void operator()(const lox::Literal&);
    void operator()(const lox::Logical& expr);
    void operator()(const lox::Ternary& expr);
    void operator()(const lox::Unary& expr);


private:
    Interpreter& interpreter;
    std::vector<std::unordered_map<std::string, bool>> scopes;
    
    FunctionType currentFunction = FunctionType::NONE;
    
    // State tracker so we don't lose the wrapper's memory address during std::visit
    const lox::Expr* currentExpr = nullptr;

    void resolveFunction(const lox::Function& function, FunctionType type);
    void beginScope();
    void endScope();
    void declare(const Token& name);
    void define(const Token& name);
    void resolveLocal(const Token& name);

    // Friend class for RAII scope management
    friend class ScopeGuard;
};

class ScopeGuard {
public:
    explicit ScopeGuard(Resolver& resolver) : resolver(resolver) {
        resolver.beginScope();
    }
    ~ScopeGuard() {
        resolver.endScope();
    }
    // Delete copy/move semantics to prevent weird behavior
    ScopeGuard(const ScopeGuard&) = delete;
    ScopeGuard& operator=(const ScopeGuard&) = delete;
private:
    Resolver& resolver;
};

}
