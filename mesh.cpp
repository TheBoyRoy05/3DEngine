#include "mesh.hpp"

#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>

#include "triangle.hpp"

/**
 * @brief Construct a new Mesh object
 *
 * @param vertices A list of the mesh's vertices in 4D space (x, y, z, w)
 * @param textures A list of the mesh's texture coordinates in 2D space (u, v)
 * @param normals A list of the mesh's normals in 4D space (x, y, z, w)
 * @param triangles A list of triangles, each as a 3x3 matrix of vertex indices
 * @param transform The mesh's transform matrix
 *
 * Sets the mesh's center to its center of mass
 */
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

/**
 * @brief Construct a new Mesh object
 *
 * @param objfilename The filename of the mesh's geometry (vertices, normals, and triangles)
 * @param mtlfilename The filename of the mesh's materials
 *
 * Sets the mesh's center to its center of mass
 */
Mesh::Mesh(const char* objfilename, const char* mtlfilename) {
    parseOBJ(objfilename);
    parseMTL(mtlfilename);
    this->setCenter(this->getCenterOfMass());
}

/**
 * @brief Parses an OBJ file to load the mesh's geometry.
 *
 * This function reads an OBJ file to extract vertex, texture coordinate, and normal data,
 * as well as triangle face definitions. It processes lines with specific prefixes:
 * - "v" for vertices, storing them in the vertices vector as 4D points with w = 1.0.
 * - "vt" for texture coordinates, storing them in the textures vector as 2D points.
 * - "vn" for vertex normals, storing them in the normals vector as 4D points with w = 0.0.
 * - "f" for faces, interpreting them as triangles or quadrilaterals and storing vertex,
 *   texture, and normal indices in the triangles vector.
 *
 * @param filename The path to the OBJ file to be parsed.
 * @throws std::runtime_error If the file cannot be opened.
 */
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

/**
 * @brief Parses a Wavefront MTL file and populates the Mesh's material data.
 *
 * The function reads the contents of the file line-by-line and parses the
 * keywords and values. It populates the Mesh's material data with the parsed
 * values. The function also saves the last material read from the file and
 * sets it as the Mesh's current material.
 *
 * @param filename The path to the MTL file to be parsed.
 */
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

/**
 * @brief Draws the Mesh to the screen using the given Camera.
 *
 * The function renders each triangle in the Mesh's list of triangles.
 * It first transforms the triangle's vertices using the Mesh's current
 * transformation matrix. Then, it projects the transformed vertices using
 * the given Camera's projection matrix. Next, it converts the projected
 * vertices to screen coordinates using the Camera's screenToNDC function.
 * Finally, it renders the triangle to the screen using either the Triangle's
 * draw or fill functions depending on the wireFrame parameter.
 *
 * @param camera The Camera to use for rendering.
 * @param wireFrame Whether to draw the Mesh in wireframe (true) or filled (false).
 */
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

/**
 * @brief Sets the transformation matrix for the Mesh.
 *
 * This function sets the transformation matrix to the given value.
 * The transformation matrix is used to transform the Mesh's vertices
 * before projecting them to the screen.
 *
 * @param transform The transformation matrix to set.
 */
void Mesh::setTransform(Matrix<float, 4, 4> transform) {
    this->transform = transform;
}


/**
 * @brief Retrieves the transformation matrix of the Mesh.
 *
 * This function returns the current transformation matrix used to
 * transform the Mesh's vertices before projecting them to the screen.
 *
 * @return The transformation matrix of the Mesh.
 */
Matrix<float, 4, 4> Mesh::getTransform() {
    return this->transform;
}

/**
 * @brief Sets the rotation of the Mesh.
 *
 * This function sets the rotation of the Mesh by changing the transformation matrix.
 * The rotation is given as Euler angles in the order of ZYX (yaw, pitch, roll).
 *
 * @param rotation The new rotation of the Mesh as Euler angles in radians.
 */
void Mesh::setRotation(Vector<float, 3> rotation) {
    this->transform.set_rotation3(rotation);
    this->rotation = rotation;
}

/**
 * @brief Retrieves the current rotation of the Mesh as Euler angles in radians.
 * @return The current rotation of the Mesh as Euler angles in radians.
 */
Vector<float, 3> Mesh::getRotation() {
    return this->rotation;
}

/**
 * @brief Sets the position of the Mesh.
 *
 * This function updates the transformation matrix to set the position
 * of the Mesh in 3D space using the given position vector.
 *
 * @param position The new position of the Mesh in 3D space.
 */
void Mesh::setPosition(Vector<float, 3> position) {
    this->transform.set_position(position);
}

/**
 * @brief Retrieves the current position of the Mesh in 3D space.
 * @return The current position of the Mesh in 3D space.
 */
Vector<float, 3> Mesh::getPosition() {
    return this->transform.get_position();
}

/**
 * @brief Sets the center of the Mesh to the specified position.
 *
 * This function adjusts all vertices of the Mesh so that the Mesh's center
 * aligns with the given center position. It effectively translates the Mesh
 * in 3D space by subtracting the provided center from each vertex.
 *
 * @param center The new center position of the Mesh in 3D space.
 */
void Mesh::setCenter(Vector<float, 3> center) {
    Vector<float, 4> center4 = Vector<float, 4>(center);
    for (auto& vertex : vertices) {
        vertex = vertex - center4;
    }
}

/**
 * @brief Calculates the center of mass of the Mesh in 3D space.
 *
 * The center of mass is calculated by summing all vertices and dividing by the number of vertices.
 * This is the average position of all vertices in the Mesh.
 *
 * @return The center of mass of the Mesh in 3D space.
 */
Vector<float, 3> Mesh::getCenterOfMass() {
    Vector<float, 3> center = {0, 0, 0};
    for (auto& vertex : vertices) {
        center = center + Vector<float, 3>(vertex);
    }
    return center / vertices.size();
}

/**
 * @brief Scales the Mesh uniformly by a scalar value.
 *
 * This function multiplies each vertex of the Mesh by the given scalar,
 * effectively scaling the Mesh uniformly in all dimensions.
 *
 * @param scale The scalar value to uniformly scale the Mesh.
 */
void Mesh::scale(float scale) {
    for (auto& vertex : vertices) {
        vertex = vertex * scale;
    }
}

/**
 * @brief Scales the Mesh by a vector of scale factors.
 *
 * This function multiplies each vertex of the Mesh by the given vector of scale factors,
 * effectively scaling the Mesh non-uniformly in the different dimensions.
 *
 * @param scale The vector of scale factors to scale the Mesh.
 */
void Mesh::scale(Vector<float, 3> scale) {
    for (auto& vertex : vertices) {
        vertex = vertex * scale;
    }
}

/**
 * @brief Sets the current material of the Mesh by name.
 *
 * This function updates the Mesh's current material to the specified material name,
 * if it exists in the materials map. If the material is not found, the current
 * material remains unchanged.
 *
 * @param materialName The name of the material to set as the current material.
 */

void Mesh::setMaterial(const std::string& materialName) {
    if (materials.find(materialName) != materials.end()) {
        currentMaterial = materialName;
    }
}

/**
 * @brief Retrieves the current material of the Mesh.
 *
 * This function returns the current material stored in the materials map,
 * based on the Mesh's current material name.
 *
 * @return The current material of the Mesh.
 */
Material Mesh::getMaterial() {
    return materials[currentMaterial];
}