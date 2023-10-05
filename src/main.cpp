#include <string>

#include <boost/program_options.hpp>

#include "include/lexer.h"
#include "include/parser.h"
#include "include/interpreter.h"
#include "include/utils.h"

namespace po = boost::program_options;

int main(int argc, char **argv)
{
    std::string fileName;

    po::options_description description("Allowed options");
    description.add_options()
            ("help,h", "Print this help message and exit")
            ("version,v", "Print the version number and exit")
            ("debug,d", "Show extra information while running")
            ("file,f", po::value<std::string>(&fileName), "Input file path")
            ("interactive,i", "Start an interactive interpreter");

    po::positional_options_description p;
    p.add("file", -1);

    po::parsed_options options = po::command_line_parser(argc, argv).options(
                    description).allow_unregistered()
            .positional(p).run();

    po::variables_map vm;
    po::store(options, vm);
    po::notify(vm);

    if (vm.count("help"))
    {
        std::ostringstream descriptionStream;
        descriptionStream << description;

        Log(Log::Type::INFO,
            std::string("AxiLang (unofficial) - A scripting language for controlling the AxiDraw plotter.\nVersion ") +
            PROJECT_VERSION + "\n" + descriptionStream.str());
        return EXIT_SUCCESS;
    }

    if (vm.count("version"))
    {
        Log(Log::Type::INFO, std::string("Version ") + PROJECT_VERSION);
        return EXIT_SUCCESS;
    }

    for (const auto &argument: options.options)
        if (argument.unregistered)
        {
            Log(Log::Type::ERROR,
                "Unknown argument \"" + argument.string_key + "\".\nTry \"" + argv[0] +
                " --help\" for more information.");
            return EXIT_FAILURE;
        }

    if (vm.count("debug")) Log(Log::Type::INFO, "DEBUG mode enabled.").enableDebug();
    if (vm.count("interactive"))
    {
        Log(Log::Type::INFO, "Starting AxiLang interpreter.");
        Interpreter().run();

        return EXIT_SUCCESS;
    }

    if (!vm.count("file") || fileName.empty())
    {
        Log(Log::Type::ERROR,
            std::string("No input file specified.\nTry \"") + argv[0] + " --help\" for more information.");
        return EXIT_FAILURE;
    }

    std::ifstream inFile;
    inFile.open(fileName);

    if (!inFile)
    {
        Log(Log::Type::ERROR, "Could not open file \"" + fileName + "\".");
        return EXIT_FAILURE;
    }

    Log(Log::Type::DEBUG, std::string("Parsing file \"") + fileName + "\".");

    Lexer lexer(fileName);
    Token token = lexer.nextToken();

    FileState fileState;
    while (token.type != Token::Type::EndOfFile)
    {
        fileState.tokens.push_back(token);
        fileState.lines.push_back(lexer.getLine());
        fileState.lineNums.push_back(lexer.getLineNumber());
        fileState.linePositions.push_back(lexer.getLinePosition());

        token = lexer.nextToken();
    }

    Log(Log::Type::DEBUG, "Tokens: ");
    for (const auto &tok: fileState.tokens) Log(Log::Type::DEBUG, "  " + tok.typeToCStr() + ": " + tok.value);

    Parser parser(fileState);
    parser.parse();

    inFile.close();
    return EXIT_SUCCESS;
}
