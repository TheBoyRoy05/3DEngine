#pragma once

class Triangle;

#include <SDL2/SDL.h>

#include <memory>

#include "linalg.hpp"
#include "material.hpp"
#include "window.hpp"

#define R(c) ((c >> 24) & 0xFF)
#define G(c) ((c >> 16) & 0xFF)
#define B(c) ((c >> 8) & 0xFF)
#define A(c) (c & 0xFF)

class Triangle {
    private:
    Window& window;

    float edge_cross(const Vector<float, 2>& v1, const Vector<float, 2>& v2, const Vector<float, 2>& v3) {
        Vector<float, 2> edge1 = v2 - v1;
        Vector<float, 2> edge2 = v3 - v1;
        return edge1[0] * edge2[1] - edge1[1] * edge2[0];
    };

    bool is_top_left(const Vector<float, 2>& v1, const Vector<float, 2>& v2) {
        return (v1[1] > v2[1]) || (v1[1] == v2[1] && v1[0] < v2[0]);
    };

    float lerp(float a, float b, float t) { return a + (b - a) * t; };
    uint32_t sample(Vector<float, 2>& uv);
    
    void drawPixel(int x, int y, uint32_t color);
    void drawLine(const Vector<float, 3>& v1, const Vector<float, 3>& v2);

    public:
    const Vector<float, 3>* v1, *v2, *v3;
    const Vector<float, 2>* uv1, *uv2, *uv3;
    const Vector<float, 3>* n1, *n2, *n3;
    const Material& material;

    Vector<float, 4> sv1, sv2, sv3;
    Vector<float, 3> sn1, sn2, sn3;

    Triangle(const Vector<float, 3>* v1, const Vector<float, 3>* v2, const Vector<float, 3>* v3,
             const Vector<float, 2>* uv1, const Vector<float, 2>* uv2, const Vector<float, 2>* uv3,
             const Vector<float, 3>* n1, const Vector<float, 3>* n2, const Vector<float, 3>* n3,
             const Material& material) : window(Window::getInstance()), v1(v1), v2(v2), v3(v3), uv1(uv1), uv2(uv2), uv3(uv3), n1(n1), n2(n2), n3(n3), material(material) {};

    void setScreenSpaceVerts(Matrix<float, 3, 4>& vertices) {
        sv1 = vertices[0];
        sv2 = vertices[1];
        sv3 = vertices[2];
    };

    void setScreenSpaceNormals(Matrix<float, 3, 3>& normals) {
        sn1 = normals[0];
        sn2 = normals[1];
        sn3 = normals[2];
    };

    void draw();
    void fill();

    void print();
};