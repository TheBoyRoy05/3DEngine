#include <SDL2/SDL.h>
#include <stdlib.h>
#include <iostream>
#include "engine.hpp"

namespace {
    Engine& engine = Engine::getInstance();
    bool paused = false;
}

/**
 * Processes SDL events and handles quitting and pausing the engine.
 *
 * This function polls for SDL events and checks for specific event types. 
 * If a quit event is detected, it calls the engine's quit method. 
 * If the space key is pressed, it toggles the paused state.
 *
 * @param event Pointer to an SDL_Event structure that will be populated by SDL_PollEvent.
 * @return Returns the result of engine.quit() if a quit event occurs, otherwise returns -1.
 */
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
