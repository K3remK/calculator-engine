//
// Created by kerem on 3/3/26.
//

#ifndef CALCULATOR_ENGINE_matrix_H
#define CALCULATOR_ENGINE_matrix_H
#include <functional>
#include <ostream>
#include <stdexcept>
#include <vector>

using Func = std::function<double(double, double)>;

template<typename T>
class Matrix {
    size_t M, N;
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
    using Row = RowProxy<std::vector<double>&>;
    using ConstRow = RowProxy<const std::vector<double>&>;


    
    size_t GetM() const {
        return M;
    }

    
    size_t GetN() const {
        return N;
    }

    const std::vector<std::vector<T>>& GetData() const {
        return m_Data;
    }
    
    bool CheckSize(const Matrix &other) const {
        return other.GetM() == GetM() && other.GetN() == GetN();
    }


    Matrix(size_t M, size_t N, T init = 0)
        : M(M), N(N)
    {
        m_Data = std::vector(M, std::vector<T>(N, init));
    }


    void SetData(const std::vector<std::vector<double>> &data) {
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


    bool IsSquare() const {
        return GetM() == GetN();
    }


    Row operator[](size_t rowIndex) {
        return Row{ m_Data[rowIndex] };
    }


    ConstRow operator[](size_t rowIndex) const {
        return ConstRow{ m_Data[rowIndex] };
    }


    Matrix Transpose() const {
        Matrix res(GetN(), GetM());

        for (size_t row = 0; row < GetM(); row++) {
            for (size_t col = 0; col < GetN(); col++) {
                res[col][row] = m_Data[row][col];
            }
        }
        return res;
    }


    Matrix Inverse() const {
        return {};
    }


    Matrix Identity() const {
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
};

template<typename T, typename A>
Matrix<T> operator*(A val, const Matrix<T>& m) {
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
    return m / val;
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
