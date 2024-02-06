#include "lexer.hpp"

#include <iostream>
#include <sstream>
#include <array>
#include <cassert>

Token makeToken(TokenType type, int line_number, std::string value) {
    Token t;
    t.type = type;
    t.line_number = line_number;
    t.value = value;
    switch (t.type) {
        case TokenType::_INT: t.type_as_string = "INT"; break;
        case TokenType::_CHR: t.type_as_string = "CHR"; break;
        case TokenType::_STR: t.type_as_string = "STR"; break;
        case TokenType::_IDN: t.type_as_string = "IDN"; break;
        case TokenType::_ADD: t.type_as_string = "ADD"; break;
        case TokenType::_SUB: t.type_as_string = "SUB"; break;
        case TokenType::_MUL: t.type_as_string = "MUL"; break;
        case TokenType::_DIV: t.type_as_string = "DIV"; break;
        case TokenType::_MOD: t.type_as_string = "MOD"; break;
       // case TokenType::_RET: t.type_as_string = "RET"; break;
        case TokenType::_PUT: t.type_as_string = "PUT"; break;
        case TokenType::_OUT: t.type_as_string = "OUT"; break;
        case TokenType::_DUP2: t.type_as_string = "DUP2"; break;
        case TokenType::_ROT: t.type_as_string = "ROT"; break;
        case TokenType::_SWP: t.type_as_string = "SWP"; break;
        case TokenType::_INV: t.type_as_string = "INV"; break;
        case TokenType::_DMP: t.type_as_string = "DMP"; break;
        case TokenType::_DUP: t.type_as_string = "DUP"; break;
        case TokenType::_DROP: t.type_as_string = "DROP"; break;
        case TokenType::_EOF: t.type_as_string = "EOF"; break;
        default:
            std::cerr << "Exhaustive handling of tokens in makeToken()" << std::endl;
            exit(EXIT_FAILURE);
        break;
    }
    return t;
}

std::vector<Token> tokenize(std::string src) {
    if (src.empty()) {
        std::cerr << "empty file..." << std::endl;
        exit(EXIT_FAILURE);
    }

    std::vector<Token> toks;
    int line_number = 0;

    assert((TokenType::_COUNT == 19) && "Exhaustive handling of tokens in tokenize()");
    for (size_t i = 0; i < src.size(); ++i) {
        // new line, increment line number
        if (src[i] == '\n') {
            ++line_number;
            continue;
        }
        
        // comment - $
        if (src[i] == '$') {
            while (src[i] != '\n') {
                ++i;
                if (i >= src.size()) break; 
            }
            --i; // dec counter so newlines are not skipped
            continue;
        }

        // skip whitespace
        if (src[i] == ' ' || src[i] == '\t') continue;

        // Math
             if (src[i] == '+') toks.push_back(makeToken(TokenType::_ADD, line_number, "+"));
        else if (src[i] == '-') toks.push_back(makeToken(TokenType::_SUB, line_number, "-"));
        else if (src[i] == '*') toks.push_back(makeToken(TokenType::_MUL, line_number, "*"));
        else if (src[i] == '/') toks.push_back(makeToken(TokenType::_DIV, line_number, "/"));
        else if (src[i] == '%') toks.push_back(makeToken(TokenType::_MOD, line_number, "%"));

        // digit encountered
        // TODO: Figure out floats
        else if (std::isdigit(src[i])) {
            std::string value;
            bool invalid = false;
            while (true) {
                // white space encountered, end token
                if (src[i] == ' ' || src[i] == '\n' || src[i] == '\t') { --i; break; }
                else {
                    value += src[i];
                    // if not a digit, token is invalid
                    if (!std::isdigit(src[i])) invalid = true;
                    ++i;
                }
                // eof
                if (i >= src.size())
                    break;
            }

            if (invalid)
                toks.push_back(makeToken(TokenType::_INV, line_number, value));
            else
                toks.push_back(makeToken(TokenType::_INT, line_number, value));
        }

        // string
        else if (src[i] == '"') {
            ++i;
            std::string str;
            while (src[i] != '"') {
                if (src[i] == '\\') {
                    ++i;
                    if (src[i] == 'n') str += '\n';
                    else if (src[i] == 't') str += '\t';
                    else if (src[i] == '\\') str += '\\';
                    else if (src[i] == '"') str += '"';
                    else {
                        std::cout << "Unknown escape sequence: " << src[i-1] << src[i] << std::endl;
                        exit(EXIT_FAILURE);
                    }
                    ++i;
                }
                else str += src[i++];

                if (i >= src.size()) {
                    // eof reached, unclosed string
                    // TODO: Better error handling here (give line_number?)
                    std::cerr << "EOF Reached, Unclosed string!\n";
                    exit(EXIT_FAILURE);
                }
            }

            toks.push_back(makeToken(TokenType::_STR, line_number, str));
        }
        
        // character
        // TODO: Does not handle escaped characters (though you can just push an int and call put to treat it as char)
        else if (src[i] == '\'') {
            if (i+2 >= src.size()) {
                std::cerr << "Error: Unclosed char at EOF\n";
                exit(EXIT_FAILURE);
            }
            else if (src[i+2] != '\'') {
                std::cerr << "Error: char definition must be patter 'x' line: " << line_number << "\n";
                exit(EXIT_FAILURE);
            }
            else {
                std::string str;
                uint64_t chr = uint64_t(src[i+1]);
                str += std::to_string(chr);
                toks.push_back(makeToken(TokenType::_CHR, line_number, str));
            }
            i+=3;
        }

        // identifier
        else {
            std::string ident;
            while (src[i] != ' ' && src[i] != '\n' && src[i] != '\t') {
                ident += src[i];
                ++i;
            }
            --i;
            // check for keywords
            //if (ident == "ret")         toks.push_back(makeToken(TokenType::_RET, line_number, ident));
            if (ident == "out")    toks.push_back(makeToken(TokenType::_OUT, line_number, ident));
            else if (ident == "put")    toks.push_back(makeToken(TokenType::_PUT, line_number, ident));
            else if (ident == "dump")   toks.push_back(makeToken(TokenType::_DMP, line_number, ident));
            else if (ident == "dup")    toks.push_back(makeToken(TokenType::_DUP, line_number, ident));
            else if (ident == "dup2")   toks.push_back(makeToken(TokenType::_DUP2, line_number, ident));
            else if (ident == "rot")    toks.push_back(makeToken(TokenType::_ROT, line_number, ident));
            else if (ident == "swap")   toks.push_back(makeToken(TokenType::_SWP, line_number, ident));
            else if (ident == "drop")   toks.push_back(makeToken(TokenType::_DROP, line_number, ident));
            else                        toks.push_back(makeToken(TokenType::_IDN, line_number, ident));
        }
    }

    toks.push_back(Token{TokenType::_EOF, line_number, "EOF"});

    return toks;
}

void printTokens(const std::vector<Token> &toks) {
    for (const auto &t : toks) {
        // Escape String (move into own function)
        std::stringstream ss;
        for (auto &c : t.value) {
            switch (c) {
                case '\\': ss << "\\\\"; break;
                case '\'': ss << "\\'"; break;
                case '\"': ss << "\\\""; break;
                case '\n': ss << "\\n"; break;
                case '\t': ss << "\\t"; break;
                default: ss << c; break;
            }
        }
        std::cout << t.line_number << "    " << t.type_as_string << " - value: " << ss.str() << "\n";
    }
}
