#pragma once

#include "linalg.hpp"
#include <string>
#include <SDL2/SDL.h>

struct Material {
    std::string name;
    float shininess;
    Vector<float, 3> ambient;
    Vector<float, 3> diffuse;
    Vector<float, 3> specular;
    std::string texturePath;
    SDL_Surface* image;
};