#pragma once

#include <array>
#include <initializer_list>
#include <iostream>

template <typename T, size_t N>
class Vector {
    private:
    std::array<T, N> data;

    public:
    /**
     * Default constructor for the Vector class.
     * Initializes all elements to the default value of the specified type T.
     */
    Vector() {
        data.fill(T{});
    }

    /**
     * Constructs a Vector from an initializer list of values.
     * Initializes the Vector with the provided values up to the size of the Vector.
     * If the list contains more elements than the Vector's capacity, the excess elements are ignored.
     *
     * @param values The initializer list of values to initialize the Vector.
     */
    Vector(std::initializer_list<T> values) {
        size_t i = 0;
        for (const auto& value : values) {
            if (i < N) data[i++] = value;
            else break;
        }
    }

    /**
     * Constructs a Vector from a different-sized Vector.
     * Copies the elements of the other Vector up to the size of the destination Vector.
     * If the other Vector is smaller than the destination Vector,
     * the rest of the elements are initialized to the default value of type T.
     *
     * @param other The other Vector to copy from.
     */
    template <size_t M>
    Vector(const Vector<T, M>& other) {
        for (size_t i = 0; i < N; ++i) {
            data[i] = i < M ? other[i] : T{};
        }
    }

    template <size_t M>
    operator Vector<T, M>() const {
        return Vector<T, M>(*this);
    }

    T& operator[](size_t index) {
        if (index >= N) throw std::out_of_range("Index out of range");
        return data[index];
    }

    const T& operator[](size_t index) const {
        if (index >= N) throw std::out_of_range("Index out of range");
        return data[index];
    }

    /**
     * Adds the elements of this Vector and another Vector element-wise.
     *
     * The length of the result Vector is the minimum of the two input Vector's sizes.
     * If the two Vectors are of different sizes, the rest of the elements
     * of the longer Vector are ignored.
     *
     * @param other The other Vector to add element-wise.
     * @return A new Vector containing the element-wise sum of the two input Vectors.
     */
    Vector operator+(const Vector& other) const {
        Vector result;
        for (size_t i = 0; i < std::min(N, other.size()); ++i) {
            result[i] = data[i] + other[i];
        }
        return result;
    }

    /**
     * Subtracts the elements of this Vector and another Vector element-wise.
     *
     * The length of the result Vector is the minimum of the two input Vector's sizes.
     * If the two Vectors are of different sizes, the rest of the elements
     * of the longer Vector are ignored.
     *
     * @param other The other Vector to subtract element-wise.
     * @return A new Vector containing the element-wise difference of the two input Vectors.
     */
    Vector operator-(const Vector& other) const {
        Vector result;
        for (size_t i = 0; i < std::min(N, other.size()); ++i) {
            result[i] = data[i] - other[i];
        }
        return result;
    }

    /**
     * Scales this Vector by a scalar value.
     * Multiplies the elements of this Vector with the provided scalar value.
     *
     * @param scalar The scalar value to multiply this Vector with.
     * @return A new Vector containing the scaled values of the input Vector.
     */
    Vector operator*(const T& scalar) const {
        Vector result;
        for (size_t i = 0; i < N; ++i) {
            result[i] = data[i] * scalar;
        }
        return result;
    }

    /**
     * Component-wise multiplication of this Vector with another Vector.
     *
     * The length of the result Vector is the minimum of the two input Vector's sizes.
     * If the two Vectors are of different sizes, the rest of the elements
     * of the longer Vector are ignored.
     *
     * @param other The other Vector to perform component-wise multiplication with.
     * @return A new Vector containing the component-wise product of the two input Vectors.
     */
    Vector operator*(const Vector& other) const {
        Vector result;
        for (size_t i = 0; i < std::min(N, other.size()); ++i) {
            result[i] = data[i] * other[i];
        }
        return result;
    }

    /**
     * Divides the elements of this Vector by a scalar value.
     * Divides the elements of this Vector with the provided scalar value.
     *
     * @param scalar The scalar value to divide this Vector with.
     * @return A new Vector containing the divided values of the input Vector.
     */
    Vector operator/(const T& scalar) const {
        Vector result;
        for (size_t i = 0; i < N; ++i) {
            result[i] = data[i] / scalar;
        }
        return result;
    }

    
    /**
     * Computes the remainder of this Vector divided by a scalar value.
     * For integral types, the remainder is computed using the %
     * operator. For floating point types, the remainder is computed
     * using the fmod() function.
     *
     * @param scalar The scalar value to divide this Vector with.
     * @return A new Vector containing the remainders of the input Vector.
     */
    Vector operator%(const T& scalar) const {
        Vector result;
        for (size_t i = 0; i < N; ++i) {
            if constexpr (std::is_integral_v<T>) result[i] = data[i] % scalar;
            else result[i] = fmod(data[i], scalar);
        }
        return result;
    }

    /**
     * Computes the dot product of this Vector and another Vector.
     *
     * The dot product is the sum of the products of the corresponding elements of the two input Vectors.
     * If the two Vectors are of different sizes, the rest of the elements
     * of the longer Vector are ignored.
     *
     * @param other The other Vector to compute the dot product with.
     * @return The dot product of the two input Vectors.
     */
    T dot(const Vector& other) const {
        T result = 0;
        for (size_t i = 0; i < std::min(N, other.size()); ++i) {
            result += data[i] * other[i];
        }
        return result;
    }

    /**
     * Prints the elements of this Vector to the standard output.
     * The elements are printed in a comma-separated format enclosed in parentheses.
     */
    void print() const {
        std::cout << "(";
        for (size_t i = 0; i < N; ++i) {
            std::cout << data[i];
            if (i < N - 1) std::cout << ", ";
        }
        std::cout << ")" << std::endl;
    }

    /**
     * Returns the size of the Vector.
     *
     * The size of a Vector is the number of elements in the Vector.
     * @return The size of the Vector.
     */
    size_t size() const {
        return N;
    }
};

template <typename T, size_t N, size_t M>
class Matrix {
    private:
    Vector<Vector<T, M>, N> data;

    public:
    /**
     * Constructs a new Matrix as the identity matrix.
     *
     * The identity matrix is a matrix with all elements on the main diagonal set to 1,
     * and all other elements set to 0.
     */
    Matrix() {
        for (size_t i = 0; i < std::min(N, M); ++i) {
            data[i][i] = static_cast<T>(1);
        }
    }

    /**
     * Constructs a Matrix from an initializer list of initializer lists.
     *
     * Each inner initializer list is expected to contain M elements, and there
     * should be N of them. If there are less than N inner initializer lists,
     * the extra rows of the matrix are initialized to zero. If there are more
     * than N, the extra inner initializer lists are ignored.
     */
    template <size_t S>
    Matrix(std::initializer_list<Vector<T, S>> values) {
        size_t i = 0;
        for (const auto& row : values) {
            if (i < N) data[i++] = Vector<T, M>(row);
            else break;
        }
    }

    /**
     * Constructs a Matrix from an initializer list of initializer lists.
     * Initializes the Matrix with the provided rows of values up to the size of the Matrix.
     * If the provided initializer list contains more rows than the Matrix's capacity,
     * the excess rows are ignored.
     *
     * @param values The initializer list of initializer lists representing rows
     *               to initialize the Matrix.
     */
    Matrix(std::initializer_list<std::initializer_list<T>> values) {
        size_t i = 0;
        for (const auto& row : values) {
            if (i < N) data[i++] = Vector<T, M>(row);
            else break;
        }
    }

    /**
     * Constructs a Matrix from another Matrix.
     *
     * Copies the elements of the other Matrix up to the size of the destination Matrix.
     * If the other Matrix is smaller than the destination Matrix,
     * the rest of the elements are initialized to the default value of type T.
     *
     * @param other The other Matrix to copy from.
     */
    template <size_t R, size_t S>
    Matrix(const Matrix<T, R, S>& other) {
        for (size_t i = 0; i < N; ++i) {
            data[i] = i < R ? Vector<T, M>(other[i]) : Vector<T, M>();
        }
    }

    template <size_t R, size_t S>
    operator Matrix<T, R, S>() const {
        return Matrix<T, R, S>(*this);
    }

    Vector<T, M>& operator[](size_t index) {
        if (index >= N) throw std::out_of_range("Index out of range");
        return data[index];
    }

    const Vector<T, M>& operator[](size_t index) const {
        if (index >= N) throw std::out_of_range("Index out of range");
        return data[index];
    }

    /**
     * Performs matrix-vector multiplication on the given vector.
     *
     * This performs a dot product on each row of the matrix with the given vector,
     * resulting in a new vector with the same number of elements as the matrix
     * has rows.
     *
     * @param other The vector to multiply with this matrix.
     * @return A new vector representing the result of the multiplication.
     */
    Vector<T, N> operator*(const Vector<T, M>& other) const {
        Vector<T, N> result;
        for (size_t i = 0; i < N; ++i) {
            result[i] = data[i].dot(other);
        }
        return result;
    }

    /**
     * Performs matrix-matrix multiplication on the given matrix.
     *
     * This performs a dot product on each row of the matrix with each column of the
     * given matrix, resulting in a new matrix with the same number of rows as the
     * matrix and the same number of columns as the given matrix.
     *
     * @param other The matrix to multiply with this matrix.
     * @return A new matrix representing the result of the multiplication.
     */
    template <size_t S>
    Matrix<T, N, S> operator*(const Matrix<T, M, S>& other) const {
        Matrix<T, N, S> result;
        Matrix<T, S, M> transpose = other.transpose();
        for (size_t i = 0; i < N; ++i) {
            for (size_t j = 0; j < S; ++j) {
                result[i][j] = data[i].dot(transpose[j]);
            }
        }
        return result;
    }

    /**
     * Transposes the matrix.
     *
     * This function swaps the rows and columns of the matrix, resulting in a
     * new matrix where the rows are the columns of the original matrix and
     * vice versa.
     *
     * @return A new transposed matrix.
     */
    Matrix<T, M, N> transpose() const {
        Matrix<T, M, N> result;
        for (size_t i = 0; i < N; ++i) {
            for (size_t j = 0; j < M; ++j) {
                result[j][i] = data[i][j];
            }
        }
        return result;
    }

    /**
     * Prints the elements of this Matrix to the standard output.
     * Each row of the matrix is printed on a new line, with the elements
     * of the row printed in a comma-separated format enclosed in parentheses.
     * The entire matrix is enclosed in square brackets.
     */
    void print() const {
        std::cout << "[" << std::endl;
        for (size_t i = 0; i < N; ++i) {
            std::cout << "  ";
            data[i].print();
        }
        std::cout << "]" << std::endl;
    }

        /**
     * Sets the position vector of this Matrix to the specified value.
     *
     * This function sets the last column of the matrix, excluding the homogeneous
     * coordinate, to the specified position vector. The elements of the matrix
     * are updated row by row from top to bottom, and only the elements that fit
     * into the matrix are updated.
     *
     * @param position The position vector to set the matrix to.
     * @note This function is only implemented for square matrices.
     */
    void set_position(const Vector<T, N - 1>& position) {
        static_assert(N == M, "Matrix must be square");
        for (size_t i = 0; i < N - 1; ++i) {
            data[i][M - 1] = position[i];
        }
    }

    /**
     * Scales the matrix by the specified scale vector.
     *
     * This function scales each element along the diagonal of the matrix
     * by the corresponding element in the scale vector. This operation
     * modifies the matrix in place, multiplying each diagonal element by
     * the scale factor provided.
     *
     * @param scale The vector containing the scale factors for each diagonal element.
     * @note This function is only implemented for square matrices.
     */
    void set_scale(const Vector<T, N - 1>& scale) {
        static_assert(N == M, "Matrix must be square");
        for (size_t i = 0; i < N - 1; ++i) {
            data[i][i] *= scale[i];
        }
    }

    /**
     * Retrieves the position vector from the last column of the matrix.
     *
     * This function extracts the position vector by accessing the elements
     * in the last column of the matrix, excluding the homogeneous coordinate.
     * It is typically used to obtain the position of a transformation matrix
     * in a 3D space.
     *
     * @return A vector representing the position extracted from the matrix.
     */
    Vector<T, N - 1> get_position() const {
        Vector<T, N - 1> position;
        for (size_t i = 0; i < N - 1; ++i) {
            position[i] = data[i][N - 1];
        }
        return position;
    }

    /**
     * Sets the rotation of the matrix to the given Euler angles.
     *
     * This function sets the rotation of the matrix to the given Euler angles
     * in the order of ZYX (yaw, pitch, roll). The matrix is multiplied by the
     * rotation matrix corresponding to the given angles.
     *
     * The rotation matrix is calculated using the following formulas:
     *
     *   R = R_z * R_y * R_x
     *
     *   R_x = [ 1,  0,    0  ]
     *         [ 0,  cx, -sx ]
     *         [ 0,  sx,  cx ]
     *
     *   R_y = [ cy,  0,   sy ]
     *         [ 0,  1,    0  ]
     *         [-sy, 0,   cy ]
     *
     *   R_z = [ cz, -sz,  0  ]
     *         [ sz,  cz,  0  ]
     *         [ 0,   0,   1  ]
     *
     * @param angles The Euler angles to set the rotation to.
     */
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
