#pragma once

#include <asyncTask.h>

#include <render_pipeline/rpcore/gui/draggable_window.hpp>

class GenericAsyncTask;

namespace rppanda {
class DirectScrolledFrame;
}

namespace rpcore {

class RenderPipeline;
class StageManager;

/** Small tool which displays the order of the graphic pipes. */
class PipeViewer: public DraggableWindow
{
public:
	PipeViewer(RenderPipeline* pipeline, NodePath parent);

	void toggle(void);

private:
	static AsyncTask::DoneStatus update_task(GenericAsyncTask* task, void* user_data);

	/**
	 * Reads the pipes and stages from the stage manager and renders those
	 * into the window.
	 */
	void populate_content(void);

	/** Internal method to create the window components. */
	virtual void create_components(void);

	RenderPipeline* _pipeline;
	int _scroll_width = 8000;
	int _scroll_height = 3000;
	bool _created = false;

	rppanda::DirectScrolledFrame* _content_frame;
	NodePath _content_node;
	NodePath _pipe_node;
	NodePath _stage_node;


	NodePath _pipe_descriptions;
};

}
