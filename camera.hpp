#pragma once

#include <SDL2/SDL.h>
#include <math.h>

#include <optional>

#include "linalg.hpp"

class Engine;

class Camera {
    public:
    Matrix<float, 4, 4> projection;
    Engine& engine;

    Camera(float fovDeg, float zNear, float zFar);
    std::optional<Vector<float, 4>> screenToNDC(Vector<float, 4> v);
};
