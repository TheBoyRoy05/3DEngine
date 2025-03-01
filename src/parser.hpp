#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "material.hpp"
#include "object.hpp"

class Parser {
    private:
    std::unordered_map<std::string, Object>& objects;
    std::unordered_map<std::string, Material>& materials;

    template <size_t N>
    Vector<float, N> readLine(std::stringstream& ss) {
        Vector<float, N> result;
        for (size_t i = 0; i < N; ++i) ss >> result[i];
        return result;
    }

    std::vector<std::string> findFilesOfType(const std::string& folderPath, const std::string& fileType);
    void parseMTL(const std::string& path);
    void parseOBJ(const std::string& path);
    void parseOBJLine(std::stringstream& ss, std::string& currObj, std::string& currMtl);
    void parseFace(std::stringstream& ss, std::string& currObj, std::string& currMtl);

    public:
    Parser(std::unordered_map<std::string, Object>& objects, std::unordered_map<std::string, Material>& materials) : objects(objects), materials(materials) {};
    void parse(const std::string& modelPath);
};