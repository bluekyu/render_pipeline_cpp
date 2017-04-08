#pragma once

#include <render_pipeline/rpcore/render_stage.h>

namespace rpplugins {

/** This stage applies the motion blur, using the screen space velocity vectors. */
class MotionBlurStage: public rpcore::RenderStage
{
public:
	MotionBlurStage(rpcore::RenderPipeline& pipeline): RenderStage(pipeline, "MotionBlurStage") {}

	virtual RequireType& get_required_inputs(void) const { return required_inputs; }
	virtual RequireType& get_required_pipes(void) const { return required_pipes; }
	virtual ProduceType get_produced_pipes(void) const;

	virtual void create(void) override;
	virtual void reload_shaders(void) override;

    void set_per_object_blur(bool per_object_blur);
	void set_tile_size(int tile_size);

private:
	virtual std::string get_plugin_id(void) const;

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

}	// namespace rpplugins
