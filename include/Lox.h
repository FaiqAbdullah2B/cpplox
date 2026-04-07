#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <string_view>
#include <stdexcept>

#include "Files.h"
#include "Scanner.h"
#include "Token.h"
#include "Parser.h"
#include "RuntimeError.h"
#include "Interpreter.h"
#include "Stmt.h"

class Lox {
public:
    Lox(){}
    void runFile(std::string path); // the path of the file to be interpreted
    void runPrompt();

    static void error (int line, std::string message);
    static void error (const Token& token, const std::string& message);
    static void runtimeError(const RuntimeError& error);
private:
    lox::Interpreter interpreter;
    
    void run(std::string_view source);

    static bool hadError;
    static bool hadRuntimeError;
    static void report(int line, std::string where, std::string message);
};