#include <string>

#include <boost/program_options.hpp>

#include "include/lexer.h"
#include "include/parser.h"
#include "include/interpreter.h"
#include "include/utils.h"

int main(int argc, char* argv[])
{
	std::string fileName;

	boost::program_options::options_description description("Allowed options");
	description.add_options()
			("help,h", "Print this help message and exit")
			("version,v", "Print the version number and exit")
			("debug,d", "Show extra information while running")
			("file,f", boost::program_options::value<std::string>(&fileName), "Input file path")
			("interactive,i", "Start an interactive interpreter");

	boost::program_options::positional_options_description p;
	p.add("file", -1);

	boost::program_options::parsed_options options = boost::program_options::command_line_parser(argc, argv).options(
					description).allow_unregistered()
			.positional(p).run();

	boost::program_options::variables_map vm;
	boost::program_options::store(options, vm);
	boost::program_options::notify(vm);

	if (vm.count("help"))
	{
		std::ostringstream descriptionStream;
		descriptionStream << description;

		Log(Log::Type::Info,
			std::string("AxiLang (unofficial) - A scripting language for controlling the AxiDraw plotter.\nVersion ") +
			PROJECT_VERSION + "\n" + descriptionStream.str());
		return EXIT_SUCCESS;
	}

	if (vm.count("version"))
	{
		Log(Log::Type::Info, std::string("Version ") + PROJECT_VERSION);
		return EXIT_SUCCESS;
	}

	for (auto &argument: options.options)
		if (argument.unregistered)
		{
			Log(Log::Type::Error,
				"Unknown argument '" + argument.string_key + "'.\nTry '" + argv[0] + " --help' for more information.");
			return EXIT_FAILURE;
		}

	if (vm.count("debug"))
	{
		Log(true);
		Log(Log::Type::Info, "Debug mode enabled.");
	}

	if (vm.count("interactive"))
	{
		Log(Log::Type::Info, "Starting AxiLang interpreter.");
		Interpreter().run();

		return EXIT_SUCCESS;
	}

	if (!vm.count("file") || fileName.empty())
	{
		Log(Log::Type::Error,
			std::string("No input file specified.\nTry '") + argv[0] + " --help' for more information.");
		return EXIT_FAILURE;
	}

	std::ifstream inFile;
	inFile.open(fileName);

	if (!inFile)
	{
		Log(Log::Type::Error, "Could not open file \"" + fileName + "\".");
		return EXIT_FAILURE;
	}

	Log(Log::Type::Debug, std::string("Parsing file '") + fileName + "'.");

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

	Log(Log::Type::Debug, "Tokens: ");
	for (auto &tok: fileState.tokens) Log(Log::Type::Debug, "  " + tok.typeToCStr() + ": " + tok.value);

	Parser parser(fileState);
	parser.parse();

	inFile.close();
	return EXIT_SUCCESS;
}
