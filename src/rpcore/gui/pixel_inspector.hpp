#pragma once

#include <nodePath.h>
#include <asyncTask.h>

#include <render_pipeline/rpcore/rpobject.hpp>

class GenericAsyncTask;

namespace rpcore {

class RenderPipeline;

/** Widget to analyze the rendered pixels, by zooming in. */
class PixelInspector: public RPObject
{
public:
    PixelInspector(RenderPipeline* pipeline);

    /** Shows the inspector. */
    void show(void);

    /** Shows the inspector. */
    void hide(void);

    /** Updates the pixel preview. */
    void update(void);

private:
    /** Internal method to init the widgets components. */
    void create_components(void);

    /**
     * Gets called after the pipeline initialized, this extracts the
     * exposure texture from the stage manager.
     */
    static AsyncTask::DoneStatus late_init(GenericAsyncTask* task, void* user_data);

    RenderPipeline* _pipeline;
    NodePath _node;

    NodePath _zoomer;
};

inline void PixelInspector::hide(void)
{
    _node.hide();
}

}
