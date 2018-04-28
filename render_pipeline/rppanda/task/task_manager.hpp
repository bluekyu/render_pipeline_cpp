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

#include <asyncTaskCollection.h>

#include <boost/optional.hpp>

#include <render_pipeline/rpcore/config.hpp>
#include <render_pipeline/rppanda/task/functional_task.hpp>

class AsyncTaskManager;
class ClockObject;

namespace rppanda {

class RENDER_PIPELINE_DECL TaskManager : public TypedObject
{
public:
    static TaskManager* get_global_instance();

    TaskManager();

    virtual ~TaskManager();

    AsyncTaskManager* get_mgr() const;
    ClockObject* get_global_clock() const;

    /**
     * Returns the task currently executing on this thread, or
     * nullptr if this is being called outside of the task manager.
     */
    TypedReferenceCount* get_current_task() const;

    /**
     * Returns true if a task chain with the indicated name has
     * already been defined, or false otherwise.  Note that
     * setupTaskChain() will implicitly define a task chain if it has
     * not already been defined, or modify an existing one if it has,
     * so in most cases there is no need to check this method
     * first.
     */
    bool has_task_chain(const std::string& chain_name) const;

    /**
     * Defines a new task chain.  Each task chain executes tasks
     * potentially in parallel with all of the other task chains (if
     * numThreads is more than zero).  When a new task is created, it
     * may be associated with any of the task chains, by name (or you
     * can move a task to another task chain with
     * task::set_task_chain()).  You can have any number of task chains,
     * but each must have a unique name.
     */
    void setup_task_chain(const std::string& chain_name, boost::optional<int> num_thread = boost::none,
        boost::optional<bool> tick_clock = boost::none, boost::optional<ThreadPriority> thread_priority = boost::none,
        boost::optional<double> frame_budget = boost::none, boost::optional<bool> frame_sync = boost::none,
        boost::optional<bool> timeslice_priority = boost::none);

    /**
     * Returns true if there is at least one task, active or
     * sleeping, with the indicated name.
     */
    bool has_task_named(const std::string& task_name) const;

    /**
     * Returns a list of all tasks, active or sleeping, with the
     * indicated name.
     */
    AsyncTaskCollection get_task_named(const std::string& task_name) const;

    /**
     * Returns a list of all tasks, active or sleeping, with a
     * name that matches the pattern, which can include standard
     * shell globbing characters like *, ?, and [].
     */
    AsyncTaskCollection get_task_matching(const GlobPattern& task_pattern) const;

    /**
     * Returns list of all tasks, active and sleeping, in arbitrary order.
     */
    AsyncTaskCollection get_all_tasks() const;

    /** eturns list of all active tasks in arbitrary order. */
    AsyncTaskCollection get_tasks() const;

    /** Returns list of all sleeping tasks in arbitrary order. */
    AsyncTaskCollection get_do_laters() const;

    /**
     * Add a task to be performed at some time in the future.
     *
     * @see add(const std::string&, GenericAsyncTask::TaskFunc*, void*, boost::optional<int>,
     * boost::optional<int>, const boost::optional<std::string>&, GenericAsyncTask::DeathFunc*);
     */
    AsyncTask* do_method_later(float delay_time,
        AsyncTask* task, const std::string& name,
        boost::optional<int> sort = boost::none, boost::optional<int> priority = boost::none,
        const boost::optional<std::string>& task_chain = boost::none);

    /**
     * Add a new task to be performed at some time in the future.
     *
     * @see add(const std::string&, GenericAsyncTask::TaskFunc*, void*, boost::optional<int>,
     * boost::optional<int>, const boost::optional<std::string>&, GenericAsyncTask::DeathFunc*);
     */
    FunctionalTask* do_method_later(float delay_time,
        const FunctionalTask::TaskFunc& func, const std::string& name,
        boost::optional<int> sort = boost::none, boost::optional<int> priority = boost::none,
        const boost::optional<std::string>& task_chain = boost::none,
        const FunctionalTask::DeathFunc& upon_death=nullptr);

    /**
     * Add a task to the task manager.
     *
     * @param   task        AsyncTask pointer.
     * @param   name        the name to assign to the Task.
     *                      This is required unless the task has already name.
     *
     * @param   sort        the sort value to assign the task.  The default sort is 0.
     *                      Within a particular task chain, it is guaranteed that the
     *                      tasks with a lower sort value will all run before tasks with a
     *                      higher sort value run.
     *
     * @param   priority    the priority at which to run the task.  The default
     *                      priority is 0.  Higher priority tasks are run sooner, and/or
     *                      more often.
     *
     * @param   task_chain  the name of the task chain to assign the task to.
     *
     * @return  pointer of the task object
     */
    AsyncTask* add(AsyncTask* task, const std::string& name = {},
        boost::optional<int> sort = boost::none, boost::optional<int> priority = boost::none,
        const boost::optional<std::string>& task_chain = boost::none);

    /**
     * Add a new task to the task manager.
     *
     * @see add(const std::string&, AsyncTask*, boost::optional<int>, boost::optional<int>, const boost::optional<std::string>&)
     *
     * @param   func        GenericAsyncTask function.
     * @param   user_data   argument to pass to the task function.
     *
     * @param   upon_death  a function to call when the task terminates,
     *                      either because it has run to completion, or because it has
     *                      been explicitly removed.
     *
     * @return  pointer of a new task object
     */
    FunctionalTask* add(const FunctionalTask::TaskFunc& func, const std::string& name,
        boost::optional<int> sort = boost::none, boost::optional<int> priority = boost::none,
        const boost::optional<std::string>& task_chain = boost::none,
        const FunctionalTask::DeathFunc& upon_death = nullptr);

    size_t remove(const std::string& task_name);
    bool remove(AsyncTask* task);

    /**
     * Removes all tasks whose names match the pattern, which can
     * include standard shell globbing characters like *, ?, and [].
     * See also remove().
     *
     * @return  the number of tasks removed.
     */
    size_t remove_task_matching(const GlobPattern& task_pattern);

private:
    AsyncTask* setup_task(AsyncTask* task, const std::string& name,
        boost::optional<int> sort, boost::optional<int> priority,
        const boost::optional<std::string>& task_chain);

    AsyncTaskManager* mgr_;
    ClockObject* global_clock_;

public:
    static TypeHandle get_class_type();
    static void init_type();
    TypeHandle get_type() const override;
    TypeHandle force_init_type() override;

private:
    static TypeHandle type_handle_;
};

// ************************************************************************************************

inline AsyncTaskManager* TaskManager::get_mgr() const
{
    return mgr_;
}

inline ClockObject* TaskManager::get_global_clock() const
{
    return global_clock_;
}

inline TypeHandle TaskManager::get_class_type()
{
    return type_handle_;
}

inline void TaskManager::init_type()
{
    TypedObject::init_type();
    register_type(type_handle_, "rppanda::TaskManager", TypedObject::get_class_type());
}

inline TypeHandle TaskManager::get_type() const
{
    return get_class_type();
}

inline TypeHandle TaskManager::force_init_type()
{
    init_type();
    return get_class_type();
}

}
