/*************************************************************************
    > File Name: Shader.h
    > Author: Netcan
    > Blog: https://netcan.github.io/
    > Mail: netcan1996@gmail.com
    > Created Time: 2024-09-07 20:59
************************************************************************/
#pragma once

#include "Model.h"
#include "renderer/Geometry.hpp"
constexpr size_t kDepth = 255;

inline const char* LoadEnv(const char* name, const char* defaultValue) {
    if (auto v = std::getenv(name)) {
        return v;
    }
    return defaultValue;
}

struct Shader {
    Shader(int& width, int& height, Vec3f& light) : width_(width), height_(height), light_(light) {}
    const auto& faces() const { return model_.faces_; }

    Point3i vertex(const Model::FaceIndex& index) {
        const auto& v      = model_.verts_[index.vIndex];
        const auto& uv     = model_.uv_[index.uvIndex];
        const auto& normal = model_.normal_[index.nIndex];

        varyingUv_[index.nth] = Point2i(uv.x * texture_.width_, uv.y * texture_.height_);
        varyingIntensity_[index.nth] = std::clamp(-normalize(light_) * normal, 0.f, 1.f);

        return Point3i((v.x + 1.) * width_ / 2., (v.y + 1.) * height_ / 2., (v.z + 1.) * kDepth / 2);
    }

    bool fragment(const Point3f& bar, Color& color) const {
        auto intensity = bar * varyingIntensity_;

        Vec uvP{
            bar * Vec{varyingUv_[0].x, varyingUv_[1].x, varyingUv_[2].x},
            bar * Vec{varyingUv_[0].y, varyingUv_[1].y, varyingUv_[2].y},
        };
        color = vcast<Color>(texture_.get(vcast<Point2i>(uvP)) * intensity);
        return false;
    }

    void dumpInfo() const {
        ImGui::Text("vertex: %zu vt: %zu normal: %zu faces: %zu", model_.verts_.size(), model_.uv_.size(),
                    model_.normal_.size(), model_.faces_.size());
    }

private:
    const int&           width_;
    const int&           height_;
    const Vec3f&         light_;
    const Model          model_{LoadEnv("MODEL", "renderer/object/AfricanHead.obj")};
    const Texture texture_{LoadEnv("TEXTURE", "renderer/object/AfricanHeadDiffuse.tga")};

    std::array<Point2i, 3> varyingUv_;
    Vec3f                  varyingIntensity_;
};
