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
class RENDER_PIPELINE_DECL DirectObject : public TypedReferenceCount
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
    static TypeHandle get_class_type();
    static void init_type();
    virtual TypeHandle get_type() const;
    virtual TypeHandle force_init_type();

private:
    static TypeHandle type_handle_;
};

// ************************************************************************************************

inline TypeHandle DirectObject::get_class_type()
{
    return type_handle_;
}

inline void DirectObject::init_type()
{
    TypedReferenceCount::init_type();
    register_type(type_handle_, "rppanda::DirectObject", TypedReferenceCount::get_class_type());
}

inline TypeHandle DirectObject::get_type() const
{
    return get_class_type();
}

inline TypeHandle DirectObject::force_init_type()
{
    init_type();
    return get_class_type();
}

}
