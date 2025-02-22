#include "Engine.hpp"

#include <algorithm>

#define BOUNDED(x, y, w, h) ((x) >= 0 && (x) < (w) && (y) >= 0 && (y) < (h))
#define RGBA(r, g, b, a) ((r & 0xFF) << 24 | (g & 0xFF) << 16 | (b & 0xFF) << 8 | (a & 0xFF))
#define R(c) ((c >> 24) & 0xFF)
#define G(c) ((c >> 16) & 0xFF)
#define B(c) ((c >> 8) & 0xFF)
#define A(c) (c & 0xFF)

Engine::Engine(int width, int height, uint32_t color) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(width, height, 0, &window, &renderer);
    SDL_SetRenderDrawColor(renderer, R(color), G(color), B(color), A(color));
    SDL_RenderClear(renderer);
}

void Engine::drawLine(const Vector<int, 2>& v1, const Vector<int, 2>& v2) {
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

void Engine::drawTriangle(const Vector<int, 2>& v1, const Vector<int, 2>& v2, const Vector<int, 2>& v3) {
    drawLine(v1, v2);
    drawLine(v2, v3);
    drawLine(v3, v1);
}

void Engine::fillTriangle(const Vector<int, 2>& v1, const Vector<int, 2>& v2, const Vector<int, 2>& v3) {
    // Scanline algorithm or barycentric coordinates
    // Placeholder implementation
    drawTriangle(v1, v2, v3);  // Just draw the outline for now
}

void Engine::update() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    const Vector<int, 2> v1 = {100, 100};
    const Vector<int, 2> v2 = {200, 200};
    const Vector<int, 2> v3 = {300, 100};
    fillTriangle(v1, v2, v3);

    SDL_RenderPresent(renderer);
}

int Engine::quit() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_SUCCESS;
}