#include "engine.hpp"

#include <algorithm>

#include "mesh.hpp"

#define MAX_DEPTH 1
#define BOUNDED(x, y) ((x) >= 0 && (x) < (width) && (y) >= 0 && (y) < (height))
#define R(c) ((c >> 24) & 0xFF)
#define G(c) ((c >> 16) & 0xFF)
#define B(c) ((c >> 8) & 0xFF)
#define A(c) (c & 0xFF)

/**
 * Initializes an instance of the Engine class.
 *
 * @param width The width of the SDL window to be created.
 * @param height The height of the SDL window to be created.
 * @param color The color of the SDL window to be created, as an RGBA value.
 *
 * This constructor initializes the SDL library and creates a window
 * and a renderer for the engine. It also initializes the depth buffer
 * for the engine.
 */
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

/**
 * Sets up the initial scene for the engine by configuring the camera
 * and loading a mesh object.
 */
void Engine::setup() {
    camera = new Camera(90, 0.1f, 10.0f);

    Matrix<float, 4, 4> transform;
    Vector<float, 3> position = {0.0f, 0.0f, 3.0f};
    transform.set_position(position);

    std::unique_ptr<Mesh> grass_block = std::make_unique<Mesh>("Assets/Grass_Block/Grass_Block.obj", "Assets/Grass_Block/Grass_Block.mtl");
    grass_block->setTransform(transform);
    meshes.push_back(std::move(grass_block));
}

/**
 * Clears the current frame by resetting the depth buffer and filling the
 * renderer with a black color.
 */
void Engine::clear() {
    int width, height;
    SDL_GetWindowSize(window, &width, &height);
    depth_buffer->assign(width * height, 2 * MAX_DEPTH);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
}

/**
 * Updates the engine's state by clearing the current frame, rendering all meshes, 
 * and presenting the rendered frame.
 */
void Engine::update() {
    clear();
    for (auto& mesh : meshes) {
        mesh->draw(camera, false);
        mesh->draw(camera, true);
        mesh->setRotation((mesh->getRotation() + Vector<float, 3>({0.01f, 0.01f, 0.01f})) % (2 * M_PI));
    }
    SDL_RenderPresent(renderer);
}

/**
 * Destroys the engine's window, renderer, and quits the SDL library.
 * This function should be called when the engine is no longer needed.
 *
 * @return EXIT_SUCCESS (0)
 */
int Engine::quit() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_SUCCESS;
}
