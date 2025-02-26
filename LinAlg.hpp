#pragma once

#include <array>
#include <iostream>
#include <initializer_list>

template <typename T, size_t N>
class Vector {
private:
    std::array<T, N> data;

public:
    Vector() {
        data.fill(T {});
    }

    Vector(std::initializer_list<T> values) {
        size_t i = 0;
        for (const auto& value : values) {
            if (i < N) data[i++] = value;
            else break;
        }
    }

    template <size_t M>
    Vector(const Vector<T, M>& other) {
        for (size_t i = 0; i < N; ++i) {
            data[i] = i < M ? other[i] : T {};
        }
    }

    template <size_t M>
    operator Vector<T, M>() const {
        return Vector<T, M>(*this);
    }

    T& operator[](size_t index) {
        return data[index];
    }

    const T& operator[](size_t index) const {
        return data[index];
    }

    Vector operator+(const Vector& other) const {
        Vector result;
        for (size_t i = 0; i < std::min(N, other.size()); ++i) {
            result[i] = data[i] + other[i];
        }
        return result;
    }

    Vector operator-(const Vector& other) const {
        Vector result;
        for (size_t i = 0; i < std::min(N, other.size()); ++i) {
            result[i] = data[i] - other[i];
        }
        return result;
    }

    Vector operator*(const T& scalar) const {
        Vector result;
        for (size_t i = 0; i < N; ++i) {
            result[i] = data[i] * scalar;
        }
        return result;
    }

    Vector operator/(const T& scalar) const {
        Vector result;
        for (size_t i = 0; i < N; ++i) {
            result[i] = data[i] / scalar;
        }
        return result;
    }

    Vector operator%(const T& scalar) const {
        Vector result;
        for (size_t i = 0; i < N; ++i) {
            result[i] = fmod(data[i], scalar);
        }
        return result;
    }

    T dot(const Vector& other) const {
        T result = 0;
        for (size_t i = 0; i < std::min(N, other.size()); ++i) {
            result += data[i] * other[i];
        }
        return result;
    }

    Vector<T, 3> cross(const Vector<T, 3>& other) const {
        static_assert(N == 3, "Cross product is only defined for 3D vectors");
        Vector<T, 3> result;
        result[0] = data[1] * other[2] - data[2] * other[1];
        result[1] = data[2] * other[0] - data[0] * other[2];
        result[2] = data[0] * other[1] - data[1] * other[0];
        return result;
    }

    T norm() {
        T total = 0;
        for (size_t i = 0; i < N; ++i) {
            total += data[i] * data[i];
        }
        return sqrt(total);
    }

    void print() const {
        std::cout << "(";
        for (size_t i = 0; i < N; ++i) {
            std::cout << data[i];
            if (i < N - 1) std::cout << ", ";
        }
        std::cout << ")" << std::endl;
    }

    size_t size() const {
        return N;
    }
};

template <typename T, size_t N, size_t M>
class Matrix {
private:
    Vector<Vector<T, M>, N> data;

    template <size_t NN = N, size_t MM = M>
    std::enable_if_t<(NN > 1 && MM > 1), Matrix<T, NN - 1, MM - 1>>
    cofactor(size_t row, size_t col) const {
        Matrix<T, NN - 1, MM - 1> result;
        size_t r = 0, c = 0;

        for (size_t i = 0; i < NN; ++i) {
            if (i == row) continue;
            c = 0;
            for (size_t j = 0; j < MM; ++j) {
                if (j == col) continue;
                result[r][c] = data[i][j];
                ++c;
            }
            ++r;
        }
        return result;
    }

public:
    Matrix() {
        for (size_t i = 0; i < std::min(N, M); ++i) {
            data[i][i] = static_cast<T>(1);
        }
    }

    Matrix(std::initializer_list<std::initializer_list<T>> values) {
        size_t i = 0;
        for (const auto& row : values) {
            if (i < N) data[i++] = Vector<T, M>(row);
            else break;
        }
    }

    template <size_t R, size_t S>
    Matrix(const Matrix<T, R, S>& other) {
        data = Vector<Vector<T, M>, N>();
        for (size_t i = 0; i < std::min(R, N); ++i) {
            for (size_t j = 0; j < std::min(S, M); ++j) {
                data[i][j] = other.data[i][j];
            }
        }
    }

    template <size_t R, size_t S>
    operator Matrix<T, R, S>() const {
        return Matrix<T, R, S>(*this);
    }

    Vector<T, M>& operator[](size_t index) {
        return data[index];
    }

    const Vector<T, M>& operator[](size_t index) const {
        return data[index];
    }

    Vector<T, N> operator*(const Vector<T, M>& other) const {
        Vector<T, N> result;
        for (size_t i = 0; i < N; ++i) {
            result[i] = data[i].dot(other);
        }
        return result;
    }

    T determinant() const {
        static_assert(N == M, "Determinant is only defined for square matrices");
        if constexpr (N == 1) return data[0][0];
        T result = 0;
        for (size_t col = 0; col < N; ++col) {
            T sign = (col % 2 == 0) ? 1 : -1;
            if constexpr (N > 1)
                result += sign * data[0][col] * cofactor(0, col).determinant();
        }
        return result;
    }

    void print() const {
        std::cout << "[" << std::endl;
        for (size_t i = 0; i < N; ++i) {
            std::cout << "  ";
            data[i].print();
        }
        std::cout << "]" << std::endl;
    }

    void set_position(const Vector<T, N-1>& position) {
        for (size_t i = 0; i < N-1; ++i) {
            data[i][N-1] = position[i];
        }
    }

    Vector<T, N-1> get_position() const {
        Vector<T, N-1> position;
        for (size_t i = 0; i < N-1; ++i) {
            position[i] = data[i][N-1];
        }
        return position;
    }

    void set_rotation3(const Vector<T, 3>& angles) {
        static_assert(N >= 3 && M >= 3, "Rotation matrix must be at least 3x3");
        T sx = sin(angles[0]), sy = sin(angles[1]), sz = sin(angles[2]);
        T cx = cos(angles[0]), cy = cos(angles[1]), cz = cos(angles[2]);

        data[0][0] = cy * cz;
        data[0][1] = cz * sx * sy - cx * sz;
        data[0][2] = cx * cz * sy + sx * sz;
        data[1][0] = cy * sz;
        data[1][1] = cx * cz + sx * sy * sz;
        data[1][2] = -cz * sx + cx * sy * sz;
        data[2][0] = -sy;
        data[2][1] = cy * sx;
        data[2][2] = cx * cy;
    }
};
