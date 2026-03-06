#include <gtest/gtest.h>
#include <cmath>
#include "../src/Lexer.hpp"
#include "../src/Parser.hpp"
#include "../src/Evaluator.hpp"

// Helper: full pipeline evaluation
static double evaluate(const std::string& expr) {
    auto tokens = Lexer::Tokenize(expr);
    auto postfix = Parser::ToPostfix(tokens);
    return std::get<double>(Evaluator::Evaluate(postfix));
}

// ============================================================
// Basic arithmetic — end-to-end
// ============================================================

TEST(Integration, BasicAddition) {
    EXPECT_DOUBLE_EQ(evaluate("2 + 3"), 5.0);
}

TEST(Integration, PrecedenceMulOverAdd) {
    EXPECT_DOUBLE_EQ(evaluate("2 + 3 * 4"), 14.0);
}

TEST(Integration, SubMulAdd) {
    EXPECT_DOUBLE_EQ(evaluate("10 - 2 * 3 + 4"), 8.0);
}

TEST(Integration, DivisionAndMul) {
    EXPECT_DOUBLE_EQ(evaluate("10 / 2 * 5"), 25.0);
}

// ============================================================
// Parenthesized expressions
// ============================================================

// BUG DETECTED: Parser treats '*' after ')' as unary minus → result is -4 instead of 20.
// This test asserts the CORRECT expected result and will FAIL until the bug is fixed.
TEST(Integration, ParenthesesOverridePrecedence) {
    EXPECT_DOUBLE_EQ(evaluate("(2 + 3) * 4"), 20.0);
}

TEST(Integration, DivisionWithParens) {
    EXPECT_DOUBLE_EQ(evaluate("10 / (2 * 5)"), 1.0);
}

// BUG DETECTED: Same parser bug — operators after ')' become negation.
// This test asserts the CORRECT expected result and will FAIL until the bug is fixed.
TEST(Integration, NestedParentheses) {
    EXPECT_DOUBLE_EQ(evaluate("((1 + 2) * 3) - 4"), 5.0);
}

// ============================================================
// Modulus
// ============================================================

TEST(Integration, ModulusOperator) {
    EXPECT_DOUBLE_EQ(evaluate("100 mod 13"), 9.0);
}

// BUG DETECTED: Parser bug — '*' after ')' is lost.
// This test asserts the CORRECT expected result and will FAIL until the bug is fixed.
TEST(Integration, ParenExpressionWithMod) {
    // (2 + 3) * 4 mod 7 = 20 mod 7 = 6
    EXPECT_DOUBLE_EQ(evaluate("(2 + 3) * 4 mod 7"), 6.0);
}

// ============================================================
// Trigonometric functions (degrees)
// ============================================================

TEST(Integration, SinAndCos) {
    // sin(0) + cos(0) = 0 + 1 = 1
    EXPECT_NEAR(evaluate("sin(0) + cos(0)"), 1.0, 1e-9);
}

// BUG DETECTED: Parser treats '+' after ')' as unary minus → 100 becomes -100.
// This test asserts the CORRECT expected result and will FAIL until the bug is fixed.
TEST(Integration, SinPlusConstant) {
    // sin(5) + 100 ≈ 100.087
    double result = evaluate("sin(5) + 100");
    EXPECT_NEAR(result, std::sin(5.0 * M_PI / 180.0) + 100.0, 1e-9);
}

// ============================================================
// PI constant
// ============================================================

TEST(Integration, PiExpression) {
    // 3 * pi / 2
    double result = evaluate("3*pi/2");
    EXPECT_NEAR(result, 3.0 * M_PI / 2.0, 1e-9);
}

// ============================================================
// Power
// ============================================================

TEST(Integration, SimplePower) {
    EXPECT_DOUBLE_EQ(evaluate("2 ^ 10"), 1024.0);
}

TEST(Integration, Pythagorean) {
    // sqrt(3^2 + 4^2) = sqrt(9 + 16) = sqrt(25) = 5
    EXPECT_NEAR(evaluate("sqrt(3^2 + 4^2)"), 5.0, 1e-9);
}

// ============================================================
// Negation
// ============================================================

TEST(Integration, DoubleNegation) {
    // 3 - (-3) + 1 = 7
    EXPECT_DOUBLE_EQ(evaluate("3--3+1"), 7.0);
}

// ============================================================
// Factorial and Percent
// ============================================================

TEST(Integration, FactorialExpression) {
    // (80 * 20%)! = (80 * 0.2)! = 16! = 20922789888000
    double result = evaluate("(80*20%)!");
    EXPECT_DOUBLE_EQ(result, 20922789888000.0);
}

// ============================================================
// Nested functions
// ============================================================

TEST(Integration, NestedMinMax) {
    // sqrt(min(100, 200, 6, 29, max(5, 2, 4, 1)))
    // max(5,2,4,1) = 5 → min(100, 200, 6, 29, 5) = 5 → sqrt(5)
    double result = evaluate("sqrt(min(100, 200, 6, 29, max(5, 2, 4, 1)))");
    EXPECT_NEAR(result, std::sqrt(5.0), 1e-9);
}

// BUG DETECTED: Parser treats '/' after ')' as unary minus → 5 becomes -5.
// This test asserts the CORRECT expected result and will FAIL until the bug is fixed.
TEST(Integration, SqrtDivision) {
    // sqrt(16) / 5 = 4 / 5 = 0.8
    EXPECT_DOUBLE_EQ(evaluate("sqrt(16) / 5"), 0.8);
}

// ============================================================
// Single function calls
// ============================================================

TEST(Integration, MinSingleArg) {
    EXPECT_DOUBLE_EQ(evaluate("min(100)"), 100.0);
}

TEST(Integration, MaxSingleArg) {
    EXPECT_DOUBLE_EQ(evaluate("max(1)"), 1.0);
}

// ============================================================
// Log, Ln, LogBase, Abs — end-to-end
// ============================================================

TEST(Integration, LogBase10) {
    // log(100) = 2
    EXPECT_DOUBLE_EQ(evaluate("log(100)"), 2.0);
}

TEST(Integration, LogOfOne) {
    // log(1) = 0
    EXPECT_DOUBLE_EQ(evaluate("log(1)"), 0.0);
}

TEST(Integration, LnOfOne) {
    // ln(1) = 0
    EXPECT_DOUBLE_EQ(evaluate("ln(1)"), 0.0);
}

TEST(Integration, AbsNegative) {
    // abs(-5) with unary minus inside
    EXPECT_DOUBLE_EQ(evaluate("abs(-5)"), 5.0);
}

TEST(Integration, AbsPositive) {
    EXPECT_DOUBLE_EQ(evaluate("abs(5)"), 5.0);
}

TEST(Integration, LogBaseBinary) {
    // logbase(8, 2) = 3
    EXPECT_NEAR(evaluate("logbase(8, 2)"), 3.0, 1e-9);
}

TEST(Integration, LogBaseDecimal) {
    // logbase(100, 10) = 2
    EXPECT_NEAR(evaluate("logbase(100, 10)"), 2.0, 1e-9);
}

// ============================================================
// Nested and complex expressions with new functions
// ============================================================

TEST(Integration, AbsOfLogSmallNumber) {
    // abs(log(0.01)) = abs(-2) = 2
    EXPECT_NEAR(evaluate("abs(log(0.01))"), 2.0, 1e-9);
}

TEST(Integration, SqrtOfAbs) {
    // sqrt(abs(-16)) = sqrt(16) = 4
    EXPECT_DOUBLE_EQ(evaluate("sqrt(abs(-16))"), 4.0);
}

TEST(Integration, LogPlusLn) {
    // log(100) + ln(1) = 2 + 0 = 2
    EXPECT_DOUBLE_EQ(evaluate("log(100) + ln(1)"), 2.0);
}

TEST(Integration, AbsTimesConstant) {
    // abs(-3) * 2 = 3 * 2 = 6
    EXPECT_DOUBLE_EQ(evaluate("abs(-3) * 2"), 6.0);
}

TEST(Integration, LogBaseInExpression) {
    // logbase(8, 2) + 1 = 3 + 1 = 4
    EXPECT_NEAR(evaluate("logbase(8, 2) + 1"), 4.0, 1e-9);
}

// ============================================================
// Matrix operations — end-to-end
// ============================================================

// Helper: full pipeline evaluation returning a matrix
static Matrix<double> evaluateMatrix(const std::string& expr) {
    auto tokens = Lexer::Tokenize(expr);
    auto postfix = Parser::ToPostfix(tokens);
    return std::get<Matrix<double>>(Evaluator::Evaluate(postfix));
}

TEST(Integration, MatrixMultiplication) {
    // [1 2; 3 4] * [5; 6] = [17; 39]
    auto result = evaluateMatrix("[1 2; 3 4] * [5; 6]");
    EXPECT_EQ(result.GetM(), 2u);
    EXPECT_EQ(result.GetN(), 1u);
    EXPECT_NEAR(result[0][0], 17.0, 1e-9);
    EXPECT_NEAR(result[1][0], 39.0, 1e-9);
}

TEST(Integration, MatrixScalarDivision) {
    // [10 20; 30 40] / 2 = [5 10; 15 20]
    auto result = evaluateMatrix("[10 20; 30 40] / 2");
    EXPECT_DOUBLE_EQ(result[0][0], 5.0);
    EXPECT_DOUBLE_EQ(result[0][1], 10.0);
    EXPECT_DOUBLE_EQ(result[1][0], 15.0);
    EXPECT_DOUBLE_EQ(result[1][1], 20.0);
}

TEST(Integration, MatrixPowerIdentity) {
    // [1 0; 0 1] ^ 3 = identity
    auto result = evaluateMatrix("[1 0; 0 1] ^ 3");
    EXPECT_DOUBLE_EQ(result[0][0], 1.0);
    EXPECT_DOUBLE_EQ(result[0][1], 0.0);
    EXPECT_DOUBLE_EQ(result[1][0], 0.0);
    EXPECT_DOUBLE_EQ(result[1][1], 1.0);
}

TEST(Integration, MatrixAddition) {
    // [1 2; 3 4] + [5 6; 7 8] = [6 8; 10 12]
    auto result = evaluateMatrix("[1 2; 3 4] + [5 6; 7 8]");
    EXPECT_DOUBLE_EQ(result[0][0], 6.0);
    EXPECT_DOUBLE_EQ(result[0][1], 8.0);
    EXPECT_DOUBLE_EQ(result[1][0], 10.0);
    EXPECT_DOUBLE_EQ(result[1][1], 12.0);
}

TEST(Integration, MatrixSubtraction) {
    // [5 6; 7 8] - [1 2; 3 4] = [4 4; 4 4]
    auto result = evaluateMatrix("[5 6; 7 8] - [1 2; 3 4]");
    EXPECT_DOUBLE_EQ(result[0][0], 4.0);
    EXPECT_DOUBLE_EQ(result[1][1], 4.0);
}

TEST(Integration, LinearSolve) {
    // [2 1; 5 3] \ [8; 21] = [3; 2]
    auto result = evaluateMatrix("[2 1; 5 3] \\ [8; 21]");
    EXPECT_NEAR(result[0][0], 3.0, 1e-9);
    EXPECT_NEAR(result[1][0], 2.0, 1e-9);
}

TEST(Integration, LinearSolve3x3) {
    // [2 0 0; 0 3 0; 0 0 4] \ [8; 9; 8] = [4; 3; 2]
    auto result = evaluateMatrix("[2 0 0; 0 3 0; 0 0 4] \\ [8; 9; 8]");
    EXPECT_NEAR(result[0][0], 4.0, 1e-9);
    EXPECT_NEAR(result[1][0], 3.0, 1e-9);
    EXPECT_NEAR(result[2][0], 2.0, 1e-9);
}

TEST(Integration, SingularMatrixSolveThrows) {
    // [1 2 3; 4 5 6; 7 8 9] \ [1; 2; 3] — singular matrix
    auto tokens = Lexer::Tokenize("[1 2 3; 4 5 6; 7 8 9] \\ [1; 2; 3]");
    auto postfix = Parser::ToPostfix(tokens);
    EXPECT_THROW(Evaluator::Evaluate(postfix), std::runtime_error);
}

TEST(Integration, NonSquareMatrixSolveThrows) {
    // [1 2 3; 4 5 6] \ [1; 2] — not square
    auto tokens = Lexer::Tokenize("[1 2 3; 4 5 6] \\ [1; 2]");
    auto postfix = Parser::ToPostfix(tokens);
    EXPECT_THROW(Evaluator::Evaluate(postfix), std::runtime_error);
}

TEST(Integration, MatrixSizeMismatchThrows) {
    // [1 2; 3 4] + [1 2 3; 4 5 6] — size mismatch
    auto tokens = Lexer::Tokenize("[1 2; 3 4] + [1 2 3; 4 5 6]");
    auto postfix = Parser::ToPostfix(tokens);
    EXPECT_THROW(Evaluator::Evaluate(postfix), std::runtime_error);
}

TEST(Integration, MatrixWithExprInEntries) {
    // Matrix entries can contain sub-expressions
    // [2^2 1; 1 2^2] should be [4 1; 1 4]
    auto result = evaluateMatrix("[2^2 1; 1 2^2]");
    EXPECT_DOUBLE_EQ(result[0][0], 4.0);
    EXPECT_DOUBLE_EQ(result[1][1], 4.0);
}
