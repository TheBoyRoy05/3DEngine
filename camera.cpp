#include "camera.hpp"
#include "engine.hpp"
#include <optional>

Camera::Camera(float fovDeg, float zNear, float zFar) : engine(Engine::getInstance()) {
    int width, height;
    SDL_GetWindowSize(engine.getWindow(), &width, &height);
    float fovRad = fovDeg * M_PI / 360.0f;

    projection = Matrix<float, 4, 4>();
    projection[0][0] = 1 / tan(fovRad);
    projection[1][1] = 1 / tan(fovRad);
    projection[2][2] = -(zFar + zNear) / (zFar - zNear);
    projection[2][3] = 2 * zFar * zNear / (zFar - zNear);
    projection[3][2] = -1;
    projection[3][3] = 0;
}

std::optional<Vector<float, 4>> Camera::screenToNDC(Vector<float, 4> v) {
    if (abs(v[2]) > 1) return std::nullopt;
    int width, height;
    SDL_GetWindowSize(engine.getWindow(), &width, &height);
    v[0] = (width + v[0] * std::min(width, height)) / 2.0f;
    v[1] = (height - v[1] * std::min(width, height)) / 2.0f;
    v.print();
    return v;
}
