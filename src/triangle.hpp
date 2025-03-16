#pragma once

#include <SDL2/SDL.h>

#include <memory>

#include "linalg.hpp"
#include "material.hpp"
#include "window.hpp"

#define R(c) ((c >> 24) & 0xFF)
#define G(c) ((c >> 16) & 0xFF)
#define B(c) ((c >> 8) & 0xFF)
#define A(c) (c & 0xFF)

struct Object;

class Triangle {
   private:
    Window& window;

    float edge_cross(const Vector<float, 3>& v0, const Vector<float, 3>& v1, const Vector<float, 3>& v2) {
        Vector<float, 2> edge1 = v1 - v0;
        Vector<float, 2> edge2 = v2 - v0;
        return edge1[0] * edge2[1] - edge1[1] * edge2[0];
    };

    bool is_top_left(const Vector<float, 2>& v1, const Vector<float, 2>& v2) {
        return (v1[1] > v2[1]) || (v1[1] == v2[1] && v1[0] < v2[0]);
    };

    float lerp(float a, float b, float t) { return a + (b - a) * t; };
    uint32_t sample(Vector<float, 2>& uv);

    void drawPixel(int x, int y, uint32_t color);
    void drawLine(const Vector<float, 3>& v1, const Vector<float, 3>& v2);

    bool inBounds(int x, int y, int w, int h) { return x >= 0 && x < w && y >= 0 && y < h; };
    bool AllOutOfBounds();

    const Vector<float, 3>& V(uint32_t idx) const;
    const Vector<float, 2>& T(uint32_t idx) const;
    const Vector<float, 3>& N(uint32_t idx) const;

   public:
    const uint32_t vidx[3];
    const uint32_t uvidx[3];
    const uint32_t nidx[3];

    const Material& material;
    const Object& object;

    Triangle(const uint32_t vidx[], const uint32_t uvidx[], uint32_t nidx[],
             const Material& material, const Object& object) : window(Window::getInstance()),
                                                               vidx{vidx[0], vidx[1], vidx[2]},
                                                               uvidx{uvidx[0], uvidx[1], uvidx[2]},
                                                               nidx{nidx[0], nidx[1], nidx[2]},
                                                               material(material),
                                                               object(object) {};
                                                               
    void draw();
    uint32_t fragmentShader(int x, int y, float z, Vector<float, 2>& uv, Vector<float, 3>& n);
    void getXBounds(Vector<float, 3> v[3], int x_starts[], int x_ends[]);
    void fill();

    void print();
};