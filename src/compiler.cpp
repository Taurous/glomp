#include "compiler.hpp"

extern "C" {
    #include <unistd.h> // for execl
    #include <sys/wait.h> // for waitpid
}
#include <cassert>
#include <cstdio> // For std::remove
#include <iostream>
#include <fstream>
#include <algorithm>
#include <filesystem>
namespace fs = std::filesystem;

void writeline(std::ofstream &ofs, std::string str) {
    ofs << str << "\n";
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

void compile(const std::vector<Token> &tokens, std::string out_path, bool asmonly) {  
    assert((TokenType::_COUNT == 19) && "Exhaustive handling of tokens in compile()");
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
    
    // printchar 
    writeline(out_file, "\nprintchar:");
    writeline(out_file, "    push    rdi");
    writeline(out_file, "    mov     rax, 1");
    writeline(out_file, "    mov     rdi, 1");
    writeline(out_file, "    mov     rsi, rsp");
    writeline(out_file, "    mov     rdx, 1");
    writeline(out_file, "    syscall");
    writeline(out_file, "    pop     rdi");
    writeline(out_file, "    ret");

    // only generate dumpstack if it is called
    auto is_dump = [](const Token& t) { return t.type == TokenType::_DMP; };
    if (std::find_if(std::begin(tokens), std::end(tokens), is_dump) != tokens.end()) {
        writeline(out_file, "\ndumpstack:");
        writeline(out_file, "    mov     rcx, rbp");
        writeline(out_file, "    sub     rcx, rsp");
        writeline(out_file, "    mov     rdx, 8");
        writeline(out_file, "dloop:");
        writeline(out_file, "    mov     rdi, [rsp+rdx]");
        writeline(out_file, "    push    rcx");
        writeline(out_file, "    push    rdx");
        writeline(out_file, "    call    out");
        writeline(out_file, "    pop     rdx");
        writeline(out_file, "    pop     rcx");
        writeline(out_file, "    add     rdx, 8");
        writeline(out_file, "    mov     rax, rcx");
        writeline(out_file, "    sub     rax, rdx");
        writeline(out_file, "    jnz     dloop");
        writeline(out_file, "    ret");
    }

    // entry point
    writeline(out_file, "\nglobal _start");
    writeline(out_file, "_start:");
    writeline(out_file, "; store pointer of bottom of stack in rbp");
    writeline(out_file, "    mov     rbp, rsp");

    auto quit = [&](std::string msg) {
        out_file.close();
        out_path += ".asm";
        int result = std::remove(out_path.c_str());
        if (result) std::cerr << "unable to delete: " << out_path << std::endl;
        std::cerr << msg << std::endl;
        exit(EXIT_FAILURE);
    };
    
    for (auto &t : tokens) {
        switch (t.type) {
        case TokenType::_INT:
        case TokenType::_CHR:
            writeline(out_file, "    push    " + t.value);
        break;
        case TokenType::_STR:
            quit("_STR NYI");
        break;
        case TokenType::_IDN:
            quit("_IDN NYI");
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
        case TokenType::_PUT:
            writeline(out_file, "    pop    rdi");
            writeline(out_file, "    call   printchar");
        break;
        case TokenType::_DMP:
            writeline(out_file, "    call   dumpstack");
        break;
        case TokenType::_DUP:
            writeline(out_file, "    pop    rax");
            writeline(out_file, "    push   rax");
            writeline(out_file, "    push   rax");
        break;
        case TokenType::_DUP2:
            writeline(out_file, "    pop    rax");
            writeline(out_file, "    pop    rcx");
            writeline(out_file, "    push   rcx");
            writeline(out_file, "    push   rax");
            writeline(out_file, "    push   rcx");
            writeline(out_file, "    push   rax");
        break;
        case TokenType::_ROT:
            writeline(out_file, "    pop    rax"); //3
            writeline(out_file, "    pop    rcx"); //2
            writeline(out_file, "    pop    rdx"); //1
            writeline(out_file, "    push   rcx"); // 2
            writeline(out_file, "    push   rax");// 1
            writeline(out_file, "    push   rdx");// 3
        break;
        case TokenType::_SWP:
            writeline(out_file, "    pop    rax");
            writeline(out_file, "    pop    rcx");
            writeline(out_file, "    push   rax");
            writeline(out_file, "    push   rcx");
        break;
        case TokenType::_DROP:
            writeline(out_file, "; drop clobbers rax, consider this in the future");
            writeline(out_file, "    pop    rax");
            //writeline(out_file, "    mov    rax, 0");
            //writeline(out_file, "    add    rsp, 8");
        break;
        case TokenType::_EOF:
            writeline(out_file, "    mov    rax, 60");
            writeline(out_file, "    pop    rdi");
            writeline(out_file, "    syscall");
        break;
        case TokenType::_INV:
        default:
            quit("unreachable - compile()");
        break;
        }
    }

    out_file.close();

    if (!asmonly) call_nasm_ld(out_path);
}
