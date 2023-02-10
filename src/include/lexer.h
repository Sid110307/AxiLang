#pragma once

/* Syntax:

Comments:
  %Line comment

  %=This is a
  block comment.
  Another line
  =%

Typical file structure:
  MODE P % The mode can be P (plot) or I (interactive).

  % Only if the mode is P:
  SETPLOT <path> % The path can be a file or an internet url.

  OPTS
  	% General options:
	ACCEL 75
	PENU_POS 60
	PEND_POS 30
	PENU_DELAY 0
 	PEND_DELAY 0
 	PENU_SPEED 75
 	PEND_SPEED 25
 	PENU_RATE 75
 	PEND_RATE 50
 	MODEL 1
 	PORT /dev/ttyACM0

 	% Interactive options:
 	UNITS 0
  END_OPTS

  % Only if the mode is P:
  PLOT

  % Only if the mode is I:
  CONNECT
  % ...
  DISCONNECT

  % In interactive mode (between CONNECT and DISCONNECT):
	  UOPTS
		% ...
	  END_UOPTS
	  PENU
	  PEND
	  PENTOGGLE

	  HOME
	  GOTO X Y
	  GOTO_REL X Y

	  DRAW X1 Y1 X2 Y2 X3 Y3 ... Xn Yn
	  WAIT T

	  GETPOS
	  GETPEN
*/

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
