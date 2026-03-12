#include <iostream>
#include <sstream>
#include "Lexer.hpp"
#include "Parser.hpp"
#include "Evaluator.hpp"


#include "IO.h"
#include "Matrix.hpp"
#include "PrettyPrint.h"
#include <variant>

int main()
{
    const std::vector<std::string> infixEq = {
        "x = (100*100/2+10)/10",
        "[1 2 3 4; 1 2 3 4] * [1;2;3;4]",
        "sin ( +max ( 2, 3 )* max(1) * 3 )",
        "2 + 3 * 4",
        "abs(-sqrt(+min(+100, 200, 6, 29, max(5, 2, 4, 1))))",
        "cos(-pi)",
        "e^2*ln(e*2)",
        "log(10)",
        "sqrt(ln(10)*10)",
        "logbase(10, 2)",
        "abs(-3 + 1)",
        "sin(-90)",
        "10+---10",
        "max(1, 2,9 ,10)",
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
        "([1 2 3; 1 2 3; 1 2 3] * [1 2 3;1 2 3;1 2 3]) / 10",
        "([1, 2 3; 1, 2, 3; 1, 2, 3] * [1, 2, 3;1, 2, 3;1, 2, 3]) / 10",
        "[1 2 3; 1 2 3; 1 2 3]^1000000",
        "[1 -2 3; 2 1 -1; -1 3 2] \\ [7; 2; -3]",                              // x = [2; -1; 1]
        "[2 1; 5 3] \\ [8; 21]",                                                 // x = [3; 2]
        "[2 0 0; 0 3 0; 0 0 4] \\ [8; 9; 8]",                                   // x = [4; 3; 2]
        "[2 1 3; 4 5 6; 2 3 1] \\ [1; -1; 3]",                                  // x = [4; -1; -2]
        "[1 2 -1 3; 2 -1 3 1; -1 3 2 -2; 3 1 -2 4] \\ [7; 1; 5; 10]",          // x = [2.8; 2.6; -0.5; -0.5]
        "[10 2 3 1; 1 8 2 4; 2 1 9 3; 3 4 1 7] \\ [23; 33; 26; 24]",            // x = [1; 3; 2; 1]
        "[4 3 2 1; 3 4 3 2; 2 3 4 3; 1 2 3 4] \\ [10; 12; 12; 10]",             // x = [1; 1; 1; 1]
        "[9 1 2 3 1; 1 8 1 2 3; 2 1 7 1 2; 3 2 1 6 1; 1 3 2 1 5] \\ [16; 15; 13; 13; 12]", // x = [1; 1; 1; 1; 1]
        "[1 2 3; 4 5 6; 7 8 9] \\ [1; 2; 3]",                                   // throws: singular
        "[1 2 3; 4 5 6] \\ [1; 2]",                                               // throws: not square
        // x = [1; 2; 3]  —  A is diagonal-dominant, guaranteed non-singular
        "[2^2  cos(0)  sin(0); cos(0)  2^2  cos(0); sin(0)  cos(0)  2^2] \\ [2^2*1+cos(0)*2+sin(0)*3; cos(0)*1+2^2*2+cos(0)*3; sin(0)*1+cos(0)*2+2^2*3]",
        // resolves to: [4 1 0; 1 4 1; 0 1 4] \ [6; 12; 14]
        // x = [1; 2; 3] ✓

        // x = [2; 3; 1]
        "[2^2  sin(0)+1  cos(0); cos(0)  2^2  sin(0)+1; sin(0)+1  cos(0)  2^2] \\ [2^2*2+sin(0)+1*3+cos(0)*1; cos(0)*2+2^2*3+sin(0)+1*1; sin(0)+1*2+cos(0)*3+2^2*1]",

        // x = [1; 2; 1]
        "[sqrt(16)/5*5  cos(0)  sin(0); cos(0)  sqrt(16)/5*5  cos(0); sin(0)  cos(0)  sqrt(16)/5*5] \\ [sqrt(16)/5*5+cos(0)*2+sin(0); cos(0)+sqrt(16)/5*5*2+cos(0); sin(0)+cos(0)*2+sqrt(16)/5*5]",

        // x = [pi; 1; 1]
        "[cos(0)  sin(0)  sin(0); sin(0)  cos(0)  sin(0); sin(0)  sin(0)  cos(0)] \\ [cos(0)*pi+sin(0)+sin(0); sin(0)*pi+cos(0)+sin(0); sin(0)*pi+sin(0)+cos(0)]",

        // x = [2; 2; 2; 2]
        "[x  cos(0)  sin(0)  cos(0); cos(0)  2^2  cos(0)  sin(0); sin(0)  cos(0)  2^2  cos(0); cos(0)  sin(0)  cos(0)  2^2] \\ [2^2*2+cos(0)*2+sin(0)*2+cos(0)*2; cos(0)*2+2^2*2+cos(0)*2+sin(0)*2; sin(0)*2+cos(0)*2+2^2*2+cos(0)*2; cos(0)*2+sin(0)*2+cos(0)*2+2^2*2]",
        "1 \\ 10",
        "x * 100",
        "sin(x) * 100",
        "y = x * 100",
        "y * 1",
        "x = x + 100",
        "x * 1"
    };

    //PrettyPrint::print(std::vector(1, Token(MatrixT, m4 * m5)));

    //auto x = (Evaluator::Evaluate(Parser::ToPostfix(v)));

    // for (auto& i : v) {
    //     std::cout << i.toString() << " ";
    // }
    // std::cout << std::endl;
    //
    // std::visit([](auto&& a) {
    //     std::cout << a << std::endl;
    // }, x);

    // int count = 1;
    // std::unordered_map<std::string, Value> variables;
    // for (const auto & i : infixEq) {
    //     try {
    //         std::vector<Token> infixTokens = Lexer::Tokenize(i, variables);
    //         std::vector<Token> postfixTokens = Parser::ToPostfix(infixTokens);
    //         auto result = Evaluator::Evaluate(postfixTokens, variables);
    //         std::cout << "Equation: " << count++ << std::endl;
    //         std::cout << "============================================="<< std::endl;
    //         infixTokens.emplace_back(Assignment);
    //         infixTokens.emplace_back(result);
    //         PrettyPrint::print(infixTokens);
    //         std::cout << "============================================="<< std::endl;
    //     } catch (const std::exception& e) {
    //         std::cerr << e.what() << std::endl;
    //         std::cout << "============================================="<< std::endl;
    //     }
    // }



    std::string eq = "";
    std::vector<std::string> hist;

    bool run = false;
    int index = 0;

    while (run) {
        bool exec = false;

        while (!exec) {

            std::cout << "\033[2K\r>" << eq;

            const int c = io::getch();

            if (c == 10) { // Enter key
                exec = true;
                std::cout << std::endl; // Visual newline
                continue; // Skip appending '10' to the equation string
            }

            // Arrow Key Detection (Escape Sequence)
            if (c == 27) {
                int c2 = io::getch();
                if (c2 == '[') { // Confirm it's a control sequence
                    int c3 = io::getch();
                    if (c3 == 'A') {
                        // UP ARROW pressed
                        index = index - 1;
                        if (index < 0) index = hist.size() - 1;
                        eq = hist[index];
                    } else if (c3 == 'B') {
                        // DOWN ARROW pressed
                        index = (index + 1) % hist.size();
                        eq = hist[index];
                    } else if (c3 == 'C') {
                        // RIGHT ARROW pressed
                    } else if (c3 == 'D') {
                        // LEFT ARROW pressed
                    }
                }
                // Skip appending any part of the escape sequence to 'eq'
                continue;
            }

            // (Optional) Handle Backspace (usually 127 on Linux/macOS, 8 on Windows)
            if (c == 127 || c == 8) {
                if (!eq.empty()) {
                    eq.pop_back();
                    // Erase character from terminal: move back, print space, move back
                    std::cout << "\b \b";
                }
                continue;
            }

            // If it's a normal character, append it and print it
            eq += char(c);
            std::cout << char(c);
        }


        try {

            if (eq == "quit" || eq == "exit") {
                run = false;
                break;
            }

            //std::cout << Evaluator::Evaluate(Parser::ToPostfix(Lexer::Tokenize(eq))) << std::endl;
            hist.push_back(eq);
            eq = "";
            exec = true;
            index = hist.size();
        } catch (const std::exception& e) {
            eq.clear();
            std::cout << e.what() << std::endl;
        }
    }
    return 0;
}
