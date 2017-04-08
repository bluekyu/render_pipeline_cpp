#pragma once

#include <render_pipeline/rpcore/render_stage.h>

#include <camera.h>
#include <orthographicLens.h>

namespace rpplugins {

/** This stage captures the sky ao by rendering the scene from above. */
class SkyAOCaptureStage: public rpcore::RenderStage
{
public:
	SkyAOCaptureStage(rpcore::RenderPipeline& pipeline);

	virtual RequireType& get_required_inputs(void) const { return required_inputs; }
	virtual RequireType& get_required_pipes(void) const { return required_pipes; }
	virtual ProduceType get_produced_pipes(void) const;
	virtual ProduceType get_produced_inputs(void) const;

	virtual void create(void) override;
	virtual void update(void) override;
	virtual void reload_shaders(void) override;

    void set_resolution(int resolution);
    void set_capture_height(float capture_height);
    void set_max_radius(float max_radius);

private:
	virtual std::string get_plugin_id(void) const;

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

}	// namespace rpplugins
