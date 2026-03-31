#pragma once

#include <string>
#include <unordered_map>

#include "Token.h"
#include "RuntimeError.h"

class Environment{
public:
    void define(const std::string& name, const LiteralType& value);
    LiteralType get(Token name);
private:
    std::unordered_map<std::string, LiteralType> values;
};