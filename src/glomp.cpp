#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stack>
#include <iomanip>
#include <optional>
#include <sstream>
#include <cctype>
#include <cassert>

#include "lexer.hpp"

void usage() {
    std::cout << "Usage: glomp [option] <input.glmp>" << std::endl;
    std::cout << "    -i    interpret program" << std::endl;
    std::cout << "    -c    compile program" << std::endl;
}

std::string getSource(std::string path) {
    std::ifstream file(path);

    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << path << std::endl;
        exit(EXIT_FAILURE);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    return buffer.str();
}

bool validate(const std::vector<Token>& tokens) {
    if (std::find_if(tokens.begin(), tokens.end(), [](const Token &t){ return t.type == TokenType::_INV; }) != tokens.end()) {
        std::cout << "Invalid Token" << std::endl;
        return false;
    }

    return true;
}

enum class Mode {
    COMPILE,
    INTERPRET
};

inline data pop(std::vector<data> &s) {
    if (s.empty()) {
        std::cerr << "Runtime Error: stack underflow!" << std::endl;
        exit(EXIT_FAILURE);
    }
    data a = s.back();
    s.pop_back();
    return a;
}

struct make_string_functor {
    std::string operator()(const std::string &s) const { return s; }
    std::string operator()(int x) const { return std::to_string(x); }
    std::string operator()(float x) const { return std::to_string(x); }
    std::string operator()(const std::monostate &s) const { return ""; }
};

void dumpStack(const std::vector<data> &st) {
    for (int i = st.size()-1; i >= 0; --i) {
        std::cout << "[" << i << "] " << escapeString(std::visit(make_string_functor(), st[i])) << "\n";
    }
}

template <typename T, N>
data add(data d1, data d2) {
    return data{ std::get<T>(d1) + std::get<N>(d2) };
}

template <typename T>
data add<std::string, T>(data d1, data d2) {
    return data{ std::get<std::string>(d1) + std::visit(make_string_functor(), d2) };
}

template <typename T>
data add<T, std::string>(data d1, data d2) {
    std::stringstream ss;
    ss << std::visit(make_string_functor(), d1);
    ss << std::get<std::string>(d2);
    return ss.str();
}

template <typename T, N>
data sub(data d1, data d2) {
    return data{ std::get<T>(d1) - std::get<N>(d2) };
}

template <typename T, N>
data mul(data d1, data d2) {
    return data{ std::get<T>(d1) * std::get<N>(d2) };
}

template <typename T, N>
data div(data d1, data d2) {
    if (std::get<T>(d2) == 0) { std::cerr << "Divide by zero!\n"; exit(EXIT_FAILURE); }
    return data{ std::get<T>(d1) / std::get<N>(d2) };
}

void interpret(const std::vector<Token> tokens) {
    assert((TokenType::_COUNT == 13) && "Exhaustive handling of tokens in interpret()");
    std::vector<data> prog_stack;
    size_t pc = 0;
    while (pc < tokens.size()) {
        const Token &token = tokens[pc++];
        data a, b;
        std::string str;
        switch (token.type) {
            case TokenType::_INT:
                prog_stack.push_back(std::get<int>(token.value));
            break;
            case TokenType::_FLT:
                prog_stack.push_back(std::get<float>(token.value));
            break;
            case TokenType::_STR:
                prog_stack.push_back(std::get<std::string>(token.value));
            break;
            case TokenType::_ADD:
                b = pop(prog_stack);
                a = pop(prog_stack);
                switch (a.index()) {
                    case 1:
                        a = add<int>(a, b);
                    break;
                    case 2:
                        a = add<float>(a, b);
                    break;
                    case 3:
                        a = add<std::string>(a, b);
                    break;
                    default:
                    break;
                }
                prog_stack.push_back(a);
            break;
            case TokenType::_SUB:
                b = pop(prog_stack);
                a = pop(prog_stack);
                switch (a.index()) {
                    case 1:
                        a = sub<int>(a, b);
                    break;
                    case 2:
                        a = sub<float>(a, b);
                    break;
                    default:
                        std::cout << "whoops\n";
                    break;
                }
                prog_stack.push_back(a);
            break;
            case TokenType::_MUL:
                b = pop(prog_stack);
                a = pop(prog_stack);
                switch (a.index()) {
                    case 1:
                        a = mul<int>(a, b);
                    break;
                    case 2:
                        a = mul<float>(a, b);
                    break;
                    default:
                    break;
                }
                prog_stack.push_back(a);
            break;
            case TokenType::_DIV:
                b = pop(prog_stack);
                a = pop(prog_stack);
                switch (a.index()) {
                    case 1:
                        a = div<int>(a, b);
                    break;
                    case 2:
                        a = div<float>(a, b);
                    break;
                    default:
                    break;
                }
                prog_stack.push_back(a);
            break;
            case TokenType::_RET:
                a = pop(prog_stack);
                try { std::cout << "Returned " << std::get<int>(a) << "\n"; }
                catch (const std::exception &e) { std::cerr << "non integer value returned: " << e.what() << "\n"; }
            break;
            case TokenType::_OUT:
                a = pop(prog_stack);
                std::cout << std::visit(make_string_functor(), a);
            break;
            case TokenType::_DMP:
                std::cout << "Dumping stack:\n";
                dumpStack(prog_stack);
            break;
            case TokenType::_IDN:
            // this is going to be complicated.
            break;
            case TokenType::_EOF:
            break;
            case TokenType::_INV:
            default:
                std::cerr << "unreachable in interpret" << std::endl;
            break;
        }
    }
}

void compile(const std::vector<Token> tokens) {
}

int main(int argc, char **argv) {
    if (argc <= 2) {
        usage();
        exit(EXIT_FAILURE);
    }

    Mode mode;
    if (std::string(argv[1]) == "-i") mode = Mode::INTERPRET;
    else if (std::string(argv[1]) == "-c") mode = Mode::COMPILE;
    else {
        std::cerr << "invalid option: " << argv[1] << std::endl;
        exit(EXIT_FAILURE);
    }

    std::vector<Token> tokens = tokenize(getSource(argv[2]));
    printTokens(tokens);
    
    if (!validate(tokens)) {
        std::cerr << "Failed" << std::endl;
        exit(EXIT_FAILURE);
    }

    switch (mode) {
        case Mode::INTERPRET:
            interpret(tokens);
            break;
        case Mode::COMPILE:
            compile(tokens);
            break;
        default:
            std::cerr << "unreachable - mode" << std::endl;
            exit(EXIT_FAILURE);
            break;
    }

    exit(EXIT_SUCCESS);
}
