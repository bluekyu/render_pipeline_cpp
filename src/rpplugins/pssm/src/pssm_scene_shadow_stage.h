#pragma once

#include <render_pipeline/rpcore/render_stage.h>

#include <boost/optional.hpp>

#include <camera.h>
#include <orthographicLens.h>

namespace rpplugins {

/**
 * This stage creates the shadow map which covers the whole important part
 * of the scene. This is required because the shadow cascades only cover the
 * view frustum, but many plugins (VXGI, EnvMaps) require a shadow map.
 */
class PSSMSceneShadowStage: public rpcore::RenderStage
{
public:
    PSSMSceneShadowStage(rpcore::RenderPipeline& pipeline);

	virtual RequireType& get_required_inputs(void) const { return required_inputs; }
	virtual RequireType& get_required_pipes(void) const { return required_pipes; }
    virtual ProduceType get_produced_inputs(void) const;
	virtual ProduceType get_produced_pipes(void) const;

	virtual void create(void) override;
    virtual void update(void) override;

    virtual void set_shader_input(const ShaderInput& inp) override;

    SamplerState make_pcf_state(void) const;

    void request_focus(const LVecBase3f& focus_point, float focus_size);

    LMatrix4f get_mvp(void) const;

	void set_resolution(int resolution);
    void set_sun_vector(const LVecBase3f& sun_vector);
    void set_sun_distance(float sun_distance);

	const boost::optional<std::pair<LVecBase3f, float>>& get_last_focus(void) const;

private:
	virtual std::string get_plugin_id(void) const;

	static RequireType required_inputs;
	static RequireType required_pipes;

    int _resolution = 2048;
    LVecBase3f _sun_vector = LVecBase3f(0, 0, 1);
    float _sun_distance = 10.0f;
    PTA_LMatrix4f _pta_mvp;
    boost::optional<std::pair<LVecBase3f, float>> _focus;
    boost::optional<std::pair<LVecBase3f, float>> _last_focus;

    PT(Camera) _camera;
    PT(OrthographicLens) _cam_lens;
    NodePath _cam_node;

	std::shared_ptr<rpcore::RenderTarget> _target = nullptr;
};

inline void PSSMSceneShadowStage::set_resolution(int resolution)
{
    _resolution = resolution;
}

inline void PSSMSceneShadowStage::set_sun_vector(const LVecBase3f& sun_vector)
{
    _sun_vector = sun_vector;
}

inline void PSSMSceneShadowStage::set_sun_distance(float sun_distance)
{
    _sun_distance = sun_distance;
}

inline const boost::optional<std::pair<LVecBase3f, float>>& PSSMSceneShadowStage::get_last_focus(void) const
{
	return _last_focus;
}

}	// namespace rpplugins
