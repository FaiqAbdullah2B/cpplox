#pragma once
#include <string_view>
#include <unordered_map>
#include <vector>
#include <variant>

#include <Lox.h>
#include <Token.h>


class Scanner {
public:
    explicit Scanner(std::string_view source) : source(source) {}

    std::vector<Token> scanTokens();
private:
    
    std::string_view source;
    std::vector<Token> tokens;

    static const std::unordered_map<std::string, TokenType> keywords;

    long unsigned int start = 0;
    long unsigned int current = 0;
    int line = 1;

    bool isAtEnd() {
        return current >= source.length();
    }

    void scanToken();

    void identifier();
    void number();
    char advance();

    void addToken(TokenType type) {
        addToken(type, std::monostate{});
    }

    void addToken(TokenType type, Literal literal) {
        std::string_view text = source.substr(start, current - start);
        tokens.emplace_back(type, std::string(text), literal, line);
    }

    bool match(char expected);
    char peek();
    char peekNext();
    bool isAlpha(char c);
    bool isDigit(char c);
    bool isAlphaNumeric(char c);
    void scanString();
};