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

void CustomRendering::drawPixel(Point p, const ImVec4& color) {
    if (p.x >= 0 && p.y >= 0 && p.x < surface_->w && p.y < surface_->h) {
        auto pixels = reinterpret_cast<Uint32 *>(surface_->pixels);
        pixels[p.y * surface_->w + p.x] = toSDLColor(surface_->format, color);
    }
}

void CustomRendering::bresenhamLine(Point p0, Point p1, const ImVec4& color) {
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
            auto v0 = vec_cast<Vec2f>(model_.verts_[face[j]]);
            auto v1 = vec_cast<Vec2f>(model_.verts_[face[(j + 1) % 3]]);
            int x0 = (v0.x + 1.) * canvasSize_.x / 2.;
            int y0 = (v0.y + 1.) * canvasSize_.y / 2.;
            int x1 = (v1.x + 1.) * canvasSize_.x / 2.;
            int y1 = (v1.y + 1.) * canvasSize_.y / 2.;
            bresenhamLine({x0, y0}, {x1, y1}, color_);
        }
    }
}

static constexpr Vec3f barycentric(Point a, Point b, Point c, Point p) {
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

void CustomRendering::triangle(Point a, Point b, Point c, const ImVec4& color)
{
    auto [minx, maxx] = std::minmax({a.x, b.x, c.x});
    auto [miny, maxy] = std::minmax({a.y, b.y, c.y});
    int lx = std::max(0, minx);
    int ly = std::max(0, miny);
    int rx = std::min(canvasSize_.x, maxx);
    int ry = std::min(canvasSize_.y, maxy);

    for (int x = lx; x <= rx; ++x) {
        for (int y = ly; y <= ry; ++y) {
            Vec p {x, y};
            auto bcCoord = barycentric(a, b, c, p);
            if (bcCoord.u < 0 || bcCoord.v < 0 || bcCoord.w < 0) {
                continue;
            }
            drawPixel(p, color);
        }
    }
}

void CustomRendering::triangleDraw() {
    Vec light { 0., 0., -1. };
    utils::high_resolution_clock::duration duration;
    {
        utils::TimePerf perf{duration};
        for (const auto &face : model_.faces_) {
            Vec2i screenCoords[3];
            Vec3f worldCoords[3];
            for (size_t i = 0; i < std::size(screenCoords); ++i) {
                const auto &v = model_.verts_[face[i]];
                screenCoords[i] = Vec2i((v.x + 1.) * canvasSize_.x / 2., (v.y + 1.) * canvasSize_.y / 2.);
                worldCoords[i] = v;
            }
            auto n = normalize(cross((worldCoords[2] - worldCoords[0]), (worldCoords[1] - worldCoords[0])));
            if (auto intensity = light * n; intensity > 0) {
                triangle(screenCoords[0], screenCoords[1], screenCoords[2],
                        ImVec4(intensity * color_.x, intensity * color_.y, intensity * color_.z, 1));
            }
        }
    }
    printf("triangle elapsed: %lld us per loop\n", std::chrono::duration_cast<std::chrono::microseconds>(duration).count());
}

static constexpr const char* RenderItems[] = {
    [CustomRendering::WireFrameDraw] = "wire frame draw",
    [CustomRendering::TriangleRasterization] = "triangle rasterization",
};


void CustomRendering::draw() {
    ImGui::Begin(__FUNCTION__);
    ImGui::ColorEdit4("color", (float *)&color_);
    ImGui::Text("surface: %p texture: %p", surface_.get(), texture_.get());
    ImGui::Text("vertex: %zu faces: %zu", model_.verts_.size(), model_.faces_.size());
    ImGui::Combo("renderType", (int*)&renderType_, RenderItems, std::size(RenderItems));

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

    SDL_UpdateTexture(texture_.get(), NULL, surface_->pixels, surface_->pitch);
    SDL_FillRect(surface_.get(), NULL, 0x000000);
    ImGui::Image(texture_.get(), vec_cast<ImVec2>(canvasSize_), {0, 1}, {1, 0});

    ImGui::End();
}
