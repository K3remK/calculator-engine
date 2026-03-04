#pragma once

#include <utility>
#include <variant>
#include <ostream>
#include <complex>
#include <unordered_map>
#include <iomanip>
#include <cstdint>
#include "Matrix.h"

enum TokenType : uint64_t {
    Add                 = 1 << 0,  // 1
    Sub                 = 1 << 1,  // 2
    Mul                 = 1 << 2,  // 4
    Div                 = 1 << 3,  // 8
    Pow                 = 1 << 4,  // 16
    Min                 = 1 << 5,  // 32
    Max                 = 1 << 6,  // 64
    Sin                 = 1 << 7,  // 128
    Cos                 = 1 << 8,  // 256
    Tan                 = 1 << 9,  // 512
    Cot                 = 1 << 10, // 1024
    Sqrt                = 1 << 11, // 2048
    Number              = 1 << 12,
    LeftParen           = 1 << 13,
    RightParen          = 1 << 14,
    Comma               = 1 << 15,
    Mod                 = 1 << 16,
    PI                  = 1 << 17,
    Fact                = 1 << 18,
    Percent             = 1 << 19,
    UnaryMinus          = 1 << 20,
    UnaryPlus           = 1 << 21,
    Log                 = 1 << 22,
    Ln                  = 1 << 23,
    LogBase             = 1 << 24,
    Abs                 = 1 << 25,
    Euler               = 1 << 26,
    MatrixT              = 1 << 27,
};

// ! When you add a new function or operator !!!! Dont forget to update the IsOperator and IsFunction functions
// ! Trigonometric functions expects degrees not radians

constexpr uint32_t MathFunctions            = Sin | Cos | Tan | Cot | Sqrt | Max | Min | Log | Ln | LogBase | Abs;
constexpr uint32_t Numbers                  = Number | PI | Euler | MatrixT;
constexpr uint32_t PostfixOperators         = Fact | Percent;
constexpr uint32_t Operators                = Add | Sub | Mul | Div | Max | UnaryMinus | UnaryPlus;
constexpr uint32_t InfixOperators           = Add | Sub | Mul | Div | Pow;
constexpr uint32_t UnaryFunctions           = Sin | Cos | Tan | Cot | Sqrt | Log | Ln | Abs | UnaryPlus | UnaryMinus | Fact | Percent;
constexpr uint32_t VariadicFunctions        = Max | Min;


struct OperatorInfo {
    int precedence;
    bool isLeftAssociative;
    int expectedArgc;
};

inline static const std::unordered_map<TokenType, OperatorInfo> operatorMap = {
    {Add            ,           {0, true,  2}},
    {Sub            ,           {0, true,  2}},
    {Mod            ,           {1, true,  2}},
    {Mul            ,           {1, true,  2}},
    {Div            ,           {1, true,  2}},
    {Pow            ,           {2, false, 2}},
    {Min            ,           {3, false, 2}},
    {Max            ,           {3, true,  2}},
    {Percent        ,           {3, false, 1}},
    {Fact           ,           {3, false, 1}},
    {UnaryMinus     ,           {3, false, 1}},
    {UnaryPlus      ,           {3, false, 1}},
    {Sin            ,           {3, false, 1}},
    {Cos            ,           {3, false, 1}},
    {Tan            ,           {3, false, 1}},
    {Cot            ,           {3, false, 1}},
    {Sqrt           ,           {3, false, 1}},
    {Log            ,           {3, false, 1}},
    {Ln             ,           {3, false, 1}},
    {LogBase        ,           {3, false, 2}},
    {Abs            ,           {3, false, 1}}
};

// This variant can hold a double, an int, or a complex number.
// The memory footprint is roughly the size of the largest type + an enum tag.
using Value = std::variant<double, Matrix<double>>;

struct Token {
    mutable TokenType type;
    Value data;
    // ? can be even a vector in the future for multidimensional calculations
    std::size_t argc = 0;

    explicit Token(const TokenType type, Value data = {})
        : type(type), data(std::move(data))
    {
    }

    [[nodiscard]] bool IsOperator() const {
        return type & (Add | Sub | Mul | Div | Pow | UnaryMinus | UnaryPlus);
    }

    [[nodiscard]] bool IsFunction() const {
        return type & (Sin | Cos | Tan | Cot | Sqrt | Min | Max | Log | Ln | LogBase | Abs);
    }

    [[nodiscard]] const OperatorInfo& GetOperatorInfo() const {
        return operatorMap.at(type);
    }

    [[nodiscard]] std::string toString() const {
        std::ostringstream os;
        switch (type) {
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
            case Number:
                std::visit([&os](auto&& a) { os << std::setprecision(15) << a; }, data);
                break;
            case MatrixT:
                std::visit([&os](auto&& a) { os << std::setprecision(15) << a; }, data);
                break;
            case LeftParen :
                os << "(";
                break;
            case RightParen:
                os << ")";
                break;
            case Comma:
                os << ",";
                break;
            case Mod:
                os << "mod";
                break;
            case PI:
                os << "π";
                break;
            case Fact:
                os << "!";
                break;
            case Percent:
                os << "%";
                break;
            case UnaryMinus:
                os << "-";
                break;
            case UnaryPlus:
                os << "+";
                break;
            case Log:
                os << "log";
                break;
            case Ln:
                os << "ln";
                break;
            case LogBase:
                os << "logbase";
                break;
            case Abs:
                os << "abs";
                break;
            case Euler:
                os << "e";
                break;
            default: throw std::invalid_argument("Unknown operator");
        }
        return os.str();
    }

    friend std::ostream& operator<<(std::ostream& os, const Token& token) {
        os << token.toString();
        return os;
    }
};
