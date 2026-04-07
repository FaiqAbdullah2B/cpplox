// src/Token.cpp
#include "Token.h"
#include "LoxCallable.h" // Now we can safely include the full definition!
#include <memory>

std::string stringifyLiteral(const LiteralType& literal) {
    return std::visit([](const auto &arg) -> std::string {
        using T = std::decay_t<decltype(arg)>;

        if constexpr (std::is_same_v<T, std::monostate>) {
            return "nil";
        } else if constexpr (std::is_same_v<T, std::string>) {
            return arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            return arg ? "true" : "false";
        } else if constexpr (std::is_same_v<T, double>) {
            std::string text = std::to_string(arg);
            text.erase(text.find_last_not_of('0') + 1, std::string::npos);
            if (text.back() == '.') {
                text.pop_back();
            }
            return text;
        } else if constexpr (std::is_same_v<T, std::shared_ptr<lox::LoxCallable>>) {
            // Now the compiler can see the full LoxCallable class and knows toString() exists!
            return arg ? arg->toString() : "nil";
        } else {
            return "unknown";
        }
    }, literal);
}