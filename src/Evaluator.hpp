//
// Created by kerem on 2/28/26.
//

#ifndef CALCULATOR_BACKEND_EVALUATOR_H
#define CALCULATOR_BACKEND_EVALUATOR_H

#include "Token.hpp"



// ─── Token Variant Helpers ──────────────────────────────────────────────────
// Every variant alternative (NumberToken, MatrixToken, RawMatrixToken,
// VarToken, FuncOpToken) carries a `.type` field of type TokenType.
// These helpers let us query it without verbose std::visit boilerplate.

/// Extract the TokenType stored inside any Token variant alternative.
TokenType GetTokenType(const Token& token);

/// Return true if the token's type has any bit in common with `mask`.
bool TokenTypeMatches(const Token& token, uint64_t mask);

/// Retrieve the OperatorInfo for a token that is an operator or function.
/// Throws std::invalid_argument if the token's type is not in operatorMap.
const OperatorInfo& GetOperatorInfoFromToken(const Token& token);


class Evaluator {
public:
    static Token Evaluate(std::vector<Token>& postfixTokens, std::unordered_map<std::string_view, Value> &variable_table);
private:

    template<class ... Ts> struct overloaded : Ts... { using Ts::operator()...; };
    template<class ... Ts> overloaded(Ts...) -> overloaded<Ts...>;

    static void validate(const std::vector<Token>& token);
    static Token evalBinary(TokenType op, const Token& left, const Token& right);
    static Token evalUnary(TokenType op, const Token& operand);
    static Token evalVariadic(TokenType op, const std::vector<double>& operands);
    static Token evaluateMod(const Token& left, const Token& right);
    static double factorial(const Token& operand);
    static Token minVariadic(std::vector<double>& operands);
    static Token maxVariadic(std::vector<double>& operands);
};


#endif //CALCULATOR_BACKEND_EVALUATOR_H