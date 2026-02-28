#pragma once

#include <variant>
#include <ostream>
#include <complex>
#include <unordered_map>
#include <functional>

enum TokenType
{
    Add =           0b000000000000001,
    Sub =           0b000000000000010,
    Mul =           0b000000000000100,
    Div =           0b000000000001000,
    Pow =           0b000000000010000,
    Min =           0b000000000100000,
    Max =           0b000000001000000,
    Sin =           0b000000010000000,
    Cos =           0b000000100000000,
    Tan =           0b000001000000000,
    Cot =           0b000010000000000,
    Sqrt =          0b000100000000000,
    Number =        0b001000000000000,
    LeftParen =     0b010000000000000,
    RightParen =    0b100000000000000,
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
                os << token.literalValue;
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
