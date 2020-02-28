#include "Parser.h"
#include <iostream>

using TT = Token::Type;

Parser::~Parser()
{
	if (head != nullptr)
		delete head;
}

void Parser::Parse(std::vector<Token>& tokens)
{
	this->tokens = &tokens;
	tokens.push_back({ TT::EOT, 0u, 0, "end of tokens" });
	index = 0u;
	good = true;

	head = Start();
}

bool Parser::Check(Token::Type type) const
{
	return (index < tokens->size()) && (type == (*tokens)[index].type) && good;
}

void Parser::Resolve(Token::Type type)
{
	if ((index < tokens->size()) && Check(type))
	{
		index++;
	}
	else
	{
		good = false;
	}
}

Token::Type Parser::GetType() const
{
	return (*tokens)[index].type;
}

const std::string & Parser::GetStr() const
{
	return (*tokens)[index].str;
}

unsigned int Parser::GetLine() const
{
	return (*tokens)[index].line;;
}


Parser::Node* Parser::Start()
{
	Parser::Node* node = new Parser::Node("start", 0, "start");

	if (Check(TT::R_BOOLEAN) |
		Check(TT::INTEGER) |
		Check(TT::R_VOID) |
		Check(TT::IDENTIFIER))
	{
		node->Add(GlobalDeclarations());
	}
	else if (Check(TT::EOT))
	{
		std::cout << "end of input" << std::endl;
	}
	else
	{
		std::cout << "error in start" << std::endl;
	}
}

Parser::Node * Parser::Literal()
{
	switch (GetType())
	{
	case TT::INTEGER:
		Resolve(TT::INTEGER);
		return new Parser::Node("number", GetLine(), GetStr());
		break;
	case TT::STRING:
		Resolve(TT::STRING);
		return new Parser::Node("string", GetLine(), GetStr());
		break;
	case TT::R_TRUE:
		Resolve(TT::R_TRUE);
		return new Parser::Node("true", GetLine(), GetStr());
		break;
	case TT::R_FALSE:
		Resolve(TT::R_FALSE);
		return new Parser::Node("false", GetLine(), GetStr());
		break;
	default:
		std::cerr << "Literal Fail" << std::endl;
		good = false;
		return new Parser::Node();
		break;
	}
}

Parser::Node * Parser::Type()
{
	Parser::Node* node = nullptr;

	switch (GetType())
	{
	case TT::R_BOOLEAN:
		Resolve(TT::R_BOOLEAN);
		return  new Parser::Node("boolean", GetLine(), GetStr());
		break;
	case TT::R_INT:
		Resolve(TT::R_INT);
		return new Parser::Node("int", GetLine(), GetStr());
		break;
	default:
		std::cerr << "Type Fail" << std::endl;
		good = false;
		return new Parser::Node();
		break;
	}
}

Parser::Node * Parser::GlobalDeclarations()
{
	Parser::Node* node = new Parser::Node("globaldeclarations", 0, "globaldeclarations");

	node->Add(GlobalDeclaration());
	
	while (Check(TT::R_BOOLEAN) | Check(TT::INTEGER) | Check(TT::R_VOID) | Check(TT::IDENTIFIER))
	{
		node->Add(GlobalDeclaration());
	}
	
	return node;
}

Parser::Node * Parser::GlobalDeclaration()
{
	Parser::Node* node = new Parser::Node("globaldeclaration", 0, "globaldeclaration");

	if (Check(TT::R_INT) || Check(TT::R_BOOLEAN))
	{
		Parser::Node* node0 = new Parser::Node();
		node0->Add(Type());
		node0->Add(Identifier());

		if (Check(TT::LRBRAC))
		{
			Resolve(TT::LRBRAC);
			node0->Set("functiondeclaration", GetLine(), "functiondeclaration");
			
			if (Check(TT::R_BOOLEAN) || Check(TT::R_INT))
			{
				node0->Add(FormalParameterList());
				Resolve(TT::RRBRAC);
				node0->Add(Block());
			}
			else if (Check(TT::RRBRAC))
			{
				Resolve(TT::RRBRAC);
				node0->Add(Block());
			}
			else
			{
				std::cerr << "GlobalDeclaration Fail Else" << std::endl;
				good = false;
			}

		}
		else if (Check(TT::SEMICOLON))
		{
			Resolve(TT::SEMICOLON);
			node0->Set("variabledeclaration", GetLine(), "variabledeclaration");
		}
		else
		{
			std::cerr << "GlobalDeclaration Fail If" << std::endl;
			good = false;
		}

		node->Add(node0);
	}
	else if (Check(TT::R_VOID))
	{
		Parser::Node* node0 = new Parser::Node("functiondeclaration", GetLine(), "functiondeclaration");
		node0->Add(new Parser::Node("void", GetLine(), "void"));
		Resolve(TT::R_VOID);
		node0->Add(FunctionHeader());
		node0->Add(Block());

		node->Add(node0);
	}
	else if (Check(TT::IDENTIFIER))
	{
		node->Add(MainFunctionDeclaration());
	}
	else
	{
		std::cerr << "GlobalDeclaration Fail" << std::endl;
		good = false;
	}

	return node;
}

Parser::Node * Parser::VariableDeclaration()
{
	Parser::Node* node = new Parser::Node("variabledeclaration", GetLine(), "variabledeclaration");

	node->Add(Type());
	node->Add(Identifier());
	Resolve(TT::COMMA);
}

Parser::Node * Parser::Identifier()
{
	Parser::Node* node = new Parser::Node("identifier", GetLine(), GetStr());
	Resolve(TT::IDENTIFIER);
	return node;
}

Parser::Node * Parser::FunctionDeclaration()
{
	Parser::Node* node = new Parser::Node("functiondeclaration", GetLine(), "functiondeclaration");
	node->Add(FunctionHeader());
	node->Add(Block());
	return node;
}

Parser::Node * Parser::FunctionHeader()
{
	Parser::Node* node = new Parser::Node("functionheader", 0u, "functionheader");

	if (Check(TT::R_BOOLEAN) || Check(TT::R_INT))
	{
		node->Add(Type());
		node->Add(FunctionDeclarator());
	}
	else if (Check(TT::R_VOID))
	{
		node->Add(new Parser::Node("void", GetLine(), GetStr()));
		Resolve(TT::R_VOID);
		node->Add(FunctionDeclarator());
	}
	else
	{
		std::cerr << "FunctionHeader Fail" << std::endl;
		good = false;
	}
	return node;
}

Parser::Node * Parser::FunctionDeclarator()
{
	Parser::Node* node = new Parser::Node("functiondeclarator", 0u, "functiondeclarator");

	node->Add(Identifier());
	Resolve(TT::LRBRAC);

	if (Check(TT::R_BOOLEAN) || Check(TT::R_INT))
	{
		node->Add(FormalParameterList());
		Resolve(TT::RRBRAC);
	}
	else if (Check(TT::RRBRAC))
	{
		Resolve(TT::RRBRAC);
	}
	else
	{
		std::cerr << "FunctionDeclarator Fail" << std::endl;
		good = false;
	}
	return node;
}

Parser::Node * Parser::FormalParameterList()
{
	Parser::Node* node = new Parser::Node("formalparameterlist", 0u, "formalparameterlist");

	node->Add(FormalParameter());

	while (Check(TT::COMMA))
	{
		node->Add(FormalParameter());
	}
	return node;
}

Parser::Node * Parser::FormalParameter()
{
	Parser::Node* node = new Parser::Node("formalparameter", 0u, "formalparameter");

	node->Add(Type());
	node->Add(Identifier());

	return node;
}

Parser::Node * Parser::MainFunctionDeclaration()
{
	Parser::Node* node = new Parser::Node("mainfunctiondeclaration", 0u, "mainfunctiondeclaration");

	node->Add(MainFunctionDeclarator());
	node->Add(Block());

	return node;
}

Parser::Node * Parser::MainFunctionDeclarator()
{
	Parser::Node* node = new Parser::Node("mainfunctiondeclarator", 0u, "mainfunctiondeclarator");

	node->Add(Identifier());
	Resolve(TT::LRBRAC);
	Resolve(TT::RRBRAC);

	return node;
}

Parser::Node * Parser::Block()
{
	Parser::Node* node = new Parser::Node("block", GetLine(), "block");

	Resolve(TT::LCBRAC);

	if (Check(TT::R_BOOLEAN) || Check(TT::R_INT) || Check(TT::LCBRAC) 
							 || Check(TT::SEMICOLON) || Check(TT::IDENTIFIER) 
							 || Check(TT::R_BREAK) || Check(TT::R_RETURN)
							 || Check(TT::R_IF) || Check(TT::R_WHILE))
	{
		node->Add(BlockStatements());
		Resolve(TT::RCBRAC);
	}
	else if (Check(TT::RCBRAC))
	{
		Resolve(TT::RCBRAC);
	}
	else
	{
		std::cerr << "Block Fail" << std::endl;
		good = false;
	}

	return node;
}

Parser::Node * Parser::BlockStatements()
{
	Parser::Node* node = new Parser::Node("blockstatements", GetLine(), "blockstatements");

	node->Add(BlockStatement());
	while (Check(TT::R_BOOLEAN) || Check(TT::R_INT) || Check(TT::LCBRAC)
		|| Check(TT::SEMICOLON) || Check(TT::IDENTIFIER)
		|| Check(TT::R_BREAK) || Check(TT::R_RETURN)
		|| Check(TT::R_IF) || Check(TT::R_WHILE))
	{
		node->Add(BlockStatement());
	}

	return node;
}

Parser::Node * Parser::BlockStatement()
{
	Parser::Node* node = new Parser::Node("blockstatement", GetLine(), "blockstatement");

	if (Check(TT::R_BOOLEAN) || Check(TT::INTEGER))
	{
		node->Add(VariableDeclaration());
	}
	else if (Check(TT::LCBRAC) || Check(TT::SEMICOLON)
		|| Check(TT::IDENTIFIER)
		|| Check(TT::R_BREAK)
		|| Check(TT::R_RETURN)
		|| Check(TT::R_IF)
		|| Check(TT::R_WHILE))
	{
		node->Add(Statement());
	}
	else
	{
		std::cerr << "BlockStatement Fail" << std::endl;
		good = false;
	}

	return node;
}

Parser::Node * Parser::Statement()
{
	Parser::Node* node = new Parser::Node("statement", GetLine(), "statement");
	Parser::Node* returnNode = new Parser::Node();
	Parser::Node* ifNode = new Parser::Node();
	Parser::Node* whileNode = new Parser::Node();

	switch (GetType())
	{
	case TT::LCBRAC:
		node->Add(Block());
		break;
	case TT::IDENTIFIER:
		node->Add(StatementExpression());
		break;
	case TT::R_BREAK:
		node->Add(new Parser::Node("break", GetLine(), GetStr()));
		Resolve(TT::R_BREAK);
		Resolve(TT::SEMICOLON);
		break;
	case TT::R_RETURN:
		returnNode->Set("return", GetLine(), GetStr());
		Resolve(TT::R_RETURN);

		if (Check(TT::O_MINUS) || Check(TT::INTEGER) || Check(TT::STRING)
			|| Check(TT::R_TRUE) || Check(TT::R_FALSE)
			|| Check(TT::LRBRAC) || Check(TT::IDENTIFIER))
		{
			returnNode->Add(Expression());
			Resolve(TT::SEMICOLON);
		}
		else if (Check(TT::SEMICOLON))
		{
			Resolve(TT::SEMICOLON);
		}
		else
		{
			std::cerr << "Statement Else Fail" << std::endl;
			good = false;
		}
		node->Add(returnNode);
		break;
	case TT::R_IF:
		ifNode->Set("if", GetLine(), GetStr());
		Resolve(TT::R_IF);
		Resolve(TT::LRBRAC);
		ifNode->Add(Expression());
		ifNode->Add(Statement());
		while (Check(TT::R_ELSE))
		{
			ifNode->Add(Statement());
		}
		node->Add(ifNode);
		break;
	case TT::R_WHILE:
		whileNode->Set("while", GetLine(), GetStr());
		Resolve(TT::R_WHILE);
		Resolve(TT::LRBRAC);
		whileNode->Add(Expression());
		whileNode->Add(Statement());
		node->Add(whileNode);
		break;
	default:
		std::cerr << "Statement Fail" << std::endl;
		good = false;
		break;
	}

	if (returnNode->IsDefault())
		delete returnNode;
	if (ifNode->IsDefault())
		delete ifNode;
	if (whileNode->IsDefault())
		delete whileNode;

	return node;
}

Parser::Node * Parser::StatementExpression()
{
	Parser::Node* node = new Parser::Node("statementexpression", 0u, "statementexpression");

	node->Add(Identifier());

	if (Check(TT::O_EQUAL))
	{
		Resolve(TT::O_EQUAL);
		node->Add(AssignmentExpression());
	}
	else if (Check(TT::LRBRAC))
	{
		Resolve(TT::LRBRAC);
		node->Add(ArgumentList());
		Resolve(TT::RRBRAC);
	}
	else
	{
		std::cerr << "StatementExpression Fail" << std::endl;
		good = false;
	}
	return node;
}

Parser::Node * Parser::Primary()
{
	Parser::Node* node = new Parser::Node("primary", 0u, "primary");

	if (Check(TT::INTEGER) || Check(TT::STRING) || Check(TT::R_TRUE) || Check(TT::R_FALSE))
	{
		node->Add(Literal());
	}
	else if (Check(TT::LRBRAC))
	{
		Resolve(TT::LRBRAC);
		node->Add(Expression());
		Resolve(TT::RRBRAC);
	}
	else if (Check(TT::IDENTIFIER))
	{
		node->Add(FunctionInvocation());
	}
	else
	{
		std::cerr << "Primary Fail" << std::endl;
		good = false;
	}

	return node;
}

Parser::Node * Parser::ArgumentList()
{
	Parser::Node* node = new Parser::Node("argumentlist", 0u, "argumentlist");

	node->Add(Expression());

	while (Check(TT::COMMA))
	{
		node->Add(Expression());
	}

	return node;
}

Parser::Node * Parser::FunctionInvocation()
{
	Parser::Node* node = new Parser::Node("functioninvocation", 0u, "functioninvocation");

	node->Add(Identifier());
	Resolve(TT::LRBRAC);

	if (Check(TT::O_MINUS) || Check(TT::INTEGER) || Check(TT::STRING)
		|| Check(TT::R_TRUE) || Check(TT::R_FALSE)
		|| Check(TT::LRBRAC) || Check(TT::IDENTIFIER))
	{
		node->Add(ArgumentList());
		Resolve(TT::RRBRAC);
	}
	else if (Check(TT::RRBRAC))
	{
		Resolve(TT::RRBRAC);
	}
	else
	{
		std::cerr << "FunctionInvocation Fail" << std::endl;
		good = false;
	}
	return node;
}

Parser::Node * Parser::PostfixExpression()
{
	Parser::Node* node = new Parser::Node("postfixexpression", 0u, "postfixexpression");

	if (Check(TT::INTEGER) || Check(TT::STRING) || Check(TT::R_TRUE) || Check(TT::R_FALSE))
	{
		Parser::Node* node0 = new Parser::Node("primary", 0u, "primary");
		node0->Add(Literal());
		node->Add(node0);
	}
	else if (Check(TT::LRBRAC))
	{
		Parser::Node* node1 = new Parser::Node("primary", 0u, "primary");
		Resolve(TT::LRBRAC);
		node1->Add(Expression());
		Resolve(TT::RRBRAC);
		node->Add(node1);
	}
	else if (Check(TT::IDENTIFIER))
	{
		Node* temp = Identifier();

		if (Check(TT::LRBRAC))
		{
			Parser::Node* node2 = new Parser::Node("functioninvocation", 0, "functioninvocation");
			Parser::Node* node3 = new Parser::Node("primary", 0, "primary");
			node2->Add(temp);
			Resolve(TT::LRBRAC);

			if (Check(TT::O_MINUS) || Check(TT::INTEGER) || Check(TT::STRING)
				|| Check(TT::R_TRUE) || Check(TT::R_FALSE)
				|| Check(TT::LRBRAC) || Check(TT::IDENTIFIER))
			{
				node2->Add(ArgumentList());
				Resolve(TT::RRBRAC);
			}
			else if (Check(TT::RRBRAC))
			{
				Resolve(TT::RRBRAC);
			}
			else
			{
				std::cerr << "PostFixExpression Fail Inner" << std::endl;
				good = false;
			}
			node3->Add(node2);
			node->Add(node3);
		}
		else
		{
			node->Add(Identifier());
		}
	}
	else
	{
		std::cerr << "PostFixExpression Fail" << std::endl;
		good = false;
	}
	return node;
}

Parser::Node * Parser::UnaryExpression()
{
	Parser::Node* node = new Parser::Node("unaryexpression", 0u, "unaryexpression");

	if (Check(TT::INTEGER) || Check(TT::STRING) || Check(TT::R_TRUE) || Check(TT::R_FALSE)
		|| Check(TT::LRBRAC) || Check(TT::IDENTIFIER))
	{
		node->Add(PostfixExpression());
	}
	else if (Check(TT::O_MINUS))
	{
		node->Add(new Parser::Node("minus", GetLine(), GetStr()));
		Resolve(TT::O_MINUS);
		node->Add(UnaryExpression());
	}
	else if (Check(TT::O_NOT))
	{
		node->Add(new Parser::Node("not", GetLine(), GetStr()));
		Resolve(TT::O_NOT);
		node->Add(UnaryExpression());
	}
	else
	{
		std::cerr << "UnaryExpression Fail" << std::endl;
		good = false;
	}
	return node;
}

Parser::Node * Parser::MultiplicativeExpression()
{
	Parser::Node* node = new Parser::Node("multiplicativeexpression", 0u, "multiplicativeexpression");
	node->Add(UnaryExpression());

	while (Check(TT::O_MULT) || Check(TT::O_DIV) || Check(TT::O_MOD))
	{
		if (Check(TT::O_MULT))
		{
			node->Add(new Parser::Node("*", GetLine(), GetStr()));
			Resolve(TT::O_MULT);
			node->Add(UnaryExpression());
		}
		else if (Check(TT::O_DIV))
		{
			node->Add(new Parser::Node("/", GetLine(), GetStr()));
			Resolve(TT::O_DIV);
			node->Add(UnaryExpression());
		}
		else if (Check(TT::O_MOD))
		{
			node->Add(new Parser::Node("%", GetLine(), GetStr()));
			Resolve(TT::O_MOD);
			node->Add(UnaryExpression());
		}
	}
	return node;
}

Parser::Node * Parser::AdditiveExpression()
{
	Parser::Node* node = new Parser::Node("additiveexpression", 0u, "additiveexpression");
	node->Add(MultiplicativeExpression());

	while (Check(TT::O_PLUS) || Check(TT::O_MINUS))
	{
		if (Check(TT::O_PLUS))
		{
			node->Add(new Parser::Node("+", GetLine(), GetStr()));
			Resolve(TT::O_PLUS);
			node->Add(MultiplicativeExpression());
		}
		else if (Check(TT::O_MINUS))
		{
			node->Add(new Parser::Node("-", GetLine(), GetStr()));
			Resolve(TT::O_MINUS);
			node->Add(MultiplicativeExpression());
		}
	}
	return node;
}

Parser::Node * Parser::RelationalExpression()
{
	Parser::Node* node = new Parser::Node("relationalexpression", 0u, "relationalexpression");
	node->Add(AdditiveExpression());

	while (Check(TT::O_LT) || Check(TT::O_GT) || Check(TT::O_LE) || Check(TT::O_GE))
	{
		if (Check(TT::O_LT))
		{
			node->Add(new Parser::Node("<", GetLine(), GetStr()));
			Resolve(TT::O_LT);
			node->Add(AdditiveExpression());
		}
		else if (Check(TT::O_GT))
		{
			node->Add(new Parser::Node(">", GetLine(), GetStr()));
			Resolve(TT::O_GT);
			node->Add(AdditiveExpression());
		}
		else if (Check(TT::O_LE))
		{
			node->Add(new Parser::Node("<=", GetLine(), GetStr()));
			Resolve(TT::O_LE);
			node->Add(AdditiveExpression());
		}
		else if (Check(TT::O_GE))
		{
			node->Add(new Parser::Node(">=", GetLine(), GetStr()));
			Resolve(TT::O_GE);
			node->Add(AdditiveExpression());
		}
	}
	return node;
}

Parser::Node * Parser::EqualityExpression()
{
	Parser::Node* node = new Parser::Node("equalityexpression", 0u, "equalityexpression");
	node->Add(RelationalExpression());

	while (Check(TT::O_EQUIV) || Check(TT::O_NOTEQUIV))
	{
		if (Check(TT::O_EQUIV))
		{
			node->Add(new Parser::Node("==", GetLine(), GetStr()));
			Resolve(TT::O_EQUIV);
			node->Add(RelationalExpression());
		}
		else if (Check(TT::O_NOTEQUIV))
		{
			node->Add(new Parser::Node("!=", GetLine(), GetStr()));
			Resolve(TT::O_NOTEQUIV);
			node->Add(RelationalExpression());
		}
	}
	return node;
}

Parser::Node * Parser::ConditionalAndExpression()
{
	Parser::Node* node = new Parser::Node("conditionalandexpression", 0u, "conditionalandexpression");
	node->Add(EqualityExpression());

	while (Check(TT::O_AND))
	{
		node->Add(new Parser::Node("and", GetLine(), GetStr()));
		Resolve(TT::O_AND);
		node->Add(EqualityExpression());
	}
	return node;
}

Parser::Node * Parser::ConditionalOrExpression()
{
	Parser::Node* node = new Parser::Node("conditionalorexpression", 0u, "conditionalorexpression");
	node->Add(ConditionalAndExpression());

	while (Check(TT::O_OR))
	{
		node->Add(new Parser::Node("or", GetLine(), GetStr()));
		Resolve(TT::O_OR);
		node->Add(ConditionalAndExpression());
	}
	return node;
}

Parser::Node * Parser::AssignmentExpression()
{
	return nullptr;
}

Parser::Node::Node(const std::string & name, unsigned int line, const std::string & attribute)
	:
	name(name),
	line(line),
	attribute(attribute)
{
}

void Parser::Node::Set(const std::string & name_in, unsigned int line_in, const std::string & attribute_in)
{
	name = name_in;
	line = line_in;
	attribute = attribute_in;
}

bool Parser::Node::IsDefault() const
{
	return name.size() == 0;
}

void Parser::Node::Add(Node * node)
{
	children.push_back(node);
}

Parser::Node::~Node()
{
	for (Node* n : children)
	{
		if (n != nullptr)
			delete n;
	}
}
