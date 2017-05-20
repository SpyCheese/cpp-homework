#include "ArgumentsParser.h"
#include "ZipUnzip.h"
#include <iostream>

int main(int argc, char * argv[])
{
    Arguments args;
    try
    {
        args = parseArguments(argc, argv);
    } catch (IncorrectArgumentExcepiton ex)
    {
		std::cerr << "Incorrect option '" << ex.name << "'.\n";
		std::cerr << "Try --help for more information.\n";
        return 1;
    }
	if (args.flagHelp || args.args.empty())
	{
		printHelp();
        return 0;
	}
	if (args.args[0] != "z" && args.args[0] != "u")
	{
		std::cerr << "Incorrect command '" << args.args[0] << "'.\n";
		std::cerr << "Try --help for more information.\n";
        return 1;
	}
	if (args.args.size() != 3)
	{
		std::cerr << "Incorrect number of arguments.\n";
		std::cerr << "Try --help for more information.\n";
        return 1;
	}

	if (args.args[0] == "z")
		return mainZip(args.args[1], args.args[2]);
	else
		return mainUnzip(args.args[1], args.args[2]);
}
