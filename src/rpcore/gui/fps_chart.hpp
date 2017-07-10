#pragma once

#include <nodePath.h>
#include <asyncTask.h>

#include <render_pipeline/rpcore/rpobject.hpp>

class ComputeNode;
class GenericAsyncTask;

namespace rpcore {

class RenderPipeline;
class Image;
class Sprite;
class Text;

/** Widget to show the FPS as a chart . */
class FPSChart: public RPObject
{
public:
	FPSChart(RenderPipeline* pipeline, NodePath parent);
	~FPSChart(void);

private:
	/** Internal method to init the widgets components. */
	void create_components(void);

	/**
	 * Gets called after the pipeline initialized, this extracts the
	 * exposure texture from the stage manager.
	 */
	static AsyncTask::DoneStatus late_init(GenericAsyncTask* task, void* user_data);

	/** Updates the widget. */
	static AsyncTask::DoneStatus update(GenericAsyncTask* task, void* user_data);

	RenderPipeline* _pipeline;
	NodePath _parent;
	NodePath _node;

	std::shared_ptr<Image> _storage_buffer = nullptr;
	PTA_int _store_index;
	PTA_float _current_ftime;
	PTA_float _chart_ms_max;
	std::shared_ptr<Image> _display_tex = nullptr;
	Sprite* _display_img = nullptr;

	Text* _display_txt = nullptr;
	Text* _display_txt_bottom = nullptr;

	PT(ComputeNode) _cshader_node = nullptr;
	NodePath _cshader_np;
	PT(Shader) _cshader;

	PT(ComputeNode) _update_shader_node = nullptr;
	NodePath _update_shader_np;
	PT(Shader) _ushader;
};

}
