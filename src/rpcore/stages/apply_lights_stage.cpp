#include "render_pipeline/rpcore/stages/apply_lights_stage.hpp"

#include "render_pipeline/rpcore/render_pipeline.hpp"
#include "render_pipeline/rpcore/render_target.hpp"

namespace rpcore {

ApplyLightsStage::RequireType ApplyLightsStage::required_inputs_ = { "AllLightsData", "IESDatasetTex", "ShadowSourceData" };
ApplyLightsStage::RequireType ApplyLightsStage::required_pipes_ = {
    "GBuffer", "CellIndices", "PerCellLights", "ShadowAtlas",
    "ShadowAtlasPCF", "CombinedVelocity", "PerCellLightsCounts" };

ApplyLightsStage::ProduceType ApplyLightsStage::get_produced_pipes(void) const
{
    return {
        ShaderInput("ShadedScene", target_->get_color_tex()),
    };
}

void ApplyLightsStage::create(void)
{
    stereo_mode_ = pipeline_.get_setting<bool>("pipeline.stereo_mode");

    target_ = create_target("ApplyLights");
    target_->add_color_attachment(16);
    if (stereo_mode_)
        target_->set_layers(2);
    target_->prepare_buffer();
}

void ApplyLightsStage::reload_shaders(void)
{
    target_->set_shader(load_shader({ "apply_lights.frag.glsl" }, stereo_mode_));
}

std::string ApplyLightsStage::get_plugin_id(void) const
{
    return std::string("render_pipeline_internal");
}

}
