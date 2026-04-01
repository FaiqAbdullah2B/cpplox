#include "Environment.h"

void Environment::define(const std::string& name, const LiteralType& value) {
    values[name] = std::move(value);
}

LiteralType Environment::get(Token name) {
    auto it = values.find(name.lexeme);

    if (it != values.end()) {
        return it->second;
    }

    if (enclosing != nullptr) return enclosing->get(name);

    throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
}

void Environment::assign(const Token& name, LiteralType value) {
    auto it = values.find(name.lexeme);

    if (it != values.end()) {
        it->second = std::move(value);
        return;
    }

    if (enclosing != nullptr) {
      enclosing->assign(name, value);
      return;
    }

    throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
}