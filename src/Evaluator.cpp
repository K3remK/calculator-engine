//
// Created by kerem on 2/28/26.
//

#include "Evaluator.hpp"
#include <stack>
#include <type_traits>
#include <variant>




double Evaluator::Evaluate(const std::vector<Token> &postfixTokens) {
    std::stack<Token> stack;

    for (const auto& token : postfixTokens) {
        if (token.type & Numbers)
            stack.push(token);
        else {


            // TODO seperate the validation and execution logic
            if (token.type & MathFunctions) {  //* one of the math functions but not unary minus
                //* if the current token is a function and doesn't have any argument -> throw an error
                if (token.argc == 0)
                    throw std::runtime_error("Invalid usage of function: " + token.toString());
                //* if the current function is a unary func and have more than 1 argument -> throw error
                if (token.type & UnaryFunctions && token.argc > 1)
                    throw std::runtime_error("Wrong argument count for unary func: " + token.toString() + ", argc: " + std::to_string(token.argc));
                if (token.type & (LogBase) && token.argc != 2)
                    throw std::runtime_error("Wrong argument count for binary func: " + token.toString() + ", argc: " + std::to_string(token.argc));
            }

            auto actionVariant = token.GetOperatorInfo().action;

            //* identify the type of function that the operator have
            //* and execute it (Unary, Binary, Variadic, etc.)
            std::visit([&stack, &token](auto&& func) {
                using FuncType = std::decay_t<decltype(func)>;
                if constexpr (std::is_same_v<FuncType, UnaryFunc>) {
                    double a = stack.top().literalValue;
                    stack.pop();
                    stack.push(Token(Number, func(a)));
                } else if constexpr (std::is_same_v<FuncType, BinaryFunc>) {
                    double right = stack.top().literalValue;
                    stack.pop();
                    double left = stack.top().literalValue;
                    stack.pop();
                    stack.push(Token(Number, func(left, right)));
                } else if constexpr (std::is_same_v<FuncType, VariadicFunc>) {
                    std::vector<double> values;
                    for (std::size_t i = 0; i < token.argc; i++) {
                        values.emplace_back(stack.top().literalValue);
                        stack.pop();
                    }
                    stack.push(Token(Number, func(values)));
                }
            }, actionVariant);
        }
    }
    return stack.top().literalValue;
}
