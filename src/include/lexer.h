#pragma once

#include "api.h"
#include <iostream>
#include <fstream>
#include <string>
#include <utility>
#include <vector>

#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

struct Token
{
	enum Type
	{
		// General commands
		Mode,
		Opts,
		EndOpts,
		UOpts,
		EndUOpts,

		// Modes
		PlotMode,
		InteractiveMode,

		// General options
		Acceleration,
		PenUpPosition,
		PenDownPosition,
		PenUpDelay,
		PenDownDelay,
		PenUpSpeed,
		PenDownSpeed,
		PenUpRate,
		PenDownRate,
		Model,
		Port,

		// Interactive options
		Units,

		// Interactive commands
		Connect,
		Disconnect,
		PenUp,
		PenDown,
		PenToggle,
		Home,
		GoTo,
		GoToRelative,
		Draw,
		Wait,
		GetPos,
		GetPen,

		// Plot commands
		SetPlot,
		Plot,

		// Data types
		Number,
		String,

		// Other
		Unknown,
		EndOfFile
	};

	Type type;
	std::string value;

	Token(Type type, std::string value) : type(type), value(std::move(value)) {}
	std::string typeToCStr();
};

struct FileState
{
	std::vector<Token> tokens;
	std::vector<std::string> lines;
	std::vector<int> lineNums;
	std::vector<int> linePositions;
};

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
