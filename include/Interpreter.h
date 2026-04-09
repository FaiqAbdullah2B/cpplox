#pragma once
#include <variant>
#include <vector>
#include <chrono> 

#include "Token.h"
#include "unordered_map"
#include "Expr.h"
#include "RuntimeError.h"
#include "Stmt.h"
#include "Environment.h"
#include "LoxFunction.h"
#include "ReturnException.h"

namespace lox{

class Interpreter {
public:

    const lox::Expr* currentExpr = nullptr;

    std::shared_ptr<Environment> globals = std::make_shared<Environment>();
    std::shared_ptr<Environment> environment = globals;

    Interpreter() {
        struct ClockCallable : public LoxCallable {
            int arity() override { return 0; }

            LiteralType call(Interpreter&, std::vector<LiteralType>) override {
                auto now = std::chrono::system_clock::now().time_since_epoch();
                double seconds = std::chrono::duration<double>(now).count();
                return seconds;
            }

            std::string toString() override { return "<native fn>"; }
        };

        globals->define("clock", std::make_shared<ClockCallable>());
    }
    void interpret(const std::vector<std::unique_ptr<lox::Stmt>>& statements);

    void executeBlock(const std::vector<std::unique_ptr<Stmt>>& statements, 
                          std::shared_ptr<Environment> env);

    LiteralType evaluate(const lox::Expr& expr);
    LiteralType execute(const lox::Stmt& stmt);

    LiteralType operator()(const lox::Block& block);
    LiteralType operator()(const lox::Expression& expression);
    LiteralType operator()(const lox::Function& function);
    LiteralType operator()(const lox::If& ifStmt);
    LiteralType operator()(const lox::Print& print);
    LiteralType operator()(const lox::Return& returnStmt);
    LiteralType operator()(const lox::Var& variable);
    LiteralType operator()(const lox::While& whileStmt);

    LiteralType operator()(const lox::Assign& assign);
    LiteralType operator()(const lox::Literal& literal);
    LiteralType operator()(const lox::Logical& logical);
    LiteralType operator()(const lox::Grouping& grouping);
    LiteralType operator()(const lox::Unary& unary);
    LiteralType operator()(const lox::Call& call);
    LiteralType operator()(const lox::Binary& binary);
    LiteralType operator()(const lox::Ternary& ternary);
    LiteralType operator()(const lox::Variable& variable);
    
    std::unordered_map<const lox::Expr*, int> locals;
    void resolve(const lox::Expr* expr, int depth);

    LiteralType lookUpVariable(const Token& name, const lox::Expr* expr);

private:
    // Helper methods
    bool isTruthy(const LiteralType& val);
    bool isEqual(const LiteralType& left, const LiteralType& right);

    void checkNumberOperand(const Token& loxperator, const LiteralType& operand);
    void checkNumberOperands(const Token& loxperator, const LiteralType& left, const LiteralType& right);

    

};

};