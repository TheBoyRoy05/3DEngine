#include "mesh.hpp"

#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>

#include "triangle.hpp"

Mesh::Mesh(std::vector<Vector<float, 4>> vertices, std::vector<Vector<float, 2>> textures,
           std::vector<Vector<float, 4>> normals, std::vector<Matrix<u_int32_t, 3, 3>> triangles,
           Matrix<float, 4, 4> transform) {
    this->vertices = vertices;
    this->textures = textures;
    this->normals = normals;
    this->triangles = triangles;
    this->transform = transform;
    this->setCenter(this->getCenterOfMass());
}

Mesh::Mesh(const char* objfilename, const char* mtlfilename) {
    parseOBJ(objfilename);
    parseMTL(mtlfilename);
    this->setCenter(this->getCenterOfMass());
}

void Mesh::parseOBJ(const char* filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + std::string(filename));
    }

    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string prefix;
        ss >> prefix;

        if (prefix == "v") {
            float x, y, z;
            ss >> x >> y >> z;
            vertices.push_back({x, y, z, 1.0f});
        } else if (prefix == "vt") {
            float u, v;
            ss >> u >> v;
            textures.push_back({u, v});
        } else if (prefix == "vn") {
            float nx, ny, nz;
            ss >> nx >> ny >> nz;
            normals.push_back({nx, ny, nz, 0.0f});
        } else if (prefix == "f") {
            std::vector<uint32_t> v_indices, vt_indices, vn_indices;
            std::string vertexData;

            while (ss >> vertexData) {
                std::stringstream vs(vertexData);
                std::string v, vt, vn;
                std::getline(vs, v, '/');
                std::getline(vs, vt, '/');
                std::getline(vs, vn, '/');

                uint32_t vi = v.empty() ? 0 : std::stoi(v) - 1;
                uint32_t vti = vt.empty() ? 0 : std::stoi(vt) - 1;
                uint32_t vni = vn.empty() ? 0 : std::stoi(vn) - 1;

                v_indices.push_back(vi);
                vt_indices.push_back(vti);
                vn_indices.push_back(vni);
            }

            if (v_indices.size() == 4) {
                triangles.push_back({{v_indices[0], v_indices[1], v_indices[2]},
                                     {vt_indices[0], vt_indices[1], vt_indices[2]},
                                     {vn_indices[0], vn_indices[1], vn_indices[2]}});

                triangles.push_back({{v_indices[0], v_indices[2], v_indices[3]},
                                     {vt_indices[0], vt_indices[2], vt_indices[3]},
                                     {vn_indices[0], vn_indices[2], vn_indices[3]}});
            } else if (v_indices.size() == 3) {
                triangles.push_back({{v_indices[0], v_indices[1], v_indices[2]},
                                     {vt_indices[0], vt_indices[1], vt_indices[2]},
                                     {vn_indices[0], vn_indices[1], vn_indices[2]}});
            }
        }
    }
    file.close();
}

void Mesh::parseMTL(const char* filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open MTL file: " + std::string(filename));
    }

    std::string line, prefix;
    Material material;

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        ss >> prefix;

        if (prefix == "newmtl") {
            if (!material.name.empty()) {
                materials[material.name] = material;
            }
            ss >> material.name;
        } else if (prefix == "Ka") {  // Ambient color
            ss >> material.ambient[0] >> material.ambient[1] >> material.ambient[2];
        } else if (prefix == "Kd") {  // Diffuse color
            ss >> material.diffuse[0] >> material.diffuse[1] >> material.diffuse[2];
        } else if (prefix == "Ks") {  // Specular color
            ss >> material.specular[0] >> material.specular[1] >> material.specular[2];
        } else if (prefix == "Ns") {  // Shininess
            ss >> material.shininess;
        } else if (prefix == "map_Kd") {  // Diffuse texture map
            ss >> material.diffuseMap;
        }
    }

    // Save the last material before exiting
    if (!material.name.empty()) {
        materials[material.name] = material;
        currentMaterial = material.name;
    }

    file.close();
}

void Mesh::draw(Camera* camera, bool wireFrame) {
    for (const auto& tri : triangles) {
        Vector<float, 4> v1 = vertices[tri[0][0]];
        Vector<float, 4> v2 = vertices[tri[0][1]];
        Vector<float, 4> v3 = vertices[tri[0][2]];

        v1 = camera->projection * (transform * v1);
        v2 = camera->projection * (transform * v2);
        v3 = camera->projection * (transform * v3);

        std::optional<Vector<float, 4>> sv1 = camera->screenToNDC(v1 / v1[3]);
        std::optional<Vector<float, 4>> sv2 = camera->screenToNDC(v2 / v2[3]);
        std::optional<Vector<float, 4>> sv3 = camera->screenToNDC(v3 / v3[3]);
        if (!sv1 || !sv2 || !sv3) continue;  // Out of bounds

        Triangle triangle(*sv1, *sv2, *sv3);
        wireFrame ? triangle.draw() : triangle.fill();
    }
}

void Mesh::setTransform(Matrix<float, 4, 4> transform) {
    this->transform = transform;
}

Matrix<float, 4, 4> Mesh::getTransform() {
    return this->transform;
}

void Mesh::setRotation(Vector<float, 3> rotation) {
    this->transform.set_rotation3(rotation);
    this->rotation = rotation;
}

Vector<float, 3> Mesh::getRotation() {
    return this->rotation;
}

void Mesh::setPosition(Vector<float, 3> position) {
    this->transform.set_position(position);
}

Vector<float, 3> Mesh::getPosition() {
    return this->transform.get_position();
}

void Mesh::setCenter(Vector<float, 3> center) {
    Vector<float, 4> center4 = Vector<float, 4>(center);
    for (auto& vertex : vertices) {
        vertex = vertex - center4;
    }
}

Vector<float, 3> Mesh::getCenterOfMass() {
    Vector<float, 3> center = {0, 0, 0};
    for (auto& vertex : vertices) {
        center = center + Vector<float, 3>(vertex);
    }
    return center / vertices.size();
}

void Mesh::setMaterial(const std::string& materialName) {
    if (materials.find(materialName) != materials.end()) {
        currentMaterial = materialName;
    }
}

Material Mesh::getMaterial() {
    return materials[currentMaterial];
}