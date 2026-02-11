#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <string_view>

#include <Files.h>
#include <Scanner.h>
#include <Token.h>

class Lox {
public:
    void runFile(std::string path); // the path of the file to be interpreted
    void runPrompt();

    void error (int line, std::string message);
private:
    void run(std::string_view source);

    void report(int line, std::string where, std::string message);
};