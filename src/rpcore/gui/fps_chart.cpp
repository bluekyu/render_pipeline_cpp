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

FPSChart::FPSChart(NodePath parent): RPObject("FPSChart"), parent_(parent)
{
    node_ = parent_.attach_new_node("FPSChartNode");
    create_components();
}

FPSChart::~FPSChart() = default;

void FPSChart::create_components()
{
    // Create the buffer which stores the last FPS values
    storage_buffer_ = Image::create_buffer("FPSValues", 250, "R16");
    storage_buffer_->set_clear_color(LColor(0));
    storage_buffer_->clear_image();

    store_index_ = PTA_int::empty_array(1);
    store_index_[0] = 0;

    current_ftime_ = PTA_float::empty_array(1);
    current_ftime_[0] = 16.0f;

    chart_ms_max_ = PTA_float::empty_array(1);
    chart_ms_max_[0] = 40.0f;

    // Create the texture where the gui component is rendered inside
    display_tex_ = Image::create_2d("FPSChartRender", 250, 120, "RGBA8");
    display_tex_->set_clear_color(LColor(0));
    display_tex_->clear_image();

    display_img_ = std::make_unique<Sprite>(display_tex_->get_texture(), 250, 120, node_, 10, 10);

    // Defer the further loading
    Globals::base->get_task_mgr()->do_method_later(0.3f, std::bind(&FPSChart::late_init, this, std::placeholders::_1), "FPSChartInit");
}

AsyncTask::DoneStatus FPSChart::late_init(rppanda::FunctionalTask* task)
{
    display_txt_ = std::make_unique<Text>("40 ms", node_, 20, 25, 13, "left", LVecBase3(1), true);
    display_txt_bottom_ = std::make_unique<Text>("0 ms", node_, 20, 120, 13, "left", LVecBase3(1), true);

    // Create the shader which generates the visualization texture
    cshader_node_ = new ComputeNode("FPSChart");
    cshader_node_->add_dispatch(250 / 10, 120 / 4, 1);
    cshader_np_ = node_.attach_new_node(cshader_node_);

    cshader_ = RPLoader::load_shader({"/$$rp/shader/fps_chart.compute.glsl"});
    cshader_np_.set_shader(cshader_);
    cshader_np_.set_shader_input("DestTex", display_tex_->get_texture());
    cshader_np_.set_shader_input("FPSValues", storage_buffer_->get_texture());
    cshader_np_.set_shader_input("index", store_index_);
    cshader_np_.set_shader_input("maxMs", chart_ms_max_);

    update_shader_node_ = new ComputeNode("FPSChart");
    update_shader_node_->add_dispatch(1, 1, 1);
    update_shader_np_ = node_.attach_new_node(update_shader_node_);

    ushader_ = RPLoader::load_shader({"/$$rp/shader/fps_chart_update.compute.glsl"});
    update_shader_np_.set_shader(ushader_);
    update_shader_np_.set_shader_input("DestTex", storage_buffer_->get_texture());
    update_shader_np_.set_shader_input("index", store_index_);
    update_shader_np_.set_shader_input("currentData", current_ftime_);

    Globals::base->add_task(std::bind(&FPSChart::update, this, std::placeholders::_1), "UpdateFPSChart", -50);

    return AsyncTask::DS_done;
}

AsyncTask::DoneStatus FPSChart::update(rppanda::FunctionalTask* task)
{
    store_index_[0] = (store_index_[0] + 1) % 250;
    current_ftime_[0] = Globals::clock->get_dt() * 1000.0f;

    double avg_fps = Globals::clock->get_average_frame_rate();
    if (avg_fps > 122.0)
        chart_ms_max_[0] = 10.0f;
    else if (avg_fps > 62)
        chart_ms_max_[0] = 20.0f;
    else if (avg_fps > 32)
        chart_ms_max_[0] = 40.0f;
    else if (avg_fps > 15)
        chart_ms_max_[0] = 70.0f;
    else
        chart_ms_max_[0] = 200.0f;

    display_txt_->set_text(std::to_string(static_cast<int>(chart_ms_max_[0])) + " ms");

    return AsyncTask::DS_cont;
}

}
