#include <iostream>
#include <sstream>
#include "Lexer.hpp"
#include "Parser.hpp"
#include "Evaluator.hpp"


#include "IO.h"
#include "Matrix.hpp"
#include "PrettyPrint.h"
#include <variant>
#include <chrono>

int main()
{
    std::string eq;
    std::vector<std::string> hist;
    std::unordered_map<std::string, Token> variables;

    bool run = true;
    int index = 0;

    while (run) {
        bool exec = false;

        int cursorPos = 0; // Track cursor relative to the string start

        while (!exec) {
            // 1. Move cursor to the start of the line, clear it, and reprint string
            // Then move the cursor back to the correct logical position
            std::cout << "\r\033[K>" << eq;
            if (cursorPos < eq.length()) {
                std::cout << "\033[" << (eq.length() - cursorPos) << "D";
            }

            const int c = io::getch();

            if (c == 10) { // Enter
                exec = true;
                std::cout << std::endl;
                continue;
            }

            // Escape Sequences (Arrows)
            if (c == 27) {
                if (io::getch() == '[') {
                    int c3 = io::getch();
                    if (c3 == 'A') { // UP
                        if (!hist.empty()) {
                            index = (index - 1 + hist.size()) % hist.size();
                            eq = hist[index];
                            cursorPos = eq.length();
                        }
                    } else if (c3 == 'B') { // DOWN
                        if (!hist.empty()) {
                            index = (index + 1) % hist.size();
                            eq = hist[index];
                            cursorPos = eq.length();
                        }
                    } else if (c3 == 'C') { // RIGHT
                        if (cursorPos < eq.length()) cursorPos++;
                    } else if (c3 == 'D') { // LEFT
                        if (cursorPos > 0) cursorPos--;
                    }
                }
                continue;
            }

            // Backspace
            if (c == 127 || c == 8) {
                if (cursorPos > 0) {
                    eq.erase(cursorPos - 1, 1);
                    cursorPos--;
                }
                continue;
            }

            // Normal Character Insertion
            std::string s(1, (char)c);
            eq.insert(cursorPos, s);
            cursorPos++;
        }


        try {

            if (eq == "quit" || eq == "exit") {
                run = false;
                break;
            }
            auto infixTokens = Lexer::Tokenize(eq, variables);
            auto postfixTokens = Parser::ToPostfix(infixTokens);
            auto result = Evaluator::Evaluate(postfixTokens, variables);
            PrettyPrint::print(result);
            hist.push_back(eq);
            eq = "";
            exec = true;
            index = hist.size();
        } catch (const std::exception& e) {
            hist.push_back(eq);
            eq.clear();
            std::cout << e.what() << std::endl;
        }
    }
    return 0;
}

