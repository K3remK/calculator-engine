#include <gtest/gtest.h>
#include <cmath>
#include "../src/Token.hpp"
#include "../src/Evaluator.hpp"

// Helper: build a postfix token vector and evaluate it
// This lets us test the Evaluator in isolation (without Lexer/Parser)

static double evalPostfix(const std::vector<Value>& tokens) {
    return std::get<double>(Evaluator::Evaluate(tokens));
}

// ============================================================
// Binary arithmetic operations
// ============================================================

TEST(EvaluatorEvaluate, Addition) {
    // 3 + 4 = 7
    std::vector<Value> postfix = {Value(Number, 3.0), Value(Number, 4.0), Value(Add)};
    EXPECT_DOUBLE_EQ(evalPostfix(postfix), 7.0);
}

TEST(EvaluatorEvaluate, Subtraction) {
    // 10 - 3 = 7
    std::vector<Value> postfix = {Value(Number, 10.0), Value(Number, 3.0), Value(Sub)};
    EXPECT_DOUBLE_EQ(evalPostfix(postfix), 7.0);
}

TEST(EvaluatorEvaluate, Multiplication) {
    // 3 * 4 = 12
    std::vector<Value> postfix = {Value(Number, 3.0), Value(Number, 4.0), Value(Mul)};
    EXPECT_DOUBLE_EQ(evalPostfix(postfix), 12.0);
}

TEST(EvaluatorEvaluate, Division) {
    // 10 / 2 = 5
    std::vector<Value> postfix = {Value(Number, 10.0), Value(Number, 2.0), Value(Div)};
    EXPECT_DOUBLE_EQ(evalPostfix(postfix), 5.0);
}

TEST(EvaluatorEvaluate, Power) {
    // 2 ^ 3 = 8
    std::vector<Value> postfix = {Value(Number, 2.0), Value(Number, 3.0), Value(Pow)};
    EXPECT_DOUBLE_EQ(evalPostfix(postfix), 8.0);
}

TEST(EvaluatorEvaluate, Modulus) {
    // 10 mod 3 = 1
    std::vector<Value> postfix = {Value(Number, 10.0), Value(Number, 3.0), Value(Mod)};
    EXPECT_DOUBLE_EQ(evalPostfix(postfix), 1.0);
}

// ============================================================
// Unary operations
// ============================================================

TEST(EvaluatorEvaluate, SinDegrees) {
    // sin(90) = 1.0
    auto t1 = Value(Number, 90.0);
    auto t2 = Value(Sin);
    t2.argc = 1;
    std::vector<Value> postfix = {t1, t2};
    EXPECT_NEAR(evalPostfix(postfix), 1.0, 1e-9);
}

TEST(EvaluatorEvaluate, CosDegrees) {
    // cos(0) = 1.0
    auto t1 = Value(Number, 0.0);
    auto t2 = Value(Cos);
    t2.argc = 1;
    std::vector<Value> postfix = {t1, t2};
    EXPECT_NEAR(evalPostfix(postfix), 1.0, 1e-9);
}

TEST(EvaluatorEvaluate, SqrtValue) {
    // sqrt(16) = 4.0
    auto t1 = Value(Number, 16.0);
    auto t2 = Value(Sqrt);
    t2.argc = 1;
    std::vector<Value> postfix = {t1, t2};
    EXPECT_DOUBLE_EQ(evalPostfix(postfix), 4.0);
}

TEST(EvaluatorEvaluate, Factorial) {
    // 5! = 120
    std::vector<Value> postfix = {Value(Number, 5.0), Value(Fact)};
    EXPECT_DOUBLE_EQ(evalPostfix(postfix), 120.0);
}

TEST(EvaluatorEvaluate, Percent) {
    // 80% = 0.8
    std::vector<Value> postfix = {Value(Number, 80.0), Value(Percent)};
    EXPECT_DOUBLE_EQ(evalPostfix(postfix), 0.8);
}

// ============================================================
// Variadic operations
// ============================================================

TEST(EvaluatorEvaluate, MinVariadic) {
    // min(5, 3, 8) = 3.0
    Value minTok(Min);
    minTok.argc = 3;
    std::vector<Value> postfix = {Value(Number, 5.0), Value(Number, 3.0), Value(Number, 8.0), minTok};
    EXPECT_DOUBLE_EQ(evalPostfix(postfix), 3.0);
}

TEST(EvaluatorEvaluate, MaxVariadic) {
    // max(5, 3, 8) = 8.0
    Value maxTok(Max);
    maxTok.argc = 3;
    std::vector<Value> postfix = {Value(Number, 5.0), Value(Number, 3.0), Value(Number, 8.0), maxTok};
    EXPECT_DOUBLE_EQ(evalPostfix(postfix), 8.0);
}

// ============================================================
// Multi-step operations
// ============================================================

TEST(EvaluatorEvaluate, MultiStepExpression) {
    // 2 + 3 * 4 = 14  →  postfix: 2 3 4 * +
    std::vector<Value> postfix = {
        Value(Number, 2.0),
        Value(Number, 3.0),
        Value(Number, 4.0),
        Value(Mul),
        Value(Add)
    };
    EXPECT_DOUBLE_EQ(evalPostfix(postfix), 14.0);
}

TEST(EvaluatorEvaluate, ParenthesizedExpression) {
    // (2 + 3) * 4 = 20  →  postfix: 2 3 + 4 *
    std::vector<Value> postfix = {
        Value(Number, 2.0),
        Value(Number, 3.0),
        Value(Add),
        Value(Number, 4.0),
        Value(Mul)
    };
    EXPECT_DOUBLE_EQ(evalPostfix(postfix), 20.0);
}

TEST(EvaluatorEvaluate, SingleNumber) {
    // Just a number → returns itself
    std::vector<Value> postfix = {Value(Number, 42.0)};
    EXPECT_DOUBLE_EQ(evalPostfix(postfix), 42.0);
}

TEST(EvaluatorEvaluate, PiConstant) {
    // PI → returns M_PI
    std::vector<Value> postfix = {Value(PI, M_PI)};
    EXPECT_DOUBLE_EQ(evalPostfix(postfix), M_PI);
}

// ============================================================
// Log, Ln, LogBase, Abs
// ============================================================

TEST(EvaluatorEvaluate, LogValue) {
    // log(100) = 2.0
    auto t1 = Value(Number, 100.0);
    auto t2 = Value(Log);
    t2.argc = 1;
    std::vector<Value> postfix = {t1, t2};
    EXPECT_DOUBLE_EQ(evalPostfix(postfix), 2.0);
}

TEST(EvaluatorEvaluate, LnValue) {
    // ln(1) = 0.0
    auto t1 = Value(Number, 1.0);
    auto t2 = Value(Ln);
    t2.argc = 1;
    std::vector<Value> postfix = {t1, t2};
    EXPECT_DOUBLE_EQ(evalPostfix(postfix), 0.0);
}

TEST(EvaluatorEvaluate, AbsNegative) {
    // abs(-5) = 5.0
    auto t1 = Value(Number, -5.0);
    auto t2 = Value(Abs);
    t2.argc = 1;
    std::vector<Value> postfix = {t1, t2};
    EXPECT_DOUBLE_EQ(evalPostfix(postfix), 5.0);
}

TEST(EvaluatorEvaluate, AbsPositive) {
    // abs(3) = 3.0
    auto t1 = Value(Number, 3.0);
    auto t2 = Value(Abs);
    t2.argc = 1;
    std::vector<Value> postfix = {t1, t2};
    EXPECT_DOUBLE_EQ(evalPostfix(postfix), 3.0);
}

TEST(EvaluatorEvaluate, LogBaseValue) {
    // logbase(8, 2) = 3.0
    auto t1 = Value(Number, 8.0);
    auto t2 = Value(Number, 2.0);
    auto t3 = Value(LogBase);
    t3.argc = 2;
    std::vector<Value> postfix = {t1, t2, t3};
    EXPECT_NEAR(evalPostfix(postfix), 3.0, 1e-9);
}

// ============================================================
// Edge cases — argc validation
// ============================================================

TEST(EvaluatorEvaluate, FunctionWithZeroArgcThrows) {
    // sin() with argc=0 should throw
    auto t = Value(Sin);
    t.argc = 0;
    std::vector<Value> postfix = {Value(Number, 1.0), t};
    EXPECT_THROW(evalPostfix(postfix), std::runtime_error);
}

TEST(EvaluatorEvaluate, UnaryFuncWithTooManyArgsThrows) {
    // log with argc=2 should throw (it's unary)
    auto t = Value(Log);
    t.argc = 2;
    std::vector<Value> postfix = {Value(Number, 1.0), Value(Number, 2.0), t};
    EXPECT_THROW(evalPostfix(postfix), std::runtime_error);
}

TEST(EvaluatorEvaluate, LogBaseWrongArgcThrows) {
    // logbase with argc=1 should throw (needs exactly 2)
    auto t = Value(LogBase);
    t.argc = 1;
    std::vector<Value> postfix = {Value(Number, 8.0), t};
    EXPECT_THROW(evalPostfix(postfix), std::runtime_error);
}

// ============================================================
// Matrix operations via Evaluator
// ============================================================

static Matrix<double> evalPostfixMatrix(const std::vector<Value>& tokens) {
    return std::get<Matrix<double>>(Evaluator::Evaluate(tokens));
}

TEST(EvaluatorEvaluate, MatrixMultiplication) {
    // [1 2; 3 4] * [5; 6] = [17; 39]
    Matrix<double> a = {{1, 2}, {3, 4}};
    Matrix<double> b(std::vector<std::vector<double>>{{5}, {6}});
    std::vector<Value> postfix = {Value(MatrixT, a), Value(MatrixT, b), Value(Mul)};
    auto result = evalPostfixMatrix(postfix);
    EXPECT_NEAR(result[0][0], 17.0, 1e-9);
    EXPECT_NEAR(result[1][0], 39.0, 1e-9);
}

TEST(EvaluatorEvaluate, MatrixScalarMultiply) {
    // [1 2; 3 4] * 2 = [2 4; 6 8]
    Matrix<double> a = {{1, 2}, {3, 4}};
    std::vector<Value> postfix = {Value(MatrixT, a), Value(Number, 2.0), Value(Mul)};
    auto result = evalPostfixMatrix(postfix);
    EXPECT_DOUBLE_EQ(result[0][0], 2.0);
    EXPECT_DOUBLE_EQ(result[1][1], 8.0);
}

TEST(EvaluatorEvaluate, MatrixScalarDivide) {
    // [10 20; 30 40] / 10 = [1 2; 3 4]
    Matrix<double> a = {{10, 20}, {30, 40}};
    std::vector<Value> postfix = {Value(MatrixT, a), Value(Number, 10.0), Value(Div)};
    auto result = evalPostfixMatrix(postfix);
    EXPECT_DOUBLE_EQ(result[0][0], 1.0);
    EXPECT_DOUBLE_EQ(result[1][1], 4.0);
}

TEST(EvaluatorEvaluate, MatrixPower) {
    // [1 0; 0 1] ^ 3 = identity (identity to any power is identity)
    Matrix<double> id = {{1, 0}, {0, 1}};
    std::vector<Value> postfix = {Value(MatrixT, id), Value(Number, 3.0), Value(Pow)};
    auto result = evalPostfixMatrix(postfix);
    EXPECT_DOUBLE_EQ(result[0][0], 1.0);
    EXPECT_DOUBLE_EQ(result[0][1], 0.0);
    EXPECT_DOUBLE_EQ(result[1][0], 0.0);
    EXPECT_DOUBLE_EQ(result[1][1], 1.0);
}

TEST(EvaluatorEvaluate, MatrixInvMul) {
    // [2 1; 5 3] \ [8; 21] = [3; 2]
    Matrix<double> A = {{2, 1}, {5, 3}};
    Matrix<double> b(std::vector<std::vector<double>>{{8}, {21}});
    std::vector<Value> postfix = {Value(MatrixT, A), Value(MatrixT, b), Value(InvMul)};
    auto result = evalPostfixMatrix(postfix);
    EXPECT_NEAR(result[0][0], 3.0, 1e-9);
    EXPECT_NEAR(result[1][0], 2.0, 1e-9);
}

TEST(EvaluatorEvaluate, InvMulScalarThrows) {
    // InvMul between scalars should throw
    std::vector<Value> postfix = {Value(Number, 2.0), Value(Number, 3.0), Value(InvMul)};
    EXPECT_THROW(evalPostfix(postfix), std::invalid_argument);
}

TEST(EvaluatorEvaluate, ModulusMatrixThrows) {
    // Modulus with matrix should throw
    Matrix<double> a = {{1, 2}, {3, 4}};
    std::vector<Value> postfix = {Value(MatrixT, a), Value(Number, 2.0), Value(Mod)};
    EXPECT_THROW(Evaluator::Evaluate(postfix), std::invalid_argument);
}
