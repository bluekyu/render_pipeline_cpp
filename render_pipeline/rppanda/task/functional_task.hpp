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

#pragma once

#include <asyncTask.h>

#include <functional>

#include <render_pipeline/rpcore/config.hpp>

namespace rppanda {

class RENDER_PIPELINE_DECL FunctionalTask : public AsyncTask
{
public:
    using TaskFunc = std::function<DoneStatus(FunctionalTask*)>;
    using BirthFunc = std::function<void(FunctionalTask*)>;
    using DeathFunc = std::function<void(FunctionalTask*, bool)>;

public:
    FunctionalTask(const TaskFunc& func = nullptr, const std::string& name = {});
    FunctionalTask(TaskFunc&& func, const std::string& name = {});

    ALLOC_DELETED_CHAIN(FunctionalTask);

    virtual ~FunctionalTask();

    void set_function(const TaskFunc& func);
    void set_function(TaskFunc&& func);
    const TaskFunc& get_function() const;

    void set_upon_birth(const BirthFunc& func);
    void set_upon_birth(BirthFunc&& func);
    const BirthFunc& get_upon_birth() const;

    void set_upon_death(const DeathFunc& func);
    void set_upon_death(DeathFunc&& func);
    const DeathFunc& get_upon_death() const;

protected:
    bool is_runnable() override;
    DoneStatus do_task() override;
    void upon_birth(AsyncTaskManager* manager) override;
    void upon_death(AsyncTaskManager* manager, bool clean_exit) override;

private:
    TaskFunc function_;
    BirthFunc upon_birth_;
    DeathFunc upon_death_;

public:
    static TypeHandle get_class_type();
    static void init_type();
    virtual TypeHandle get_type() const override;
    virtual TypeHandle force_init_type() override;

private:
    static TypeHandle type_handle_;
};

// ************************************************************************************************

inline void FunctionalTask::set_function(const TaskFunc& func)
{
    function_ = func;
}

inline void FunctionalTask::set_function(TaskFunc&& func)
{
    function_ = func;
}

inline const FunctionalTask::TaskFunc& FunctionalTask::get_function() const
{
    return function_;
}

inline void FunctionalTask::set_upon_birth(const BirthFunc& func)
{
    upon_birth_ = func;
}

inline void FunctionalTask::set_upon_birth(BirthFunc&& func)
{
    upon_birth_ = func;
}

inline const FunctionalTask::BirthFunc& FunctionalTask::get_upon_birth() const
{
    return upon_birth_;
}

inline void FunctionalTask::set_upon_death(const DeathFunc& func)
{
    upon_death_ = func;
}

inline void FunctionalTask::set_upon_death(DeathFunc&& func)
{
    upon_death_ = func;
}

inline const FunctionalTask::DeathFunc& FunctionalTask::get_upon_death() const
{
    return upon_death_;
}

inline bool FunctionalTask::is_runnable()
{
    return bool(function_);
}

inline AsyncTask::DoneStatus FunctionalTask::do_task()
{
    return function_(this);
}

}
