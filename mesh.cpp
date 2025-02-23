#include "mesh.hpp"
#include "engine.hpp"

void Mesh::draw(Camera* camera, Engine* engine) {
    // Implement drawing logic here
}

void Mesh::drawWireFrame(Camera* camera, Engine* engine) {
    for (const auto& triangle : triangles) {
        Vector<float, 4> v1 = vertices[triangle[0][0]];
        Vector<float, 4> v2 = vertices[triangle[0][1]];
        Vector<float, 4> v3 = vertices[triangle[0][2]];
        
        v1 = camera->projection * (transform * v1);
        v2 = camera->projection * (transform * v2);
        v3 = camera->projection * (transform * v3);
        
        v1 = camera->screenToNDC(v1 / v1[3]);
        v2 = camera->screenToNDC(v2 / v2[3]);
        v3 = camera->screenToNDC(v3 / v3[3]);
        
        engine->drawTriangle(v1, v2, v3);
    }
}
