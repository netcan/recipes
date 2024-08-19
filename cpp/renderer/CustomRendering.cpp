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

void CustomRendering::drawPixel(int x, int y) {
    if (x >= 0 && y >= 0 && x < surface_->w && y < surface_->h) {
        auto pixels = reinterpret_cast<Uint32 *>(surface_->pixels);
        pixels[y * surface_->w + x] = toSDLColor(surface_->format, color_);
    }
}

void CustomRendering::bresenhamLine(int x0, int y0, int x1, int y1) {
    auto dx = abs(x1 - x0);
    auto dy = abs(y1 - y0);
    auto slope = dy > dx;

    if (slope) {
        std::swap(x0, y0);
        std::swap(x1, y1);
    }

    if (x0 > x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    dx = abs(x1 - x0);
    dy = abs(y1 - y0);
    auto error = dx / 2;
    auto y = y0;
    auto ystep = y0 < y1 ? 1 : -1;

    for (int x = x0; x <= x1; ++x) {
        if (slope) {
            drawPixel(y, x);
        } else {
            drawPixel(x, y);
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
            bresenhamLine(x0, y0, x1, y1);
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
