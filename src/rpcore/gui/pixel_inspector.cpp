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

#include "rpcore/gui/pixel_inspector.hpp"

#include <cardMaker.h>
#include <graphicsWindow.h>

#include "render_pipeline/rppanda/showbase/showbase.hpp"

#include "render_pipeline/rpcore/globals.hpp"
#include "render_pipeline/rpcore/stage_manager.hpp"
#include "render_pipeline/rpcore/render_pipeline.hpp"
#include "render_pipeline/rpcore/loader.hpp"

namespace rpcore {

PixelInspector::PixelInspector(RenderPipeline* pipeline): RPObject("PixelInspector"), _pipeline(pipeline)
{
    _node = Globals::base->get_pixel_2d().attach_new_node("PixelInspectorNode");
    create_components();
    hide();
}

void PixelInspector::show()
{
    _node.show();
    if (Globals::base->get_render_2d().is_hidden())
        Globals::base->get_render_2d().show();
}

void PixelInspector::update()
{
    if (Globals::base->get_win()->is_of_type(GraphicsWindow::get_class_type()))
    {
        const MouseData& mouse = Globals::base->get_win()->get_pointer(0);
        if (mouse.get_in_window())
        {
            LVecBase3f pos(mouse.get_x(), 1, -mouse.get_y());
            LVecBase2f rel_mouse_pos(mouse.get_x(), Globals::native_resolution.get_y() - mouse.get_y());
            _node.set_pos(pos);
            _zoomer.set_shader_input("mousePos", rel_mouse_pos);
        }
    }
}

void PixelInspector::create_components()
{
    CardMaker card_maker("PixelInspector");
    card_maker.set_frame(-200, 200, -150, 150);
    _zoomer = _node.attach_new_node(card_maker.generate());

    // Defer the further loading
    Globals::base->do_method_later(1.0f, late_init, "PixelInspectorLateInit", this);
    Globals::base->accept("q",
        [](const Event* ev, void* data) {
        reinterpret_cast<PixelInspector*>(data)->show();
    }, this);
    Globals::base->accept("q-up",
        [](const Event* ev, void* data) {
        reinterpret_cast<PixelInspector*>(data)->hide();
    }, this);
}

AsyncTask::DoneStatus PixelInspector::late_init(GenericAsyncTask* task, void* user_data)
{
    PixelInspector* pixel_inspector = reinterpret_cast<PixelInspector*>(user_data);
    PT(Texture) scene_tex = pixel_inspector->_pipeline->get_stage_mgr()->get_pipe("ShadedScene").get_texture();
    pixel_inspector->_zoomer.set_shader(RPLoader::load_shader({
            "/$$rp/shader/default_gui_shader.vert.glsl",
            "/$$rp/shader/pixel_inspector.frag.glsl"}));
    pixel_inspector->_zoomer.set_shader_input("SceneTex", scene_tex);

    return AsyncTask::DS_done;
}

}
