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

#include "rpcore/gui/fps_chart.hpp"

#include <computeNode.h>

#include "render_pipeline/rppanda/showbase/showbase.hpp"

#include "render_pipeline/rpcore/globals.hpp"
#include "render_pipeline/rpcore/render_pipeline.hpp"
#include "render_pipeline/rpcore/image.hpp"
#include "render_pipeline/rpcore/gui/sprite.hpp"
#include "render_pipeline/rpcore/gui/text.hpp"
#include "render_pipeline/rpcore/loader.hpp"
#include "render_pipeline/rppanda/task/task_manager.hpp"

namespace rpcore {

FPSChart::FPSChart(RenderPipeline* pipeline, NodePath parent): RPObject("FPSChart"), _pipeline(pipeline), _parent(parent)
{
    _node = _parent.attach_new_node("FPSChartNode");
    create_components();
}

FPSChart::~FPSChart()
{
    delete _display_img;
    delete _display_txt;
    delete _display_txt_bottom;
}

void FPSChart::create_components()
{
    // Create the buffer which stores the last FPS values
    _storage_buffer = Image::create_buffer("FPSValues", 250, "R16");
    _storage_buffer->set_clear_color(LColorf(0));
    _storage_buffer->clear_image();

    _store_index = PTA_int::empty_array(1);
    _store_index[0] = 0;

    _current_ftime = PTA_float::empty_array(1);
    _current_ftime[0] = 16.0f;

    _chart_ms_max = PTA_float::empty_array(1);
    _chart_ms_max[0] = 40.0f;

    // Create the texture where the gui component is rendered inside
    _display_tex = Image::create_2d("FPSChartRender", 250, 120, "RGBA8");
    _display_tex->set_clear_color(LColorf(0));
    _display_tex->clear_image();

    _display_img = new Sprite(_display_tex->get_texture(), 250, 120, _node, 10, 10);

    // Defer the further loading
    Globals::base->get_task_mgr()->do_method_later(0.3f, std::bind(&FPSChart::late_init, this, std::placeholders::_1), "FPSChartInit");
}

AsyncTask::DoneStatus FPSChart::late_init(rppanda::FunctionalTask* task)
{
    _display_txt = new Text("40 ms", _node, 20, 25, 13, "left", LVecBase3f(1), true);
    _display_txt_bottom = new Text("0 ms", _node, 20, 120, 13, "left", LVecBase3f(1), true);

    // Create the shader which generates the visualization texture
    _cshader_node = new ComputeNode("FPSChart");
    _cshader_node->add_dispatch(250 / 10, 120 / 4, 1);
    _cshader_np = _node.attach_new_node(_cshader_node);

    _cshader = RPLoader::load_shader({"/$$rp/shader/fps_chart.compute.glsl"});
    _cshader_np.set_shader(_cshader);
    _cshader_np.set_shader_input("DestTex", _display_tex->get_texture());
    _cshader_np.set_shader_input("FPSValues", _storage_buffer->get_texture());
    _cshader_np.set_shader_input("index", _store_index);
    _cshader_np.set_shader_input("maxMs", _chart_ms_max);

    _update_shader_node = new ComputeNode("FPSChart");
    _update_shader_node->add_dispatch(1, 1, 1);
    _update_shader_np = _node.attach_new_node(_update_shader_node);

    _ushader = RPLoader::load_shader({"/$$rp/shader/fps_chart_update.compute.glsl"});
    _update_shader_np.set_shader(_ushader);
    _update_shader_np.set_shader_input("DestTex", _storage_buffer->get_texture());
    _update_shader_np.set_shader_input("index", _store_index);
    _update_shader_np.set_shader_input("currentData", _current_ftime);

    Globals::base->add_task(std::bind(&FPSChart::update, this, std::placeholders::_1), "UpdateFPSChart", -50);

    return AsyncTask::DS_done;
}

AsyncTask::DoneStatus FPSChart::update(rppanda::FunctionalTask* task)
{
    _store_index[0] = (_store_index[0] + 1) % 250;
    _current_ftime[0] = Globals::clock->get_dt() * 1000.0f;

    double avg_fps = Globals::clock->get_average_frame_rate();
    if (avg_fps > 122.0)
        _chart_ms_max[0] = 10.0f;
    else if (avg_fps > 62)
        _chart_ms_max[0] = 20.0f;
    else if (avg_fps > 32)
        _chart_ms_max[0] = 40.0f;
    else if (avg_fps > 15)
        _chart_ms_max[0] = 70.0f;
    else
        _chart_ms_max[0] = 200.0f;

    _display_txt->set_text(std::to_string(int(_chart_ms_max[0])) + " ms");

    return AsyncTask::DS_cont;
}

}
