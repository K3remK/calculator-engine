#include <iostream>
#include <sstream>
#include "Lexer.hpp"
#include "Util.hpp"
#include "Parser.hpp"
#include "Evaluator.hpp"


int main()
{
    std::vector<std::string> infixEq = {
        "2^2^(2*sin(20))",
        "min(1, 2, 3, 4, 5)",
        "sqrt(min(100, 200, 6, 29, max(5, 2, 4, 1)))",
        "sin ( max ( 2, 3 ) / 3 * 3 )",
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
        std::cout << infixEq[i] << " -> ";
        for (auto& t : tokens) {
            std::cout << t << " ";
        }
        std::cout << " = " << Evaluator::Evaluate(tokens) << std::endl;
    }
    return 0;
}