//
// Created by kerem on 2/28/26.
//

#include "Lexer.hpp"
#include <cctype>



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
            case '%':
                tokens.emplace_back(Percent);
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
            case ',':
                tokens.emplace_back(Comma);
                break;
            case '!':
                tokens.emplace_back(Fact);
                break;
            case '[':
                cursor++;
                tokens.emplace_back(tokenizeMatrix());
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
    return Token(Number, std::stod(std::string(raw)));
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
            return Token(PI, M_PI);
        if (type == Euler)
            return Token(Euler, M_E);
        return Token(type);
    } catch (const std::out_of_range& e) {
        throw std::out_of_range("Invalid identifier: " + std::string(raw));
    }
}

Token Lexer::tokenizeMatrix() {
    std::vector<std::vector<double>> data = std::vector<std::vector<double>>(1);
    if (input[cursor] == ']') throw std::invalid_argument("Empty matrix!");
    size_t row = 0;
    size_t col = 0;
    const size_t start = cursor;
    while (cursor < input.length() && input[cursor] != ']') {
        if (input[cursor] == ';') {
            if (cursor == start) throw std::invalid_argument("First row empty!");
            data.emplace_back();
            row++;
            cursor++;
            continue;
        }
        if (input[cursor] == ',' || input[cursor] == ' ') {
            cursor++;
            continue;
        }
        if (std::isdigit(input[cursor]) || input[cursor] == '.')
            data[row].push_back(std::get<double>(tokenizeNumber().data));
        else
            throw std::invalid_argument("Invalid matrix data!");
        if (!row) col++;
    }
    if (cursor >= input.length()) throw std::invalid_argument("Unclosed matrix!");
    Matrix<double> m(row + 1, col);
    m.SetData(data);
    return Token(MatrixT, m);
}
