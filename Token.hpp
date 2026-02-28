#pragma once

#include <variant>
#include <ostream>
#include <iostream>
#include <array>
#include <complex>
#include <unordered_map>
#include <functional>

enum TokenType
{
    Add ,
    Sub ,
    Mul ,
    Div ,
    Pow ,
    Min ,
    Max ,
    Sin ,
    Cos ,
    Tan ,
    Cot ,
    Sqrt,
    Number,
    LeftParen,
    RightParen,
};

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
    {Pow , {2, true, BinaryFunc([](const double a, const double b) { return std::pow(a, b); })}},
    {Min , {3, true, VariadicFunc([](const std::vector<double>& v){
        if (v.empty()) return 0.0;   // ! mabe throwing an exception would be better
        //* std::min_element returns a pointer to the min element in the vector 
        return static_cast<double>(*std::min_element(v.begin(), v.end()));
    })}},
    {Max , {3, true, VariadicFunc([](const std::vector<double>& v) {
        if (v.empty()) return 0.0;   // ! again throw an error when a global exception handler is implemented
        return static_cast<double>(*std::max_element(v.begin(), v.end()));
    })}},
    {Sin , {3, false, UnaryFunc([](double a) { return std::sin(a); })}},
    {Cos , {3, false, UnaryFunc([](double a) { return std::cos(a); })}},
    {Tan , {3, false, UnaryFunc([](double a) { return std::tan(a); })}},
    {Cot , {3, false, UnaryFunc([](double a) { return 1 / std::tan(a); })}},
    {Sqrt , {3, false, UnaryFunc([](double a) { return std::sqrt(a); })}},
};

struct Token {
    TokenType type;
    double literalValue = 0;

    explicit Token(const TokenType type, const double literalValue = 0)
        : type(type), literalValue(literalValue) 
    {

    }

    [[nodiscard]] const OperatorInfo& GetOperatorInfo() const {
        return operatorMap.at(type);
    }

    friend std::ostream& operator<<(std::ostream& os, const Token& token) {
        os << token.type;
        if (token.literalValue != 0) {
            os << " Value: " << token.literalValue;
        }
        return os;
    }
};
