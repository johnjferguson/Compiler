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

Node * Parser::Statement()
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
