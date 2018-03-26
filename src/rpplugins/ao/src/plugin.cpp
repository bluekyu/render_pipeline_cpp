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

#include <boost/any.hpp>
#include <boost/dll/alias.hpp>

#include <render_pipeline/rpcore/stages/ambient_stage.hpp>

RENDER_PIPELINE_PLUGIN_CREATOR(rpplugins::Plugin)

namespace rpplugins {

Plugin::RequrieType Plugin::require_plugins;

Plugin::Plugin(rpcore::RenderPipeline& pipeline): BasePlugin(pipeline, RPPLUGIN_ID_STRING)
{
}

Plugin::~Plugin() = default;

void Plugin::on_stage_setup()
{
    auto stage = std::make_unique<AOStage>(pipeline_);

    stage->set_quality(boost::any_cast<std::string>(get_setting("blur_quality")));

    add_stage(std::move(stage));

    // Make the stages use our output
    rpcore::AmbientStage::get_global_required_pipes().push_back("AmbientOcclusion");
}

}
