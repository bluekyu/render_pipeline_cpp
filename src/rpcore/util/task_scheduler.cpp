#include <render_pipeline/rpcore/util/task_scheduler.h>

// XXX: fix compile error for std::vector<std::string>
#include <vector_string.h>
#include <numeric>

#include "rplibs/yaml.hpp"

namespace rpcore {

TaskScheduler::TaskScheduler(RenderPipeline* pipeline): RPObject("TaskScheduler"), _pipeline(pipeline)
{
	_frame_index = 0;

	load_config();
}

bool TaskScheduler::is_scheduled(const std::string& task_name) const
{
	check_missing_schedule(task_name);
	return std::find(_tasks[_frame_index].begin(), _tasks[_frame_index].end(), task_name) != _tasks[_frame_index].end();
}

void TaskScheduler::step(void)
{
	_frame_index = (_frame_index + 1) % _tasks.size();
}

size_t TaskScheduler::get_num_tasks(void) const
{
	return std::accumulate(_tasks.begin(), _tasks.end(), size_t(0), [](const size_t& sum, const std::vector<std::string>& tasks) {
		return sum + tasks.size();
	});
	return 0;
}

void TaskScheduler::load_config(void)
{
    YAML::Node config_node;
    if (!rplibs::load_yaml_file("/$$rpconfig/task-scheduler.yaml", config_node))
        return;

	for (const auto& frame_node: config_node["frame_cycles"])
	{
		// add frame
		_tasks.push_back({});

		// XXX: omap of yaml-cpp is list.
		for (const auto& frame_name_tasks: frame_node)
		{
			for (const auto& task_name: frame_name_tasks.second)
				_tasks.back().push_back(task_name.as<std::string>());
		}
	}
}

void TaskScheduler::check_missing_schedule(const std::string& task_name) const
{
	bool found = false;
	for (const auto& tasks: _tasks)
	{
		if (std::find(tasks.begin(), tasks.end(), task_name) != tasks.end())
		{
			found = true;
			break;
		}
	}

	if (!found)
		error(std::string("Task '") + task_name + "' is never scheduled and thus will never run!");
}

}	// namespace rpcore
