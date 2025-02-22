#ifndef ENGINE_HPP
#define ENGINE_HPP

#include <SDL2/SDL.h>
#include <cstdint>
#include <vector>
#include "LinAlg.hpp"

class Engine {
    private:
    SDL_Renderer* renderer = NULL;
    SDL_Window* window = NULL;

    public:
    Engine(int width, int height, uint32_t color);
    void drawLine(const Vector<int, 2>& v1, const Vector<int, 2>& v2);
    void drawTriangle(const Vector<int, 2>& v1, const Vector<int, 2>& v2, const Vector<int, 2>& v3);
    void fillTriangle(const Vector<int, 2>& v1, const Vector<int, 2>& v2, const Vector<int, 2>& v3);
    void update();
    int quit();
};

#endif  // ENGINE_HPP