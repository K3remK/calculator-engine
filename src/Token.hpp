#pragma once

#include <utility>
#include <variant>
#include <ostream>
#include <unordered_map>
#include <iomanip>
#include <memory>

#include "Matrix.hpp"

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
    Assignment          = 1ULL << 30,   // TODO
    Var                 = 1ULL << 31    // TODO
};

enum TokenT {
    FuncOpTT,
    NumberTT,
    RawMatrixTT,
    NMatrixTT,
    VarTokenTT
};

// ! When you add a new function or operator !!!! Dont forget to update the IsOperator and IsFunction functions
// ! Trigonometric functions expects degrees not radians

constexpr uint64_t MathFunctions            = Sin | Cos | Tan | Cot | Sqrt | Max | Min | Log | Ln | LogBase | Abs;
constexpr uint64_t Numbers                  = Number | PI | Euler | MatrixT | Var;
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

inline const std::unordered_map<TokenType, OperatorInfo> operatorMap = {
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
    {Assignment     ,           {-1, false, 2}},    // Highest precedence so that the expression on the right side will be evaluated first
};

using Value = std::variant<double, std::unique_ptr<Matrix<double>>>;

struct FuncOpToken {
    TokenType type;
    size_t argc;

    explicit FuncOpToken(const TokenType type) : type(type), argc(0) {
        if (!operatorMap.contains(type)) throw std::invalid_argument("Invalid function token: " + std::to_string(type));
    }

    [[nodiscard]] std::string_view ToString() const {
        return std::string_view(std::to_string(type));
    }
};

struct NumberToken {
    TokenType type = Number;
    double data;
    NumberToken(const auto& data)
        : data(data)
    {}

    [[nodiscard]] std::string_view ToString() const {
        return std::string_view(std::to_string(data));
    }
};

struct MatrixToken {
    TokenType type = MatrixT;
    std::unique_ptr<Matrix<double>> data;

    MatrixToken(std::unique_ptr<Matrix<double>> ptr)
    : data(std::move(ptr)) {}

    MatrixToken(const MatrixToken& other)
        : type(other.type),
          data(other.data ? std::make_unique<Matrix<double>>(*other.data) : nullptr)
    {}

    [[nodiscard]] std::string_view ToString() const {
        std::stringstream ss;
        ss << *data;
        return ss.view();
    }
};

struct RawMatrixToken {
    TokenType type = RawMatrixT;
    std::unique_ptr<Matrix<std::string_view>> rawData;
    RawMatrixToken(std::unique_ptr<Matrix<std::string_view>> rawData)
        : rawData(std::move(rawData))
    {}

    RawMatrixToken(const RawMatrixToken& other)
        : type(other.type),
          rawData(other.rawData ? std::make_unique<Matrix<std::string_view>>(*other.rawData) : nullptr)
    {}

    [[nodiscard]] std::string_view ToString() const {
        std::stringstream ss;
        ss << *rawData;
        return ss.view();
    }
};

struct VarToken {
    TokenType type = Var;
    std::string_view variable_name;
    Value data;

    VarToken(const auto& variable_name, const auto& data_type, const auto& data)
        : variable_name(variable_name), data(data)
    {}

    VarToken(const auto& variable_name) : variable_name(variable_name) {
        data = 0.0;
    }

    VarToken(VarToken&& other) noexcept
        : variable_name(other.variable_name), data(std::move(other.data))
    {}

    VarToken(const VarToken& other)
        : type(other.type), variable_name(other.variable_name)
    {
        std::visit([&]<typename T>(const T& val) {
            if constexpr (std::is_same_v<T, double>) {
                data = val;
            } else if constexpr (std::is_same_v<T, std::unique_ptr<Matrix<double>>>) {
                data = val ? std::make_unique<Matrix<double>>(*val) : nullptr;
            }
        }, other.data);
    }

    VarToken& operator=(const VarToken& other) noexcept {
        if (this == &other) return *this;
        variable_name = other.variable_name;
        std::visit([&]<typename T0>(T0&& val) {
            using T = std::decay_t<T0>;
            if constexpr (std::is_same_v<T, double>) {
                data = val;
            }
            else if constexpr (std::is_same_v<T, std::unique_ptr<Matrix<double>>>)
                data = std::make_unique<Matrix<double>>(val->GetData());
        }, other.data);
        return *this;
    }

    [[nodiscard]] std::string_view ToString() const {
        std::stringstream ss;
        ss << variable_name << ": ";
        std::visit([&]<typename T0>(T0&& val) {
            if constexpr (std::is_same_v<T0, double>) {
                ss << val;
            } else if constexpr (std::is_same_v<T0, std::unique_ptr<Matrix<double>>>) {
                ss << *val;
            }
        }, data);
        return ss.view();
    }
};

using Token = std::variant<NumberToken, MatrixToken, RawMatrixToken, VarToken, FuncOpToken>;

static bool IsOperator(const TokenType type) {
    return type & (Add | Sub | Mul | Div | Pow | UnaryMinus | UnaryPlus | InvMul | Assignment);
}

static bool IsFunction(const TokenType type) {
    return type & (Sin | Cos | Tan | Cot | Sqrt | Min | Max | Log | Ln | LogBase | Abs);
}

static const OperatorInfo& GetOperatorInfo(const TokenType type) {
    return operatorMap.at(type);
}