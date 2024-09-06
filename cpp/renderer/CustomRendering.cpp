/*************************************************************************
    > File Name: CustomRendering.cpp
    > Author: Netcan
    > Blog: https://netcan.github.io/
    > Mail: netcan1996@gmail.com
    > Created Time: 2024-08-16 22:31
************************************************************************/
#include <SDL_rect.h>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <utils/TimePerf.hpp>
#include "CustomRendering.h"
#include "imgui.h"
#include "renderer/Geometry.hpp"

constexpr Uint32 toSDLColor(const SDL_PixelFormat *format, Color color) {
    return SDL_MapRGB(format, color.r, color.g, color.b);
}

void Canvas::drawPixel(Point2i p, const Color& color) {
    if (p.x >= 0 && p.y >= 0 && p.x < surface_->w && p.y < surface_->h) {
        auto pixels = reinterpret_cast<Uint32 *>(surface_->pixels);
        pixels[p.y * surface_->w + p.x] = toSDLColor(surface_->format, color);
    }
}

void Canvas::bresenhamLine(Point2i p0, Point2i p1, const Color& color) {
    auto dx = abs(p1.x - p0.x);
    auto dy = abs(p1.y - p0.y);
    auto slope = dy > dx;

    if (slope) {
        std::swap(p0.x, p0.y);
        std::swap(p1.x, p1.y);
    }

    if (p0.x > p1.x) {
        std::swap(p0.x, p1.x);
        std::swap(p0.y, p1.y);
    }

    dx = abs(p1.x - p0.x);
    dy = abs(p1.y - p0.y);
    auto error = dx / 2;
    auto y = p0.y;
    auto ystep = p0.y < p1.y ? 1 : -1;

    for (int x = p0.x; x <= p1.x; ++x) {
        if (slope) {
            drawPixel({y, x}, color);
        } else {
            drawPixel({x, y}, color);
        }
        error -= dy;
        if (error < 0) {
            y += ystep;
            error += dx;
        }
    }
}

void CustomRendering::wireFrameDraw()
{
    for (const auto& face: model_.faces_) {
        for (int j = 0; j < 3; j++) {
            auto v0 = vec_cast<Vec2f>(model_.verts_[face[j].vIndex]);
            auto v1 = vec_cast<Vec2f>(model_.verts_[face[(j + 1) % 3].vIndex]);
            int x0 = (v0.x + 1.) * width_ / 2.;
            int y0 = (v0.y + 1.) * height_ / 2.;
            int x1 = (v1.x + 1.) * width_ / 2.;
            int y1 = (v1.y + 1.) * height_ / 2.;
            canvas_.bresenhamLine({x0, y0}, {x1, y1}, color_);
        }
    }
}

static constexpr Point3f barycentric(Point2i a, Point2i b, Point2i c, Point2i p) {
    // P = uA + vB + (1 - u - v)C;
    //   = uCA + vCB + C
    // uCA + vCB + PC = 0
    // uCAx + vCBx + PCx = 0
    // uCAy + vCBy + PCy = 0
    // return { u, v, 1 - u - v }

    auto ca = a - c, cb = b - c, pc = c - p;
    auto n = cross(Vec{ca.x, cb.x, pc.x}, Vec{ca.y, cb.y, pc.y});
    if (n.z == 0) {
        return {1, 1, -1};
    }
    auto u = n.x * 1./n.z;
    auto v = n.y * 1./n.z;
    return {u, v, 1 - u - v};

}


void Canvas::triangle(Point3i a, Point3i b, Point3i c, ZBuffer& zbuffer, const Color& color) {
    auto [minx, maxx] = std::minmax({a.x, b.x, c.x});
    auto [miny, maxy] = std::minmax({a.y, b.y, c.y});
    int lx = std::max(0, minx);
    int ly = std::max(0, miny);
    int rx = std::min(surface_->w, maxx);
    int ry = std::min(surface_->h, maxy);

    for (int x = lx; x <= rx; ++x) {
        for (int y = ly; y <= ry; ++y) {
            Vec p {x, y};
            auto bcCoord = barycentric(vec_cast<Point2i>(a), vec_cast<Point2i>(b), vec_cast<Point2i>(c), p);
            if (bcCoord.u < 0 || bcCoord.v < 0 || bcCoord.w < 0) {
                continue;
            }
            int z = bcCoord.u * a.z + bcCoord.v * b.z + bcCoord.w * c.z;
            if (z > zbuffer[point2Index(p)]) {
                zbuffer[point2Index(p)] = z;
                drawPixel(p, color);
            }
        }
    }
}

void CustomRendering::triangleDraw() {
    constexpr Vec light{0., 0., -1.};
    ZBuffer zbuffer((width_ + 1) * (height_ + 1), 0);
    for (const auto &face : model_.faces_) {
        Point3i screenCoords[3];
        Vec3f worldCoords[3];
        for (size_t i = 0; i < std::size(screenCoords); ++i) {
            const auto &v = model_.verts_[face[i].vIndex];
            screenCoords[i] = Vec3i((v.x + 1.) * width_ / 2., (v.y + 1.) * height_ / 2., (v.z + 1.) * kDepth / 2);
            worldCoords[i] = v;
        }
        auto n = normalize(cross((worldCoords[2] - worldCoords[0]), (worldCoords[1] - worldCoords[0])));
        if (auto intensity = light * n; intensity > 0) {
            canvas_.triangle(screenCoords[0], screenCoords[1], screenCoords[2], zbuffer,
                             Color(intensity * color_.r, intensity * color_.g, intensity * color_.b));
        }
    }
    dumpZbuffer(zbuffer);
}

void CustomRendering::dumpZbuffer(const ZBuffer& zbuffer) {
    if (ImGui::Begin(__FUNCTION__, NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        Point2i p;
        for (p.x = 0; p.x < width_; ++p.x) {
            for (p.y = 0; p.y < height_; ++p.y) {
                auto z = zbuffer[zbufferCanvas_.point2Index(p)];
                zbufferCanvas_.drawPixel(p, {z, z, z});
            }
        }
        ImGui::Image(zbufferCanvas_.refresh(), ImVec2(width_, height_), {0, 1}, {1, 0});
    }
    ImGui::End();
}

static constexpr const char* RenderItems[] = {
    [CustomRendering::WireFrameDraw] = "wire frame draw",
    [CustomRendering::TriangleRasterization] = "triangle rasterization",
};

void CustomRendering::updateWindowSize()
{
    auto availableSz = ImGui::GetContentRegionAvail();
    if ((int)availableSz.x == width_ && (int)availableSz.y == height_) {
        return;
    }
    width_ = availableSz.x;
    height_ = availableSz.y;
    canvas_ = {width_, height_, render_};
    zbufferCanvas_ = {width_, height_, render_};
}

void CustomRendering::draw() {
    ImGui::Begin(__FUNCTION__);

    static float color[3] {1, 1, 1};
    ImGui::ColorEdit3("color", color);
    color_ = Color(color[0] * 255, color[1] * 255, color[2] * 255);

    ImGui::Text("vertex: %zu vt: %zu normal: %zu faces: %zu", model_.verts_.size(), model_.uv_.size(),
                model_.normal_.size(), model_.faces_.size());
    ImGui::Combo("renderType", (int *)&renderType_, RenderItems, std::size(RenderItems));
    updateWindowSize();

    switch (renderType_) {
        case WireFrameDraw: {
            wireFrameDraw();
            break;
        }
        case TriangleRasterization: {
            triangleDraw();
            break;
        }
    }

    ImGui::Image(canvas_.refresh(), ImVec2(width_, height_), {0, 1}, {1, 0});

    ImGui::End();
}
