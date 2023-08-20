#pragma once

#include <fstream>
#include <string>
#include <cassert>
#include <sstream>

#include <boost/filesystem.hpp>

#include "utils.h"

class Lexer
{
public:
    explicit Lexer(const std::string &);
    Lexer();
    ~Lexer();

    Token nextToken();
    std::vector<Token> lexInput(const std::string &);

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
