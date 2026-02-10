#include "Lox.h"

void Lox::runFile(std::string path) {
    Files handler;
    std::vector<std::byte> bytes = handler.readAllBytes(path);

    
}

void Lox::runPrompt() {

}