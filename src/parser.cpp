#include "parser.hpp"

#include <SDL2/SDL_image.h>

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <sstream>

void Parser::parse(const std::string& modelPath) {
    std::vector<std::string> matFiles = findFilesOfType(modelPath, ".mtl");
    if (matFiles.empty()) throw std::runtime_error("No .mtl file found in: " + modelPath);
    for (auto& mtlFile : matFiles) parseFile(mtlFile);

    std::vector<std::string> objFiles = findFilesOfType(modelPath, ".obj");
    if (objFiles.empty()) throw std::runtime_error("No .obj file found in: " + modelPath);
    parseFile(objFiles[0]);
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

void Parser::parseFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) throw std::runtime_error("Failed to open MTL file: " + std::string(path));

    std::string folderPath = path.substr(0, path.find_last_of('/'));
    std::string fileType = path.substr(path.find_last_of('.'));
    std::string line, currObj, currMtl;

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        if (fileType == ".mtl") parseMTLLine(ss, folderPath, currMtl);
        if (fileType == ".obj") parseOBJLine(ss, currObj, currMtl);
    }

    file.close();
}

void Parser::parseMTLLine(std::stringstream& ss, std::string& folderPath, std::string& currMtl) {
    std::string prefix;
    ss >> prefix;
    if (prefix.empty() || prefix == "#") return;

    if (prefix == "newmtl") {
        ss >> currMtl;
        materials[currMtl] = Material{currMtl};
    }

    if (currMtl.empty()) {
        currMtl = "default";
        materials[currMtl] = Material{currMtl};
    }

    if (prefix == "Ka")
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

        if (!materials[currMtl].image)
            std::cerr << "Failed to load image: " << IMG_GetError() << std::endl;
    }
}

void Parser::parseOBJLine(std::stringstream& ss, std::string& currObj, std::string& currMtl) {
    std::string prefix;
    ss >> prefix;
    if (prefix.empty() || prefix == "#" || prefix == "mtllib") return;

    if (prefix == "o") {
        ss >> currObj;
        objects[currObj] = Object{currObj};
    }

    if (currObj.empty()) {
        currObj = "default";
        objects[currObj] = Object{currObj};
    }

    if (prefix == "v")
        objects[currObj].vertices.push_back(std::make_unique<Vector<float, 3>>(readLine<3>(ss)));
    else if (prefix == "vt")
        objects[currObj].textures.push_back(std::make_unique<Vector<float, 2>>(readLine<2>(ss)));
    else if (prefix == "vn")
        objects[currObj].normals.push_back(std::make_unique<Vector<float, 3>>(readLine<3>(ss)));
    else if (prefix == "usemtl")
        ss >> currMtl;

    if (materials.find(currMtl) == materials.end()) {
        materials[currMtl] = Material{currMtl};
        materials[currMtl].image = nullptr;
    }

    if (prefix == "f") parseFace(ss, currObj, currMtl);
}

void Parser::parseFace(std::stringstream& ss, std::string& objName, std::string& mtlName) {
    std::vector<uint32_t> vi, vti, vni;
    std::string vertexData;

    // Use references to avoid copying vectors
    auto& vertices = objects[objName].vertices;
    auto& textures = objects[objName].textures;
    auto& normals = objects[objName].normals;
    auto& triangles = objects[objName].triangles;

    while (ss >> vertexData) {
        std::stringstream vs(vertexData);
        std::string v, vt, vn;
        std::getline(vs, v, '/');   // vertex index
        std::getline(vs, vt, '/');  // texture index (may be empty)
        std::getline(vs, vn, '/');  // normal index (may be empty)

        // Convert to index, handling empty values
        size_t vidx = v.empty() ? 0 : std::stoi(v);
        size_t vtidx = (vt.empty() || vt == " ") ? 0 : std::stoi(vt);
        size_t vnidx = vn.empty() ? 0 : std::stoi(vn);

        // Handle negative indexing
        if (vidx < 0) vidx += vertices.size() + 1;
        if (!vt.empty() && vtidx < 0) vtidx += textures.size() + 1;
        if (!vn.empty() && vnidx < 0) vnidx += normals.size() + 1;

        // Adjust for 0-based indexing and check validity
        vi.push_back(vidx - 1);
        vti.push_back(vtidx > 0 && vtidx <= textures.size() ? vtidx - 1 : UINT32_MAX);
        vni.push_back(vnidx > 0 && vnidx <= normals.size() ? vnidx - 1 : UINT32_MAX);
    }

    // Create triangles
    for (uint32_t i = 1; i < vi.size() - 1; i++) {
        // if (triangles.size() >= 10) break;

        Vector<float, 2> defaultTexture;
        Vector<float, 3> defaultNormal;

        // Compute normal if any normal index is missing
        if (vni[0] == UINT32_MAX || vni[i] == UINT32_MAX || vni[i + 1] == UINT32_MAX) {
            auto newNormal = std::make_unique<Vector<float, 3>>(
                ((*vertices[vi[i + 1]]) - (*vertices[vi[0]]))
                    .cross((*vertices[vi[i]]) - (*vertices[vi[0]]))
                    .normalize());

            // Store the new normal and update missing indices
            normals.push_back(std::move(newNormal));
            size_t newNormalIdx = normals.size() - 1;
            if (vni[0] == UINT32_MAX) vni[0] = newNormalIdx;
            if (vni[i] == UINT32_MAX) vni[i] = newNormalIdx;
            if (vni[i + 1] == UINT32_MAX) vni[i + 1] = newNormalIdx;
        }

        // Create the triangle with proper references
        triangles.push_back(std::make_unique<Triangle>(
            vertices[vi[0]].get(), vertices[vi[i]].get(), vertices[vi[i + 1]].get(),
            vti[0] != UINT32_MAX ? textures[vti[0]].get() : &defaultTexture,
            vti[i] != UINT32_MAX ? textures[vti[i]].get() : &defaultTexture,
            vti[i + 1] != UINT32_MAX ? textures[vti[i + 1]].get() : &defaultTexture,
            normals[vni[0]].get(),
            normals[vni[i]].get(),
            normals[vni[i + 1]].get(),
            materials[mtlName]));
    }
}
