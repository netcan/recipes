/*************************************************************************
    > File Name: CustomRendering.h
    > Author: Netcan
    > Blog: https://netcan.github.io/
    > Mail: netcan1996@gmail.com
    > Created Time: 2024-08-16 22:31
************************************************************************/
#include <SDL.h>
#include <SDL_pixels.h>
#include <memory>
#include "imgui.h"
#pragma once

template<auto Fp>
struct Delector {
    template<typename ...Args>
    constexpr auto operator()(Args&&... args) const noexcept {
        return Fp(std::forward<Args>(args)...);
    }
};

constexpr Uint32 toSDLColor(const SDL_PixelFormat *format, ImVec4 color) {
    return SDL_MapRGBA(format, (Uint8)(color.x * 255), (Uint8)(color.y * 255), (Uint8)(color.z * 255),
                       (Uint8)(color.w * 255));
}

struct CustomRendering {
    CustomRendering(SDL_Renderer *render) : render_(render) { }
    void draw();

private:
    void bresenhamLine(int x0, int y0, int x1, int y1);
    void drawPixel(int x, int y);

private:
    SDL_Renderer *render_;
    int width_{640};
    int height_{480};
    std::unique_ptr<SDL_Surface, Delector<SDL_FreeSurface>> surface_{
        SDL_CreateRGBSurfaceWithFormat(0, width_, height_, 32, SDL_PIXELFORMAT_ARGB8888)};
    std::unique_ptr<SDL_Texture, Delector<SDL_DestroyTexture>> texture_ {
        SDL_CreateTextureFromSurface(render_, surface_.get())
    };
    ImVec4 color_ {0.45f, 0.55f, 0.60f, 1.00f};
};

