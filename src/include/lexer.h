#pragma once

#include <fstream>
#include <string>
#include <cassert>

#include <boost/filesystem.hpp>

#include "utils.h"

class Lexer
{
public:
	explicit Lexer(const std::string &);
	Lexer();
	~Lexer();

	Token nextToken();
	Token readInput(const std::string &);

	int getLineNumber() const;
	int getLinePosition() const;
	std::string getLine() const;

private:
	std::ifstream file;
	std::string line;

	int lineNum;
	int linePos;

	static Token::Type getTokenType(const std::string &);
};
