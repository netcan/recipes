/*************************************************************************
    > File Name: Shader.h
    > Author: Netcan
    > Blog: https://netcan.github.io/
    > Mail: netcan1996@gmail.com
    > Created Time: 2024-09-07 20:59
************************************************************************/
#pragma once

#include "Model.h"
constexpr size_t kDepth = 255;
constexpr Vec light{0., 0., -1.};

inline const char* LoadEnv(const char* name, const char* defaultValue) {
    if (auto v = std::getenv(name)) {
        return v;
    }
    return defaultValue;
}

struct Shader {
    Shader(int &width, int &height) : width_(width), height_(height) {}
    const auto &faces() const { return model_.faces_; }

    Point3i vertex(const Model::FaceIndex& index) {
        const auto &v = model_.verts_[index.vIndex];
        varyingWorld_[index.nth] = v;
        const auto &uv = model_.uv_[index.uvIndex];
        varyingUv_[index.nth] = Point2i(uv.x * texture_.width_, uv.y * texture_.height_);
        return Vec3i((v.x + 1.) * width_ / 2., (v.y + 1.) * height_ / 2., (v.z + 1.) * kDepth / 2);
    }

    bool fragment(const Point3f& bar, Color& color) const {
        auto n = normalize(cross((varyingWorld_[2] - varyingWorld_[0]), (varyingWorld_[1] - varyingWorld_[0])));
        auto intensity = light * n;
        if (intensity <= 0) {
            return true;
        }

        Vec uvP{bar.u * varyingUv_[0].x + bar.v * varyingUv_[1].x + bar.w * varyingUv_[2].x,
                bar.u * varyingUv_[0].y + bar.v * varyingUv_[1].y + bar.w * varyingUv_[2].y};
        color = vec_cast<Color>(texture_.get(vec_cast<Point2i>(uvP)) * intensity);
        return false;
    }

    void dumpInfo() const {
        ImGui::Text("vertex: %zu vt: %zu normal: %zu faces: %zu", model_.verts_.size(), model_.uv_.size(),
                    model_.normal_.size(), model_.faces_.size());
    }

private:
    const int& width_;
    const int& height_;
    const Model model_ {LoadEnv("MODEL", "renderer/object/AfricanHead.obj")};
    const Texture texture_ { LoadEnv("TEXTURE", "renderer/object/AfricanHeadDiffuse.tga")};

    std::array<Point2i, 3> varyingUv_;
    std::array<Point3f, 3> varyingWorld_;
};
