#pragma once
#include <string>
#include "Scanner.h"

class Compiler
{
public:
	int Run(const std::string& path);
	void PrintTokens(std::queue<Token>& tokens) const;
private:
	Scanner scanner;
};
