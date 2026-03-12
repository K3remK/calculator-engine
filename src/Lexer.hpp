//
// Created by kerem on 2/28/26.
//

#ifndef CALCULATOR_BACKEND_LEXER_H
#define CALCULATOR_BACKEND_LEXER_H

#include <string_view>
#include <vector>
#include "Token.hpp"



inline static const std::unordered_map<std::string_view, TokenType> typeMapper = {
    {"+", Add},
    {"-", Sub},
    {"*", Mul},
    {"/", Div},
    {"^", Pow},
    {"\\", InvMul},
    {"min", Min},
    {"max", Max},
    {"sin", Sin},
    {"cos", Cos},
    {"tan", Tan},
    {"cot", Cot},
    {"sqrt", Sqrt},
    {"mod", Mod},
    {"pi", PI},
    {",", Comma},
    {"log", Log},
    {"ln", Ln},
    {"logbase", LogBase},
    {"abs", Abs},
    {"e", Euler}
};

class Lexer {
public:
    static std::vector<Token> Tokenize(std::string_view);
private:
    std::string_view input;
    std::size_t cursor;

    explicit Lexer(const std::string_view& input);

    std::vector<Token> tokenizeCore();
    Token tokenizeNumber();
    Token tokenizeIdentifier();
    Token tokenizeMatrix();
};


#endif //CALCULATOR_BACKEND_LEXER_H