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
#include "Geometry.hpp"
#include "imgui.h"
#include "Model.h"
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

    enum RenderType: int {
        WireFrameDraw,
        TriangleRasterization,
    };

private:
    void bresenhamLine(Point2i p0, Point2i p1, const ImVec4& color);
    void drawPixel(Point2i p, const ImVec4& color);
    void wireFrameDraw();

private:
    void triangle(Point3i a, Point3i b, Point3i c, std::vector<int>& zbuffer, const ImVec4& color);
    void triangleDraw();
    constexpr size_t point2Index(Point2i p) {
        return  p.y * canvasSize_.w + p.x;
    }

private:
    SDL_Renderer *render_;
    Vec2i canvasSize_ {960, 720};
    static constexpr size_t kDepth = 255;
    std::unique_ptr<SDL_Surface, Delector<SDL_FreeSurface>> surface_{
        SDL_CreateRGBSurfaceWithFormat(0, canvasSize_.w, canvasSize_.h, 32, SDL_PIXELFORMAT_ARGB8888)};
    std::unique_ptr<SDL_Texture, Delector<SDL_DestroyTexture>> texture_ {
        SDL_CreateTextureFromSurface(render_, surface_.get())
    };
    ImVec4 color_ {0.45f, 0.55f, 0.60f, 1.00f};
    Model model_ {"renderer/AfricanHead.obj"};
    RenderType renderType_ {TriangleRasterization};
};

