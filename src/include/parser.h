#pragma once

#include <fstream>
#include <string>
#include <regex>

#include "api.h"
#include "utils.h"

#include <boost/filesystem.hpp>
#include <utility>
#include <curl/curl.h>

class Parser
{
public:
    explicit Parser(FileState fileState, bool shouldExitOnError = true)
            : fileState(std::move(fileState)), axiDraw(), isModeSet(false), isModePlot(false),
              shouldExitOnError(shouldExitOnError) {}
    void parse();

private:
    FileState fileState;
    AxiDraw axiDraw;

    bool isModeSet, isModePlot, shouldExitOnError = true;
    void checkInteractive(const std::string &);
};

