#pragma once

#include <SDL2/SDL.h>

#include <unordered_map>

#include "camera.hpp"
#include "linalg.hpp"
#include "material.hpp"
#include "object.hpp"

class Mesh {
    private:
    std::unordered_map<std::string, Object> objects;
    std::unordered_map<std::string, Material> materials;

    Matrix<float, 4, 4> transform;
    Vector<float, 3> rotation;

    public:
    Mesh(const std::string& modelPath);
    ~Mesh();

    void setScale(float scale) { this->setScale({scale, scale, scale}); };
    void setScale(Vector<float, 3> scale) { this->transform.set_scale(scale); };
    void setPosition(Vector<float, 3> position) { this->transform.set_position(position); };
    Vector<float, 3> getPosition() { return this->transform.get_position(); };
    Matrix<float, 4, 4> getTransform() { return this->transform; };
    void setTransform(Matrix<float, 4, 4> transform) { this->transform = transform; };
    Vector<float, 3> getRotation() { return this->rotation; };
    void setRotation(Vector<float, 3> rotation) { this->transform.set_rotation3(this->rotation = rotation); };

    void setCenter(Vector<float, 3> center);
    Vector<float, 3> getCenterOfMass();

    void draw(Camera* camera, bool wireFrame = false);
    void printObjects();
    void printMaterials();
};
