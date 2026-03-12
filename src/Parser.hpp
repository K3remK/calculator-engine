//
// Created by kerem on 2/28/26.
//

#ifndef CALCULATOR_BACKEND_PARSER_H
#define CALCULATOR_BACKEND_PARSER_H

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

// ─── Parser ─────────────────────────────────────────────────────────────────

class Parser {
public:
    /// Full pipeline: Validate → resolve unary operators → Shunting-Yard.
    static std::vector<Token> ToPostfix(std::vector<Token>& infixTokens);

    /// Standalone validation — throws std::runtime_error on structural errors.
    /// Can be called independently without converting.
    static void Validate(const std::vector<Token>& infixTokens);

private:
    /// Rewrite Sub/Add tokens that act as unary minus/plus into
    /// UnaryMinus/UnaryPlus FuncOpTokens.  Modifies the vector in place.
    static void resolveUnaryOperators(std::vector<Token>& infixTokens);

    /// Pure Shunting-Yard conversion.  Assumes the input is already validated
    /// and unary operators have been resolved.
    static std::vector<Token> shuntingYard(std::vector<Token>& infixTokens);
};

#endif //CALCULATOR_BACKEND_PARSER_H