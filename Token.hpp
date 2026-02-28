#pragma once

#include <variant>
#include <ostream>
#include <complex>
#include <unordered_map>
#include <functional>
#include <cmath>
#include <iomanip>
#include <cstdint>

enum TokenType : uint32_t {
    Add        = 1 << 0,  // 1
    Sub        = 1 << 1,  // 2
    Mul        = 1 << 2,  // 4
    Div        = 1 << 3,  // 8
    Pow        = 1 << 4,  // 16
    Min        = 1 << 5,  // 32
    Max        = 1 << 6,  // 64
    Sin        = 1 << 7,  // 128
    Cos        = 1 << 8,  // 256
    Tan        = 1 << 9,  // 512
    Cot        = 1 << 10, // 1024
    Sqrt       = 1 << 11, // 2048
    Number     = 1 << 12,
    LeftParen  = 1 << 13,
    RightParen = 1 << 14,
    Comma      = 1 << 15
};

// ! Trigonometric functions expects degrees not radians

constexpr uint32_t MathFunctions = (Sin | Cos | Tan | Cot | Sqrt | Max | Min);

using UnaryFunc = std::function<double(double)>;
using BinaryFunc = std::function<double(double, double)>;
using VariadicFunc = std::function<double(const std::vector<double>&)>;


struct OperatorInfo {
    int precedence;
    bool isLeftAssociative;
    std::variant<UnaryFunc, BinaryFunc, VariadicFunc> action;
};

static inline const std::unordered_map<TokenType, OperatorInfo> operatorMap = {
    {Add , {0, true, BinaryFunc(std::plus<double>{})}},
    {Sub , {0, true, BinaryFunc(std::minus<double>{})}},
    {Mul , {1, true, BinaryFunc(std::multiplies<double>{})}},
    {Div , {1, true, BinaryFunc(std::divides<double>{})}},
    {Pow , {2, false, BinaryFunc([](const double a, const double b) { return std::pow(a, b); })}},
    {Min , {3, false, VariadicFunc([](const std::vector<double>& v){
        if (v.empty()) return 0.0;   // ! mabe throwing an exception would be better
        //* std::min_element returns a pointer to the min element in the vector
        return static_cast<double>(*std::min_element(v.begin(), v.end()));
    })}},
    {Max , {3, true, VariadicFunc([](const std::vector<double>& v) {
        if (v.empty()) return 0.0;   // ! again throw an error when a global exception handler is implemented
        return static_cast<double>(*std::max_element(v.begin(), v.end()));
    })}},
    {Sin , {3, false, UnaryFunc([](double a) { return std::sin(a * M_PI / 180.0); })}},
    {Cos , {3, false, UnaryFunc([](double a) { return std::cos(a * M_PI / 180.0); })}},
    {Tan , {3, false, UnaryFunc([](double a) { return std::tan(a * M_PI / 180.0); })}},
    {Cot , {3, false, UnaryFunc([](double a) { return 1 / std::tan(a * M_PI / 180.0); })}},
    {Sqrt , {3, false, UnaryFunc([](double a) { return std::sqrt(a); })}},
};

#include <iostream>

struct Token {
    TokenType type;
    double literalValue = 0;
    // ? can be even a vector in the future for multidimensional calculations
    std::size_t argc = 0;

    explicit Token(const TokenType type, const double literalValue = 0)
        : type(type), literalValue(literalValue)
    {
    }

    [[nodiscard]] bool IsOperator() const {
        return type & (Add | Sub | Mul | Div | Pow);
    }

    [[nodiscard]] bool IsFunction() const {
        return type & (Sin | Cos | Tan | Cot | Sqrt | Min | Max);
    }

    [[nodiscard]] const OperatorInfo& GetOperatorInfo() const {
        return operatorMap.at(type);
    }

    friend std::ostream& operator<<(std::ostream& os, const Token& token) {
        switch (token.type) {
            case Add      :
                os << "+";
                break;
            case Sub      :
                os << "-";
                break;
            case Mul      :
                os << "*";
                break;
            case Div      :
                os << "/";
                break;
            case Pow      :
                os << "^";
                break;
            case Min      :
                os << "min";
                break;
            case Max      :
                os << "max";
                break;
            case Sin      :
                os << "sin";
                break;
            case Cos      :
                os << "cos";
                break;
            case Tan      :
                os << "tan";
                break;
            case Cot      :
                os << "cot";
                break;
            case Sqrt     :
                os << "sqrt";
                break;
            case Number   :
                os << std::setprecision(9) << token.literalValue;
                break;
            case LeftParen :
                os << "(";
                break;
            case RightParen:
                os << ")";
                break;
        }
        return os;
    }
};
