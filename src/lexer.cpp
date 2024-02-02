#include "lexer.hpp"

#include <iostream>
#include <sstream>
#include <array>
#include <cassert>

std::vector<Token> tokenize(std::string src) {
    if (src.empty()) {
        std::cerr << "empty file..." << std::endl;
        exit(EXIT_FAILURE);
    }

    std::vector<Token> toks;
    int line_number = 0;

    assert((TokenType::_COUNT == 13) && "Exhaustive handling of tokens in tokenize()");
    for (size_t i = 0; i < src.size(); ++i) {
        // new line, increment line number
        if (src[i] == '\n') {
            ++line_number;
            continue;
        }

        // skip whitespace
        else if (src[i] == ' ' || src[i] == '\t')
            continue;

        // Math
        else if (src[i] == '+') toks.push_back(Token{TokenType::_ADD, line_number, "+"});
        else if (src[i] == '-') toks.push_back(Token{TokenType::_SUB, line_number, "-"});
        else if (src[i] == '*') toks.push_back(Token{TokenType::_MUL, line_number, "*"});
        else if (src[i] == '/') toks.push_back(Token{TokenType::_DIV, line_number, "/"});

        // digit encountered
        // TODO: Figure out floats
        else if (std::isdigit(src[i])) {
            std::string ident;
            bool invalid = false;
            while (true) {
                // white space encountered, end token
                if (src[i] == ' ' || src[i] == '\n') { --i; break; }
                else {
                    ident += src[i];
                    // if not a digit, token is invalid
                    if (!std::isdigit(src[i])) invalid = true;
                    ++i;
                }
                // eof
                if (i >= src.size())
                    break;
            }

            if (invalid)
                toks.push_back(Token{TokenType::_INV, line_number, ident});
            else
                toks.push_back(Token{TokenType::_INT, line_number, ident});
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

            toks.push_back(Token{TokenType::_STR, line_number, str});
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
            if (ident == "ret")         toks.push_back(Token{TokenType::_RET, line_number, ident});
            else if (ident == "out")    toks.push_back(Token{TokenType::_OUT, line_number, ident});
            else if (ident == "dump")   toks.push_back(Token{TokenType::_DMP, line_number, ident});
            else if (ident == "dup")    toks.push_back(Token{TokenType::_DUP, line_number, ident});
            else                        toks.push_back(Token{TokenType::_IDN, line_number, ident});
        }
    }

    toks.push_back(Token{TokenType::_EOF, line_number, "EOF"});

    return toks;
}

void printTokens(const std::vector<Token> &toks) {
    assert((TokenType::_COUNT == 13) && "Exhaustive handling of tokens in printTokens()");
    for (const auto &t : toks) {
        std::string token_name;
        switch (t.type) {
            case TokenType::_INT: token_name = "INT"; break;
            case TokenType::_STR: token_name = "STR"; break;
            case TokenType::_IDN: token_name = "IDN"; break;
            case TokenType::_ADD: token_name = "ADD"; break;
            case TokenType::_SUB: token_name = "SUB"; break;
            case TokenType::_MUL: token_name = "MUL"; break;
            case TokenType::_DIV: token_name = "DIV"; break;
            case TokenType::_RET: token_name = "RET"; break;
            case TokenType::_OUT: token_name = "OUT"; break;
            case TokenType::_INV: token_name = "INV"; break;
            case TokenType::_DMP: token_name = "DMP"; break;
            case TokenType::_DUP: token_name = "DUP"; break;
            case TokenType::_EOF: token_name = "EOF"; break;
            default:
                std::cerr << "unreachable - printTokens()" << std::endl;
                exit(EXIT_FAILURE);
                break;
        }

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
        std::cout << t.line_number << "    " << token_name << " - value: " << ss.str() << "\n";
    }
}
