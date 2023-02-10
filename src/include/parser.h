#pragma once

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

