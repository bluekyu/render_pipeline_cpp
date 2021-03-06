/**
 * Render Pipeline C++
 *
 * Copyright (c) 2014-2016 tobspr <tobias.springer1@gmail.com>
 * Copyright (c) 2016-2017 Center of Human-centered Interaction for Coexistence.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial
 * portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
 * LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "rpcore/gui/loading_screen.hpp"

#include <graphicsEngine.h>

#include "render_pipeline/rpcore/globals.hpp"
#include "render_pipeline/rpcore/gui/sprite.hpp"
#include "render_pipeline/rppanda/showbase/showbase.hpp"

namespace rpcore {

Filename LoadingScreen::Default::image_source = "/$$rp/data/gui/loading_screen_bg.txo";

LoadingScreen::LoadingScreen(const Filename& image_source): RPObject("LoadingScreen"),
    image_source_(image_source)
{
}

LoadingScreen::~LoadingScreen() = default;

void LoadingScreen::create()
{
    trace("Creating loading screen ...");

    const int screen_w = Globals::native_resolution.get_x();
    const int screen_h = Globals::native_resolution.get_y();
    fullscreen_node_ = Globals::base->get_pixel_2dp().attach_new_node("LoadingScreen");
    fullscreen_node_.set_bin("fixed", 10);
    fullscreen_node_.set_depth_test(false);

    // FIXME: hard corded value
    const float image_w = 1920.0f;
    const float image_h = 1080.0f;
    const float scale = (std::min)(screen_w / image_w, screen_h / image_h);

    fullscreen_bg_ = std::make_unique<Sprite>(image_source_, static_cast<int>(image_w * scale),
        static_cast<int>(image_h * scale), fullscreen_node_,
        static_cast<int>(screen_w - image_w * scale) / 2,
        static_cast<int>(screen_h - image_h * scale) / 2, true, false);

    GraphicsEngine* graphics_engine = Globals::base->get_graphics_engine();
    graphics_engine->render_frame();
    graphics_engine->render_frame();
}

void LoadingScreen::remove()
{
    fullscreen_bg_->get_node()->get_texture()->release_all();
    fullscreen_node_.remove_node();
    fullscreen_bg_.reset();
}

}
