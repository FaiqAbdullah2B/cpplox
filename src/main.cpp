#include <iostream>
#include <Lox.h>

int main (int argc, char* argv[]) {
    Lox lox;

    if (argc > 2) {
        std::cout << "Usage: " << argv[0] << " [script]\n";
        return 64; // EX_USAGE
    } else if (argc == 2) {
        lox.runFile(argv[1]);
    } else { 
        lox.runPrompt();
    }

    return 0;
}