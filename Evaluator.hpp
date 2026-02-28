//
// Created by kerem on 2/28/26.
//

#ifndef CALCULATOR_BACKEND_EVALUATOR_H
#define CALCULATOR_BACKEND_EVALUATOR_H

#include "Token.hpp"

class Evaluator {
public:
    static double Evaluate(const std::vector<Token>& postfixTokens);
};


#endif //CALCULATOR_BACKEND_EVALUATOR_H