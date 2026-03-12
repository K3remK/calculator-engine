#include <gtest/gtest.h>
#include <cmath>
#include "../src/Token.hpp"

// ============================================================
// Token::IsOperator()
// ============================================================

TEST(TokenIsOperator, ReturnsTrueForArithmeticOperators) {
    EXPECT_TRUE(Value(Add).IsOperator());
    EXPECT_TRUE(Value(Sub).IsOperator());
    EXPECT_TRUE(Value(Mul).IsOperator());
    EXPECT_TRUE(Value(Div).IsOperator());
    EXPECT_TRUE(Value(Pow).IsOperator());
}

TEST(TokenIsOperator, ReturnsFalseForNonOperators) {
    EXPECT_FALSE(Value(Number, 5.0).IsOperator());
    EXPECT_FALSE(Value(Sin).IsOperator());
    EXPECT_FALSE(Value(LeftParen).IsOperator());
    EXPECT_FALSE(Value(RightParen).IsOperator());
    EXPECT_FALSE(Value(Comma).IsOperator());
    EXPECT_FALSE(Value(PI, M_PI).IsOperator());
    EXPECT_FALSE(Value(Fact).IsOperator());
    EXPECT_FALSE(Value(Percent).IsOperator());
}

// ============================================================
// Token::IsFunction()
// ============================================================

TEST(TokenIsFunction, ReturnsTrueForMathFunctions) {
    EXPECT_TRUE(Value(Sin).IsFunction());
    EXPECT_TRUE(Value(Cos).IsFunction());
    EXPECT_TRUE(Value(Tan).IsFunction());
    EXPECT_TRUE(Value(Cot).IsFunction());
    EXPECT_TRUE(Value(Sqrt).IsFunction());
    EXPECT_TRUE(Value(Min).IsFunction());
    EXPECT_TRUE(Value(Max).IsFunction());
    EXPECT_TRUE(Value(Log).IsFunction());
    EXPECT_TRUE(Value(Ln).IsFunction());
    EXPECT_TRUE(Value(LogBase).IsFunction());
    EXPECT_TRUE(Value(Abs).IsFunction());
}

TEST(TokenIsFunction, ReturnsFalseForNonFunctions) {
    EXPECT_FALSE(Value(Add).IsFunction());
    EXPECT_FALSE(Value(Number, 5.0).IsFunction());
    EXPECT_FALSE(Value(LeftParen).IsFunction());
    EXPECT_FALSE(Value(Fact).IsFunction());
    EXPECT_FALSE(Value(Percent).IsFunction());
    EXPECT_FALSE(Value(UnaryMinus).IsFunction());
    EXPECT_FALSE(Value(UnaryPlus).IsFunction());
}

// ============================================================
// Token::GetOperatorInfo() — precedence & associativity
// ============================================================

TEST(TokenGetOperatorInfo, PrecedenceLevels) {
    // Precedence 0: Add, Sub
    EXPECT_EQ(Value(Add).GetOperatorInfo().precedence, 0);
    EXPECT_EQ(Value(Sub).GetOperatorInfo().precedence, 0);
    // Precedence 1: Mul, Div, Mod
    EXPECT_EQ(Value(Mul).GetOperatorInfo().precedence, 1);
    EXPECT_EQ(Value(Div).GetOperatorInfo().precedence, 1);
    EXPECT_EQ(Value(Mod).GetOperatorInfo().precedence, 1);
    // Precedence 2: Pow
    EXPECT_EQ(Value(Pow).GetOperatorInfo().precedence, 2);
    // Precedence 3: functions & postfix
    EXPECT_EQ(Value(Sin).GetOperatorInfo().precedence, 3);
    EXPECT_EQ(Value(Fact).GetOperatorInfo().precedence, 3);
    EXPECT_EQ(Value(Percent).GetOperatorInfo().precedence, 3);
    EXPECT_EQ(Value(Log).GetOperatorInfo().precedence, 3);
    EXPECT_EQ(Value(Ln).GetOperatorInfo().precedence, 3);
    EXPECT_EQ(Value(LogBase).GetOperatorInfo().precedence, 3);
    EXPECT_EQ(Value(Abs).GetOperatorInfo().precedence, 3);
}

TEST(TokenGetOperatorInfo, Associativity) {
    EXPECT_TRUE(Value(Add).GetOperatorInfo().isLeftAssociative);
    EXPECT_TRUE(Value(Sub).GetOperatorInfo().isLeftAssociative);
    EXPECT_TRUE(Value(Mul).GetOperatorInfo().isLeftAssociative);
    EXPECT_TRUE(Value(Div).GetOperatorInfo().isLeftAssociative);
    EXPECT_FALSE(Value(Pow).GetOperatorInfo().isLeftAssociative);  // right-associative
}

// ============================================================
// Token::toString()
// ============================================================

TEST(TokenToString, OperatorSymbols) {
    EXPECT_EQ(Value(Add).toString(), "+");
    EXPECT_EQ(Value(Sub).toString(), "-");
    EXPECT_EQ(Value(Mul).toString(), "*");
    EXPECT_EQ(Value(Div).toString(), "/");
    EXPECT_EQ(Value(Pow).toString(), "^");
    EXPECT_EQ(Value(Mod).toString(), "mod");
    EXPECT_EQ(Value(Fact).toString(), "!");
    EXPECT_EQ(Value(Percent).toString(), "%");
}

TEST(TokenToString, FunctionNames) {
    EXPECT_EQ(Value(Sin).toString(), "sin");
    EXPECT_EQ(Value(Cos).toString(), "cos");
    EXPECT_EQ(Value(Tan).toString(), "tan");
    EXPECT_EQ(Value(Cot).toString(), "cot");
    EXPECT_EQ(Value(Sqrt).toString(), "sqrt");
    EXPECT_EQ(Value(Min).toString(), "min");
    EXPECT_EQ(Value(Max).toString(), "max");
    EXPECT_EQ(Value(Log).toString(), "log");
    EXPECT_EQ(Value(Ln).toString(), "ln");
    EXPECT_EQ(Value(LogBase).toString(), "logbase");
    EXPECT_EQ(Value(Abs).toString(), "abs");
}

TEST(TokenToString, SpecialTokens) {
    EXPECT_EQ(Value(LeftParen).toString(), "(");
    EXPECT_EQ(Value(RightParen).toString(), ")");
    EXPECT_EQ(Value(Comma).toString(), ",");
}

TEST(TokenToString, NumberIncludesValue) {
    std::string result = Value(Number, 42.0).toString();
    EXPECT_EQ(result, "42");
}


