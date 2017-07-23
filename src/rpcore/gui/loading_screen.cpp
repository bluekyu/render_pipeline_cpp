#include "rpcore/gui/loading_screen.hpp"

#include <graphicsEngine.h>

#include "render_pipeline/rpcore/globals.hpp"
#include "render_pipeline/rpcore/gui/sprite.hpp"
#include "render_pipeline/rppanda/showbase/showbase.hpp"

namespace rpcore {

LoadingScreen::LoadingScreen(RenderPipeline* pipeline, const std::string& image_source): RPObject("LoadingScreen"),
    pipeline(pipeline), image_source(image_source)
{

}

LoadingScreen::~LoadingScreen(void)
{
    delete fullscreen_bg;
}

void LoadingScreen::create(void)
{
    trace("Creating loading screen ...");

    const int screen_w = Globals::native_resolution.get_x();
    const int screen_h = Globals::native_resolution.get_y();
    fullscreen_node = Globals::base->get_pixel_2dp().attach_new_node("LoadingScreen");
    fullscreen_node.set_bin("fixed", 10);
    fullscreen_node.set_depth_test(false);

    // FIXME: hard corded value
    const float image_w = 1920.0f;
    const float image_h = 1080.0f;
    const float scale = (std::min)(screen_w / image_w, screen_h / image_h);

    fullscreen_bg = new Sprite(image_source, int(image_w * scale), int(image_h * scale), fullscreen_node,
        int(screen_w - image_w * scale) / 2, int(screen_h - image_h * scale) / 2, true, false);

    GraphicsEngine* graphics_engine = rppanda::ShowBase::get_global_ptr()->get_graphics_engine();
    graphics_engine->render_frame();
    graphics_engine->render_frame();
}

void LoadingScreen::remove(void)
{
    fullscreen_bg->get_node()->get_texture()->release_all();
    fullscreen_node.remove_node();
    delete fullscreen_bg;
    fullscreen_bg = nullptr;
}

}
