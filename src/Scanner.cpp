#include <Scanner.h>

std::vector<Token> Scanner::scanTokens() {
    while (!isAtEnd()) {
        // We are at the begging of the next Lexeme
        start = current;
        scanToken();
    }

    tokens.emplace_back(TokenType::EOF_TOKEN, "", std::monostate{}, line);

    return tokens;
};

void Scanner::scanToken() {
    using enum TokenType;
    
    char c = advance();
    switch (c) {
        case '(': addToken(LEFT_PAREN); break;
        case ')': addToken(RIGHT_PAREN); break;
        case '{': addToken(LEFT_BRACE); break;
        case '}': addToken(RIGHT_BRACE); break;
        case ',': addToken(COMMA); break;
        case '.': addToken(DOT); break;
        case '-': addToken(MINUS); break;
        case '+': addToken(PLUS); break;
        case ';': addToken(SEMICOLON); break;
        case '*': addToken(STAR); break; 
    }
}

char Scanner::advance() {

}