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

void dumpStack(const std::vector<uint64_t> &data_stack) {
    std::cout << "Dumping stack:\n";
    for (int i = data_stack.size()-1; i >= 0; --i) {
        std::cout << "[" << i << "] " << data_stack[i] << "\n";
    }
}

int interpret(const std::vector<Token> &tokens) {
    assert((TokenType::_COUNT == 27) && "Exhaustive handling of tokens in interpret()");
    
    std::vector<uint64_t> data_stack;   // Program Stack
    size_t pc = 0;

    uint64_t return_val = 0;
    while (pc < tokens.size()) {
        const Token &token = tokens[pc++];
        uint64_t a, b, c;
        switch (token.type) {
            case TokenType::_INT:
                data_stack.push_back(token.value);
            break;
            case TokenType::_CHR:
                data_stack.push_back(token.value);
            break;
            case TokenType::_ADD:
                b = pop(data_stack);
                a = pop(data_stack);
                data_stack.push_back(a + b);
            break;
            case TokenType::_SUB:
                b = pop(data_stack);
                a = pop(data_stack);
                data_stack.push_back(a - b);
            break;
            case TokenType::_MUL:
                b = pop(data_stack);
                a = pop(data_stack);
                data_stack.push_back(a * b);
            break;
            case TokenType::_DIV:
                b = pop(data_stack);
                a = pop(data_stack);
                if (b == 0) { std::cerr << "Divide by zero! Location " << token.line << ":" << token.column << std::endl; exit(EXIT_FAILURE); }
                data_stack.push_back(a / b);
            break;
            case TokenType::_MOD:
                b = pop(data_stack);
                a = pop(data_stack);
                if (b == 0) data_stack.push_back(a);
                else data_stack.push_back(a % b);
            break;
            /*case TokenType::_RET:
            // need to figure out how this works.
                a = pop(data_stack);
                std::cout << "Returned " << a << std::endl;
            break;*/
            case TokenType::_IDN:
                assert(false && "_IDN Not yet implemented...\n");
            break;
            case TokenType::_STR:
                assert(false && "_STR Not yet implemented...\n");
            /*// put characters onto stack in reverse and then put size of string
                for (int i = token.value.size() - 1; i >= 0; --i) {
                    data_stack.push_back(int(token.value[i]));
                }
                data_stack.push_back(int(token.value.size()));
            */break;
            case TokenType::_OUT:
                a = pop(data_stack);
                std::cout << a;
            break;
            case TokenType::_PUT:
                a = pop(data_stack);
                std::cout << char(a);
            break;
            case TokenType::_DMP:
                dumpStack(data_stack);
            break;
            case TokenType::_DUP:
                // a b c -> a b c c
                a = pop(data_stack);
                data_stack.push_back(a);
                data_stack.push_back(a);
            break;
            case TokenType::_DUP2:
                b = pop(data_stack);
                a = pop(data_stack);
                data_stack.push_back(a);
                data_stack.push_back(b);
                data_stack.push_back(a);
                data_stack.push_back(b);
            break;
            case TokenType::_ROT:
                c = pop(data_stack);
                b = pop(data_stack);
                a = pop(data_stack);
                data_stack.push_back(b);
                data_stack.push_back(c);
                data_stack.push_back(a);
            break;
            case TokenType::_SWP:
                b = pop(data_stack);
                a = pop(data_stack);
                data_stack.push_back(b);
                data_stack.push_back(a);
            break;
            case TokenType::_DROP:
                pop(data_stack);
            break;
            case TokenType::_IF:
                a = pop(data_stack);
                if (a) continue;
                else pc = size_t(token.value+1);
            break;
            case TokenType::_END:
                continue;
            break;
            case TokenType::_GR:
                b = pop(data_stack);
                a = pop(data_stack);
                data_stack.push_back(uint64_t(a > b));
            break;
            case TokenType::_GE:
                b = pop(data_stack);
                a = pop(data_stack);
                data_stack.push_back(uint64_t(a >= b));
            break;
            case TokenType::_EQ:
                b = pop(data_stack);
                a = pop(data_stack);
                data_stack.push_back(uint64_t(a == b));
            break;
            case TokenType::_LE:
                b = pop(data_stack);
                a = pop(data_stack);
                data_stack.push_back(uint64_t(a <= b));
            break;
            case TokenType::_LT:
                b = pop(data_stack);
                a = pop(data_stack);
                data_stack.push_back(uint64_t(a < b));
            break;
            case TokenType::_NT:
                b = pop(data_stack);
                a = pop(data_stack);
                data_stack.push_back(uint64_t(a != b));
            break;
            case TokenType::_EOF:
                return_val = pop(data_stack);
            break;
            case TokenType::_INV:
            default:
                std::cerr << "unreachable in interpret" << std::endl;
            break;
        }
    }
    return int(return_val);
}
