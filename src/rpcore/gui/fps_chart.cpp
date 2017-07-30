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

namespace rpcore {

FPSChart::FPSChart(RenderPipeline* pipeline, NodePath parent): RPObject("FPSChart"), _pipeline(pipeline), _parent(parent)
{
    _node = _parent.attach_new_node("FPSChartNode");
    create_components();
}

FPSChart::~FPSChart(void)
{
    delete _display_img;
    delete _display_txt;
    delete _display_txt_bottom;
}

void FPSChart::create_components(void)
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
    Globals::base->do_method_later(0.3f, late_init, "FPSChartInit", this);
}

AsyncTask::DoneStatus FPSChart::late_init(GenericAsyncTask* task, void* user_data)
{
    FPSChart* fc = reinterpret_cast<FPSChart*>(user_data);

    {
        Text::Parameters params;
        params.text = "40 ms";
        params.parent = fc->_node;
        params.x = 20;
        params.y = 25;
        params.size = 13;
        params.align = "left";
        params.color = LVecBase3f(1);
        params.may_change = true;
        fc->_display_txt = new Text(params);
    }
    {
        Text::Parameters params;
        params.text = "0 ms";
        params.parent = fc->_node;
        params.x = 20;
        params.y = 120;
        params.size = 13;
        params.align = "left";
        params.color = LVecBase3f(1);
        params.may_change = true;
        fc->_display_txt_bottom = new Text(params);
    }

    // Create the shader which generates the visualization texture
    fc->_cshader_node = new ComputeNode("FPSChart");
    fc->_cshader_node->add_dispatch(250 / 10, 120 / 4, 1);
    fc->_cshader_np = fc->_node.attach_new_node(fc->_cshader_node);

    fc->_cshader = RPLoader::load_shader({"/$$rp/shader/fps_chart.compute.glsl"});
    fc->_cshader_np.set_shader(fc->_cshader);
    fc->_cshader_np.set_shader_input("DestTex", fc->_display_tex->get_texture());
    fc->_cshader_np.set_shader_input("FPSValues", fc->_storage_buffer->get_texture());
    fc->_cshader_np.set_shader_input("index", fc->_store_index);
    fc->_cshader_np.set_shader_input("maxMs", fc->_chart_ms_max);

    fc->_update_shader_node = new ComputeNode("FPSChart");
    fc->_update_shader_node->add_dispatch(1, 1, 1);
    fc->_update_shader_np = fc->_node.attach_new_node(fc->_update_shader_node);

    fc->_ushader = RPLoader::load_shader({"/$$rp/shader/fps_chart_update.compute.glsl"});
    fc->_update_shader_np.set_shader(fc->_ushader);
    fc->_update_shader_np.set_shader_input("DestTex", fc->_storage_buffer->get_texture());
    fc->_update_shader_np.set_shader_input("index", fc->_store_index);
    fc->_update_shader_np.set_shader_input("currentData", fc->_current_ftime);

    Globals::base->add_task(update, fc, "UpdateFPSChart", -50);

    return AsyncTask::DS_done;
}

AsyncTask::DoneStatus FPSChart::update(GenericAsyncTask* task, void* user_data)
{
    FPSChart* fc = reinterpret_cast<FPSChart*>(user_data);

    fc->_store_index[0] = (fc->_store_index[0] + 1) % 250;
    fc->_current_ftime[0] = Globals::clock->get_dt() * 1000.0f;

    double avg_fps = Globals::clock->get_average_frame_rate();
    if (avg_fps > 122.0)
        fc->_chart_ms_max[0] = 10.0f;
    else if (avg_fps > 62)
        fc->_chart_ms_max[0] = 20.0f;
    else if (avg_fps > 32)
        fc->_chart_ms_max[0] = 40.0f;
    else if (avg_fps > 15)
        fc->_chart_ms_max[0] = 70.0f;
    else
        fc->_chart_ms_max[0] = 200.0f;

    fc->_display_txt->set_text(std::to_string(int(fc->_chart_ms_max[0])) + " ms");

    return AsyncTask::DS_cont;
}

}
