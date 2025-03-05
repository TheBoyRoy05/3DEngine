#include "parser.hpp"

#include <SDL2/SDL_image.h>

#include <filesystem>
#include <fstream>
#include <sstream>

void Parser::parse(const std::string& modelPath) {
    std::vector<std::string> matFiles = findFilesOfType(modelPath, ".mtl");
    if (matFiles.empty()) throw std::runtime_error("No .mtl file found in: " + modelPath);
    for (auto& mtlFile : matFiles) parseMTL(mtlFile);

    std::vector<std::string> objFiles = findFilesOfType(modelPath, ".obj");
    if (objFiles.empty()) throw std::runtime_error("No .obj file found in: " + modelPath);
    parseOBJ(objFiles[0]);
}

std::vector<std::string> Parser::findFilesOfType(const std::string& folderPath, const std::string& fileType) {
    std::vector<std::string> files;
    for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
        if (entry.is_regular_file() && entry.path().extension() == fileType) {
            files.push_back(entry.path().string());
        }
    }
    return files;
}

void Parser::parseMTL(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) throw std::runtime_error("Failed to open MTL file: " + std::string(path));

    std::string folderPath = path.substr(0, path.find_last_of('/'));
    std::string line, prefix;
    std::string currMtl;

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        ss >> prefix;

        if (prefix == "newmtl") {
            ss >> currMtl;
            materials[currMtl] = Material{currMtl};
        }

        if (currMtl.empty())
            continue;
        else if (prefix == "Ka")
            materials[currMtl].ambient = readLine<3>(ss);
        else if (prefix == "Kd")
            materials[currMtl].diffuse = readLine<3>(ss);
        else if (prefix == "Ks")
            materials[currMtl].specular = readLine<3>(ss);
        else if (prefix == "Ns")
            ss >> materials[currMtl].shininess;
        else if (prefix == "map_Kd") {
            ss >> materials[currMtl].texturePath;
            materials[currMtl].texturePath = folderPath + "/" + materials[currMtl].texturePath;

            materials[currMtl].image = IMG_Load(materials[currMtl].texturePath.c_str());
            if (!materials[currMtl].image) {
                std::cerr << "Failed to load image: " << IMG_GetError() << std::endl;
                return;
            }
        }
    }

    file.close();
}

void Parser::parseOBJ(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) throw std::runtime_error("Failed to open file: " + std::string(path));

    std::string line;
    std::string currObj;
    std::string currMtl;

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        parseOBJLine(ss, currObj, currMtl);
    }
    file.close();
}

void Parser::parseOBJLine(std::stringstream& ss, std::string& currObj, std::string& currMtl) {
    std::string prefix;
    ss >> prefix;

    if (prefix == "o") {
        ss >> currObj;
        objects[currObj] = Object{currObj};
    }

    if (currObj.empty())
        return;
    else if (prefix == "v")
        objects[currObj].vertices.push_back(readLine<3>(ss));
    else if (prefix == "vt")
        objects[currObj].textures.push_back(readLine<2>(ss));
    else if (prefix == "vn")
        objects[currObj].normals.push_back(readLine<3>(ss));
    else if (prefix == "usemtl")
        ss >> currMtl;
    else if (prefix == "f")
        parseFace(ss, currObj, currMtl);
}

void Parser::parseFace(std::stringstream& ss, std::string& objName, std::string& mtlName) {
    std::vector<uint32_t> vi, vti, vni;
    std::string vertexData;

    std::vector<Vector<float, 3>>& vertices = objects[objName].vertices;
    std::vector<Vector<float, 2>>& textures = objects[objName].textures;
    std::vector<Vector<float, 3>>& normals = objects[objName].normals;

    while (ss >> vertexData) {
        std::stringstream vs(vertexData);
        std::string v, vt, vn;
        std::getline(vs, v, '/');
        std::getline(vs, vt, '/');
        std::getline(vs, vn, '/');

        // convert to index
        int32_t vidx = v.empty() ? 0 : std::stoi(v);
        int32_t vtidx = vt.empty() ? 0 : std::stoi(vt);
        int32_t vnidx = vn.empty() ? 0 : std::stoi(vn);

        // handle negative indexing
        if (vidx < 0) vidx += vertices.size() + 1;
        if (vtidx < 0) vtidx += textures.size() + 1;
        if (vnidx < 0) vnidx += normals.size() + 1;

        // adjust for 0 indexing
        vi.push_back(vidx - 1);
        vti.push_back(vtidx - 1);
        vni.push_back(vnidx - 1);
    }

    // check if material exists
    if (materials.find(mtlName) == materials.end()) {
        throw std::runtime_error("Material not found: " + mtlName);
    }

    // create triangles
    for (uint32_t i = 1; i < vi.size() - 1; i++) {
        objects[objName].triangles.push_back(std::make_unique<Triangle>(
            vertices[vi[0]], vertices[vi[i]], vertices[vi[i + 1]],
            textures[vti[0]], textures[vti[i]], textures[vti[i + 1]],
            normals[vni[0]], normals[vni[i]], normals[vni[i + 1]],
            materials[mtlName]));
    }
}