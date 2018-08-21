/**
 * Copyright (c) 2008, Carnegie Mellon University.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of Carnegie Mellon University nor the names of
 *    other contributors may be used to endorse or promote products
 *    derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHORS "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * (This is the Modified BSD License.  See also
 * http://www.opensource.org/licenses/bsd-license.php )
 */

/**
 * This is C++ porting codes of direct/src/task/task.py
 */

#include "render_pipeline/rppanda/task/task_manager.hpp"

#include <asyncTaskManager.h>

#include <fmt/ostream.h>

#include "rppanda/task/config_rppanda_task.hpp"

namespace rppanda {

TaskManager* TaskManager::get_global_instance()
{
    static TaskManager instance;
    return &instance;
}

TaskManager::TaskManager() : mgr_(AsyncTaskManager::get_global_ptr()),
    global_clock_(mgr_->get_clock())
{
}

TaskManager::~TaskManager() = default;

TypedReferenceCount* TaskManager::get_current_task() const
{
    return Thread::get_current_thread()->get_current_task();
}

bool TaskManager::has_task_chain(const std::string& chain_name) const
{
    return mgr_->find_task_chain(chain_name) != nullptr;
}

void TaskManager::setup_task_chain(const std::string& chain_name, boost::optional<int> num_threads,
    boost::optional<bool> tick_clock, boost::optional<ThreadPriority> thread_priority,
    boost::optional<double> frame_budget, boost::optional<bool> frame_sync,
    boost::optional<bool> timeslice_priority)
{
    auto chain = mgr_->make_task_chain(chain_name);
    if (num_threads)
        chain->set_num_threads(num_threads.value());
    if (tick_clock)
        chain->set_tick_clock(tick_clock.value());
    if (thread_priority)
        chain->set_thread_priority(thread_priority.value());
    if (frame_budget)
        chain->set_frame_budget(frame_budget.value());
    if (frame_sync)
        chain->set_frame_sync(frame_sync.value());
    if (timeslice_priority)
        chain->set_timeslice_priority(timeslice_priority.value());
}

bool TaskManager::has_task_named(const std::string& task_name) const
{
    return mgr_->find_task(task_name) != nullptr;
}

AsyncTaskCollection TaskManager::get_task_named(const std::string& task_name) const
{
    return mgr_->find_tasks(task_name);
}

AsyncTaskCollection TaskManager::get_task_matching(const GlobPattern& task_pattern) const
{
    return mgr_->find_tasks_matching(task_pattern);
}

AsyncTaskCollection TaskManager::get_all_tasks() const
{
    return mgr_->get_tasks();
}

AsyncTaskCollection TaskManager::get_tasks() const
{
    return mgr_->get_active_tasks();
}

AsyncTaskCollection TaskManager::get_do_laters() const
{
    return mgr_->get_sleeping_tasks();
}

AsyncTask* TaskManager::do_method_later(float delay_time, AsyncTask* task,
    const std::string& name,
    boost::optional<int> sort, boost::optional<int> priority,
    const boost::optional<std::string>& task_chain)
{
    if (delay_time < 0)
        rppanda_task_cat.warning() << fmt::format("do_method_later: added task: {} with negative delay: {}", name, delay_time) << std::endl;

    setup_task(task, name, sort, priority, task_chain);
    task->set_delay(delay_time);
    mgr_->add(task);
    return task;
}

FunctionalTask* TaskManager::do_method_later(float delay_time,
    const FunctionalTask::TaskFunc& func, const std::string& name,
    boost::optional<int> sort, const FunctionalTask::UserDataType& data,
    boost::optional<int> priority,
    const boost::optional<std::string>& task_chain,
    const FunctionalTask::DeathFunc& upon_death)
{
    PT(FunctionalTask) task = new FunctionalTask(func, data, name);
    if (upon_death)
        task->set_upon_death(upon_death);

    do_method_later(delay_time, task, {}, sort, priority, task_chain);

    return task;
}

AsyncTask* TaskManager::add(AsyncTask* task, const std::string& name,
    boost::optional<int> sort, boost::optional<int> priority,
    const boost::optional<std::string>& task_chain)
{
    setup_task(task, name, sort, priority, task_chain);
    mgr_->add(task);
    return task;
}

FunctionalTask* TaskManager::add(const FunctionalTask::TaskFunc& func, const std::string& name,
    boost::optional<int> sort, const FunctionalTask::UserDataType& data,
    boost::optional<int> priority,
    const boost::optional<std::string>& task_chain,
    const FunctionalTask::DeathFunc& upon_death)
{
    PT(FunctionalTask) task = new FunctionalTask(func, data, name);
    if (upon_death)
        task->set_upon_death(upon_death);

    add(task, {}, sort, priority, task_chain);

    return task;
}

size_t TaskManager::remove(const std::string& task_name)
{
    return mgr_->remove(mgr_->find_tasks(task_name));
}

bool TaskManager::remove(AsyncTask* task)
{
    return mgr_->remove(task);
}

size_t TaskManager::remove_task_matching(const GlobPattern& task_pattern)
{
    return mgr_->remove(mgr_->find_tasks_matching(task_pattern));
}

AsyncTask* TaskManager::setup_task(AsyncTask* task, const std::string& name,
    boost::optional<int> sort, boost::optional<int> priority,
    const boost::optional<std::string>& task_chain)
{
    if (!name.empty())
        task->set_name(name);

    if (!task->has_name())
    {
        rppanda_task_cat.error() << "setup_task: Task has NO name." << std::endl;
        throw std::runtime_error("Task has NO name.");
    }

    if (sort)
        task->set_sort(sort.value());

    if (priority)
        task->set_priority(priority.value());

    if (task_chain)
        task->set_task_chain(task_chain.value());

    return task;
}

}
