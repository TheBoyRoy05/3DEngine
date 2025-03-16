#pragma once

#include <SDL2/SDL.h>
#include <math.h>

#include "linalg.hpp"

#define CLAMP(x, min, max) ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))

class Camera {
    private:
    Matrix<float, 4, 4> projection;
    Matrix<float, 4, 4> view;
    Vector<float, 3> position;
    Vector<float, 2> rotation;
    float ooTan, zNear, zFar;

    public:
    Matrix<float, 4, 4> getView() { return this->view; }
    Matrix<float, 4, 4> getProjection() { return this->projection; }
    Matrix<float, 3, 3> getRotationMatrix() { return Matrix<float, 3, 3>(this->view).transpose(); }

    Vector<float, 3> getPosition() { return this->position; }
    Vector<float, 2> getRotation() { return this->rotation; }

    Vector<float, 3> getUp() { return Vector<float, 3>({0, 1, 0}); }
    Vector<float, 3> getRight() { return getRotationMatrix() * Vector<float, 3>({1, 0, 0}); }
    Vector<float, 3> getForward() { return getRotationMatrix() * Vector<float, 3>({0, 0, -1}); }

    void setRotation(Vector<float, 3> rotation) {
        float pitch = CLAMP(rotation[0], -M_PI_2, M_PI_2);
        float yaw = fmod(rotation[1], 2 * M_PI);
        this->rotation = Vector<float, 2>({pitch, yaw});
        this->view.set_view(this->rotation);
        setPosition(this->position);
    }

    void setPosition(Vector<float, 3> position) {
        this->position = position;
        this->view.set_position(Matrix<float, 3, 3>(this->view) * this->position * -1);
    }

    Camera(float fovDeg, float zNear, float zFar) {
        this->zNear = zNear;
        this->zFar = zFar;
        this->ooTan = 1.0f / tan(fovDeg * M_PI / 360.0f);
    
        rotation = Vector<float, 3>();
        view = Matrix<float, 4, 4>();
        projection = Matrix<float, 4, 4>();
    
        projection[0][0] = ooTan;
        projection[1][1] = ooTan;
        projection[2][2] = -(zFar + zNear) / (zFar - zNear);
        projection[2][3] = -2 * zFar * zNear / (zFar - zNear);
        projection[3][2] = -1;
        projection[3][3] = 0;
    }
};
