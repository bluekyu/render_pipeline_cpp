#include "pssm_shadow_stage.h"

#include <graphicsBuffer.h>

#include <render_pipeline/rpcore/render_target.h>
#include <render_pipeline/rpcore/globals.h>

namespace rpplugins {

PSSMShadowStage::RequireType PSSMShadowStage::required_inputs;
PSSMShadowStage::RequireType PSSMShadowStage::required_pipes;

PSSMShadowStage::ProduceType PSSMShadowStage::get_produced_pipes(void) const
{
	return {
        ShaderInput("PSSMShadowAtlas", _target->get_depth_tex()),
        ShaderInput("PSSMShadowAtlasPCF", _target->get_depth_tex(), make_pcf_state()),
	};
}

SamplerState PSSMShadowStage::make_pcf_state(void) const
{
    SamplerState state;
    state.set_minfilter(SamplerState::FT_shadow);
    state.set_magfilter(SamplerState::FT_shadow);
    return state;
}

Texture* PSSMShadowStage::get_shadow_tex(void) const
{
    return _target->get_depth_tex();
}

void PSSMShadowStage::create(void)
{
    _target = create_target("ShadowMap");
    _target->set_size(_split_resolution * _num_splits, _split_resolution);
    _target->add_depth_attachment(32);
    _target->prepare_render(NodePath());

    // Remove all unused display regions
    GraphicsBuffer* internal_buffer = _target->get_internal_buffer();
    internal_buffer->remove_all_display_regions();
    internal_buffer->get_display_region(0)->set_active(false);
    internal_buffer->disable_clears();

    // Set a clear on the buffer instead on all regions
    internal_buffer->set_clear_depth(1);
    internal_buffer->set_clear_depth_active(true);

    // Prepare the display regions
    for (int i=0; i < _num_splits; ++i)
    {
        PT(DisplayRegion) region = internal_buffer->make_display_region(
            i / float(_num_splits),
            i / float(_num_splits) + 1 / float(_num_splits), 0, 1);
        region->set_sort(25 + i);
        region->disable_clears();
        region->set_active(true);
        _split_regions.push_back(region);
    }
}

void PSSMShadowStage::set_shader_input(const ShaderInput& inp)
{
    rpcore::Globals::render.set_shader_input(inp);
}

std::string PSSMShadowStage::get_plugin_id(void) const
{
	return RPCPP_PLUGIN_ID_STRING;
}

}	// namespace rpplugins
