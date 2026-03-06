#include <gtest/gtest.h>
#include "../src/Matrix.hpp"
#include <cmath>

// ============================================================
// Constructors
// ============================================================

TEST(MatrixConstructor, SizeAndInit) {
    Matrix<double> m(3, 4, 1.0);
    EXPECT_EQ(m.GetM(), 3u);
    EXPECT_EQ(m.GetN(), 4u);
    for (size_t i = 0; i < 3; i++)
        for (size_t j = 0; j < 4; j++)
            EXPECT_DOUBLE_EQ(m[i][j], 1.0);
}

TEST(MatrixConstructor, DefaultInitZero) {
    Matrix<double> m(2, 2);
    EXPECT_DOUBLE_EQ(m[0][0], 0.0);
    EXPECT_DOUBLE_EQ(m[1][1], 0.0);
}

TEST(MatrixConstructor, VectorData) {
    std::vector<std::vector<double>> data = {{1, 2}, {3, 4}};
    Matrix<double> m(data);
    EXPECT_EQ(m.GetM(), 2u);
    EXPECT_EQ(m.GetN(), 2u);
    EXPECT_DOUBLE_EQ(m[0][0], 1.0);
    EXPECT_DOUBLE_EQ(m[1][1], 4.0);
}

TEST(MatrixConstructor, InitializerList) {
    Matrix<double> m = {{1, 2, 3}, {4, 5, 6}};
    EXPECT_EQ(m.GetM(), 2u);
    EXPECT_EQ(m.GetN(), 3u);
    EXPECT_DOUBLE_EQ(m[0][2], 3.0);
    EXPECT_DOUBLE_EQ(m[1][0], 4.0);
}

TEST(MatrixConstructor, CopyConstructor) {
    Matrix<double> a = {{1, 2}, {3, 4}};
    Matrix<double> b(a);
    EXPECT_EQ(b, a);
    // Ensure deep copy — modifying b doesn't affect a
    b[0][0] = 99;
    EXPECT_DOUBLE_EQ(a[0][0], 1.0);
}

TEST(MatrixConstructor, MoveConstructor) {
    Matrix<double> a = {{1, 2}, {3, 4}};
    Matrix<double> copy(a);
    Matrix<double> b(std::move(a));
    EXPECT_EQ(b, copy);
}

// ============================================================
// Matrix-Matrix Arithmetic
// ============================================================

TEST(MatrixArithmetic, Addition) {
    Matrix<double> a = {{1, 2}, {3, 4}};
    Matrix<double> b = {{5, 6}, {7, 8}};
    Matrix<double> expected = {{6, 8}, {10, 12}};
    EXPECT_EQ(a + b, expected);
}

TEST(MatrixArithmetic, Subtraction) {
    Matrix<double> a = {{5, 6}, {7, 8}};
    Matrix<double> b = {{1, 2}, {3, 4}};
    Matrix<double> expected = {{4, 4}, {4, 4}};
    EXPECT_EQ(a - b, expected);
}

TEST(MatrixArithmetic, Multiplication) {
    Matrix<double> a = {{1, 2}, {3, 4}};
    Matrix<double> b = {{5, 6}, {7, 8}};
    // [1*5+2*7, 1*6+2*8] = [19, 22]
    // [3*5+4*7, 3*6+4*8] = [43, 50]
    Matrix<double> expected = {{19, 22}, {43, 50}};
    EXPECT_EQ(a * b, expected);
}

TEST(MatrixArithmetic, MultiplicationNonSquare) {
    // 2x3 * 3x1 = 2x1
    Matrix<double> a = {{1, 2, 3}, {4, 5, 6}};
    Matrix<double> b(std::vector<std::vector<double>>{{1}, {2}, {3}});
    Matrix<double> expected(std::vector<std::vector<double>>{{14}, {32}});
    EXPECT_EQ(a * b, expected);
}

// ============================================================
// Matrix-Matrix Dimension Mismatch Errors
// ============================================================

TEST(MatrixArithmetic, AdditionSizeMismatchThrows) {
    Matrix<double> a = {{1, 2}, {3, 4}};
    Matrix<double> b = {{1, 2, 3}, {4, 5, 6}};
    EXPECT_THROW(a + b, std::runtime_error);
}

TEST(MatrixArithmetic, SubtractionSizeMismatchThrows) {
    Matrix<double> a(2, 2, 1.0);
    Matrix<double> b(3, 3, 1.0);
    EXPECT_THROW(a - b, std::runtime_error);
}

TEST(MatrixArithmetic, MultiplicationSizeMismatchThrows) {
    // 2x2 * 3x2 — inner dimensions don't match
    Matrix<double> a(2, 2, 1.0);
    Matrix<double> b(3, 2, 1.0);
    EXPECT_THROW(a * b, std::runtime_error);
}

// ============================================================
// Matrix-Scalar Operations
// ============================================================

TEST(MatrixScalar, AddScalar) {
    Matrix<double> a = {{1, 2}, {3, 4}};
    Matrix<double> expected = {{11, 12}, {13, 14}};
    EXPECT_EQ(a + 10.0, expected);
}

TEST(MatrixScalar, SubtractScalar) {
    Matrix<double> a = {{10, 20}, {30, 40}};
    Matrix<double> expected = {{5, 15}, {25, 35}};
    EXPECT_EQ(a - 5.0, expected);
}

TEST(MatrixScalar, MultiplyScalar) {
    Matrix<double> a = {{1, 2}, {3, 4}};
    Matrix<double> expected = {{3, 6}, {9, 12}};
    EXPECT_EQ(a * 3.0, expected);
}

TEST(MatrixScalar, DivideScalar) {
    Matrix<double> a = {{10, 20}, {30, 40}};
    Matrix<double> expected = {{5, 10}, {15, 20}};
    EXPECT_EQ(a / 2.0, expected);
}

// ============================================================
// Commutative scalar-matrix operations (free functions)
// ============================================================

TEST(MatrixScalar, ScalarTimesMatrix) {
    Matrix<double> a = {{1, 2}, {3, 4}};
    Matrix<double> expected = {{2, 4}, {6, 8}};
    EXPECT_EQ(2.0 * a, expected);
}

TEST(MatrixScalar, ScalarPlusMatrix) {
    Matrix<double> a = {{1, 2}, {3, 4}};
    Matrix<double> expected = {{6, 7}, {8, 9}};
    EXPECT_EQ(5.0 + a, expected);
}

// ============================================================
// Unary Negation
// ============================================================

TEST(MatrixUnary, Negation) {
    Matrix<double> a = {{1, -2}, {3, -4}};
    Matrix<double> expected = {{-1, 2}, {-3, 4}};
    EXPECT_EQ(-a, expected);
}

// ============================================================
// Element-wise abs
// ============================================================

TEST(MatrixUnary, AbsElementWise) {
    Matrix<double> a = {{-1, 2}, {-3, 4}};
    Matrix<double> expected = {{1, 2}, {3, 4}};
    EXPECT_EQ(abs(a), expected);
}

// ============================================================
// Utilities: Transpose, Identity, IsSquare, IsEmpty, CheckSize
// ============================================================

TEST(MatrixUtilities, Transpose) {
    Matrix<double> a = {{1, 2, 3}, {4, 5, 6}};
    Matrix<double> expected = {{1, 4}, {2, 5}, {3, 6}};
    auto t = a.Transpose();
    EXPECT_EQ(t.GetM(), 3u);
    EXPECT_EQ(t.GetN(), 2u);
    EXPECT_EQ(t, expected);
}

TEST(MatrixUtilities, TransposeSquare) {
    Matrix<double> a = {{1, 2}, {3, 4}};
    Matrix<double> expected = {{1, 3}, {2, 4}};
    EXPECT_EQ(a.Transpose(), expected);
}

TEST(MatrixUtilities, IdentitySquare) {
    Matrix<double> a = {{1, 2}, {3, 4}};
    Matrix<double> expected = {{1, 0}, {0, 1}};
    EXPECT_EQ(a.Identity(), expected);
}

TEST(MatrixUtilities, IdentityNonSquareThrows) {
    Matrix<double> a(2, 3, 1.0);
    EXPECT_THROW(a.Identity(), std::runtime_error);
}

TEST(MatrixUtilities, IsSquare) {
    EXPECT_TRUE(Matrix<double>(3, 3).IsSquare());
    EXPECT_FALSE(Matrix<double>(2, 3).IsSquare());
}

TEST(MatrixUtilities, IsEmpty) {
    // Non-empty matrix constructed normally
    Matrix<double> a(2, 2);
    EXPECT_FALSE(a.IsEmpty());
}

TEST(MatrixUtilities, CheckSizeTrue) {
    Matrix<double> a(3, 3, 1.0);
    Matrix<double> b(3, 3, 2.0);
    EXPECT_TRUE(a.CheckSize(b));
}

TEST(MatrixUtilities, CheckSizeFalse) {
    Matrix<double> a(2, 3, 1.0);
    Matrix<double> b(3, 2, 1.0);
    EXPECT_FALSE(a.CheckSize(b));
}

// ============================================================
// Equality Operators
// ============================================================

TEST(MatrixEquality, EqualMatrices) {
    Matrix<double> a = {{1, 2}, {3, 4}};
    Matrix<double> b = {{1, 2}, {3, 4}};
    EXPECT_EQ(a, b);
    EXPECT_FALSE(a != b);
}

TEST(MatrixEquality, UnequalMatrices) {
    Matrix<double> a = {{1, 2}, {3, 4}};
    Matrix<double> b = {{1, 2}, {3, 5}};
    EXPECT_NE(a, b);
    EXPECT_FALSE(a == b);
}

// ============================================================
// Assignment
// ============================================================

TEST(MatrixAssignment, CopyAssignment) {
    Matrix<double> a = {{1, 2}, {3, 4}};
    Matrix<double> b(2, 2);
    b = a;
    EXPECT_EQ(b, a);
    b[0][0] = 99;
    EXPECT_DOUBLE_EQ(a[0][0], 1.0); // deep copy
}

TEST(MatrixAssignment, SelfAssignment) {
    Matrix<double> a = {{1, 2}, {3, 4}};
    a = a;
    EXPECT_DOUBLE_EQ(a[0][0], 1.0);
}

// ============================================================
// SetData
// ============================================================

TEST(MatrixSetData, ValidData) {
    Matrix<double> m(2, 2);
    m.SetData({{10, 20}, {30, 40}});
    EXPECT_DOUBLE_EQ(m[0][0], 10.0);
    EXPECT_DOUBLE_EQ(m[1][1], 40.0);
}

TEST(MatrixSetData, RowMismatchThrows) {
    Matrix<double> m(2, 2);
    EXPECT_THROW(m.SetData({{1, 2}, {3, 4}, {5, 6}}), std::runtime_error);
}

TEST(MatrixSetData, ColMismatchThrows) {
    Matrix<double> m(2, 2);
    EXPECT_THROW(m.SetData({{1, 2, 3}, {4, 5, 6}}), std::runtime_error);
}

// ============================================================
// Determinant
// ============================================================

TEST(MatrixDeterminant, TwoByTwo) {
    // det([[1, 2],[3, 4]]) = 1*4 - 2*3 = -2
    Matrix<double> a = {{1, 2}, {3, 4}};
    EXPECT_NEAR(a.Determinant(), -2.0, 1e-9);
}

TEST(MatrixDeterminant, ThreeByThree) {
    // det([[1, 2, 3],[0, 1, 4],[5, 6, 0]]) = 1
    Matrix<double> a = {{1, 2, 3}, {0, 1, 4}, {5, 6, 0}};
    EXPECT_NEAR(a.Determinant(), 1.0, 1e-9);
}

TEST(MatrixDeterminant, IdentityMatrix) {
    Matrix<double> a = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};
    EXPECT_NEAR(a.Determinant(), 1.0, 1e-9);
}

TEST(MatrixDeterminant, SingularMatrixThrows) {
    // [[1, 2, 3],[4, 5, 6],[7, 8, 9]] — singular, rows are linearly dependent
    Matrix<double> a = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
    EXPECT_THROW((void)a.Determinant(), std::runtime_error);
}

// ============================================================
// Inverse
// ============================================================

TEST(MatrixInverse, TwoByTwo) {
    // [[4, 7],[2, 6]] → inverse = [[0.6, -0.7],[-0.2, 0.4]]
    Matrix<double> a = {{4, 7}, {2, 6}};
    auto inv = a.Inverse();
    // A * A^-1 should be identity
    auto product = a * inv;
    EXPECT_NEAR(product[0][0], 1.0, 1e-9);
    EXPECT_NEAR(product[0][1], 0.0, 1e-9);
    EXPECT_NEAR(product[1][0], 0.0, 1e-9);
    EXPECT_NEAR(product[1][1], 1.0, 1e-9);
}

TEST(MatrixInverse, ThreeByThree) {
    Matrix<double> a = {{1, 2, 3}, {0, 1, 4}, {5, 6, 0}};
    auto inv = a.Inverse();
    auto product = a * inv;
    for (size_t i = 0; i < 3; i++)
        for (size_t j = 0; j < 3; j++)
            EXPECT_NEAR(product[i][j], (i == j) ? 1.0 : 0.0, 1e-6);
}

TEST(MatrixInverse, SingularMatrixThrows) {
    Matrix<double> a = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
    EXPECT_THROW((void)a.Inverse(), std::runtime_error);
}

// ============================================================
// Solve (LU Decomposition)
// ============================================================

TEST(MatrixSolve, SimpleSystem) {
    // [2 1; 5 3] \ [8; 21] = [3; 2]
    Matrix<double> A = {{2, 1}, {5, 3}};
    Matrix<double> b(std::vector<std::vector<double>>{{8}, {21}});
    auto x = Matrix<double>::Solve(A, b);
    EXPECT_NEAR(x[0][0], 3.0, 1e-9);
    EXPECT_NEAR(x[1][0], 2.0, 1e-9);
}

TEST(MatrixSolve, ThreeByThreeSystem) {
    // [1 -2 3; 2 1 -1; -1 3 2] \ [7; 2; -3]
    Matrix<double> A = {{1, -2, 3}, {2, 1, -1}, {-1, 3, 2}};
    Matrix<double> b(std::vector<std::vector<double>>{{7}, {2}, {-3}});
    auto x = Matrix<double>::Solve(A, b);
    // Verify Ax ≈ b
    auto result = A * x;
    for (size_t i = 0; i < 3; i++)
        EXPECT_NEAR(result[i][0], b[i][0], 1e-6);
}

TEST(MatrixSolve, DiagonalSystem) {
    // [2 0 0; 0 3 0; 0 0 4] \ [8; 9; 8] = [4; 3; 2]
    Matrix<double> A = {{2, 0, 0}, {0, 3, 0}, {0, 0, 4}};
    Matrix<double> b(std::vector<std::vector<double>>{{8}, {9}, {8}});
    auto x = Matrix<double>::Solve(A, b);
    EXPECT_NEAR(x[0][0], 4.0, 1e-9);
    EXPECT_NEAR(x[1][0], 3.0, 1e-9);
    EXPECT_NEAR(x[2][0], 2.0, 1e-9);
}

TEST(MatrixSolve, SingularMatrixThrows) {
    Matrix<double> A = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
    Matrix<double> b(std::vector<std::vector<double>>{{1}, {2}, {3}});
    EXPECT_THROW(Matrix<double>::Solve(A, b), std::runtime_error);
}

TEST(MatrixSolve, NonSquareMatrixThrows) {
    Matrix<double> A = {{1, 2, 3}, {4, 5, 6}};
    Matrix<double> b(std::vector<std::vector<double>>{{1}, {2}});
    EXPECT_THROW(Matrix<double>::Solve(A, b), std::runtime_error);
}

TEST(MatrixSolve, DimensionMismatchThrows) {
    // A is 3x3 but b has 2 rows
    Matrix<double> A = {{1, 2, 3}, {4, 5, 6}, {7, 8, 10}};
    Matrix<double> b(std::vector<std::vector<double>>{{1}, {2}});
    EXPECT_THROW(Matrix<double>::Solve(A, b), std::runtime_error);
}

TEST(MatrixSolve, NonColumnVectorThrows) {
    // b must be a column vector (Nx1)
    Matrix<double> A = {{1, 2}, {3, 4}};
    Matrix<double> b = {{1, 2}, {3, 4}};
    EXPECT_THROW(Matrix<double>::Solve(A, b), std::runtime_error);
}

// ============================================================
// Edge Cases
// ============================================================

TEST(MatrixEdgeCases, OneByOneMatrix) {
    Matrix<double> a = {{5}};
    EXPECT_DOUBLE_EQ(a[0][0], 5.0);
    EXPECT_TRUE(a.IsSquare());
    EXPECT_NEAR(a.Determinant(), 5.0, 1e-9);
}

TEST(MatrixEdgeCases, OneByOneMultiplication) {
    Matrix<double> a = {{3}};
    Matrix<double> b = {{4}};
    auto c = a * b;
    EXPECT_DOUBLE_EQ(c[0][0], 12.0);
}

TEST(MatrixEdgeCases, IdentityTimesMatrix) {
    Matrix<double> a = {{1, 2}, {3, 4}};
    Matrix<double> id = a.Identity();
    EXPECT_EQ(id * a, a);
}

TEST(MatrixEdgeCases, TransposeOfTranspose) {
    Matrix<double> a = {{1, 2, 3}, {4, 5, 6}};
    EXPECT_EQ(a.Transpose().Transpose(), a);
}

TEST(MatrixEdgeCases, MultiplyByZeroScalar) {
    Matrix<double> a = {{1, 2}, {3, 4}};
    Matrix<double> expected(2, 2, 0.0);
    EXPECT_EQ(a * 0.0, expected);
}
