#include "engine.hpp"

#include <algorithm>

#include "mesh.hpp"

#define BOUNDED(x, y) ((x) >= 0 && (x) < (width) && (y) >= 0 && (y) < (height))
#define RGBA(r, g, b, a) ((r & 0xFF) << 24 | (g & 0xFF) << 16 | (b & 0xFF) << 8 | (a & 0xFF))
#define R(c) ((c >> 24) & 0xFF)
#define G(c) ((c >> 16) & 0xFF)
#define B(c) ((c >> 8) & 0xFF)
#define A(c) (c & 0xFF)

namespace Triangle {
float edge_cross(const Vector<float, 2>& v1, const Vector<float, 2>& v2, const Vector<float, 2>& v3) {
    Vector<float, 2> edge1 = v2 - v1;
    Vector<float, 2> edge2 = v3 - v1;
    return edge1[0] * edge2[1] - edge1[1] * edge2[0];
}

bool is_top_left(const Vector<float, 2>& v1, const Vector<float, 2>& v2) {
    Vector<float, 2> edge = v2 - v1;
    return edge[1] < 0 || (edge[0] > 0 && edge[1] == 0);
}
}  // namespace Triangle

Engine::Engine(int windowWidth, int windowHeight, uint32_t color) {
    width = windowWidth;
    height = windowHeight;
    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(width, height, 0, &window, &renderer);
    SDL_SetRenderDrawColor(renderer, R(color), G(color), B(color), A(color));
    SDL_RenderClear(renderer);
    depth_buffer.assign(width * height, 2);
    camera = new Camera(90, width, height, 0.1, 10);
    setup();
}

void Engine::drawLine(const Vector<float, 3>& v1, const Vector<float, 3>& v2) {
    int x0 = v1[0], y0 = v1[1];
    int x1 = v2[0], y1 = v2[1];
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    while (true) {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderDrawPoint(renderer, x0, y0);

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

void Engine::drawTriangle(const Vector<float, 3>& v1, const Vector<float, 3>& v2, const Vector<float, 3>& v3) {
    drawLine(v1, v2);
    drawLine(v2, v3);
    drawLine(v3, v1);
}

void Engine::fillTriangle(const Vector<float, 3>& v1, const Vector<float, 3>& v2, const Vector<float, 3>& v3) {
    float z1 = v1[2], z2 = v2[2], z3 = v3[2];
    Vector<float, 2> sv1 = Vector<float, 2>(v1);
    Vector<float, 2> sv2 = Vector<float, 2>(v2);
    Vector<float, 2> sv3 = Vector<float, 2>(v3);

    Vector<float, 3> normal = (v2 - v1).cross(v3 - v1);
    if (normal[2] < 0) return;
    float twice_area = normal.norm();

    float min_x = std::min(sv1[0], std::min(sv2[0], sv3[0]));
    float max_x = std::max(sv1[0], std::max(sv2[0], sv3[0]));
    float min_y = std::min(sv1[1], std::min(sv2[1], sv3[1]));
    float max_y = std::max(sv1[1], std::max(sv2[1], sv3[1]));

    Vector<int, 3> delta_col{int(v2[1] - v3[1]), int(v3[1] - v1[1]), int(v1[1] - v2[1])};
    Vector<int, 3> delta_row{int(v3[0] - v2[0]), int(v1[0] - v3[0]), int(v2[0] - v1[0])};

    Vector<float, 2> p0 = {min_x, min_y};
    Vector<int, 3> w_row{int(Triangle::edge_cross(sv2, sv3, p0)),
                         int(Triangle::edge_cross(sv3, sv1, p0)),
                         int(Triangle::edge_cross(sv1, sv2, p0))};
    Vector<int, 3> bias{Triangle::is_top_left(sv2, sv3) ? 1 : 0,
                        Triangle::is_top_left(sv3, sv1) ? 1 : 0,
                        Triangle::is_top_left(sv1, sv2) ? 1 : 0};
    w_row = w_row + bias;

    for (float y = min_y; y <= max_y; y++) {
        Vector<int, 3> w = w_row;
        for (float x = min_x; x <= max_x; x++) {
            if (w[0] < 0 || w[1] < 0 || w[2] < 0) break;

            float alpha = w[0] / twice_area;
            float beta = w[1] / twice_area;
            float gamma = w[2] / twice_area;
            w = w + delta_col;

            float z = alpha * z1 + beta * z2 + gamma * z3;
            if (z >= depth_buffer[x + y * width]) continue;

            depth_buffer[x + y * width] = z;
            int c = std::max(0, std::min(255, int(255 * (z + 1) / 2.0f)));
            SDL_SetRenderDrawColor(renderer, c, c, c, 255);
            SDL_RenderDrawPoint(renderer, x, y);
        }
        w_row = w_row + delta_row;
    }
}

void Engine::setup() {
    Matrix<float, 4, 4> transform;
    Vector<float, 3> position = {0.0f, 0.0f, 5.0f};
    transform.set_position(position);

    std::unique_ptr<Mesh> grass_block = std::make_unique<Mesh>("Assets/Grass_Block/Grass_Block.obj", "Assets/Grass_Block/Grass_Block.mtl");
    grass_block->setTransform(transform);
    meshes.push_back(std::move(grass_block));
}

void Engine::update() {
    depth_buffer.assign(width * height, 2);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // for (auto& mesh : meshes) {
    //     mesh->drawWireFrame(camera, this);
    //     mesh->setRotation((mesh->getRotation() + Vector<float, 3>({0.01f, 0.01f, 0.01f})) % (2 * M_PI));
    // }

    fillTriangle({100, 100, 1}, {200, 100, 0}, {100, 200, 1});
    SDL_RenderPresent(renderer);
}

int Engine::quit() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_SUCCESS;
}