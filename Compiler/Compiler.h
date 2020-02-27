#pragma once
#include <string>
#include "Scanner.h"
#include "Parser.h"

class Compiler
{
public:
	Compiler() = default;
	int Run(const std::string& path);
	void PrintTokens(std::vector<Token>& tokens) const;
private:
	Scanner scanner;
	Parser parser;
};
