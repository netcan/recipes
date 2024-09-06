/*************************************************************************
    > File Name: Model.cpp
    > Author: Netcan
    > Blog: https://netcan.github.io/
    > Mail: netcan1996@gmail.com
    > Created Time: 2024-09-06 22:51
************************************************************************/
#include "Model.h"
#include <iostream>
#include <fstream>
#include <sstream>

Model::Model(const char *filename) {
    std::ifstream in;
    in.open(filename, std::ifstream::in);
    if (in.fail())
        return;
    std::string line;
    while (!in.eof()) {
        std::getline(in, line);
        std::istringstream iss(line);
        char trash;
        if (line.starts_with("v ")) {
            iss >> trash;
            Vec3f v{};
            iss >> v.x >> v.y >> v.z;
            verts_.push_back(v);
        } else if (line.starts_with("vn ")) {
            iss >> trash;
            Vec3f v{};
            iss >> v.x >> v.y >> v.z;
            normal_.push_back(v);
        } else if (line.starts_with("vt ")) {
            iss >> trash;
            Vec2f v{};
            iss >> v.x >> v.y;
            uv_.push_back(v);
        } else if (line.starts_with("f ")) {
            std::vector<FaceIndex> f;
            FaceIndex index;
            iss >> trash;
            while (iss >> index.vIndex >> trash >> index.uvIndex >> trash >> index.nIndex) {
                f.push_back({index.vIndex - 1, index.uvIndex - 1, index.nIndex - 1});
            }
            faces_.push_back(f);
        }
    }
}
