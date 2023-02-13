#pragma once

#include <fstream>
#include <string>
#include <map>
#include <cassert>

#include <boost/filesystem.hpp>

#include "utils.h"

class Lexer
{
public:
	Lexer(const std::string &);
	~Lexer();

	Token nextToken();

	int getLineNumber() const;
	int getLinePosition() const;
	std::string getLine() const;

private:
	std::ifstream file;
	std::string line;

	int lineNum;
	int linePos;

	Token::Type getTokenType(const std::string &);
};
