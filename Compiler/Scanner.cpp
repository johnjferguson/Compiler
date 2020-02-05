#include "Scanner.h"
#include <fstream>
#include <iostream>
#include <cctype>

std::unordered_map<std::string, Token::Type> Scanner::reservedMap = 
{
	{"true",	Token::Type::R_TRUE},
	{"false",   Token::Type::R_FALSE},
	{"boolean", Token::Type::R_BOOLEAN },
	{"int",		Token::Type::R_INT},
	{"void" ,	Token::Type::R_VOID},
	{"if",		Token::Type::R_IF},
	{"else",	Token::Type::R_ELSE},
	{"while",	Token::Type::R_WHILE},
	{"break" ,	Token::Type::R_BREAK},
	{"return",	Token::Type::R_RETURN}
};


std::queue<Token> Scanner::Tokenize(const std::string & path) const
{
	std::ifstream file(path);
	if (!file.good())
	{
		std::cerr << "Scanner Error: Failed to open - " << path << std::endl;
		good = false;
		return std::queue<Token>();
	}

	std::queue<Token> tokens;
	unsigned int lineNum = 1;

	std::string line;

	// variables that need to be declared outside of switch
	std::string str;
	int target;

	while (std::getline(file, line))
	{
		bool eol = false;
		for (int i = 0; i < (int)line.size(); i++)
		{
			// checking if its the last character in the line
			eol = i + 1 < (int)line.size() ? false : true;

			switch (line[i])
			{
			case ' ':
			case '\t':
				// ignore white space
				break;
			case '/':
				if (!eol && line[i + 1] == '/')
				{
					i = line.size();
				}
				else
				{
					tokens.push({ Token::Type::O_DIV, lineNum, 0, "/" });
				}
				break;
			case '<':
				if (!eol && line[i + 1] == '=')
				{
					tokens.push({ Token::Type::O_LE, lineNum, 0, "<=" });
					i++;
				}
				else
				{
					tokens.push({ Token::Type::O_LT, lineNum, 0, "<" });
				}
				break;
			case '>':
				if (!eol && line[i + 1] == '=')
				{
					tokens.push({ Token::Type::O_GE, lineNum, 0, ">=" });
					i++;
				}
				else
				{
					tokens.push({ Token::Type::O_GT, lineNum, 0, ">" });
				}
				break;
			case '=':
				if (!eol && line[i + 1] == '=')
				{
					tokens.push({ Token::Type::O_EQUIV, lineNum, 0, "==" });
					i++;
				}
				else
				{
					tokens.push({ Token::Type::O_EQUAL, lineNum, 0, "=" });
				}
				break;
			case '!':
				if (!eol && line[i + 1] == '=')
				{
					tokens.push({ Token::Type::O_NOTEQUIV, lineNum, 0, "!=" });
					i++;
				}
				else
				{
					tokens.push({ Token::Type::O_NOT, lineNum, 0, "!" });
				}
				break;
			case '&':
				if (!eol && line[i + 1] == '&')
				{
					tokens.push({ Token::Type::O_AND, lineNum, 0, "&&" });
					i++;
				}
				else
				{
					std::cerr << "Scanner Error: Unexpected character after '&' on line - " << lineNum << std::endl;
					good = false;
					return std::queue<Token>();
				}
				break;
			case '|':
				if (!eol && line[i + 1] == '|')
				{
					tokens.push({ Token::Type::O_OR, lineNum, 0, "||" });
					i++;
				}
				else
				{
					std::cerr << "Scanner Error: Unexpected character after '|' on line - " << lineNum << std::endl;
					good = false;
					return std::queue<Token>();
				}
				break;
			case '+':
				tokens.push({ Token::Type::O_PLUS, lineNum, 0, "+" });
				break;
			case '-':
				tokens.push({ Token::Type::O_MINUS, lineNum, 0, "-" });
				break;
			case '*':
				tokens.push({ Token::Type::O_MULT, lineNum, 0, "*" });
				break;
			case '%':
				tokens.push({ Token::Type::O_MOD, lineNum, 0, "%" });
				break;
			case '(':
				tokens.push({ Token::Type::LRBRAC, lineNum, 0, "(" });
				break;
			case ')':
				tokens.push({ Token::Type::RRBRAC, lineNum, 0, ")" });
				break;
			case '{':
				tokens.push({ Token::Type::LCBRAC, lineNum, 0, "{" });
				break;
			case '}':
				tokens.push({ Token::Type::RCBRAC, lineNum, 0, "}" });
				break;
			case ';':
				tokens.push({ Token::Type::SEMICOLON, lineNum, 0, ";" });
				break;
			case ',':
				tokens.push({ Token::Type::COMMA, lineNum, 0, "," });
				break;
			case '"':
				if (eol)
				{
					std::cerr << "Scanner Error: No matching '\"' on line - " << lineNum << std::endl;
					good = false;
					return std::queue<Token>();
				}
				target = -1;
				str = std::string();
				i++;
				while (i < (int)line.size() && target < 0)
				{
					if (line[i] == '"')
					{
						target = i;
					}
					else
					{
						str.push_back(line[i++]);
					}
				}
				if (target > 0)
				{
					tokens.push({ Token::Type::STRING, lineNum, 0, str });
				}
				else
				{
					std::cerr << "Scanner Error: No matching '\"' on line - " << lineNum << std::endl;
					good = false;
					return std::queue<Token>();
				}
				break;
			default:
				if (std::isdigit(line[i]))
				{
					str = std::string();
					while (i < (int)line.size() && std::isdigit(line[i]))
					{
						str.push_back(line[i++]);
					}
					i--;
					tokens.push({ Token::Type::INTEGER, lineNum, std::stoi(str), str });
				}
				else if (AlphaNumPU(line[i]))
				{
					str = std::string();
					while (i < (int)line.size() && AlphaNumPU(line[i]))
					{
						str.push_back(line[i++]);
					}
					// checking if identifier is a reserved word
					if (reservedMap.find(str) == reservedMap.end())
					{
						tokens.push({ Token::Type::IDENTIFIER, lineNum, 0, str });
					}
					else
					{
						tokens.push({ reservedMap.find(str)->second, lineNum, 0, str });
					}
					// went one too far
					i--;
				}
				else
				{
					std::cerr << "Scanner Error: Unknown character - "<< line[i] << " - on line - " << lineNum << std::endl;
					good = false;
					return std::queue<Token>();
				}

			}
		}

		lineNum++;
	}

	return std::move(tokens);
}

bool Scanner::AlphaNumPU(unsigned char c) const
{
	return std::isalnum(c) || (c == '_');
}

bool Scanner::Good() const
{
	return good;
}
