#pragma once

#include <nodePath.h>
#include <asyncTask.h>

#include <render_pipeline/rpcore/rpobject.h>

class ComputeNode;
class GenericAsyncTask;

namespace rppanda {
class DirectFrame;
}

namespace rpcore {

class RenderPipeline;
class Image;
class Sprite;
class Text;

/** Widget to show the current exposure. */
class ExposureWidget: public RPObject
{
public:
	ExposureWidget(RenderPipeline* pipeline, NodePath parent);
	~ExposureWidget(void);

private:
	/** Internal method to init the widgets components. */
	void create_components(void);

	/**
	 * Gets called after the pipeline initialized, this extracts the
	 * exposure texture from the stage manager.
	 */
	static AsyncTask::DoneStatus late_init(GenericAsyncTask* task, void* user_data);

	RenderPipeline* _pipeline;
	NodePath _parent;
	NodePath _node;

	std::shared_ptr<Image> _storage_tex = nullptr;
	rppanda::DirectFrame* _bg_frame = nullptr;
	Sprite* _display_img = nullptr;
	Text* _display_txt = nullptr;
	PT(ComputeNode) _cshader_node = nullptr;
	NodePath _cshader_np;
	PT(Shader) _cshader;
};

}
