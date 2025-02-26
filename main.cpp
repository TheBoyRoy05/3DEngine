#include <SDL2/SDL.h>
#include <stdlib.h>
#include <iostream>
#include "engine.hpp"

namespace {
    Engine& engine = Engine::getInstance();
    bool paused = false;
}

int handleEvents(SDL_Event* event) {
    while (SDL_PollEvent(event)) {
        if (event->type == SDL_QUIT) return engine.quit();
        if (event->type == SDL_KEYDOWN) {
            if (event->key.keysym.sym == SDLK_SPACE) paused = !paused;
        }
    }
    return -1;
}

int main() {
    SDL_Event event;
    engine.setup();
    
    while (1) {
        SDL_Delay(16);
        if (!paused) engine.update();
        int eventResponse = handleEvents(&event);
        if (eventResponse != -1) return eventResponse;
    }

    return engine.quit();
}
