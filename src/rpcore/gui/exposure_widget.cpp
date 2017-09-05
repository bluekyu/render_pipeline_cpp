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

#include "rpcore/gui/exposure_widget.hpp"

#include <computeNode.h>

#include "render_pipeline/rppanda/gui/direct_frame.hpp"
#include "render_pipeline/rppanda/showbase/showbase.hpp"

#include "render_pipeline/rpcore/globals.hpp"
#include "render_pipeline/rpcore/render_pipeline.hpp"
#include "render_pipeline/rpcore/stage_manager.hpp"
#include "render_pipeline/rpcore/image.hpp"
#include "render_pipeline/rpcore/gui/sprite.hpp"
#include "render_pipeline/rpcore/gui/text.hpp"
#include "render_pipeline/rpcore/loader.hpp"

namespace rpcore {

ExposureWidget::ExposureWidget(RenderPipeline* pipeline, NodePath parent): RPObject("ExposureWidget"), _pipeline(pipeline), _parent(parent)
{
    _node = _parent.attach_new_node("ExposureWidgetNode");

    create_components();
}

ExposureWidget::~ExposureWidget()
{
    delete _display_img;
    delete _display_txt;
}

void ExposureWidget::create_components()
{
    _node.hide();

    // Create the texture where the gui component is rendered inside
    _storage_tex = Image::create_2d("ExposureDisplay", 140, 20, "RGBA8");
    _storage_tex->set_clear_color(LColor(0.2, 0.6, 1.0, 1.0));
    _storage_tex->clear_image();

    auto option = std::make_shared<rppanda::DirectFrame::Options>();
    option->frame_color = LColor(0.1, 0.1, 0.1, 1.0);
    option->frame_size = LVecBase4(200, 0, -10, -85);
    option->pos = LVecBase3(0, 0, 0);
    _bg_frame = new rppanda::DirectFrame(_node, option);

    _display_img = new Sprite(_storage_tex->get_texture(), 140, 20, _node, 20, 50);

    _display_txt = new Text("CURRENT EXPOSURE", _node, 160, 40, 13, "right", LVecBase3(0.8f));

    // Create the shader which generates the visualization texture
    _cshader_node = new ComputeNode("ExposureWidget");
    _cshader_node->add_dispatch(140 / 10, 20 / 4, 1);

    _cshader_np = _node.attach_new_node(_cshader_node);

    // Defer the further loading
    Globals::base->do_method_later(1.0f, &ExposureWidget::late_init, "ExposureLateInit", this);
}

AsyncTask::DoneStatus ExposureWidget::late_init(GenericAsyncTask* task, void* user_data)
{
    ExposureWidget* ew = reinterpret_cast<ExposureWidget*>(user_data);
    StageManager* stage_mgr = ew->_pipeline->get_stage_mgr();

    const auto& pipe = stage_mgr->get_pipe("Exposure");
    if (pipe == ShaderInput::get_blank())
    {
        ew->debug("Disabling exposure widget, could not find the exposure data.");
        ew->_node.remove_node();
        return AsyncTask::DS_done;
    }

    ew->_node.show();

    Texture* exposure_tex = pipe.get_texture();
    ew->_cshader = RPLoader::load_shader({"/$$rp/shader/visualize_exposure.compute.glsl"});
    ew->_cshader_np.set_shader(ew->_cshader);
    ew->_cshader_np.set_shader_input("DestTex", ew->_storage_tex->get_texture());
    ew->_cshader_np.set_shader_input("ExposureTex", exposure_tex);

    return AsyncTask::DS_done;
}

}
