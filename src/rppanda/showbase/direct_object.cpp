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

namespace rppanda {

TypeHandle DirectObject::type_handle_;

bool DirectObject::accept(const std::string& ev_name, EventHandler::EventFunction* func)
{
    return EventHandler::get_global_event_handler()->add_hook(ev_name, func);
}

bool DirectObject::accept(const std::string& ev_name, EventHandler::EventCallbackFunction* func, void* user_data)
{
    return EventHandler::get_global_event_handler()->add_hook(ev_name, func, user_data);
}

bool DirectObject::ignore(const std::string& ev_name)
{
    return EventHandler::get_global_event_handler()->remove_hooks(ev_name);
}

bool DirectObject::ignore(const std::string& ev_name, EventHandler::EventFunction* func)
{
    return EventHandler::get_global_event_handler()->remove_hook(ev_name, func);
}

bool DirectObject::ignore(const std::string& ev_name, EventHandler::EventCallbackFunction* func, void* user_data)
{
    return EventHandler::get_global_event_handler()->remove_hook(ev_name, func, user_data);
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

}
