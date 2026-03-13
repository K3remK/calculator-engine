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

TEST(TokenIsOperator, VariableIsNotOperator) {
    EXPECT_FALSE(Token("x").IsOperator());
}

TEST(TokenIsOperator, AssignmentIsNotArithmeticOperator) {
    // Assignment is in InfixOperators but not in IsOperator()
    EXPECT_FALSE(Token(Assignment).IsOperator());
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

TEST(TokenIsFunction, VariableIsNotFunction) {
    EXPECT_FALSE(Token("myVar").IsFunction());
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

TEST(TokenGetOperatorInfo, AssignmentPrecedence) {
    // Assignment has lowest precedence (-1)
    EXPECT_EQ(Token(Assignment).GetOperatorInfo().precedence, -1);
    EXPECT_TRUE(Token(Assignment).GetOperatorInfo().isLeftAssociative);
    EXPECT_EQ(Token(Assignment).GetOperatorInfo().expectedArgc, 2);
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

TEST(TokenToString, AssignmentSymbol) {
    EXPECT_EQ(Token(Assignment).toString(), "=");
}

TEST(TokenToString, EulerConstant) {
    EXPECT_EQ(Token(Euler).toString(), "e");
}

TEST(TokenToString, PiConstant) {
    EXPECT_EQ(Token(PI, M_PI).toString(), "π");
}

// ============================================================
// Variable Token construction and toString()
// ============================================================

TEST(TokenVariable, ConstructionFromString) {
    Token t("myVar");
    EXPECT_EQ(t.type, Variable);
    ASSERT_NE(t.variable_name, nullptr);
    EXPECT_EQ(*t.variable_name, "myVar");
}

TEST(TokenVariable, ConstructionWithDoubleValue) {
    Token t("x", 5.0);
    EXPECT_EQ(t.type, Variable);
    ASSERT_NE(t.variable_name, nullptr);
    EXPECT_EQ(*t.variable_name, "x");
    EXPECT_DOUBLE_EQ(std::get<double>(t.data), 5.0);
}

TEST(TokenVariable, ToStringIncludesNameAndValue) {
    Token t("x", 42.0);
    std::string s = t.toString();
    // Variable toString format: "name(value)"
    EXPECT_NE(s.find("x"), std::string::npos);
    EXPECT_NE(s.find("42"), std::string::npos);
}

TEST(TokenVariable, NonOperatorTokenHasNullVariableName) {
    Token t(Number, 5.0);
    EXPECT_EQ(t.variable_name, nullptr);
}

// ============================================================
// Token copy and move semantics with unique_ptr
// ============================================================

TEST(TokenCopyMove, CopyConstructsDouble) {
    Token original(Number, 42.0);
    Token copy(original);
    EXPECT_EQ(copy.type, Number);
    EXPECT_DOUBLE_EQ(std::get<double>(copy.data), 42.0);
}

TEST(TokenCopyMove, CopyConstructsVariable) {
    Token original("x", 5.0);
    Token copy(original);
    EXPECT_EQ(copy.type, Variable);
    ASSERT_NE(copy.variable_name, nullptr);
    EXPECT_EQ(*copy.variable_name, "x");
    EXPECT_DOUBLE_EQ(std::get<double>(copy.data), 5.0);
    // Ensure deep copy — different pointers
    EXPECT_NE(copy.variable_name.get(), original.variable_name.get());
}

TEST(TokenCopyMove, CopyConstructsMatrix) {
    Matrix<double> m = {{1, 2}, {3, 4}};
    Token original(MatrixT, std::make_unique<Matrix<double>>(m));
    Token copy(original);
    EXPECT_EQ(copy.type, MatrixT);
    auto& copyMatrix = *std::get<std::unique_ptr<Matrix<double>>>(copy.data);
    EXPECT_DOUBLE_EQ(copyMatrix[0][0], 1.0);
    EXPECT_DOUBLE_EQ(copyMatrix[1][1], 4.0);
}

TEST(TokenCopyMove, MoveConstructsVariable) {
    Token original("y", 7.0);
    Token moved(std::move(original));
    EXPECT_EQ(moved.type, Variable);
    ASSERT_NE(moved.variable_name, nullptr);
    EXPECT_EQ(*moved.variable_name, "y");
    EXPECT_DOUBLE_EQ(std::get<double>(moved.data), 7.0);
}

TEST(TokenCopyMove, CopyAssignmentOperator) {
    Token original("x", 10.0);
    Token target(Number, 0.0);
    target = original;
    EXPECT_EQ(target.type, Variable);
    ASSERT_NE(target.variable_name, nullptr);
    EXPECT_EQ(*target.variable_name, "x");
    EXPECT_DOUBLE_EQ(std::get<double>(target.data), 10.0);
}
