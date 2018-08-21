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

#include "plugin.hpp"

#include <boost/dll/alias.hpp>
#include <boost/any.hpp>

#include <render_pipeline/rpcore/loader.hpp>
#include <render_pipeline/rpcore/image.hpp>

RENDER_PIPELINE_PLUGIN_CREATOR(rpplugins::Plugin)

namespace rpplugins {

Plugin::RequrieType Plugin::require_plugins;

Plugin::Plugin(rpcore::RenderPipeline& pipeline): BasePlugin(pipeline, RPPLUGINS_ID_STRING)
{
}

void Plugin::on_stage_setup()
{
    auto bloom_stage = std::make_unique<BloomStage>(pipeline_);
    bloom_stage_ = bloom_stage.get();
    add_stage(std::move(bloom_stage));

    bloom_stage_->set_num_mips(boost::any_cast<int>(get_setting("num_mipmaps")));
    bloom_stage_->set_remove_fireflies(boost::any_cast<bool>(get_setting("remove_fireflies")));
}

void Plugin::on_pipeline_created()
{
    Texture* dirt_tex = rpcore::RPLoader::load_texture(get_resource("lens_dirt.txo"));
    bloom_stage_->set_shader_input(ShaderInput("LensDirtTex", dirt_tex));
}

}
