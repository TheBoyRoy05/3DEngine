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
    float baseSpeed = 2.0f;
    float sprintSpeed = 4.0f;
    float speed = baseSpeed;

    float sensitivity = 0.003f;
}  // namespace Settings

namespace Engine {
    namespace {
        std::vector<std::unique_ptr<Mesh>> meshes;

        void loadMesh(std::string path, Vector<float, 3> position = {0, 0, 0}, Vector<float, 3> scale = {1, 1, 1}, Vector<float, 3> rotation = {0, 0, 0}) {
            std::unique_ptr<Mesh> mesh = std::make_unique<Mesh>(path);
            // mesh->printObjects();
            // mesh->printTriangles();
            // mesh->printMaterials();
            mesh->setRotation(rotation);
            mesh->setPosition(position);
            mesh->setScale(scale);
            meshes.push_back(std::move(mesh));
        }
    }  // namespace

    std::unique_ptr<Camera> camera;

    void setup() {
        camera = std::make_unique<Camera>(60, 0.1f, 100.0f);
        // loadMesh("src/Assets/Grass_Block", {0.0f, 0.0f, -10.0f});
        loadMesh("src/Assets/Utah_Teapot", {0.0f, 0.0f, -10.0f}, {0.05f, 0.05f, 0.05f});
    };

    void update(float deltaTime) {
        for (auto& mesh : meshes) {
            mesh->draw(camera.get(), false);
            // mesh->draw(camera.get(), true);
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

bool handleEvents(SDL_Event* event, float deltaTime) {
    Camera* camera = Engine::camera.get();
    
    bool eventHandled = false;
    while (SDL_PollEvent(event)) {
        eventHandled = true;
        if (event->type == SDL_QUIT) State::running = false;

        if (event->type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON_LEFT) State::mouseDown = true;
        if (event->type == SDL_MOUSEBUTTONUP && event->button.button == SDL_BUTTON_LEFT) State::mouseDown = false;

        if (event->type == SDL_MOUSEMOTION && State::mouseDown) {
            camera->setRotation(camera->getRotation() + (MousePos() - State::mousePos) * Settings::sensitivity);
        }
        State::mousePos = MousePos();

        if (event->type == SDL_KEYDOWN) {
            if (event->key.keysym.sym == SDLK_SPACE) State::paused = !State::paused;
            if (event->key.keysym.sym == SDLK_LSHIFT) Settings::speed = Settings::sprintSpeed;
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

        if (event->type == SDL_KEYUP) {
            if (event->key.keysym.sym == SDLK_LSHIFT) Settings::speed = Settings::baseSpeed;
        }
    }

    return eventHandled;
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
        
        bool eventHandled = handleEvents(&event, deltaTime);
        if (!eventHandled || State::paused) continue;
        std::cout << "FPS: " << (deltaTime > 0 ? 1.0f / deltaTime : 0) << std::endl;

        window.clear();
        Engine::update(deltaTime);
        window.render();
    }

    Engine::cleanup();
    return window.quit();
}
