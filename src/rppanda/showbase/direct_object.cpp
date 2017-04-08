#include <render_pipeline/rppanda/showbase/direct_object.h>

#include <asyncTaskManager.h>

#include "render_pipeline/rppanda/showbase/showbase.h"

namespace rppanda {

bool DirectObject::accept(const std::string& ev_name, EventHandler::EventCallbackFunction* func, void* user_data)
{
	return EventHandler::get_global_event_handler()->add_hook(ev_name, func, user_data);
}

bool DirectObject::ignore(const std::string& ev_name)
{
	return EventHandler::get_global_event_handler()->remove_hooks(ev_name);
}

GenericAsyncTask* DirectObject::add_task(GenericAsyncTask::TaskFunc* func, void* user_data, const std::string& name)
{
	PT(GenericAsyncTask) task = new GenericAsyncTask(name, func, user_data);

	assert(task->has_name());

	AsyncTaskManager::get_global_ptr()->add(task);

	return task;
}

GenericAsyncTask* DirectObject::add_task(GenericAsyncTask::TaskFunc* func, void* user_data, const std::string& name, int sort)
{
	PT(GenericAsyncTask) task = new GenericAsyncTask(name, func, user_data);

	assert(task->has_name());

	task->set_sort(sort);

	AsyncTaskManager::get_global_ptr()->add(task);

	return task;
}

int DirectObject::remove_task(const std::string& task_name)
{
	return AsyncTaskManager::get_global_ptr()->remove(AsyncTaskManager::get_global_ptr()->find_tasks(task_name));
}

GenericAsyncTask* DirectObject::do_method_later(float delay, GenericAsyncTask::TaskFunc* func, const std::string& name, void* user_data)
{
	PT(GenericAsyncTask) task = new GenericAsyncTask(name, func, user_data);
	task->set_delay(delay);
	AsyncTaskManager::get_global_ptr()->add(task);

	return task;
}

}	// namespace rppanda
