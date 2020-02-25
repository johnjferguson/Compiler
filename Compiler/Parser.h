#pragma once
#include "Token.h"

class Parser
{
public:
	Parser() = default;
	void Parse(const std::vector<Token>& tokens);
private:
	int Start(const std::vector<Token>& tokens, unsigned int index, unsigned int depth);
	int GlobalDeclarations(const std::vector<Token>& tokens, unsigned int index, unsigned int depth);
	int GlobalDeclaration(const std::vector<Token>& tokens, unsigned int index, unsigned int depth);
};
