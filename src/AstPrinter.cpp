#include <iostream>
#include <string>
#include <vector>
#include <variant>
#include <sstream>
#include "Expr.h" 


struct AstPrinter {
    // 1. Entry Point
    std::string print(const lox::Expr& expr) {
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
            std::string operator()(double d) const { return std::to_string(d); }
            std::string operator()(const std::string& s) const { return s; }
            std::string operator()(bool b) const { return b ? "true" : "false"; }
        };
        return std::visit(LiteralVisitor{}, expr.value);
    }

    std::string operator()(const lox::Unary& expr) const {
        return parenthesize(expr.loxperator.lexeme, *expr.right);
    }

private:
    // Helper to handle the recursion
    template <typename... Exprs>
    std::string parenthesize(std::string_view name, const Exprs&... parts) const {
        std::stringstream ss;
        ss << "(" << name;
        ((ss << " " << std::visit(*this, parts.value)), ...);
        ss << ")";
        return ss.str();
    }
};

int main() {
    // Build the tree: (* (- 123) (group 45.67))
    // We construct the Tokens INLINE now. No more missing variables.
    
    lox::Expr expression = lox::Binary(
        // Left: Unary (- 123)
        std::make_unique<lox::Expr>(lox::Unary(
            Token(TokenType::MINUS, "-", std::monostate{}, 1), 
            std::make_unique<lox::Expr>(lox::Literal(123.0))
        )),
        // Operator: *
        Token(TokenType::STAR, "*", std::monostate{}, 1),
        // Right: Grouping (45.67)
        std::make_unique<lox::Expr>(lox::Grouping(
            std::make_unique<lox::Expr>(lox::Literal(45.67))
        ))
    );

    AstPrinter printer;
    std::cout << printer.print(expression) << "\n";
}