#pragma once

#include <SDL2/SDL.h>
#include <math.h>

#include "linalg.hpp"
#include "window.hpp"

class Camera {
    private:
    Matrix<float, 4, 4> projection;
    Matrix<float, 4, 4> view;
    Vector<float, 3> rotation;
    float ooTan, zNear, zFar;
    Window& window;

    public:
    Vector<float, 3> getPosition() { return this->view.get_position(); }
    void setPosition(Vector<float, 3> position) { this->view.set_position(position); }
    Matrix<float, 4, 4> getView() { return this->view; }
    void setView(Matrix<float, 4, 4> view) { this->view = view; }
    Vector<float, 3> getRotation() { return this->rotation; }
    void setRotation(Vector<float, 3> rotation) { this->view.set_rotation3(this->rotation = rotation); }
    Matrix<float, 4, 4> getProjection() { return this->projection; }

    Camera(float fovDeg, float zNear, float zFar);
    void screenToNDC(Matrix<float, 3, 4>& verticies);
};
