#include <iostream>
#include <fstream>
#include <string>
#include "Lexer.hpp"
#include "Token.hpp"
#include "Parser.hpp"
#include "IrGenerator.hpp"
#include "Logger.hpp"

enum Target {
    TOKENS,
    AST,
    IR,
    ASM
};

int main(int argc, char** argv) {
    Target target;
    if (argc < 4) {
        err() << "invalid arguments\n";
        return 1;
    }

    if (strcmp(argv[1], "-l") == 0) {
        target = TOKENS;
    } else if (strcmp(argv[1], "-p") == 0) {
        target = AST;
    } else if (strcmp(argv[1], "-i") == 0) {
        target = IR;
    } else if (strcmp(argv[1], "-s") == 0) {
        target = ASM;
    } else {
        err() << "no target\n";
        return 1;
    }

    std::string inFilePath(argv[2]);
    std::string outFilePath(argv[4]);

    Lexer lexer(inFilePath);
    lexer.lex();
    if (lexer.hasError()) return 1;
    if (target == TOKENS) {
        lexer.outputTokens(outFilePath);
        return 0;
    }
    Parser parser(std::move(lexer.getTokens()));
    parser.parse();
    if (parser.hasError()) return 1;
    if (target == AST) {
        parser.outputAst(outFilePath);
        return 0;
    }
    IrGenerator irGen(std::move(parser.getCompUnits()));
    irGen.codegen();
    if (target == IR) {
        irGen.printModule(outFilePath);
        return 0;
    }
    if (target == ASM) {
        irGen.output(outFilePath, llvm::CGFT_AssemblyFile);
    }
    return 0;
}
