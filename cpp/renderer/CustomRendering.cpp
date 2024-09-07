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
#include <iterator>
#include <utils/TimePerf.hpp>
#include "CustomRendering.h"
#include "imgui.h"
#include "renderer/Geometry.hpp"
#include "renderer/Shader.h"

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
    for (const auto& face: shader_.faces()) {
        std::array<Point3i, 3> screenCoords;
        for (size_t i = 0; i < std::size(screenCoords); ++i) {
            screenCoords[i] = shader_.vertex(face[i]);
        }
        for (int i = 0; i < 3; i++) {
            auto v0 = vec_cast<Point2i>(screenCoords[i]);
            auto v1 = vec_cast<Point2i>(screenCoords[(i + 1) % 3]);
            canvas_.bresenhamLine(v0, v1, color_);
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
    auto u = n.x * 1.f/n.z;
    auto v = n.y * 1.f/n.z;
    return {u, v, 1.f - u - v};

}

void Canvas::triangle(const std::array<Point3i, 3> &vertex, const Shader &shader, ZBuffer &zbuffer) {
    const auto &[va, vb, vc] = vertex;
    auto [minx, maxx] = std::minmax({va.x, vb.x, vc.x});
    auto [miny, maxy] = std::minmax({va.y, vb.y, vc.y});
    int lx = std::max(0, minx);
    int ly = std::max(0, miny);
    int rx = std::min(surface_->w, maxx);
    int ry = std::min(surface_->h, maxy);

    for (int x = lx; x <= rx; ++x) {
        for (int y = ly; y <= ry; ++y) {
            Vec p {x, y};
            auto bcCoord = barycentric(vec_cast<Point2i>(va), vec_cast<Point2i>(vb), vec_cast<Point2i>(vc), p);
            int z = bcCoord.u * va.z + bcCoord.v * vb.z + bcCoord.w * vc.z;
            if (bcCoord.u < 0 || bcCoord.v < 0 || bcCoord.w < 0 || z <= zbuffer[point2Index(p)]) {
                continue;
            }
            Color c;
            if (shader.fragment(bcCoord, c))
                continue;
            zbuffer[point2Index(p)] = z;
            drawPixel(p, c);
        }
    }
}

void CustomRendering::triangleDraw() {
    ZBuffer zbuffer((width_ + 1) * (height_ + 1), 0);
    for (auto& face: shader_.faces()) {
        std::array<Point3i, 3> screenCoords;
        for (size_t i = 0; i < std::size(screenCoords); ++i) {
            screenCoords[i] = shader_.vertex(face[i]);
        }
        canvas_.triangle(screenCoords, shader_, zbuffer);
    }

    dumpZbuffer(zbuffer);
    dumpLight();
}

void CustomRendering::dumpLight() {
    // auto light = light_ * 10;
    auto o     = Vec3f((0 + 1.) * width_ / 2., (0 + 1.) * height_ / 2., (0 + 1.) * kDepth / 2);
    auto light = Vec3f((light_.x + 1.) * width_ / 2., (light_.y + 1.) * height_ / 2., (light_.z + 1.) * kDepth / 2);
    canvas_.bresenhamLine(vec_cast<Point2i>(o), vec_cast<Point2i>(light), color_);
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
    if (availableSz.x < 0 || availableSz.y < 0) {
        return;
    }
    width_ = availableSz.x;
    height_ = availableSz.y;
    canvas_ = {width_, height_, render_};
    zbufferCanvas_ = {width_, height_, render_};
}

void CustomRendering::draw() {
    if (!ImGui::Begin(__FUNCTION__)) {
        ImGui::End();
        return;
    }

    static Vec color {1.f, 1.f, 1.f};
    ImGui::ColorEdit3("color", color.data);
    color_ = vec_cast<Color>(color * 255);
    shader_.dumpInfo();
    ImGui::Combo("renderType", (int *)&renderType_, RenderItems, std::size(RenderItems));
    ImGui::DragFloat3("light", light_.data, 0, -1, 1);
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
