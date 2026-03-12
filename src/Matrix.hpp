//
// Created by kerem on 3/3/26.
//


#ifndef CALCULATOR_ENGINE_matrix_H
#define CALCULATOR_ENGINE_matrix_H
#include <functional>
#include <ostream>
#include <stdexcept>
#include <vector>
#include <cstdint>

using Func = std::function<double(double, double)>;

template<typename T>
class Matrix {
    uint16_t M, N;
    std::vector<std::vector<T>> m_Data;

    template<typename RowType>
    struct RowProxy {
        RowType rowData;

        explicit RowProxy(RowType rowData) : rowData(rowData) {}

        auto& operator[](const size_t cot) const {
            return rowData[cot];
        }

        bool operator==(const RowProxy &right) const {
            return rowData == right.rowData;
        }

        bool operator!=(const RowProxy &right) const {
            return rowData != right.rowData;
        }
    };

public:
    using Row = RowProxy<std::vector<T>&>;
    using ConstRow = RowProxy<const std::vector<T>&>;

    [[nodiscard]] size_t GetM() const {
        return M;
    }

    [[nodiscard]] size_t GetN() const {
        return N;
    }

    [[nodiscard]] const std::vector<std::vector<T>>& GetData() const {
        return m_Data;
    }
    
    [[nodiscard]] bool CheckSize(const Matrix &other) const {
        return other.GetM() == GetM() && other.GetN() == GetN();
    }

    [[nodiscard]] bool IsEmpty() const {
        return m_Data.empty();
    }

    // Constructor
    Matrix(size_t M, size_t N, T init = 0)
        : M(M), N(N)
    {
        m_Data = std::vector(M, std::vector<T>(N, init));
    }

    explicit Matrix(const std::vector<std::vector<T>>& data)
        : M(data.size()), N(data.begin()->size()), m_Data(data)
    {}

    // Move ctor.
    Matrix(Matrix &&other) noexcept
        : M(other.M), N(other.N), m_Data(std::move(other.m_Data))
    {
    }

    // Copy ctor.
    Matrix(const Matrix &other)
        : M(other.M), N(other.N), m_Data(other.m_Data)
    {
    }

    Matrix(std::initializer_list<std::initializer_list<T>> list) {
        M = list.size();
        N = list.begin()->size();
        m_Data.assign(list.begin(), list.end());
    }

    // Assignment
    Matrix& operator=(const Matrix &other) {
        if (this == &other) return *this;
        M = other.M;
        N = other.N;
        m_Data = other.m_Data;
        return *this;
    }

    // Move assignment
    Matrix& operator=(Matrix &&other) noexcept {
        if (this == &other) return *this;
        M = other.M;
        N = other.N;
        m_Data = std::move(other.m_Data);
        return *this;
    }

    ~Matrix() = default;

    void SetData(const std::vector<std::vector<T>> &data) {
        if (M != data.size()) throw std::runtime_error("Matrix size mismatch!");
        for (const auto & i : data) {
            if (N != i.size()) throw std::runtime_error("Matrix size mismatch!");
        }
        this->m_Data = data;
    }

    Matrix operator+(const Matrix &other) const {
        if (!CheckSize(other)) throw std::runtime_error("Matrix sizes do not match!");

        Matrix res(GetM(), GetN());

        for (size_t row = 0; row < GetM(); row++) {
            for (size_t col = 0; col < GetN(); col++) {
                res[row][col] = m_Data[row][col] + other[row][col];
            }
        }

        return res;
    }

    Matrix operator-(const Matrix &other) const {
        if (!CheckSize(other)) throw std::runtime_error("Matrix sizes do not match!");

        Matrix res(GetM(), GetN());

        for (size_t row = 0; row < GetM(); row++) {
            for (size_t col = 0; col < GetN(); col++) {
                res[row][col] = m_Data[row][col] - other[row][col];
            }
        }

        return res;
    }

    Matrix operator*(const Matrix &right) const {
        if (GetN() != right.GetM()) throw std::runtime_error("Matrix sizes do not match!");

        Matrix res(GetM(), right.GetN());
        size_t r_col = 0;
        while (r_col < right.GetN()) {
            size_t t_row = 0;
            for (auto& row : m_Data) {
                size_t r_row = 0;
                double val = 0;
                for (auto& rowVal : row) {
                    val += rowVal * right[r_row++][r_col];
                }
                res[t_row++][r_col] = val;
            }
            r_col++;
        }

        return res;
    }

    Matrix operator+(T val) const {
        Matrix res(GetM(), GetN());

        for (size_t row = 0; row < GetM(); row++) {
            for (size_t col = 0; col < GetN(); col++) {
                res[row][col] = val + m_Data[row][col];
            }
        }

        return res;
    }

    Matrix operator-(T val) const {
        Matrix res(GetM(), GetN());

        for (size_t row = 0; row < GetM(); row++) {
            for (size_t col = 0; col < GetN(); col++) {
                res[row][col] = m_Data[row][col] - val;
            }
        }

        return res;
    }

    Matrix operator*(T val) const {
        Matrix res(GetM(), GetN());

        for (size_t row = 0; row < GetM(); row++) {
            for (size_t col = 0; col < GetN(); col++) {
                res[row][col] = m_Data[row][col] * val;
            }
        }

        return res;
    }

    Matrix operator/(T val) const {
        Matrix res(GetM(), GetN());

        for (size_t row = 0; row < GetM(); row++) {
            for (size_t col = 0; col < GetN(); col++) {
                res[row][col] = m_Data[row][col] / val;
            }
        }

        return res;
    }

    bool operator==(const Matrix &other) const {
        for (size_t row = 0; row < GetM(); row++) {
            if ((*this)[row] != other[row]) return false;
        }
        return true;
    }

    bool operator!=(const Matrix &other) const {
        return !(*this == other);
    }

    [[nodiscard]] bool IsSquare() const {
        return GetM() == GetN();
    }

    Row operator[](size_t rowIndex) {
        return Row{ m_Data[rowIndex] };
    }

    ConstRow operator[](size_t rowIndex) const {
        return ConstRow{ m_Data[rowIndex] };
    }

    [[nodiscard]] Matrix Transpose() const {
        Matrix res(GetN(), GetM());

        for (size_t row = 0; row < GetM(); row++) {
            for (size_t col = 0; col < GetN(); col++) {
                res[col][row] = m_Data[row][col];
            }
        }
        return res;
    }

    [[nodiscard]] double Determinant() const {
        Matrix mat = *this;
        const size_t n = mat.GetN();

        double det = 1.0;
        size_t swap = 0;

        for (size_t col = 0; col < n; col++) {

            // Find the row with the largest absolute value in the current column
            size_t pivotRow = col;
            for (size_t row = col + 1; row < n; row++) {
                if (std::abs(mat[row][col]) > std::abs(mat[pivotRow][col])) {
                    pivotRow = row;
                }
            }

            if (abs(mat[pivotRow][col]) < 1e-12) {
                throw std::runtime_error("Matrix is singular!");
            }

            // Swap the pivot row with the current row
            if (pivotRow != col) {
                std::swap(mat.m_Data[pivotRow], mat.m_Data[col]);
                ++swap;
            }

            // eliminate the below row values in the current column
            for (size_t row = col + 1; row < n; row++) {
                double factor = mat[row][col] / mat[col][col];
                for (size_t col2 = col + 1; col2 < n; col2++) {
                    mat[row][col2] -= mat[col][col2] * factor;
                }
            }
        }

        // product of the diagonal columns
        for (size_t col = 0; col < n; col++) {
            det *= mat[col][col];
        }

        // the number of swaps is odd -> change the sign of det.
        if (swap % 2 == 1) {
            det = -det;
        }

        return det;
    }

    [[nodiscard]] Matrix Inverse() const {
        int n = GetN();

        Matrix mat = *this;

        // Build augmented matrix [A | I]
        Matrix aug(n, n * 2, 0.0);
        for (size_t row = 0; row < n; row++) {
            for (size_t col = 0; col < n; col++) {
                aug[row][col] = mat[row][col];
            }
            aug[row][n + row] = 1;
        }

        // Forward Elimination
        for (size_t col = 0; col < n; col++) {

            // Partial pivoting
            size_t pivotRow = col;
            for (size_t row = col + 1; row < n; row++) {
                if (std::abs(aug[row][col]) > std::abs(aug[pivotRow][col])) {
                    pivotRow = row;
                }
            }

            if (abs(aug[pivotRow][col]) < 1e-12) {
                throw std::runtime_error("Matrix is singular!");
            }

            if (pivotRow != col) {
                std::swap(aug.m_Data[pivotRow], aug.m_Data[col]);
            }

            // Eliminate the above row values in the current column
            for (size_t row = col + 1; row < n; row++) {
                double factor = aug[row][col] / aug[col][col];
                for (size_t col2 = 0; col2 < n * 2; col2++) {
                    aug[row][col2] -= aug[col][col2] * factor;
                }
            }
        }

        // Backward Substitution
        for (int col = n - 1; col >= 0; --col) {
            // Eliminate entries above
            for (int row = col - 1; row >= 0; --row) {
                double factor = aug[row][col] / aug[col][col];
                if (std::abs(factor) < 1e-12) continue;
                for (size_t col2 = 0; col2 < n * 2; col2++) {
                    aug[row][col2] -= aug[col][col2] * factor;
                }
            }


            // Scale the pivot so the diagonal becomes 1
            double pivotVal = aug[col][col];
            for (size_t col2 = 0; col2 < n * 2; col2++) {
                aug[col][col2] /= pivotVal;
            }
        }


        // Extract the right half
        Matrix res(n, n);
        for (size_t row = 0; row < n; row++) {
            for (size_t col = 0; col < n; col++) {
                res[row][col] = aug[row][col + n];
            }
        }

        return res;
    }

    [[nodiscard]] Matrix Identity() const {
        if (!IsSquare()) throw std::runtime_error("Identity matrix is not square!");

        Matrix res(GetM(), GetN());

        size_t row = 0, col = 0;

        while (row < GetM() && col < GetN()) {
            res[row++][col++] = 1;
        }

        return res;
    }

    friend std::ostream& operator<<(std::ostream& os, const Matrix& m) {
        for (size_t row = 0; row < m.GetM(); row++) {
            for (size_t col = 0; col < m.GetN(); col++) {
                os << m[row][col] << " ";
            }
            os << std::endl;
        }
        return os;
    }

    static Matrix Solve(const Matrix& A, const Matrix& b) {
        if (A.GetM() != A.GetN()) {
            throw std::runtime_error("Coefficient matrix must be square!");
        }
        if (b.GetM() != A.GetM() || b.GetN() != 1) {
            throw std::runtime_error("Right-hand side must be a column vector with matching row count!");
        }

        Matrix<int> P(A.GetM(), 1);

        if (Matrix tempA = A; luDecompose(tempA, P, static_cast<int>(A.GetM()))) {
            Matrix x = luSolve(tempA, P, b, static_cast<int>(A.GetM()));
            if (!verify(A, x, b)) throw std::runtime_error("Solution verification failed — system may be ill-conditioned!");
            return x;
        }

        throw std::runtime_error("Matrix is singular!");
    }

private:
    // -------------------------------------------------------
    // LU Decomposition with Partial Pivoting
    //
    // Decomposes A into:
    //   P * A = L * U
    //
    // where:
    //   L = lower triangular (1s on diagonal)
    //   U = upper triangular
    //   P = permutation vector (tracks row swaps)
    //
    // All stored in-place in a single matrix for efficiency:
    //   - Upper triangle + diagonal → U
    //   - Strictly lower triangle   → L (diagonal 1s are implicit)
    // -------------------------------------------------------
    static bool luDecompose(Matrix& A, Matrix<int>& P, const int n) {
        for (int i = 0; i < n; i++) {
            P[i][0] = i;
        }

        for (int col = 0; col < n; col++) {
            int pivotRow = col;
            for (int row = col + 1; row < n; row++) {
                if (std::abs(A[row][col]) > std::abs(A[pivotRow][col])) {
                    pivotRow = row;
                }
            }

            if (std::abs(A[pivotRow][col]) < 1e-9) {
                return false;
            }

            if (pivotRow != col) {
                std::swap(A.m_Data[col], A.m_Data[pivotRow]);
                std::swap(P[col][0], P[pivotRow][0]);
            }

            for (int row = col + 1; row < n; row++) {
                A[row][col] /= A[col][col];

                for (int k = col + 1; k < n; k++) {
                    A[row][k] -= A[row][col] * A[col][k];
                }
            }
        }
        return true;
    }

    // -------------------------------------------------------
    // Solve Ax = b using a pre-computed LU decomposition
    //
    // Steps:
    //   1. Apply permutation P to b  → Pb
    //   2. Forward substitution  Ly = Pb
    //   3. Back substitution     Ux = y
    // -------------------------------------------------------
    static Matrix luSolve(const Matrix& LU, const Matrix<int>& P, const Matrix& b, int n) {
        Matrix y(n, 1);
        for (int i = 0; i < n; i++) {
            y[i][0] = b[P[i][0]][0];
        }

        for (int i = 1; i < n; i++) {
            for (int j = 0; j < i; j++) {
                y[i][0] -= LU[i][j] * y[j][0];
            }
        }

        Matrix x(n, 1);
        for (int i = n - 1; i >= 0; i--) {
            x[i][0] = y[i][0];
            for (int j = i + 1; j < n; j++) {
                x[i][0] -= LU[i][j] * x[j][0];
            }
            x[i][0] /= LU[i][i];
        }

        return x;
    }

    // -------------------------------------------------------
    // Verify solution: compute A*x and compare with b
    // -------------------------------------------------------
    static bool verify(const Matrix& A, const Matrix& x, const Matrix& b) {
        auto b_t = A * x;
        for (size_t i = 0; i < b.GetM(); i++) {
            if (std::abs(b[i][0] - b_t[i][0]) > 1e-6) {
                return false;
            }
        }
        return true;
    }
};

template<typename T, typename A>
Matrix<T> operator*(const A val, const Matrix<T>& m) {
    return m * val;
}

template<typename T, typename A>
Matrix<T> operator+(A val, const Matrix<T>& m) {
    return m + val;
}

template<typename T, typename A>
Matrix<T> operator-(A val, const Matrix<T>& m) {
    return val + (m * -1);
}

template<typename T, typename A>
Matrix<T> operator/(A val, const Matrix<T>& m) {
    return val * m.Inverse();
}

template<typename T>
Matrix<T> operator-(const Matrix<T>& m) {
    return -1 * m;
}

template<typename T>
Matrix<T> abs(const Matrix<T> &m) {
    Matrix<T> res(m.GetM(), m.GetN());
    for (size_t row = 0; row < m.GetM(); row++) {
        for (size_t col = 0; col < m.GetN(); col++) {
            res[row][col] = std::abs(m[row][col]);
        }
    }
    return res;
}


#endif //CALCULATOR_ENGINE_matrix_H
