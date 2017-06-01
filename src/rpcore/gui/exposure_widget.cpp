#include <render_pipeline/rpcore/gui/exposure_widget.h>

#include <computeNode.h>

#include <render_pipeline/rppanda/gui/direct_frame.h>
#include <render_pipeline/rppanda/showbase/showbase.h>

#include <render_pipeline/rpcore/globals.h>
#include <render_pipeline/rpcore/render_pipeline.h>
#include <render_pipeline/rpcore/stage_manager.h>
#include <render_pipeline/rpcore/image.h>
#include <render_pipeline/rpcore/gui/sprite.h>
#include <render_pipeline/rpcore/gui/text.h>
#include <render_pipeline/rpcore/loader.h>

namespace rpcore {

ExposureWidget::ExposureWidget(RenderPipeline* pipeline, NodePath parent): RPObject("ExposureWidget"), _pipeline(pipeline), _parent(parent)
{
	_node = _parent.attach_new_node("ExposureWidgetNode");

	create_components();
}

ExposureWidget::~ExposureWidget(void)
{
	delete _bg_frame;
	delete _display_img;
	delete _display_txt;
}

void ExposureWidget::create_components(void)
{
	_node.hide();

	// Create the texture where the gui component is rendered inside
	_storage_tex = Image::create_2d("ExposureDisplay", 140, 20, "RGBA8");
	_storage_tex->set_clear_color(LColor(0.2, 0.6, 1.0, 1.0));
	_storage_tex->clear_image();

	auto option = std::make_shared<rppanda::DirectFrame::Options>();
	option->frame_color = LColor(0.1, 0.1, 0.1, 1.0);
	option->frame_size = LVecBase4(200, 0, -10, -85);
	option->pos = LVecBase3(0, 0, 0);
	_bg_frame = new rppanda::DirectFrame(_node, option);

    _display_img = new Sprite(_storage_tex->get_texture(), 140, 20, _node, 20, 50);

	Text::Parameters params;
	params.text = "CURRENT EXPOSURE";
	params.parent = _node;
	params.x = 160;
	params.y = 40;
	params.size = 13;
	params.align = "right";
	params.color = LVecBase3(0.8f);
	_display_txt = new Text(params);

	// Create the shader which generates the visualization texture
	_cshader_node = new ComputeNode("ExposureWidget");
	_cshader_node->add_dispatch(140 / 10, 20 / 4, 1);

	_cshader_np = _node.attach_new_node(_cshader_node);

	// Defer the further loading
	Globals::base->do_method_later(1.0f, &ExposureWidget::late_init, "ExposureLateInit", this);
}

AsyncTask::DoneStatus ExposureWidget::late_init(GenericAsyncTask* task, void* user_data)
{
    ExposureWidget* ew = reinterpret_cast<ExposureWidget*>(user_data);
    StageManager* stage_mgr = ew->_pipeline->get_stage_mgr();

    const auto& pipe = stage_mgr->get_pipe("Exposure");
    if (pipe == ShaderInput::get_blank())
    {
        ew->debug("Disabling exposure widget, could not find the exposure data.");
        ew->_node.remove_node();
        return AsyncTask::DS_done;
    }

    ew->_node.show();

    Texture* exposure_tex = pipe.get_texture();
    ew->_cshader = RPLoader::load_shader({std::string("/$$rp/shader/visualize_exposure.compute.glsl")});
    ew->_cshader_np.set_shader(ew->_cshader);
    ew->_cshader_np.set_shader_input("DestTex", ew->_storage_tex->get_texture());
    ew->_cshader_np.set_shader_input("ExposureTex", exposure_tex);

    return AsyncTask::DS_done;
}

}
