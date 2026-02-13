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

        case '!':
            addToken(match('=') ? BANG_EQUAL : BANG);
            break;
        case '=':
            addToken(match('=') ? EQUAL_EQUAL : EQUAL);
            break;
        case '<':
            addToken(match('=') ? LESS_EQUAL : LESS);
            break;
        case '>':
            addToken(match('=') ? GREATER_EQUAL : GREATER);
            break;

        case '/':
            if (match('/')) {
            // A comment goes until the end of the line.
                while (peek() != '\n' && !isAtEnd()) 
                advance();
            } else {
                addToken(SLASH);
            }
            break;
    

        case ' ':
        case '\r':
        case '\t':
            // Ignore whitespace.
            break;

        case '\n':
            line++;
            break;
        
        case '"': scanString(); break;

        default: 
            if (isDigit(c)) {
                number();
            } else if (isAlpha(c)) {
                identifier();
            } else {
                Lox::error(line, "Unexpected character.");
            } 
            break;
    }
    
}

void Scanner::identifier() {
    
}

void Scanner::number() {
    while(isDigit(peek())) advance();

    // Look for a fractional part
    if (peek() == '.' && isDigit(peekNext())) {
        // consume the "."
        advance();

        while(isDigit(peek())) advance();
    }

    std::string_view lexemeView = source.substr(start, current - start);

    double value = std::stod(std::string{lexemeView});

    addToken(TokenType::NUMBER, value);
}
// this functions matches the next value and if it does, it increments current by 1
bool Scanner::match(char expected) {
    if (isAtEnd()) 
        return false;
    if (source[current] != expected)
        return false;
    
    current++;
    return true;
}

char Scanner::advance() {
    return source[current++];
}

char Scanner::peek() {
    if (isAtEnd()) return '\0';
    return source[current];
}

char Scanner::peekNext() {
    if (current + 1 >= source.length()) return '\0';
    return source[current+1];
}

bool Scanner::isDigit(char c) {
    return c >= '0' && c <= '9';
} 

bool Scanner::isAlpha(char c) {
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
            c == '_';
}

bool Scanner::isAlphaNumeric(char c) {
    return isAlpha(c) || isDigit(c);
}

void Scanner::scanString() {
    while (peek() != '"' && !isAtEnd()) {
      if (peek() == '\n') line++;
      advance();
    }

    if (isAtEnd()) {
      Lox::error(line, "Unterminated string.");
      return;
    }

    // The closing ".
    advance();

    // Trim the surrounding quotes.
    std::string value {source.substr(start + 1, current - start - 2)};
    addToken(TokenType::STRING, value);
}

