#include <gtest/gtest.h>
#include <cmath>
#include "../src/Token.hpp"
#include "../src/Evaluator.hpp"

// Helper: build a postfix token vector and evaluate it
// This lets us test the Evaluator in isolation (without Lexer/Parser)

static double evalPostfix(const std::vector<Token>& tokens) {
    std::unordered_map<std::string, Token> vars;
    return std::get<double>(Evaluator::Evaluate(tokens, vars).data);
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
    std::vector<Token> postfix = {std::move(t1), std::move(t2)};
    EXPECT_NEAR(evalPostfix(postfix), 1.0, 1e-9);
}

TEST(EvaluatorEvaluate, CosDegrees) {
    // cos(0) = 1.0
    auto t1 = Token(Number, 0.0);
    auto t2 = Token(Cos);
    t2.argc = 1;
    std::vector<Token> postfix = {std::move(t1), std::move(t2)};
    EXPECT_NEAR(evalPostfix(postfix), 1.0, 1e-9);
}

TEST(EvaluatorEvaluate, SqrtValue) {
    // sqrt(16) = 4.0
    auto t1 = Token(Number, 16.0);
    auto t2 = Token(Sqrt);
    t2.argc = 1;
    std::vector<Token> postfix = {std::move(t1), std::move(t2)};
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
    std::vector<Token> postfix;
    postfix.push_back(Token(Number, 5.0));
    postfix.push_back(Token(Number, 3.0));
    postfix.push_back(Token(Number, 8.0));
    postfix.push_back(std::move(minTok));
    EXPECT_DOUBLE_EQ(evalPostfix(postfix), 3.0);
}

TEST(EvaluatorEvaluate, MaxVariadic) {
    // max(5, 3, 8) = 8.0
    Token maxTok(Max);
    maxTok.argc = 3;
    std::vector<Token> postfix;
    postfix.push_back(Token(Number, 5.0));
    postfix.push_back(Token(Number, 3.0));
    postfix.push_back(Token(Number, 8.0));
    postfix.push_back(std::move(maxTok));
    EXPECT_DOUBLE_EQ(evalPostfix(postfix), 8.0);
}

// ============================================================
// Multi-step operations
// ============================================================

TEST(EvaluatorEvaluate, MultiStepExpression) {
    // 2 + 3 * 4 = 14  →  postfix: 2 3 4 * +
    std::vector<Token> postfix;
    postfix.push_back(Token(Number, 2.0));
    postfix.push_back(Token(Number, 3.0));
    postfix.push_back(Token(Number, 4.0));
    postfix.push_back(Token(Mul));
    postfix.push_back(Token(Add));
    EXPECT_DOUBLE_EQ(evalPostfix(postfix), 14.0);
}

TEST(EvaluatorEvaluate, ParenthesizedExpression) {
    // (2 + 3) * 4 = 20  →  postfix: 2 3 + 4 *
    std::vector<Token> postfix;
    postfix.push_back(Token(Number, 2.0));
    postfix.push_back(Token(Number, 3.0));
    postfix.push_back(Token(Add));
    postfix.push_back(Token(Number, 4.0));
    postfix.push_back(Token(Mul));
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
    std::vector<Token> postfix = {std::move(t1), std::move(t2)};
    EXPECT_DOUBLE_EQ(evalPostfix(postfix), 2.0);
}

TEST(EvaluatorEvaluate, LnValue) {
    // ln(1) = 0.0
    auto t1 = Token(Number, 1.0);
    auto t2 = Token(Ln);
    t2.argc = 1;
    std::vector<Token> postfix = {std::move(t1), std::move(t2)};
    EXPECT_DOUBLE_EQ(evalPostfix(postfix), 0.0);
}

TEST(EvaluatorEvaluate, AbsNegative) {
    // abs(-5) = 5.0
    auto t1 = Token(Number, -5.0);
    auto t2 = Token(Abs);
    t2.argc = 1;
    std::vector<Token> postfix = {std::move(t1), std::move(t2)};
    EXPECT_DOUBLE_EQ(evalPostfix(postfix), 5.0);
}

TEST(EvaluatorEvaluate, AbsPositive) {
    // abs(3) = 3.0
    auto t1 = Token(Number, 3.0);
    auto t2 = Token(Abs);
    t2.argc = 1;
    std::vector<Token> postfix = {std::move(t1), std::move(t2)};
    EXPECT_DOUBLE_EQ(evalPostfix(postfix), 3.0);
}

TEST(EvaluatorEvaluate, LogBaseValue) {
    // logbase(8, 2) = 3.0
    auto t1 = Token(Number, 8.0);
    auto t2 = Token(Number, 2.0);
    auto t3 = Token(LogBase);
    t3.argc = 2;
    std::vector<Token> postfix;
    postfix.push_back(std::move(t1));
    postfix.push_back(std::move(t2));
    postfix.push_back(std::move(t3));
    EXPECT_NEAR(evalPostfix(postfix), 3.0, 1e-9);
}

// ============================================================
// Edge cases — argc validation
// ============================================================

TEST(EvaluatorEvaluate, FunctionWithZeroArgcThrows) {
    // sin() with argc=0 should throw
    auto t = Token(Sin);
    t.argc = 0;
    std::vector<Token> postfix;
    postfix.push_back(Token(Number, 1.0));
    postfix.push_back(std::move(t));
    EXPECT_THROW(evalPostfix(postfix), std::runtime_error);
}

TEST(EvaluatorEvaluate, UnaryFuncWithTooManyArgsThrows) {
    // log with argc=2 should throw (it's unary)
    auto t = Token(Log);
    t.argc = 2;
    std::vector<Token> postfix;
    postfix.push_back(Token(Number, 1.0));
    postfix.push_back(Token(Number, 2.0));
    postfix.push_back(std::move(t));
    EXPECT_THROW(evalPostfix(postfix), std::runtime_error);
}

TEST(EvaluatorEvaluate, LogBaseWrongArgcThrows) {
    // logbase with argc=1 should throw (needs exactly 2)
    auto t = Token(LogBase);
    t.argc = 1;
    std::vector<Token> postfix;
    postfix.push_back(Token(Number, 8.0));
    postfix.push_back(std::move(t));
    EXPECT_THROW(evalPostfix(postfix), std::runtime_error);
}

// ============================================================
// Matrix operations via Evaluator
// ============================================================

static Matrix<double> evalPostfixMatrix(const std::vector<Token>& tokens) {
    std::unordered_map<std::string, Token> vars;
    auto result = Evaluator::Evaluate(tokens, vars);
    return *std::get<std::unique_ptr<Matrix<double>>>(std::move(result.data));
}

TEST(EvaluatorEvaluate, MatrixMultiplication) {
    // [1 2; 3 4] * [5; 6] = [17; 39]
    Matrix<double> a = {{1, 2}, {3, 4}};
    Matrix<double> b(std::vector<std::vector<double>>{{5}, {6}});
    std::vector<Token> postfix;
    postfix.push_back(Token(MatrixT, std::make_unique<Matrix<double>>(a)));
    postfix.push_back(Token(MatrixT, std::make_unique<Matrix<double>>(b)));
    postfix.push_back(Token(Mul));
    auto result = evalPostfixMatrix(postfix);
    EXPECT_NEAR(result[0][0], 17.0, 1e-9);
    EXPECT_NEAR(result[1][0], 39.0, 1e-9);
}

TEST(EvaluatorEvaluate, MatrixScalarMultiply) {
    // [1 2; 3 4] * 2 = [2 4; 6 8]
    Matrix<double> a = {{1, 2}, {3, 4}};
    std::vector<Token> postfix;
    postfix.push_back(Token(MatrixT, std::make_unique<Matrix<double>>(a)));
    postfix.push_back(Token(Number, 2.0));
    postfix.push_back(Token(Mul));
    auto result = evalPostfixMatrix(postfix);
    EXPECT_DOUBLE_EQ(result[0][0], 2.0);
    EXPECT_DOUBLE_EQ(result[1][1], 8.0);
}

TEST(EvaluatorEvaluate, MatrixScalarDivide) {
    // [10 20; 30 40] / 10 = [1 2; 3 4]
    Matrix<double> a = {{10, 20}, {30, 40}};
    std::vector<Token> postfix;
    postfix.push_back(Token(MatrixT, std::make_unique<Matrix<double>>(a)));
    postfix.push_back(Token(Number, 10.0));
    postfix.push_back(Token(Div));
    auto result = evalPostfixMatrix(postfix);
    EXPECT_DOUBLE_EQ(result[0][0], 1.0);
    EXPECT_DOUBLE_EQ(result[1][1], 4.0);
}

TEST(EvaluatorEvaluate, MatrixPower) {
    // [1 0; 0 1] ^ 3 = identity (identity to any power is identity)
    Matrix<double> id = {{1, 0}, {0, 1}};
    std::vector<Token> postfix;
    postfix.push_back(Token(MatrixT, std::make_unique<Matrix<double>>(id)));
    postfix.push_back(Token(Number, 3.0));
    postfix.push_back(Token(Pow));
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
    std::vector<Token> postfix;
    postfix.push_back(Token(MatrixT, std::make_unique<Matrix<double>>(A)));
    postfix.push_back(Token(MatrixT, std::make_unique<Matrix<double>>(b)));
    postfix.push_back(Token(InvMul));
    auto result = evalPostfixMatrix(postfix);
    EXPECT_NEAR(result[0][0], 3.0, 1e-9);
    EXPECT_NEAR(result[1][0], 2.0, 1e-9);
}

TEST(EvaluatorEvaluate, InvMulScalarThrows) {
    // InvMul between scalars should throw
    std::vector<Token> postfix = {Token(Number, 2.0), Token(Number, 3.0), Token(InvMul)};
    EXPECT_THROW(evalPostfix(postfix), std::invalid_argument);
}

TEST(EvaluatorEvaluate, ModulusMatrixThrows) {
    // Modulus with matrix should throw
    Matrix<double> a = {{1, 2}, {3, 4}};
    std::vector<Token> postfix;
    postfix.push_back(Token(MatrixT, std::make_unique<Matrix<double>>(a)));
    postfix.push_back(Token(Number, 2.0));
    postfix.push_back(Token(Mod));
    std::unordered_map<std::string, Token> vars;
    EXPECT_THROW(Evaluator::Evaluate(postfix, vars), std::invalid_argument);
}

// ============================================================
// Variable operations via Evaluator
// ============================================================

TEST(EvaluatorEvaluate, VariableAssignment) {
    // x = 5  →  postfix: Variable(x) 5 Assignment
    std::unordered_map<std::string, Token> vars;
    std::vector<Token> postfix;
    postfix.push_back(Token("x"));
    postfix.push_back(Token(Number, 5.0));
    postfix.push_back(Token(Assignment));
    auto result = Evaluator::Evaluate(postfix, vars);
    EXPECT_EQ(result.type, Variable);
    EXPECT_DOUBLE_EQ(std::get<double>(result.data), 5.0);
    // Variable should now be stored
    ASSERT_TRUE(vars.contains("x"));
    EXPECT_DOUBLE_EQ(std::get<double>(vars.at("x").data), 5.0);
}

TEST(EvaluatorEvaluate, VariableLookup) {
    // Evaluate x + 3 when x = 10
    std::unordered_map<std::string, Token> vars;
    vars.insert_or_assign("x", Token("x", 10.0));

    std::vector<Token> postfix;
    postfix.push_back(Token("x"));
    postfix.push_back(Token(Number, 3.0));
    postfix.push_back(Token(Add));
    auto result = Evaluator::Evaluate(postfix, vars);
    EXPECT_DOUBLE_EQ(std::get<double>(result.data), 13.0);
}

TEST(EvaluatorEvaluate, UndefinedVariableThrows) {
    // Using a variable that doesn't exist should throw
    std::unordered_map<std::string, Token> vars;
    std::vector<Token> postfix;
    postfix.push_back(Token("undefined_var"));
    postfix.push_back(Token(Number, 3.0));
    postfix.push_back(Token(Add));
    EXPECT_THROW(Evaluator::Evaluate(postfix, vars), std::runtime_error);
}

TEST(EvaluatorEvaluate, VariableReassignment) {
    // Assign x = 5, then reassign x = 10
    std::unordered_map<std::string, Token> vars;

    // First assignment: x = 5
    std::vector<Token> postfix1;
    postfix1.push_back(Token("x"));
    postfix1.push_back(Token(Number, 5.0));
    postfix1.push_back(Token(Assignment));
    Evaluator::Evaluate(postfix1, vars);
    EXPECT_DOUBLE_EQ(std::get<double>(vars.at("x").data), 5.0);

    // Reassignment: x = 10
    std::vector<Token> postfix2;
    postfix2.push_back(Token("x"));
    postfix2.push_back(Token(Number, 10.0));
    postfix2.push_back(Token(Assignment));
    Evaluator::Evaluate(postfix2, vars);
    EXPECT_DOUBLE_EQ(std::get<double>(vars.at("x").data), 10.0);
}

TEST(EvaluatorEvaluate, MultipleVariables) {
    // x = 3, y = 4, then evaluate x + y
    std::unordered_map<std::string, Token> vars;
    vars.insert_or_assign("x", Token("x", 3.0));
    vars.insert_or_assign("y", Token("y", 4.0));

    std::vector<Token> postfix;
    postfix.push_back(Token("x"));
    postfix.push_back(Token("y"));
    postfix.push_back(Token(Add));
    auto result = Evaluator::Evaluate(postfix, vars);
    EXPECT_DOUBLE_EQ(std::get<double>(result.data), 7.0);
}
