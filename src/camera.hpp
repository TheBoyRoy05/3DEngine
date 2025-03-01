#pragma once

#include <SDL2/SDL.h>
#include <math.h>
#include "linalg.hpp"
#include "window.hpp"

class Camera {
    public:
    Matrix<float, 4, 4> projection;
    Window& window;

    Camera(float fovDeg, float zNear, float zFar);
    void screenToNDC(Matrix<float, 3, 4>& verticies);
};
