#include <iostream>
#include <Lox.h>

int main (int argc, char* argv[]) {
    Lox lox;

    if (argc > 2) {
        std::cout << "Usage: " << argv[0] << " [script]\n";
        return 64; // EX_USAGE
    } else if (argc == 2) {
        try {
            lox.runFile(argv[1]);
        } catch(std::runtime_error &e) {
            std::cerr << e.what();
            return 1;
        }
        
    } else { 
        lox.runPrompt();
    }

    return 0;
}