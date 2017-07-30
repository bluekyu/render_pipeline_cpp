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

/**
 * Defines the DirectObject class, a convenient class to inherit from if the
 * object needs to be able to respond to events.
 *
 * This is C++ porting codes of direct/src/showbase/DirectObject.py
 */

#pragma once

#include <genericAsyncTask.h>
#include <eventHandler.h>

#include <string>

#include <render_pipeline/rpcore/config.hpp>

namespace rppanda {

/** This is the class that all Direct/SAL classes should inherit from. */
class RENDER_PIPELINE_DECL DirectObject: public TypedReferenceCount
{
public:
    bool accept(const std::string& ev_name, EventHandler::EventFunction* func);
    bool accept(const std::string& ev_name, EventHandler::EventCallbackFunction* func, void* user_data);

    bool ignore(const std::string& ev_name);
    bool ignore(const std::string& ev_name, EventHandler::EventFunction* func);
    bool ignore(const std::string& ev_name, EventHandler::EventCallbackFunction* func, void* user_data);

    GenericAsyncTask* add_task(GenericAsyncTask::TaskFunc* func, void* user_data, const std::string& name);
    GenericAsyncTask* add_task(GenericAsyncTask::TaskFunc* func, void* user_data, const std::string& name, int sort);

    int remove_task(const std::string& task_name);

    GenericAsyncTask* do_method_later(float delay, GenericAsyncTask::TaskFunc* func, const std::string& name, void* user_data);

public:
    static TypeHandle get_class_type(void);
    static void init_type(void);
    virtual TypeHandle get_type(void) const;
    virtual TypeHandle force_init_type(void);

private:
    static TypeHandle type_handle_;
};

// ************************************************************************************************

inline TypeHandle DirectObject::get_class_type(void)
{
    return type_handle_;
}

inline void DirectObject::init_type(void)
{
    TypedReferenceCount::init_type();
    register_type(type_handle_, "rppanda::DirectObject", TypedReferenceCount::get_class_type());
}

inline TypeHandle DirectObject::get_type(void) const
{
    return get_class_type();
}

inline TypeHandle DirectObject::force_init_type(void)
{
    init_type();
    return get_class_type();
}

}
