/*************************************************************************
    > File Name: CustomRendering.cpp
    > Author: Netcan
    > Blog: https://netcan.github.io/
    > Mail: netcan1996@gmail.com
    > Created Time: 2024-08-16 22:31
************************************************************************/
#include <SDL_rect.h>
#include <cstdio>
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
    for (const auto& face: model_.faces_) {
        for (int j = 0; j < 3; j++) {
            auto v0 = model_.verts_[face[j]];
            auto v1 = model_.verts_[face[(j + 1) % 3]];
            int x0 = (v0.x + 1.) * width_ / 2.;
            int y0 = (v0.y + 1.) * height_ / 2.;
            int x1 = (v1.x + 1.) * width_ / 2.;
            int y1 = (v1.y + 1.) * height_ / 2.;
            bresenhamLine({x0, y0}, {x1, y1});
        }
    }

}


void CustomRendering::draw() {
    ImGui::Begin(__FUNCTION__);
    ImGui::ColorEdit4("color", (float *)&color_);
    ImGui::Text("surface: %p texture: %p", surface_.get(), texture_.get());
    ImGui::Text("vertex: %zu faces: %zu", model_.verts_.size(), model_.faces_.size());

    wireFrameDraw();
    SDL_UpdateTexture(texture_.get(), NULL, surface_->pixels, surface_->pitch);

    ImGui::Image(texture_.get(), ImVec2(width_, height_), {0, 1}, {1, 0});

    ImGui::End();
}
