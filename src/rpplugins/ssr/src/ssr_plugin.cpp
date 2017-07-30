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

#include <render_pipeline/rpcore/stages/ambient_stage.hpp>

#include "ssr_stage.hpp"

RENDER_PIPELINE_PLUGIN_CREATOR(rpplugins::SSRPlugin)

namespace rpplugins {

class SSRPlugin::Impl
{
public:
    static RequrieType require_plugins_;

    std::shared_ptr<SSRStage> ssr_stage_;
};

SSRPlugin::RequrieType SSRPlugin::Impl::require_plugins_;

// ************************************************************************************************

SSRPlugin::SSRPlugin(rpcore::RenderPipeline& pipeline): rpcore::BasePlugin(pipeline, RPPLUGIN_ID_STRING), impl_(std::make_unique<Impl>())
{
}

SSRPlugin::RequrieType& SSRPlugin::get_required_plugins(void) const
{
    return impl_->require_plugins_;
}

void SSRPlugin::on_stage_setup(void)
{
    impl_->ssr_stage_ = std::make_shared<SSRStage>(pipeline_);
    add_stage(impl_->ssr_stage_);

    if (is_plugin_enabled("color_correction"))
        impl_->ssr_stage_->get_global_required_pipes().push_back("FuturePipe::Exposure");
}

}
