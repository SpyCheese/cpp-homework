#include "ArgumentsParser.h"
#include "ZipUnzip.h"
#include <iostream>

int main(int argc, char * argv[])
{
	Arguments args = parseArguments(argc, argv);
	if (args.flagHelp || args.args.empty())
	{
		printHelp();
		exit(0);
	}
	if (args.args[0] != "z" && args.args[0] != "u")
	{
		std::cerr << "Incorrect command '" << args.args[0] << "'.\n";
		std::cerr << "Try --help for more information.\n";
		exit(1);
	}
	if (args.args.size() != 3)
	{
		std::cerr << "Incorrect number of arguments.\n";
		std::cerr << "Try --help for more information.\n";
		exit(1);
	}

	if (args.args[0] == "z")
		mainZip(args.args[1], args.args[2]);
	else
		mainUnzip(args.args[1], args.args[2]);
}
