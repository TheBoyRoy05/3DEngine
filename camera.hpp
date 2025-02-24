#pragma once

#include <math.h>
#include "linalg.hpp"

class Camera {
    public:
    Matrix<float, 4, 4> projection;
    int width, height;

    Camera(float fovDeg, int width, int height, float zNear, float zFar) {
        this->width = width;
        this->height = height;
        
        float fovRad = fovDeg * M_PI / 360.0f;
        float aspect = width / height;
        projection = Matrix<float, 4, 4>();
        projection[0][0] = 1 / (aspect * tan(fovRad));
        projection[1][1] = 1 / tan(fovRad);
        projection[2][2] = -(zFar + zNear) / (zFar - zNear);
        projection[2][3] = -2 * zFar * zNear / (zFar - zNear);
        projection[3][2] = -1;
    };

    Vector<float, 4> screenToNDC(Vector<float, 4> v) {
        v[0] = (width + v[0] * std::min(width, height)) / 2.0f;
        v[1] = (height - v[1] * std::min(width, height)) / 2.0f;
        return v;
    }
};