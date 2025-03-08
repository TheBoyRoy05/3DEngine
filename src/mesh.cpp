#include "mesh.hpp"

#include "parser.hpp"
#include "triangle.hpp"

/**
 * @brief Constructs a Mesh from the specified model file.
 *
 * This constructor initializes the Mesh by parsing the model file
 * located at the given path. It uses a Parser to read the file and
 * populate the Mesh's internal data structures with objects and materials.
 * After parsing, it sets the Mesh's center to its calculated center of mass.
 *
 * @param modelPath The path to the model file to be loaded.
 */
Mesh::Mesh(const std::string& modelPath) {
    Parser parser(objects, materials);
    parser.parse(modelPath);
    this->setCenter(this->getCenterOfMass());
}

/**
 * @brief Destructor for the Mesh class.
 *
 * This destructor releases resources held by the Mesh, specifically freeing
 * the SDL_Surface associated with each material's image. It iterates through
 * the materials map and calls SDL_FreeSurface on each material's image to
 * prevent memory leaks.
 */
Mesh::~Mesh() {
    for (auto& [name, mat] : materials) {
        SDL_FreeSurface(mat.image);
    }
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
    for (auto& [name, obj] : objects) {
        for (auto& triangle : obj.triangles) {
            Matrix<float, 4, 4> vertices_temp{triangle->v1, triangle->v2, triangle->v3};

            // Set homogeneous coordinate to 1
            vertices_temp.set_position({1.0f, 1.0f, 1.0f});
            Matrix<float, 3, 4> vertices = (camera->getProjection() *
                                            (camera->getView() *
                                             (transform *
                                              vertices_temp.transpose())))
                                               .transpose();

            bool outOfBounds = camera->toDeviceCoordinates(vertices);
            if (outOfBounds) continue;

            triangle->setScreenSpaceVertices(vertices);
            wireFrame ? triangle->draw() : triangle->fill();
        }
    }
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
    for (auto& [name, obj] : objects) {
        for (auto& vertex : obj.vertices) {
            vertex = vertex - center4;
        }
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
    int numPoints = 0;
    Vector<float, 3> center = {0, 0, 0};
    for (auto& [name, obj] : objects) {
        for (auto& vertex : obj.vertices) {
            center = center + vertex;
            numPoints++;
        }
    }
    return center / numPoints;
}

void Mesh::printObjects() {
    for (auto& [name, obj] : objects) {
        std::cout << "\nObject: " << name << ":\n";
        std::cout << "\nVertices:\n";
        for (auto& vertex : obj.vertices) {
            vertex.print();
        }
        std::cout << "\nTextures:\n";
        for (auto& texture : obj.textures) {
            texture.print();
        }
        std::cout << "\nNormals:\n";
        for (auto& normal : obj.normals) {
            normal.print();
        }
        int i = 0;
        std::cout << "\nTriangles:\n";
        for (auto& triangle : obj.triangles) {
            std::cout << "\nTriangle: " << ++i << " \n";
            triangle->print();
        }
        std::cout << std::endl;
    }
}

void Mesh::printMaterials() {
    for (auto& [name, material] : materials) {
        std::cout << "\nMaterial: " << name << ":\n";
        std::cout << "Shininess: " << material.shininess << "\n";
        std::cout << "Ambient: ";
        material.ambient.print();
        std::cout << "Diffuse: ";
        material.diffuse.print();
        std::cout << "Specular: ";
        material.specular.print();
        std::cout << "Texture: " << material.texturePath << "\n";
        std::cout << std::endl;
    }
}