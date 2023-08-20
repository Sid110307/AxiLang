#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <csignal>

#include "lexer.h"
#include "parser.h"
#include "utils.h"

class Interpreter
{
public:
    Interpreter() = default;
    ~Interpreter() = default;

    void run();

private:
    std::string input;
    std::string prompt = "\033[1;32mAxiLang\033[0m>> ";
    std::vector<std::string> history;
    Lexer lexer;

    static void printHelp();
    void printHistory();
    void clearHistory();
};
