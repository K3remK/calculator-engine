//
// Created by kerem on 2/28/26.
//

#include "Lexer.hpp"
#include <cctype>
#include <cmath>

#include "Evaluator.hpp"
#include "Parser.hpp"


std::vector<Token> Lexer::Tokenize(const std::string_view input_, std::unordered_map<std::string, Value> &variables) {
    Lexer lexer(input_);
    return lexer.tokenizeCore(variables);
}

Lexer::Lexer(const std::string_view& input)
    : input(input), cursor(0)
{
}

std::vector<Token> Lexer::tokenizeCore(std::unordered_map<std::string, Value> &variables) {
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
                tokens.emplace_back(FuncOpToken(Add));
                break;
            case '-':
                tokens.emplace_back(FuncOpToken(Sub));
                break;
            case '*':
                tokens.emplace_back(FuncOpToken(Mul));
                break;
            case '%':
                tokens.emplace_back(FuncOpToken(Percent));
                break;
            case '/':
                tokens.emplace_back(FuncOpToken(Div));
                break;
            case '^':
                tokens.emplace_back(FuncOpToken(Pow));
                break;
            case '(':
                tokens.emplace_back(FuncOpToken(LeftParen));
                break;
            case ')':
                tokens.emplace_back(FuncOpToken(RightParen));
                break;
            case ',':
                tokens.emplace_back(FuncOpToken(Comma));
                break;
            case '!':
                tokens.emplace_back(FuncOpToken(Fact));
                break;
            case '\\':
                tokens.emplace_back(FuncOpToken(InvMul));
                break;
            case '[':
                cursor++;
                tokens.emplace_back(tokenizeRawMatrix(variables));
                break;
            case '=':
                tokens.emplace_back(FuncOpToken(Assignment));
                break;
            default:
                //* Unknown symbol
                throw std::invalid_argument("Invalid symbol: " + std::string(1, currentChar));
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
    return NumberToken(std::stod(std::string(raw)));
}

Token Lexer::tokenizeIdentifier() {
    const std::size_t start = cursor;

    while (cursor < input.length() && std::isalnum(input[cursor])) {
        cursor++;
    }

    const std::string_view raw = input.substr(start, cursor - start);
    try {
        const auto type = typeMapper.at(std::string(raw));
        if (type == PI)
            return NumberToken(M_PI);
        if (type == Euler)
            return NumberToken(M_E);
        return FuncOpToken(type);
    } catch (const std::out_of_range& e) {
        return VarToken(raw);    // Variable token type
        /* throw std::out_of_range("Invalid identifier: " + std::string(raw)); */
    }
}

Token Lexer::tokenizeRawMatrix(std::unordered_map<std::string, Value> &variables) {
    auto data = std::vector<std::vector<std::string_view>>();
    if (input[cursor] == ']') throw std::invalid_argument("Empty matrix!");
    size_t row = 0;
    size_t col = 0;
    size_t start = cursor;
    while (cursor < input.length() && input[cursor] != ']') {
        if (input[cursor] == ';') {
            if (start == cursor || input[start] == ';') throw std::invalid_argument("Empty matrix!");
            auto rowEntry = input.substr(start, cursor - start);
            //! HACK : FIX THIS BY SEPERATING THIS PROCESS INTO SOME SUBPARSER
            data[row].emplace_back(rowEntry);
            cursor++;
            data.emplace_back();
            start = cursor;
            row++;
            continue;
        }
        if (input[cursor] == ',' || input[cursor] == ' ') {
            if (cursor == start && input[cursor] != ' ') throw std::invalid_argument("Empty matrix!");
            auto s = input.substr(start, cursor - start);
            if (s == " " || s.empty()) {
                cursor++;
                continue;
            }
            data[row].emplace_back(s);
            if (row == 0) col++;
            cursor++;
            start = cursor;
        }
        cursor++;
    }
    if (cursor > input.size()) throw std::invalid_argument("Unclosed matrix!");
    if (input[cursor] != ']') throw std::invalid_argument("Unclosed matrix!");
    auto rowEntry = input.substr(start, cursor - start);
    data[row].emplace_back(rowEntry);
    return RawMatrixToken(std::make_unique<Matrix<std::string_view>>(Matrix(data)));
}
