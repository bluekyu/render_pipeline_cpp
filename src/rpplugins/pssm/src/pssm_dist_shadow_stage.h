#pragma once

#include <render_pipeline/rpcore/render_stage.h>

#include <camera.h>
#include <orthographicLens.h>

namespace rpplugins {

/** This stage generates a depth map using Variance Shadow Maps for very distant objects. */
class PSSMDistShadowStage: public rpcore::RenderStage
{
public:
    PSSMDistShadowStage(rpcore::RenderPipeline& pipeline);

	virtual RequireType& get_required_inputs(void) const { return required_inputs; }
	virtual RequireType& get_required_pipes(void) const { return required_pipes; }
	virtual ProduceType get_produced_inputs(void) const;
	virtual ProduceType get_produced_pipes(void) const;

	virtual void create(void) override;
	virtual void update(void) override;
	virtual void reload_shaders(void) override;

	virtual void set_shader_input(const ShaderInput& inp) override;

	LMatrix4f get_mvp(void) const;

	void set_resolution(int resolution);
	void set_clip_size(float clip_size);
	void set_sun_distance(float sun_distance);
	void set_sun_vector(const LVecBase3f& sun_vector);

private:
	virtual std::string get_plugin_id(void) const;

	static RequireType required_inputs;
	static RequireType required_pipes;

	int _resolution = 2048;
	float _clip_size = 500.0f;
	float _sun_distance = 8000;
	LVecBase3f _sun_vector = LVecBase3f(0, 0, 1);
	PTA_LMatrix4f _pta_mvp;

	PT(Camera) _camera;
	PT(OrthographicLens) _cam_lens;
	NodePath _cam_node;

	std::shared_ptr<rpcore::RenderTarget> _target = nullptr;
	std::shared_ptr<rpcore::RenderTarget> _target_convert = nullptr;
	std::shared_ptr<rpcore::RenderTarget> _target_blur_v = nullptr;
	std::shared_ptr<rpcore::RenderTarget> _target_blur_h = nullptr;
};

inline void PSSMDistShadowStage::set_resolution(int resolution)
{
	_resolution = resolution;
}

inline void PSSMDistShadowStage::set_clip_size(float clip_size)
{
	_clip_size = clip_size;
}

inline void PSSMDistShadowStage::set_sun_distance(float sun_distance)
{
	_sun_distance = sun_distance;
}

inline void PSSMDistShadowStage::set_sun_vector(const LVecBase3f& sun_vector)
{
	_sun_vector = sun_vector;
}

}	// namespace rpplugins
