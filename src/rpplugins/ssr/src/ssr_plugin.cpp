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

#include "ssr_plugin.hpp"

#include <boost/dll/alias.hpp>

#include "ssr_stage.hpp"

RENDER_PIPELINE_PLUGIN_CREATOR(rpplugins::SSRPlugin)

namespace rpplugins {

RENDER_PIPELINE_PLUGIN_DOWNCAST_IMPL(SSRPlugin);

SSRPlugin::RequrieType SSRPlugin::require_plugins_;

// ************************************************************************************************

SSRPlugin::SSRPlugin(rpcore::RenderPipeline& pipeline): rpcore::BasePlugin(pipeline, RPPLUGIN_ID_STRING)
{
}

SSRPlugin::RequrieType& SSRPlugin::get_required_plugins() const
{
    return require_plugins_;
}

void SSRPlugin::on_stage_setup()
{
    auto ssr_stage = std::make_unique<SSRStage>(pipeline_);

    if (is_plugin_enabled("color_correction"))
        ssr_stage->get_global_required_pipes().push_back("FuturePipe::Exposure");

    add_stage(std::move(ssr_stage));
}

}
