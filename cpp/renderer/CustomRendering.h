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

struct Canvas {
    Canvas(int w, int h, SDL_Renderer *render)
        : surface_(SDL_CreateRGBSurfaceWithFormat(0, w, h, 32, SDL_PIXELFORMAT_ARGB8888)),
          texture_(SDL_CreateTextureFromSurface(render, surface_.get())) {}

    void *refresh() {
        SDL_UpdateTexture(texture_.get(), NULL, surface_->pixels, surface_->pitch);
        SDL_FillRect(surface_.get(), NULL, 0x000000);
        return texture_.get();
    }

    constexpr size_t point2Index(Point2i p) {
        return  p.y * surface_->w + p.x;
    }

    void drawPixel(Point2i p, const ImVec4 &color);
    void triangle(Point3i a, Point3i b, Point3i c, std::vector<int>& zbuffer, const ImVec4& color);
    void bresenhamLine(Point2i p0, Point2i p1, const ImVec4& color);

private:
    std::unique_ptr<SDL_Surface, Delector<SDL_FreeSurface>> surface_;
    std::unique_ptr<SDL_Texture, Delector<SDL_DestroyTexture>> texture_;
};

struct CustomRendering {
    CustomRendering(SDL_Renderer *render) : render_(render) { }
    void draw();

    enum RenderType: int {
        WireFrameDraw,
        TriangleRasterization,
    };

private:
    void wireFrameDraw();

private:
    void triangleDraw();
    void dumpZbuffer(const std::vector<int>& zbuffer);

private:
    SDL_Renderer *render_ {};
    static constexpr size_t kDepth = 255;

    int width_ = 960;
    int height_ = 720;

    Canvas canvas_ { width_, height_, render_ };
    Canvas zbufferCanvas_ { width_, height_, render_ };

    ImVec4 color_ {1., 1., 1., 1.};
    Model model_ {"renderer/AfricanHead.obj"};
    RenderType renderType_ {TriangleRasterization};
};

