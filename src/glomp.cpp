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

void writeline(std::ofstream &ofs, std::string str) {
    ofs << str << "\n";
}

void usage() {
    std::cout << "Usage: glomp [option] <input.glmp>" << std::endl;
    std::cout << "    -i    interpret program" << std::endl;
    std::cout << "    -c    compile program" << std::endl;
    std::cout << "    -d    dump tokens to stdout" << std::endl;
    std::cout << "    -o    <output_path>" << std::endl;
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
    ERROR,
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
    assert((TokenType::_COUNT == 16) && "Exhaustive handling of tokens in interpret()");
    std::vector<int> st;
    size_t pc = 0;
    while (pc < tokens.size()) {
        const Token &token = tokens[pc++];
        int a, b, c;
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
            case TokenType::_DUP2:
                b = pop(st);
                a = pop(st);
                st.push_back(a);
                st.push_back(b);
                st.push_back(a);
                st.push_back(b);
            break;
            case TokenType::_ROT:
                c = pop(st);
                b = pop(st);
                a = pop(st);
                st.push_back(b);
                st.push_back(c);
                st.push_back(a);
            break;
            case TokenType::_SWP:
                b = pop(st);
                a = pop(st);
                st.push_back(b);
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

void compile(const std::vector<Token> tokens, std::string out_path) {  
    assert((TokenType::_COUNT == 16) && "Exhaustive handling of tokens in interpret()");

    std::ofstream out_file(out_path, std::ofstream::trunc | std::ofstream::out);

    if (!out_file.is_open()) {
        std::cerr << "unable to create file: " << out_path << std::endl;
        exit(EXIT_FAILURE);
    }
    writeline(out_file, "segment .text");
    out_file << "\n";
    // `out` subroutine - prints uint64_t to stdout
    writeline(out_file, "out:");
    writeline(out_file, "    sub     rsp, 40");
    writeline(out_file, "    mov     ecx, 30");
    writeline(out_file, "    mov     r9, -3689348814741910323");
    writeline(out_file, "    mov     BYTE [rsp+31], 10");
    writeline(out_file, ".L2:");
    writeline(out_file, "    mov     rax, rdi");
    writeline(out_file, "    mov     r8, rcx");
    writeline(out_file, "    sub     rcx, 1");
    writeline(out_file, "    mul     r9");
    writeline(out_file, "    mov     rax, rdi");
    writeline(out_file, "    shr     rdx, 3");
    writeline(out_file, "    lea     rsi, [rdx+rdx*4]");
    writeline(out_file, "    add     rsi, rsi");
    writeline(out_file, "    sub     rax, rsi");
    writeline(out_file, "    add     eax, 48");
    writeline(out_file, "    mov     BYTE [rsp+1+rcx], al");
    writeline(out_file, "    mov     rax, rdi");
    writeline(out_file, "    mov     rdi, rdx");
    writeline(out_file, "    cmp     rax, 9");
    writeline(out_file, "    ja      .L2");
    writeline(out_file, "    mov     edx, 32");
    writeline(out_file, "    lea     rsi, [rsp+r8]");
    writeline(out_file, "    mov     edi, 1");
    writeline(out_file, "    sub     rdx, rcx");
    writeline(out_file, "    mov     rax, 1");
    writeline(out_file, "    syscall");
    writeline(out_file, "    add     rsp, 40");
    writeline(out_file, "    ret");
    out_file << "\n";
    writeline(out_file, "global _start");
    writeline(out_file, "_start:");

    for (auto &t : tokens) {
        switch (t.type) {
        case TokenType::_INT:
            writeline(out_file, "    push   " + t.value);
        break;
        case TokenType::_ADD:
            writeline(out_file, "    pop    rax");
            writeline(out_file, "    pop    rbx");
            writeline(out_file, "    add    rax, rbx");
            writeline(out_file, "    push   rax");
        break;
        case TokenType::_OUT:
            writeline(out_file, "    pop    rdi");
            writeline(out_file, "    call   out");
        break;
        default:
            std::cerr << "Token not implemented yet...\n";
        break;
        }
    }

    writeline(out_file, "    mov    rax, 60");
    writeline(out_file, "    mov    rdi, 0");
    writeline(out_file, "    syscall");
}

int main(int argc, char **argv) {
    if (argc <= 2) {
        usage();
        exit(EXIT_FAILURE);
    }
    
    std::string out_file = "glmp.out";
    bool dump = false;
    Mode mode = Mode::ERROR;
    for (int i = 1; i < argc-1; ++i) {
        std::string option = argv[i];
        if (option == "-i") { if (mode != Mode::ERROR) { std::cerr << "error: -i and -c are mutually exclusive" << std::endl; } else mode = Mode::INTERPRET; }
        else if (option == "-c") { if (mode != Mode::ERROR) { std::cerr << "error: -i and -c are mutually exclusive" << std::endl; } else mode = Mode::COMPILE; }
        else if (option == "-d") dump = true;
        else if (option == "-o") {
            if (i + 1 >= argc - 1) { std::cerr << "error: -o" << std::endl; exit(EXIT_FAILURE); }
            out_file = argv[++i];
        }
        else {
            std::cerr << "invalid option: " << option << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    
    if (mode == Mode::ERROR) {
        std::cerr << "error: -i or -c are required" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::vector<Token> tokens = tokenize(getSource(argv[argc-1]));
    if (dump) printTokens(tokens);
    
    if (!validate(tokens)) {
        std::cerr << "Failed" << std::endl;
        exit(EXIT_FAILURE);
    }

    switch (mode) {
        case Mode::INTERPRET:
            interpret(tokens);
            break;
        case Mode::COMPILE:
            compile(tokens, out_file);
            break;
        default:
            std::cerr << "unreachable - mode" << std::endl;
            exit(EXIT_FAILURE);
            break;
    }

    exit(EXIT_SUCCESS);
}
