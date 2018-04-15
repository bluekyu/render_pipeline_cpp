/**
 * Render Pipeline C++
 *
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

#include "render_pipeline/rppanda/task/functional_task.hpp"

namespace rppanda {

TypeHandle FunctionalTask::type_handle_;

FunctionalTask::FunctionalTask(const TaskFunc& func, const std::string& name) : AsyncTask(name), function_(func)
{
}

FunctionalTask::FunctionalTask(TaskFunc&& func, const std::string& name) : AsyncTask(name), function_(func)
{
}

FunctionalTask::~FunctionalTask() = default;

void FunctionalTask::upon_birth(AsyncTaskManager* manager)
{
    AsyncTask::upon_birth(manager);
    if (upon_birth_)
        upon_birth_(this);
}

void FunctionalTask::upon_death(AsyncTaskManager* manager, bool clean_exit)
{
    AsyncTask::upon_death(manager, clean_exit);
    if (upon_death_)
        upon_death_(this, clean_exit);
}

TypeHandle FunctionalTask::get_class_type()
{
    return type_handle_;
}

void FunctionalTask::init_type()
{
    AsyncTask::init_type();
    register_type(type_handle_, "rppanda::FunctionalTask", AsyncTask::get_class_type());
}

TypeHandle FunctionalTask::get_type() const
{
    return get_class_type();
}

TypeHandle FunctionalTask::force_init_type()
{
    init_type();
    return get_class_type();
}

}
