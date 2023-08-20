#include "include/interpreter.h"

void Interpreter::run()
{
    std::signal(SIGINT, [](int signal)
    {
        if (signal == SIGINT)
        {
            std::cout << std::endl;
            Log(Log::Type::INFO, "Exiting interpreter.");
            exit(EXIT_SUCCESS);
        }
    });

    FileState fileState;
    Parser parser(fileState, false);

    Log(Log::Type::INFO, "Type 'help' for a list of commands.");
    while (true)
    {
        std::cout << prompt;
        std::getline(std::cin, input);

        if (std::cin.eof())
        {
            std::cin.clear();
            std::cout << std::endl;

            break;
        }

        sanitize(input);
        if (!input.empty()) history.push_back(input);

        if (toLower(input) == "help") printHelp();
        else if (toLower(input) == "history") printHistory();
        else if (toLower(input) == "clear") clearHistory();
        else if (toLower(input) == "exit") break;
        else
        {
            if (input.empty()) continue;

            for (const auto &token: lexer.lexInput(input))
            {
                Log(Log::Type::DEBUG, "Token: " + token.value + " (" + token.typeToCStr() + ")");
                if (token.type == Token::Type::EndOfFile || token.type == Token::Type::Unknown) continue;

                fileState.tokens.push_back(token);
                fileState.lines.push_back(input);
            }

            Log(Log::Type::DEBUG, "State: [Tokens: " + std::to_string(fileState.tokens.size()) +
                                  ", Lines: " + std::to_string(fileState.lines.size()) +
                                  ", Line Numbers: " + std::to_string(fileState.lineNums.size()) +
                                  ", Line Positions: " + std::to_string(fileState.linePositions.size()) + "]");

            for (auto &token: fileState.tokens) Log(Log::Type::DEBUG, "  " + token.value + ": " + token.typeToCStr());
            parser.parse();
        }

        input.clear();
        std::cin.clear();
    }

    Log(Log::Type::INFO, "Exiting interpreter.");
}

void Interpreter::printHelp()
{
    Log(Log::Type::INFO, std::string("AxiLang (unofficial) - Version ") + PROJECT_VERSION);
    Log(Log::Type::INFO, "Available commands:");
    Log(Log::Type::INFO, "  exit: Exit the interpreter.");
    Log(Log::Type::INFO, "  history: Show the command history.");
    Log(Log::Type::INFO, "  clear: Clear the command history.");
    Log(Log::Type::INFO, "  help: Show this help message.");
}

void Interpreter::printHistory()
{
    if (history.empty())
    {
        Log(Log::Type::INFO, "Command history is empty.");
        return;
    }

    Log(Log::Type::INFO, "Command history:");
    for (int i = 0; i < (int) history.size(); ++i)
        Log(Log::Type::INFO, "  " + std::to_string(i + 1) + ": " + history[i]);
}

void Interpreter::clearHistory()
{
    history.clear();
    Log(Log::Type::INFO, "Cleared command history.");
}
