//
// Created by kerem on 2/28/26.
//

#include "Lexer.h"
#include <cctype>
#include "Util.hpp"

std::vector<Token> Lexer::Tokenize(const std::string_view input_) {
    Lexer lexer(input_);
    return lexer.tokenizeCore();
}

Lexer::Lexer(const std::string_view& input)
    : input(input), cursor(0)
{
}

std::vector<Token> Lexer::tokenizeCore() {
    std::vector<Token> tokens;

    while (cursor < input.length()) {
        const char currentChar = input[cursor];

        if (std::isspace(currentChar)) {
            cursor++;
            continue;
        }

        if (std::isdigit(currentChar)) {
            tokens.emplace_back(tokenizeNumber());
            continue;
        }

        if (std::isalpha(currentChar)) {
            tokens.emplace_back(tokenizeIdentifier());
            continue;
        }

        switch (currentChar) {
            case '+':
                tokens.emplace_back(Add);
                break;
            case '-':
                tokens.emplace_back(Sub);
                break;
            case '*':
                tokens.emplace_back(Mul);
                break;
            case '/':
                tokens.emplace_back(Div);
                break;
            case '^':
                tokens.emplace_back(Pow);
                break;
            case '(':
                tokens.emplace_back(LeftParen);
                break;
            case ')':
                tokens.emplace_back(RightParen);
                break;
            default:
                // ! if we are here it means that we have a symbol that is not defined yet
                // ! throw a runtime error to be caught in the global exception handler
                break;
        }
        cursor++;
    }
    return tokens;
}

Token Lexer::tokenizeNumber() {

    const std::size_t start = cursor;
    while (cursor < input.length() && (std::isdigit(input[cursor]) || input[cursor] == '.')) {
        cursor++;
    }

    const std::string_view raw = input.substr(start, cursor - start);

    return Token(Number, std::stod(std::string(raw)));
}

Token Lexer::tokenizeIdentifier() {
    const std::size_t start = cursor;

    while (cursor < input.length() && std::isalnum(input[cursor])) {
        cursor++;
    }

    const std::string_view raw = input.substr(start, cursor - start);

    return Token(typeMapper.at(raw));
}
