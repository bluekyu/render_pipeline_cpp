#pragma once

#include <render_pipeline/rpcore/render_stage.hpp>

#include <camera.h>
#include <orthographicLens.h>

namespace rpplugins {

/** This stage captures the sky ao by rendering the scene from above. */
class SkyAOCaptureStage: public rpcore::RenderStage
{
public:
    SkyAOCaptureStage(rpcore::RenderPipeline& pipeline);

    RequireType& get_required_inputs(void) const final { return required_inputs; }
    RequireType& get_required_pipes(void) const final { return required_pipes; }
    ProduceType get_produced_pipes(void) const final;
    ProduceType get_produced_inputs(void) const final;

    void create(void) final;
    void update(void) final;
    void reload_shaders(void) final;

    void set_resolution(int resolution);
    void set_capture_height(float capture_height);
    void set_max_radius(float max_radius);

private:
    std::string get_plugin_id(void) const final;

    static RequireType required_inputs;
    static RequireType required_pipes;

    PTA_LVecBase3f pta_position_;
    int resolution_ = 512;
    float capture_height_ = 100.0f;
    float max_radius_ = 100.0f;

    std::shared_ptr<rpcore::RenderTarget> target_ = nullptr;
    std::shared_ptr<rpcore::RenderTarget> target_convert_ = nullptr;

    PT(Camera) _camera;
    PT(OrthographicLens) _cam_lens;
    NodePath _cam_node;
};

inline void SkyAOCaptureStage::set_resolution(int resolution)
{
    resolution_ = resolution;
}

inline void SkyAOCaptureStage::set_capture_height(float capture_height)
{
    capture_height_ = capture_height;
}

inline void SkyAOCaptureStage::set_max_radius(float max_radius)
{
    max_radius_ = max_radius;
}

}    // namespace rpplugins
