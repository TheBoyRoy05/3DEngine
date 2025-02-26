#include "engine.hpp"

#include <algorithm>

#include "mesh.hpp"

#define MAX_DEPTH 1
#define BOUNDED(x, y) ((x) >= 0 && (x) < (width) && (y) >= 0 && (y) < (height))
#define R(c) ((c >> 24) & 0xFF)
#define G(c) ((c >> 16) & 0xFF)
#define B(c) ((c >> 8) & 0xFF)
#define A(c) (c & 0xFF)

Engine::Engine(int width, int height, uint32_t color) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(width, height, 0, &window, &renderer);
    SDL_SetRenderDrawColor(renderer, R(color), G(color), B(color), A(color));
    SDL_RenderClear(renderer);
    depth_buffer = std::make_shared<std::vector<float>>(width * height, 2 * MAX_DEPTH);
}

Engine& Engine::getInstance(int width, int height, uint32_t color) {
    static Engine instance(width, height, color);
    return instance;
}

void Engine::setup() {
    camera = new Camera(90, 0.1f, 10.0f);

    Matrix<float, 4, 4> transform;
    Vector<float, 3> position = {0.0f, 0.0f, 3.0f};
    transform.set_position(position);

    std::unique_ptr<Mesh> grass_block = std::make_unique<Mesh>("Assets/Grass_Block/Grass_Block.obj", "Assets/Grass_Block/Grass_Block.mtl");
    grass_block->setTransform(transform);
    meshes.push_back(std::move(grass_block));
}

void Engine::clear() {
    int width, height;
    SDL_GetWindowSize(window, &width, &height);
    depth_buffer->assign(width * height, 2 * MAX_DEPTH);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
}

void Engine::update() {
    clear();
    for (auto& mesh : meshes) {
        mesh->draw(camera, false);
        mesh->draw(camera, true);
        mesh->setRotation((mesh->getRotation() + Vector<float, 3>({0.01f, 0.01f, 0.01f})) % (2 * M_PI));
    }
    SDL_RenderPresent(renderer);
}

int Engine::quit() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_SUCCESS;
}
