//
// Created by kerem on 2/28/26.
//

#include "Lexer.hpp"
#include <cctype>
#include <cmath>

#include "Evaluator.hpp"
#include "Parser.hpp"


std::vector<Token> Lexer::Tokenize(const std::string_view input_, std::unordered_map<std::string, Token>& variables) {
    Lexer lexer(input_);
    return lexer.tokenizeCore(variables);
}

Lexer::Lexer(const std::string_view& input)
    : input(input), cursor(0)
{
}

std::vector<Token> Lexer::tokenizeCore(std::unordered_map<std::string, Token> &variables) {
    std::vector<Token> tokens;

    while (cursor < input.length()) {
        const char currentChar = input[cursor];

        if (std::isspace(currentChar)) {
            cursor++;
            continue;
        }

        if (std::isdigit(currentChar)) {
            tokens.push_back(std::move(tokenizeNumber()));
            continue;
        }

        if (std::isalpha(currentChar)) {
            tokens.push_back(std::move(tokenizeIdentifier()));
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
            case '\\':
                tokens.emplace_back(InvMul);
                break;
            case '[':
                cursor++;
                tokens.push_back(std::move(tokenizeMatrix(variables)));
                break;
            case '=':
                tokens.emplace_back(Assignment);
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
        return Token(std::string(raw));
        // throw std::out_of_range("Invalid identifier: " + std::string(raw));
    }
}

Token Lexer::tokenizeMatrix(std::unordered_map<std::string, Token>& variables) {
    auto data = std::vector<std::vector<double>>(1);
    if (input[cursor] == ']') throw std::invalid_argument("Empty matrix!");
    size_t row = 0;
    size_t col = 0;
    size_t start = cursor;
    while (cursor < input.length() && input[cursor] != ']') {
        //! REWRITE THE WHOLE LOGIC SO THAT IT WILL ACCEPT EACH [ROW, COL] ENTRY AS A SEPERATE EQUATION
        if (input[cursor] == ';') {
            if (start == cursor || input[start] == ';') throw std::invalid_argument("Empty matrix!");
            auto tokens = Tokenize(input.substr(start, cursor - start), variables);
            data[row].push_back(std::get<double>(Evaluator::Evaluate(Parser::ToPostfix(tokens), variables).data));
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
            auto tokens = Tokenize(s, variables);
            data[row].push_back(std::get<double>(Evaluator::Evaluate(Parser::ToPostfix(tokens), variables).data));
            if (row == 0) col++;
            cursor++;
            start = cursor;
        }
        cursor++;
    }
    if (cursor > input.size()) throw std::invalid_argument("Unclosed matrix!");
    if (input[cursor] != ']') throw std::invalid_argument("Unclosed matrix!");
    auto tokens = Tokenize(input.substr(start, cursor - start), variables);
    data[row].push_back(std::get<double>(Evaluator::Evaluate(Parser::ToPostfix(tokens), variables).data));
    return Token(MatrixT, std::make_unique<Matrix<double>>(Matrix(data)));
}

