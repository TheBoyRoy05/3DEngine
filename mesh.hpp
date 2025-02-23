#pragma once

#include <vector>
#include <SDL2/SDL.h>
#include "linalg.hpp"
#include "camera.hpp"

class Engine;

class Mesh {
    private:
    std::vector<Vector<float, 4>> vertices;
    std::vector<Vector<float, 2>> textures;
    std::vector<Vector<float, 4>> normals;
    std::vector<Matrix<u_int32_t, 3, 3>> triangles;
    Matrix<float, 4, 4> transform;
    
    public:
    Mesh(std::vector<Vector<float, 4>> vertices, std::vector<Vector<float, 2>> textures, 
        std::vector<Vector<float, 4>> normals, std::vector<Matrix<u_int32_t, 3, 3>> triangles, 
        Matrix<float, 4, 4> transform) {
        this->vertices = vertices;
        this->textures = textures;
        this->normals = normals;
        this->triangles = triangles;
        this->transform = transform;
    }
    
    void setTransform(Matrix<float, 4, 4> transform) { this->transform = transform; }

    void draw(Camera* camera, Engine* engine);
    void drawWireFrame(Camera* camera, Engine* engine);
};
