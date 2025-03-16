#include "window.hpp"

#define FLOAT_MAX std::numeric_limits<float>::max()
#define R(c) ((c >> 24) & 0xFF)
#define G(c) ((c >> 16) & 0xFF)
#define B(c) ((c >> 8) & 0xFF)
#define A(c) (c & 0xFF)

Window::Window(int width, int height, uint32_t bgColor): bgColor(bgColor) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(width, height, 0, &window, &renderer);
    // SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

    SDL_SetRenderDrawColor(renderer, R(bgColor), G(bgColor), B(bgColor), A(bgColor));
    SDL_RenderClear(renderer);
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "software");
    depth_buffer = std::make_shared<std::vector<float>>(width * height, FLOAT_MAX);
}

Vector<float, 4> Window::toDeviceCoordinates(Vector<float, 4> vertex) {
    int width, height;
    SDL_GetWindowSize(window, &width, &height);

    float temp = vertex[3];
    vertex = vertex / vertex[3];
    
    vertex[0] = (width + vertex[0] * std::max(width, height)) / 2.0f;
    vertex[1] = (height - vertex[1] * std::max(width, height)) / 2.0f;
    vertex[2] = temp;

    return vertex;
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
    depth_buffer->assign(width * height, FLOAT_MAX);
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