//
// Created by kerem on 2/28/26.
//

#include "Parser.hpp"

#include <cassert>
#include <queue>
#include <stack>

std::vector<Token> Parser::ToPostfix(const std::vector<Token>& infixTokens) {
    auto stack = std::stack<Token>();
    auto argcStack = std::stack<std::size_t>();

    std::vector<Token> queue;
    std::size_t index = 0;
    // ! Currently this algorithm does not take into account of the existence
    // ! of variable number functions such as min or max.
    while (index < infixTokens.size()) {
        const auto& token = infixTokens[index];

        if (token.type == Number) {
            queue.emplace_back(token);
        }
        else if (token.type & MathFunctions) {
            stack.push(token);
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

            // If the token before the parentheses was a function, pop it to the queue
            if (!stack.empty() && (stack.top().type & MathFunctions)) {
                auto t = stack.top();
                t.argc = argcStack.top() + 1;
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

            // Increment the argument count for the current variadic function
            const std::size_t argc = argcStack.top();
            argcStack.pop();
            argcStack.push(argc + 1);
        }
        else {
            // Default block: This means the token is an operator (+, -, * etc.)
            while (!stack.empty() && stack.top().IsOperator() &&
                   (stack.top().GetOperatorInfo().precedence > token.GetOperatorInfo().precedence ||
                   (stack.top().GetOperatorInfo().precedence == token.GetOperatorInfo().precedence &&
                    token.GetOperatorInfo().isLeftAssociative))) {

                queue.emplace_back(stack.top());
                stack.pop();
            }
            stack.push(token);
        }

        index++;
    }

    while (!stack.empty()) {
        if (stack.top().type & (LeftParen | RightParen)) {
            // ! There are mis matched parentheses in the input
        }
        assert(stack.top().type != LeftParen);
        queue.emplace_back(stack.top());
        stack.pop();
    }
    return queue;
}
