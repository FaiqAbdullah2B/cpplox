#include "Lox.h"

bool Lox::hadError = false;

void Lox::runFile(std::string path) {
    Files handler;
    std::vector<std::byte> bytes = handler.readAllBytes(path);

    // convert vector to String
    std::string_view view(reinterpret_cast<const char*>(bytes.data()), bytes.size());

    run(view);

    if (hadError)
        throw std::runtime_error("Error found in File");
}

void Lox::runPrompt() {
    std::string line;

    std::cout << "> ";

    while (std::getline(std::cin, line)) {
        if (!line.empty()) {
            run(line);
            hadError = false;
        }
        
        std::cout << "> ";
    }
    
    std::cout << "\n";
}

void Lox::run(std::string_view source) {
    Scanner scanner(source);
    std::vector<Token> tokens = scanner.scanTokens();

    for (Token token : tokens) {
        std::cout << token.toString() << "\n";
    }
}

void Lox::error (int line, std::string message){
    report(line, " ", message);
}

void Lox::report (int line, std::string where, std::string message) {
    std::cout << "[line " << line << "] Error" << where << ": " << message << "\n"; 
    hadError = true;
}