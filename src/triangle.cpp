#include "triangle.hpp"

#include <SDL2/SDL_image.h>
#include <limits>
#include <omp.h>

#include "object.hpp"

#define RGBA(r, g, b, a) ((r & 0xFF) << 24 | (g & 0xFF) << 16 | (b & 0xFF) << 8 | (a & 0xFF))
#define CLAMP(x, min, max) ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))
#define MISSING_COLOR RGBA(255, 255, 255, 255)

const Vector<float, 3>& Triangle::V(uint32_t i) const { return object.vertices[vidx[i]]; }
const Vector<float, 2>& Triangle::T(uint32_t i) const { return object.textures[uvidx[i]]; }
const Vector<float, 3>& Triangle::N(uint32_t i) const { return object.normals[nidx[i]]; }

bool Triangle::AllOutOfBounds() {
    int w, h;
    SDL_GetWindowSize(window.getWindow(), &w, &h);
    return !inBounds(V(0)[0], V(0)[1], w, h) &&
           !inBounds(V(1)[0], V(1)[1], w, h) &&
           !inBounds(V(2)[0], V(2)[1], w, h);
};

void Triangle::drawPixel(int x, int y, uint32_t color) {
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
    int x = v1[0], y = v1[1];
    int x1 = v2[0], y1 = v2[1];

    int dx = abs(x1 - x);
    int dy = abs(y1 - y);
    int sx = (x < x1) ? 1 : -1;
    int sy = (y < y1) ? 1 : -1;
    int err = dx - dy;

    int width, height;
    SDL_GetWindowSize(window.getWindow(), &width, &height);

    while (true) {
        if (inBounds(x, y, width, height)) drawPixel(x, y, 0xFF0000FF);

        if (x == x1 && y == y1) break;
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x += sx;
        }
        if (e2 < dx) {
            err += dx;
            y += sy;
        }
    }
}

/**
 * Draws the triangle by drawing a line between each pair of its vertices.
 */
void Triangle::draw() {
    drawLine(V(0), V(1));
    drawLine(V(1), V(2));
    drawLine(V(2), V(0));
}

uint32_t Triangle::fragmentShader(int x, int y, float z, Vector<float, 2>& uv, Vector<float, 3>& n) {
    // Texture Shader
    // Vector<float, 2> uv = puv * coord * z;
    // uint32_t color = sample(uv);

    // Lighting Shader
    // int c = CLAMP(n.dot({0, 0, 1}) * 255, 0, 255);
    // uint32_t color = RGBA(c, c, c, 255);

    // Direction Shader
    Vector<float, 3> c = n * 255;
    uint32_t color = RGBA(int(abs(c[0])), int(abs(c[1])), int(abs(c[2])), 255);

    return color;
}

void Triangle::getXBounds(Vector<float, 3> v[3], int x_starts[], int x_ends[]) {
    float dx1 = (v[1][0] - v[0][0]) / (v[1][1] - v[0][1] + 1e-6);
    float dx2 = (v[2][0] - v[0][0]) / (v[2][1] - v[0][1] + 1e-6);
    float dx3 = (v[2][0] - v[1][0]) / (v[2][1] - v[1][1] + 1e-6);

    bool middleIsOnLeft = dx1 < dx2;
    float ds = middleIsOnLeft ? dx1 : dx2;
    float de = middleIsOnLeft ? dx2 : dx1;
    float xs = v[0][0], xe = v[0][0];

    int y_start = static_cast<int>(std::round(v[0][1]));
    int y_mid = static_cast<int>(std::round(v[1][1]));
    int y_end = static_cast<int>(std::round(v[2][1]));

    for (int y = y_start; y < y_mid; y++) {
        x_starts[y - y_start] = std::floor(xs);
        x_ends[y - y_start] = std::ceil(xe);
        xs += ds;
        xe += de;
    }

    ds = middleIsOnLeft ? dx3 : dx2;
    de = middleIsOnLeft ? dx2 : dx3;
    if (middleIsOnLeft) xs = v[1][0];
    else xe = v[1][0];

    for (int y = y_mid; y <= y_end; y++) {
        x_starts[y - y_start] = std::floor(xs);
        x_ends[y - y_start] = std::ceil(xe);
        xs += ds;
        xe += de;
    }
}

void Triangle::fill() {
    if (AllOutOfBounds()) return;
    float twice_area = edge_cross(V(0), V(1), V(2));
    if (twice_area > -1) return;
    const float inv_twice_area = 1.0f / twice_area;

    int width, height;
    SDL_GetWindowSize(window.getWindow(), &width, &height);

    // Sort vertices by y-coordinate (top to bottom)
    Vector<float, 3> v[] = {V(0), V(1), V(2)};
    if (v[0][1] > v[1][1]) std::swap(v[0], v[1]);
    if (v[1][1] > v[2][1]) std::swap(v[1], v[2]);
    if (v[0][1] > v[1][1]) std::swap(v[0], v[1]);

    // Barycentric coordinates
    Vector<float, 3> delta_col = Vector<float, 3>{V(1)[1] - V(2)[1], V(2)[1] - V(0)[1], V(0)[1] - V(1)[1]} * inv_twice_area;
    Vector<float, 3> delta_row = Vector<float, 3>{V(2)[0] - V(1)[0], V(0)[0] - V(2)[0], V(1)[0] - V(0)[0]} * inv_twice_area;
    Vector<float, 3> coord_init = Vector<float, 3>{edge_cross(V(1), V(2), v[0]), edge_cross(V(2), V(0), v[0]), edge_cross(V(0), V(1), v[0])} * inv_twice_area;

    // Perspective-correct interpolation setup
    Vector<float, 3> zinv = {1 / V(0)[2], 1 / V(1)[2], 1 / V(2)[2]};
    Matrix<float, 3, 3> pn = Matrix<float, 3, 3>({N(0) * zinv[0], N(1) * zinv[1], N(2) * zinv[2]}).transpose();
    Matrix<float, 2, 3> puv = Matrix<float, 3, 2>({T(0) * zinv[0], T(1) * zinv[1], T(2) * zinv[2]}).transpose();

    int y_start = static_cast<int>(std::round(v[0][1]));
    int y_end = static_cast<int>(std::round(v[2][1]));
    int x_starts[y_end - y_start + 1];
    int x_ends[y_end - y_start + 1];
    getXBounds(v, x_starts, x_ends);

    struct Pixel {
        int x, y;
        uint32_t color;
    };
    std::vector<Pixel> pixels;
    std::vector<std::vector<Pixel>> thread_pixels(omp_get_max_threads());

// #pragma omp parallel for schedule(guided)
    for (int y = y_start; y <= y_end; y++) {
        int x_start = x_starts[y - y_start];
        int x_end = x_ends[y - y_start];

        Vector<float, 3> coord = coord_init + delta_col * (x_start - v[0][0] - 1) + delta_row * (y - v[0][1]);
        for (int x = x_start; x <= x_end; x++) {
            coord = coord + delta_col;
            if (x < 0 || x >= width || y < 0 || y >= height) continue;
            if (coord[0] < -1 || coord[1] < -1 || coord[2] < -1) continue;

            float z = 1 / coord.dot(zinv);
            int bufferIndex = x + y * width;
            if (z > window.getDepthBuffer()->at(bufferIndex) + 1e-6) continue;
            window.getDepthBuffer()->at(bufferIndex) = z;

            Vector<float, 2> uv = puv * coord * z;
            Vector<float, 3> normal = (pn * coord * z).normalize();

            // uint32_t color = fragmentShader(x, y, z, uv, normal);
            // if (color) thread_pixels[omp_get_thread_num()].push_back({x, y, color});
            drawPixel(x, y, fragmentShader(x, y, z, uv, normal));
        }
    }

    // for (const auto& thread_pixel : thread_pixels) {
    //     for (const auto& pixel : thread_pixel) {
    //         drawPixel(pixel.x, pixel.y, pixel.color);
    //     }
    // }
}

void Triangle::print() {
    std::cout << "Vertices: " << vidx[0] << ", " << vidx[1] << ", " << vidx[2] << "\n";
    V(0).print();
    V(1).print();
    V(2).print();
    std::cout << "\nTextures: " << uvidx[0] << ", " << uvidx[1] << ", " << uvidx[2] << "\n";
    T(0).print();
    T(1).print();
    T(2).print();
    std::cout << "\nNormals: " << nidx[0] << ", " << nidx[1] << ", " << nidx[2] << "\n";
    N(0).print();
    N(1).print();
    N(2).print();
    std::cout << "\nMaterial: " << material.name << "\n";
}