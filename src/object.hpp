#pragma once
#include <string>
#include <vector>

#include "linalg.hpp"
#include "triangle.hpp"

struct Object {
    std::string name;
    std::vector<std::unique_ptr<Vector<float, 3>>> vertices;
    std::vector<std::unique_ptr<Vector<float, 2>>> textures;
    std::vector<std::unique_ptr<Vector<float, 3>>> normals;
    std::vector<std::unique_ptr<Triangle>> triangles;
};