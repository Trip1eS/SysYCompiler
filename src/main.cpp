#include <iostream>
#include <fstream>
#include "Lexer.hpp"
#include "Token.hpp"
#include "Parser.hpp"

int main(int, char **) {
    Lexer lexer;
    auto [tokens, errors] = lexer.lex("../../samples/sysy.sy");
    Parser parser(std::move(tokens));
    parser.parse();

    return 0;
}
