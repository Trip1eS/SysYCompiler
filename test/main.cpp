#include <iostream>
#include <fstream>
#include "Lexer.hpp"
#include "Token.hpp"
#include "Parser.hpp"
#include "IrGenerator.hpp"

int main(int, char **) {
    Lexer lexer;
    auto tokens = lexer.lex("../../test/sysy/sysy.sy");
    if (lexer.hasError()) return 1;
    Parser parser(std::move(tokens));
    parser.parse();
    if (!parser.hasError()) parser.outputAst("ast.txt");

    return 0;
}
