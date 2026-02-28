//
// Created by kerem on 2/28/26.
//

#include "Parser.hpp"

#include <cassert>
#include <queue>
#include <stack>
#include <iostream>

std::vector<Token> Parser::ToPostfix(const std::vector<Token>& infixTokens) {
    auto stack = std::stack<Token>();
    //auto queue = std::queue<Token>();

    std::vector<Token> queue;
    std::size_t index = 0;
    // ! Currently this algorithm does not take into account of the existence
    // ! of variable number functions such as min or max.
    while ( index < infixTokens.size() ) {
        switch (const auto& token = infixTokens[index]; token.type) {
            case Number:
                queue.emplace_back(token);
                break;
            case Sin | Cos | Tan | Cot | Sqrt:
                stack.push(token);
                break;
            case LeftParen:
                stack.push(token);
                break;
            case RightParen:
                assert(!stack.empty());
                while (!stack.empty() && stack.top().type != LeftParen) {
                    // ! if stack runs out of elements before finding a left parent.
                    // ! it means there is a parenthesis mismatch
                    assert(!stack.empty());
                    // ! there is an empty parentheses pair the input
                    queue.emplace_back(stack.top());
                    stack.pop();
                }
                assert(stack.top().type == LeftParen);
                stack.pop();
                if (!stack.empty() && stack.top().type & (Sin | Cos | Tan | Sqrt)) {
                    queue.emplace_back(stack.top());
                    stack.pop();
                }
                break;
            default:
                //* This means that token is an operator (+, -, * etc.)
                while (!stack.empty() && stack.top().IsOperator() && (stack.top().GetOperatorInfo().precedence > token.GetOperatorInfo().precedence
                    || (stack.top().GetOperatorInfo().precedence == token.GetOperatorInfo().precedence && token.GetOperatorInfo().isLeftAssociative))) {
                    queue.emplace_back(stack.top());
                    stack.pop();
                }
                stack.push(token);
                break;
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
