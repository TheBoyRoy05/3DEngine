#include <SDL2/SDL.h>
#include <stdlib.h>
#include <iostream>
#include "Engine.hpp"

int main() {
    SDL_Event event;
    Engine engine(800, 600, 0);
    
    while (1) {
        SDL_Delay(16);
        engine.update();

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) return engine.quit();
        }
    }

    return engine.quit();
}
