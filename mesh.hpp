#pragma once

#include <vector>
#include <SDL2/SDL.h>
#include <unordered_map>
#include "linalg.hpp"
#include "camera.hpp"

class Engine;

struct Material {
    std::string name;
    Vector<float, 3> ambient;
    Vector<float, 3> diffuse;
    Vector<float, 3> specular;
    float shininess;
    std::string diffuseMap;
};

class Mesh {
    private:
    std::vector<Vector<float, 4>> vertices;
    std::vector<Vector<float, 2>> textures;
    std::vector<Vector<float, 4>> normals;
    std::vector<Matrix<u_int32_t, 3, 3>> triangles;
    
    Vector<float, 3> rotation;
    Matrix<float, 4, 4> transform;

    std::unordered_map<std::string, Material> materials;
    std::string currentMaterial;
    
    public:
    Mesh(const char* objfilename, const char* mtlfilename);
    Mesh(std::vector<Vector<float, 4>> vertices, std::vector<Vector<float, 2>> textures, 
        std::vector<Vector<float, 4>> normals, std::vector<Matrix<u_int32_t, 3, 3>> triangles, 
        Matrix<float, 4, 4> transform);

    void setTransform(Matrix<float, 4, 4> transform);
    Matrix<float, 4, 4> getTransform();
    void setRotation(Vector<float, 3> rotation);
    Vector<float, 3> getRotation();
    void setPosition(Vector<float, 3> position);
    Vector<float, 3> getPosition();
    void setCenter(Vector<float, 3> center);
    Vector<float, 3> getCenterOfMass();
    void scale(float scale);
    void scale(Vector<float, 3> scale);

    void parseOBJ(const char* filename);
    void parseMTL(const char* filename);
    void setMaterial(const std::string& materialName);
    Material getMaterial();

    void draw(Camera* camera, bool wireFrame = false);
};
