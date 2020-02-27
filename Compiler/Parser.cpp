#include "Parser.h"
#include <iostream>

using TT = Token::Type;

void Parser::Parse(std::vector<Token>& tokens)
{
	this->tokens = &tokens;
	tokens.push_back({ TT::EOT, 0u, 0, "end of tokens" });
	index = 0u;
	depth = 0u;

	Start();
}

bool Parser::Check(Token::Type type) const
{
	return (index < tokens->size()) && (type == (*tokens)[index].type);
}

bool Parser::Resolve(Token::Type type)
{
	if ((index < tokens->size()) && (type == (*tokens)[index].type))
	{
		index++;
		return true;
	}
	return false;
}

bool Parser::Start()
{
	if (Check(TT::R_BOOLEAN) |
		Check(TT::INTEGER) |
		Check(TT::R_VOID) |
		Check(TT::IDENTIFIER))
	{
		std::cout << "start is working" << std::endl;
		return true;
	}
	else if (Check(TT::EOT))
	{
		std::cout << "start is not working" << std::endl;
		return true;
	}
	else
	{
		std::cout << "error in start" << std::endl;
		return false;
	}
}

