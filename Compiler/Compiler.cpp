#include "Compiler.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include "Token.h"

int Compiler::Run(const std::string & path)
{
	std::vector<Token> tokens = scanner.Tokenize(path);
	if (!scanner.Good())
	{
		std::cout << "Compiler didn't complete successfully\n";
		return 1;
	}
	//PrintTokens(tokens);

	parser.Parse(tokens); 
	if (!parser.Good())
	{
		std::cout << "Compiler didn't complete successfully\n";
		return 1;
	}
	parser.Print();

	std::cout << "Compiler completed successfully\n";
	std::cin.get();
	return 0;
}

void Compiler::PrintTokens(std::vector<Token>& tokens) const
{
	for (auto& v : tokens)
	{
		std::stringstream ss;
		ss << "{ Type: " << std::setw(10) << std::left << Token::GetTokenString(v.type) <<
			"  Line: " << std::setw(4) << std::left << v.line <<
			"  Value: " << std::setw(4) << std::left << v.value <<
			"  String: " << std::setw(40) << std::left << v.str << " }\n";
		std::cout << ss.str();
	}
}
