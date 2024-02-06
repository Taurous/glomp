#pragma once

#include <vector>
#include <string>
#include "tokens.hpp"

std::vector<Token> tokenize(std::string str);
void printTokens(const std::vector<Token> &toks);
