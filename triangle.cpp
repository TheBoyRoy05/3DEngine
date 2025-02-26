#include "triangle.hpp"

#define RGBA(r, g, b, a) ((r & 0xFF) << 24 | (g & 0xFF) << 16 | (b & 0xFF) << 8 | (a & 0xFF))
#define R(c) ((c >> 24) & 0xFF)
#define G(c) ((c >> 16) & 0xFF)
#define B(c) ((c >> 8) & 0xFF)
#define A(c) (c & 0xFF)

float Triangle::edge_cross(const Vector<float, 2>& v1, const Vector<float, 2>& v2, const Vector<float, 2>& v3) {
    Vector<float, 2> edge1 = v2 - v1;
    Vector<float, 2> edge2 = v3 - v1;
    return edge1[0] * edge2[1] - edge1[1] * edge2[0];
}

bool Triangle::is_top_left(const Vector<float, 2>& v1, const Vector<float, 2>& v2) {
    return (v1[1] > v2[1]) || (v1[1] == v2[1] && v1[0] < v2[0]);
}

void Triangle::drawPixel(int x, int y, uint32_t color) {
    SDL_SetRenderDrawColor(engine.getRenderer(), R(color), G(color), B(color), A(color));
    SDL_RenderDrawPoint(engine.getRenderer(), x, y);
}

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

void Triangle::draw() {
    drawLine(v1, v2);
    drawLine(v2, v3);
    drawLine(v3, v1);
}

void Triangle::fill() {
    Vector<float, 3> normal = (v2 - v1).cross(v3 - v1);
    float twice_area = normal.norm();
    if (normal[2] < 0 || twice_area < 1) return;
    int window_width;
    SDL_GetWindowSize(engine.getWindow(), &window_width, NULL);

    float z1 = v1[2], z2 = v2[2], z3 = v3[2];
    Vector<float, 2> sv1 = Vector<float, 2>(v1);
    Vector<float, 2> sv2 = Vector<float, 2>(v2);
    Vector<float, 2> sv3 = Vector<float, 2>(v3);

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
            float z = coord.dot({z1, z2, z3});
            drawPixel(x, y, RGBA(255, 0, 0, 255)); 
            if (z > engine.getDepthBuffer()->at(x + y * window_width) + 1e-6) continue;

            engine.getDepthBuffer()->at(x + y * window_width) = z;
            int c = std::max(0, std::min(255, int(255 * (z + 1) / 2.0f)));
            drawPixel(x, y, RGBA(c, c, c, 255));
        }
        w_row = w_row + delta_row;
    }
}