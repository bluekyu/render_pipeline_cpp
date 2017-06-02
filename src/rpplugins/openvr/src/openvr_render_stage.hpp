#pragma once

#include <render_pipeline/rpcore/render_stage.hpp>

#include <openvr.h>

namespace rpcore {
class RenderTarget;
}

namespace rpplugins {

class OpenvrRenderStage: public rpcore::RenderStage
{
public:
	OpenvrRenderStage(rpcore::RenderPipeline& pipeline): RenderStage(pipeline, "OpenvrRenderStage") {}

	RequireType& get_required_inputs(void) const final { return required_inputs_; }
	RequireType& get_required_pipes(void) const final { return required_pipes_; }

	void create(void) final;
	void reload_shaders(void) final;

	void set_render_target_size(int width, int height);
	unsigned int get_eye_texture(vr::EVREye vr_eye);

private:
	std::string get_plugin_id(void) const final;

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
