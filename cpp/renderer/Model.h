/*************************************************************************
    > File Name: Model.h
    > Author: Netcan
    > Blog: https://netcan.github.io/
    > Mail: netcan1996@gmail.com
    > Created Time: 2024-08-19 22:26
************************************************************************/

#pragma once
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <imgui.h>

struct Model {
	Model(const char *filename) {
        std::ifstream in;
        in.open(filename, std::ifstream::in);
        if (in.fail())
            return;
        std::string line;
        while (!in.eof()) {
            std::getline(in, line);
            std::istringstream iss(line.c_str());
            char trash;
            if (!line.compare(0, 2, "v ")) {
                iss >> trash;
                ImVec4 v{};
                iss >> v.x >> v.y >> v.z;
                verts_.push_back(v);
            } else if (!line.compare(0, 2, "f ")) {
                std::vector<int> f;
                int itrash, idx;
                iss >> trash;
                while (iss >> idx >> trash >> itrash >> trash >> itrash) {
                    f.push_back(idx - 1);
                }
                faces_.push_back(f);
            }
        }
    }

	std::vector<ImVec4> verts_;
	std::vector<std::vector<int> > faces_;
};
