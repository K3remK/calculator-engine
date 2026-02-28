#include <iostream>
#include <sstream>
#include "Lexer.hpp"
#include "Util.hpp"
#include "Parser.hpp"


int main()
{
    std::vector<std::string> infixEq = {
        "2 + 3 * 4",
        "10 - 2 * 3 + 4",
        "10 / 2 * 5",
        "(2 + 3) * 4",
        "10 / (2 * 5)",
        "((1 + 2) * 3) - 4",
        "( 31 / 5 ) * 1023",
        "sqrt(16) + 5",
        "sin(0) + cos(0)",
        "sqrt(3^2 + 4^2)",

    };

    for (int i = 0; i < infixEq.size(); i++) {
        std::vector<Token> tokens = Parser::ToPostfix(Lexer::Tokenize(infixEq[i]));

        for (auto& t : tokens) {
            std::cout << t << " ";
        }
        std::cout << std::endl;
    }
    return 0;
}