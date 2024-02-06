#include "interpreter.hpp"

#include <iostream>
#include <cassert>
#include <cinttypes>

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

int interpret(const std::vector<Token> &tokens) {
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
    return int(return_val);
}
