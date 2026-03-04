//
// Created by kerem on 2/28/26.
//

#include "Evaluator.hpp"
#include "Matrix.h"
#include <stack>
#include <type_traits>

#include <iostream>

// TODO: return a new type Result that contains a void pointer just like Token and a type if it is matrix (or vector) or double
Value Evaluator::Evaluate(const std::vector<Token> &postfixTokens) {
    std::stack<Token> stack;

    for (const auto& token : postfixTokens) {
        if (token.type & Numbers)
            stack.push(token);
        else {
            validate(token);
            if (token.type & VariadicFunctions) {
                std::vector<Value> operands;
                for (size_t i = 0; i < token.argc; i++) {
                    operands.push_back(stack.top().data);
                    stack.pop();
                }
                auto result = evalVariadic(token.type, operands);
                stack.emplace(Number, result);
            } else if (token.type & UnaryFunctions) {
                auto right = stack.top().data;
                stack.pop();
                stack.emplace(Number, evalUnary(token.type, right));
            } else {
                auto right = stack.top().data;
                stack.pop();
                auto left = stack.top().data;
                stack.pop();
                stack.emplace(Number, evalBinary(token.type, left, right));
            }
        }
    }
    return stack.top().data;
}

void Evaluator::validate(const Token &token) {
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
}

Value Evaluator::minVariadic(const std::vector<Value> &operands) {
    if (operands.empty())
        throw std::invalid_argument("min() requires at least one argument");

    auto currentMin = std::numeric_limits<double>::max();
    bool initialized = false;

    for (const Value& v : operands) {
        double x = std::visit([](auto&& a) -> double {
            using T = std::decay_t<decltype(a)>;
            if constexpr (std::is_same_v<T, int>) return static_cast<double>(a);
            else if constexpr (std::is_same_v<T, double>) return a;
            else throw std::invalid_argument("min() supports only int/double arguments");
        }, v);

        if (!initialized) { currentMin = x; initialized = true; }
        else currentMin = std::min(currentMin, x);
    }

    return currentMin;
}

Value Evaluator::maxVariadic(const std::vector<Value> &operands) {
    if (operands.empty())
        throw std::invalid_argument("max() requires at least one argument");

    auto currentMax = std::numeric_limits<double>::lowest();
    bool initialized = false;

    for (const Value& v : operands) {
        double x = std::visit([](auto&& a) -> double {
            using T = std::decay_t<decltype(a)>;
            if constexpr (std::is_same_v<T, int>) return static_cast<double>(a);
            else if constexpr (std::is_same_v<T, double>) return a;
            else throw std::invalid_argument("min() supports only int/double arguments");
        }, v);

        if (!initialized) { currentMax = x; initialized = true; }
        else currentMax = std::max(currentMax, x);
    }

    return currentMax;
}

Value Evaluator::evalBinary(const TokenType op, const Value &left, const Value &right) {
    switch (op) {
        case Add:
            return std::visit([](auto&& a, auto&& b) -> Value { return a + b; }, left, right);
        case Sub:
            return std::visit([](auto&& a, auto&& b) -> Value { return a - b; }, left, right);
        case Mul:
            return std::visit([](auto&& a, auto&& b) -> Value { return a * b; }, left, right);
        case Div:
            return std::visit([](auto&& a, auto&& b) -> Value { return a / b; }, left, right);
        case Mod:
            return evaluateMod(left, right);
        case Pow:
            return std::visit(overloaded{
                [](const Matrix<double>& m, const double a) -> Value {
                    Matrix<double> result = m.Identity();
                    for (int i = 0; i < a; i++)
                        result = result * m;
                    return result;
                },
                [](const double a, const double b) -> Value { return std::pow(a, b); },
                [](auto&&, auto&&) -> Value {
                    throw std::invalid_argument("Pow not supported for this type");
                }
            }, left, right);
        case LogBase:
            return std::visit(overloaded{
                [](const double a, const double b) -> Value { return std::log(a) / std::log(b); },
                [](auto&&, auto&&) -> Value {
                    throw std::invalid_argument("LogBase not supported for this type");
                }
            }, left, right);
        default:
            throw std::invalid_argument("Invalid operator: " + std::to_string(op));
    }
}


Value Evaluator::evalUnary(TokenType op, const Value &operand) {
    return std::visit([op](auto&& a) -> Value {
        using T = std::decay_t<decltype(a)>;
        switch (op) {
        case Fact:
                return factorial(a);
            case Percent:
                // ! Maybe throw an error if it is a matrix
                return a * 0.01;
            case UnaryMinus:
                return -a;
            case UnaryPlus:
                return a;
            case Sin:
                if constexpr (std::is_same_v<T, double>)
                    return std::sin(a * M_PI / 180);
                throw std::invalid_argument("Sin only supports double/int");
            case Cos:
                if constexpr (std::is_same_v<T, double>)
                    return std::cos(a * M_PI / 180);
                throw std::invalid_argument("Cos only supports double/int");
            case Tan:
                if constexpr (std::is_same_v<T, double>)
                    return std::tan(a * M_PI / 180);
                throw std::invalid_argument("Tan only supports double/int");
            case Cot:
                if constexpr (std::is_same_v<T, double>)
                    return 1.0 / std::tan(a * M_PI / 180);
                throw std::invalid_argument("Cot only supports double/int");
            case Sqrt:
                if constexpr (std::is_same_v<T, double>)
                    return std::sqrt(a);
                throw std::invalid_argument("Sqrt only supports double/int");
            case Log:
                if constexpr (std::is_same_v<T, double>)
                    return std::log10(a);
                throw std::invalid_argument("Log only supports double/int");
            case Abs:
                if constexpr (std::is_same_v<T, double>)
                    return std::abs(a);
                throw std::invalid_argument("abs only supports double/int");
            case Ln:
                if constexpr (std::is_same_v<T, double>)
                    return std::log(a);
                throw std::invalid_argument("Ln only supports double/int");
            default: throw std::invalid_argument("Invalid operator: " + std::to_string(op));
        }
    }, operand);
}

Value Evaluator::evalVariadic(TokenType op, const std::vector<Value> &operands) {
    switch (op) {
        case Min:
            return minVariadic(operands);
        case Max:
            return maxVariadic(operands);
        default:
            throw std::invalid_argument("Invalid operator: " + std::to_string(op));
    }
}

Value Evaluator::evaluateMod(const Value &left, const Value &right) {
    return std::visit(overloaded{
        [](const double a, const double b) -> Value { return fmod(a, b); },
        [](auto&&, auto&&) -> Value {
                    throw std::invalid_argument("Modulus not supported for this type");
        }
    }, left, right);
}

Value Evaluator::factorial(const Value &operand) {
    return std::visit(overloaded{
            [](double a) -> Value {
                if (a == 0) return 1.0;
                if (static_cast<int>(a) != a) throw std::invalid_argument("Factorial supports only integers");
                double x = 1.0;
                while (a > 1.0) { x *= a; a -= 1.0; }
                return x;
            },
            [](auto&&) -> Value {
                throw std::invalid_argument("Factorial not supported for this type");
            }
        }, operand);
}
