#pragma once

#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <variant>
#include <sstream>
#include <memory>
#include "Expr.h" 

namespace lox {

struct AstPrinter {
    // 1. Entry Point
    // Marked nodiscard because calling print() without using the string is a bug
    [[nodiscard]] std::string print(const lox::Expr& expr) const {
        return std::visit(*this, expr.value);
    }

    // 2. Visitor Overloads
    std::string operator()(const lox::Binary& expr) const {
        return parenthesize(expr.loxperator.lexeme, *expr.left, *expr.right);
    }

    std::string operator()(const lox::Grouping& expr) const {
        return parenthesize("group", *expr.expression);
    }

    std::string operator()(const lox::Literal& expr) const {
        struct LiteralVisitor {
            std::string operator()(std::monostate) const { return "nil"; }
            
            std::string operator()(double d) const {
                // Strips trailing zeros from floating point numbers
                std::string text = std::to_string(d);
                text.erase(text.find_last_not_of('0') + 1, std::string::npos);
                if (text.back() == '.') {
                    text.pop_back();
                }
                return text;
            }
            
            std::string operator()(const std::string& s) const { return s; }
            
            std::string operator()(bool b) const { return b ? "true" : "false"; }
            
            // The crucial missing branch for functions/classes!
            std::string operator()(const std::shared_ptr<lox::LoxCallable>& callable) const {
                return callable ? callable->toString() : "nil";
            }
        };
        return std::visit(LiteralVisitor{}, expr.value);
    }

    std::string operator()(const lox::Unary& expr) const {
        return parenthesize(expr.loxperator.lexeme, *expr.right);
    }

    std::string operator()(const lox::Ternary& expr) const {
        std::string opName = expr.op1.lexeme + expr.op2.lexeme; 
        return parenthesize(opName, *expr.left, *expr.middle, *expr.right);
    }

    std::string operator()(const lox::Variable& expr) const {
        return expr.name.lexeme;
    }

    std::string operator()(const lox::Assign& expr) const {
        return parenthesize("= " + expr.name.lexeme, *expr.value);
    }

    std::string operator()(const lox::Logical& expr) const {
        return parenthesize(expr.loxperator.lexeme, *expr.left, *expr.right);
    }

    // --- THE FIX: Custom handling for dynamic argument lists ---
    std::string operator()(const lox::Call& expr) const {
        std::stringstream ss;
        // Print the callee (the function name usually)
        ss << "(call " << std::visit(*this, expr.callee->value);
        
        // Loop through the variable number of arguments
        for (const auto& arg : expr.arguments) {
            ss << " " << std::visit(*this, arg->value);
        }
        ss << ")";
        return ss.str();
    }

private:
    // Helper to handle the recursion for fixed-arity expressions
    template <typename... Exprs>
    std::string parenthesize(std::string_view name, const Exprs&... parts) const {
        std::stringstream ss;
        ss << "(" << name;
        // This fold expression cleanly expands no matter how many 'parts' we pass
        ((ss << " " << std::visit(*this, parts.value)), ...);
        ss << ")";
        return ss.str();
    }
};

} // namespace lox
// #pragma once
// #include <iostream>
// #include <string>
// #include <vector>
// #include <variant>
// #include <sstream>
// #include "Expr.h" 

// struct AstPrinter {
//     // 1. Entry Point
//     std::string print(const lox::Expr& expr) {
//         return std::visit(*this, expr.value);
//     }

//     // 2. Visitor Overloads
//     std::string operator()(const lox::Binary& expr) const {
//         return parenthesize(expr.loxperator.lexeme, *expr.left, *expr.right);
//     }

//     std::string operator()(const lox::Grouping& expr) const {
//         return parenthesize("group", *expr.expression);
//     }

//     std::string operator()(const lox::Literal& expr) const {
//         struct LiteralVisitor {
//             std::string operator()(std::monostate) const { return "nil"; }
//             std::string operator()(double d) const { return std::to_string(d); }
//             std::string operator()(const std::string& s) const { return s; }
//             std::string operator()(bool b) const { return b ? "true" : "false"; }
//         };
//         return std::visit(LiteralVisitor{}, expr.value);
//     }

//     std::string operator()(const lox::Unary& expr) const {
//         return parenthesize(expr.loxperator.lexeme, *expr.right);
//     }

//     // --- NEW: Ternary Overload ---
//     std::string operator()(const lox::Ternary& expr) const {
//         // We combine the two operator lexemes (usually "?" and ":") to make "?:"
//         std::string opName = expr.op1.lexeme + expr.op2.lexeme; 
        
//         // Pass all three child expressions to the variadic helper!
//         return parenthesize(opName, *expr.left, *expr.middle, *expr.right);
//     }

//     std::string operator()(const lox::Variable& expr) const {
//     // For variables, we usually just print the name of the variable
//         return expr.name.lexeme;
//     }

//     std::string operator()(const lox::Assign& expr) const {
//     // We include the variable name in the "name" field of the parenthesize call
//     // format: (= <name> <value>)
//     return parenthesize("= " + expr.name.lexeme, *expr.value);
//     }

//     // Add this right next to your Binary visitor
//     std::string operator()(const lox::Logical& expr) const {
//         return parenthesize(expr.loxperator.lexeme, *expr.left, *expr.right);
//     }

//     std::string operator()(const lox::Call& expr) const {
//     std::vector<const lox::Expr*> parts;
//     parts.push_back(expr.callee.get());
//     for (const auto& arg : expr.arguments) {
//         parts.push_back(arg.get());
//     }
//     return parenthesize("call", parts);
// }

// private:
//     // Helper to handle the recursion
//     template <typename... Exprs>
//     std::string parenthesize(std::string_view name, const Exprs&... parts) const {
//         std::stringstream ss;
//         ss << "(" << name;
//         // This fold expression cleanly expands no matter how many 'parts' we pass
//         ((ss << " " << std::visit(*this, parts.value)), ...);
//         ss << ")";
//         return ss.str();
//     }
// };