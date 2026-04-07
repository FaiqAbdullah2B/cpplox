#include "LoxFunction.h"
#include "Interpreter.h" // We need the full interpreter here to execute

namespace lox {

    LoxFunction::LoxFunction(const Function* declaration) 
        : declaration(declaration) {}

    int LoxFunction::arity() {
        return declaration->params.size();
    }

    std::string LoxFunction::toString() {
        return "<fn " + declaration->name.lexeme + ">";
    }

    LiteralType LoxFunction::call(Interpreter& interpreter, std::vector<LiteralType> arguments) {
        // 1. Create a new environment. Right now, its parent is the globals.
        // (We will change this to 'closure' in Chapter 11, but globals is correct for now).
        auto environment = std::make_shared<Environment>(interpreter.globals);

        // 2. Bind the arguments to the parameter names
        for (size_t i = 0; i < declaration->params.size(); ++i) {
            environment->define(declaration->params[i].lexeme, std::move(arguments[i]));
        }

        // 3. Execute the function body using this new environment
        try {
            // Run the body. If it hits a return statement, it will throw!
            interpreter.executeBlock(declaration->body, environment);
        } 
        catch (const ReturnException& returnValue) {
            // We caught the eject seat! Return the payload to the caller.
            return returnValue.value;
        }

        // 4. If we don't hit a 'return' statement (which we haven't implemented yet),
        // Lox functions implicitly return nil.
        return std::monostate{};
    }

}