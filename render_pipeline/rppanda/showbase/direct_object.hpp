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

#include <dtoolbase.h>

#include <string>
#include <unordered_map>

#include <boost/optional.hpp>

#include <render_pipeline/rppanda/task/functional_task.hpp>
#include <render_pipeline/rppanda/showbase/messenger.hpp>

namespace rppanda {

/** This is the class that all Direct/SAL classes should inherit from. */
class RENDER_PIPELINE_DECL DirectObject
{
public:
    DirectObject() = default;
    DirectObject(const DirectObject&) = delete;
    DirectObject(DirectObject&&) = default;

    virtual ~DirectObject();

    DirectObject& operator=(const DirectObject&) = delete;
    DirectObject& operator=(DirectObject&&) = default;

    void accept(const std::string& ev_name, const Messenger::EventFunction& func);

    void accept_once(const std::string& ev_name, const Messenger::EventFunction& func);

    void ignore(const std::string& ev_name);

    void ignore_all();

    /**
     * @see TaskManager::add
     */
    AsyncTask* add_task(AsyncTask* task, const std::string& name = {},
        boost::optional<int> sort = boost::none, boost::optional<int> priority = boost::none,
        const boost::optional<std::string>& task_chain = boost::none);

    /**
     * @see TaskManager::add
     */
    FunctionalTask* add_task(const FunctionalTask::TaskFunc& func, const std::string& name = {},
        boost::optional<int> sort = boost::none, const FunctionalTask::UserDataType& data = {},
        boost::optional<int> priority = boost::none,
        const boost::optional<std::string>& task_chain = boost::none,
        const FunctionalTask::DeathFunc& upon_death = nullptr);

    /**
     * @see TaskManager::do_method_later
     */
    AsyncTask* do_method_later(float delay_time,
        AsyncTask* task, const std::string& name,
        boost::optional<int> sort = boost::none, boost::optional<int> priority = boost::none,
        const boost::optional<std::string>& task_chain = boost::none);

    /**
     * @see TaskManager::do_method_later
     */
    FunctionalTask* do_method_later(float delay_time,
        const FunctionalTask::TaskFunc& func, const std::string& name,
        boost::optional<int> sort = boost::none, const FunctionalTask::UserDataType& data = {},
        boost::optional<int> priority = boost::none,
        const boost::optional<std::string>& task_chain = boost::none,
        const FunctionalTask::DeathFunc& upon_death = nullptr);

    void remove_task(const std::string& task_name);
    void remove_task(AsyncTask* task);

    void remove_all_tasks();

private:
    class TaskContainer : public WeakPointerCallback
    {
    public:
        TaskContainer(DirectObject* owner, AsyncTask* task);
        virtual ~TaskContainer();

        void wp_callback(void*) final;

        WPT(AsyncTask) task_;

    private:
        DirectObject* const owner_;
        const AtomicAdjust::Integer task_id_;   // While destructing AsyncTask, ID already is deleted
                                                // So, we save it.
    };

    void do_add_task(AsyncTask* task);

    std::unordered_map<AtomicAdjust::Integer, TaskContainer> task_list_;
};

}
