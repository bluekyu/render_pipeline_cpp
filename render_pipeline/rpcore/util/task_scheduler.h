#pragma once

#include <vector>

#include <render_pipeline/rpcore/rpobject.h>

namespace rpcore {

class RenderPipeline;

/**
 * This class manages the scheduled tasks and splits them over multiple
 * frames. Plugins can query whether their subtasks should be executed
 * or queued for later frames. Also performs analysis on the task configuration
 * to figure if tasks are distributed uniformly.
 */
class RENDER_PIPELINE_DECL TaskScheduler: public RPObject
{
public:
    TaskScheduler(RenderPipeline* pipeline);

    /** Returns whether a given task is supposed to run this frame. */
    bool is_scheduled(const std::string& task_name) const;

    void step(void);

    /** Returns the total amount of tasks. */
    size_t get_num_tasks(void) const;

    /** Returns the amount of scheduled tasks this frame. */
    size_t get_num_scheduled_tasks(void) const;

private:
    /** Loads the tasks distribution configuration. */
    void load_config(void);

    /**
     * Checks whether the given task is scheduled at some point. This can
     * be used to check whether any task is missing in the task scheduler config.
     */
    void check_missing_schedule(const std::string& task_name) const;

    RenderPipeline* _pipeline;
    int _frame_index;
    std::vector<std::vector<std::string>> _tasks;
};

inline size_t TaskScheduler::get_num_scheduled_tasks(void) const
{
    return _tasks[_frame_index].size();
}

}
