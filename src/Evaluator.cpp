//
// Created by kerem on 2/28/26.
//

#include "Evaluator.hpp"
#include <complex>
#include "Matrix.hpp"
#include <stack>
#include <type_traits>
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


Token Evaluator::Evaluate(std::vector<Token> &postfixTokens, std::unordered_map<std::string_view, Value> &variable_map) {

    validate(postfixTokens);

    std::stack<Token> stack;
    const size_t N = postfixTokens.size();
    for (size_t index = 0; index < N; ++index) {
        auto& token = postfixTokens[index];
        std::visit([&]<typename T0>(T0&& t) {
            using T = std::decay_t<T0>;
            if constexpr (std::is_same_v<T, NumberToken> || std::is_same_v<T, VarToken> || std::is_same_v<T, MatrixToken>) {
                stack.push(t);
            } else if constexpr (std::is_same_v<T, FuncOpToken>) {
                if (t.argc > 2) {
                    std::vector<double> operands;
                    for (size_t i = 0; i < t.argc; ++i) {
                        auto& operand = stack.top();
                        stack.pop();
                        std::visit([&]<typename T1>(T1&& val) {
                            using T = std::decay_t<T1>;
                            if constexpr (std::is_same_v<T, std::unique_ptr<Matrix<double>>>)
                                throw std::runtime_error(std::format("Matrix operands are not supported for the function: {}", t.ToString()));
                            else if constexpr (std::is_same_v<T, double>)
                                operands.push_back(val);
                        }, operand);
                    }
                } else if (t.argc == 2) {
                    const Token right = stack.top();
                    stack.pop();
                    const Token left = stack.top();
                    stack.pop();
                    stack.push(evalBinary(t.type, left, right));
                } else {
                    const Token left = stack.top();
                    stack.pop();
                    stack.push(evalUnary(t.type, left));
                }
            }
        }, token);
    }
}

void Evaluator::validate(const std::vector<Token> &tokens) {

}

Token Evaluator::minVariadic(std::vector<double> &operands) {
    return NumberToken(*std::min_element(operands.begin(), operands.end()));
}

Token Evaluator::maxVariadic(std::vector<double> &operands) {
    return NumberToken(*std::max_element(operands.begin(), operands.end()));
}

Token Evaluator::evalBinary(const TokenType op, const Token &left, const Token &right) {
    switch (op) {
        case Add:
            return std::visit([](auto&& a, auto&& b) -> Value {
                using T = std::decay_t<decltype(a)>;
                using U = std::decay_t<decltype(b)>;
                if constexpr (std::is_same_v<T, NumberToken> && std::is_same_v<U, NumberToken>) {

                }
                return a + b;
            }, left, right);
        case Sub:
            return std::visit([](auto&& a, auto&& b) -> Value { return a - b; }, left, right);
        case Mul:
            return std::visit([](auto&& a, auto&& b) -> Value { return a * b; }, left, right);
        case Div:
            return std::visit([](auto&& a, auto&& b) -> Value { return a / b; }, left, right);
        case InvMul:
            return std::visit(overloaded{
                [](const Matrix<double>& a, const Matrix<double>& b) -> Value { return Matrix<double>::Solve(a, b); },
                [](auto&&, auto&&) -> Value {
                    throw std::invalid_argument("InvMul not supported for this type");
                }
            }, left, right);
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


Token Evaluator::evalUnary(TokenType op, const Token &operand) {


}

Token Evaluator::evalVariadic(const TokenType op, const std::vector<double> &operands) {

}

Token Evaluator::evaluateMod(const Token &left, const Token &right) {

}

double Evaluator::factorial(const Token &operand) {

}
