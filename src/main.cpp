#include <boost/program_options.hpp>
#include <iostream>

#include "include/lexer.h"
#include "include/parser.h"

namespace po = boost::program_options;

int main(int argc, char* argv[])
{
	std::string fileName;

	po::options_description description("Allowed options");
	description.add_options()
			("help,h", "Print this help message and exit")
			("version,v", "Print version number")
			("file,f", po::value<std::string>(&fileName), "Input file name");

	po::positional_options_description p;
	p.add("file", -1);

	po::parsed_options options = po::command_line_parser(argc, argv).options(description).allow_unregistered()
			.positional(p).run();

	po::variables_map vm;
	po::store(options, vm);
	po::notify(vm);

	if (vm.count("help"))
	{
		std::cout << "AxiLang (unofficial) - A scripting language for controlling the AxiDraw plotter.\n"
				  << "Version " << PROJECT_VERSION << "\n" << description << std::endl;
		return EXIT_SUCCESS;
	}

	if (vm.count("version"))
	{
		std::cout << "Version " << PROJECT_VERSION << std::endl;
		return EXIT_SUCCESS;
	}

	for (auto &argument: options.options)
		if (argument.unregistered)
		{
			std::cerr << "[\033[1;31mERROR\033[0m]: Unknown argument '" << argument.string_key << "'.\n"
					  << "Try '" << argv[0] << " --help' for more information." << std::endl;
			return EXIT_FAILURE;
		}

	if (!vm.count("file") || fileName.empty())
	{
		std::cerr << "[\033[1;31mERROR\033[0m]: No input file specified.\n"
				  << "Try '" << argv[0] << " --help' for more information." << std::endl;
		return EXIT_FAILURE;
	}

	std::ifstream inFile;
	inFile.open(fileName);

	if (!inFile)
	{
		std::cerr << "[\033[1;31mERROR\033[0m]: Could not open file '" << fileName << "'." << std::endl;
		return EXIT_FAILURE;
	}

	std::cout << "[\033[1;30mDEBUG\033[0m]: Parsing file '" << fileName << "'." << std::endl;

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

	Parser parser(fileState);
	parser.parse();

	inFile.close();
	return EXIT_SUCCESS;
}
