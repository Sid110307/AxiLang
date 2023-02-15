// Create a command-line interpreter that reads input, lexes, and parses it.
// Also, add support for multi-line input.
// There should also be support for history of commands, viewing and clearing history too.
// The interpreter should also be able to print out help messages.
// The interpreter should also be able to exit.

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
	String input;
	std::string prompt = "\033[1;32mAxiLang\033[0m>> ";
	std::vector<std::string> history;
	Lexer lexer;

	void printHelp();
	void printHistory();
	void clearHistory();
};
