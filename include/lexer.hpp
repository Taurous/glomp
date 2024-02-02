#pragma once

#include <vector>
#include <string>
#include <optional>

enum TokenType {
    // Data Types
    _INT, // integer
    _STR, // string
    _IDN, // identifier

    // Operations
    _ADD, // addition
    _SUB, // subtract
    _MUL, // multiply
    _DIV, // divide
    
    // Keywords
    _RET, // returns
    _OUT, // output
    _DMP, // dump stack
    _DUP, // duplicate

    // Other
    _INV, // invalid
    _EOF, // end of file

    _COUNT
};

struct Token {
    TokenType type;
    int line_number;
    std::string value;
};

std::vector<Token> tokenize(std::string str);
void printTokens(const std::vector<Token> &toks);
