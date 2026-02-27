#pragma once

#include <variant>
#include <map>
#include <ostream>

enum class TokenType
{
    Addition,
    Subtraction,
    Multiplication,
    Division,
    Exponentiation,
    Parentheses,
    Number
};

/*
Precedence:
    addition = 0, subraction = 0,
    multiplication = division = 1,
    exponentiation = 2,
    parentheses = 3
*/

union Value
{
    double number;
    char op;
};

struct Token
{
    TokenType type;
    Value value;
    int precedence;

    Token() = default;

    Token(char type_)
    {
        this->type = typeMapper.at(type_);
        value.op = type_;
    }

    Token(double number)
    {
        type = TokenType::Number;
        value.number = number;
    }

    friend std::ostream &operator<<(std::ostream &os, const Token &t)
    {
        if (t.type == TokenType::Number)
        {
            os << "Token: " << t.value.number;
        }
        else
        {
            os << "Token: " << (t.value.op);
        }

        return os;
    }

private:
    std::map<char, int> precedenceMapper = {
        {'+', 0},
        {'-', 0},
        {'/', 1},
        {'*', 1},
        {'^', 2},
        {'(', 3},
        {')', 3}};
    std::map<char, TokenType> typeMapper = {
        {'+', TokenType::Addition},
        {'-', TokenType::Subtraction},
        {'*', TokenType::Multiplication},
        {'/', TokenType::Division},
        {'^', TokenType::Exponentiation},
        {'(', TokenType::Parentheses},
        {')', TokenType::Parentheses},
        {'N', TokenType::Number}};
};