#pragma once

#include <string>
#include <unordered_map>
#include <memory>

#include "Token.h"
#include "RuntimeError.h"

class Environment{
public:
    std::shared_ptr<Environment> enclosing;

    Environment() : enclosing(nullptr) {}

    explicit Environment(std::shared_ptr<Environment> enclosing) 
        : enclosing(std::move(enclosing)) {};

    void define(const std::string& name, const LiteralType& value);
    void assign(const Token& name, LiteralType value);

    LiteralType get(Token name);
private:
    std::unordered_map<std::string, LiteralType> values;
};