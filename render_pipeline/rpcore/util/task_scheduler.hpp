/**
 * Render Pipeline C++
 *
 * Copyright (c) 2014-2016 tobspr <tobias.springer1@gmail.com>
 * Copyright (c) 2016-2017 Center of Human-centered Interaction for Coexistence.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial
 * portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
 * LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#pragma once

#include <vector>

#include <render_pipeline/rpcore/rpobject.hpp>

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
