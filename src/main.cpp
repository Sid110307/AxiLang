#include <iostream>
#include <fstream>

#include <boost/program_options.hpp>

#define VERSION "0.1.0"

namespace po = boost::program_options;

int main(int argc, char *argv[])
{
	std::string fileName;

	po::options_description desc("Allowed options");
	desc.add_options()
			("help,h", "Print this help message and exit")
			("version,v", "Print version number")
			("file,f", po::value<std::string>(&fileName), "Input file name");

	po::positional_options_description p;
	p.add("file", -1);

	po::variables_map vm;
	po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
	po::notify(vm);

	if (vm.count("help"))
	{
		std::cout << desc << std::endl;
		return EXIT_SUCCESS;
	}

	if (vm.count("version"))
	{
		std::cout << "Version " << VERSION << std::endl;
		return EXIT_SUCCESS;
	}

	if (!vm.count("file"))
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

	std::string contents;
	while (std::getline(inFile, contents)) std::cout << contents << std::endl;

	inFile.close();
	return EXIT_SUCCESS;
}
