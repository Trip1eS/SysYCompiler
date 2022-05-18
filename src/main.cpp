#include <iostream>
#include <fstream>
#include <string>
#include "Lexer.hpp"
#include "Token.hpp"
#include "Parser.hpp"
#include "IrGenerator.hpp"

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "error: no input file\n";
        return 1;
    }
    std::string inFilePath(argv[1]);
    std::string outFilePath;
    if (argc >= 3 && strcmp(argv[2], "-o") == 0) {
        if (argc == 4) {
            outFilePath = argv[3];
        } else {
            std::cerr << "error: no output file\n";
            return 1;
        }
    } else {
        outFilePath = "out.s";
    }

    Lexer lexer;
    auto [tokens, errors] = lexer.lex(inFilePath);
    Parser parser(std::move(tokens));
    parser.parse();
    IrGenerator irGen(std::move(parser.getCompUnits()));
    irGen.codegen();
    irGen.output(outFilePath, llvm::CGFT_AssemblyFile);

    return 0;
}
