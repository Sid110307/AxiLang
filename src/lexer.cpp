#include "include/lexer.h"

std::string Token::typeToCStr() const
{
    assert(Token::Type::EndOfFile == 36);

    static const std::map<Token::Type, std::string> tokenMap = {
            {Token::Type::Mode,            "Mode"},
            {Token::Type::Opts,            "Opts"},
            {Token::Type::EndOpts,         "EndOpts"},
            {Token::Type::UOpts,           "UOpts"},
            {Token::Type::EndUOpts,        "EndUOpts"},
            {Token::Type::PlotMode,        "PlotMode"},
            {Token::Type::InteractiveMode, "InteractiveMode"},
            {Token::Type::Acceleration,    "Acceleration"},
            {Token::Type::PenUpPosition,   "PenUpPosition"},
            {Token::Type::PenDownPosition, "PenDownPosition"},
            {Token::Type::PenUpDelay,      "PenUpDelay"},
            {Token::Type::PenDownDelay,    "PenDownDelay"},
            {Token::Type::PenUpSpeed,      "PenUpSpeed"},
            {Token::Type::PenDownSpeed,    "PenDownSpeed"},
            {Token::Type::PenUpRate,       "PenUpRate"},
            {Token::Type::PenDownRate,     "PenDownRate"},
            {Token::Type::Model,           "Model"},
            {Token::Type::Port,            "Port"},
            {Token::Type::Units,           "Units"},
            {Token::Type::Connect,         "Connect"},
            {Token::Type::Disconnect,      "Disconnect"},
            {Token::Type::PenUp,           "PenUp"},
            {Token::Type::PenDown,         "PenDown"},
            {Token::Type::PenToggle,       "PenToggle"},
            {Token::Type::Home,            "Home"},
            {Token::Type::GoTo,            "GoTo"},
            {Token::Type::GoToRelative,    "GoToRelative"},
            {Token::Type::Draw,            "Draw"},
            {Token::Type::Wait,            "Wait"},
            {Token::Type::GetPos,          "GetPos"},
            {Token::Type::GetPen,          "GetPen"},
            {Token::Type::SetPlot,         "SetPlot"},
            {Token::Type::Plot,            "Plot"},
            {Token::Type::Number,          "Number"},
            {Token::Type::String,          "String"},
            {Token::Type::Unknown,         "Unknown"},
            {Token::Type::EndOfFile,       "EndOfFile"},
    };

    auto it = tokenMap.find(type);
    if (it == tokenMap.end()) throw std::runtime_error("Invalid token type.");

    return it->second;
}

Lexer::Lexer(const std::string &path)
{
    if (!boost::filesystem::exists(path)) Log(Log::Type::FATAL, "File does not exist.");
    if (boost::filesystem::file_size(path) == 0) Log(Log::Type::FATAL, "File is empty.");

    file.open(path);
    lineNum = 0;
    linePos = 0;
}

Lexer::Lexer()
{
    lineNum = 0;
    linePos = 0;

    file.close();
}

Lexer::~Lexer()
{
    file.close();
}

Token Lexer::nextToken(bool isSingleLine)
{
    assert(Token::Type::EndOfFile == 36);

    if (linePos >= (int) line.length())
    {
        if (file.eof() || isSingleLine) return {Token::Type::EndOfFile, "EOF"};

        std::getline(file, line);
        lineNum++;
        linePos = 0;
    }

    while (linePos < (int) line.length() && isspace(line[linePos])) linePos++;

    if (linePos < (int) line.length() && line[linePos] == '%')
    {
        linePos++;
        if (linePos < (int) line.length() && line[linePos] == '=')
        {
            linePos++;
            while (linePos < (int) line.length() && !(line[linePos] == '=' && line[linePos + 1] == '%'))
                linePos++;

            linePos += 2;
        } else while (linePos < (int) line.length() && line[linePos] != '\n') linePos++;

        return nextToken(isSingleLine);
    }

    std::string value;
    while (linePos < (int) line.length() && !isspace(line[linePos]))
    {
        value += line[linePos];
        linePos++;
    }

    Token::Type type = getTokenType(value);
    if (type == Token::Type::Unknown)
    {
        if (value.empty()) return nextToken(isSingleLine);
        if (std::all_of(value.begin(), value.end(), ::isdigit)) return {Token::Type::Number, value};

        if (value[0] == '"')
        {
            value = value.substr(1, value.length() - 2);
            return {Token::Type::String, value};
        }

        Log(Log::Type::ERROR,
            "Unknown token \"" + value + "\" on line " + std::to_string(lineNum) + ".\n  " + line + "\n  " +
            std::string(linePos - value.length(), ' ') + "\033[1;31m" + std::string(value.length(), '^') + "\033[0m");
    }

    return {type, value};
}

std::vector<Token> Lexer::lexInput(const std::string &input)
{
    assert(Token::Type::EndOfFile == 36);

    lineNum = 0;
    linePos = 0;
    line = input;

    std::vector<Token> tokens;
    Token token = nextToken(true);

    while (token.type != Token::Type::EndOfFile)
    {
        tokens.push_back(token);
        token = nextToken(true);
    }

    return tokens;
}

int Lexer::getLineNumber() const
{
    return lineNum;
}

int Lexer::getLinePosition() const
{
    return linePos;
}

std::string Lexer::getLine() const
{
    return line;
}

Token::Type Lexer::getTokenType(const std::string &value)
{
    static const std::map<std::string, Token::Type> tokenMap = {
            {"MODE",       Token::Type::Mode},
            {"OPTS",       Token::Type::Opts},
            {"END_OPTS",   Token::Type::EndOpts},
            {"UOPTS",      Token::Type::UOpts},
            {"END_UOPTS",  Token::Type::EndUOpts},
            {"P",          Token::Type::PlotMode},
            {"I",          Token::Type::InteractiveMode},
            {"ACCEL",      Token::Type::Acceleration},
            {"PENU_POS",   Token::Type::PenUpPosition},
            {"PEND_POS",   Token::Type::PenDownPosition},
            {"PENU_DELAY", Token::Type::PenUpDelay},
            {"PEND_DELAY", Token::Type::PenDownDelay},
            {"PENU_SPEED", Token::Type::PenUpSpeed},
            {"PEND_SPEED", Token::Type::PenDownSpeed},
            {"PENU_RATE",  Token::Type::PenUpRate},
            {"PEND_RATE",  Token::Type::PenDownRate},
            {"MODEL",      Token::Type::Model},
            {"PORT",       Token::Type::Port},
            {"UNITS",      Token::Type::Units},
            {"CONNECT",    Token::Type::Connect},
            {"DISCONNECT", Token::Type::Disconnect},
            {"PENUP",      Token::Type::PenUp},
            {"PENDOWN",    Token::Type::PenDown},
            {"PENTOGGLE",  Token::Type::PenToggle},
            {"HOME",       Token::Type::Home},
            {"GOTO",       Token::Type::GoTo},
            {"GOTO_REL",   Token::Type::GoToRelative},
            {"DRAW",       Token::Type::Draw},
            {"WAIT",       Token::Type::Wait},
            {"GETPOS",     Token::Type::GetPos},
            {"GETPEN",     Token::Type::GetPen},
            {"SETPLOT",    Token::Type::SetPlot},
            {"PLOT",       Token::Type::Plot},
    };

    auto it = tokenMap.find(value);
    if (it == tokenMap.end()) return Token::Type::Unknown;

    return it->second;
}
