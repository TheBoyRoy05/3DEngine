#include "triangle.hpp"

#define RGBA(r, g, b, a) ((r & 0xFF) << 24 | (g & 0xFF) << 16 | (b & 0xFF) << 8 | (a & 0xFF))

void Triangle::drawPixel(int x, int y, uint32_t color) {
    SDL_SetRenderDrawColor(window.getRenderer(), R(color), G(color), B(color), A(color));
    SDL_RenderDrawPoint(window.getRenderer(), x, y);
};

uint32_t Triangle::sample(Vector<float, 2> uv) {
    // float x = std::min(std::max(uv[0], 0.0f), 1.0f);
    // float y = std::min(std::max(uv[1], 0.0f), 1.0f);
    return 0;
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
    if (twice_area < 1) return;
    
    int window_width;
    SDL_GetWindowSize(window.getWindow(), &window_width, NULL);

    float min_x = std::min(sv1[0], std::min(sv2[0], sv3[0]));
    float max_x = std::max(sv1[0], std::max(sv2[0], sv3[0]));
    float min_y = std::min(sv1[1], std::min(sv2[1], sv3[1]));
    float max_y = std::max(sv1[1], std::max(sv2[1], sv3[1]));

    Vector<float, 3> delta_col{v2[1] - v3[1], v3[1] - v1[1], v1[1] - v2[1]};
    Vector<float, 3> delta_row{v3[0] - v2[0], v1[0] - v3[0], v2[0] - v1[0]};

    Vector<float, 2> p0 = {min_x, min_y};
    Vector<float, 3> w_row{edge_cross(sv2, sv3, p0),
                           edge_cross(sv3, sv1, p0),
                           edge_cross(sv1, sv2, p0)};
    Vector<float, 3> bias{is_top_left(sv2, sv3) ? 1.0f : 0,
                          is_top_left(sv3, sv1) ? 1.0f : 0,
                          is_top_left(sv1, sv2) ? 1.0f : 0};
    w_row = w_row + bias;

    for (int y = min_y; y <= max_y; y++) {
        Vector<float, 3> w = w_row;
        for (int x = min_x; x <= max_x; x++) {
            Vector<float, 3> coord = Vector<float, 3>(w) / twice_area;
            w = w + delta_col;

            if (w[0] < 0 || w[1] < 0 || w[2] < 0) continue;
            float z = coord.dot({sv1[2], sv2[2], sv3[2]});
            if (z > window.getDepthBuffer()->at(x + y * window_width) + 1e-6) continue;

            window.getDepthBuffer()->at(x + y * window_width) = z;
            int c = std::max(0, std::min(255, int(255 * (z + 1) / 2.0f)));
            drawPixel(x, y, RGBA(c, c, c, 255));
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