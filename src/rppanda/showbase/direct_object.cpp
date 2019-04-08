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
 * This is C++ porting codes of direct/src/showbase/DirectObject.py
 */

#include "render_pipeline/rppanda/showbase/direct_object.hpp"

#include "render_pipeline/rppanda/showbase/showbase.hpp"
#include "render_pipeline/rppanda/task/task_manager.hpp"

namespace rppanda {

DirectObject::TaskContainer::TaskContainer(DirectObject* owner, AsyncTask* task) : task_(task), owner_(owner), task_id_(task->get_task_id())
{
    task_.add_callback(this);
}

DirectObject::TaskContainer::~TaskContainer()
{
    if (!deleting_)
        task_.remove_callback(this);
}

void DirectObject::TaskContainer::wp_callback(void*)
{
    deleting_ = true;                       // to avoid dead lock
    owner_->task_list_.erase(task_id_);     // will delete this
}

// ************************************************************************************************

DirectObject::~DirectObject()
{
    ignore_all();
}

void DirectObject::accept(const std::string& ev_name, const Messenger::EventFunction& func)
{
    Messenger::get_global_instance()->accept(ev_name, func, this, true);
}

void DirectObject::accept_once(const std::string& ev_name, const Messenger::EventFunction& func)
{
    Messenger::get_global_instance()->accept(ev_name, func, this, false);
}

void DirectObject::ignore(const std::string& ev_name)
{
    Messenger::get_global_instance()->ignore(ev_name, this);
}

void DirectObject::ignore_all()
{
    Messenger::get_global_instance()->ignore_all(this);
}

AsyncTask* DirectObject::add_task(AsyncTask* task, const std::string& name,
    boost::optional<int> sort, boost::optional<int> priority,
    const boost::optional<std::string>& task_chain)
{
    auto new_task = TaskManager::get_global_instance()->add(task, name, sort, priority, task_chain);
    do_add_task(new_task);
    return new_task;
}

FunctionalTask* DirectObject::add_task(const FunctionalTask::TaskFunc& func, const std::string& name,
    boost::optional<int> sort, const FunctionalTask::UserDataType& data,
    boost::optional<int> priority,
    const boost::optional<std::string>& task_chain,
    const FunctionalTask::DeathFunc& upon_death)
{
    auto new_task = TaskManager::get_global_instance()->add(
        func, name, sort, data, priority, task_chain, upon_death);
    do_add_task(new_task);
    return new_task;
}

AsyncTask* DirectObject::do_method_later(float delay_time, AsyncTask* task,
    const std::string& name,
    boost::optional<int> sort, boost::optional<int> priority,
    const boost::optional<std::string>& task_chain)
{
    auto new_task = TaskManager::get_global_instance()->do_method_later(
        delay_time, task, name, sort, priority, task_chain);
    do_add_task(new_task);
    return new_task;
}

FunctionalTask* DirectObject::do_method_later(float delay_time,
    const FunctionalTask::TaskFunc& func, const std::string& name,
    boost::optional<int> sort, const FunctionalTask::UserDataType& data,
    boost::optional<int> priority,
    const boost::optional<std::string>& task_chain,
    const FunctionalTask::DeathFunc& upon_death)
{
    auto new_task = TaskManager::get_global_instance()->do_method_later(
        delay_time, func, name, sort, data, priority, task_chain, upon_death);
    do_add_task(new_task);
    return new_task;
}

void DirectObject::remove_task(const std::string& task_name)
{
    for (const auto& task_container: task_list_)
    {
        auto& task = task_container.second->task_;
        if (task->get_name() == task_name)
            remove_task(task.p());
    }
}

void DirectObject::remove_task(AsyncTask* task)
{
    task_list_.erase(task->get_task_id());
    task->remove();
}

void DirectObject::remove_all_tasks()
{
    std::vector<AsyncTask*> tasks;
    tasks.reserve(task_list_.size());
    for (const auto& task : task_list_)
        tasks.push_back(task.second->task_.p());

    task_list_.clear();

    for (const auto& task : tasks)
        task->remove();
}

void DirectObject::do_add_task(AsyncTask* task)
{
    // task_id is unique
    task_list_.emplace(task->get_task_id(), std::make_unique<TaskContainer>(this, task));
}

}
