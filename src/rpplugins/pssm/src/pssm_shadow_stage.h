#pragma once

#include <render_pipeline/rpcore/render_stage.h>

namespace rpplugins {

/** This stage generates the depth-maps used for rendering PSSM. */
class PSSMShadowStage: public rpcore::RenderStage
{
public:
    PSSMShadowStage(rpcore::RenderPipeline& pipeline): RenderStage(pipeline, "PSSMShadowStage") {}

	virtual RequireType& get_required_inputs(void) const { return required_inputs; }
	virtual RequireType& get_required_pipes(void) const { return required_pipes; }
	virtual ProduceType get_produced_pipes(void) const;

	virtual void create(void) override;

    virtual void set_shader_input(const ShaderInput& inp) override;

    SamplerState make_pcf_state(void) const;

    Texture* get_shadow_tex(void) const;

    void set_num_splits(int num_splits);
    void set_split_resolution(int split_resolution);

    const std::vector<PT(DisplayRegion)>& get_split_regions(void) const;

private:
	virtual std::string get_plugin_id(void) const;

	static RequireType required_inputs;
	static RequireType required_pipes;

    int _num_splits = 3;
    int _split_resolution = 512;
    std::vector<PT(DisplayRegion)> _split_regions;

    std::shared_ptr<rpcore::RenderTarget> _target = nullptr;
};

inline void PSSMShadowStage::set_num_splits(int num_splits)
{
    _num_splits = num_splits;
}

inline void PSSMShadowStage::set_split_resolution(int split_resolution)
{
    _split_resolution = split_resolution;
}

inline const std::vector<PT(DisplayRegion)>& PSSMShadowStage::get_split_regions(void) const
{
    return _split_regions;
}

}	// namespace rpplugins
