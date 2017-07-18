#pragma once

#include <render_pipeline/rpcore/render_stage.hpp>

namespace rpplugins {

/** This stage applies the motion blur, using the screen space velocity vectors. */
class MotionBlurStage: public rpcore::RenderStage
{
public:
    MotionBlurStage(rpcore::RenderPipeline& pipeline): RenderStage(pipeline, "MotionBlurStage") {}

    RequireType& get_required_inputs(void) const final { return required_inputs; }
    RequireType& get_required_pipes(void) const final { return required_pipes; }
    ProduceType get_produced_pipes(void) const final;

    void create(void) final;
    void reload_shaders(void) final;

    void set_per_object_blur(bool per_object_blur);
    void set_tile_size(int tile_size);

private:
    std::string get_plugin_id(void) const final;

    static RequireType required_inputs;
    static RequireType required_pipes;

    bool per_object_blur_;
    int _tile_size;
    std::shared_ptr<rpcore::RenderTarget> _tile_target = nullptr;
    std::shared_ptr<rpcore::RenderTarget> _tile_target_horiz = nullptr;
    std::shared_ptr<rpcore::RenderTarget> _minmax_target = nullptr;
    std::shared_ptr<rpcore::RenderTarget> _pack_target = nullptr;
    std::shared_ptr<rpcore::RenderTarget> _target = nullptr;
    std::shared_ptr<rpcore::RenderTarget> _target_cam_blur = nullptr;
};

inline void MotionBlurStage::set_per_object_blur(bool per_object_blur)
{
    per_object_blur_ = per_object_blur;
}

inline void MotionBlurStage::set_tile_size(int tile_size)
{
    _tile_size = tile_size;
}

}    // namespace rpplugins
