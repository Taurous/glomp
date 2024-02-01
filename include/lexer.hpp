#pragma once

#include <vector>
#include <string>
#include <optional>

enum TokenType {
    _INT, // integer
    _ADD, // addition
    _SUB, // subtract
    _RET, // return
    _IDN, // identifier
    _STR, // string
    _OUT, // output
    _DMP, // dump stack
    _INV, // invalid
    _EOF, // end of file

    _COUNT
};

struct Token {
    TokenType type;
    int line_number;
    std::optional<std::string> value;
};

std::vector<Token> tokenize(std::string str);
void printTokens(const std::vector<Token> &toks);
