#include "smaa_plugin.hpp"

#include <boost/dll/alias.hpp>
#include <boost/any.hpp>

#include <lens.h>
#include <texture.h>

#include <render_pipeline/rpcore/globals.hpp>
#include <render_pipeline/rppanda/showbase/showbase.hpp>
#include <render_pipeline/rpcore/loader.hpp>

#include "smaa_stage.hpp"
#include "jitters.hpp"

RENDER_PIPELINE_PLUGIN_CREATOR(rpplugins::SMAAPlugin)

namespace rpplugins {

struct SMAAPlugin::Impl
{
    Impl(SMAAPlugin& self);

    void on_pre_render_update(void);

    /** Internal method to compute the SMAA sub-pixel frame offsets. */
    void compute_jitters(void);

    /** Loads all required textures. */
    void load_textures(void);

    size_t get_history_length(void) const;

    void update_jitter_pattern(void);

public:
    static RequrieType require_plugins_;

    SMAAPlugin& self_;

    std::vector<LVecBase2> jitters_;
    int jitter_index_;
    std::shared_ptr<SMAAStage> smaa_stage_;
};

SMAAPlugin::RequrieType SMAAPlugin::Impl::require_plugins_;

SMAAPlugin::Impl::Impl(SMAAPlugin& self): self_(self)
{
}

void SMAAPlugin::Impl::on_pre_render_update(void)
{
    // Apply jitter for temporal aa
    if (smaa_stage_->use_reprojection())
    {
        float jitter_scale = boost::any_cast<float>(self_.get_setting("jitter_amount"));
        LVecBase2 jitter = jitters_[jitter_index_];
        jitter = jitter * jitter_scale;

        rpcore::Globals::base->get_cam_lens()->set_film_offset(jitter);
        smaa_stage_->set_jitter_index(jitter_index_);

        // Increase jitter index
        jitter_index_ += 1;
        if (jitter_index_ >= jitters_.size())
            jitter_index_ = 0;
    }
}

void SMAAPlugin::Impl::compute_jitters(void)
{
    jitter_index_ = 0;
    float scale = 1.0f / float(rpcore::Globals::native_resolution.get_x());

    // Reduce jittering to 35% to avoid flickering
    scale *= 0.35;

    for (const LVecBase2& xy: JITTERS.at(boost::any_cast<std::string>(self_.get_setting("jitter_pattern"))))
    {
        jitters_.push_back((xy * 2 - 1) * scale * 0.5);
    }
}

void SMAAPlugin::Impl::load_textures(void)
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

size_t SMAAPlugin::Impl::get_history_length(void) const
{
    if (boost::any_cast<bool>(self_.get_setting("use_reprojection")))
        return jitters_.size();
    return 1;
}

void SMAAPlugin::Impl::update_jitter_pattern(void)
{
    compute_jitters();
}

// ************************************************************************************************

SMAAPlugin::SMAAPlugin(rpcore::RenderPipeline& pipeline): BasePlugin(pipeline, RPPLUGIN_ID_STRING), impl_(std::make_unique<Impl>(*this))
{
}

SMAAPlugin::RequrieType& SMAAPlugin::get_required_plugins(void) const
{
    return impl_->require_plugins_;
}

void SMAAPlugin::on_stage_setup(void)
{
    const bool use_reprojection = boost::any_cast<bool>(get_setting("use_reprojection"));
    if (use_reprojection)
        impl_->compute_jitters();

    impl_->smaa_stage_ = std::make_shared<SMAAStage>(pipeline_, use_reprojection);
    add_stage(impl_->smaa_stage_);
    impl_->load_textures();
}

void SMAAPlugin::on_pre_render_update(void)
{
    impl_->on_pre_render_update();
}

void SMAAPlugin::on_window_resized(void)
{
    impl_->compute_jitters();
}

}
