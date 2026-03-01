#include <gtest/gtest.h>
#include "../src/Lexer.hpp"

// Helper: collect token types from a tokenized string
static std::vector<TokenType> tokenTypes(std::string_view input) {
    auto tokens = Lexer::Tokenize(input);
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
    auto tokens = Lexer::Tokenize("42");
    ASSERT_EQ(tokens.size(), 1);
    EXPECT_EQ(tokens[0].type, Number);
    EXPECT_DOUBLE_EQ(tokens[0].literalValue, 42.0);
}

TEST(LexerTokenize, DecimalNumber) {
    auto tokens = Lexer::Tokenize("3.14");
    ASSERT_EQ(tokens.size(), 1);
    EXPECT_EQ(tokens[0].type, Number);
    EXPECT_NEAR(tokens[0].literalValue, 3.14, 1e-9);
}

TEST(LexerTokenize, MultipleDigitInteger) {
    auto tokens = Lexer::Tokenize("12345");
    ASSERT_EQ(tokens.size(), 1);
    EXPECT_DOUBLE_EQ(tokens[0].literalValue, 12345.0);
}

TEST(LexerTokenize, ZeroValue) {
    auto tokens = Lexer::Tokenize("0");
    ASSERT_EQ(tokens.size(), 1);
    EXPECT_DOUBLE_EQ(tokens[0].literalValue, 0.0);
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
    auto tokens = Lexer::Tokenize("pi");
    ASSERT_EQ(tokens.size(), 1);
    EXPECT_EQ(tokens[0].type, PI);
    EXPECT_DOUBLE_EQ(tokens[0].literalValue, M_PI);
}

// ============================================================
// Whitespace handling
// ============================================================

TEST(LexerTokenize, WhitespaceIsSkipped) {
    auto tokens = Lexer::Tokenize("  1  +  2  ");
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
    EXPECT_THROW(Lexer::Tokenize("@"), std::invalid_argument);
    EXPECT_THROW(Lexer::Tokenize("2 & 3"), std::invalid_argument);
}

TEST(LexerTokenize, InvalidIdentifierThrows) {
    EXPECT_THROW(Lexer::Tokenize("foo"), std::out_of_range);
    EXPECT_THROW(Lexer::Tokenize("xyz"), std::out_of_range);
}
