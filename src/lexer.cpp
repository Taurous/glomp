#include "lexer.hpp"

#include <iostream>
#include <array>
#include <cassert>

std::string escapeString(const std::string &str) {
    std::stringstream ss;
    for (const auto &c : str) {
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
        else if (src[i] == '+')
            toks.push_back(Token{TokenType::_ADD, line_number});
        else if (src[i] == '-')
            toks.push_back(Token{TokenType::_SUB, line_number});
        else if (src[i] == '*')
            toks.push_back(Token{TokenType::_MUL, line_number});
        else if (src[i] == '/')
            toks.push_back(Token{TokenType::_DIV, line_number});

        // digit encountered
        else if (std::isdigit(src[i])) {
            std::string value;
            bool flt = false;
            bool invalid = false;
            while (true) {
                // white space encountered, end token
                if (src[i] == ' ' || src[i] == '\n') { --i; break; }
                else if (src[i] == '.') {
                    flt = true;
                    value += src[i];
                    ++i;
                }
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

            try {
                if (invalid)
                    toks.push_back(Token{TokenType::_INV, line_number, value});
                else if (flt)
                    toks.push_back(Token{TokenType::_FLT, line_number, std::stof(value)});
                else
                    toks.push_back(Token{TokenType::_INT, line_number, std::stoi(value)});
            }
            catch (const std::exception &e) {
                std::cerr << "Error tokenizing number: " << e.what() << std::endl;
                toks.push_back(Token{TokenType::_INV, line_number, value});
            }
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
            if (ident == "ret")
                toks.push_back(Token{TokenType::_RET, line_number});
            else if (ident == "out")
                toks.push_back(Token{TokenType::_OUT, line_number});
            else if (ident == "dump")
                toks.push_back(Token{TokenType::_DMP, line_number});
            else
                toks.push_back(Token{TokenType::_IDN, line_number, ident});
        }
    }

    toks.push_back(Token{TokenType::_EOF, line_number});

    return toks;
}


void printTokens(const std::vector<Token> &toks) {
    assert((TokenType::_COUNT == 13) && "Exhaustive handling of tokens in printTokens()");
    for (const auto &t : toks) {
        std::string token_name;
        switch (t.type) {
            case TokenType::_INT:
                token_name = "INT";
                break;
            case TokenType::_FLT:
                token_name = "FLT";
                break;
            case TokenType::_ADD:
                token_name = "ADD";
                break;
            case TokenType::_SUB:
                token_name = "SUB";
                break;
            case TokenType::_MUL:
                token_name = "MUL";
                break;
            case TokenType::_DIV:
                token_name = "DIV";
                break;
            case TokenType::_RET:
                token_name = "RET";
                break;
            case TokenType::_STR:
                token_name = "STR";
                break;
            case TokenType::_OUT:
                token_name = "OUT";
                break;
            case TokenType::_INV:
                token_name = "INV";
                break;
            case TokenType::_DMP:
                token_name = "DMP";
                break;
            case TokenType::_IDN:
                token_name = "IDN";
                break;
            case TokenType::_EOF:
                token_name = "EOF";
                break;
            default:
                std::cerr << "unreachable - printTokens()" << std::endl;
                exit(EXIT_FAILURE);
                break;
        }

        std::cout << t.line_number << "    " << token_name;
        switch (t.value.index()) {
            case 1:
                std::cout << "  value: " << std::get<int>(t.value);
            break;
            case 2:
                std::cout << "  value: " << std::get<float>(t.value);
            break;
            case 3:
                std::cout << "  value: " << escapeString(std::get<std::string>(t.value));
            break;
            default:
            break;
        }
        std::cout << "\n";
    }
}
