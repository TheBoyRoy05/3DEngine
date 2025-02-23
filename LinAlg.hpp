#pragma once

#include <array>
#include <iostream>

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
        for (size_t i = 0; i < std::min(N, M); ++i) {
            data[i] = other[i];
        }
        for (size_t i = M; i < N; ++i) {
            data[i] = T {};
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
        if (N != other.size()) throw std::runtime_error("Vector sizes do not match");
        Vector result;
        for (size_t i = 0; i < N; ++i) {
            result[i] = data[i] + other[i];
        }
        return result;
    }

    Vector operator-(const Vector& other) const {
        if (N != other.size()) throw std::runtime_error("Vector sizes do not match");
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

    Vector operator/(const T& scalar) const {
        Vector result;
        for (size_t i = 0; i < N; ++i) {
            result[i] = data[i] / scalar;
        }
        return result;
    }

    T dot(const Vector& other) const {
        if (N != other.size()) throw std::runtime_error("Vector sizes do not match");
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
        data = Vector<Vector<T, M>, N>();
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
