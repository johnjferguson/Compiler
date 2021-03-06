#pragma once
#include "Token.h"
#include <sstream>

class Parser
{
public:
	class Node
	{
	public:
		Node() = default;
		Node(const std::string& name, unsigned int line, const std::string& attribute);
		void Set(const std::string& name_in, unsigned int line_in, const std::string& attribute_in);
		void Print(int depth) const;
		bool IsDefault() const;
		// make sure no one(myself) cannot copy the pointers
		Node(const Node&) = delete;
		Node& operator=(const Node&) = delete;
		// add node
		void Add(Node* node);
		// delete all the way down
		~Node();
	private:
		std::vector<Node*> children;
		std::string name;
		unsigned int line;
		std::string attribute;
	};
public:
	Parser() = default;
	~Parser();
	// no copies
	Parser(const Parser&) = delete;
	Parser& operator=(const Parser&) = delete;
	void Parse(std::vector<Token>& tokens);
	void Print() const;
	bool Good() const;
private:
	bool Check(Token::Type type) const;
	// functions were nesting too deep added some lookahead
	bool CheckForward(Token::Type type) const;
	bool Check2Forward(Token::Type type) const;
	void Resolve(Token::Type type);
	Token::Type GetType() const;
	const std::string& GetStr() const;
	unsigned int GetLine() const;
private:
	// suffering
	Node* Start();
	Node* Literal();
	Node* Type();
	Node* GlobalDeclarations();
	Node* GlobalDeclaration();
	Node* VariableDeclaration();
	Node* Identifier();
	Node* FunctionDeclaration();
	Node* FunctionHeader();
	Node* FunctionDeclarator();
	Node* FormalParameterList();
	Node* FormalParameter();
	Node* MainFunctionDeclaration();
	Node* MainFunctionDeclarator();
	Node* Block();
	Node* BlockStatements();
	Node* BlockStatement();
	Node* Statement();
	Node* StatementExpression();
	Node* Primary();
	Node* ArgumentList();
	Node* FunctionInvocation();
	Node* PostfixExpression();
	Node* UnaryExpression();
	Node* MultiplicativeExpression();
	Node* AdditiveExpression();
	Node* RelationalExpression();
	Node* EqualityExpression();
	Node* ConditionalAndExpression();
	Node* ConditionalOrExpression();
	Node* AssignmentExpression();
	Node* Assignment();
	Node* Expression();
	
private:
	std::vector<Token>* tokens = nullptr;
	unsigned int index;
	bool good;
	bool error_reported;
private:
	Node* head = nullptr;
};
