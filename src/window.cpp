#include "window.hpp"

#define MAX_DEPTH 1
#define R(c) ((c >> 24) & 0xFF)
#define G(c) ((c >> 16) & 0xFF)
#define B(c) ((c >> 8) & 0xFF)
#define A(c) (c & 0xFF)

Window::Window(int width, int height, uint32_t bgColor): bgColor(bgColor) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(width, height, 0, &window, &renderer);
    SDL_SetRenderDrawColor(renderer, R(bgColor), G(bgColor), B(bgColor), A(bgColor));
    SDL_RenderClear(renderer);
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "software");
    depth_buffer = std::make_shared<std::vector<float>>(width * height, 2 * MAX_DEPTH);
}

Window& Window::getInstance(int width, int height, uint32_t bgColor) {
    static Window instance(width, height, bgColor);
    return instance;
}

void Window::clear() {
    int width, height;
    SDL_GetWindowSize(window, &width, &height);
    SDL_SetRenderDrawColor(renderer, R(bgColor), G(bgColor), B(bgColor), 255);
    SDL_RenderClear(renderer);
    depth_buffer->assign(width * height, 2 * MAX_DEPTH);
}

int Window::quit() {
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
    SDL_Quit();
    return EXIT_SUCCESS;
}