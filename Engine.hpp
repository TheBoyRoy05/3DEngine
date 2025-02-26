#pragma once

#include <SDL2/SDL.h>

#include <cstdint>
#include <memory>
#include <vector>

#include "camera.hpp"
#include "linalg.hpp"
#include "mesh.hpp"

class Engine {
    private:
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    std::shared_ptr<std::vector<float>> depth_buffer;
    std::vector<std::unique_ptr<Mesh>> meshes;
    Camera* camera;

    Engine(int width, int height, uint32_t color);

    public:
    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;

    static Engine& getInstance(int width = 800, int height = 600, uint32_t color = 0x000000);

    void setup();
    void clear();
    void update();
    int quit();

    SDL_Window* getWindow() { return window; }
    SDL_Renderer* getRenderer() { return renderer; }
    std::shared_ptr<std::vector<float>> getDepthBuffer() { return depth_buffer; }
};
