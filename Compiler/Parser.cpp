#include "Parser.h"
#include <iostream>

void Parser::Parse(const std::vector<Token>& tokens)
{
}

int Parser::Start(const std::vector<Token>& tokens, unsigned int index, unsigned int depth)
{
	std::cout << "program {'type' : 'program'}" << std::endl;
	if (tokens.size() == 0)
	{
		return 0;
	}
	GlobalDeclarations(tokens, index, depth++);
}

int Parser::GlobalDeclarations(const std::vector<Token>& tokens, unsigned int index, unsigned int depth)
{
	GlobalDeclaration(tokens, index, ++depth);
	depth--;

	if (index < tokens.size())
	{
		GlobalDeclarations(tokens, index, depth);
	}
	return 0;
}

int Parser::GlobalDeclaration(const std::vector<Token>& tokens, unsigned int index, unsigned int depth)
{
	/*
	switch (tokens[index].type)
	{
		case Token::Type::
	}
	*/
	return 0;
}
