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
#include "renderer/Geometry.hpp"

struct Model {
    struct FaceIndex {
        size_t vIndex {};
        size_t uvIndex {};
        size_t nIndex {};
    };
	Model(const char *filename);

	std::vector<Vec3f> verts_;
	std::vector<Vec2f> uv_;
	std::vector<Vec3f> normal_;
	std::vector<std::vector<FaceIndex> > faces_;
};
