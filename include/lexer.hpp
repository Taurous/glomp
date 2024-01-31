#pragma once

#include <vector>
#include <string>
#include <optional>
#include <iostream>

enum class TokenType {
    _INT, // integer
    _ADD, // addition
    _SUB, // subtract
    _RET, // return
    _IDN, // identifier
    _STR, // string
    _OUT, // output
    _DMP, // dump stack
    _INV  // invalid
};

struct Token {
    TokenType type;
    int line_number;
    std::optional<std::string> value;
};

std::vector<Token> tokenize(std::string str);
void printTokens(const std::vector<Token> &toks);
