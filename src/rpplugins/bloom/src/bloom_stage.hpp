#pragma once

#include <render_pipeline/rpcore/render_stage.hpp>
#include <render_pipeline/rpcore/image.hpp>

namespace rpplugins {

class BloomStage: public rpcore::RenderStage
{
public:
    BloomStage(rpcore::RenderPipeline& pipeline): RenderStage(pipeline, "BloomStage") {}

    RequireType& get_required_inputs(void) const final { return required_inputs; }
    RequireType& get_required_pipes(void) const final { return required_pipes; }
    ProduceType get_produced_pipes(void) const final;

    void create(void) final;
    void reload_shaders(void) final;

    void set_dimensions(void) final;

    void set_num_mips(int num_mips);

    void set_remove_fireflies(bool remove_fireflies);

private:
    std::string get_plugin_id(void) const final;

    static RequireType required_inputs;
    static RequireType required_pipes;

    bool stereo_mode_ = false;

    int _num_mips = 6;
    bool _remove_fireflies = false;

    std::shared_ptr<rpcore::RenderTarget> _target_firefly = nullptr;
    std::shared_ptr<rpcore::RenderTarget> _target_apply = nullptr;
    std::shared_ptr<rpcore::RenderTarget> _target_extract = nullptr;
    std::shared_ptr<rpcore::Image> _scene_target_img;
    std::vector<std::shared_ptr<rpcore::RenderTarget>> _downsample_targets;
    std::vector<std::shared_ptr<rpcore::RenderTarget>> _upsample_targets;
};

inline void BloomStage::set_num_mips(int num_mips)
{
    _num_mips = num_mips;
}

inline void BloomStage::set_remove_fireflies(bool remove_fireflies)
{
    _remove_fireflies = remove_fireflies;
}

}    // namespace rpplugins
