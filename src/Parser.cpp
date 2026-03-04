//
// Created by kerem on 2/28/26.
//

#include "Parser.hpp"

#include <cassert>
#include <queue>
#include <stack>


// TODO: make this function smaller by implementing new smaller validation functions like validate number or function etc.

std::vector<Token> Parser::ToPostfix(const std::vector<Token>& infixTokens) {

    auto stack = std::stack<Token>();
    auto argcStack = std::stack<int>();

    if (const auto& t = infixTokens[infixTokens.size()-1] ; t.type & (MathFunctions | InfixOperators))
        //* wrong usage of functions or operators
            throw std::runtime_error("Invalid operator or function usage: " + t.toString());

    std::vector<Token> queue;
    std::size_t index = 0;
    while (index < infixTokens.size()) {
        //* Postfix operators like '%', '!' etc. are treated as numbers
        if (auto token = infixTokens[index]; token.type & (Numbers | PostfixOperators)) {
            if (token.type & Numbers) {
                if ((index > 0 && infixTokens[index-1].type & (Numbers | PostfixOperators | RightParen)) ||
                (index < infixTokens.size()-1 && infixTokens[index+1].type & (Numbers | LeftParen)))
                //* there is a number or Postfix operator that doesn't have anything to do with the equation
                    throw std::runtime_error("Invalid token: " + token.toString());
            }
            queue.emplace_back(token);
            if (token.type & PostfixOperators && !(infixTokens[index-1].type & (Numbers | RightParen))) {
                //* Invalid usage of postfix operators !5 or %8 for example
                throw std::runtime_error("Invalid postfix operator usage: " + token.toString());
            }
            //* if one of the following 4 comes before the number we can count it as an argument for the current function we are in
            if (!argcStack.empty() && infixTokens[index-1].type & (Comma | LeftParen | UnaryMinus | UnaryPlus)) {
                //* Increment the argument count for the current function
                const std::size_t argc = argcStack.top();
                argcStack.pop();
                argcStack.push(argc + 1);
            }
        }
        else if (token.type & MathFunctions) {
            stack.push(token);

            //* there is a function used inside a function so we increment the argc of the prev function
            //* to be countable as an argument prev token needs to be one of the following 4
            if (!argcStack.empty() && infixTokens[index-1].type & (Comma | LeftParen | UnaryMinus | UnaryPlus)) {
                //* Increment the argument count for the previous function
                //* because a new function is used inside the prev function
                const std::size_t argc = argcStack.top();
                argcStack.pop();
                argcStack.push(argc + 1);
            }

            argcStack.push(0);
        }
        else if (token.type == LeftParen) {
            stack.push(token);
        }
        else if (token.type == RightParen) {
            assert(!stack.empty());

            while (!stack.empty() && stack.top().type != LeftParen) {
                assert(!stack.empty());

                auto t = stack.top();

                queue.emplace_back(t);
                stack.pop();
            }

            assert(stack.top().type == LeftParen);
            stack.pop(); // Discard the LeftParen

            // *If the token before the parentheses was a function, pop it to the queue
            if (!stack.empty() && (stack.top().type & MathFunctions)) {
                auto t = stack.top();
                t.argc = argcStack.top();
                argcStack.pop();
                queue.emplace_back(t);
                stack.pop();
            }
        }
        else if (token.type == Comma) {
            while (!stack.empty() && stack.top().type != LeftParen) {
                queue.emplace_back(stack.top());
                stack.pop();
            }
        }
        else {
            //* Default block: This means the token is an operator (+, -, * etc.)
            if (index == 0 || !(infixTokens[index-1].type & (Numbers | PostfixOperators | RightParen))) {
                //* If we are here it means that there is an operator in the middle of an operator and a number
                //* meaning it is here to declare the sign of the number if it is a Sub token make it UnaryMinus token
                //* to be able to process multiple negations
                // TODO: the following
                //? maybe detecting the unary minus function inside the lexer class might be better for compatibility
                if (token.type == Sub) {
                    token = Token(UnaryMinus);
                    infixTokens[index].type = token.type;
                }
                else if (token.type == Add) {
                    token = Token(UnaryPlus);
                    infixTokens[index].type = token.type;
                }
                else
                    //* if it is not Sub or Add operator that means there is a miss usage of an operator (3 + * 3) throw an error
                    throw std::runtime_error("Invalid operator usage: " + token.toString());
            } else {
                while (!stack.empty() && stack.top().IsOperator() &&
                   (stack.top().GetOperatorInfo().precedence > token.GetOperatorInfo().precedence ||
                   (stack.top().GetOperatorInfo().precedence == token.GetOperatorInfo().precedence &&
                    token.GetOperatorInfo().isLeftAssociative))) {
                    queue.emplace_back(stack.top());
                    stack.pop();
                }
            }
            stack.push(token);
        }
        index++;
    }

    while (!stack.empty()) {
        if (stack.top().type & (LeftParen | RightParen)) {
            throw std::runtime_error("Mismatched parentheses");
        }
        assert(stack.top().type != LeftParen);
        queue.emplace_back(stack.top());
        stack.pop();
    }
    return queue;
}
