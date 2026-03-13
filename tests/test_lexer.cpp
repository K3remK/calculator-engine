#include <gtest/gtest.h>
#include <cmath>
#include "../src/Lexer.hpp"

// Helper: collect token types from a tokenized string
static std::vector<TokenType> tokenTypes(std::string_view input) {
    std::unordered_map<std::string, Token> vars;
    auto tokens = Lexer::Tokenize(input, vars);
    std::vector<TokenType> types;
    types.reserve(tokens.size());
    for (const auto& t : tokens)
        types.push_back(t.type);
    return types;
}

// ============================================================
// Number tokenization
// ============================================================

TEST(LexerTokenize, SingleInteger) {
    std::unordered_map<std::string, Token> vars;
    auto tokens = Lexer::Tokenize("42", vars);
    ASSERT_EQ(tokens.size(), 1);
    EXPECT_EQ(tokens[0].type, Number);
    EXPECT_DOUBLE_EQ(std::get<double>(tokens[0].data), 42.0);
}

TEST(LexerTokenize, DecimalNumber) {
    std::unordered_map<std::string, Token> vars;
    auto tokens = Lexer::Tokenize("3.14", vars);
    ASSERT_EQ(tokens.size(), 1);
    EXPECT_EQ(tokens[0].type, Number);
    EXPECT_NEAR(std::get<double>(tokens[0].data), 3.14, 1e-9);
}

TEST(LexerTokenize, MultipleDigitInteger) {
    std::unordered_map<std::string, Token> vars;
    auto tokens = Lexer::Tokenize("12345", vars);
    ASSERT_EQ(tokens.size(), 1);
    EXPECT_DOUBLE_EQ(std::get<double>(tokens[0].data), 12345.0);
}

TEST(LexerTokenize, ZeroValue) {
    std::unordered_map<std::string, Token> vars;
    auto tokens = Lexer::Tokenize("0", vars);
    ASSERT_EQ(tokens.size(), 1);
    EXPECT_DOUBLE_EQ(std::get<double>(tokens[0].data), 0.0);
}

// ============================================================
// Single operator tokens
// ============================================================

TEST(LexerTokenize, SingleOperators) {
    EXPECT_EQ(tokenTypes("+"), std::vector<TokenType>({Add}));
    EXPECT_EQ(tokenTypes("-"), std::vector<TokenType>({Sub}));
    EXPECT_EQ(tokenTypes("*"), std::vector<TokenType>({Mul}));
    EXPECT_EQ(tokenTypes("/"), std::vector<TokenType>({Div}));
    EXPECT_EQ(tokenTypes("^"), std::vector<TokenType>({Pow}));
}

// ============================================================
// Parentheses and comma
// ============================================================

TEST(LexerTokenize, Parentheses) {
    EXPECT_EQ(tokenTypes("("), std::vector<TokenType>({LeftParen}));
    EXPECT_EQ(tokenTypes(")"), std::vector<TokenType>({RightParen}));
}

TEST(LexerTokenize, Comma) {
    EXPECT_EQ(tokenTypes(","), std::vector<TokenType>({Comma}));
}

// ============================================================
// Postfix operators
// ============================================================

TEST(LexerTokenize, FactorialOperator) {
    EXPECT_EQ(tokenTypes("!"), std::vector<TokenType>({Fact}));
}

TEST(LexerTokenize, PercentOperator) {
    EXPECT_EQ(tokenTypes("%"), std::vector<TokenType>({Percent}));
}

// ============================================================
// Identifier tokenization (keywords)
// ============================================================

TEST(LexerTokenize, TrigFunctions) {
    EXPECT_EQ(tokenTypes("sin"), std::vector<TokenType>({Sin}));
    EXPECT_EQ(tokenTypes("cos"), std::vector<TokenType>({Cos}));
    EXPECT_EQ(tokenTypes("tan"), std::vector<TokenType>({Tan}));
    EXPECT_EQ(tokenTypes("cot"), std::vector<TokenType>({Cot}));
}

TEST(LexerTokenize, OtherFunctions) {
    EXPECT_EQ(tokenTypes("sqrt"), std::vector<TokenType>({Sqrt}));
    EXPECT_EQ(tokenTypes("min"), std::vector<TokenType>({Min}));
    EXPECT_EQ(tokenTypes("max"), std::vector<TokenType>({Max}));
    EXPECT_EQ(tokenTypes("mod"), std::vector<TokenType>({Mod}));
}

TEST(LexerTokenize, LogLnLogBaseAbsFunctions) {
    EXPECT_EQ(tokenTypes("log"), std::vector<TokenType>({Log}));
    EXPECT_EQ(tokenTypes("ln"), std::vector<TokenType>({Ln}));
    EXPECT_EQ(tokenTypes("logbase"), std::vector<TokenType>({LogBase}));
    EXPECT_EQ(tokenTypes("abs"), std::vector<TokenType>({Abs}));
}

TEST(LexerTokenize, PiConstant) {
    std::unordered_map<std::string, Token> vars;
    auto tokens = Lexer::Tokenize("pi", vars);
    ASSERT_EQ(tokens.size(), 1);
    EXPECT_EQ(tokens[0].type, PI);
    EXPECT_DOUBLE_EQ(std::get<double>(tokens[0].data), M_PI);
}

// ============================================================
// Whitespace handling
// ============================================================

TEST(LexerTokenize, WhitespaceIsSkipped) {
    std::unordered_map<std::string, Token> vars;
    auto tokens = Lexer::Tokenize("  1  +  2  ", vars);
    ASSERT_EQ(tokens.size(), 3);
    EXPECT_EQ(tokens[0].type, Number);
    EXPECT_EQ(tokens[1].type, Add);
    EXPECT_EQ(tokens[2].type, Number);
}

// ============================================================
// Full expressions
// ============================================================

TEST(LexerTokenize, FullExpression) {
    auto types = tokenTypes("sin(30)+2*3");
    std::vector<TokenType> expected = {Sin, LeftParen, Number, RightParen, Add, Number, Mul, Number};
    EXPECT_EQ(types, expected);
}

TEST(LexerTokenize, ExpressionWithMod) {
    auto types = tokenTypes("100 mod 13");
    std::vector<TokenType> expected = {Number, Mod, Number};
    EXPECT_EQ(types, expected);
}

TEST(LexerTokenize, ExpressionWithComma) {
    auto types = tokenTypes("min(1,2,3)");
    std::vector<TokenType> expected = {Min, LeftParen, Number, Comma, Number, Comma, Number, RightParen};
    EXPECT_EQ(types, expected);
}

TEST(LexerTokenize, LogExpression) {
    auto types = tokenTypes("log(100)");
    std::vector<TokenType> expected = {Log, LeftParen, Number, RightParen};
    EXPECT_EQ(types, expected);
}

TEST(LexerTokenize, AbsExpression) {
    auto types = tokenTypes("abs(5)");
    std::vector<TokenType> expected = {Abs, LeftParen, Number, RightParen};
    EXPECT_EQ(types, expected);
}

TEST(LexerTokenize, LogBaseExpression) {
    auto types = tokenTypes("logbase(8,2)");
    std::vector<TokenType> expected = {LogBase, LeftParen, Number, Comma, Number, RightParen};
    EXPECT_EQ(types, expected);
}

TEST(LexerTokenize, LnExpression) {
    auto types = tokenTypes("ln(1)");
    std::vector<TokenType> expected = {Ln, LeftParen, Number, RightParen};
    EXPECT_EQ(types, expected);
}

// ============================================================
// Error cases
// ============================================================

TEST(LexerTokenize, InvalidCharacterThrows) {
    std::unordered_map<std::string, Token> vars;
    EXPECT_THROW(Lexer::Tokenize("@", vars), std::invalid_argument);
    EXPECT_THROW(Lexer::Tokenize("2 & 3", vars), std::invalid_argument);
}

// ============================================================
// Variable tokenization
// ============================================================

TEST(LexerTokenize, UnknownIdentifierProducesVariable) {
    // Unknown identifiers are now Variable tokens, not errors
    std::unordered_map<std::string, Token> vars;
    auto tokens = Lexer::Tokenize("foo", vars);
    ASSERT_EQ(tokens.size(), 1);
    EXPECT_EQ(tokens[0].type, Variable);
    ASSERT_NE(tokens[0].variable_name, nullptr);
    EXPECT_EQ(*tokens[0].variable_name, "foo");
}

TEST(LexerTokenize, MultipleVariableNames) {
    std::unordered_map<std::string, Token> vars;
    auto tokens = Lexer::Tokenize("xyz", vars);
    ASSERT_EQ(tokens.size(), 1);
    EXPECT_EQ(tokens[0].type, Variable);
    EXPECT_EQ(*tokens[0].variable_name, "xyz");
}

TEST(LexerTokenize, VariableInExpression) {
    // x + 3  →  Variable, Add, Number
    auto types = tokenTypes("x + 3");
    std::vector<TokenType> expected = {Variable, Add, Number};
    EXPECT_EQ(types, expected);
}

TEST(LexerTokenize, AssignmentExpression) {
    // x = 5  →  Variable, Assignment, Number
    auto types = tokenTypes("x = 5");
    std::vector<TokenType> expected = {Variable, Assignment, Number};
    EXPECT_EQ(types, expected);
}

TEST(LexerTokenize, VariableWithFunction) {
    // sin(x)  →  Sin, LeftParen, Variable, RightParen
    auto types = tokenTypes("sin(x)");
    std::vector<TokenType> expected = {Sin, LeftParen, Variable, RightParen};
    EXPECT_EQ(types, expected);
}

TEST(LexerTokenize, MultipleVariablesInExpression) {
    // x + y * z  →  Variable, Add, Variable, Mul, Variable
    auto types = tokenTypes("x + y * z");
    std::vector<TokenType> expected = {Variable, Add, Variable, Mul, Variable};
    EXPECT_EQ(types, expected);
}

TEST(LexerTokenize, AssignmentOperator) {
    EXPECT_EQ(tokenTypes("="), std::vector<TokenType>({Assignment}));
}

// ============================================================
// Matrix literal tokenization
// ============================================================

TEST(LexerTokenize, MatrixLiteralSpaces) {
    std::unordered_map<std::string, Token> vars;
    auto tokens = Lexer::Tokenize("[1 2; 3 4]", vars);
    ASSERT_EQ(tokens.size(), 1u);
    EXPECT_EQ(tokens[0].type, MatrixT);
    auto& m = *std::get<std::unique_ptr<Matrix<double>>>(tokens[0].data);
    EXPECT_EQ(m.GetM(), 2u);
    EXPECT_EQ(m.GetN(), 2u);
    EXPECT_DOUBLE_EQ(m[0][0], 1.0);
    EXPECT_DOUBLE_EQ(m[0][1], 2.0);
    EXPECT_DOUBLE_EQ(m[1][0], 3.0);
    EXPECT_DOUBLE_EQ(m[1][1], 4.0);
}

TEST(LexerTokenize, MatrixLiteralCommas) {
    std::unordered_map<std::string, Token> vars;
    auto tokens = Lexer::Tokenize("[1, 2; 3, 4]", vars);
    ASSERT_EQ(tokens.size(), 1u);
    EXPECT_EQ(tokens[0].type, MatrixT);
    auto& m = *std::get<std::unique_ptr<Matrix<double>>>(tokens[0].data);
    EXPECT_DOUBLE_EQ(m[0][0], 1.0);
    EXPECT_DOUBLE_EQ(m[1][1], 4.0);
}

TEST(LexerTokenize, MatrixColumnVector) {
    std::unordered_map<std::string, Token> vars;
    auto tokens = Lexer::Tokenize("[1; 2; 3]", vars);
    ASSERT_EQ(tokens.size(), 1u);
    auto& m = *std::get<std::unique_ptr<Matrix<double>>>(tokens[0].data);
    EXPECT_EQ(m.GetM(), 3u);
    EXPECT_EQ(m.GetN(), 1u);
}

TEST(LexerTokenize, MatrixRowVector) {
    std::unordered_map<std::string, Token> vars;
    auto tokens = Lexer::Tokenize("[1 2 3]", vars);
    ASSERT_EQ(tokens.size(), 1u);
    auto& m = *std::get<std::unique_ptr<Matrix<double>>>(tokens[0].data);
    EXPECT_EQ(m.GetM(), 1u);
    EXPECT_EQ(m.GetN(), 3u);
}

TEST(LexerTokenize, MatrixWithOperator) {
    // [1 2; 3 4] * [5; 6] should produce: MatrixT Mul MatrixT
    std::unordered_map<std::string, Token> vars;
    auto tokens = Lexer::Tokenize("[1 2; 3 4] * [5; 6]", vars);
    ASSERT_EQ(tokens.size(), 3u);
    EXPECT_EQ(tokens[0].type, MatrixT);
    EXPECT_EQ(tokens[1].type, Mul);
    EXPECT_EQ(tokens[2].type, MatrixT);
}

TEST(LexerTokenize, InvMulOperator) {
    std::unordered_map<std::string, Token> vars;
    auto tokens = Lexer::Tokenize("[1 2; 3 4] \\ [5; 6]", vars);
    ASSERT_EQ(tokens.size(), 3u);
    EXPECT_EQ(tokens[0].type, MatrixT);
    EXPECT_EQ(tokens[1].type, InvMul);
    EXPECT_EQ(tokens[2].type, MatrixT);
}

// ============================================================
// Malformed matrix errors
// ============================================================

TEST(LexerTokenize, EmptyMatrixThrows) {
    std::unordered_map<std::string, Token> vars;
    EXPECT_THROW(Lexer::Tokenize("[]", vars), std::invalid_argument);
}

TEST(LexerTokenize, MalformedMatrixEmptyRowThrows) {
    std::unordered_map<std::string, Token> vars;
    EXPECT_THROW(Lexer::Tokenize("[;1]", vars), std::invalid_argument);
}

TEST(LexerTokenize, MalformedMatrixEmptyCommaThrows) {
    std::unordered_map<std::string, Token> vars;
    EXPECT_THROW(Lexer::Tokenize("[,1;2]", vars), std::invalid_argument);
}
