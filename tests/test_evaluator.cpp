#include <gtest/gtest.h>
#include "../src/Token.hpp"
#include "../src/Evaluator.hpp"

// Helper: build a postfix token vector and evaluate it
// This lets us test the Evaluator in isolation (without Lexer/Parser)

static double evalPostfix(const std::vector<Token>& tokens) {
    return Evaluator::Evaluate(tokens);
}

// ============================================================
// Binary arithmetic operations
// ============================================================

TEST(EvaluatorEvaluate, Addition) {
    // 3 + 4 = 7
    std::vector<Token> postfix = {Token(Number, 3.0), Token(Number, 4.0), Token(Add)};
    EXPECT_DOUBLE_EQ(evalPostfix(postfix), 7.0);
}

TEST(EvaluatorEvaluate, Subtraction) {
    // 10 - 3 = 7
    std::vector<Token> postfix = {Token(Number, 10.0), Token(Number, 3.0), Token(Sub)};
    EXPECT_DOUBLE_EQ(evalPostfix(postfix), 7.0);
}

TEST(EvaluatorEvaluate, Multiplication) {
    // 3 * 4 = 12
    std::vector<Token> postfix = {Token(Number, 3.0), Token(Number, 4.0), Token(Mul)};
    EXPECT_DOUBLE_EQ(evalPostfix(postfix), 12.0);
}

TEST(EvaluatorEvaluate, Division) {
    // 10 / 2 = 5
    std::vector<Token> postfix = {Token(Number, 10.0), Token(Number, 2.0), Token(Div)};
    EXPECT_DOUBLE_EQ(evalPostfix(postfix), 5.0);
}

TEST(EvaluatorEvaluate, Power) {
    // 2 ^ 3 = 8
    std::vector<Token> postfix = {Token(Number, 2.0), Token(Number, 3.0), Token(Pow)};
    EXPECT_DOUBLE_EQ(evalPostfix(postfix), 8.0);
}

TEST(EvaluatorEvaluate, Modulus) {
    // 10 mod 3 = 1
    std::vector<Token> postfix = {Token(Number, 10.0), Token(Number, 3.0), Token(Mod)};
    EXPECT_DOUBLE_EQ(evalPostfix(postfix), 1.0);
}

// ============================================================
// Unary operations
// ============================================================

TEST(EvaluatorEvaluate, SinDegrees) {
    // sin(90) = 1.0
    auto t1 = Token(Number, 90.0);
    auto t2 = Token(Sin);
    t2.argc = 1;
    std::vector<Token> postfix = {t1, t2};
    EXPECT_NEAR(evalPostfix(postfix), 1.0, 1e-9);
}

TEST(EvaluatorEvaluate, CosDegrees) {
    // cos(0) = 1.0
    auto t1 = Token(Number, 0.0);
    auto t2 = Token(Cos);
    t2.argc = 1;
    std::vector<Token> postfix = {t1, t2};
    EXPECT_NEAR(evalPostfix(postfix), 1.0, 1e-9);
}

TEST(EvaluatorEvaluate, SqrtValue) {
    // sqrt(16) = 4.0
    auto t1 = Token(Number, 16.0);
    auto t2 = Token(Sqrt);
    t2.argc = 1;
    std::vector<Token> postfix = {t1, t2};
    EXPECT_DOUBLE_EQ(evalPostfix(postfix), 4.0);
}

TEST(EvaluatorEvaluate, Factorial) {
    // 5! = 120
    std::vector<Token> postfix = {Token(Number, 5.0), Token(Fact)};
    EXPECT_DOUBLE_EQ(evalPostfix(postfix), 120.0);
}

TEST(EvaluatorEvaluate, Percent) {
    // 80% = 0.8
    std::vector<Token> postfix = {Token(Number, 80.0), Token(Percent)};
    EXPECT_DOUBLE_EQ(evalPostfix(postfix), 0.8);
}

// ============================================================
// Variadic operations
// ============================================================

TEST(EvaluatorEvaluate, MinVariadic) {
    // min(5, 3, 8) = 3.0
    Token minTok(Min);
    minTok.argc = 3;
    std::vector<Token> postfix = {Token(Number, 5.0), Token(Number, 3.0), Token(Number, 8.0), minTok};
    EXPECT_DOUBLE_EQ(evalPostfix(postfix), 3.0);
}

TEST(EvaluatorEvaluate, MaxVariadic) {
    // max(5, 3, 8) = 8.0
    Token maxTok(Max);
    maxTok.argc = 3;
    std::vector<Token> postfix = {Token(Number, 5.0), Token(Number, 3.0), Token(Number, 8.0), maxTok};
    EXPECT_DOUBLE_EQ(evalPostfix(postfix), 8.0);
}

// ============================================================
// Multi-step operations
// ============================================================

TEST(EvaluatorEvaluate, MultiStepExpression) {
    // 2 + 3 * 4 = 14  →  postfix: 2 3 4 * +
    std::vector<Token> postfix = {
        Token(Number, 2.0),
        Token(Number, 3.0),
        Token(Number, 4.0),
        Token(Mul),
        Token(Add)
    };
    EXPECT_DOUBLE_EQ(evalPostfix(postfix), 14.0);
}

TEST(EvaluatorEvaluate, ParenthesizedExpression) {
    // (2 + 3) * 4 = 20  →  postfix: 2 3 + 4 *
    std::vector<Token> postfix = {
        Token(Number, 2.0),
        Token(Number, 3.0),
        Token(Add),
        Token(Number, 4.0),
        Token(Mul)
    };
    EXPECT_DOUBLE_EQ(evalPostfix(postfix), 20.0);
}

TEST(EvaluatorEvaluate, SingleNumber) {
    // Just a number → returns itself
    std::vector<Token> postfix = {Token(Number, 42.0)};
    EXPECT_DOUBLE_EQ(evalPostfix(postfix), 42.0);
}

TEST(EvaluatorEvaluate, PiConstant) {
    // PI → returns M_PI
    std::vector<Token> postfix = {Token(PI, M_PI)};
    EXPECT_DOUBLE_EQ(evalPostfix(postfix), M_PI);
}

// ============================================================
// Log, Ln, LogBase, Abs
// ============================================================

TEST(EvaluatorEvaluate, LogValue) {
    // log(100) = 2.0
    auto t1 = Token(Number, 100.0);
    auto t2 = Token(Log);
    t2.argc = 1;
    std::vector<Token> postfix = {t1, t2};
    EXPECT_DOUBLE_EQ(evalPostfix(postfix), 2.0);
}

TEST(EvaluatorEvaluate, LnValue) {
    // ln(1) = 0.0
    auto t1 = Token(Number, 1.0);
    auto t2 = Token(Ln);
    t2.argc = 1;
    std::vector<Token> postfix = {t1, t2};
    EXPECT_DOUBLE_EQ(evalPostfix(postfix), 0.0);
}

TEST(EvaluatorEvaluate, AbsNegative) {
    // abs(-5) = 5.0
    auto t1 = Token(Number, -5.0);
    auto t2 = Token(Abs);
    t2.argc = 1;
    std::vector<Token> postfix = {t1, t2};
    EXPECT_DOUBLE_EQ(evalPostfix(postfix), 5.0);
}

TEST(EvaluatorEvaluate, AbsPositive) {
    // abs(3) = 3.0
    auto t1 = Token(Number, 3.0);
    auto t2 = Token(Abs);
    t2.argc = 1;
    std::vector<Token> postfix = {t1, t2};
    EXPECT_DOUBLE_EQ(evalPostfix(postfix), 3.0);
}

TEST(EvaluatorEvaluate, LogBaseValue) {
    // logbase(8, 2) = 3.0
    auto t1 = Token(Number, 8.0);
    auto t2 = Token(Number, 2.0);
    auto t3 = Token(LogBase);
    t3.argc = 2;
    std::vector<Token> postfix = {t1, t2, t3};
    EXPECT_NEAR(evalPostfix(postfix), 3.0, 1e-9);
}

// ============================================================
// Edge cases — argc validation
// ============================================================

TEST(EvaluatorEvaluate, FunctionWithZeroArgcThrows) {
    // sin() with argc=0 should throw
    auto t = Token(Sin);
    t.argc = 0;
    std::vector<Token> postfix = {Token(Number, 1.0), t};
    EXPECT_THROW(evalPostfix(postfix), std::runtime_error);
}

TEST(EvaluatorEvaluate, UnaryFuncWithTooManyArgsThrows) {
    // log with argc=2 should throw (it's unary)
    auto t = Token(Log);
    t.argc = 2;
    std::vector<Token> postfix = {Token(Number, 1.0), Token(Number, 2.0), t};
    EXPECT_THROW(evalPostfix(postfix), std::runtime_error);
}

TEST(EvaluatorEvaluate, LogBaseWrongArgcThrows) {
    // logbase with argc=1 should throw (needs exactly 2)
    auto t = Token(LogBase);
    t.argc = 1;
    std::vector<Token> postfix = {Token(Number, 8.0), t};
    EXPECT_THROW(evalPostfix(postfix), std::runtime_error);
}
