//
// Created by kerem on 2/28/26.
//

#ifndef CALCULATOR_BACKEND_EVALUATOR_H
#define CALCULATOR_BACKEND_EVALUATOR_H

#include "Token.hpp"

class Evaluator {
public:
    static Value Evaluate(const std::vector<Token>& postfixTokens);
private:

    template<class ... Ts> struct overloaded : Ts... { using Ts::operator()...; };
    template<class ... Ts> overloaded(Ts...) -> overloaded<Ts...>;

    static void validate(const Token& token);
    static Value evalBinary(TokenType op, const Value& left, const Value& right);
    static Value evalUnary(TokenType op, const Value& operand);
    static Value evalVariadic(TokenType op, const std::vector<Value>& operands);
    static Value evaluateMod(const Value& left, const Value& right);
    static Value factorial(const Value& operand);
    static Value minVariadic(const std::vector<Value>& operands);
    static Value maxVariadic(const std::vector<Value>& operands);
};


#endif //CALCULATOR_BACKEND_EVALUATOR_H