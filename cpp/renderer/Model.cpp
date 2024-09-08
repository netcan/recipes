/*************************************************************************
    > File Name: Model.cpp
    > Author: Netcan
    > Blog: https://netcan.github.io/
    > Mail: netcan1996@gmail.com
    > Created Time: 2024-09-06 22:51
************************************************************************/
#include "Model.h"
#include <cassert>
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
            iss >> v.x_() >> v.y_() >> v.z_();
            verts_.push_back(v);
        } else if (line.starts_with("vn ")) {
            iss >> trash >> trash;
            Vec3f v{};
            iss >> v.x_() >> v.y_() >> v.z_();
            normal_.push_back(v.normalize());
        } else if (line.starts_with("vt ")) {
            iss >> trash >> trash;
            Vec2f v{};
            iss >> v.x_() >> v.y_();
            uv_.push_back(v);
        } else if (line.starts_with("f ")) {
            std::vector<FaceIndex> f;
            FaceIndex index;
            iss >> trash;
            size_t nth = 0;
            while (iss >> index.vIndex >> trash >> index.uvIndex >> trash >> index.nIndex) {
                f.push_back({nth++, index.vIndex - 1, index.uvIndex - 1, index.nIndex - 1});
            }
            assert(nth == 3);
            faces_.push_back(f);
        }
    }
}

