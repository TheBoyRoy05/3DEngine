#ifndef LINALG_HPP
#define LINALG_HPP

#include <array>
#include <iostream>

#define ASSERT_SIZE_EQUAL(x, y) static_assert(x == y, "Vector sizes do not match")

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

    T& operator[](size_t index) {
        return data[index];
    }

    const T& operator[](size_t index) const {
        return data[index];
    }

    Vector operator+(const Vector& other) const {
        ASSERT_SIZE_EQUAL(N, other.size());
        Vector result;
        for (size_t i = 0; i < N; ++i) {
            result[i] = data[i] + other[i];
        }
        return result;
    }

    Vector operator-(const Vector& other) const {
        ASSERT_SIZE_EQUAL(N, other.size());
        Vector result;
        for (size_t i = 0; i < N; ++i) {
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

    T dot(const Vector& other) const {
        ASSERT_SIZE_EQUAL(N, other.size());
        T result = 0;
        for (size_t i = 0; i < N; ++i) {
            result += data[i] * other[i];
        }
        return result;
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

public:
    Matrix() {
        data.fill(Vector<T, M>());
    }

    Matrix(std::initializer_list<std::initializer_list<T>> values) {
        size_t i = 0;
        for (const auto& row : values) {
            if (i < N) data[i++] = Vector<T, M>(row);
            else break;
        }
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

    void print() const {
        std::cout << "[" << std::endl;
        for (size_t i = 0; i < N; ++i) {
            std::cout << "  ";
            data[i].print();
        }
        std::cout << "]" << std::endl;
    }
};

#endif // LINALG_HPP