#pragma once

#include <string>

enum TokenType {
    // Data Types
    _INT, // integer
    _CHR, // char
    _STR, // string
    _IDN, // identifier

    // Operations
    _ADD, // addition
    _SUB, // subtract
    _MUL, // multiply
    _DIV, // divide
    _MOD, // modulus (remainder of division)

    // Keywords
//  _RET, // returns
    _OUT, // output integer to stdout
    _PUT, // output char to stdout
    _DMP, // dump stack
    _DUP, // duplicate
    _DUP2, // duplicate x2
    _ROT, // rotate
    _SWP, // swap
    _DROP,// remove item from stack

    // Other
    _INV, // invalid
    _EOF, // end of file

    _COUNT
};

struct Token {
    TokenType type = TokenType::_INV;
    int line_number;
    std::string value;
    std::string type_as_string;
};
