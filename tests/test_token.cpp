#include <gtest/gtest.h>
#include "../src/Token.hpp"

// ============================================================
// Token::IsOperator()
// ============================================================

TEST(TokenIsOperator, ReturnsTrueForArithmeticOperators) {
    EXPECT_TRUE(Token(Add).IsOperator());
    EXPECT_TRUE(Token(Sub).IsOperator());
    EXPECT_TRUE(Token(Mul).IsOperator());
    EXPECT_TRUE(Token(Div).IsOperator());
    EXPECT_TRUE(Token(Pow).IsOperator());
}

TEST(TokenIsOperator, ReturnsFalseForNonOperators) {
    EXPECT_FALSE(Token(Number, 5.0).IsOperator());
    EXPECT_FALSE(Token(Sin).IsOperator());
    EXPECT_FALSE(Token(LeftParen).IsOperator());
    EXPECT_FALSE(Token(RightParen).IsOperator());
    EXPECT_FALSE(Token(Comma).IsOperator());
    EXPECT_FALSE(Token(PI, M_PI).IsOperator());
    EXPECT_FALSE(Token(Fact).IsOperator());
    EXPECT_FALSE(Token(Percent).IsOperator());
}

// ============================================================
// Token::IsFunction()
// ============================================================

TEST(TokenIsFunction, ReturnsTrueForMathFunctions) {
    EXPECT_TRUE(Token(Sin).IsFunction());
    EXPECT_TRUE(Token(Cos).IsFunction());
    EXPECT_TRUE(Token(Tan).IsFunction());
    EXPECT_TRUE(Token(Cot).IsFunction());
    EXPECT_TRUE(Token(Sqrt).IsFunction());
    EXPECT_TRUE(Token(Min).IsFunction());
    EXPECT_TRUE(Token(Max).IsFunction());
}

TEST(TokenIsFunction, ReturnsFalseForNonFunctions) {
    EXPECT_FALSE(Token(Add).IsFunction());
    EXPECT_FALSE(Token(Number, 5.0).IsFunction());
    EXPECT_FALSE(Token(LeftParen).IsFunction());
    EXPECT_FALSE(Token(Fact).IsFunction());
    EXPECT_FALSE(Token(Percent).IsFunction());
}

// ============================================================
// Token::GetOperatorInfo() — precedence & associativity
// ============================================================

TEST(TokenGetOperatorInfo, PrecedenceLevels) {
    // Precedence 0: Add, Sub
    EXPECT_EQ(Token(Add).GetOperatorInfo().precedence, 0);
    EXPECT_EQ(Token(Sub).GetOperatorInfo().precedence, 0);
    // Precedence 1: Mul, Div, Mod
    EXPECT_EQ(Token(Mul).GetOperatorInfo().precedence, 1);
    EXPECT_EQ(Token(Div).GetOperatorInfo().precedence, 1);
    EXPECT_EQ(Token(Mod).GetOperatorInfo().precedence, 1);
    // Precedence 2: Pow
    EXPECT_EQ(Token(Pow).GetOperatorInfo().precedence, 2);
    // Precedence 3: functions & postfix
    EXPECT_EQ(Token(Sin).GetOperatorInfo().precedence, 3);
    EXPECT_EQ(Token(Fact).GetOperatorInfo().precedence, 3);
    EXPECT_EQ(Token(Percent).GetOperatorInfo().precedence, 3);
}

TEST(TokenGetOperatorInfo, Associativity) {
    EXPECT_TRUE(Token(Add).GetOperatorInfo().isLeftAssociative);
    EXPECT_TRUE(Token(Sub).GetOperatorInfo().isLeftAssociative);
    EXPECT_TRUE(Token(Mul).GetOperatorInfo().isLeftAssociative);
    EXPECT_TRUE(Token(Div).GetOperatorInfo().isLeftAssociative);
    EXPECT_FALSE(Token(Pow).GetOperatorInfo().isLeftAssociative);  // right-associative
}

// ============================================================
// Token::toString()
// ============================================================

TEST(TokenToString, OperatorSymbols) {
    EXPECT_EQ(Token(Add).toString(), "+");
    EXPECT_EQ(Token(Sub).toString(), "-");
    EXPECT_EQ(Token(Mul).toString(), "*");
    EXPECT_EQ(Token(Div).toString(), "/");
    EXPECT_EQ(Token(Pow).toString(), "^");
    EXPECT_EQ(Token(Mod).toString(), "mod");
    EXPECT_EQ(Token(Fact).toString(), "!");
    EXPECT_EQ(Token(Percent).toString(), "%");
}

TEST(TokenToString, FunctionNames) {
    EXPECT_EQ(Token(Sin).toString(), "sin");
    EXPECT_EQ(Token(Cos).toString(), "cos");
    EXPECT_EQ(Token(Tan).toString(), "tan");
    EXPECT_EQ(Token(Cot).toString(), "cot");
    EXPECT_EQ(Token(Sqrt).toString(), "sqrt");
    EXPECT_EQ(Token(Min).toString(), "min");
    EXPECT_EQ(Token(Max).toString(), "max");
}

TEST(TokenToString, SpecialTokens) {
    EXPECT_EQ(Token(LeftParen).toString(), "(");
    EXPECT_EQ(Token(RightParen).toString(), ")");
    EXPECT_EQ(Token(Comma).toString(), ",");
}

TEST(TokenToString, NumberIncludesValue) {
    std::string result = Token(Number, 42.0).toString();
    EXPECT_EQ(result, "42");
}

// ============================================================
// operatorMap lambdas — Arithmetic
// ============================================================

TEST(OperatorMapActions, BinaryArithmetic) {
    auto add = std::get<BinaryFunc>(operatorMap.at(Add).action);
    EXPECT_DOUBLE_EQ(add(2.0, 3.0), 5.0);

    auto sub = std::get<BinaryFunc>(operatorMap.at(Sub).action);
    EXPECT_DOUBLE_EQ(sub(10.0, 4.0), 6.0);

    auto mul = std::get<BinaryFunc>(operatorMap.at(Mul).action);
    EXPECT_DOUBLE_EQ(mul(3.0, 7.0), 21.0);

    auto div = std::get<BinaryFunc>(operatorMap.at(Div).action);
    EXPECT_DOUBLE_EQ(div(20.0, 4.0), 5.0);

    auto pw = std::get<BinaryFunc>(operatorMap.at(Pow).action);
    EXPECT_DOUBLE_EQ(pw(2.0, 10.0), 1024.0);
}

TEST(OperatorMapActions, Modulus) {
    auto mod = std::get<BinaryFunc>(operatorMap.at(Mod).action);
    EXPECT_DOUBLE_EQ(mod(10.0, 3.0), 1.0);
    EXPECT_DOUBLE_EQ(mod(100.0, 13.0), 9.0);
}

TEST(OperatorMapActions, ModulusByZeroThrows) {
    auto mod = std::get<BinaryFunc>(operatorMap.at(Mod).action);
    EXPECT_THROW(mod(10.0, 0.0), std::invalid_argument);
}

// ============================================================
// operatorMap lambdas — Trigonometric (degrees)
// ============================================================

TEST(OperatorMapActions, SinDegrees) {
    auto sinFn = std::get<UnaryFunc>(operatorMap.at(Sin).action);
    EXPECT_NEAR(sinFn(0.0), 0.0, 1e-9);
    EXPECT_NEAR(sinFn(30.0), 0.5, 1e-9);
    EXPECT_NEAR(sinFn(90.0), 1.0, 1e-9);
}

TEST(OperatorMapActions, CosDegrees) {
    auto cosFn = std::get<UnaryFunc>(operatorMap.at(Cos).action);
    EXPECT_NEAR(cosFn(0.0), 1.0, 1e-9);
    EXPECT_NEAR(cosFn(60.0), 0.5, 1e-9);
    EXPECT_NEAR(cosFn(90.0), 0.0, 1e-9);
}

TEST(OperatorMapActions, TanDegrees) {
    auto tanFn = std::get<UnaryFunc>(operatorMap.at(Tan).action);
    EXPECT_NEAR(tanFn(0.0), 0.0, 1e-9);
    EXPECT_NEAR(tanFn(45.0), 1.0, 1e-9);
}

TEST(OperatorMapActions, CotDegrees) {
    auto cotFn = std::get<UnaryFunc>(operatorMap.at(Cot).action);
    EXPECT_NEAR(cotFn(45.0), 1.0, 1e-9);
}

TEST(OperatorMapActions, CotAt90DegreesThrows) {
    auto cotFn = std::get<UnaryFunc>(operatorMap.at(Cot).action);
    // cot(90) = 1/tan(90), tan(90) ≈ very large but not exactly 0 in floating point.
    // However, cot(0) would cause division by zero since tan(0) = 0.
    EXPECT_THROW(cotFn(0.0), std::invalid_argument);
}

// ============================================================
// operatorMap lambdas — Sqrt, Factorial, Percent
// ============================================================

TEST(OperatorMapActions, Sqrt) {
    auto sqrtFn = std::get<UnaryFunc>(operatorMap.at(Sqrt).action);
    EXPECT_DOUBLE_EQ(sqrtFn(16.0), 4.0);
    EXPECT_DOUBLE_EQ(sqrtFn(0.0), 0.0);
    EXPECT_DOUBLE_EQ(sqrtFn(1.0), 1.0);
}

TEST(OperatorMapActions, Factorial) {
    auto factFn = std::get<UnaryFunc>(operatorMap.at(Fact).action);
    EXPECT_DOUBLE_EQ(factFn(0.0), 1.0);
    EXPECT_DOUBLE_EQ(factFn(1.0), 1.0);
    EXPECT_DOUBLE_EQ(factFn(5.0), 120.0);
    EXPECT_DOUBLE_EQ(factFn(10.0), 3628800.0);
}

TEST(OperatorMapActions, FactorialNonIntegerThrows) {
    auto factFn = std::get<UnaryFunc>(operatorMap.at(Fact).action);
    EXPECT_THROW(factFn(3.5), std::invalid_argument);
}

TEST(OperatorMapActions, Percent) {
    auto pctFn = std::get<UnaryFunc>(operatorMap.at(Percent).action);
    EXPECT_DOUBLE_EQ(pctFn(100.0), 1.0);
    EXPECT_DOUBLE_EQ(pctFn(50.0), 0.5);
    EXPECT_DOUBLE_EQ(pctFn(0.0), 0.0);
}

// ============================================================
// operatorMap lambdas — Variadic Min/Max
// ============================================================

TEST(OperatorMapActions, MinVariadic) {
    auto minFn = std::get<VariadicFunc>(operatorMap.at(Min).action);
    EXPECT_DOUBLE_EQ(minFn({5.0, 3.0, 8.0, 1.0}), 1.0);
    EXPECT_DOUBLE_EQ(minFn({42.0}), 42.0);
}

TEST(OperatorMapActions, MinEmptyThrows) {
    auto minFn = std::get<VariadicFunc>(operatorMap.at(Min).action);
    EXPECT_THROW(minFn({}), std::invalid_argument);
}

TEST(OperatorMapActions, MaxVariadic) {
    auto maxFn = std::get<VariadicFunc>(operatorMap.at(Max).action);
    EXPECT_DOUBLE_EQ(maxFn({5.0, 3.0, 8.0, 1.0}), 8.0);
    EXPECT_DOUBLE_EQ(maxFn({42.0}), 42.0);
}

TEST(OperatorMapActions, MaxEmptyThrows) {
    auto maxFn = std::get<VariadicFunc>(operatorMap.at(Max).action);
    EXPECT_THROW(maxFn({}), std::invalid_argument);
}
