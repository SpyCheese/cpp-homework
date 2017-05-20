#include "ArgumentsParser.h"
#include <iostream>

static void processFlag(std::string const & flag, Arguments & r)
{
	if (flag == "--help" || flag == "-h")
		r.flagHelp = true;
	else
        throw IncorrectArgumentExcepiton(flag);
}

Arguments parseArguments(int argc, char * argv[])
{
	Arguments r;
	bool endFlags = false;
	for (int i = 1; i < argc; ++i)
	{
		std::string s = argv[i];
		if (endFlags || s.empty() || s[0] != '-')
			r.args.push_back(s);
		else if (s == "--")
			endFlags = true;
		else
		{
			if (s.length() == 1)
                throw IncorrectArgumentExcepiton("-");
			if (s[1] == '-')
				processFlag(s, r);
			else
			{
				for (size_t j = 1; j < s.length(); ++j)
					processFlag(std::string({'-', s[j]}), r);
			}
		}
	}
	return r;
}

void printHelp()
{
	std::cerr << "Usage:\n";
	std::cerr << "    dz4zipper z SOURCE DEST   compress file SOURCE into DEST\n";
	std::cerr << "    dz4zipper u SOURCE DEST   decompress file SOURCE into DEST\n";
	std::cerr << "Options:\n";
	std::cerr << "    -h, --help      print this message and exit\n";
}
