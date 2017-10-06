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

#include <spdlog/fmt/ostr.h>

#include "rppanda/task/config_rppanda_task.hpp"

namespace rppanda {

TypeHandle TaskManager::type_handle_;

TaskManager* TaskManager::get_global_instance()
{
    static TaskManager instance;
    return &instance;
}

TaskManager::TaskManager() : mgr_(AsyncTaskManager::get_global_ptr()),
    global_clock_(mgr_->get_clock())
{
}

bool TaskManager::has_task_named(const std::string& task_name) const
{
    return mgr_->find_task(task_name) != nullptr;
}

AsyncTaskCollection TaskManager::get_task_named(const std::string& task_name) const
{
    return mgr_->find_tasks(task_name);
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
    boost::optional<int> sort, boost::optional<int> priority,
    const boost::optional<std::string>& task_chain,
    const FunctionalTask::DeathFunc& upon_death)
{
    PT(FunctionalTask) task = new FunctionalTask(func, name);
    if (upon_death)
        task->set_upon_death(upon_death);

#if !defined(_MSC_VER) || _MSC_VER >= 1900
    do_method_later(delay_time, task, {}, sort, priority, task_chain);
#else
    do_method_later(delay_time, task.p(), std::string(), sort, priority, task_chain);
#endif

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
    boost::optional<int> sort, boost::optional<int> priority,
    const boost::optional<std::string>& task_chain,
    const FunctionalTask::DeathFunc& upon_death)
{
    PT(FunctionalTask) task = new FunctionalTask(func, name);
    if (upon_death)
        task->set_upon_death(upon_death);

#if !defined(_MSC_VER) || _MSC_VER >= 1900
    add(task, {}, sort, priority, task_chain);
#else
    add(task.p(), {}, sort, priority, task_chain);
#endif

    return task;
}

int TaskManager::remove(const std::string& task_name)
{
    return mgr_->remove(mgr_->find_tasks(task_name));
}

int TaskManager::remove(AsyncTask* task)
{
    return mgr_->remove(task);
}

AsyncTask* TaskManager::setup_task(AsyncTask* task, const std::string& name,
    boost::optional<int> sort, boost::optional<int> priority,
    const boost::optional<std::string>& task_chain)
{
    if (!name.empty())
        task->set_name(name);

    if (!task->has_name())
        throw std::runtime_error("Task has NO name.");

    if (sort)
        task->set_sort(sort.get());

    if (priority)
        task->set_priority(priority.get());

    if (task_chain)
        task->set_task_chain(task_chain.get());

    return task;
}

}
