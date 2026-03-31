#include "Environment.h"

void Environment::define(const std::string& name, const LiteralType& value) {
    values[name] = std::move(value);
}

LiteralType Environment::get(Token name) {
    auto it = values.find(name.lexeme);
    if (it != values.end()) {
        return it->second;
    }

    throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
}