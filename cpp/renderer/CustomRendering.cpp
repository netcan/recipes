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
#include <iterator>
#include <limits>
#include <utils/TimePerf.hpp>
#include "CustomRendering.h"
#include "imgui.h"
#include "renderer/Geometry.hpp"
#include "renderer/Shader.h"

constexpr Uint32 toSDLColor(const SDL_PixelFormat *format, Color color) {
    return SDL_MapRGB(format, color.r_(), color.g_(), color.b_());
}

void Canvas::drawPixel(Point2i p, const Color& color) {
    if (p.x_() >= 0 && p.y_() >= 0 && p.x_() < surface_->w && p.y_() < surface_->h) {
        auto pixels = reinterpret_cast<Uint32 *>(surface_->pixels);
        pixels[p.y_() * surface_->w + p.x_()] = toSDLColor(surface_->format, color);
    }
}

void Canvas::bresenhamLine(Point2i p0, Point2i p1, const Color& color) {
    auto dx = abs(p1.x_() - p0.x_());
    auto dy = abs(p1.y_() - p0.y_());
    auto slope = dy > dx;

    if (slope) {
        std::swap(p0.x_(), p0.y_());
        std::swap(p1.x_(), p1.y_());
    }

    if (p0.x_() > p1.x_()) {
        std::swap(p0.x_(), p1.x_());
        std::swap(p0.y_(), p1.y_());
    }

    dx = abs(p1.x_() - p0.x_());
    dy = abs(p1.y_() - p0.y_());
    auto error = dx / 2;
    auto y = p0.y_();
    auto ystep = p0.y_() < p1.y_() ? 1 : -1;

    for (int x = p0.x_(); x <= p1.x_(); ++x) {
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
    for (const auto& face: shader_.faces()) {
        std::array<Point3i, 3> screenCoords;
        for (size_t i = 0; i < std::size(screenCoords); ++i) {
            screenCoords[i] = shader_.vertex(face[i]);
        }
        for (int i = 0; i < std::size(screenCoords); i++) {
            canvas_.bresenhamLine(screenCoords[i], screenCoords[(i + 1) % 3], color_);
        }
    }
}

static constexpr Point3f barycentric(Point2i a, Point2i b, Point2i c, Point2i p) {
    // P = uA + vB + (1 - u - v)C;
    //   = uCA + vCB + C
    // uCA + vCB + PC = 0
    // uCAx + vCBx + PCx = 0
    // uCAy + vCBy + PCy = 0
    // return { u, v, 1 - u - v }

    auto ca = a - c, cb = b - c, pc = c - p;
    auto n = Vec{ca.x_(), cb.x_(), pc.x_()}.cross(Vec{ca.y_(), cb.y_(), pc.y_()});
    if (n.z_() == 0) {
        return {1, 1, -1};
    }
    auto u = n.x_() * 1.f/n.z_();
    auto v = n.y_() * 1.f/n.z_();
    return {u, v, 1.f - u - v};

}

void Canvas::triangle(const std::array<Point3i, 3> &vertex, const Shader &shader, ZBuffer &zbuffer) {
    const auto &[va, vb, vc] = vertex;
    auto [minx, maxx] = std::minmax({va.x_(), vb.x_(), vc.x_()});
    auto [miny, maxy] = std::minmax({va.y_(), vb.y_(), vc.y_()});
    int lx = std::max(0, minx);
    int ly = std::max(0, miny);
    int rx = std::min(surface_->w, maxx);
    int ry = std::min(surface_->h, maxy);

    for (int x = lx; x <= rx; ++x) {
        for (int y = ly; y <= ry; ++y) {
            Vec p {x, y};
            auto bcCoord = barycentric(va, vb, vc, p);
            int z = bcCoord.u_() * va.z_() + bcCoord.v_() * vb.z_() + bcCoord.w_() * vc.z_();
            if (bcCoord.u_() < 0 || bcCoord.v_() < 0 || bcCoord.w_() < 0 || z <= zbuffer[point2Index(p)]) {
                continue;
            }
            Color c;
            if (shader.fragment(bcCoord, c))
                continue;
            zbuffer[point2Index(p)] = z;
            drawPixel(p, c);
        }
    }
}

void CustomRendering::triangleDraw() {
    ZBuffer zbuffer((width_ + 1) * (height_ + 1), std::numeric_limits<ZBuffer::value_type>::min());
    for (auto& face: shader_.faces()) {
        std::array<Point3i, 3> screenCoords;
        for (size_t i = 0; i < std::size(screenCoords); ++i) {
            screenCoords[i] = shader_.vertex(face[i]);
        }
        canvas_.triangle(screenCoords, shader_, zbuffer);
    }

    viewerController();
    dumpZbuffer(zbuffer);
    dumpLight();
}

void CustomRendering::viewerController() {
    ImGui::Begin(__FUNCTION__);
    ImGui::DragFloat3("light", light_.data, 0, -1, 1);
    ImGui::DragFloat3("camera", camera_.data, 0, -5, 5);
    ImGui::DragFloat3("cameraUp", cameraUp_.data, 0, -5, 5);
    ImGui::DragFloat3("cameraO", cameraO_.data, 0, -5, 5);
    ImGui::DragInt2("viewO", viewO_.data, 0, -width_, width_);
    ImGui::End();
}

void CustomRendering::dumpLight() {
    // auto light = light_ * 10;
    auto o     = Vec3f((0 + 1.) * width_ / 2., (0 + 1.) * height_ / 2., (0 + 1.) * kDepth / 2);
    auto light = Vec3f((light_.x_() + 1.) * width_ / 2., (light_.y_() + 1.) * height_ / 2., (light_.z_() + 1.) * kDepth / 2);
    canvas_.bresenhamLine(o, light, color_);
}

void CustomRendering::dumpZbuffer(const ZBuffer& zbuffer) {
    if (ImGui::Begin(__FUNCTION__, NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        Point2i p;
        for (p.x_() = 0; p.x_() < width_; ++p.x_()) {
            for (p.y_() = 0; p.y_() < height_; ++p.y_()) {
                auto z = zbuffer[zbufferCanvas_.point2Index(p)];
                zbufferCanvas_.drawPixel(p, {z, z, z});
            }
        }
        ImGui::Image(zbufferCanvas_.refresh(), ImVec2(width_, height_), {0, 1}, {1, 0});
    }
    ImGui::End();
}

static constexpr const char* RenderItems[] = {
    [CustomRendering::WireFrameDraw] = "wire frame draw",
    [CustomRendering::TriangleRasterization] = "triangle rasterization",
};

void CustomRendering::updateWindowSize()
{
    auto availableSz = ImGui::GetContentRegionAvail();
    if ((int)availableSz.x == width_ && (int)availableSz.y == height_) {
        return;
    }
    if (availableSz.x < 0 || availableSz.y < 0) {
        return;
    }
    // width_ = availableSz.x;
    // height_ = availableSz.y;
    // keep squre
    height_ = width_ = std::max(availableSz.x, availableSz.y);
    canvas_ = {width_, height_, render_};
    zbufferCanvas_ = {width_, height_, render_};
}

void CustomRendering::draw() {
    if (!ImGui::Begin(__FUNCTION__)) {
        ImGui::End();
        return;
    }

    static Vec color {1.f, 1.f, 1.f};
    ImGui::ColorEdit3("color", color.data);
    ImGui::Combo("renderType", (int *)&renderType_, RenderItems, std::size(RenderItems));
    color_ = color * 255;
    shader_.dumpInfo();
    updateWindowSize();

    shader_.updateM(viewport(viewO_, width_, height_, -1, 1), projection(camera_.norm()),
                    lookat(camera_, cameraUp_, cameraO_));

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

    ImGui::Image(canvas_.refresh(), ImVec2(width_, height_), {0, 1}, {1, 0});

    ImGui::End();
}
