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

    if (fileType == ".obj") {
        objects[currObj].modelVertices = objects[currObj].vertices;
        objects[currObj].modelNormals = objects[currObj].normals;
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
        if (!currObj.empty()) {
            objects[currObj].modelVertices = objects[currObj].vertices;
            objects[currObj].modelNormals = objects[currObj].normals;
        }

        ss >> currObj;
        objects[currObj] = Object{currObj};

        objects[currObj].vertices.push_back(Vector<float, 3>{0, 0, 0});
        objects[currObj].textures.push_back(Vector<float, 2>{0, 0});
        objects[currObj].normals.push_back(Vector<float, 3>{0, 0, 0});
    }

    if (currObj.empty()) {
        currObj = "default";
        objects[currObj] = Object{currObj};

        objects[currObj].vertices.push_back(Vector<float, 3>{0, 0, 0});
        objects[currObj].textures.push_back(Vector<float, 2>{0, 0});
        objects[currObj].normals.push_back(Vector<float, 3>{0, 0, 0});
    }

    if (prefix == "v")
        objects[currObj].vertices.push_back(readLine<3>(ss));
    else if (prefix == "vt")
        objects[currObj].textures.push_back(readLine<2>(ss));
    else if (prefix == "vn")
        objects[currObj].normals.push_back(readLine<3>(ss));
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
        if (vidx < 0) vidx += vertices.size();
        if (!vt.empty() && vtidx < 0) vtidx += textures.size();
        if (!vn.empty() && vnidx < 0) vnidx += normals.size();

        // Adjust for 0-based indexing and check validity
        vi.push_back(vidx > 0 && vidx <= vertices.size() ? vidx : 0);
        vti.push_back(vtidx > 0 && vtidx <= textures.size() ? vtidx : 0);
        vni.push_back(vnidx > 0 && vnidx <= normals.size() ? vnidx : 0);
    }

    if (vni[0] == 0) {
        size_t newNormalIdx = normals.size();
        normals.push_back((vertices[vi[1]] - vertices[vi[0]]).cross(vertices[vi[2]] - vertices[vi[0]]).normalize());

        for (uint32_t i = 0; i < vi.size(); i++) {
            if (vni[i] == 0) vni[i] = newNormalIdx;
        }
    }

    // Create triangles
    for (uint32_t i = 1; i < vi.size() - 1; i++) {
        uint32_t vidx[] = {vi[0], vi[i], vi[i + 1]};
        uint32_t uvidx[] = {vti[0], vti[i], vti[i + 1]};
        uint32_t nidx[] = {vni[0], vni[i], vni[i + 1]};
        
        triangles.push_back(std::make_unique<Triangle>(vidx, uvidx, nidx, materials[mtlName], objects[objName]));
    }
}
