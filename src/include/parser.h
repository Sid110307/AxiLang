#pragma once

#include <fstream>
#include <string>
#include <regex>

#include "api.h"
#include "utils.h"

#include <boost/filesystem.hpp>
#include <curl/curl.h>

class Parser
{
public:
	Parser(FileState fileState, bool shouldExitOnError = true)
			: fileState(fileState), axiDraw(), isModeSet(false), isModePlot(false),
			  shouldExitOnError(shouldExitOnError) {}
	~Parser() = default;

	void parse();

private:
	FileState fileState;
	AxiDraw axiDraw;

	bool isModeSet;
	bool isModePlot;
	bool shouldExitOnError = true;

	void checkInteractive(std::string);
};

