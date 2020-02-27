#pragma once
#include "Token.h"

class Parser
{
public:
	Parser() = default;
	void Parse(std::vector<Token>& tokens);
private:
	bool Check(Token::Type type) const;
	bool Resolve(Token::Type type);
private:
	bool Start();
	bool Literal();
	bool Type();
	bool GlobalDeclarations();
	bool GlobalDeclaration();
	bool VariableDeclaration();
	bool Identifier();
	bool FunctionDeclaration();
	bool FunctionHeader();
	bool FunctionDeclarator();
	bool FormalParameterList();
	bool FormalParameter();
	bool MainFunctionDeclaration();
	bool MainFunctionDeclarator();
	bool Block();
	bool BlockStatement();
	bool Statement();

private:
	std::vector<Token>* tokens = nullptr;
	unsigned int index;
	unsigned int depth;
};
