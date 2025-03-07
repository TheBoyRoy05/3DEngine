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

    void moveCamera(Vector<float, 3> direction) {
        camera->setPosition(camera->getPosition() + direction);
    }

    void setup() {
        camera = std::make_unique<Camera>(60, 0.1f, 100.0f);

        Matrix<float, 4, 4> transform;
        transform.set_position({0.0f, 0.0f, -20.0f});

        std::unique_ptr<Mesh> grass_block = std::make_unique<Mesh>("src/Assets/Grass_Block");
        grass_block->setTransform(transform);
        meshes.push_back(std::move(grass_block));
    };

    void update(float deltaTime) {
        for (auto& mesh : meshes) {
            mesh->draw(camera.get(), false);
            // mesh->setRotation((mesh->getRotation() + Vector<float, 3>({0.6f, 0.6f, 0.6f}) * deltaTime) % (2 * M_PI));
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
    float speed = 2.0f;
}  // namespace State

void handleEvents(SDL_Event* event, float deltaTime) {
    while (SDL_PollEvent(event)) {
        if (event->type == SDL_QUIT) State::running = false;
        if (event->type == SDL_KEYDOWN) {
            if (event->key.keysym.sym == SDLK_SPACE) State::paused = !State::paused;
            if (event->key.keysym.sym == int('w')) Engine::moveCamera(Vector<float, 3>({0.0f, 0.0f, State::speed * deltaTime}));
            if (event->key.keysym.sym == int('s')) Engine::moveCamera(Vector<float, 3>({0.0f, 0.0f, -State::speed * deltaTime}));
            if (event->key.keysym.sym == int('a')) Engine::moveCamera(Vector<float, 3>({State::speed * deltaTime, 0.0f, 0.0f}));
            if (event->key.keysym.sym == int('d')) Engine::moveCamera(Vector<float, 3>({-State::speed * deltaTime, 0.0f, 0.0f}));
            if (event->key.keysym.sym == int('e')) Engine::moveCamera(Vector<float, 3>({0.0f, -State::speed * deltaTime, 0.0f}));
            if (event->key.keysym.sym == int('q')) Engine::moveCamera(Vector<float, 3>({0.0f, State::speed * deltaTime, 0.0f}));
        }
    }
}

int main() {
    Window& window = Window::getInstance();
    SDL_Event event;
    Engine::setup();

    uint32_t lastTime = SDL_GetTicks();
    while (State::running) {
        uint32_t currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;
        handleEvents(&event, deltaTime);

        if (State::paused) continue;
        window.clear();
        Engine::update(deltaTime);
        window.render();
    }

    Engine::cleanup();
    return window.quit();
}
