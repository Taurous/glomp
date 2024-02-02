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

inline int pop(std::vector<int> &s) {
    if (s.empty()) {
        std::cerr << "Runtime Error: stack underflow!" << std::endl;
        exit(EXIT_FAILURE);
    }
    int a = s.back();
    s.pop_back();
    return a;
}

void dumpStack(const std::vector<int> &st) {
    for (int i = st.size()-1; i >= 0; --i) {
        std::cout << "[" << i << "] " << st[i] << "\n";
    }
}

void interpret(const std::vector<Token> tokens) {
    assert((TokenType::_COUNT == 13) && "Exhaustive handling of tokens in interpret()");
    std::vector<int> st;
    size_t pc = 0;
    while (pc < tokens.size()) {
        const Token &token = tokens[pc++];
        int a, b;
        std::string str;
        switch (token.type) {
            case TokenType::_INT:
                st.push_back(std::stoi(token.value));
            break;
            case TokenType::_ADD:
                b = pop(st);
                a = pop(st);
                st.push_back(a + b);
            break;
            case TokenType::_SUB:
                b = pop(st);
                a = pop(st);
                st.push_back(a - b);
            break;
            case TokenType::_MUL:
                b = pop(st);
                a = pop(st);
                st.push_back(a * b);
            break;
            case TokenType::_DIV:
                b = pop(st);
                a = pop(st);
                if (b == 0) { std::cerr << "Divide by zero! Line number " << token.line_number << std::endl; exit(EXIT_FAILURE); }
                st.push_back(a / b);
            break;
            case TokenType::_RET:
            // need to figure out how this works.
                a = pop(st);
                std::cout << "Returned " << a << std::endl;
            break;
            case TokenType::_IDN:
            // this is going to be complicated.
            break;
            case TokenType::_STR:
            // put characters onto stack in reverse and then put size of string
                for (int i = token.value.size() - 1; i >= 0; --i) {
                    st.push_back(int(token.value[i]));
                }
                st.push_back(int(token.value.size()));
            break;
            case TokenType::_OUT:
                a = pop(st);
                str = "";
                for (int i = 0; i < a; ++i) {
                    str += char(pop(st));
                }
                std::cout << str;
            break;
            case TokenType::_DMP:
                std::cout << "Dumping stack:\n";
                dumpStack(st);
            break;
            case TokenType::_DUP:
                // a b c -> a b c c
                a = pop(st);
                st.push_back(a);
                st.push_back(a);
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
