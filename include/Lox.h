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
    Lox(){}
    void runFile(std::string path); // the path of the file to be interpreted
    void runPrompt();

    static void error (int line, std::string message);
private:
    void run(std::string_view source);

    static bool hadError;
    static void report(int line, std::string where, std::string message);
};