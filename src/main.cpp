#include <SDL2/SDL.h>
#include <stdlib.h>

#include <iostream>

#include "camera.hpp"
#include "mesh.hpp"
#include "window.hpp"

namespace Engine {
    namespace {
        std::unique_ptr<Camera> camera;
        std::vector<std::unique_ptr<Mesh>> meshes;
    }  // namespace

    void setup() {
        camera = std::make_unique<Camera>(90, 0.1f, 10.0f);

        Matrix<float, 4, 4> transform;
        transform.set_position({0.0f, 0.0f, 3.0f});

        std::unique_ptr<Mesh> grass_block = std::make_unique<Mesh>("src/Assets/Grass_Block");
        grass_block->setTransform(transform);
        meshes.push_back(std::move(grass_block));
    };

    void update(float deltaTime) {
        for (auto& mesh : meshes) {
            mesh->draw(camera.get(), false);
            mesh->setRotation((mesh->getRotation() + Vector<float, 3>({0.6f, 0.6f, 0.6f}) * deltaTime) % (2 * M_PI));
        }
    };

    void cleanup() { 
        meshes.clear(); 
        camera.reset();
    };
}  // namespace Engine

namespace State {
    bool running = true;
    bool paused = false;
}  // namespace State

void handleEvents(SDL_Event* event) {
    while (SDL_PollEvent(event)) {
        if (event->type == SDL_QUIT) State::running = false;
        if (event->type == SDL_KEYDOWN) {
            if (event->key.keysym.sym == SDLK_SPACE) State::paused = !State::paused;
        }
    }
}

int main() {
    Window& window = Window::getInstance();
    SDL_Event event;
    Engine::setup();

    uint32_t lastTime = SDL_GetTicks();
    while (State::running) {
        handleEvents(&event);
        if (State::paused) continue;

        uint32_t currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;

        window.clear();
        Engine::update(deltaTime);
        window.render();
    }

    Engine::cleanup();
    return window.quit();
}
