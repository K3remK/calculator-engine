#include <iostream>
#include <sstream>
#include "Lexer.hpp"
#include "Util.hpp"
#include "Parser.hpp"
#include "Evaluator.hpp"


int main()
{
    const std::vector<std::string> infixEq = {
        "(80*20%)!",
        "(4!+10)!",
        "sin(5)+100",
        "3*pi/2",
        "100 mod 13",
        "min(100)",
        "max(1)",
        "2^2^(2*sin(20))",
        "sqrt(min(100, 200, 6, 29, max(5, 2, 4, 1)))",
        "sin ( max ( 2, 3 ) / 0 * 3 )",
        "2 + 3 * 4",
        "10 - 2 * 3 + 4",
        "10 / 2 * 5",
        "(2 + 3) * 4 mod 7",
        "10 / (2 * 5)",
        "((1 + 2) * 3) - 4",
        "( 31 / 0 ) * 1023",
        "sqrt(16) / 5",
        "sin(0) + cos(0)",
        "sqrt(3^2 + 4^2)",
    };


    try {
        for (const auto & i : infixEq) {
            std::vector<Token> tokens = Parser::ToPostfix(Lexer::Tokenize(i));
            std::cout << i << " -> ";
            for (auto& t : tokens) {
                std::cout << t << " ";
            }
            std::cout << " = " << Evaluator::Evaluate(tokens) << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}