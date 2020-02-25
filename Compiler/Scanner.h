#pragma once
#include <queue>
#include <string>
#include <vector>
#include <unordered_map>
#include "Token.h"

class Scanner
{
public:
	Scanner() = default;
	std::vector<Token> Tokenize(const std::string& path) const;
	bool AlphaNumPU(unsigned char c) const;
	bool IsCharacter(unsigned char c) const;
	bool Good() const;
private:
	mutable bool good = true;
	static std::unordered_map<std::string, Token::Type> reservedMap;
};


