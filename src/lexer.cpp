#include "lexer.hpp"

#include <iostream>
#include <sstream>
#include <array>
#include <cassert>

std::string escapeStr(std::string str) {
    std::stringstream ss;
    for (auto &c : str) {
        switch (c) {
            case '\\': ss << "\\\\"; break;
            case '\'': ss << "\\'"; break;
            case '\"': ss << "\\\""; break;
            case '\n': ss << "\\n"; break;
            case '\t': ss << "\\t"; break;
            default: ss << c; break;
        }
    }
    return ss.str();
}

Token makeToken(TokenType type, int line, int column, uint64_t value = 0) {
    Token t;
    t.type = type;
    t.line = line;
    t.value = value;
    switch (t.type) {
        case TokenType::_INT: t.as_string = "INT"; break;
        case TokenType::_CHR: t.as_string = "CHR"; break;
        case TokenType::_STR: t.as_string = "STR"; break;
        case TokenType::_IDN: t.as_string = "IDN"; break;
        case TokenType::_ADD: t.as_string = "ADD"; break;
        case TokenType::_SUB: t.as_string = "SUB"; break;
        case TokenType::_MUL: t.as_string = "MUL"; break;
        case TokenType::_DIV: t.as_string = "DIV"; break;
        case TokenType::_MOD: t.as_string = "MOD"; break;
       // case TokenType::_RET: t.as_string = "RET"; break;
        case TokenType::_PUT: t.as_string = "PUT"; break;
        case TokenType::_OUT: t.as_string = "OUT"; break;
        case TokenType::_DUP2: t.as_string = "DUP2"; break;
        case TokenType::_ROT: t.as_string = "ROT"; break;
        case TokenType::_SWP: t.as_string = "SWP"; break;
        case TokenType::_INV: t.as_string = "INV"; break;
        case TokenType::_DMP: t.as_string = "DMP"; break;
        case TokenType::_DUP: t.as_string = "DUP"; break;
        case TokenType::_DROP: t.as_string = "DROP"; break;
        case TokenType::_IF: t.as_string = "IF"; break;
        case TokenType::_END: t.as_string = "END"; break;
        case TokenType::_GR: t.as_string = "GR"; break;
        case TokenType::_GE: t.as_string = "GE"; break;
        case TokenType::_EQ: t.as_string = "EQ"; break;
        case TokenType::_LE: t.as_string = "LE"; break;
        case TokenType::_LT: t.as_string = "LT"; break;
        case TokenType::_NT: t.as_string = "NT"; break;
        case TokenType::_EOF: t.as_string = "EOF"; break;
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
    int line = 0;
    int column = 0;

    assert((TokenType::_COUNT == 27) && "Exhaustive handling of tokens in tokenize()");
    for (size_t i = 0; i < src.size(); ++i) {
        // new line, increment line number
        if (src[i] == '\n') {
            ++line;
            column = 0;
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
        if (src[i] == ' ' || src[i] == '\t') { ++column; continue; }

        // Math and conditions
             if (src[i] == '+') toks.push_back(makeToken(TokenType::_ADD, line, column));
        else if (src[i] == '-') toks.push_back(makeToken(TokenType::_SUB, line, column));
        else if (src[i] == '*') toks.push_back(makeToken(TokenType::_MUL, line, column));
        else if (src[i] == '/') toks.push_back(makeToken(TokenType::_DIV, line, column));
        else if (src[i] == '%') toks.push_back(makeToken(TokenType::_MOD, line, column));
        else if (src[i] == '>') {
            if (i+1 < src.size() && src[i+1] == '=') toks.push_back(makeToken(TokenType::_GE, line, column));
            else toks.push_back(makeToken(TokenType::_GR, line, column));
        }
        else if (src[i] == '<') {
            if (i+1 < src.size() && src[i+1] == '=') toks.push_back(makeToken(TokenType::_LE, line, column));
            else toks.push_back(makeToken(TokenType::_LT, line, column));
        }
        else if (src[i] == '=') toks.push_back(makeToken(TokenType::_EQ, line, column));
        else if (src[i] == '!') toks.push_back(makeToken(TokenType::_NT, line, column));

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
                toks.push_back(makeToken(TokenType::_INV, line, column));
            else
                toks.push_back(makeToken(TokenType::_INT, line, column, std::stoull(value)));
        }

        // string
        /*else if (src[i] == '"') {
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
                        // TODO: Better error handling here
                        std::cout << "Unknown escape sequence: " << src[i-1] << src[i] << std::endl;
                        exit(EXIT_FAILURE);
                    }
                    ++i;
                }
                else str += src[i++];

                if (i >= src.size()) {
                    // eof reached, unclosed string
                    // TODO: Better error handling here (give line?)
                    std::cerr << "EOF Reached, Unclosed string!\n";
                    exit(EXIT_FAILURE);
                }
            }

            toks.push_back(makeToken(TokenType::_STR, line, str));
        }*/
        
        // char
        // TODO: Does not handle escaped characters (though you can just push an int and call `put` to treat it as char)
        else if (src[i] == '\'') {
            if (i+2 >= src.size()) {
                std::cerr << "Error: Unclosed char at EOF\n";
                exit(EXIT_FAILURE);
            }
            else if (src[i+2] != '\'') {
                std::cerr << "Error: char definition must be pattern 'x' line: " << line << "\n";
                exit(EXIT_FAILURE);
            }
            else {
                toks.push_back(makeToken(TokenType::_CHR, line, column, uint64_t(src[i+1])));
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
            //if (ident == "ret")         toks.push_back(makeToken(TokenType::_RET, line, column));
                 if (ident == "out")    toks.push_back(makeToken(TokenType::_OUT,  line, column));
            else if (ident == "put")    toks.push_back(makeToken(TokenType::_PUT,  line, column));
            else if (ident == "dump")   toks.push_back(makeToken(TokenType::_DMP,  line, column));
            else if (ident == "dup")    toks.push_back(makeToken(TokenType::_DUP,  line, column));
            else if (ident == "dup2")   toks.push_back(makeToken(TokenType::_DUP2, line, column));
            else if (ident == "rot")    toks.push_back(makeToken(TokenType::_ROT,  line, column));
            else if (ident == "swap")   toks.push_back(makeToken(TokenType::_SWP,  line, column));
            else if (ident == "drop")   toks.push_back(makeToken(TokenType::_DROP, line, column));
            else if (ident == "if")     toks.push_back(makeToken(TokenType::_IF,   line, column));
            else if (ident == "end")    toks.push_back(makeToken(TokenType::_END,  line, column));
            else                        toks.push_back(makeToken(TokenType::_IDN,  line, column));
        }
    ++column;
    }

    toks.push_back(Token{TokenType::_EOF, line, column});

    return toks;
}

void printTokens(const std::vector<Token> &toks) {
    for (const auto &t : toks) {
        switch (t.type) {
            case TokenType::_INT:
                std::cout << t.line << "    " << t.as_string << " - value: " << t.value << "\n";
            break;
            case TokenType::_CHR:
                std::cout << t.line << "    " << t.as_string << " - value: " << char(t.value) << "\n";
            break;
            default:
                std::cout << t.line << "    " << t.as_string << "\n";
            break;
        }
    }
}
