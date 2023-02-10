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

#include <curl/curl.h>

#include "lexer.h"
#include "api.h"
#include "enumerator.h"

class Parser
{
public:
	Parser(FileState fileState) : fileState(fileState), axiDraw(), isModeSet(false) {}
	~Parser() = default;

	void parse();

private:
	FileState fileState;
	AxiDraw axiDraw;

	bool isModeSet;
	bool isModePlot;
	void checkInteractive(std::string);
};

