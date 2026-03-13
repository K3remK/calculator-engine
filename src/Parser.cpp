//
// Created by kerem on 2/28/26.
//

#include "Parser.hpp"
#include <queue>
#include <stack>

std::vector<Token> Parser::ToPostfix(std::vector<Token>& infixTokens) {
    Validate(infixTokens);
    resolveUnaryOperators(infixTokens);
    return shuntingYard(infixTokens);
}

void Parser::Validate(const std::vector<Token> &infixTokens) {
    if (infixTokens.empty())
        throw std::runtime_error("Empty expression");

    // Parenthesis Check
    int parentDepth = 0;
    for (const auto& token : infixTokens) {
        if (token.type == LeftParen) parentDepth++;
        else if (token.type == RightParen) {
            parentDepth--;
            if (parentDepth < 0) throw std::runtime_error("Mismatched parentheses: unexpected ')'");
        }
    }
    if (parentDepth != 0) throw std::runtime_error("Mismatched parentheses: unexpected '('");

    // First token checks
    {
        const auto& first = infixTokens.front();
        // an infix operator usage at the beginning of the expression
        if (first.type & InfixOperators && !(first.type & (Add | Sub)))
            throw std::runtime_error("Invalid operator usage: " + first.toString());
    }

    //Last token checks
    {
        const auto& last = infixTokens.back();
        if (last.type & InfixOperators)
            throw std::runtime_error("Expression ends with an operator: " + last.toString());
        if (last.type == LeftParen)
            throw std::runtime_error("Expression ends with '('");
        if (last.type == Comma)
            throw std::runtime_error("Expression ends with ','");
    }

    //Adjacency checks
    for (size_t i = 1; i < infixTokens.size(); ++i) {
        const auto& prev = infixTokens.at(i-1);
        const auto& curr = infixTokens.at(i);

        // Two consecutive value-like tokens without an operator between them
        // "2 3", "pi 5", "[matrix] pi"
        // Postfix operators are value-like on their left side, so "5! 3" is also caught
        const bool prevIsValue = (prev.type & (Number)) || (prev.type & PostfixOperators) || (prev.type == RightParen);
        const bool currIsValue = (curr.type & (Numbers ^ Variable)) || (curr.type == LeftParen);
        const bool currIsFunc  = curr.type & MathFunctions;

        if (prevIsValue && (currIsValue || currIsFunc))
            throw std::runtime_error("Invalid format: two consecutive values without an operator");

        // Operator right after '(' for example: "(* 3)"
        // +/- after '(' is allowed (unary), so exclude those
        if (prev.type == LeftParen && (curr.type & InfixOperators) && !(curr.type & (Add | Sub)))
            throw std::runtime_error("Operator immediately after '('");

        // ')' immediately after '(' — empty parentheses
        if (prev.type == LeftParen && curr.type == RightParen)
            throw std::runtime_error("Empty parentheses");

        // Comma not inside a function call context — basic check:
        // comma after '(' is fine, comma after a value is fine,
        // but comma after comma or comma after an infix operator is wrong
        if (curr.type == Comma && (prev.type == Comma || ((prev.type & InfixOperators) && !(prev.type & (Add | Sub)))))
            throw std::runtime_error("Invalid comma placement");
    }
}

void Parser::resolveUnaryOperators(std::vector<Token> &infixTokens) {
    for (size_t i = 0; i < infixTokens.size(); ++i) {
        const TokenType type = infixTokens[i].type;

        // Only Sub and Add can be unary
        if (type != Sub && type != Add) continue;

        // It is unary if:
        //  - it is the first token, OR
        //  - the previous token is '(', ',', another operator, or a unary op
        bool isUnary = false;
        if (i == 0) {
            isUnary = true;
        } else {
            const TokenType prev = infixTokens[i-1].type;
            // After (, comma, or any infix/unary operator → unary context
            isUnary = (prev == LeftParen)
                    || (prev == Comma)
                    || (prev & InfixOperators)
                    || (prev == UnaryMinus)
                    || (prev == UnaryPlus);
        }

        if (isUnary) {
            const TokenType newType = (type == Sub) ? UnaryMinus : UnaryPlus;
            infixTokens[i] = Token(newType);
        }
    }
}

std::vector<Token> Parser::shuntingYard(const std::vector<Token> &infixTokens) {
    std::vector<Token>  output;
    std::stack<Token>    opStack;
    std::stack<size_t>   argcStack;   // tracks argument count per function

    for (size_t i = 0; i < infixTokens.size(); ++i) {
        auto& token = infixTokens[i];

        //Values: Number, PI, Euler, Matrix, RawMatrix, Var
        // Postfix operators (!, %) are also pushed straight to output
        if ((token.type & Numbers) || (token.type & PostfixOperators)) {
            output.push_back(token);

            // If we are inside a function and the preceding token was
            // ( , UnaryMinus, or UnaryPlus, this value counts as a new argument
            if (!argcStack.empty() && i > 0) {
                const TokenType prev = infixTokens[i - 1].type;
                if (prev == Comma || prev == LeftParen
                    || prev == UnaryMinus || prev == UnaryPlus) {
                    ++argcStack.top();
                }
            }
            // Edge: first token of a function when argcStack already active and i==0
            // (Shouldn't happen in practice since functions always start with func name + '(')
        }
        //Math functions (sin, cos, max, min, ...)
        else if (token.type & MathFunctions) {
            opStack.push(token);

            // If nested inside another function, the outer function gets +1 arg
            if (!argcStack.empty() && i > 0) {
                const TokenType prev = infixTokens[i - 1].type;
                if (prev == Comma || prev == LeftParen
                    || prev == UnaryMinus || prev == UnaryPlus) {
                    ++argcStack.top();
                }
            }

            // Push a new argc counter for THIS function
            argcStack.push(0);
        }
        //Left Parenthesis
        else if (token.type == LeftParen) {
            opStack.push(token);
        }
        //Right Parenthesis
        else if (token.type == RightParen) {
            // Pop operators until we find LeftParen
            while (!opStack.empty() && opStack.top().type != LeftParen) {
                output.emplace_back(std::move(opStack.top()));
                opStack.pop();
            }

            if (opStack.empty())
                throw std::runtime_error("Mismatched parentheses");

            // Discard the LeftParen
            opStack.pop();

            // If the top of the stack is a function, pop it with its argc
            if (!opStack.empty() && opStack.top().type & MathFunctions) {
                // Set the argc on the FuncOpToken before pushing to output
                auto funcToken = std::move(opStack.top());
                opStack.pop();

                if (!argcStack.empty()) {
                    funcToken.argc = argcStack.top();
                    argcStack.pop();
                }

                output.emplace_back(std::move(funcToken));
            }
        }
        //Comma
        else if (token.type == Comma) {
            // Pop operators until LeftParen (don't pop the paren itself)
            while (!opStack.empty() && opStack.top().type != LeftParen) {
                output.emplace_back(std::move(opStack.top()));
                opStack.pop();
            }
        }
        //Operators (+, -, *, /, ^, mod, UnaryMinus, UnaryPlus, etc.)
        else {
            const auto& currentInfo = token.GetOperatorInfo();

            // Standard Shunting-Yard precedence/associativity handling
            while (!opStack.empty()) {
                const TokenType topType = opStack.top().type;

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

            opStack.push(token);
        }
    }

    // Drain remaining operators
    while (!opStack.empty()) {
        const TokenType topType = opStack.top().type;
        if (topType == LeftParen || topType == RightParen)
            throw std::runtime_error("Mismatched parentheses");

        output.emplace_back(std::move(opStack.top()));
        opStack.pop();
    }

    return output;
}
