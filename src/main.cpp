#include <iostream>
#include <sstream>
#include "Lexer.hpp"
#include "Parser.hpp"
#include "Evaluator.hpp"


#include "IO.h"
#include "Matrix.h"

int main()
{
    const std::vector<std::string> infixEq = {
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
    };

    Matrix<double> m(3, 3, 1);
    const std::vector<std::vector<double>> data = {
        {1, 2, 3},
        {1, 10, 3},
        {1, 2, 3}
    };
    m.SetData(data);

    Matrix<double> m2(3, 3, 1);
    const std::vector<std::vector<double>> data2 = {
        {1, 2, 3},
        {1, 2, 3},
        {1, 2, 3}
    };
    m2.SetData(data2);

    Matrix<double> m3(3, 1, 1);
    const std::vector<std::vector<double>> data3 = {
        {1},
        {2},
        {3}
    };
    m3.SetData(data3);

    Matrix<double> t = m * m3;
    t = m.Transpose();
    t = m.Identity();

    const std::vector v = { Token(MatrixT, m), Token(Pow), Token(Number, 2.0)};

    auto x = (Evaluator::Evaluate(Parser::ToPostfix(v)));

    for (auto& i : v) {
        std::cout << i.toString() << " ";
    }
    std::cout << std::endl;

    std::visit([](auto&& a) {
        std::cout << a << std::endl;
    }, x);

    try {
        for (const auto & i : infixEq) {
            std::vector<Token> tokens = Parser::ToPostfix(Lexer::Tokenize(i));
            std::cout << i << " -> ";
            for (auto& t : tokens) {
                std::cout << t << " ";
            }
            std::cout << " = " << std::setprecision(10);
            std::visit([](auto&& a){ std::cout << a; }, Evaluator::Evaluate(tokens));
            std::cout << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    std::string eq = "";
    std::vector<std::string> hist;

    bool run = false;
    size_t index = 0;

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