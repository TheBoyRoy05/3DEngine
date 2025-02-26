#include "camera.hpp"
#include "engine.hpp"
#include <optional>

/**
 * @brief Constructs a new Camera object with a perspective projection matrix.
 *
 * This constructor initializes a Camera object with a given field of view
 * in degrees, and near and far clipping planes. It calculates the perspective
 * projection matrix based on these parameters and the current window size,
 * provided by the Engine instance. The projection matrix is configured
 * to transform 3D points to a normalized device coordinate space, suitable
 * for rendering.
 *
 * @param fovDeg The field of view in degrees.
 * @param zNear The near clipping plane distance.
 * @param zFar The far clipping plane distance.
 */
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

/**
 * @brief Converts a 3D point from screen coordinates to normalized device coordinates.
 *
 * This function transforms a 3D point from screen coordinates to normalized
 * device coordinates (NDC), which is a 3D coordinate space where the x, y and
 * z coordinates are in the range [-1, 1]. The transformation is based on the
 * current window size and the current projection matrix. If the point is outside
 * the view frustum (i.e. the absolute value of the z-coordinate is greater than 1),
 * the function returns an empty optional.
 *
 * @param v The 3D point in screen coordinates.
 * @return An optional containing the 3D point in normalized device coordinates.
 */
std::optional<Vector<float, 4>> Camera::screenToNDC(Vector<float, 4> v) {
    if (abs(v[2]) > 1) return std::nullopt;
    int width, height;
    SDL_GetWindowSize(engine.getWindow(), &width, &height);
    v[0] = (width + v[0] * std::min(width, height)) / 2.0f;
    v[1] = (height - v[1] * std::min(width, height)) / 2.0f;
    v.print();
    return v;
}
