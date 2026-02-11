#include <string_view>
#include <Token.h>
#include <vector>

class Scanner {
public:
    explicit Scanner(std::string_view source) : source(source) {}

    std::vector<Token> scanTokens();
private:
    std::string_view source;
};