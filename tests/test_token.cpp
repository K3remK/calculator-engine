#include <gtest/gtest.h>
#include <cmath>
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
    EXPECT_TRUE(Token(Log).IsFunction());
    EXPECT_TRUE(Token(Ln).IsFunction());
    EXPECT_TRUE(Token(LogBase).IsFunction());
    EXPECT_TRUE(Token(Abs).IsFunction());
}

TEST(TokenIsFunction, ReturnsFalseForNonFunctions) {
    EXPECT_FALSE(Token(Add).IsFunction());
    EXPECT_FALSE(Token(Number, 5.0).IsFunction());
    EXPECT_FALSE(Token(LeftParen).IsFunction());
    EXPECT_FALSE(Token(Fact).IsFunction());
    EXPECT_FALSE(Token(Percent).IsFunction());
    EXPECT_FALSE(Token(UnaryMinus).IsFunction());
    EXPECT_FALSE(Token(UnaryPlus).IsFunction());
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
    EXPECT_EQ(Token(Log).GetOperatorInfo().precedence, 3);
    EXPECT_EQ(Token(Ln).GetOperatorInfo().precedence, 3);
    EXPECT_EQ(Token(LogBase).GetOperatorInfo().precedence, 3);
    EXPECT_EQ(Token(Abs).GetOperatorInfo().precedence, 3);
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
    EXPECT_EQ(Token(Log).toString(), "log");
    EXPECT_EQ(Token(Ln).toString(), "ln");
    EXPECT_EQ(Token(LogBase).toString(), "logbase");
    EXPECT_EQ(Token(Abs).toString(), "abs");
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


