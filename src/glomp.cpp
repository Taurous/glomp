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
extern "C" {
    #include <unistd.h>
    #include <sys/wait.h>
}
#include <algorithm>
#include <filesystem>
namespace fs = std::filesystem;

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
    std::cout << "    -a    generate asm" << std::endl;
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

//TODO: This function is way too simple
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

inline uint64_t pop(std::vector<uint64_t> &s) {
    if (s.empty()) {
        std::cerr << "Runtime Error: stack underflow!" << std::endl;
        exit(EXIT_FAILURE);
    }
    uint64_t a = s.back();
    s.pop_back();
    return a;
}

void dumpStack(const std::vector<uint64_t> &st) {
    for (int i = st.size()-1; i >= 0; --i) {
        std::cout << "[" << i << "] " << st[i] << "\n";
    }
}

uint64_t interpret(const std::vector<Token> tokens) {
    assert((TokenType::_COUNT == 17) && "Exhaustive handling of tokens in interpret()");
    
    std::vector<uint64_t> st;       // Program Stack
    size_t pc = 0;                  // Stack Pointer
    uint64_t return_val = 0;

    while (pc < tokens.size()) {
        const Token &token = tokens[pc++];
        uint64_t a, b, c;
        switch (token.type) {
            case TokenType::_INT:
                st.push_back(std::stoull(token.value));
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
            case TokenType::_MOD:
                b = pop(st);
                a = pop(st);
                if (b == 0) st.push_back(a);
                else st.push_back(a % b);
            break;
            /*case TokenType::_RET:
            // need to figure out how this works.
                a = pop(st);
                std::cout << "Returned " << a << std::endl;
            break;*/
            case TokenType::_IDN:
                assert(false && "_IDN Not yet implemented...\n");
            break;
            case TokenType::_STR:
                assert(false && "_STR Not yet implemented...\n");
            /*// put characters onto stack in reverse and then put size of string
                for (int i = token.value.size() - 1; i >= 0; --i) {
                    st.push_back(int(token.value[i]));
                }
                st.push_back(int(token.value.size()));
            */break;
            case TokenType::_OUT:
                a = pop(st);
                std::cout << a << '\n';
            break;
            // Is dump implementable in assembly or will it destroy the stack?
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
                // b a 
                b = pop(st);
                a = pop(st);
                st.push_back(b);
                st.push_back(a);
            break;
            case TokenType::_DROP:
                pop(st);
            break;
            case TokenType::_EOF:
                return_val = pop(st);
            break;
            case TokenType::_INV:
            default:
                std::cerr << "unreachable in interpret" << std::endl;
            break;
        }
    }
    return return_val;
}

int call_nasm_ld(std::string out_path) {
    std::string asmfile = out_path + ".asm";
    std::string objfile = out_path + ".o";
    
    std::string nasmcmd = "nasm -felf64 " + asmfile + " -o " + objfile;
    std::string ldcmd = "ld " + objfile + " -o " + out_path;
    std::string rmcmd = "rm " + objfile + " " + asmfile;
    std::string cmd = nasmcmd + " && " + ldcmd + " && " + rmcmd;

    int pid;
    int status;

    if ((pid = fork())) {
        //parent, wait
        waitpid(pid, &status, 0);
    } else {
        execl("/bin/bash", "bash", "-c", cmd.c_str(), (char*)NULL);
    }
    return status;
}

void compile(const std::vector<Token> tokens, std::string out_path, bool asmonly) {  
    assert((TokenType::_COUNT == 17) && "Exhaustive handling of tokens in compile()");
    std::ofstream out_file(out_path + ".asm", std::ofstream::trunc | std::ofstream::out);

    if (!out_file.is_open()) {
        std::cerr << "unable to create file: " << out_path << std::endl;
        exit(EXIT_FAILURE);
    }
    writeline(out_file, "BITS 64\n");
    writeline(out_file, "segment .text\n");
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
    writeline(out_file, "    mov     edx, 31");
    writeline(out_file, "    lea     rsi, [rsp+r8]");
    writeline(out_file, "    mov     edi, 1");
    writeline(out_file, "    sub     rdx, rcx");
    writeline(out_file, "    mov     rax, 1");
    writeline(out_file, "    syscall");
    writeline(out_file, "    add     rsp, 40");
    writeline(out_file, "    ret");
    writeline(out_file, "\nglobal _start");
    writeline(out_file, "_start:");

    for (auto &t : tokens) {
        switch (t.type) {
        case TokenType::_INT:
            writeline(out_file, "    push   " + t.value);
        break;
        case TokenType::_STR:
            assert(false && "_STR Not yet implemented\n");
        break;
        case TokenType::_IDN:
            assert(false && "_IDN Not yet implemented\n");
        break;
        case TokenType::_ADD:
            writeline(out_file, "    pop    rcx");
            writeline(out_file, "    pop    rax");
            writeline(out_file, "    add    rax, rcx");
            writeline(out_file, "    push   rax");
        break;
        case TokenType::_SUB:
            writeline(out_file, "    pop    rcx");
            writeline(out_file, "    pop    rax");
            writeline(out_file, "    sub    rax, rcx");
            writeline(out_file, "    push   rax");
        break;
        case TokenType::_MUL:
            writeline(out_file, "    pop    rcx");
            writeline(out_file, "    pop    rax");
            writeline(out_file, "    mul    rcx");
            writeline(out_file, "    push   rax");
        break;
        case TokenType::_DIV:
            writeline(out_file, "    pop    rcx");
            writeline(out_file, "    pop    rax");
            writeline(out_file, "    mov    rdx, 0");
            writeline(out_file, "    div    rcx");
            writeline(out_file, "    push   rax");
        break; 
        case TokenType::_MOD:
            writeline(out_file, "    pop    rcx");
            writeline(out_file, "    pop    rax");
            writeline(out_file, "    div    rcx");
            writeline(out_file, "    push   rdx");
        break; 
        case TokenType::_OUT:
            writeline(out_file, "    pop    rdi");
            writeline(out_file, "    call   out");
        break;
        case TokenType::_DMP:
            assert(false && "_STR Not yet implemented\n");
        break;
        case TokenType::_DUP:
            assert(false && "_DUP Not yet implemented\n");
        break;
        case TokenType::_DUP2:
            assert(false && "_DUP2 Not yet implemented\n");
        break;
        case TokenType::_ROT:
            assert(false && "_ROT Not yet implemented\n");
        break;
        case TokenType::_SWP:
            assert(false && "_SWP Not yet implemented\n");
        break;
        case TokenType::_DROP:
            assert(false && "_DROP Not yet implemented\n");
        break;
        case TokenType::_EOF:
            writeline(out_file, "    mov    rax, 60");
            writeline(out_file, "    pop    di");
            writeline(out_file, "    syscall");
        break;
        case TokenType::_INV:
        default:
            std::cerr << "unreachable - compile()" << std::endl;
            exit(EXIT_FAILURE);
        break;
        }
    }

    out_file.close();

    if (!asmonly) call_nasm_ld(out_path);
}

int main(int argc, char **argv) {
    if (argc <= 2) {
        usage();
        exit(EXIT_FAILURE);
    }
    
    std::string out_file = "glmp.out";
    std::string in_file;
    bool dump = false;
    bool asmonly = false;
    Mode mode = Mode::ERROR;
    for (int i = 1; i < argc; ++i) {
        std::string option = argv[i];
        if (option == "-i") {
            if (mode != Mode::ERROR) {
                std::cerr << "notice: -i and -c are mutually exclusive" << std::endl;
            } else mode = Mode::INTERPRET;
        }
        else if (option == "-c") {
            if (mode != Mode::ERROR) {
                std::cerr << "notice: -i and -c are mutually exclusive" << std::endl;
            } else mode = Mode::COMPILE;
        }
        else if (option == "-d") dump = true;
        else if (option == "-o") {
            if (i + 1 >= argc) { std::cerr << "error: -o must be followed by output path" << std::endl; exit(EXIT_FAILURE); }
            out_file = argv[++i];
        }
        else if (option == "-a") asmonly = true;
        else in_file = argv[i];
    }

    if (in_file.empty()) {
        std::cerr << "error: did not provide input file" << std::endl;
        usage();
        exit(EXIT_FAILURE);
    }
    else if (mode == Mode::ERROR) {
        std::cerr << "error: -i or -c are required" << std::endl;
        usage();
        exit(EXIT_FAILURE);
    }

    std::vector<Token> tokens = tokenize(getSource(in_file));
    if (dump) printTokens(tokens);
    
    if (!validate(tokens)) {
        std::cerr << "Failed" << std::endl;
        exit(EXIT_FAILURE);
    }
    
    int return_val = 0;
    switch (mode) {
        case Mode::INTERPRET:
            return_val = int(interpret(tokens));
            break;
        case Mode::COMPILE:
            compile(tokens, out_file, asmonly);
            break;
        default:
            std::cerr << "unreachable - mode" << std::endl;
            exit(EXIT_FAILURE);
            break;
    }

    return return_val;
}
