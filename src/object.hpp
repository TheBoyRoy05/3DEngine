#pragma once
#include <string>
#include <vector>

#include "linalg.hpp"
#include "triangle.hpp"

struct Object {
    std::string name;
    std::vector<Vector<float, 3>> vertices;
    std::vector<Vector<float, 2>> textures;
    std::vector<Vector<float, 3>> normals;
    std::vector<Vector<float, 3>> modelVertices;
    std::vector<Vector<float, 3>> modelNormals;
    std::vector<std::unique_ptr<Triangle>> triangles;
};