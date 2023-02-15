#include "include/interpreter.h"

//Token Lexer::readInput(const String &input)
//{
//	assert(Token::Type::EndOfFile == 36);
//
//	if (linePos >= (int) line.length())
//	{
//		if (input == "EOL") return Token(Token::Type::EndOfFile, "EOF");
//
//		line = input;
//		lineNum++;
//		linePos = 0;
//	}
//
//	while (linePos < (int) line.length() && isspace(line[linePos])) linePos++;
//
//	if (linePos < (int) line.length() && line[linePos] == '%')
//	{
//		linePos++;
//		if (linePos < (int) line.length() && line[linePos] == '=')
//		{
//			linePos++;
//			while (linePos < (int) line.length() && !(line[linePos] == '=' && line[linePos + 1] == '%'))
//				linePos++;
//
//			linePos += 2;
//		} else
//		{
//			while (linePos < (int) line.length() && line[linePos] != '\r' && line[linePos] != '\n')
//				linePos++;
//		}
//
//		return readInput(input);
//	}
//
//	std::string value;
//	while (linePos < (int) line.length() && !isspace(line[linePos]))
//	{
//		value += line[linePos];
//		linePos++;
//	}
//
//	Token::Type type = getTokenType(value);
//	if (type == Token::Type::Unknown)
//	{
//		if (value.empty()) return readInput(input);
//		if (std::all_of(value.begin(), value.end(), ::isdigit)) return Token(Token::Type::Number, value);
//
//		if (value[0] == '"')
//		{
//			value = value.substr(1, value.length() - 2);
//			return Token(Token::Type::String, value);
//		}
//
//		Log(Log::Type::Error,
//			"Unknown token '" + value + "'.\n  " + line + "\n  " +
//			std::string(linePos - value.length(), ' ') + "\033[1;31m" + std::string(value.length(), '^') +
//			"\033[0m", {}, false);
//		return Token(Token::Type::Unknown, value);
//	}
//
//	return Token(type, value);
//}

void Interpreter::run()
{
	std::signal(SIGINT, [](int signal)
	{
		if (signal == SIGINT)
		{
			std::cout << std::endl;
			Log(Log::Type::Info, "Exiting interpreter.");
			exit(EXIT_SUCCESS);
		}
	});

	Log(Log::Type::Info, "Type 'help' for a list of commands.");
	while (true)
	{
		std::cout << prompt;
		std::getline(std::cin, input);
		input.trim().sanitize();

		if (std::cin.eof())
		{
			std::cin.clear();
			std::cout << std::endl;

			break;
		}

		// TODO: use up and down arrows to go through history.
		// TODO: use left and right arrows to go through input.

		if (input.getCase(String::Case::Lower) == "help") printHelp();
		else if (input.getCase(String::Case::Lower) == "history") printHistory();
		else if (input.getCase(String::Case::Lower) == "clear") clearHistory();
		else if (input.getCase(String::Case::Lower) == "exit") break;
		else
		{
			if (input.empty()) continue;
			if (input.getCase(String::Case::Lower) == "eol")
			{
				if (history.empty()) continue;
				input = history.back();
			}

			history.push_back(input);
			Token token = lexer.readInput(input);

			if (token.type == Token::Type::EndOfFile || token.type == Token::Type::Unknown) continue;

			FileState fileState;
			fileState.tokens.push_back(token);
			fileState.lines.push_back(input);

			Parser parser(fileState);
			parser.parse();
		}
	}

	Log(Log::Type::Info, "Exiting interpreter.");
}

void Interpreter::printHelp()
{
	Log(Log::Type::Info, "Available commands:");
	Log(Log::Type::Info, "  exit: Exit the interpreter.");
	Log(Log::Type::Info, "  history: Show the command history.");
	Log(Log::Type::Info, "  clear: Clear the command history.");
	Log(Log::Type::Info, "  eol: End the current command.");
	Log(Log::Type::Info, "  help: Show this help message.");
}

void Interpreter::printHistory()
{
	if (history.empty())
	{
		Log(Log::Type::Info, "Command history is empty.");
		return;
	}

	Log(Log::Type::Info, "Command history:");
	for (int i = 0; i < (int) history.size(); i++)
		Log(Log::Type::Info, "  " + std::to_string(i + 1) + ": " + history[i]);
}

void Interpreter::clearHistory()
{
	history.clear();
	Log(Log::Type::Info, "Cleared command history.");
}
