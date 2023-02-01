

#include "include/lexer.h"

Lexer::Lexer(const std::string &path)
{
	if (!fs::exists(path))
	{
		std::cerr << "[\033[1;31mERROR\033[0m]: File does not exist." << std::endl;
		exit(EXIT_FAILURE);
	}

	if (fs::file_size(path) == 0)
	{
		std::cerr << "[\033[1;31mERROR\033[0m]: File is empty." << std::endl;
		exit(EXIT_FAILURE);
	}

	file.open(path);
	lineNum = 0;
}

Lexer::~Lexer()
{
	file.close();
}

Token Lexer::nextToken()
{
	if (linePos >= (int) line.length())
	{
		if (file.eof())
			return Token(Token::Type::EndOfFile, "");

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
		} else
		{
			while (linePos < (int) line.length() && line[linePos] != '\r' && line[linePos] != '\n')
				linePos++;
		}

		return nextToken();
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
		if (value.empty()) return nextToken();
		if (std::all_of(value.begin(), value.end(), ::isdigit)) return Token(Token::Type::Number, value);

		if (value[0] == '"')
		{
			value = value.substr(1, value.length() - 2);
			return Token(Token::Type::String, value);
		}

		std::cerr << "[\033[1;31mERROR\033[0m]: Unknown token '" << value << "' on line " << lineNum << "."
				  << std::endl;
		exit(EXIT_FAILURE);
	}

	return Token(type, value);
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