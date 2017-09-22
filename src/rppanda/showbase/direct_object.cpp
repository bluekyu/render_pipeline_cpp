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

#include <asyncTaskManager.h>

#include "render_pipeline/rppanda/showbase/showbase.hpp"
#include "render_pipeline/rppanda/task/task_manager.hpp"

namespace rppanda {

TypeHandle DirectObject::type_handle_;

AsyncTask* DirectObject::add_task(AsyncTask* task, const std::string& name,
    boost::optional<int> sort, boost::optional<int> priority,
    const boost::optional<std::string>& task_chain)
{
    auto new_task = TaskManager::get_global_instance()->add(task, name, sort, priority, task_chain);
    do_add_task(new_task);
    return new_task;
}

FunctionalTask* DirectObject::add_task(const FunctionalTask::TaskFunc& func, const std::string& name,
    boost::optional<int> sort, boost::optional<int> priority,
    const boost::optional<std::string>& task_chain,
    const FunctionalTask::DeathFunc& upon_death)
{
    auto new_task = TaskManager::get_global_instance()->add(func, name, sort, priority, task_chain, upon_death);
    do_add_task(new_task);
    return new_task;
}

AsyncTask* DirectObject::do_method_later(float delay_time, AsyncTask* task,
    const std::string& name,
    boost::optional<int> sort, boost::optional<int> priority,
    const boost::optional<std::string>& task_chain)
{
    auto new_task = TaskManager::get_global_instance()->do_method_later(delay_time, task, name, sort, priority, task_chain);
    do_add_task(new_task);
    return new_task;
}

FunctionalTask* DirectObject::do_method_later(float delay_time,
    const FunctionalTask::TaskFunc& func, const std::string& name,
    boost::optional<int> sort, boost::optional<int> priority,
    const boost::optional<std::string>& task_chain,
    const FunctionalTask::DeathFunc& upon_death)
{
    auto new_task = TaskManager::get_global_instance()->do_method_later(delay_time, func, name, sort, priority, task_chain, upon_death);
    do_add_task(new_task);
    return new_task;
}

void DirectObject::remove_task(const std::string& task_name)
{
    for (const auto& task: task_list_)
    {
        if (task.second->get_name() == task_name)
            remove_task(task.second);
    }
}

void DirectObject::remove_task(AsyncTask* task)
{
    auto id = task->get_task_id();
    task->remove();
    task_list_.erase(id);
}

void DirectObject::do_add_task(AsyncTask* task)
{
#if !defined(_MSC_VER) || _MSC_VER >= 1900
    task_list_.insert_or_assign(task->get_task_id(), task);
#else
    task_list_.insert({task->get_task_id(), task});
#endif
}

}
