#pragma once

#include <utility>
#include <variant>
#include <ostream>
#include <unordered_map>
#include <iomanip>
#include <cstdint>
#include "Matrix.hpp"
#include <memory>

enum TokenType : uint64_t {
    Add                 = 1ULL << 0,  // 1
    Sub                 = 1ULL << 1,  // 2
    Mul                 = 1ULL << 2,  // 4
    Div                 = 1ULL << 3,  // 8
    Pow                 = 1ULL << 4,  // 16
    Min                 = 1ULL << 5,  // 32
    Max                 = 1ULL << 6,  // 64
    Sin                 = 1ULL << 7,  // 128
    Cos                 = 1ULL << 8,  // 256
    Tan                 = 1ULL << 9,  // 512
    Cot                 = 1ULL << 10, // 1024
    Sqrt                = 1ULL << 11, // 2048
    Number              = 1ULL << 12,
    LeftParen           = 1ULL << 13,
    RightParen          = 1ULL << 14,
    Comma               = 1ULL << 15,
    Mod                 = 1ULL << 16,
    PI                  = 1ULL << 17,
    Fact                = 1ULL << 18,
    Percent             = 1ULL << 19,
    UnaryMinus          = 1ULL << 20,
    UnaryPlus           = 1ULL << 21,
    Log                 = 1ULL << 22,
    Ln                  = 1ULL << 23,
    LogBase             = 1ULL << 24,
    Abs                 = 1ULL << 25,
    Euler               = 1ULL << 26,
    MatrixT             = 1ULL << 27,
    RawMatrixT          = 1ULL << 28,
    InvMul              = 1ULL << 29,
    Equality            = 1ULL << 30,   // TODO
    Assignment          = 1ULL << 31,   // TODO
    Variable            = 1ULL << 32    // TODO
};

// ! When you add a new function or operator !!!! Dont forget to update the IsOperator and IsFunction functions
// ! Trigonometric functions expects degrees not radians

constexpr uint64_t MathFunctions            = Sin | Cos | Tan | Cot | Sqrt | Max | Min | Log | Ln | LogBase | Abs;
constexpr uint64_t Numbers                  = Number | PI | Euler | MatrixT | RawMatrixT | Variable;
constexpr uint64_t PostfixOperators         = Fact | Percent;
constexpr uint64_t Operators                = Add | Sub | Mul | Div | Max | UnaryMinus | UnaryPlus | InvMul;
constexpr uint64_t InfixOperators           = Add | Sub | Mul | Div | Pow | InvMul | Assignment;
constexpr uint64_t UnaryFunctions           = Sin | Cos | Tan | Cot | Sqrt | Log | Ln | Abs | UnaryPlus | UnaryMinus | Fact | Percent;
constexpr uint64_t VariadicFunctions        = Max | Min;


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
    {InvMul         ,           {2, true,  2}},
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
    {Abs            ,           {3, false, 1}},
    {Assignment     ,           {-1, true, 2}},
};

// This variant can hold a double, an int, or a complex number.
// The memory footprint is roughly the size of the largest type + an enum tag.
using Value = std::variant<double, Matrix<double>>;

struct Token {
    TokenType type;
    Value data;
    // ? can be even a vector in the future for multidimensional calculations
    uint16_t argc = 0;
    std::unique_ptr<std::string> variable_name;

    explicit Token(const TokenType type, Value data = {})
        : type(type), data(std::move(data))
    {
        variable_name = nullptr;
    }

    explicit Token(const std::string& variable_name_, Value data = {})
        : type(Variable), data(std::move(data)), variable_name(std::make_unique<std::string>(variable_name_)) {
    }

    Token(Token&& other) noexcept = default;
    Token& operator=(Token&& other) noexcept = default;

    // Copy constructor for Token to handle deep copy
    Token(const Token& other) {
        type = other.type;
        data = other.data;
        argc = other.argc;
        variable_name = other.variable_name ? std::make_unique<std::string>(*other.variable_name) : nullptr;
    }

    // Copy assignment for Token to handle deep copy
    Token& operator=(const Token& other) {
        if (this == &other) return *this;
        type = other.type;
        data = other.data;
        argc = other.argc;
        variable_name = other.variable_name ? std::make_unique<std::string>(*other.variable_name) : nullptr;
        return *this;
    }

    [[nodiscard]] bool IsOperator() const {
        return type & (Add | Sub | Mul | Div | Pow | UnaryMinus | UnaryPlus | InvMul);
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
            case InvMul:
                os << "\\";
                break;
            case Number:
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
            case Equality:
            case Assignment:
                os << "=";
                break;
            case Variable:
                os << *variable_name;
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
