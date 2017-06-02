#include "render_pipeline/rpcore/stages/combine_velocity_stage.hpp"

#include "render_pipeline/rpcore/render_pipeline.hpp"
#include "render_pipeline/rpcore/render_target.hpp"

namespace rpcore {

CombineVelocityStage::RequireType CombineVelocityStage::required_inputs_;
CombineVelocityStage::RequireType CombineVelocityStage::required_pipes_ = { "GBuffer" };

CombineVelocityStage::ProduceType CombineVelocityStage::get_produced_pipes(void) const
{
    return {
        ShaderInput("CombinedVelocity", target_->get_color_tex()),
    };
}

void CombineVelocityStage::create(void)
{
    stereo_mode_ = pipeline_.get_setting<bool>("pipeline.stereo_mode");

    target_ = create_target("CombineVelocity");
    target_->add_color_attachment(LVecBase4i(16, 16, 0, 0));
    if (stereo_mode_)
        target_->set_layers(2);
    target_->prepare_buffer();
}

void CombineVelocityStage::reload_shaders(void)
{
    target_->set_shader(load_shader({ "combine_velocity.frag.glsl" }, stereo_mode_));
}

std::string CombineVelocityStage::get_plugin_id(void) const
{
    return std::string("render_pipeline_internal");
}

}
