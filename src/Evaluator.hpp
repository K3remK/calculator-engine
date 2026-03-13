//
// Created by kerem on 2/28/26.
//

#ifndef CALCULATOR_BACKEND_EVALUATOR_H
#define CALCULATOR_BACKEND_EVALUATOR_H

#include "Token.hpp"

class Evaluator {
public:
    static Token Evaluate(const std::vector<Token>& postfixTokens, std::unordered_map<std::string_view, Token>& variables);
private:

    template<class ... Ts> struct overloaded : Ts... { using Ts::operator()...; };
    template<class ... Ts> overloaded(Ts...) -> overloaded<Ts...>;

    static void validate(const Token& token);
    static Token evalBinary(TokenType op, const Token& left, const Token& right);
    static Token evalUnary(TokenType op, const Token& operand);
    static Token evalVariadic(TokenType op, const std::vector<Token>& operands);
    static Token evaluateMod(const Token& left, const Token& right);
    static Token factorial(const Token& operand);
    static Token minVariadic(const std::vector<Token>& operands);
    static Token maxVariadic(const std::vector<Token>& operands);
};


#endif //CALCULATOR_BACKEND_EVALUATOR_H