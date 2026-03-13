//
// Created by kerem on 2/28/26.
//

#ifndef CALCULATOR_BACKEND_PARSER_H
#define CALCULATOR_BACKEND_PARSER_H

#include "Token.hpp"


class Parser {
public:
    static std::vector<Token> ToPostfix(std::vector<Token>& infixTokens);
    static void Validate(const std::vector<Token>& infixTokens);
private:
    static void resolveUnaryOperators(std::vector<Token>& infixTokens);
    static std::vector<Token> shuntingYard(const std::vector<Token>& infixTokens);
};


#endif //CALCULATOR_BACKEND_PARSER_H