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

#include "smaa_plugin.hpp"

#include <boost/dll/alias.hpp>

#include <lens.h>
#include <texture.h>

#include <render_pipeline/rpcore/globals.hpp>
#include <render_pipeline/rppanda/showbase/showbase.hpp>
#include <render_pipeline/rpcore/loader.hpp>

#include "smaa_stage.hpp"
#include "jitters.hpp"

RENDER_PIPELINE_PLUGIN_CREATOR(rpplugins::SMAAPlugin)

namespace rpplugins {

class SMAAPlugin::Impl
{
public:
    Impl(SMAAPlugin& self);

    void on_pre_render_update();

    /** Internal method to compute the SMAA sub-pixel frame offsets. */
    void compute_jitters();

    /** Loads all required textures. */
    void load_textures();

    size_t get_history_length() const;

    void update_jitter_pattern();

public:
    static RequrieType require_plugins_;

    SMAAPlugin& self_;

    std::vector<LVecBase2> jitters_;
    int jitter_index_;
    SMAAStage* smaa_stage_;
};

SMAAPlugin::RequrieType SMAAPlugin::Impl::require_plugins_;

SMAAPlugin::Impl::Impl(SMAAPlugin& self): self_(self)
{
}

void SMAAPlugin::Impl::on_pre_render_update()
{
    // Apply jitter for temporal aa
    if (smaa_stage_->use_reprojection())
    {
        float jitter_scale = self_.get_setting<rpcore::FloatType>("jitter_amount");
        const LVecBase2 jitter = jitters_[jitter_index_] * jitter_scale;

        rpcore::Globals::base->get_cam_lens()->set_film_offset(jitter);
        smaa_stage_->set_jitter_index(jitter_index_);

        // Increase jitter index
        jitter_index_ += 1;
        if (jitter_index_ >= static_cast<int>(jitters_.size()))
            jitter_index_ = 0;
    }
}

void SMAAPlugin::Impl::compute_jitters()
{
    jitter_index_ = 0;
    float scale = 1.0f / float(rpcore::Globals::native_resolution.get_x());

    // Reduce jittering to 35% to avoid flickering
    scale *= 0.35f;

    for (const LVecBase2& xy: JITTERS.at(self_.get_setting<rpcore::EnumType>("jitter_pattern")))
    {
        jitters_.push_back((xy * 2 - 1) * scale * 0.5f);
    }
}

void SMAAPlugin::Impl::load_textures()
{
    PT(Texture) search_tex = rpcore::RPLoader::load_texture(self_.get_resource("search_tex.png"));
    PT(Texture) area_tex = rpcore::RPLoader::load_texture(self_.get_resource("area_tex.png"));

    for (Texture* tex: {search_tex, area_tex})
    {
        tex->set_minfilter(SamplerState::FT_linear);
        tex->set_magfilter(SamplerState::FT_linear);
        tex->set_wrap_u(SamplerState::WM_clamp);
        tex->set_wrap_v(SamplerState::WM_clamp);
    }

    smaa_stage_->set_area_tex(area_tex);
    smaa_stage_->set_search_tex(search_tex);
}

size_t SMAAPlugin::Impl::get_history_length() const
{
    if (self_.get_setting<rpcore::BoolType>("use_reprojection"))
        return jitters_.size();
    return 1;
}

void SMAAPlugin::Impl::update_jitter_pattern()
{
    compute_jitters();
}

// ************************************************************************************************

SMAAPlugin::SMAAPlugin(rpcore::RenderPipeline& pipeline): BasePlugin(pipeline, RPPLUGINS_ID_STRING), impl_(std::make_unique<Impl>(*this))
{
}

SMAAPlugin::RequrieType& SMAAPlugin::get_required_plugins() const
{
    return impl_->require_plugins_;
}

void SMAAPlugin::on_stage_setup()
{
    const bool use_reprojection = get_setting<rpcore::BoolType>("use_reprojection");
    if (use_reprojection)
        impl_->compute_jitters();

    auto smaa_stage = std::make_unique<SMAAStage>(pipeline_, use_reprojection);
    impl_->smaa_stage_ = smaa_stage.get();
    add_stage(std::move(smaa_stage));

    impl_->load_textures();
}

void SMAAPlugin::on_pre_render_update()
{
    impl_->on_pre_render_update();
}

void SMAAPlugin::on_window_resized()
{
    impl_->compute_jitters();
}

}
