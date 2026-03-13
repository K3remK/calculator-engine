#include <gtest/gtest.h>
#include <cmath>
#include "../src/Lexer.hpp"
#include "../src/Parser.hpp"
#include "../src/Evaluator.hpp"

// Helper: full pipeline evaluation
static double evaluate(const std::string& expr) {
    std::unordered_map<std::string, Token> vars;
    auto tokens = Lexer::Tokenize(expr, vars);
    auto postfix = Parser::ToPostfix(tokens);
    return std::get<double>(Evaluator::Evaluate(postfix, vars).data);
}

// Helper: full pipeline evaluation with a shared variable map
static double evaluate(const std::string& expr, std::unordered_map<std::string, Token>& vars) {
    auto tokens = Lexer::Tokenize(expr, vars);
    auto postfix = Parser::ToPostfix(tokens);
    return std::get<double>(Evaluator::Evaluate(postfix, vars).data);
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

TEST(Integration, ParenthesesOverridePrecedence) {
    EXPECT_DOUBLE_EQ(evaluate("(2 + 3) * 4"), 20.0);
}

TEST(Integration, DivisionWithParens) {
    EXPECT_DOUBLE_EQ(evaluate("10 / (2 * 5)"), 1.0);
}

TEST(Integration, NestedParentheses) {
    EXPECT_DOUBLE_EQ(evaluate("((1 + 2) * 3) - 4"), 5.0);
}

// ============================================================
// Modulus
// ============================================================

TEST(Integration, ModulusOperator) {
    EXPECT_DOUBLE_EQ(evaluate("100 mod 13"), 9.0);
}

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
    std::unordered_map<std::string, Token> vars;
    auto tokens = Lexer::Tokenize(expr, vars);
    auto postfix = Parser::ToPostfix(tokens);
    auto result = Evaluator::Evaluate(postfix, vars);
    return *std::get<std::unique_ptr<Matrix<double>>>(std::move(result.data));
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
    std::unordered_map<std::string, Token> vars;
    auto tokens = Lexer::Tokenize("[1 2 3; 4 5 6; 7 8 9] \\ [1; 2; 3]", vars);
    auto postfix = Parser::ToPostfix(tokens);
    EXPECT_THROW(Evaluator::Evaluate(postfix, vars), std::runtime_error);
}

TEST(Integration, NonSquareMatrixSolveThrows) {
    // [1 2 3; 4 5 6] \ [1; 2] — not square
    std::unordered_map<std::string, Token> vars;
    auto tokens = Lexer::Tokenize("[1 2 3; 4 5 6] \\ [1; 2]", vars);
    auto postfix = Parser::ToPostfix(tokens);
    EXPECT_THROW(Evaluator::Evaluate(postfix, vars), std::runtime_error);
}

TEST(Integration, MatrixSizeMismatchThrows) {
    // [1 2; 3 4] + [1 2 3; 4 5 6] — size mismatch
    std::unordered_map<std::string, Token> vars;
    auto tokens = Lexer::Tokenize("[1 2; 3 4] + [1 2 3; 4 5 6]", vars);
    auto postfix = Parser::ToPostfix(tokens);
    EXPECT_THROW(Evaluator::Evaluate(postfix, vars), std::runtime_error);
}

TEST(Integration, MatrixWithExprInEntries) {
    // Matrix entries can contain sub-expressions
    // [2^2 1; 1 2^2] should be [4 1; 1 4]
    auto result = evaluateMatrix("[2^2 1; 1 2^2]");
    EXPECT_DOUBLE_EQ(result[0][0], 4.0);
    EXPECT_DOUBLE_EQ(result[1][1], 4.0);
}

// ============================================================
// Variable support — end-to-end
// ============================================================

TEST(Integration, SimpleVariableAssignment) {
    // x = 5 should store 5 and return the variable
    std::unordered_map<std::string, Token> vars;
    auto tokens = Lexer::Tokenize("x = 5", vars);
    auto postfix = Parser::ToPostfix(tokens);
    auto result = Evaluator::Evaluate(postfix, vars);
    EXPECT_DOUBLE_EQ(std::get<double>(result.data), 5.0);
    ASSERT_TRUE(vars.contains("x"));
    EXPECT_DOUBLE_EQ(std::get<double>(vars.at("x").data), 5.0);
}

TEST(Integration, VariableUsageAfterAssignment) {
    // x = 5, then x + 3 = 8
    std::unordered_map<std::string, Token> vars;
    evaluate("x = 5", vars);
    EXPECT_DOUBLE_EQ(evaluate("x + 3", vars), 8.0);
}

TEST(Integration, VariableReassignment) {
    // x = 5, then x = 10, then x + 1 = 11
    std::unordered_map<std::string, Token> vars;
    evaluate("x = 5", vars);
    evaluate("x = 10", vars);
    EXPECT_DOUBLE_EQ(evaluate("x + 1", vars), 11.0);
}

TEST(Integration, VariableInComplexExpression) {
    // x = 4, then sqrt(x) + x^2 = 2 + 16 = 18
    std::unordered_map<std::string, Token> vars;
    evaluate("x = 4", vars);
    EXPECT_DOUBLE_EQ(evaluate("sqrt(x) + x^2", vars), 18.0);
}

TEST(Integration, MultipleVariables) {
    // x = 3, y = 4, then x + y = 7
    std::unordered_map<std::string, Token> vars;
    evaluate("x = 3", vars);
    evaluate("y = 4", vars);
    EXPECT_DOUBLE_EQ(evaluate("x + y", vars), 7.0);
}

TEST(Integration, VariableMultiplication) {
    // x = 6, y = 7, then x * y = 42
    std::unordered_map<std::string, Token> vars;
    evaluate("x = 6", vars);
    evaluate("y = 7", vars);
    EXPECT_DOUBLE_EQ(evaluate("x * y", vars), 42.0);
}

TEST(Integration, VariableInFunction) {
    // x = 90, then sin(x) = 1
    std::unordered_map<std::string, Token> vars;
    evaluate("x = 90", vars);
    EXPECT_NEAR(evaluate("sin(x)", vars), 1.0, 1e-9);
}

TEST(Integration, VariableAssignmentWithExpression) {
    // x = 2 + 3, then x should be 5
    std::unordered_map<std::string, Token> vars;
    evaluate("x = 2 + 3", vars);
    EXPECT_DOUBLE_EQ(evaluate("x", vars), 5.0);
}

TEST(Integration, VariableAssignedFromOtherVariable) {
    // x = 5, y = x + 1, then y should be 6
    std::unordered_map<std::string, Token> vars;
    evaluate("x = 5", vars);
    evaluate("y = x + 1", vars);
    EXPECT_DOUBLE_EQ(evaluate("y", vars), 6.0);
}

TEST(Integration, UndefinedVariableThrows) {
    // Using a variable that doesn't exist should throw
    std::unordered_map<std::string, Token> vars;
    auto tokens = Lexer::Tokenize("x + 3", vars);
    auto postfix = Parser::ToPostfix(tokens);
    EXPECT_THROW(Evaluator::Evaluate(postfix, vars), std::runtime_error);
}

TEST(Integration, VariableWithNegation) {
    // x = 5, then -x + 10 = 5
    std::unordered_map<std::string, Token> vars;
    evaluate("x = 5", vars);
    EXPECT_DOUBLE_EQ(evaluate("-x + 10", vars), 5.0);
}

TEST(Integration, VariableWithPiAndEuler) {
    // r = 2, then pi * r^2 ≈ 4π
    std::unordered_map<std::string, Token> vars;
    evaluate("r = 2", vars);
    EXPECT_NEAR(evaluate("pi * r^2", vars), M_PI * 4.0, 1e-9);
}
