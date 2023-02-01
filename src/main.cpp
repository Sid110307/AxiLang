#include <iostream>
#include <fstream>

#include <boost/program_options.hpp>

#include "include/lexer.h"

#define VERSION "0.2.0"

namespace po = boost::program_options;

int main(int argc, char *argv[])
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
				  << "Version " << VERSION << "\n"
				  << description << std::endl;
		return EXIT_SUCCESS;
	}

	if (vm.count("version"))
	{
		std::cout << "Version " << VERSION << std::endl;
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

	Lexer lexer(fileName);
	Token token = lexer.nextToken();

	while (token.type != Token::Type::EndOfFile)
	{
		std::cout << token.type << ": " << token.value << std::endl;
		token = lexer.nextToken();
	}

	inFile.close();
	return EXIT_SUCCESS;
}
