#include "godray_stage.hpp"

#include <render_pipeline/rpcore/render_target.hpp>

namespace rpplugins {

GodrayStage::RequireType GodrayStage::required_inputs;
GodrayStage::RequireType GodrayStage::required_pipes = { "ShadedScene", "GBuffer" };

GodrayStage::GodrayStage(rpcore::RenderPipeline& pipeline): RenderStage(pipeline, "GodrayStage")
{
}

GodrayStage::ProduceType GodrayStage::get_produced_pipes(void) const
{
    return {
        ShaderInput("ShadedScene", _target->get_color_tex()),
    };
}

void GodrayStage::create(void)
{
    _target = create_target("ComputeGodrays");
    _target->add_color_attachment(16);
    _target->prepare_buffer();
}

void GodrayStage::reload_shaders(void)
{
    _target->set_shader(load_plugin_shader({"compute_godrays.frag.glsl"}));
}

std::string GodrayStage::get_plugin_id(void) const
{
    return RPPLUGIN_ID_STRING;
}

}    // namespace rpplugins
