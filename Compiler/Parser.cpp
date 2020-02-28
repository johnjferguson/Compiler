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
	error_reported = false;

	head = Start();
}

void Parser::Print() const
{
	if (good)
	{
		head->Print(0);
	}
}

bool Parser::Good() const
{
	return good;
}

bool Parser::Check(Token::Type type) const
{
	return (index < tokens->size()) && (type == (*tokens)[index].type) && good;
}

bool Parser::CheckForward(Token::Type type) const
{
	if (index + 1 < tokens->size())
	{
		return (*tokens)[index + 1].type == type && good;
	}
	return false;
}

bool Parser::Check2Forward(Token::Type type) const
{
	if (index + 2 < tokens->size())
	{
		return (*tokens)[index + 2].type == type && good;
	}
	return false;
}

void Parser::Resolve(Token::Type type)
{
	if ((index < tokens->size()) && Check(type))
	{
		index++;
	}
	else
	{
		if (!error_reported)
		{
			std::cerr << "Parser Error: unexpected token around -> " << (*tokens)[index].str << " <- on line: " << (*tokens)[index].line << std::endl;
		}
		good = false;
		error_reported = true;
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
		Check(TT::R_INT) |
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
		good = false;
	}
	return node;
}

Parser::Node * Parser::Literal()
{
	std::string str = GetStr();
	unsigned int line = GetLine();

	switch (GetType())
	{
	case TT::INTEGER:
		Resolve(TT::INTEGER);
		return new Parser::Node("number", line, str);
		break;
	case TT::STRING:
		Resolve(TT::STRING);
		return new Parser::Node("string", line, str);
		break;
	case TT::R_TRUE:
		Resolve(TT::R_TRUE);
		return new Parser::Node("true", line, str);
		break;
	case TT::R_FALSE:
		Resolve(TT::R_FALSE);
		return new Parser::Node("false", line, str);
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
	std::string str = GetStr();
	unsigned int line = GetLine();

	switch (GetType())
	{
	case TT::R_BOOLEAN:
		Resolve(TT::R_BOOLEAN);
		return  new Parser::Node("boolean", line, str);
		break;
	case TT::R_INT:
		Resolve(TT::R_INT);
		return new Parser::Node("int", line, str);
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
	
	while (Check(TT::R_BOOLEAN) || Check(TT::R_INT) || Check(TT::R_VOID) || Check(TT::IDENTIFIER))
	{
		node->Add(GlobalDeclaration());
	}
	return node;
}

Parser::Node * Parser::GlobalDeclaration()
{
	Parser::Node* node = new Parser::Node("globaldeclaration", 0, "globaldeclaration");

	if ((Check(TT::R_BOOLEAN) || Check(TT::R_INT)) && CheckForward(TT::IDENTIFIER) && Check2Forward(TT::SEMICOLON))
	{
		node->Add(VariableDeclaration());
	}
	else if (Check(TT::R_BOOLEAN) || Check(TT::R_INT) || Check(TT::R_VOID))
	{
		node->Add(FunctionDeclaration());
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
	Resolve(TT::SEMICOLON);
	return node;
}

Parser::Node * Parser::Identifier()
{
	std::string s = GetStr();
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

	if (Check(TT::R_BOOLEAN) || Check(TT::R_INT))
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
	Parser::Node* node = new Parser::Node("statement", 0, "statement");
	
	if (Check(TT::LCBRAC))
	{
		return Block();
	}
	else if (Check(TT::SEMICOLON))
	{
		Resolve(TT::SEMICOLON);
	}
	else if (Check(TT::IDENTIFIER))
	{
		return StatementExpression();
	}
	else if (Check(TT::R_RETURN) && CheckForward(TT::SEMICOLON))
	{
		Node* returnNode = new Node("return", GetLine(), GetStr());
		Resolve(TT::R_RETURN);
		Resolve(TT::SEMICOLON);
		return returnNode;
	}
	else if (Check(TT::R_RETURN))
	{
		Node* returnNode = new Node("return", GetLine(), GetStr());
		Resolve(TT::R_RETURN);
		returnNode->Add(Expression());
		Resolve(TT::SEMICOLON);
		return returnNode;
	}
	else if (Check(TT::R_WHILE))
	{
		Node* whileNode = new Node("while", GetLine(), GetStr());
		Resolve(TT::R_WHILE);
		Resolve(TT::LRBRAC);
		whileNode->Add(Expression());
		Resolve(TT::RRBRAC);
		whileNode->Add(Statement());
		return whileNode;
	}
	else if (Check(TT::R_IF))
	{
		Node* ifNode = new Node("if", GetLine(), GetStr());
		Resolve(TT::R_IF);
		Resolve(TT::LRBRAC);
		ifNode->Add(Expression());
		Resolve(TT::RRBRAC);
		//ifNode->Add(Statement());
		ifNode->Add(BlockStatements()); // temp
		if (Check(TT::R_ELSE))
		{
			Node* elseNode = new Node("else", GetLine(), GetStr());
			Resolve(TT::R_ELSE);
			//elseNode->Add(Statement());
			elseNode->Add(BlockStatements()); // temp
			ifNode->Add(elseNode);
		}
		return ifNode;
	}
	else
	{
		std::cerr << "Statement Fail" << std::endl;
		good = false;
	}

	return node;
}

Parser::Node * Parser::StatementExpression()
{
	Parser::Node* node = new Parser::Node("statementexpression", 0u, "statementexpression");

	if (Check(TT::IDENTIFIER) && CheckForward(TT::O_EQUAL))
	{
		node->Add(Assignment());
	}
	else if (Check(TT::IDENTIFIER))
	{
		node->Add(FunctionInvocation());
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
	if (Check(TT::INTEGER) || Check(TT::STRING) || Check(TT::R_TRUE) || Check(TT::R_FALSE))
	{
		return Literal();
	}
	else if (Check(TT::LRBRAC))
	{
		Resolve(TT::LRBRAC);
		Parser::Node* node = Expression();
		Resolve(TT::RRBRAC);
		return node;
	}
	else if (Check(TT::IDENTIFIER))
	{
		return FunctionInvocation();
	}
	else
	{
		std::cerr << "Primary Fail" << std::endl;
		good = false;
	}

	return new Parser::Node();
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
	if (Check(TT::INTEGER) || Check(TT::STRING) || Check(TT::R_TRUE) || Check(TT::R_FALSE)
		|| Check(TT::LRBRAC) || (Check(TT::IDENTIFIER) && CheckForward(TT::LRBRAC)))
	{
		return Primary();
	}
	else if (Check(TT::IDENTIFIER))
	{
		return Identifier();
	}
	else
	{
		std::cerr << "PostFixExpression Fail" << std::endl;
		good = false;
	}
	return new Parser::Node();
}

Parser::Node * Parser::UnaryExpression()
{
	if (Check(TT::INTEGER) || Check(TT::STRING) || Check(TT::R_TRUE) || Check(TT::R_FALSE)
		|| Check(TT::LRBRAC) || Check(TT::IDENTIFIER))
	{
		return PostfixExpression();
	}
	else if (Check(TT::O_MINUS))
	{
		Parser::Node* minusNode = new Parser::Node("'-'", GetLine(), GetStr());
		Resolve(TT::O_MINUS);
		minusNode->Add(UnaryExpression());
		return minusNode;
	}
	else if (Check(TT::O_NOT))
	{
		Parser::Node* notNode = new Parser::Node("!", GetLine(), GetStr());
		Resolve(TT::O_NOT);
		notNode->Add(UnaryExpression());
		return notNode;
	}
	else
	{
		std::cerr << "UnaryExpression Fail" << std::endl;
		good = false;
	}
	return new Parser::Node();
}

Parser::Node * Parser::MultiplicativeExpression()
{
	Parser::Node* node = UnaryExpression();

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
	Parser::Node* node = MultiplicativeExpression();;

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
			node->Add(new Parser::Node("'-'", GetLine(), GetStr()));
			Resolve(TT::O_MINUS);
			node->Add(MultiplicativeExpression());
		}
	}
	return node;
}

Parser::Node * Parser::RelationalExpression()
{
	Parser::Node* node = AdditiveExpression();

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
	Parser::Node* node = RelationalExpression();

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
	Parser::Node* node = EqualityExpression();

	while (Check(TT::O_AND))
	{
		node->Add(new Parser::Node("&&", GetLine(), GetStr()));
		Resolve(TT::O_AND);
		node->Add(EqualityExpression());
	}
	return node;
}

Parser::Node * Parser::ConditionalOrExpression()
{
	Parser::Node* node = ConditionalAndExpression();

	while (Check(TT::O_OR))
	{
		node->Add(new Parser::Node("||", GetLine(), GetStr()));
		Resolve(TT::O_OR);
		node->Add(ConditionalAndExpression());
	}
	return node;
}

Parser::Node * Parser::AssignmentExpression()
{
	Parser::Node* node = new Parser::Node("assignmentexpression", 0u, "assignmentexpression");

	if (Check(TT::IDENTIFIER) && CheckForward(TT::O_EQUAL))
	{
		node->Add(Assignment());
	}
	else if (Check(TT::O_MINUS) || Check(TT::O_NOT) || Check(TT::INTEGER) 
			|| Check(TT::STRING) || Check(TT::R_TRUE) || Check(TT::R_FALSE)
		|| Check(TT::LRBRAC) || Check(TT::IDENTIFIER))
	{
		node->Add(ConditionalOrExpression());
	}
	else
	{
		std::cerr << "AssignmentExpression Fail" << std::endl;
	}
	return node;
}

Parser::Node * Parser::Assignment()
{
	Parser::Node* node = new Parser::Node("assignment", 0u, "assignment");

	node->Add(Identifier());
	node->Add(new Parser::Node("=", GetLine(), GetStr()));
	Resolve(TT::O_EQUAL);
	node->Add(AssignmentExpression());
	return node;
}

Parser::Node * Parser::Expression()
{
	Parser::Node* node = new Parser::Node("expression", 0u, "expression");
	node->Add(AssignmentExpression());
	return node;
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

void Parser::Node::Print(int depth) const
{
	std::stringstream ss;
	ss << std::string(depth, '-') << name << " { 'type': " << name;
	if (line != 0)
		ss << ", 'lineno':" << line;
	if (name != attribute)
		ss << ", 'attr': " << attribute;
		
	ss << " }";
	std::cout << ss.str() << std::endl;
	depth++;
	for (const Node* n : children)
	{
		n->Print(depth);
	}
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
