/*************************************************************************
    > File Name: Model.h
    > Author: Netcan
    > Blog: https://netcan.github.io/
    > Mail: netcan1996@gmail.com
    > Created Time: 2024-08-19 22:26
************************************************************************/

#pragma once
#include <vector>
#include <imgui.h>
#include "renderer/Geometry.hpp"
#include "stb/stb_image.h"
#include "utils/Delector.hpp"

struct Model {
    struct FaceIndex {
        size_t vIndex{};
        size_t uvIndex{};
        size_t nIndex{};
    };
    explicit Model(const char *filename);

    std::vector<Vec3f> verts_;
	std::vector<Vec2f> uv_;
	std::vector<Vec3f> normal_;
	std::vector<std::vector<FaceIndex> > faces_;
};

struct Texture {
    explicit Texture(const char *filename) : data_{stbi_load(filename, &width_, &height_, &comp_, 0)} {
        printf("Loaded TGA image with dimensions: %d x %d, and %d components (channels):\n", width_, height_, comp_);
    }

private:
    int width_{};
    int height_{};
    int comp_{};
    std::unique_ptr<unsigned char, utils::Delector<stbi_image_free>> data_{};
};
