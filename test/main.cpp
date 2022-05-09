#include <iostream>
#include <fstream>
#include "Lexer.hpp"
#include "Token.hpp"
#include "Parser.hpp"

int main(int, char **) {
    Lexer lexer;
    auto [tokens, errors] = lexer.lex("../../test/sysy/parser_error.sy");
    Parser parser(std::move(tokens));
    parser.parse();
    if (!parser.hasError()) parser.outputAst("ast.txt");

    return 0;
}
