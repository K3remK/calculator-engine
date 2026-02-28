//
// Created by kerem on 2/28/26.
//

#ifndef CALCULATOR_BACKEND_PARSER_H
#define CALCULATOR_BACKEND_PARSER_H

#include "Token.hpp"


class Parser {
    public:
        static std::vector<Token> ToPostfix(const std::vector<Token>& infixTokens);
};


#endif //CALCULATOR_BACKEND_PARSER_H