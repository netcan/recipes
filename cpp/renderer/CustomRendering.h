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
#include "renderer/Shader.h"
#include "utils/Delector.hpp"
#include "Geometry.hpp"
#include "Model.h"
#pragma once

using ZBuffer = std::vector<uint8_t>;
struct Canvas {
    Canvas(int w, int h, SDL_Renderer *render)
        : surface_(SDL_CreateRGBSurfaceWithFormat(0, w, h, 0, SDL_PIXELFORMAT_RGB888)),
          texture_(SDL_CreateTextureFromSurface(render, surface_.get())) {}

    void *refresh() {
        SDL_UpdateTexture(texture_.get(), NULL, surface_->pixels, surface_->pitch);
        SDL_FillRect(surface_.get(), NULL, 0x000000);
        return texture_.get();
    }

    constexpr size_t point2Index(Point2i p) {
        return  p.y * surface_->w + p.x;
    }

    void drawPixel(Point2i p, const Color& color);
    void triangle(const std::array<Point3i, 3> &vertex, const Shader &shader, ZBuffer &zbuffer);
    void bresenhamLine(Point2i p0, Point2i p1, const Color &color);

private:
    std::unique_ptr<SDL_Surface, utils::Delector<SDL_FreeSurface>> surface_;
    std::unique_ptr<SDL_Texture, utils::Delector<SDL_DestroyTexture>> texture_;
};

struct CustomRendering {
    CustomRendering(SDL_Renderer *render) : render_(render) { }
    void draw();
    void updateWindowSize();

    enum RenderType: int {
        WireFrameDraw,
        TriangleRasterization,
    };

private:
    void wireFrameDraw();

private:
    void triangleDraw();
    void dumpZbuffer(const ZBuffer& zbuffer);

private:
    SDL_Renderer *render_ {};

    int width_ = 960;
    int height_ = 720;

    Canvas canvas_ { width_, height_, render_ };
    Canvas zbufferCanvas_ { width_, height_, render_ };

    Shader shader_ {width_, height_};
    Color color_ {255, 255, 255};
    RenderType renderType_ {TriangleRasterization};
};

