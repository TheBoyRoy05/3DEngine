#include "triangle.hpp"

#include <SDL2/SDL_image.h>

#define RGBA(r, g, b, a) ((r & 0xFF) << 24 | (g & 0xFF) << 16 | (b & 0xFF) << 8 | (a & 0xFF))

void Triangle::drawPixel(int x, int y, uint32_t color) {
    SDL_SetRenderDrawColor(window.getRenderer(), R(color), G(color), B(color), A(color));
    SDL_RenderDrawPoint(window.getRenderer(), x, y);
};

void Triangle::sampleAndDraw(int xpos, int ypos, Vector<float, 2>& uv) {
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

    drawPixel(xpos, ypos, RGBA(r, g, b, a));
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

    float min_x = std::min(sv1[0], std::min(sv2[0], sv3[0]));
    float max_x = std::max(sv1[0], std::max(sv2[0], sv3[0]));
    float min_y = std::min(sv1[1], std::min(sv2[1], sv3[1]));
    float max_y = std::max(sv1[1], std::max(sv2[1], sv3[1]));

    Vector<float, 3> delta_col{sv2[1] - sv3[1], sv3[1] - sv1[1], sv1[1] - sv2[1]};
    Vector<float, 3> delta_row{sv3[0] - sv2[0], sv1[0] - sv3[0], sv2[0] - sv1[0]};

    Vector<float, 2> p0 = {min_x, min_y};
    Vector<float, 3> w_row{edge_cross(sv2, sv3, p0),
                           edge_cross(sv3, sv1, p0),
                           edge_cross(sv1, sv2, p0)};
    Vector<float, 3> bias{is_top_left(sv2, sv3) ? 1.0f : 0,
                          is_top_left(sv3, sv1) ? 1.0f : 0,
                          is_top_left(sv1, sv2) ? 1.0f : 0};
    w_row = w_row + bias;

    Vector<float, 3> zinv = {1 / sv1[3], 1 / sv2[3], 1 / sv3[3]};
    Vector<float, 2> puv1 = uv1 * zinv[0];
    Vector<float, 2> puv2 = uv2 * zinv[1];
    Vector<float, 2> puv3 = uv3 * zinv[2];

    int window_width;
    SDL_GetWindowSize(window.getWindow(), &window_width, NULL);

    for (int y = min_y; y <= max_y; y++) {
        Vector<float, 3> w = w_row;
        for (int x = min_x; x <= max_x; x++) {
            // Barycentric Coordinates
            Vector<float, 3> coord = Vector<float, 3>(w) / twice_area;
            w = w + delta_col;

            // Check if the pixel is inside the triangle
            if (coord[0] < 0 || coord[1] < 0 || coord[2] < 0) continue;

            // Check depth buffer
            float z = 1 / coord.dot(zinv);
            if (z > window.getDepthBuffer()->at(x + y * window_width) + 1e-6) continue;
            window.getDepthBuffer()->at(x + y * window_width) = z;

            // Barycentric RGB Shader
            // Vector<float, 3> color = coord * Vector<float, 3>{255 / sv1[2], 255 / sv2[2], 255 / sv3[2]} * z;
            // drawPixel(x, y, RGBA((int)color[0], (int)color[1], (int)color[2], 255));

            // Depth Shader
            // int c = std::max(0, std::min(255, int(255 * (z + 1) / 2.0f)));
            // drawPixel(x, y, RGBA(c, c, c, 255));

            // Texture Shader
            Matrix<float, 3, 2> puv{puv1, puv2, puv3};
            Vector<float, 2> uv_coord = (puv.transpose() * coord) * z;
            sampleAndDraw(x, y, uv_coord);
        }
        w_row = w_row + delta_row;
    }
}

void Triangle::print() {
    std::cout << "\nVertices:\n";
    v1.print();
    v2.print();
    v3.print();
    std::cout << "\nTextures:\n";
    uv1.print();
    uv2.print();
    uv3.print();
    std::cout << "\nNormals:\n";
    n1.print();
    n2.print();
    n3.print();
    std::cout << "\nMaterial: " << material.name << "\n";
}