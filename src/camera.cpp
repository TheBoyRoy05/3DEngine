#include "camera.hpp"
#include "window.hpp"

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
Camera::Camera(float fovDeg, float zNear, float zFar) : window(Window::getInstance()) {
    float fovRad = fovDeg * M_PI / 360.0f;

    projection = Matrix<float, 4, 4>();
    projection[0][0] = 1 / tan(fovRad);
    projection[1][1] = 1 / tan(fovRad);
    projection[2][2] = -(zFar + zNear) / (zFar - zNear);
    projection[2][3] = 2 * zFar * zNear / (zFar - zNear);
    projection[3][2] = -1;
    projection[3][3] = 0;
}

void Camera::screenToNDC(Matrix<float, 3, 4>& verticies) {
    int width, height;
    SDL_GetWindowSize(window.getWindow(), &width, &height);
    for (int i = 0; i < 3; i++) {
        Vector<float, 4>& vertex = verticies[i];
        vertex = vertex / vertex[3];
        vertex[0] = (width + vertex[0] * std::min(width, height)) / 2.0f;
        vertex[1] = (height - vertex[1] * std::min(width, height)) / 2.0f;
    }
}
