#include <gtest/gtest.h>
#include "../src/Lexer.hpp"
#include "../src/Parser.hpp"

// Helper: convert expression string to postfix token type sequence
static std::vector<TokenType> postfixTypes(const std::string& expr) {
    auto infix = Lexer::Tokenize(expr);
    auto postfix = Parser::ToPostfix(infix);
    std::vector<TokenType> types;
    types.reserve(postfix.size());
    for (const auto& t : postfix)
        types.push_back(t.type);
    return types;
}

// ============================================================
// Basic operator precedence
// ============================================================

TEST(ParserToPostfix, SimpleAddition) {
    auto types = postfixTypes("2 + 3");
    std::vector<TokenType> expected = {Number, Number, Add};
    EXPECT_EQ(types, expected);
}

TEST(ParserToPostfix, MulBeforeAdd) {
    // 2 + 3 * 4  →  2 3 4 * +
    auto types = postfixTypes("2 + 3 * 4");
    std::vector<TokenType> expected = {Number, Number, Number, Mul, Add};
    EXPECT_EQ(types, expected);
}

TEST(ParserToPostfix, SubAndMul) {
    // 10 - 2 * 3 + 4  →  10 2 3 * - 4 +
    auto types = postfixTypes("10 - 2 * 3 + 4");
    std::vector<TokenType> expected = {Number, Number, Number, Mul, Sub, Number, Add};
    EXPECT_EQ(types, expected);
}

TEST(ParserToPostfix, DivAndMulLeftAssociative) {
    // 10 / 2 * 5  →  10 2 / 5 *
    auto types = postfixTypes("10 / 2 * 5");
    std::vector<TokenType> expected = {Number, Number, Div, Number, Mul};
    EXPECT_EQ(types, expected);
}

// ============================================================
// Parenthesized grouping
// ============================================================

TEST(ParserToPostfix, ParenthesesOverridePrecedence) {
    // (2 + 3) * 4  →  2 3 + 4 *
    auto types = postfixTypes("(2 + 3) * 4");
    std::vector<TokenType> expected = {Number, Number, Add, Number, Mul};
    EXPECT_EQ(types, expected);
}

TEST(ParserToPostfix, NestedParentheses) {
    // ((1 + 2) * 3) - 4  →  1 2 + 3 * 4 -
    auto types = postfixTypes("((1 + 2) * 3) - 4");
    std::vector<TokenType> expected = {Number, Number, Add, Number, Mul, Number, Sub};
    EXPECT_EQ(types, expected);
}

TEST(ParserToPostfix, DivisionWithParens) {
    // 10 / (2 * 5)  →  10 2 5 * /
    auto types = postfixTypes("10 / (2 * 5)");
    std::vector<TokenType> expected = {Number, Number, Number, Mul, Div};
    EXPECT_EQ(types, expected);
}

// ============================================================
// Right-associative power
// ============================================================

TEST(ParserToPostfix, PowerRightAssociative) {
    // 2 ^ 3 ^ 2  →  2 3 2 ^ ^
    auto types = postfixTypes("2 ^ 3 ^ 2");
    std::vector<TokenType> expected = {Number, Number, Number, Pow, Pow};
    EXPECT_EQ(types, expected);
}

// ============================================================
// Unary minus / negation
// ============================================================

TEST(ParserToPostfix, NegationAtStart) {
    // -3 + 1 → the -3 becomes a single negative number token
    auto infix = Lexer::Tokenize("-3 + 1");
    auto postfix = Parser::ToPostfix(infix);
    ASSERT_EQ(postfix.size(), 4);  // -3, 1, +
    EXPECT_DOUBLE_EQ(postfix[0].literalValue, 3.0);
    EXPECT_EQ(postfix[3].type, Add);
}

TEST(ParserToPostfix, DoubleNegation) {
    // 3--3+1  →  3 -3 - 1 +  (or similar)
    auto infix = Lexer::Tokenize("3--3+1");
    auto postfix = Parser::ToPostfix(infix);
    // Should have 5 tokens: 3, (-3), -, 1, +
    // The second number should be negative
    EXPECT_TRUE(postfix[2].type == UnaryMinus);
}

// ============================================================
// Functions
// ============================================================

TEST(ParserToPostfix, UnaryFunction) {
    // sin(30)  →  30 sin
    const auto types = postfixTypes("sin(30)");
    const std::vector expected = {Number, Sin};
    EXPECT_EQ(types, expected);
}

TEST(ParserToPostfix, VariadicFunctionArgcTracking) {
    // max(1, 2, 3) → 1 2 3 max (with argc=3)
    const auto infix = Lexer::Tokenize("max(1, 2, 3)");
    const auto postfix = Parser::ToPostfix(infix);

    // Last token should be Max with argc == 3
    ASSERT_FALSE(postfix.empty());
    const auto& maxToken = postfix.back();
    EXPECT_EQ(maxToken.type, Max);
    EXPECT_EQ(maxToken.argc, 3u);
}

TEST(ParserToPostfix, NestedFunctions) {
    // sqrt(min(4, 9))  →  4 9 min sqrt
    auto types = postfixTypes("sqrt(min(4, 9))");
    std::vector<TokenType> expected = {Number, Number, Min, Sqrt};
    EXPECT_EQ(types, expected);
}

// ============================================================
// Postfix operators
// ============================================================

TEST(ParserToPostfix, FactorialPostfix) {
    // 5!  →  5 !
    auto types = postfixTypes("5!");
    std::vector<TokenType> expected = {Number, Fact};
    EXPECT_EQ(types, expected);
}

TEST(ParserToPostfix, PercentPostfix) {
    // 80%  →  80 %
    auto types = postfixTypes("80%");
    std::vector<TokenType> expected = {Number, Percent};
    EXPECT_EQ(types, expected);
}

TEST(ParserToPostfix, ComplexPostfixWithGrouping) {
    // (4! + 10)!  — factorial of a grouped expression
    auto infix = Lexer::Tokenize("(4! + 10)!");
    auto postfix = Parser::ToPostfix(infix);
    // Should end with Fact
    ASSERT_FALSE(postfix.empty());
    EXPECT_EQ(postfix.back().type, Fact);
}

// ============================================================
// Modulus operator
// ============================================================



TEST(ParserToPostfix, ModOperator) {
    // (2 + 3) * 4 mod 7  →  2 3 + 4 * 7 mod
    auto types = postfixTypes("(2 + 3) * 4 mod 7");
    std::vector<TokenType> expected = {Number, Number, Add, Number, Mul, Number, Mod};
    EXPECT_EQ(types, expected);
}

// ============================================================
// Error cases
// ============================================================

TEST(ParserToPostfix, MismatchedParenthesesThrows) {
    auto infix = Lexer::Tokenize("(2 + 3");
    EXPECT_THROW(Parser::ToPostfix(infix), std::runtime_error);
}

TEST(ParserToPostfix, InvalidTokenPlacementThrows) {
    // Two consecutive numbers without an operator: "2 3"
    auto infix = Lexer::Tokenize("2 3");
    EXPECT_THROW(Parser::ToPostfix(infix), std::runtime_error);
}
