#include "Token.h"

std::vector<std::string> Token::tokenMap = {
		"INVALID",
		"IDENTIFIER",
		"STRING",
		"INTEGER",
		"R_TRUE",
		"R_FALSE",
		"R_BOOLEAN",
		"R_INT",
		"R_VOID",
		"R_IF",
		"R_ELSE",
		"R_WHILE",
		"R_BREAK",
		"R_RETURN",
		"O_PLUS",
		"O_MINUS",
		"O_MULT",
		"O_DIV",
		"O_MOD",
		"O_LT",
		"O_GT",
		"O_LE",
		"O_GE",
		"O_EQUAL",
		"O_EQUIV",
		"O_NOTEQUIV",
		"O_NOT",
		"O_AND",
		"O_OR",
		"LRBRAC",
		"RRBRAC",
		"LCBRAC",
		"RCBRAC",
		"SEMICOLON",
		"COMMA"
};

 std::string & Token::GetTokenString(Token::Type type)
{
	return tokenMap[int(type)];
}
