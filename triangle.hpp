#pragma once

#include <SDL2/SDL.h>

#include <memory>
#include "camera.hpp"
#include "linalg.hpp"
#include "mesh.hpp"
#include "engine.hpp"

class Triangle {
    private:
    Engine& engine = Engine::getInstance();
    Vector<float, 3> v1, v2, v3;

    float edge_cross(const Vector<float, 2>& v1, const Vector<float, 2>& v2, const Vector<float, 2>& v3);
    bool is_top_left(const Vector<float, 2>& v1, const Vector<float, 2>& v2);
    
    void drawPixel(int x, int y, uint32_t color);
    void drawLine(const Vector<float, 3>& v1, const Vector<float, 3>& v2);

    public:
    Triangle(Vector<float, 3> v1, Vector<float, 3> v2, Vector<float, 3> v3): v1(v1), v2(v2), v3(v3) {}
    void draw();
    void fill();
};