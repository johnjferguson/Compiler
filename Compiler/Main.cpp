#include <iostream>
#include "Compiler.h"
#include <string>

int main(int argc, char** argv)
{
	if (argc == 2)
	{
		return Compiler{}.Run(std::string(argv[1]));
	}
	else
	{
		std::cerr << "Main Error: Compiler needs a single file path as input." << std::endl;
		return 1;
	}
}