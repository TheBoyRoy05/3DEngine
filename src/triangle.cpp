#include "triangle.hpp"

#include <SDL2/SDL_image.h>

#define RGBA(r, g, b, a) ((r & 0xFF) << 24 | (g & 0xFF) << 16 | (b & 0xFF) << 8 | (a & 0xFF))
#define CLAMP(x, min, max) ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))
#define MISSING_COLOR RGBA(255, 255, 255, 255)

void Triangle::drawPixel(int x, int y, uint32_t color) {
    int width, height;
    SDL_GetWindowSize(window.getWindow(), &width, &height);
    if (x < 0 || x >= width || y < 0 || y >= height) return;

    SDL_SetRenderDrawColor(window.getRenderer(), R(color), G(color), B(color), A(color));
    SDL_RenderDrawPoint(window.getRenderer(), x, y);
};

uint32_t Triangle::sample(Vector<float, 2>& uv) {
    if (!material.image) return MISSING_COLOR;

    uint32_t* pixels = (uint32_t*)material.image->pixels;
    SDL_PixelFormat* format = material.image->format;

    float x = std::min(std::max(uv[0], 0.0f), 1.0f) * material.image->w;
    float y = (1 - std::min(std::max(uv[1], 0.0f), 1.0f)) * material.image->h;
    float dx = x - floor(x);
    float dy = y - floor(y);

    uint32_t c0 = pixels[(int)floor(y) * material.image->w + (int)floor(x)];
    uint32_t c1 = pixels[(int)floor(y) * material.image->w + (int)ceil(x)];
    uint32_t c2 = pixels[(int)ceil(y) * material.image->w + (int)floor(x)];
    uint32_t c3 = pixels[(int)ceil(y) * material.image->w + (int)ceil(x)];

    uint8_t r0, g0, b0, a0, r1, g1, b1, a1, r2, g2, b2, a2, r3, g3, b3, a3;
    SDL_GetRGBA(c0, format, &r0, &g0, &b0, &a0);
    SDL_GetRGBA(c1, format, &r1, &g1, &b1, &a1);
    SDL_GetRGBA(c2, format, &r2, &g2, &b2, &a2);
    SDL_GetRGBA(c3, format, &r3, &g3, &b3, &a3);

    uint8_t r = lerp(lerp(r0, r1, dx), lerp(r2, r3, dx), dy);
    uint8_t g = lerp(lerp(g0, g1, dx), lerp(g2, g3, dx), dy);
    uint8_t b = lerp(lerp(b0, b1, dx), lerp(b2, b3, dx), dy);
    uint8_t a = lerp(lerp(a0, a1, dx), lerp(a2, a3, dx), dy);

    return RGBA(r, g, b, a);
}

/**
 * Draws a line between two points on the screen using Bresenham's line algorithm.
 *
 * @param v1 The first point on the line.
 * @param v2 The second point on the line.
 */
void Triangle::drawLine(const Vector<float, 3>& v1, const Vector<float, 3>& v2) {
    int x0 = v1[0], y0 = v1[1];
    int x1 = v2[0], y1 = v2[1];
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    while (true) {
        drawPixel(x0, y0, 0xFF0000FF);

        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}

/**
 * Draws the triangle by drawing a line between each pair of its vertices.
 */
void Triangle::draw() {
    drawLine(sv1, sv2);
    drawLine(sv2, sv3);
    drawLine(sv3, sv1);
}

void Triangle::fill() {
    float twice_area = edge_cross(sv1, sv2, sv3);
    if (twice_area > -1) return;
    const float inv_twice_area = 1.0f / twice_area;

    // Sort vertices by y-coordinate (top to bottom)
    Vector<float, 3> v[3] = {sv1, sv2, sv3};
    if (v[0][1] > v[1][1]) std::swap(v[0], v[1]);
    if (v[1][1] > v[2][1]) std::swap(v[1], v[2]);
    if (v[0][1] > v[1][1]) std::swap(v[0], v[1]);

    int width, height;
    SDL_GetWindowSize(window.getWindow(), &width, &height);

    // Edge deltas
    float dx1 = (v[1][0] - v[0][0]) / (v[1][1] - v[0][1] + 1e-6);
    float dx2 = (v[2][0] - v[0][0]) / (v[2][1] - v[0][1] + 1e-6);
    float dx3 = (v[2][0] - v[1][0]) / (v[2][1] - v[1][1] + 1e-6);

    // Perspective-correct interpolation setup
    Vector<float, 3> zinv = {1 / sv1[3], 1 / sv2[3], 1 / sv3[3]};
    Matrix<float, 3, 3> pn = Matrix<float, 3, 3>({sn1 * zinv[0], sn2 * zinv[1], sn3 * zinv[2]}).transpose();

    // Fill first part of the triangle (flat-bottom)
    float xs = v[0][0], xe = v[0][0];
    bool middleIsOnLeft = v[1][0] < dx2 * (v[1][1] - v[0][1]) + v[0][0];

    float ds = middleIsOnLeft ? dx1 : dx2;
    float de = middleIsOnLeft ? dx2 : dx1;
    for (int y = v[0][1]; y <= v[1][1]; y++) {
        if (y < 0 || y >= height) continue;
        int x_start = std::ceil(xs), x_end = std::ceil(xe);
        for (int x = x_start; x <= x_end; x++) {
            if (x < 0 || x >= width) continue;
            Vector<float, 3> coord = computeBarycentric(sv1, sv2, sv3, x, y) * inv_twice_area;
            if (coord[0] < 0 || coord[1] < 0 || coord[2] < 0) continue;

            float z = 1 / coord.dot(zinv);
            int bufferIndex = x + y * width;
            if (z > window.getDepthBuffer()->at(bufferIndex) + 1e-6) continue;
            window.getDepthBuffer()->at(bufferIndex) = z;

            Vector<float, 3> normal = (pn * coord * z).normalize();
            Vector<float, 3> c = normal * 255;
            drawPixel(x, y, RGBA(int(abs(c[0])), int(abs(c[1])), int(abs(c[2])), 255));
        }
        xs += ds;
        xe += de;
    }

    // Fill second part of the triangle (flat-top)
    if (middleIsOnLeft) xs = v[1][0];
    else xe = v[1][0];

    ds = middleIsOnLeft ? dx3 : dx2;
    de = middleIsOnLeft ? dx2 : dx3;
    for (int y = v[1][1]; y <= v[2][1]; y++) {
        if (y < 0 || y >= height) continue;
        int x_start = std::ceil(xs), x_end = std::ceil(xe);
        for (int x = x_start; x <= x_end; x++) {
            if (x < 0 || x >= width) continue;
            Vector<float, 3> coord = computeBarycentric(sv1, sv2, sv3, x, y) * inv_twice_area;
            if (coord[0] < 0 || coord[1] < 0 || coord[2] < 0) continue;

            float z = 1 / coord.dot(zinv);
            int bufferIndex = x + y * width;
            if (z > window.getDepthBuffer()->at(bufferIndex) + 1e-6) continue;
            window.getDepthBuffer()->at(bufferIndex) = z;

            Vector<float, 3> normal = (pn * coord * z).normalize();
            Vector<float, 3> c = normal * 255;
            drawPixel(x, y, RGBA(int(abs(c[0])), int(abs(c[1])), int(abs(c[2])), 255));
        }
        xs += ds;
        xe += de;
    }
}

Vector<float, 3> Triangle::computeBarycentric(Vector<float, 4>& v0, Vector<float, 4>& v1, Vector<float, 4>& v2, float x, float y) {
    float w0 = edge_cross(v1, v2, {x, y});
    float w1 = edge_cross(v2, v0, {x, y});
    float w2 = edge_cross(v0, v1, {x, y});

    return {w0, w1, w2};
}

void Triangle::print() {
    std::cout << "Vertices:\n";
    v1->print();
    v2->print();
    v3->print();
    std::cout << "\nTextures:\n";
    uv1->print();
    uv2->print();
    uv3->print();
    std::cout << "\nNormals:\n";
    n1->print();
    n2->print();
    n3->print();
    std::cout << "\nMaterial: " << material.name << "\n";
}