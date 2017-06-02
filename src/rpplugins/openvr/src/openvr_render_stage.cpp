#include "openvr_render_stage.h"

#include <glgsg.h>

#include <render_pipeline/rppanda/showbase/showbase.hpp>
#include <render_pipeline/rpcore/globals.hpp>
#include <render_pipeline/rpcore/render_target.hpp>

namespace rpplugins {

OpenvrRenderStage::RequireType OpenvrRenderStage::required_inputs_;
OpenvrRenderStage::RequireType OpenvrRenderStage::required_pipes_ = { "ShadedScene" };

void OpenvrRenderStage::create(void)
{
	// without glTextureView
	target_left_ = create_target("left_distortion");
	target_left_->add_color_attachment(8, true);
	target_left_->set_size(render_target_size_);
	target_left_->prepare_buffer();
	target_left_->set_shader_input(ShaderInput("vr_eye", LVecBase4i(0, 0, 0, 0)));

	target_right_ = create_target("right_distortion");
	target_right_->add_color_attachment(8, true);
	target_right_->set_size(render_target_size_);
	target_right_->prepare_buffer();
	target_right_->set_shader_input(ShaderInput("vr_eye", LVecBase4i(1, 0, 0, 0)));
}

void OpenvrRenderStage::reload_shaders(void)
{
	target_left_->set_shader(load_plugin_shader({"openvr_render.frag.glsl"}));
	target_right_->set_shader(load_plugin_shader({"openvr_render.frag.glsl"}));
}

unsigned int OpenvrRenderStage::get_eye_texture(vr::EVREye vr_eye)
{
    GLGraphicsStateGuardian* glgsg = reinterpret_cast<GLGraphicsStateGuardian*>(rpcore::Globals::base->get_win()->get_gsg());
    if (!glgsg)
        return 0;

    const std::shared_ptr<rpcore::RenderTarget> target = vr_eye == vr::EVREye::Eye_Left ? target_left_ : target_right_;
    return reinterpret_cast<GLTextureContext*>(target->get_color_tex()->prepare_now(
        glgsg->get_current_tex_view_offset(), glgsg->get_prepared_objects(), glgsg))->_index;
}

std::string OpenvrRenderStage::get_plugin_id(void) const
{
	return RPPLUGIN_ID_STRING;
}

}	// namespace rpplugins
