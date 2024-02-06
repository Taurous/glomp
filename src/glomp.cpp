#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

#include "lexer.hpp"
#include "interpreter.hpp"
#include "compiler.hpp"

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
            return_val = interpret(tokens);
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
