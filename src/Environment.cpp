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

Environment* Environment::ancestor(int distance) {
    Environment* env = this;
    for (int i = 0; i < distance; i++) {
        // .get() grabs the raw pointer out of the shared_ptr without copying it
        env = env->enclosing.get(); 
    }
    return env;
}

LiteralType Environment::getAt(int distance, const std::string& name) {
    // The Resolver already PROVED this variable exists here. 
    // We don't even check, we just grab it.
    return ancestor(distance)->values[name]; 
}

void Environment::assignAt(int distance, const Token& name, LiteralType value) {
    ancestor(distance)->values[name.lexeme] = value;
}