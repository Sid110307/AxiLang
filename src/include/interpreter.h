#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <csignal>

#include "lexer.h"
#include "parser.h"
#include "utils.h"

constexpr std::string_view PROMPT = "\033[1;32mAxiLang\033[0m>> ";

class Interpreter
{
public:
    Interpreter();
    void run();
private:
    std::string input;
    std::vector<std::string> history;

    Lexer lexer;
    FileState fileState;
    Parser parser;

    void printHistory();
    void clearHistory();
    static void printHelp();
    void execute(const std::string &);
};
