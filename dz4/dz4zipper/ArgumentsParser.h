#ifndef __ARGUMENTS_PARSER_H__
#define __ARGUMENTS_PARSER_H__

#include <string>
#include <vector>

struct Arguments
{
	std::vector < std::string > args;
	bool flagHelp = false;
};

Arguments parseArguments(int argc, char * argv[]);
void printHelp();

#endif // __ARGUMENTS_PARSER_H__
