#include <string_view>
#include <Token.h>
#include <vector>
#include <variant>

class Scanner {
public:
    explicit Scanner(std::string_view source) : source(source) {}

    std::vector<Token> scanTokens();
private:
    std::string_view source;
    std::vector<Token> tokens;

    int start = 0;
    int current = 0;
    int line = 1;

    bool isAtEnd() {
        return current >= source.length();
    }

    void scanToken();
    char advance();

    void addToken(TokenType type) {
        addToken(type, std::monostate{});
    }

    void addToken(TokenType type, Literal literal) {
        std::string_view text = source.substr(start, current - start);
        tokens.emplace_back(type, std::string(text), literal, line);
    }
};