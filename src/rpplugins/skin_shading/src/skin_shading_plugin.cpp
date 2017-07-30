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

#include "skin_shading_plugin.hpp"

#include <boost/dll/alias.hpp>

#include "skin_shading_stage.hpp"

RENDER_PIPELINE_PLUGIN_CREATOR(rpplugins::SkinShadingPlugin)

namespace rpplugins {

class SkinShadingPlugin::Impl
{
public:
    static RequrieType require_plugins_;

    std::shared_ptr<SkinShadingStage> stage_;
};

SkinShadingPlugin::RequrieType SkinShadingPlugin::Impl::require_plugins_;

// ************************************************************************************************

SkinShadingPlugin::SkinShadingPlugin(rpcore::RenderPipeline& pipeline): BasePlugin(pipeline, RPPLUGIN_ID_STRING), impl_(std::make_unique<Impl>())
{
}

SkinShadingPlugin::RequrieType& SkinShadingPlugin::get_required_plugins(void) const
{
    return impl_->require_plugins_;
}

void SkinShadingPlugin::on_stage_setup(void)
{
    impl_->stage_ = std::make_shared<SkinShadingStage>(pipeline_);
    add_stage(impl_->stage_);
}

}
