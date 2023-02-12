#pragma once

#include "utils.h"

#include <regex>
#include <algorithm>

#include <curl/curl.h>

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

