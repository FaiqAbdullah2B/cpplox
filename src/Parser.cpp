#include "Parser.h"
using namespace lox;

std::vector<std::unique_ptr<Stmt>> Parser::parse() {
    std::vector<std::unique_ptr<Stmt>> statements;
    while (!isAtEnd()) {
        statements.push_back(declaration());
    }

    return statements;
}

std::unique_ptr<Stmt> Parser::declaration() {
    try {
        if (match({TokenType::VAR})) {
            return varDeclaration();
        }
        return statement();
    } catch (const ParseError& error) {
        synchronize();
        return nullptr;
    }
}

std::unique_ptr<Stmt> Parser::varDeclaration() {
    Token name = consume(TokenType::IDENTIFIER, "Expect variable name.");

    std::unique_ptr<Expr> initializer = nullptr;
    if (match({TokenType::EQUAL})) {
        initializer = expression();
    }

    consume(TokenType::SEMICOLON, "Expect ';' after variable declaration.");
    return std::make_unique<Stmt>(
        Var(name, std::move(initializer))
    );
}

std::unique_ptr<Stmt> Parser::statement() {
    if (match({TokenType::PRINT}))
        return printStatement();
    
    if (match({TokenType::LEFT_BRACE})) {
        return std::make_unique<Stmt>(
            Block(block())
        );
    }
    return expressionStatement();

}

std::vector<std::unique_ptr<Stmt>> Parser::block() {
    std::vector<std::unique_ptr<Stmt>> statements;

    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        statements.push_back(declaration());
    }

    consume(TokenType::RIGHT_BRACE, "Expect '}' after block.");
    return statements;
}

std::unique_ptr<Stmt> Parser::printStatement() {
    std::unique_ptr<Expr> value = expression();
    consume(TokenType::SEMICOLON, "Expect ; after value.");
    return std::make_unique<Stmt>(
        Print(std::move(value))
    );
}

std::unique_ptr<Stmt> Parser::expressionStatement() {
    std::unique_ptr<Expr> value = expression();
    consume(TokenType::SEMICOLON, "Expect ; after expression.");
    return std::make_unique<Stmt>(
        Expression(std::move(value))
    );
}

std::unique_ptr<lox::Expr> Parser::expression() {
    return comma();
}

std::unique_ptr<Expr> Parser::comma() {
    std::unique_ptr<Expr> expr = assignment();

    while (match({TokenType::COMMA})) {
        Token op = previous();
        std::unique_ptr<lox::Expr> right = assignment();
        expr = std::make_unique<lox::Expr>(
            Binary(std::move(expr), op, std::move(right))
        );
    }

    return expr;
}

std::unique_ptr<Expr> Parser::assignment() {
    std::unique_ptr<Expr> expr = ternary();

    if (match({TokenType::EQUAL})) {
        Token equals = previous();
        std::unique_ptr<Expr> value = assignment();

        if (std::holds_alternative<Variable>(expr->value)) {
            Token name = std::get<Variable>(expr->value).name;
            return std::make_unique<lox::Expr>(
                Assign(name, std::move(value))
            );
        }

        error(equals, "Invalid assignment target.");
    }

    return expr;
}

std::unique_ptr<Expr> Parser::ternary() {
    std::unique_ptr<Expr> expr = equality();

    if (match({TokenType::Q_MARK})) {
        Token op1 = previous();
        std::unique_ptr<Expr> middle = expression();
        Token op2 = consume(TokenType::COLON, "Expect ':' after expression");
        std::unique_ptr<Expr> right = ternary();
        expr = std::make_unique<Expr>(
            Ternary(std::move(expr), op1, std::move(middle), op2, std::move(right))
        );
    }

    return expr;
}

std::unique_ptr<lox::Expr> Parser::equality() {
    std::unique_ptr<lox::Expr> expr = comparison();

    while (match({TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL})) {
        Token op = previous();
        std::unique_ptr<lox::Expr> right = comparison();
        expr = std::make_unique<lox::Expr>(
            Binary(std::move(expr), op, std::move(right))
        );
    }

    return expr;
}

std::unique_ptr<lox::Expr> Parser::comparison() {
    std::unique_ptr<lox::Expr> expr = term();

    while (match({TokenType::GREATER, TokenType::GREATER_EQUAL,
                  TokenType::LESS, TokenType::LESS_EQUAL})) {
        Token op = previous();
        std::unique_ptr<lox::Expr> right = term();
        expr = std::make_unique<lox::Expr>(
            Binary(std::move(expr), op, std::move(right))
        );
    }

    return expr;
}

std::unique_ptr<lox::Expr> Parser::term() {
    std::unique_ptr<lox::Expr> expr = factor();

    while (match({TokenType::MINUS, TokenType::PLUS})) {
        Token op = previous();
        std::unique_ptr<lox::Expr> right = factor();
        expr = std::make_unique<lox::Expr>(
            Binary(std::move(expr), op, std::move(right))
        );
    }

    return expr;
}

std::unique_ptr<lox::Expr> Parser::factor() {
    std::unique_ptr<lox::Expr> expr = unary();

    while (match({TokenType::SLASH, TokenType::STAR})) {
        Token op = previous();
        std::unique_ptr<lox::Expr> right = unary();
        expr = std::make_unique<lox::Expr>(
            Binary(std::move(expr), op, std::move(right))
        );
    }

    return expr;
}

std::unique_ptr<lox::Expr> Parser::unary() {
    if (match({TokenType::BANG, TokenType::MINUS})) {
        Token op = previous();
        std::unique_ptr<lox::Expr> right = unary();
        return std::make_unique<lox::Expr>(
            Unary(op, std::move(right))
        );
    }

    return primary();
}

std::unique_ptr<lox::Expr> Parser::primary() {
    if (match({TokenType::FALSE})) {
        return std::make_unique<lox::Expr>(
            Literal(false)
        );
    }
    if (match({TokenType::TRUE})) {
        return std::make_unique<lox::Expr>(
            Literal(true)
        );
    }
    if (match({TokenType::NIL})) {
        return std::make_unique<lox::Expr>(
            Literal(std::monostate{})
        );
    }

    if (match({TokenType::NUMBER, TokenType::STRING})) {
        return std::make_unique<lox::Expr>(
            Literal(previous().literal)
        );
    }

    if (match({TokenType::LEFT_PAREN})) {
        std::unique_ptr<lox::Expr> expr = expression();
        consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
        return std::make_unique<lox::Expr>(
            Grouping(std::move(expr))
        );
    }

    if (match({TokenType::IDENTIFIER})) {
        return std::make_unique<lox::Expr>(
            Variable(previous())
        );
    }

    throw error(peek(), "Expect Expression.");
}

bool Parser::match(std::initializer_list<TokenType> types) {
    for (TokenType type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

bool Parser::check(TokenType type) {
    if (isAtEnd()) return false;
    return peek().type == type; 
}

Token Parser::advance() {
    if (!isAtEnd()) current++;
    return previous();
}

bool Parser::isAtEnd() const {
    return peek().type == TokenType::EOF_TOKEN;
}

Token Parser::peek() const {
    return tokens.at(current);
}

Token Parser::previous() const {
    return tokens.at(current-1);
}

Token Parser::consume(TokenType type, std::string message) {
    if (check(type)) return advance();
    
    throw error(peek(), message);
}

Parser::ParseError Parser::error(const Token& token, std::string_view message) {
    Lox::error(token, std::string(message));
    
    return ParseError();
}

void Parser::synchronize() {
    advance();

    while(!isAtEnd()) {
        if (previous().type == TokenType::SEMICOLON) return;

        switch (peek().type) {
            case TokenType::CLASS:
            case TokenType::FUN:
            case TokenType::VAR:
            case TokenType::FOR:
            case TokenType::IF:
            case TokenType::WHILE:
            case TokenType::PRINT:
            case TokenType::RETURN:
                return;
            default:
                break;
        }

        advance();
    }
}