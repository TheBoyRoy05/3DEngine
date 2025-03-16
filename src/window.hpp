#pragma once

#include "linalg.hpp"

#include <SDL2/SDL.h>
#include <vector>
#include <memory>

class Window {
private:
    uint32_t bgColor;
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    std::shared_ptr<std::vector<float>> depth_buffer;

    Window(int width, int height, uint32_t bgColor);

public:
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    static Window& getInstance(int width = 800, int height = 600, uint32_t bgColor = 0x000000FF);

    SDL_Window* getWindow() { return window; }
    SDL_Renderer* getRenderer() { return renderer; }
    std::shared_ptr<std::vector<float>> getDepthBuffer() { return depth_buffer; }

    Vector<float, 4> toDeviceCoordinates(Vector<float, 4> vertex);

    void render() { SDL_RenderPresent(renderer); }
    void clear();
    int quit();
    ~Window() { quit(); };
};