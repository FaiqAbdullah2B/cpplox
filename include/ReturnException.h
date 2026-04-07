#pragma once
#include <stdexcept>
#include "Token.h" // Needed so it knows what a LiteralType is

namespace lox {
    // We inherit from std::exception so it plays nice with standard C++ try/catch rules
    class ReturnException : public std::exception {
    public:
        const LiteralType value;

        // Constructor moves the value into the exception to avoid unnecessary copies
        explicit ReturnException(LiteralType value) : value(std::move(value)) {}

        // Override what() just to be a good C++ citizen, though we won't print it
        const char* what() const noexcept override {
            return "Return exception used for control flow.";
        }
    };
}