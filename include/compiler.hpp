#pragma once

#include <vector>
#include <string>
#include "tokens.hpp"

void compile(const std::vector<Token> &tokens, std::string out_path, bool asmonly);
