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

