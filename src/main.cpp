#include <iostream>
#include <sstream>
#include "Lexer.hpp"
#include "Util.hpp"
#include "Parser.hpp"
#include "Evaluator.hpp"


int main()
{
    const std::vector<std::string> infixEq = {
        "e^2*ln(e*2)",
        "log(10)",
        "sqrt(ln(10)*10)",
        "logbase(10, 2)",
        "cos(-pi)",
        "abs(-3 + 1)",
        "sin(-90)",
        "10+---10",
        "abs(-sqrt(+min(+100, 200, 6, 29, max(5, 2, 4, 1))))",
        "max(1, 2,9 ,10)",
        "sin ( +max ( 2, 3 )* max(1) * 3 )",
        "2 + 3 * 4",
        "sin(1)",
        "(80*20%)!",
        "(4!+10)!",
        "sin(5)+100",
        "3*pi/2",
        "100 mod 13",
        "min(100)",
        "max(1)",
        "2^2^(2*sin(20))",
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


    // try {
    //     for (const auto & i : infixEq) {
    //         std::vector<Token> tokens = Parser::ToPostfix(Lexer::Tokenize(i));
    //         std::cout << i << " -> ";
    //         for (auto& t : tokens) {
    //             std::cout << t << " ";
    //         }
    //         std::cout << " = " << std::setprecision(10) << Evaluator::Evaluate(tokens) << std::endl;
    //     }
    // } catch (const std::exception& e) {
    //     std::cerr << e.what() << std::endl;
    // }

    std::string eq;

    while (true) {
        std::cout << ">";
        std::getline(std::cin, eq);
        if (eq == "exit" || eq == "q") {
            break;
        }
        try {
            std::cout << Evaluator::Evaluate(Parser::ToPostfix(Lexer::Tokenize(eq))) << std::endl;
        } catch (const std::exception& e) {
            std::cout << e.what() << std::endl;
        }
    }
    return 0;
}