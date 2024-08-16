/*************************************************************************
    > File Name: CustomRendering.cpp
    > Author: Netcan
    > Blog: https://netcan.github.io/
    > Mail: netcan1996@gmail.com
    > Created Time: 2024-08-16 22:31
************************************************************************/
#include "CustomRendering.h"
#include "imgui/imgui.h"
#include <SDL_rect.h>
#include <cstdio>

static constexpr void drawPixel(int x, int y, const ImVec4& color, SDL_Surface* surface) {
    if (x >= 0 && y >= 0 && x < surface->w && y < surface->h) {
        auto pixels = reinterpret_cast<Uint32*>(surface->pixels);
        pixels[y * surface->w + x] = toSDLColor(surface->format, color);
    }
}

static constexpr void line(int x0, int y0, int x1, int y1, const ImVec4& color, SDL_Surface* surface) {
    for (float t = 0.; t < 1.; t += .01) {
        int x = x0 + (x1 - x0) * t;
        int y = y0 + (y1 - y0) * t;
        drawPixel(x, y, color, surface);
    }
}

void CustomRendering::BresenhamLineDrawing() {
    ImGui::Begin(__FUNCTION__);
    ImGui::ColorEdit4("color", (float *)&color_);
    ImGui::Text("surface: %p texture: %p", surface_.get(), texture_.get());

    line(100, 200, 300, 400, color_, surface_.get());
    SDL_UpdateTexture(texture_.get(), NULL, surface_->pixels, surface_->pitch);

    ImGui::Image(texture_.get(), ImVec2(width_, height_), {0, 1}, {1, 0});

    ImGui::End();
}
