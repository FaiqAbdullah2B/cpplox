#pragma once
#include <vector>
#include <memory>

#include <Token.h>

namespace lox {
    class Interpreter;

    class LoxCallable {
    public:
        virtual ~LoxCallable() = default;

        virtual LiteralType call(Interpreter&, std::vector<LiteralType>) = 0;

        virtual int arity() = 0;

        virtual std::string toString() = 0;
    };
};