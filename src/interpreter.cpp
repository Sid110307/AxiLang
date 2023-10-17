#include "include/interpreter.h"

Interpreter::Interpreter() : parser(fileState, false) {}

void Interpreter::run()
{
    std::signal(SIGINT, [](int)
    {
        std::cout << std::endl;
        std::cout << PROMPT;
        std::cin.clear();
    });

    Log(Log::Type::INFO, "Type \"help\" for a list of commands.");
    std::cout << PROMPT;

    while (std::getline(std::cin, input))
    {
        std::cout << PROMPT;

        if (std::cin.eof())
        {
            input.clear();
            std::cin.clear();
            std::cout << std::endl;

            break;
        }

        sanitize(input);
        if (!input.empty()) history.push_back(input);

        if (input.rfind("source", 0) == 0)
        {
            if (trim(input).length() == 6)
            {
                Log(Log::Type::ERROR, "No file specified.", {}, false);
                continue;
            }
            std::string path = input.substr(7);

            if (!(boost::filesystem::exists(path) || boost::filesystem::is_regular_file(path)))
            {
                Log(Log::Type::ERROR, "File \"" + path + "\" does not exist/is not a file.", {}, false);
                continue;
            }

            std::ifstream file(path);
            if (!file.is_open())
            {
                Log(Log::Type::ERROR, "Failed to open file \"" + path + "\".", {}, false);
                continue;
            }

            std::string line;
            while (std::getline(file, line))
            {
                sanitize(line);
                if (line.empty()) continue;
                Log(Log::Type::DEBUG, "Executing \"" + line + "\".");
                execute(line);
            }

            file.close();
        } else if (input == "history") printHistory();
        else if (input == "clear") clearHistory();
        else if (input == "help") printHelp();
        else if (input == "exit") break;
        else
        {
            if (input.empty()) continue;
            execute(input);
        }

        input.clear();
        std::cin.clear();
    }

    Log(Log::Type::INFO, "Exiting interpreter.");
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

void Interpreter::printHelp()
{
    Log(Log::Type::INFO, std::string("AxiLang (unofficial) - Version ") + PROJECT_VERSION);
    Log(Log::Type::INFO, "Available commands:");
    Log(Log::Type::INFO, "  source <path>: Load a source file.");
    Log(Log::Type::INFO, "  history: Show the command history.");
    Log(Log::Type::INFO, "  clear: Clear the command history.");
    Log(Log::Type::INFO, "  help: Show this help message.");
    Log(Log::Type::INFO, "  exit: Exit the interpreter.");
}

void Interpreter::execute(const std::string &str)
{
    for (const auto &token: lexer.lexInput(str))
    {
        Log(Log::Type::DEBUG, "Token: " + token.value + " (" + token.typeToCStr() + ")");
        while (token.type != Token::Type::EndOfFile)
        {
            fileState.tokens.push_back(token);
            fileState.lines.push_back(str);
            fileState.lineNums.push_back(lexer.getLineNumber());
            fileState.linePositions.push_back(lexer.getLinePosition());
        }
    }

    Log(Log::Type::DEBUG, "State: [Tokens: " + std::to_string(fileState.tokens.size()) +
                          ", Lines: " + std::to_string(fileState.lines.size()) +
                          ", Line Numbers: " + std::to_string(fileState.lineNums.size()) +
                          ", Line Positions: " + std::to_string(fileState.linePositions.size()) + "]");

    for (const auto &token: fileState.tokens) Log(Log::Type::DEBUG, "  " + token.value + ": " + token.typeToCStr());
    parser.parse();
}
