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

void CustomRendering::draw() {
    ImGui::Begin(__FUNCTION__);
    ImGui::ColorEdit4("color", (float *)&color_);
    ImGui::Text("surface: %p texture: %p", surface_.get(), texture_.get());

    bresenhamLine(13, 20, 600, 40);
    bresenhamLine(20, 13, 40, 400);
    SDL_UpdateTexture(texture_.get(), NULL, surface_->pixels, surface_->pitch);

    ImGui::Image(texture_.get(), ImVec2(width_, height_), {0, 1}, {1, 0});

    ImGui::End();
}
