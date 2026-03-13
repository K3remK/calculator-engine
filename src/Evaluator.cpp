//
// Created by kerem on 2/28/26.
//

#include "Evaluator.hpp"

#include <complex>
#include <map>

#include "Matrix.hpp"
#include <stack>
#include <type_traits>


Token Evaluator::Evaluate(const std::vector<Token> &postfixTokens, std::unordered_map<std::string_view, Token> &variables) {
    std::stack<Token> stack;

    //TODO: ADD A MORE GENERAL VALIDATION CHECK

    for (const auto& token : postfixTokens) {
        validate(token);
        if (token.type & Numbers)
            if (token.type & Variable && variables.find(*token.variable_name) != variables.end())
                stack.push(variables.at(*token.variable_name));
            else
                stack.push(token);
        else {
            if (token.type & VariadicFunctions) {
                std::vector<Token> operands;
                operands.reserve(token.argc);
                //! ORDER OF PARAMETERS REVERSED HERE CURRENTLY DOES NOT MATTER FOR MIN AND MAX FUNCTIONS
                for (size_t i = 0; i < token.argc; i++) {
                    operands.push_back(std::move(stack.top()));
                    stack.pop();
                }
                stack.push(evalVariadic(token.type, operands));
            } else if (token.type & UnaryFunctions) {
                auto right = std::move(stack.top());
                stack.pop();
                stack.push(evalUnary(token.type, right));
            } else {
                auto right = std::move(stack.top());
                stack.pop();
                auto left = std::move(stack.top());
                stack.pop();
                stack.push(evalBinary(token.type, left, right));
            }
        }
    }
    auto result = std::move(stack.top());
    stack.pop();

    if (result.type & Variable) variables.insert_or_assign(*result.variable_name, result);

    return result;
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

Token Evaluator::minVariadic(const std::vector<Token> &operands) {
    if (operands.empty())
        throw std::invalid_argument("min() requires at least one argument");

    auto currentMin = std::numeric_limits<double>::max();
    bool initialized = false;

    for (const Token& v : operands) {
        double x = std::visit([](auto&& a) -> double {
            using T = std::decay_t<decltype(a)>;
            if constexpr (std::is_same_v<T, double>) return a;
            else throw std::invalid_argument("min() supports only int/double arguments");
        }, v.data);

        if (!initialized) { currentMin = x; initialized = true; }
        else currentMin = std::min(currentMin, x);
    }

    return Token(Number, currentMin);
}

Token Evaluator::maxVariadic(const std::vector<Token> &operands) {
    if (operands.empty())
        throw std::invalid_argument("max() requires at least one argument");

    auto currentMax = std::numeric_limits<double>::lowest();
    bool initialized = false;

    for (const Token& v : operands) {
        double x = std::visit([](auto&& a) -> double {
            using T = std::decay_t<decltype(a)>;
            if constexpr (std::is_same_v<T, int>) return static_cast<double>(a);
            else if constexpr (std::is_same_v<T, double>) return a;
            else throw std::invalid_argument("max() supports only int/double arguments");
        }, v.data);

        if (!initialized) { currentMax = x; initialized = true; }
        else currentMax = std::max(currentMax, x);
    }

    return Token(Number, currentMax);
}

Token Evaluator::evalBinary(const TokenType op, const Token &left, const Token &right) {
    switch (op) {
        case Add:
            return std::visit(overloaded{
                [](const double a, const double b) -> Token {
                    return Token(Number, a + b);
                },
                [](auto&& a, auto&& b) -> Token {
                    return Token(MatrixT, a + b);
                }
            }, left.data, right.data);
        case Sub:
            return std::visit(overloaded{
                [](const double a, const double b) -> Token {
                    return Token(Number, a - b);
                },
                [](auto&& a, auto&& b) -> Token {
                    return Token(MatrixT, a - b);
                }
            }, left.data, right.data);
        case Mul:
            return std::visit(overloaded{
                [](const double a, const double b) -> Token {
                    return Token(Number, a * b);
                },
                [](auto&& a, auto&& b) -> Token {
                    return Token(MatrixT, a * b);
                }
            }, left.data, right.data);
        case Div:
            return std::visit(overloaded{
                [](const double a, const double b) -> Token {
                    return Token(Number, a / b);
                },
                [](auto&& a, auto&& b) -> Token {
                    return Token(MatrixT, a / b);
                }
            }, left.data, right.data);
        case InvMul:
            return std::visit(overloaded{
                [](const Matrix<double>& a, const Matrix<double>& b) -> Token { return Token(MatrixT, Matrix<double>::Solve(a, b)); },
                [](auto&&, auto&&) -> Token {
                    throw std::invalid_argument("InvMul not supported for this type");
                }
            }, left.data, right.data);
        case Mod:
            return evaluateMod(left, right);
        case Pow:
            return std::visit(overloaded{
                [](const Matrix<double>& m, const double a) -> Token {
                    Matrix<double> result = m.Identity();
                    for (int i = 0; i < a; i++)
                        result = result * m;
                    return Token(MatrixT, result);
                },
                [](const double a, const double b) -> Token { return Token(Number, std::pow(a, b)); },
                [](auto&&, auto&&) -> Token {
                    throw std::invalid_argument("Pow not supported for this type");
                }
            }, left.data, right.data);
        case LogBase:
            return std::visit(overloaded{
                [](const double a, const double b) -> Token { return Token(Number, std::log(a) / std::log(b)); },
                [](auto&&, auto&&) -> Token {
                    throw std::invalid_argument("LogBase not supported for this type");
                }
            }, left.data, right.data);
        case Assignment: {
            if (!(left.type & Variable)) throw std::invalid_argument("Invalid assignment: " + left.toString());
            if (!(right.type & Numbers)) throw std::invalid_argument("Invalid assignment: " + right.toString());
            return Token(*left.variable_name, right.data);
        }
        default:
            throw std::invalid_argument("Invalid operator: " + std::to_string(op));
    }
}


Token Evaluator::evalUnary(const TokenType op, const Token &operand) {
    return std::visit([&](auto&& a) -> Token {
        using T = std::decay_t<decltype(a)>;
        switch (op) {
        case Fact:
                return factorial(operand);
            case Percent:
                if constexpr (std::is_same_v<T, double>)
                    return Token(Number, a * 0.01);
                throw std::invalid_argument("Sin only supports double/int");
            case UnaryMinus:
                return Token(operand.type, -a);
            case UnaryPlus:
                return Token(operand.type, a);
            case Sin:
                if constexpr (std::is_same_v<T, double>)
                    return Token(Number, std::sin(a * M_PI / 180));
                throw std::invalid_argument("Sin only supports double/int");
            case Cos:
                if constexpr (std::is_same_v<T, double>)
                    return Token(Number, std::cos(a * M_PI / 180));
                throw std::invalid_argument("Cos only supports double/int");
            case Tan:
                if constexpr (std::is_same_v<T, double>)
                    return Token(Number, std::tan(a * M_PI / 180));
                throw std::invalid_argument("Tan only supports double/int");
            case Cot:
                if constexpr (std::is_same_v<T, double>)
                    return Token(Number, 1.0 / std::tan(a * M_PI / 180));
                throw std::invalid_argument("Cot only supports double/int");
            case Sqrt:
                if constexpr (std::is_same_v<T, double>)
                    return Token(Number, std::sqrt(a));
                throw std::invalid_argument("Sqrt only supports double/int");
            case Log:
                if constexpr (std::is_same_v<T, double>)
                    return Token(Number, std::log10(a));
                throw std::invalid_argument("Log only supports double/int");
            case Abs:
                if constexpr (std::is_same_v<T, double>)
                    return Token(Number, std::abs(a));
                throw std::invalid_argument("abs only supports double/int");
            case Ln:
                if constexpr (std::is_same_v<T, double>)
                    return Token(Number, std::log(a));
                throw std::invalid_argument("Ln only supports double/int");
            default: throw std::invalid_argument("Invalid operator: " + std::to_string(op));
        }
    }, operand.data);
}

Token Evaluator::evalVariadic(TokenType op, const std::vector<Token> &operands) {
    switch (op) {
        case Min:
            return minVariadic(operands);
        case Max:
            return maxVariadic(operands);
        default:
            throw std::invalid_argument("Invalid operator: " + std::to_string(op));
    }
}

Token Evaluator::evaluateMod(const Token &left, const Token &right) {
    return std::visit(overloaded{
        [](const double a, const double b) -> Token { return Token(Number, fmod(a, b)); },
        [](auto&&, auto&&) -> Token {
                    throw std::invalid_argument("Modulus not supported for this type");
        }
    }, left.data, right.data);
}

Token Evaluator::factorial(const Token &operand) {
    return std::visit(overloaded{
            [](double a) -> Token {
                if (a == 0) return Token(Number, 1.0);
                if (static_cast<int>(a) != a) throw std::invalid_argument("Factorial supports only integers");
                double x = 1.0;
                while (a > 1.0) { x *= a; a -= 1.0; }
                return Token(Number, x);
            },
            [](auto&&) -> Token {
                throw std::invalid_argument("Factorial not supported for this type");
            }
        }, operand.data);
}
