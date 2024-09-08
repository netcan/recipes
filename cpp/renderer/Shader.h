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

inline const char* LoadEnv(const char* name, const char* defaultValue) {
    if (auto v = std::getenv(name)) {
        return v;
    }
    return defaultValue;
}

struct Shader {
    Shader(const Matrix44f& M, Vec3f& light) : M_(M), light_(light) {}
    const auto& faces() const { return model_.faces_; }

    Point3i vertex(const Model::FaceIndex& index) {
        const auto& v      = model_.verts_[index.vIndex].toHomogeneous().toM();
        const auto& uv     = model_.uv_[index.uvIndex];
        const auto& normal = model_.normal_[index.nIndex];

        varyingUv_.setCol(index.nth, Vec2i(uv.x_() * texture_.width_, uv.y_() * texture_.height_));

        varyingIntensity_[index.nth] = std::clamp(-light_.normalize() * normal, 0.f, 1.f);

        return (M_ * v).toV().toAffine();
    }

    bool fragment(const Point3f& bar, Color& color) const {
        auto intensity = bar * varyingIntensity_;

        Vec2f uvP = varyingUv_ * bar.toM();
        color = texture_.get(uvP) * intensity;
        return false;
    }

    void dumpInfo() const {
        ImGui::Text("vertex: %zu vt: %zu normal: %zu faces: %zu", model_.verts_.size(), model_.uv_.size(),
                    model_.normal_.size(), model_.faces_.size());
    }

private:
    const Matrix44f& M_;
    const Vec3f&         light_;
    const Model          model_{LoadEnv("MODEL", "renderer/object/AfricanHead.obj")};
    const Texture        texture_{LoadEnv("TEXTURE", "renderer/object/AfricanHeadDiffuse.tga")};

    Matrixi<2, 3> varyingUv_;
    Vec3f         varyingIntensity_;
};
