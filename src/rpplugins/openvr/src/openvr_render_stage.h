#pragma once

#include <render_pipeline/rpcore/render_stage.h>

#include <openvr.h>

namespace rpcore {
class RenderTarget;
}

namespace rpplugins {

class OpenvrRenderStage: public rpcore::RenderStage
{
public:
	OpenvrRenderStage(rpcore::RenderPipeline& pipeline): RenderStage(pipeline, "OpenvrRenderStage") {}

	virtual RequireType& get_required_inputs(void) const override { return required_inputs_; }
	virtual RequireType& get_required_pipes(void) const override { return required_pipes_; }

	virtual void create(void) override;
	virtual void reload_shaders(void) override;

	void set_render_target_size(int width, int height);
	unsigned int get_eye_texture(vr::EVREye vr_eye);

private:
	virtual std::string get_plugin_id(void) const;

	static RequireType required_inputs_;
	static RequireType required_pipes_;

	std::shared_ptr<rpcore::RenderTarget> target_left_ = nullptr;
	std::shared_ptr<rpcore::RenderTarget> target_right_ = nullptr;

	LVecBase2i render_target_size_;
};

// ************************************************************************************************
inline void OpenvrRenderStage::set_render_target_size(int width, int height)
{
	render_target_size_ = LVecBase2i(width, height);
}

}	// namespace rpplugins
