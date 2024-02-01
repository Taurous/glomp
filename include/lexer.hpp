#pragma once

#include <vector>
#include <string>
#include <sstream>
#include <variant>

using data = std::variant<std::monostate, int, float, std::string>;

enum TokenType {
    // Data Types
    _INT, // integer        default 32bit
    _FLT, // float          default 32bit must start with digit in front of decimal
    _STR, // string         non terminated string

    // Math
    _ADD, // addition       +
    _SUB, // subtract       -
    _MUL, // multiply       *
    _DIV, // divide         /

    // Keywords
    _RET, // return         ret
    _OUT, // output         out
    _DMP, // dump stack     dump

    // Other
    _IDN, // identifier     function/variable/label/etc
    _INV, // invalid        bad token(eg. 123var (identifier cannot start with a number))
    _EOF, // end of file

    _COUNT
};

struct Token {
    TokenType type;
    int line_number;
    data value;
};

std::vector<Token> tokenize(std::string str);
void printTokens(const std::vector<Token> &toks);

std::string escapeString(const std::string &str);
