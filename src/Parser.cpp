//
// Created by kerem on 2/28/26.
//

#include "Parser.hpp"
#include <queue>
#include <stack>
#include <format>

TokenType GetTokenType(const Token& token) {
    return std::visit([](const auto& t) -> TokenType {
        return t.type;
    }, token);
}

bool TokenTypeMatches(const Token& token, uint64_t mask) {
    return GetTokenType(token) & mask;
}

const OperatorInfo& GetOperatorInfoFromToken(const Token& token) {
    return operatorMap.at(GetTokenType(token));
}

void Parser::Validate(const std::vector<Token>& infixTokens) {
    if (infixTokens.empty())
        throw std::runtime_error("Empty expression");

    //Parenthesis balance
    int parenDepth = 0;
    for (const auto& token : infixTokens) {
        const TokenType type = GetTokenType(token);
        if (type == LeftParen) ++parenDepth;
        else if (type == RightParen) {
            --parenDepth;
            if (parenDepth < 0)
                throw std::runtime_error("Mismatched parentheses: unexpected ')'");
        }
    }
    if (parenDepth != 0)
        throw std::runtime_error("Mismatched parentheses");

    // First token checks
    {
        const TokenType first = GetTokenType(infixTokens.front());
        // Leading infix operator that is NOT +/- (those become unary)
        // and NOT a function or paren or value
        if ((first & InfixOperators) && !(first & (Add | Sub | Assignment)))
            throw std::runtime_error("Expression starts with invalid operator");
    }

    //Last token checks
    {
        const TokenType last = GetTokenType(infixTokens.back());
        if (last & InfixOperators)
            throw std::runtime_error("Expression ends with an operator");
        if (last == LeftParen)
            throw std::runtime_error("Expression ends with '('");
        if (last == Comma)
            throw std::runtime_error("Expression ends with ','");
    }

    //Adjacency checks
    for (size_t i = 1; i < infixTokens.size(); ++i) {
        const TokenType prev = GetTokenType(infixTokens[i - 1]);
        const TokenType curr = GetTokenType(infixTokens[i]);

        // Two consecutive value-like tokens without an operator between them
        // "2 3", "pi 5", "[matrix] pi"
        // Postfix operators are value-like on their left side, so "5! 3" is also caught
        const bool prevIsValue = (prev & Numbers) || (prev & PostfixOperators) || (prev == RightParen);
        const bool currIsValue = (curr & Numbers) || (curr == LeftParen);
        const bool currIsFunc  = curr & MathFunctions;

        if (prevIsValue && (currIsValue || currIsFunc))
            throw std::runtime_error("Invalid format: two consecutive values without an operator");

        // Operator right after '(' for example: "(* 3)"
        // +/- after '(' is allowed (unary), so exclude those
        if (prev == LeftParen && (curr & InfixOperators) && !(curr & (Add | Sub)))
            throw std::runtime_error("Operator immediately after '('");

        // ')' immediately after '(' — empty parentheses
        if (prev == LeftParen && curr == RightParen)
            throw std::runtime_error("Empty parentheses");

        // Comma not inside a function call context — basic check:
        // comma after '(' is fine, comma after a value is fine,
        // but comma after comma or comma after an infix operator is wrong
        if (curr == Comma && (prev == Comma || ((prev & InfixOperators) && !(prev & (Add | Sub)))))
            throw std::runtime_error("Invalid comma placement");
    }
}

// Resolve Unary Operators

void Parser::resolveUnaryOperators(std::vector<Token>& infixTokens) {
    for (size_t i = 0; i < infixTokens.size(); ++i) {
        const TokenType type = GetTokenType(infixTokens[i]);

        // Only Sub and Add can be unary
        if (type != Sub && type != Add) continue;

        // It is unary if:
        //  - it is the first token, OR
        //  - the previous token is '(', ',', another operator, or a unary op
        bool isUnary = false;
        if (i == 0) {
            isUnary = true;
        } else {
            const TokenType prev = GetTokenType(infixTokens[i - 1]);
            // After (, comma, or any infix/unary operator → unary context
            isUnary = (prev == LeftParen)
                    || (prev == Comma)
                    || (prev & InfixOperators)
                    || (prev == UnaryMinus)
                    || (prev == UnaryPlus);
        }

        if (isUnary) {
            const TokenType newType = (type == Sub) ? UnaryMinus : UnaryPlus;
            infixTokens[i] = FuncOpToken(newType);
        }
    }
}

//Shunting-Yard

std::vector<Token> Parser::shuntingYard(std::vector<Token>& infixTokens) {
    std::vector<Token>  output;
    std::stack<Token>    opStack;
    std::stack<size_t>   argcStack;   // tracks argument count per function

    for (size_t i = 0; i < infixTokens.size(); ++i) {
        auto& token = infixTokens[i];
        const TokenType type = GetTokenType(token);

        //Values: Number, PI, Euler, Matrix, RawMatrix, Var
        // Postfix operators (!, %) are also pushed straight to output
        if ((type & Numbers) || (type & PostfixOperators)) {
            output.emplace_back(std::move(token));

            // If we are inside a function and the preceding token was
            // ( , UnaryMinus, or UnaryPlus, this value counts as a new argument
            if (!argcStack.empty() && i > 0) {
                const TokenType prev = GetTokenType(infixTokens[i - 1]);
                if (prev == Comma || prev == LeftParen
                    || prev == UnaryMinus || prev == UnaryPlus) {
                    ++argcStack.top();
                }
            }
            // Edge: first token of a function when argcStack already active and i==0
            // (Shouldn't happen in practice since functions always start with func name + '(')
        }
        //Math functions (sin, cos, max, min, ...)
        else if (type & MathFunctions) {
            opStack.push(std::move(token));

            // If nested inside another function, the outer function gets +1 arg
            if (!argcStack.empty() && i > 0) {
                const TokenType prev = GetTokenType(infixTokens[i - 1]);
                if (prev == Comma || prev == LeftParen
                    || prev == UnaryMinus || prev == UnaryPlus) {
                    ++argcStack.top();
                }
            }

            // Push a new argc counter for THIS function
            argcStack.push(0);
        }
        //Left Parenthesis
        else if (type == LeftParen) {
            opStack.push(std::move(token));
        }
        //Right Parenthesis
        else if (type == RightParen) {
            // Pop operators until we find LeftParen
            while (!opStack.empty() && GetTokenType(opStack.top()) != LeftParen) {
                output.emplace_back(std::move(opStack.top()));
                opStack.pop();
            }

            if (opStack.empty())
                throw std::runtime_error("Mismatched parentheses");

            // Discard the LeftParen
            opStack.pop();

            // If the top of the stack is a function, pop it with its argc
            if (!opStack.empty() && TokenTypeMatches(opStack.top(), MathFunctions)) {
                // Set the argc on the FuncOpToken before pushing to output
                auto funcToken = std::move(opStack.top());
                opStack.pop();

                if (!argcStack.empty()) {
                    auto& func = std::get<FuncOpToken>(funcToken);
                    func.argc = argcStack.top();
                    argcStack.pop();
                }

                output.emplace_back(std::move(funcToken));
            }
        }
        //Comma
        else if (type == Comma) {
            // Pop operators until LeftParen (don't pop the paren itself)
            while (!opStack.empty() && GetTokenType(opStack.top()) != LeftParen) {
                output.emplace_back(std::move(opStack.top()));
                opStack.pop();
            }
        }
        //Operators (+, -, *, /, ^, mod, UnaryMinus, UnaryPlus, etc.)
        else {
            const auto& currentInfo = GetOperatorInfoFromToken(token);

            // Standard Shunting-Yard precedence/associativity handling
            while (!opStack.empty()) {
                const TokenType topType = GetTokenType(opStack.top());

                // Stop at left parentheses
                if (topType == LeftParen) break;

                // Stop if the top is not an operator/function in the operator map
                if (!operatorMap.contains(topType)) break;

                const auto& topInfo = operatorMap.at(topType);

                // Pop if top has higher precedence, or same precedence + left-associative
                if (topInfo.precedence > currentInfo.precedence ||
                    (topInfo.precedence == currentInfo.precedence && currentInfo.isLeftAssociative)) {
                    output.emplace_back(std::move(opStack.top()));
                    opStack.pop();
                } else {
                    break;
                }
            }

            opStack.push(std::move(token));
        }
    }

    // Drain remaining operators
    while (!opStack.empty()) {
        const TokenType topType = GetTokenType(opStack.top());
        if (topType == LeftParen || topType == RightParen)
            throw std::runtime_error("Mismatched parentheses");

        output.emplace_back(std::move(opStack.top()));
        opStack.pop();
    }

    return output;
}

//Public Entry Point

std::vector<Token> Parser::ToPostfix(std::vector<Token>& infixTokens) {
    Validate(infixTokens);
    resolveUnaryOperators(infixTokens);
    return shuntingYard(infixTokens);
}
