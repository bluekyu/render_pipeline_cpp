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

#include "render_pipeline/rpcore/util/task_scheduler.hpp"

#if !defined(_MSC_VER) || _MSC_VER >= 1900
#else
// XXX: fix compile error for std::vector<std::string>
#include <vector_string.h>
#endif
#include <filename.h>

#include <numeric>
#include <algorithm>

#include "rplibs/yaml.hpp"

namespace rpcore {

TaskScheduler::TaskScheduler(): RPObject("TaskScheduler")
{
    frame_index_ = 0;

    load_config();
}

bool TaskScheduler::is_scheduled(const std::string& task_name) const
{
    check_missing_schedule(task_name);
    return std::find(tasks_[frame_index_].cbegin(), tasks_[frame_index_].cend(), task_name) != tasks_[frame_index_].cend();
}

void TaskScheduler::step()
{
    frame_index_ = (frame_index_ + 1) % tasks_.size();
}

size_t TaskScheduler::get_num_tasks() const
{
    return std::accumulate(tasks_.begin(), tasks_.end(), size_t(0), [](const size_t& sum, const std::vector<std::string>& tasks) {
        return sum + tasks.size();
    });
}

void TaskScheduler::load_config()
{
    YAML::Node config_node;
    if (!rplibs::load_yaml_file("/$$rpconfig/task-scheduler.yaml", config_node))
        return;

    for (const auto& frame_node: config_node["frame_cycles"])
    {
        // add frame
        tasks_.push_back({});

        // XXX: omap of yaml-cpp is list.
        for (const auto& frame_name_tasks: frame_node)
        {
            for (const auto& task_name: frame_name_tasks.second)
                tasks_.back().push_back(task_name.as<std::string>());
        }
    }
}

void TaskScheduler::check_missing_schedule(const std::string& task_name) const
{
    bool found = false;
    for (const auto& tasks: tasks_)
    {
        if (std::find(tasks.cbegin(), tasks.cend(), task_name) != tasks.cend())
        {
            found = true;
            break;
        }
    }

    if (!found)
        error(std::string("Task '") + task_name + "' is never scheduled and thus will never run!");
}

}
