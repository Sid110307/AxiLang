#include "include/interpreter.h"

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

		if (input.getCase(String::Case::Lower) == "help") printHelp();
		else if (input.getCase(String::Case::Lower) == "history") printHistory();
		else if (input.getCase(String::Case::Lower) == "clear") clearHistory();
		else if (input.getCase(String::Case::Lower) == "exit") break;
		else
		{
			if (input.empty()) continue;

			history.push_back(input);
			Token token = lexer.readInput(input);

			if (token.type == Token::Type::EndOfFile || token.type == Token::Type::Unknown) continue;

			FileState fileState;
			fileState.tokens.push_back(token);
			fileState.lines.push_back(input);

			Parser parser(fileState, false);
			parser.parse();
		}

		input.clear();
	}

	Log(Log::Type::Info, "Exiting interpreter.");
}

void Interpreter::printHelp()
{
	Log(Log::Type::Info, std::string("AxiLang (unofficial) - Version ") + PROJECT_VERSION);
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
