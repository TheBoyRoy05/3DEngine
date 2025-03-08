#include <SDL2/SDL.h>
#include <stdlib.h>

#include <iostream>

#include "camera.hpp"
#include "linalg.hpp"
#include "mesh.hpp"
#include "window.hpp"

namespace State {
    bool running = true;
    bool paused = false;

    bool mouseDown = false;
    Vector<float, 2> mousePos = {0, 0};
}  // namespace State

namespace Settings {
    float speed = 2.0f;
    float sensitivity = 0.003f;
}  // namespace Settings

namespace Engine {
    namespace {
        std::vector<std::unique_ptr<Mesh>> meshes;
    }  // namespace

    std::unique_ptr<Camera> camera;

    void setup() {
        camera = std::make_unique<Camera>(60, 0.1f, 100.0f);

        Matrix<float, 4, 4> transform;
        transform.set_position({0.0f, 0.0f, -10.0f});

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

Vector<float, 2> MousePos() {
    int MouseX, MouseY;
    SDL_GetMouseState(&MouseX, &MouseY);
    return Vector<float, 2>({float(MouseY), float(MouseX)});
}

void handleEvents(SDL_Event* event, float deltaTime) {
    Camera* camera = Engine::camera.get();
    
    while (SDL_PollEvent(event)) {
        if (event->type == SDL_QUIT) State::running = false;

        if (event->type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON_LEFT) State::mouseDown = true;
        if (event->type == SDL_MOUSEBUTTONUP && event->button.button == SDL_BUTTON_LEFT) State::mouseDown = false;

        if (event->type == SDL_MOUSEMOTION && State::mouseDown) {
            camera->setRotation(camera->getRotation() + (MousePos() - State::mousePos) * Settings::sensitivity);
        }
        State::mousePos = MousePos();

        if (event->type == SDL_KEYDOWN) {
            if (event->key.keysym.sym == SDLK_SPACE) State::paused = !State::paused;
            if (event->key.keysym.sym == int('w'))
                camera->setPosition(camera->getPosition() + camera->getForward() * deltaTime * Settings::speed);
            if (event->key.keysym.sym == int('s'))
                camera->setPosition(camera->getPosition() - camera->getForward() * deltaTime * Settings::speed);
            if (event->key.keysym.sym == int('d'))
                camera->setPosition(camera->getPosition() + camera->getRight() * deltaTime * Settings::speed);
            if (event->key.keysym.sym == int('a'))
                camera->setPosition(camera->getPosition() - camera->getRight() * deltaTime * Settings::speed);
            if (event->key.keysym.sym == int('e'))
                camera->setPosition(camera->getPosition() + camera->getUp() * deltaTime * Settings::speed);
            if (event->key.keysym.sym == int('q'))
                camera->setPosition(camera->getPosition() - camera->getUp() * deltaTime * Settings::speed);
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
