#pragma once
#include "LoxCallable.h"
#include "Stmt.h"
#include "Environment.h"
#include <string>

namespace lox {
    class Interpreter;

    class LoxFunction : public LoxCallable {
    private:
        const Function* declaration; 
        std::shared_ptr<Environment> closure; // The environment where the function was declared
    public:
        LoxFunction(const Function* declaration, std::shared_ptr<Environment> closure);

        int arity() override;
        LiteralType call(Interpreter& interpreter, std::vector<LiteralType> arguments) override;
        std::string toString() override;
    };
}