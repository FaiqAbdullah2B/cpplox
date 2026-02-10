#pragma once
#include <string>
#include <vector>
#include <iostream>
#include "Files.h"

class Lox {
    public:
        void runFile(std::string path); // the path of the file to be interpreted
        void runPrompt();
    private:
};