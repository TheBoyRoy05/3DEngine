#include <algorithm>
#include "engine.hpp"
#include "mesh.hpp"

#define BOUNDED(x, y) ((x) >= 0 && (x) < (width) && (y) >= 0 && (y) < (height))
#define RGBA(r, g, b, a) ((r & 0xFF) << 24 | (g & 0xFF) << 16 | (b & 0xFF) << 8 | (a & 0xFF))
#define R(c) ((c >> 24) & 0xFF)
#define G(c) ((c >> 16) & 0xFF)
#define B(c) ((c >> 8) & 0xFF)
#define A(c) (c & 0xFF)

Engine::Engine(int windowWidth, int windowHeight, uint32_t color) {
    width = windowWidth;
    height = windowHeight;
    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(width, height, 0, &window, &renderer);
    SDL_SetRenderDrawColor(renderer, R(color), G(color), B(color), A(color));
    SDL_RenderClear(renderer);
    depthBuffer.resize(width * height);
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
    // Scanline algorithm or barycentric coordinates
    // Placeholder implementation
}

void Engine::setup() {
    Matrix<float, 4, 4> transform;
    Vector<float, 3> position = { 0.0f, 0.0f, 5.0f };
    transform.set_position(position);

    std::unique_ptr<Mesh> grass_block = std::make_unique<Mesh>("Assets/Grass_Block/Grass_Block.obj", "Assets/Grass_Block/Grass_Block.mtl");
    grass_block->setTransform(transform);
    meshes.push_back(std::move(grass_block));
}

void Engine::update() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    for (auto& mesh : meshes) {
        mesh->drawWireFrame(camera, this);
        mesh->setRotation((mesh->getRotation() + Vector<float, 3>({ 0.01f, 0.01f, 0.01f })) % (2*M_PI));
    }

    SDL_RenderPresent(renderer);
}

int Engine::quit() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_SUCCESS;
}