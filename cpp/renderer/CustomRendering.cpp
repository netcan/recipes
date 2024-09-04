/*************************************************************************
    > File Name: CustomRendering.cpp
    > Author: Netcan
    > Blog: https://netcan.github.io/
    > Mail: netcan1996@gmail.com
    > Created Time: 2024-08-16 22:31
************************************************************************/
#include <SDL_rect.h>
#include <cstdio>
#include <cstdlib>
#include "CustomRendering.h"

void CustomRendering::drawPixel(Point p) {
    if (p.x >= 0 && p.y >= 0 && p.x < surface_->w && p.y < surface_->h) {
        auto pixels = reinterpret_cast<Uint32 *>(surface_->pixels);
        pixels[p.y * surface_->w + p.x] = toSDLColor(surface_->format, color_);
    }
}

void CustomRendering::bresenhamLine(Point p0, Point p1) {
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
            drawPixel({y, x});
        } else {
            drawPixel({x, y});
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
    ImGui::Text("vertex: %zu faces: %zu", model_.verts_.size(), model_.faces_.size());
    for (const auto& face: model_.faces_) {
        for (int j = 0; j < 3; j++) {
            auto v0 = vec_cast<Vec2f>(model_.verts_[face[j]]);
            auto v1 = vec_cast<Vec2f>(model_.verts_[face[(j + 1) % 3]]);
            int x0 = (v0.x + 1.) * canvasSize_.x / 2.;
            int y0 = (v0.y + 1.) * canvasSize_.y / 2.;
            int x1 = (v1.x + 1.) * canvasSize_.x / 2.;
            int y1 = (v1.y + 1.) * canvasSize_.y / 2.;
            bresenhamLine({x0, y0}, {x1, y1});
        }
    }
}

void CustomRendering::triangle(Point a, Point b, Point c)
{
    int lx = std::max(0, std::min({a.x, b.x, c.x}));
    int ly = std::max(0, std::min({a.y, b.y, c.y}));
    int rx = std::min(canvasSize_.x, std::max({a.x, b.x, c.x}));
    int ry = std::min(canvasSize_.y, std::max({a.y, b.y, c.y}));

    for (int x = lx; x <= rx; ++x) {
        for (int y = ly; y <= ry; ++y) {
            Vec p {x, y};
            auto ab = vec_cast<Vec3i>(b - a), bc = vec_cast<Vec3i>(c - b), ca = vec_cast<Vec3i>(a - c);
            auto ap = vec_cast<Vec3i>(p - a), bp = vec_cast<Vec3i>(p - b), cp = vec_cast<Vec3i>(p - c);
            auto c1 = cross(ab, ap).z, c2 = cross(bc, bp).z, c3 = cross(ca, cp).z;
            // whether a point belongs to triangle
            if (((c1 >= 0) && (c2 >= 0) && (c3 >= 0)) || ((c1 <= 0) && (c2 <= 0) && (c3 <= 0))) {
                drawPixel(p);
            }
        }
    }
}

void CustomRendering::triangleDraw() {
    triangle({10, 70}, {50, 160}, {70, 80});
    triangle({180, 50}, {150, 1}, {70, 180});
    triangle({180, 150}, {120, 160}, {130, 180});
}

static constexpr const char* RenderItems[] = {
    [CustomRendering::WireFrameDraw] = "wire frame draw",
    [CustomRendering::TriangleRasterization] = "triangle rasterization",
};


void CustomRendering::draw() {
    ImGui::Begin(__FUNCTION__);
    ImGui::ColorEdit4("color", (float *)&color_);
    ImGui::Text("surface: %p texture: %p", surface_.get(), texture_.get());
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
