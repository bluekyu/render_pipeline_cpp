#pragma once

#include <render_pipeline/rpcore/render_stage.hpp>

namespace rpplugins {

/** This stage generates the depth-maps used for rendering PSSM. */
class PSSMShadowStage: public rpcore::RenderStage
{
public:
    PSSMShadowStage(rpcore::RenderPipeline& pipeline): RenderStage(pipeline, "PSSMShadowStage") {}

    RequireType& get_required_inputs(void) const final { return required_inputs; }
    RequireType& get_required_pipes(void) const final { return required_pipes; }
    ProduceType get_produced_pipes(void) const final;

    void create(void) final;

    void set_shader_input(const ShaderInput& inp) final;

    SamplerState make_pcf_state(void) const;

    Texture* get_shadow_tex(void) const;

    void set_num_splits(int num_splits);
    void set_split_resolution(int split_resolution);

    const std::vector<PT(DisplayRegion)>& get_split_regions(void) const;

private:
    std::string get_plugin_id(void) const final;

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

}
