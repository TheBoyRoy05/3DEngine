#pragma once

#include <SDL2/SDL.h>
#include <math.h>

#include "linalg.hpp"
#include "window.hpp"

#define CLAMP(x, min, max) ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))

class Camera {
    private:
    Matrix<float, 4, 4> projection;
    Matrix<float, 4, 4> view;
    Vector<float, 3> position;
    Vector<float, 2> rotation;
    float ooTan, zNear, zFar;
    Window& window;

    public:
    Matrix<float, 4, 4> getView() { return this->view; }
    Matrix<float, 4, 4> getProjection() { return this->projection; }
    Matrix<float, 3, 3> getRotationMatrix() { return Matrix<float, 3, 3>(this->view).transpose(); }

    Vector<float, 3> getPosition() { return this->position; }
    Vector<float, 2> getRotation() { return this->rotation; }

    Vector<float, 3> getUp() { return Vector<float, 3>({0, 1, 0}); }
    Vector<float, 3> getRight() { return getRotationMatrix() * Vector<float, 3>({1, 0, 0}); }
    Vector<float, 3> getForward() { return getRotationMatrix() * Vector<float, 3>({0, 0, -1}); }

    void setRotation(Vector<float, 2> rotation) {
        float yaw = fmod(rotation[0], 2 * M_PI);
        float pitch = CLAMP(rotation[1], -M_PI_2, M_PI_2);
        this->rotation = Vector<float, 2>{yaw, pitch};
        this->view.set_view(this->rotation);
        setPosition(this->position);
    }

    void setPosition(Vector<float, 3> position) {
        this->position = position;
        this->view.set_position(Matrix<float, 3, 3>(this->view) * this->position * -1);
    }

    Camera(float fovDeg, float zNear, float zFar);
    bool toDeviceCoordinates(Matrix<float, 3, 4>& verticies);
};
