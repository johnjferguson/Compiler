#include "Compiler.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include "Token.h"

int Compiler::Run(const std::string & path)
{
	std::queue<Token> tokens = scanner.Tokenize(path);
	if (!scanner.Good())
	{
		std::cin.get();
		return 1;
	}
	// this acually consumes the tokens... just how queue works
	// could swap out with vector if needed
	PrintTokens(tokens);
	std::cin.get();
	return 0;
}

void Compiler::PrintTokens(std::queue<Token>& tokens) const
{
	while (tokens.size() > 0)
	{
		std::stringstream ss;
		const Token& t = tokens.front();
		ss << "{ Type: " << std::setw(10) << std::left << Token::GetTokenString(t.type) <<
			 "  Line: " << std::setw(4) << std::left << t.line <<
			"  Value: " << std::setw(4) << std::left << t.value <<
			 "  String: " << std::setw(40) << std::left << t.str << " }\n";
		std::cout << ss.str();
		tokens.pop();
	}
}
