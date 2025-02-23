#pragma once

#include <SDL2/SDL.h>
#include <cstdint>
#include <vector>
#include <memory>
#include "linalg.hpp"
#include "camera.hpp"
#include "mesh.hpp"

class Engine {
    private:
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;

    std::vector<std::unique_ptr<Mesh>> meshes;
    std::vector<float> depthBuffer {};
    int width, height;
    Camera* camera;
    
    public:
    Engine(int width, int height, uint32_t color);
    void drawLine(const Vector<float, 3>& v1, const Vector<float, 3>& v2);
    void drawTriangle(const Vector<float, 3>& v1, const Vector<float, 3>& v2, const Vector<float, 3>& v3);
    void fillTriangle(const Vector<float, 3>& v1, const Vector<float, 3>& v2, const Vector<float, 3>& v3);
    void setup();
    void update();
    int quit();
};